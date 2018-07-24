#!/usr/bin/python
#
# SIS - Simple Installer System Installer
#
# This file is part of SIS
# Copyright (c) 2017, Hugues Casse <casse@irit.fr>
# 
# SIS is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# SIS is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with SIS; if not, write to the Free Software 
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA


import argparse
import datetime
import platform
import os
import os.path
import re
import shutil
import subprocess
import sys
import tempfile
import time
import urllib2
import urlparse
import xml.etree.ElementTree as ET

# configuration
APP				= "OTAWA"
DB_URL			= "http://tracesgroup.net/otawa/packages/1.0"
DB_TOP			= ".."
DB_CONF			= "share/Otawa/install.xml"
VERSION_FILE	= "VERSION"

# debugging
DEBUG	= False

# convenient constant
NULL_VERS = "0.0"
SRCE_VERS = "source"
SIS_EXTEND_TAG = "sis-extend"
SIS_INSTALL_TAG = "sis-install"

KNOWN_CONFIGS = {
	("Linux",	"i686", 	32)	: "linux-x86",
	("Linux",	"i686",		64)	: "linux-x86_64",
	("Linux",	"x86_64",	32)	: "linux-x86",	
	("Linux",	"x86_64",	64)	: "linux-x86_64",
	("Windows",	None, 		32)	: "win",
	("Windows", None, 		64) : "win64",
	("Darwin",	"x86_64", 	64) : "darwin-x86_64"
}

DYNLIB_EXT = {
	"linux_x86":		".so",
	"linux-x86_64":		".so",
	"win":				".dll",
	"win64":			".dll",
	"darwin-x86_64":	".dsym"
}

def is_younger(v1, v2):
	"""Test if v1 is younger than v2."""
	return v1.split() > v2.split()

def is_older(v1, v2):
	"""Test if v1 is younger than v2."""
	return v1.split() < v2.split()

def do_closure(f, s):
	"""Build the closure of set s using the function f to get
	successors of item in s. f takes an item of s and return the list
	of successors."""
	res = []
	todo = list(s)
	while todo <> []:
		v = todo.pop()
		for w in f(v):
			if w not in res:
				res.append(w)
				todo.append(w)
	return res

def ensure_dir(path):
	"""Ensure the given path exists, is a directory and accessible.
	Create it else. Raise OSError in case of error."""
	if not os.path.exists(path):
		try:
			os.makedirs(path)
		except OSError as e:
			return str(e)
	elif not os.path.isdir(path) or not os.access(path, os.R_OK | os.W_OK | os.X_OK):
		raise OSError("%s is not a directory or is not accessible!" % path)


def download(addr, mon):
	"""Download the given file in the working directory.
	On success, return the file path. Raise IOError else."""
	try:
		file = os.path.basename(addr)
		path = os.path.join(mon.get_build_dir(), file)
		in_stream = urllib2.urlopen(addr)
		out_stream = open(path, "w")
		shutil.copyfileobj(in_stream, out_stream)
		in_stream.close()
		out_stream.close()
		return path
	except (urllib2.URLError, IOError, shutil.Error) as e:
		raise IOError(str(e))


class Monitor:
	
	def __init__(self):
		self.env = { }
		self.out = sys.stdout
		self.err = sys.stderr
		self.log_path = None
		self.log_file = None
		self.verbose = False
		self.dry = False
		self.force = False
		self.build_dir = None
		self.top_dir = None
		self.host_type = None
		self.site_path = None
		self.errors = 0
		self.allocated_build_dir = False
		self.allocated_log_file = False
		self.var_re = re.compile("((^|[^$])\$\(([a-zA-Z0-9_]+)\))|(\$\$)")

	def get(self, name, default = None):
		try:
			return self.env[name]
		except KeyError:
			return default

	def set(self, name, val):
		self.env[name] = val

	def get_host_type(self):
		"""Build the architecture string of the current system."""
		if self.host_type == None:
			os = platform.system()
			mach = platform.machine()
			if sys.maxint > 2**32:
				size = 64 
			else:
				size = 32
			try:
				self.host_type = KNOWN_CONFIGS[(os, mach, size)]
			except KeyError:
				self.fatal("unsupported host type")
		return self.host_type

	def ask_yes_no(self, msg):
		"""Ask the question to the user and parse answer."""
		self.out.write(msg + " [yes/NO]: ")
		self.out.flush()
		line = sys.stdin.readline().strip().lower()
		return line == "yes"

	def say(self, msg):
		self.out.write("%s\n" % msg)
	
	def warn(self, msg):
		self.err.write("WARNING: %s\n" % msg)
	
	def error(self, msg):
		self.err.write("ERROR: %s\n" % msg)
		self.errors = self.errors + 1

	def fatal(self, msg):
		self.err.write("ERROR: %s\n" % msg)
		self.errors = self.errors + 1
		self.cleanup()
		exit(1)
	
	def check(self, msg):
		self.out.write("%s ... " % msg)
		self.out.flush()
	
	def succeed(self):
		self.out.write("[OK]\n")
	
	def fail(self):
		self.out.write("[FAILED]\n")

	def comment(self, msg):
		if self.verbose:
			self.err.write("# %s\n" % msg)
	
	def execute(self, cmd, out = None, err = None, log = False):
		if not out:
			if log:
				out = self.get_log_file()
			else:
				out = self.out
		if not err:
			if log:
				err = self.get_log_file()
			else:
				err = self.err
		return subprocess.call(cmd, stdout=out, stderr=err, shell=True)

	def result_of(self, cmd, err = None, log = False):
		"""Execute the given command and return result of it if it doesn't
		fail. Return None else."""
		if not err:
			if log:
				err = self.get_log_file()
			else:
				err = self.err
		try:
			return subprocess.check_output(cmd, stderr=err, shell=True)
		except CalledProcessError as e:
			self.log(str(e))
			return None

	def open_log(self, path):
		try:
			self.log_file = open(path, "w")
			self.log("=== created by sis-install.py V1.0 (%s) ===\n\n" % datetime.datetime.now())
			self.log_path = path
		except IOError as e:
			self.warn("cannot log to %s: %s. Falling back to stderr." % (path, e))
			self.log_file = self.stderr

	def log(self, msg):
		self.get_log_file().write(msg)
		self.get_log_file().flush()
	
	def eval(self, str):
		"""Replace the $(...) variables with the values of monitor variables
		and $$ by single $."""
		p = 0
		m = self.var_re.search(str, p)
		while m:
			if m.group(4):
				rep = "$"
			else:
				rep = self.get(m.group(3), "")
			s = m.start() if m.group()[0] == '$' else m.start() + 1
			str = str[:s] + rep + str[m.end():]
			p = s + len(rep)
			m = self.var_re.search(str, p)
		return str
	
	def get_build_dir(self):
		"""Obtain a building directory."""
		if self.build_dir == None:
			self.build_dir = tempfile.mkdtemp("-sis")
			self.allocated_build_dir = True
		return self.build_dir

	def get_log_file(self):
		"""Get the file to perform logging inside."""
		if self.log_file == None:
			if self.log_path <> None:
				self.open_log(self.log_path)
			else:
				self.open_log(os.path.join(os.getcwd(), "build.log"))
				self.allocated_log_file = True
		return self.log_file

	def cleanup(self):
		"""Called at the end to remove useless ressources."""
		if not DEBUG and self.allocated_build_dir:
			shutil.rmtree(self.build_dir)
		if self.errors == 0 and self.allocated_log_file:
			os.remove(self.log_path)


# global variables
DB = { }
MONITOR = Monitor()
DEPS = { }


# Action classes
class Action:
	"""Base class of actions for installation / uninstallation."""

	def __init__(self, id):
		self.id = id

	def perform(self, mon):
		"""Called to perform the action.
		Return True for success, False else."""
		pass
	
	def parse(self, elt, mon):
		"""Called to let the action to configure itself.
		elt is the element defining the action.
		May raise IOError."""
		pass
	
	def save(self, elt):
		"""Called to configure the given element with the information
		of the action. elt already contains the identifier of the action."""
		pass

	def reverse(self):
		"""Ask to generate an action to perform the reverse action
		(for uninstallation for instance). Can return the reverse action
		or None."""
		return None

class Remove(Action):
	"""Action removing a file."""
	
	def __init__(self, path = None):
		Action.__init__(self, "remove")
		self.path = path
	
	def perform(self, mon):
		try:
			os.remove(self.path)
			return True
		except OSError as e:
			mon.error("cannot remove %s: %s" % (self.path, e))
			return False
	
	def parse(self, elt, mon):
		self.path = elt.get("path", None)
		if self.path == None:
			raise IOError("path attribute is required in remove!")
	
	def save(self, elt):
		elt.set("path", self.path)


class Install(Action):
	"""Represents an installation action."""
	
	def __init__(self, path = None, to = None):
		Action.__init__(self, "install")
		self.path = path
		self.to = to if to <> None else path
	
	def perform(self, mon):	
		spath = self.path
		tpath = os.path.join(mon.top_dir, self.to)
		mon.comment("copy %s to %s" % (spath, tpath))
		ppath = os.path.dirname(tpath)
		try:
			if not os.path.exists(ppath):
				os.makedirs(ppath)
			shutil.copy2(spath, tpath)
		except os.error as e:
			raise IOError("%s at %s install" % (e, self.path))
		
	def parse(self, elt, mon):
		to = elt.get("to", None)
		self.path = elt.get("dynlib", None)
		if self.path <> None:
			ext = DYNLIB_EXT[mon.get_host_type()]
		else:
			ext = ""
			self.path = elt.get("file", None)
			if self.path == None:
				raise IOError("malformed installation file")
		self.path = self.path + ext
		if self.to == None:
			self.to = self.path
		self.to = self.to + ext

	def save(self, elt):
		elt.set("file", self.path)
		elt.set("to", self.to)
	
	def reverse(self):
		return Remove(self.to)


ACTIONS = {
	"install": Install,
	"remove": Remove
}
def get_action(elt, mon):
	"""Create an action from an XML element.
	Return the action or None if there is an error (unknown action
	or bad configuration) in element."""
	try:
		action = ACTIONS[elt.tag]()
		action.parse(elt, mon)
		return action
	except KeyError:
		return None
	except IOError:
		return None

def save_action(parent, action):
	"""Save the action an XML element. parent is the XML parent element
	of the action element to create."""
	elt = ET.SubElement(parent, action.id)
	action.save(elt)


# Dep classes
class Dep:
	"""This class enables the test of dependencies that are out of
	the repository like special tools, libraries, etc."""
	
	def __init__(self, name):
		assert not DEPS.has_key(name)
		self.name = name
		DEPS[name] = self
		self.tested = False
		self.succeeded = False
	
	def message(self):
		"""Get the message explaining the test."""
		return self.name
	
	def help(self):
		"""In case of failure, display the given message."""
		return ""
	
	def do_test(self, mon):
		"""This function must be called to perform the test."""
		mon.check("testing %s" % self.message())
		self.test(mon)
		if self.succeeded:
			mon.succeed()
		else:
			mon.fail(self.help())
	
	def test(self, mon):
		"""This function must be overload to customize the test.
		If it succeeds, it must call succeed() function, else
		the fail() function."""
		self.fail()
	
	def succeed(self):
		self.tested = True
		self.succeeded = True
	
	def fail(self):
		self.tested = True
		self.succeeded = False
	
	def added_deps(self):
		"""Called to get additional dependencies."""
		return []

class CommandDep(Dep):
	"""Look for a command with possible different names.
	The XML element must have type="command" and attribute
	commands="..." with a "," separated list of commands to test."""
	
	def __init__(self, name, elt, commands = ""):
		Dep.__init__(self, name)
		if elt <>  None:
			self.commands = elt.get("commands", None)
		else:
			self.commands = commands
		assert self.commands <> None
		self.commands = self.commands.split(",")
		self.found = None
	
	def message(self):
		return "command %s" % self.name
	
	def help(self):
		return "%s is satisfied if one of the following program is available: %s" % (self.name, self.commands.join(", "))
	
	def test(self, mon):
		
		# build paths and exts
		paths = []
		for path in os.getenv("PATH", "").split(os.pathsep):
			paths.append(path.strip('"'))
		exts = [ext.lower() for ext in os.getenv("PATHEXT", "").split(";")]
		if exts == []:
			exts = [""]

		# look for command
		for cmd in self.commands:
			for path in paths:
				for ext in exts:
					cpath = os.path.join(path, cmd + ext)
					mon.comment("\tlook for executable %s" % cpath)
					if os.path.isfile(cpath) and os.access(cpath, os.X_OK):
						self.found = cpath
						self.succeed()
						return
		self.fail()

class LibraryDep(Dep):
	"""A library dependency look up for a library and possibly an
	associated header file. The XML element name is used as the libary
	name. A "header" attribute may be passed and used as header to test.
	A "lang" attribute equal to "c" or "c++" may also be passed.
	Additionnaly, "cflags" and "ldflags" attributes provides commands
	to get the corresponding flags."""
	
	def __init__(self, name, elt, header = "", lang = "c", cflags = None, ldflags = None):
		Dep.__init__(self, name)
		if elt <>  None:
			self.header = elt.get("header", None)
			self.lang = elt.get("lang", "c")
			self.cflags = elt.get("cflags", None)
			self.ldflags = elt.get("ldflags", None) 
		else:
			self.header = header
			self.lang = lang
			self.cflags = cflags
			self.ldflags = ldflags
	
	def message(self):
		return "libary  %s" % self.name
	
	def test(self, mon):
		cflags = ""
		ldflags = ""
		if self.cflags:
			cflags = mon.result_of(self.cflags, None, True)
			if cflags == None:
				self.fail()
				return
			else:
				cflags = cflags.strip()
				mon.log("cflags = %s\n" % cflags)
		if self.ldflags:
			ldflags = mon.result_of(self.ldflags, None, True)
			if ldflags == None:
				self.fail()
				return			
			else:
				ldflags = ldflags.strip()
				mon.log("ldflags = %s\n" % ldflags)
		p = tempfile.mkstemp(self.suffix())[1]
		op = tempfile.mkstemp()[1]
		h = open(p, "w")
		if self.header:
			h.write("#include <%s>\n" % self.header)
		h.write("int main(void) { return 0; }\n")
		h.flush()
		if not ldflags:
			ldflags = "-l" + self.name[3:]
		cmd = "%s %s -o %s %s %s" % (self.dep().found, p, op, cflags, ldflags)
		mon.log("running %s\n" % cmd)
		res = mon.execute(cmd, None, None, True)
		if res == 0:
			self.succeed()
		else:
			self.fail()

	def suffix(self):
		return ".cpp" if self.lang == "c++" else ".c"
	
	def dep(self):
		return CPP_DEP if self.lang == "c++" else C_DEP

	def added_deps(self):
		return [self.dep()]


dep_makers = {
	"command": CommandDep,
	"library": LibraryDep
}
def make_dep(elt):
	name = elt.get("name", "")
	try:
		return DEPS[name]
	except KeyError:
		try:
			type = elt.get("type", None)
			return dep_makers[type](name, elt)
		except KeyError:
			print "DEBUG: no dep matching type %s" % type
			assert False

def add_deps(elt, deps):
	"""Add to the list of dependencies the dependency given by the elt
	element. Retur True if the operation is successful, else False."""
	dep = make_dep(elt)
	if dep == None:
		return False
	else:
		deps.extend(do_closure(lambda d: d.added_deps(), [dep]))
		deps.append(dep)
		return True


# predefined dependencies
CMAKE_DEP	= CommandDep("cmake", 		None, "cmake")
HG_DEP		= CommandDep("mercurial",	None, "hg")
C_DEP		= CommandDep("cc",			None, "cc,gcc")
CPP_DEP		= CommandDep("c++",			None, "c++,g++")
MAKE_DEP	= CommandDep("make",		None, "make")
TAR_DEP		= CommandDep("tar",			None, "tar")
UNZIP_DEP	= CommandDep("unzip",		None, "unzip")
UNRAR_DEP	= CommandDep("unrar",		None, "unrar")
GZIP_DEP	= CommandDep("gzip",		None, "gzip")
BZIP2_DEP	= CommandDep("bzip2",		None, "gzip")


# packing management
def default_unpack(cmd, file, mon):
	mon.comment("unpacking with %s" % file)
	old_cwd = os.getcwd()
	os.chdir(os.path.dirname(file))
	try:
		rcode = subprocess.call(cmd,
			stdout = open(os.devnull, "w"),
			stderr = subprocess.STDOUT,
			shell = True)
	finally:
		os.chdir(old_cwd)
	if rcode <> 0:
		mon.fatal("cannot unpack %s" % file)

class Unpacker:
	"""Unpacker support."""
	
	def __init__(self, deps, cmd):
		self.deps = deps
		self.cmd = cmd

	def unpack(self, f, m):
		default_unpack(self.cmd % f, f, m)

UNPACKERS = {
	"tar.gz"	:	Unpacker([TAR_DEP, GZIP_DEP],	"tar xvf %s"),
	"tar.bz2"	:	Unpacker([TAR_DEP, BZIP2_DEP],	"tar xvf %s"),
	"zip"		:	Unpacker([UNZIP_DEP],		"unzip %s"),
	"rar"		:	Unpacker([UNRAR_DEP], 		"unrar %s")
}

def unpack(file, mon):
	try:
		p = file.index(".")
		ext = file[p + 1:]
		UNPACKERS[ext].unpack(file, mon)
		return file[:p]
	except (KeyError, ValueError):
		mon.fatal("don't know how to unpack %s" % file)

def unpack_deps(file):
	try:
		p = file.index(".")
		ext = file[p + 1:]
		return UNPACKERS[ext].deps
	except (KeyError, ValueError):
		return None


# Donwloader classes	
class Downloader:
	
	def __init__(self, pack):
		self.pack = pack
	
	def download(self, mon):
		"""Called to perform the download."""
		return False

	def add_deps(self, deps):
		"""Called to let the downloader add its own dependencies"""
		pass

class HgDownloader(Downloader):
	
	def __init__(self, pack, elt):
		Downloader.__init__(self, pack)
		self.address = elt.get("address", None)
		assert self.address <> None
	
	def add_deps(self, deps):
		deps.append(HG_DEP)		
	
	def download(self, mon):
		target = os.path.join(mon.get_build_dir(), self.pack.name)
		if os.path.exists(target):
			shutil.rmtree(target)
		cmd = "hg clone %s %s" % (self.address, target)
		mon.log("\nDowloading %s: %s" % (self.pack.name, cmd))
		res = mon.execute(cmd, mon.get_log_file(), mon.get_log_file())
		return res == 0

class ArchiveDownloader(Downloader):
	"""Downloader for an archive. Requires attribute address to get
	the archive."""
	
	def __init__(self, pack, elt):
		Downloader.__init__(self, pack)
		self.address = elt.get("address", None)
		assert self.address <> None

	def add_deps(self, deps):
		adeps = unpack_deps(os.path.basename(self.address))
		if adeps:
			deps.extend(adeps)
	
	def download(self, mon):
		
		# get the archive
		try:
			path = download(self.address, mon)
		except IOError as e:
			return False
		
		# extract it
		dir = unpack(path, mon)
		return True

DOWNLOADERS = {
	"hg"		: HgDownloader,
	"archive"	: ArchiveDownloader
}
def make_downloader(pack, elt):
	type = elt.get("type", elt)
	try:
		return DOWNLOADERS[type](pack, elt)
	except KeyError:
		raise AssertionError


class Builder:
	
	def __init__(self, pack):
		self.pack = pack

	def add_deps(self, deps):
		"""Called to let the builder add its own dependencies to the
		given version."""
		pass

	def build(self, mon):
		"""Perform a the build. Return True for success, False else."""
		return True
	
	def install(self, mon):
		"""Perform the installation and return None for failure,
		list of uninstall actions for success."""
		return []

class MakeBuilder(Builder):
	
	def __init__(self, pack, elt):
		Builder.__init__(self, pack)
		self.flags = elt.get("flags", "")
	
	def add_deps(self, deps):
		deps.append(MAKE_DEP)
	
	def build(self, mon):
		path = os.path.join(mon.get_build_dir(), self.pack.name)
		cur = os.getcwd()
		os.chdir(path)
		cmd = "make %s" % mon.eval(self.flags)
		mon.log("\nBuilding %s: %s\n" % (self.pack.name, cmd))
		res = mon.execute(cmd, log = True)
		os.chdir(path)
		return res == 0

	def install(self, mon):
		path = os.path.join(mon.get_build_dir(), self.pack.name)
		cur = os.getcwd()
		os.chdir(path)
		cmd = "make install %s" % mon.eval(self.flags)
		mon.log("\nInstalling %s: %s\n" % (self.pack.name, cmd))
		res = mon.execute(cmd, log = True)
		os.chdir(path)
		if res == 0:
			return []
		else:
			return None
		

class CMakeBuilder(Builder):
	
	def __init__(self, pack, elt):
		Builder.__init__(self, pack)
		self.flags = elt.get("flags", "")
	
	def add_deps(self, deps):
		deps.append(CMAKE_DEP)
	
	def build(self, mon):
		path = os.path.join(mon.get_build_dir(), self.pack.name)
		cur = os.getcwd()
		os.chdir(path)
		cmd = "cmake . %s" % mon.eval(self.flags)
		mon.log("\nSetting up %s: %s\n" % (self.pack.name, cmd))
		res = mon.execute(cmd, log = True)
		if res == 0:
			mon.log("\nBuilding %s: make\n" % self.pack.name)
			res = mon.execute("make", log = True)
		os.chdir(path)
		return res == 0		
		
	def install(self, mon):
		path = os.path.join(mon.get_build_dir(), self.pack.name)
		cur = os.getcwd()
		os.chdir(path)
		cmd = "make install"
		mon.log("\nInstalling %s: %s\n" % (self.pack.name, cmd))
		res = mon.result_of(cmd, log = True)
		os.chdir(path)
		if res == None:
			return False
		else:
			acts = []
			for line in res.split("\n"):
				if line.startswith("-- Installing: "):
					file = os.path.relpath(line[15:], mon.top_dir)
					acts.append(Remove(file))
			return acts

class CommandBuilder(Builder):
	"""Builder based on a command call."""
	
	def __init__(self, pack, elt):
		Builder.__init__(self, pack)
		self.build_cmd = elt.get("build")
	
	def build(self, mon):
		if self.build:
			path = os.path.join(mon.get_build_dir(), self.pack.name)
			cur = os.getcwd()
			mon.log("cwd set to %s\n" % path)
			os.chdir(path)
			mon.log("executing %s\n" % self.build_cmd)
			res = mon.execute(self.build_cmd, None, None, True)
			os.chdir(path)
			if res == 0:
				return []
			else:
				return None
		else:
			return []
		

BUILDERS = {
	"make": MakeBuilder,
	"cmake": CMakeBuilder,
	"command": CommandBuilder
}
def make_builder(pack, elt):
	type = elt.get("type", None)
	assert type <> None
	try:
		return BUILDERS[type](pack, elt)
	except KeyError:
		raise AssertionError


# version management
class Version:
	
	def __init__(self, version):
		self.version = version
		self.pack = None
	
	def install(self, mon):
		"""Install the given module."""
		pass

	def __repr__(self):
		return "%s (%s)" % (self.pack.name, self.version)

	def __lt__(self, v):
		return is_older(self.version, v.version)

class BinaryVersion(Version):
	"""Represents a binary package to download."""
	
	def __init__(self, version, file, size, checksum):
		Version.__init__(self, version)
		self.file = file
		self.size = size
		self.checksum = checksum
		self.deps = []
	
	def install(self, mon):
		"""Install a package by downloading it from a server."""
	
		try:
			
			# download the package
			file = os.path.basename(self.file)
			mon.comment("source URL: %s" % self.file)
			mon.check("loading " + file)
			path = download(self.file, mon)
			mon.comment("local file: %s" % path)
			mon.succeed()
		
			# unpack it
			mon.check("unpacking %s" % file)
			dpath = unpack(path, mon)
			mon.succeed()

			# parse the installs script
			doc = ET.parse(os.path.join(dpath, "install.xml"))
			root = doc.getroot()
			if root.tag <> SIS_INSTALL_TAG:
				mon.fatal("error in package: bad script")
			actions = []
			for elt in root:
				if elt.tag <> None:
					action = get_action(elt, mon)
					if action == None:
						raise IOError("bad action")
					actions.append(action)
			
			# install it
			mon.check("installing %s" % file)
			uninstall = []
			old_cwd = os.getcwd()
			os.chdir(dpath)
			for action in actions:
				action.perform(mon)
				raction = action.reverse()
				if raction <> None:
					uninstall.append(raction)
		
			# record the installation
			mon.succeed()
			self.pack.installed = True
			self.pack.installed_version = self.version
			save_site(mon.site_path, self.pack, mon, uninstall)
		
		except urllib2.URLError as e:
			mon.fatal(e)
		except IOError as e:
			mon.fatal(e)


class SourceVersion(Version):
	"""Version to download from sources."""
	
	def __init__(self, download, build, deps):
		Version.__init__(self, SRCE_VERS)
		self.download = download
		self.build = build
		self.deps = deps
	
	def get_version(self):
		"""Obtain the version either from the object itself, or from
		a file named "VERSION" in the root directory of sources."""
		if self.version == SRCE_VERS and os.access(VERSION_FILE, os.R_OK):
			self.version = open(VERSION_FILE).readline().strip()
		return self.version
	
	def install(self, mon):
		"""Install a package from source."""

		# download
		if self.download:
			mon.check("downloading %s" % self.pack.name)
			if self.download.download(mon):
				mon.succeed()
			else:
				mon.fail()
				mon.fatal("Download failed: see errors in build.log")

		# build
		if self.build:
			mon.check("building %s" % self.pack.name)
			if self.build.build(mon):
				mon.succeed()
			else:
				mon.fail()
				mon.fatal("Build failed: see errors in build.log")

		# install
		if not self.pack.tool:
			mon.check("installing %s" % self.pack.name)
			res = self.build.install(mon)
			if res <> None:
				mon.succeed()
				self.pack.installed = True
				self.pack.installed_version = self.get_version()
				save_site(mon.site_path, self.pack, mon, res)
			else:
				mon.fail()
				mon.fatal("Installation failed: see errors in build.log")


# package definition
class Package:
	
	def __init__(self, name):
		self.name = name
		self.tool = False

		self.reqs = []
		self.uses = []
		self.closed_uses = None
		self.closed_reqs = None
		self.desc = None
		self.copyright = None
		self.license = None
		self.license_url = None
		self.category = None
		self.contact = None
		self.web = None
		self.versions = []
		self.uninstall = []
		
		self.done = False
		self.installed = False
		self.installed_version = None
		self.initial = False
	
	def add_version(self, version):
		self.versions.append(version)
		version.pack = self

	def source(self):
		"""Obtain the source version if any. If none, return None."""
		for v in self.versions:
			if v.version == SRCE_VERS:
				return v
		return None
	
	def latest(self):
		"""Return the latest version."""
		l = None
		for v in self.versions:
			if v.version <> SRCE_VERS and (l == None or is_younger(v, l)):
				l = v
		if l == None:
			return self.source()
		else:
			return l

	def get_closed_uses(self):
		"""Close the list of uses."""
		if self.closed_uses == None:
			self.closed_uses = do_closure(lambda p: p.uses + p.reqs, [self])
		return self.closed_uses
	
	def get_close_reqs(self):
		"""Close the ist of requirements."""
		if self.closed_reqs == None:
			self.closed_reqs = do_closure(lambda p: p.reqs, [self])
		return self.closed_reqs
	
	def __repr__(self):
		return self.name


# get the database
def to_str(s, d):
	if s == None:
		return d
	else:
		return s.text

def to_int(s, d):
	if s == None:
		return d
	else:
		return int(s.text)

def to_bool(s, d):
	if s == None:
		return d
	else:
		return bool(s)

def to_xml(s, d):
	if s == None:
		return d
	else:
		return s


def load_db(url, mon, installed = False):
	"""Load the data base from the given URL. The load is incremental.
	If a plugin already exist, it is completed with new values.
	installed parameter is used to set the installed attribute."""
	mon.comment("getting database from %s" % url)
	try:
		
		# get the file
		stream = urllib2.urlopen(url)
		doc = ET.parse(stream)
		root = doc.getroot()
		assert root.tag == SIS_EXTEND_TAG
		
		# parse the content
		for p in root.findall("package"):
			
			# get the package
			id = p.get("id", None)
			assert id
			try:
				pack = DB[id]
				mon.comment("package %s already exsisting." % id)
			except KeyError:
				pack = Package(id)
				pack.installed = installed
				DB[id] = pack
				MONITOR.comment("package %s added." % id)
			
			# get information from the package
			pack.tool = to_bool(p.get("tool", None), pack.tool)
			desc = p.find("desc")
			pack.desc = to_xml(p.find("desc"), pack.desc)
			pack.copyright = to_str(p.find("copyright"), pack.copyright)
			pack.license = p.find("license")
			if pack.license <> None:
				pack.license_url = pack.license.get("ref", pack.license_url)
				pack.license = to_str(pack.license, pack.license)
			pack.category = to_str(p.find("category"), pack.category)
			pack.web = to_str(p.find("web"), pack.web)
			pack.contact = to_str(p.find("contact"), pack.contact)
			if installed:
				pack.installed = True
				pack.installed_version = p.get("version", NULL_VERS)

			# get requirements
			reqs = p.find("reqs")
			if reqs <> None:
				for req in reqs.findall("req"):
					assert req.attrib.has_key("name")
					pack.reqs.append(req.get("name"))
			pack.reqs = pack.reqs + [e.get("name") for e in p.findall("req")]
			
			# get uses
			pack.uses = [e.get("name") for e in p.findall("use")]
			
			# get binary versions
			for velt in p.findall("version"):
				version = velt.get("number", None)
				assert version <> None
				file = to_str(velt.find("file"), None)
				assert file <> None
				try:
					file = url[:url.rfind("/")] + "/" + file
				except ValueError:
					pass
				size = to_int(velt.find("size"), None)
				checksum = to_str(velt.find("checksum"), None)
				pack.add_version(BinaryVersion(version, file, size, checksum))

			# get build information
			belt = p.find("build")
			if belt <> None:
				download = belt.find("download")
				if download <> None:
					download = make_downloader(pack, download)
				make = belt.find("make")
				build = None
				if make <> None:
					build = make_builder(pack, make)
				deps = []
				for dep in belt.findall("dep"):
					add_deps(dep, deps)
				if download <> None:
					download.add_deps(deps)
				if build <> None:
					build.add_deps(deps)
				pack.add_version(SourceVersion(download, build, deps))
			
			# get uninstall if required
			if installed:
				uelt = p.find("uninstall")
				if uelt <> None:
					for aelt in uelt:
						action = get_action(aelt, mon)
						assert action <> None
						pack.uninstall.append(action)
			
	except (AssertionError, ET.ParseError):
		mon.fatal("bad DB. Stopping.")
	except urllib2.URLError as e:
		mon.fatal(e)


def save_site(path, ipack, mon, uninstall = None, remove = False):
	"""Save the data base at the given URL, more exacly, load the database
	and add newly installed packages. uninstall (optional) is the list
	of action to apply to uninstall the package. If remove is true,
	a removal is performed."""
	
	mon.comment("getting database from %s" % path)
	try:
		
		# get the file
		stream = open(path)
		doc = ET.parse(stream)
		root = doc.getroot()
		assert root.tag == SIS_EXTEND_TAG

		# look for package with the good name
		done = False
		for pack in root.findall("package"):
			if pack.get("id", None) == ipack.name:
				done = True
				break
		
		# remove case
		if remove:
			if done:
				root.remove(pack)
		
		# install case
		else:
		
			# not found: create it
			if not done:
				pack = ET.SubElement(root, "package")
				pack.attrib["id"] = ipack.name			
		
			# update the package
			pack.set("version", ipack.installed_version) 

			# update the uninstall information
			uelt = pack.find("unistall")
			if uelt <> None:
				pack.remove(uelt)
			if uninstall:
				uelt = ET.SubElement(pack, "uninstall")
				for a in uninstall:
					save_action(uelt, a)
		
		# save the database
		stream.close()
		
		# save the configuration
		old_path = path + ".old"
		mon.comment("writing installed DB to %s" % path)
		os.rename(path, old_path)
		try:
			doc.write(path, encoding="UTF-8", xml_declaration = True)
			os.remove(old_path)
		except OSError, e:
			os.remove(path)
			os.rename(old_path, path)
			mon.fatal("cannot write install DB: %s" % e)

	except AssertionError:
		mon.fatal("ERROR: bad DB. Stopping.")
	except urllib2.URLError as e:
		mon.fatal(e)


def resolve_reqs(mon):
	"""Delayed resolution of requirements (before all is loaded)."""
	for pack in DB.values():
		
		nreqs = []
		for req in pack.reqs:
			try:
				nreqs.append(DB[req])
			except KeyError:
				mon.fatal("don't know to get %s required by %s" % (req, pack.name))
		pack.reqs = nreqs

		nuses = []
		for use in pack.uses:
			try:
				nuses.append(DB[use])
			except KeyError:
				mon.fatal("don't know to get %s used by %s" % (use, pack.name))
		pack.uses = nuses


def list_packs( mon):
	"""List the packages."""
	for pack in DB.values():
		if pack.initial:
			continue
		if pack.tool:
			continue
		msg = pack.name
		
		if not pack.installed:
			lv = pack.latest()
			if lv == None:
				continue
			msg = "%s not installed (avail. %s)" % (msg, lv.version)			
		else:
			if pack.installed_version:
				if pack.installed_version == SRCE_VERS:
					msg = "%s source" % msg
				else:
					msg = "%s V%s" % (msg, pack.installed_version)
			if pack.installed_version:
				lv = pack.latest()
				if lv and lv.version <> SRCE_VERS and is_younger(lv.version, pack.installed_version):
					msg = "%s (avail. %s)" % (msg, lv.version)

		mon.say(msg)
	
	
def info_pack(pack, mon):
	"""Display information for the given package."""
	line = pack.name
	if pack.installed_version:
		line = "%s V%s" % (line, pack.installed_version)
	if pack.category:
		line = "%s (%s)" % (line, pack.category)
	mon.say(line)
	for vers in pack.versions:
		if vers.version == SRCE_VERS:
			mon.say("\tsource install")
		else:
			msg = "\tV%s binary install" % (vers.version)
			if vers.size:
				msg = "%s (%.1f KB)" % (msg, vers.size)
			mon.say(msg)
	if pack.copyright != None:
		mon.say("\t%s" % pack.copyright)
	if pack.license != None:
		mon.say("\t%s" % pack.license)
	if pack.desc != None:
		mon.say("\t%s" % pack.desc.text)
	if pack.web != None:
		mon.say("\t%s" % pack.web)
	if pack.contact != None:
		mon.say("\t%s" % pack.contact)
	reqs = pack.get_close_reqs()
	if reqs:
		mon.say("\trequirements: %s" % (", ".join(sorted([repr(r) for r in reqs]))))
	exit(0)
		

def install_packs(vs, mon):
	"""Install the given packages made of version list."""
	vmap = { v.pack: v for v in vs }
	
	# closure of packages to download
	todo = set(vs)
	to_install = { }
	while todo:
		v = todo.pop()
		if not v.pack.installed or (mon.force and v in vs):
			rvs = set()
			for req in v.pack.reqs:
				try:
					rv = vmap[req]
				except KeyError:
					rv = req.latest()
					vmap[req] = rv
				if rv <> None:
					rvs.add(rv)
					if rv not in to_install:
						todo.add(rv)
			to_install[v] = rvs

	# check all deps first
	for v in to_install:
		for dep in v.deps:
			if not dep.tested:
				dep.do_test(mon)
				if not dep.succeeded:
					mon.fatal("missing dependency %s. Stopping." % dep.name)

	# perform the build
	done = []
	
	def is_ready(v):
		return v in done or v.pack.installed
	
	def req_ready(v):
		res = all([is_ready(rv) for rv in to_install[v]])
		return res
	
	def may_use(v1, v2):
		if v1.pack in v2.pack.get_closed_uses():
			return -1
		elif v2.pack in v1.pack.get_closed_uses():
			return +1
		else:
			return 0
	
	while to_install:
		v = sorted(filter(req_ready, to_install), may_use)[0]
		del to_install[v]
		if mon.dry:
			mon.say("installing %s" % v.pack.name)
		else:
			try:
				v.install(mon)
			except IOError as e:
				mon.fatal("error during installation: %s" % e)
		done.append(v)


def uninstall_packs(packs, mon):
	"""Uninstall the given list of packages. Take as parameter
	the list of parameter names."""
	
	# check packages
	for pack in packs:
		if not pack.installed:
			mon.fatal("package %s is not installed!" % pack.name)
	
	# compute closure of packages to uninstall
	upacks = do_closure(lambda p: [u for u in DB.values() if p in u.reqs and u.installed], packs)
	if len(upacks) > 0 and \
	not mon.ask_yes_no("The packages %s will also be unistalled?" % ", ".join([str(p) for p in upacks])):
		mon.fatal("Uninstallation aborted.")
	packs = packs + upacks

	# perform uninstallation
	old_dir = os.getcwd()
	os.chdir(mon.top_dir)
	for pack in packs:
		mon.check("uinstalling %s" % pack.name)
		for action in pack.uninstall:
			action.perform(mon)
		save_site(mon.site_path, pack, mon, None, True)
		mon.succeed()
	os.chdir(old_dir)

def get_packs(names, mon):
	"""Get the list of packages from the list of names."""
	try:
		return [DB[name] for name in names]
	except KeyError as e:
		MONITOR.fatal("unknown package: %s" % e)

def get_versions(names, mon):
	"""Get the list of version from the list of names"""
	try:
		vs = set()
		for name in names:
			cs = name.split(":")
			pack = DB[cs[0]]
			if len(cs) == 1:
				vs.add(pack.latest())
			else:
				done = False
				for vers in pack.versions:
					if vers.version == cs[1]:
						vs.add(vers)
						done = True
						break
				if not done:
					mon.fatal("unknown version %s for %s" % (cs[1], cs[0]))
		return vs
	except KeyError as e:
		MONITOR.fatal("unknown package: %s" % e)

def install_root(mon):
	"""Install the root directory, that is, the install database
	and the installation script itself."""
	try:
		ensure_dir(mon.top_dir)
		db_path = os.path.join(mon.top_dir, DB_CONF)
		ensure_dir(os.path.dirname(db_path))
		out = open(db_path, "w")
		out.write(
"""<?xml version='1.0' encoding='UTF-8'?>
<sis-extend>
</sis-extend>
""")
		out.close()
		bin_dir = os.path.join(mon.top_dir, "bin")
		ensure_dir(bin_dir)
		shutil.copy2(__file__, bin_dir)
	except OSError as e:
		mon.fatal(str(e))


# parse arguments
parser = argparse.ArgumentParser(description="installer tool for %s" % APP)
parser.add_argument('--base', '-b',
	help="select the database to use (default to " + DB_URL,
	default=DB_URL)
parser.add_argument('--list', '-l', action="store_true",
	help="display the list of available packages")
parser.add_argument('--info', '-i', action="store_true",
	help="display information information about the passed packages")
parser.add_argument('--top', '-t',
	help="select the top directory of the installation")
parser.add_argument('--build-dir', '-B',
	help="select the directory to perform the build in")
parser.add_argument('--dry', '-D', action="store_true",
	help="dry execution: only display packahes to install")
parser.add_argument('--verbose', '-v', action="store_true",
	help="verbose mode")
parser.add_argument('--force', '-f', action="store_true",
	help="force installation of selected package")
parser.add_argument('--log', '-L',
	help="select the log file to use")
parser.add_argument('--root', '-R',
	help="perform installation from scratch in the given directory")
parser.add_argument('--uninstall', '-u', action="store_true",
	help="uninstall the selected packages")
parser.add_argument('packages', nargs='*', help="packages to install")
args = parser.parse_args()

if args.build_dir:
	try:
		ensure_dir(args.build_dir)
		MONITOR.build_dir = args.build_dir
	except OSError as e:
		MONITOR.fatal("%s for the building directory!" % e)
MONITOR.verbose = args.verbose
MONITOR.dry = args.dry
MONITOR.force = args.force
MONITOR.comment("build directory = %s" % MONITOR.build_dir)

# find the top directory
if args.root <> None:	
	ROOTED = True
	MONITOR.top_dir = args.root
	install_root(MONITOR)
else:
	ROOTED = False
	MONITOR.top_dir = args.top
	if not MONITOR.top_dir:
		MONITOR.top_dir = os.path.join(os.path.dirname(__file__), DB_TOP)
MONITOR.top_dir = os.path.abspath(MONITOR.top_dir)
MONITOR.comment("top directory = %s" % MONITOR.top_dir)
MONITOR.set("top_dir", MONITOR.top_dir)
MONITOR.site_path = os.path.join(MONITOR.top_dir, DB_CONF)

# load the configuration
pack = None
load_db("file:" + MONITOR.site_path, MONITOR, True)
load_db(DB_URL + "/index.xml", MONITOR)
conf = MONITOR.get_host_type()
if conf <> None:
	load_db(DB_URL + "/" + conf + "/index.xml", MONITOR)		
resolve_reqs(MONITOR)

# perform actions
if args.list:
	list_packs(MONITOR)
elif args.info:
	for pack in get_packs(args.packages, MONITOR):
		info_pack(pack, MONITOR)
elif args.uninstall:
	uninstall_packs(get_packs(args.packages, MONITOR), MONITOR)
else:
	install_packs(get_versions(args.packages, MONITOR), MONITOR)
