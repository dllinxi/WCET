# doc -- Thot document escription module
# Copyright (C) 2009  <hugues.casse@laposte.net>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

import sys
import os
import os.path
import imp
import re
import traceback
import subprocess


class ThotException(Exception):
	"""Exception of the Thot system.
	Any back-passed to the Thot system must inherit this exception.
	Other exceptions will not be caught."""
	
	def __init__(self, msg):
		self.msg = msg
	
	def __str__(self):
		return self.msg
	
	def __repr__(self):
		return self.msg


class ParseException(ThotException):
	"""This exception may be thrown by any parser encountering an error.
	File and line information will be added by the parser."""
	
	def __init__(self, msg):
		ThotException.__init__(self, msg)


class BackException(ThotException):
	"""Exception thrown by a back-end."""
	
	def __init__(self, msg):
		ThotException.__init__(self, msg)


class CommandException(ThotException):
	"""Thrown if there is an error during a command call."""
	
	def __init__(self, msg):
		ThotException.__init__(self, msg)


IS_VERBOSE = False
ENCODING = "UTF-8"


def onVerbose(f):
	"""Invoke and display the result of the given function if verbose
	mode is activated."""
	if IS_VERBOSE:
		sys.stderr.write(f(()))
		sys.stderr.write("\n")


def show_stack():
	"""Show the stack (for debugging purpose)."""
	#exc_type, exc_obj, exc_tb = sys.exc_info()
	#traceback.print_tb(exc_tb)
	#traceback.print_stack()
	traceback.print_exc()
	return ""


def onParseError(msg):
	raise ParseException(msg)


def onError(text):
	"""Display the given error and stop the application."""
	onVerbose(lambda _: show_stack())
	sys.stderr.write("ERROR: %s\n" % text)
	sys.exit(1)


def onWarning(message):
	"""Display a warning message."""
	sys.stderr.write("WARNING: %s\n" % message)


def onInfo(message):
	"""Display an information message."""
	sys.stderr.write("INFO: %s\n" % message)


DEPRECATED = []
def onDeprecated(msg):
	"""Display a deprecated message with the given message."""
	if msg not in DEPRECATED:
		sys.stderr.write("DEPRECATED: %s\n" % msg)
		DEPRECATED.append(msg)


def loadModule(name, path):
	"""Load a module by its name and its path
	and return its object."""
	path = os.path.join(path, name + ".py")
	try:
		if os.path.exists(path):
			return imp.load_source(name, path)
		else:
			path = path + "c"
			if os.path.exists(path):
				return imp.load_compiled(name, path)
			else:
				return None
	except Exception, e:
		onError("cannot open module '%s': %s" % (path, str(e)))

AUTHOR_RE = re.compile('(.*)\<([^>]*)\>\s*')
def scanAuthors(text):
	"""Scan the author text to get structured representation of authors.
	text -- text containing author declaration separated by ','
	and with format "NAME <EMAIL>"
	Return a list of authors where each author dictionary
	containing 'name' and 'email' keys."""

	authors = []
	words = text.split(',')
	for word in words:
		author = {}
		match = AUTHOR_RE.match(word)
		if not match:
			author['name'] = word
		else:
			author['name'] = match.group(1)
			author['email'] = match.group(2)
		authors.append(author)
	return authors


def is_exe(fpath):
	return os.path.exists(fpath) and os.access(fpath, os.X_OK)


def which(program):
	"""Function to test if an executable is available.
	program: program to look for
	return: the found path of None."""
	
	fpath, fname = os.path.split(program)
	if fpath:
		if is_exe(program):
			return program
	else:
		for path in os.environ["PATH"].split(os.pathsep):
			exe_file = os.path.join(path, program)
			if is_exe(exe_file):
				return exe_file
	return None


def getLinuxDistrib():
	"""Look for the current linux distribution.
	Return (distribution, release) or None if version cannot be found."""
	try:
		id = ""
		release = 0
		for line in open("/etc/lsb-release"):
			if line.startswith("DISTRIB_ID="):
				id = line[11:-1]
			elif line.startswith("DISTRIB_RELEASE="):
				release = line[16:-1]
		return (id, release)
	except IOError,e:
		return ("", 0)


def onRaise(msg):
	"""Raise a command error with the given message."""
	raise CommandError(msg)

def onIgnore(msg):
	"""Ignore the error."""
	pass

ERROR_FAIL = onError
ERROR_RAISE = onRaise
ERROR_WARN = onWarning
ERROR_IGNORE = onIgnore


class CommandRequirement:
	"""Implements facilities for test for the existence of a command."""
	checked = False
	path = None
	cmd = None
	msg = None
	error = False
	
	def __init__(self, cmd, msg = None, error = ERROR_WARN):
		self.cmd = cmd
		self.msg = msg
		self.error = error
	
	def get(self):
		if not self.checked:
			self.checked = True
			self.path = which(self.cmd)
			if not self.path:
				self.error(self.msg)
		return self.path 


class Command(CommandRequirement):
	"""Handle the operation of an external command."""
	
	def __init__(self, cmd, msg = None, error = False):
		CommandRequirement.__init__(self, cmd, msg, error)
	
	def call(self, args = [], input = None, quiet = False):
		"""Call the command. Throw CommandException if there is an error.
		args -- list of arguments.
		input -- input to pass to the called command.
		quiet -- do not produce any output."""
		cmd = self.get()
		if not cmd:
			return
		out = None
		if quiet:
			out = open(os.devnull, "w")
		cmd = " ".join([ cmd ] + args)
		try:
			code = subprocess.check_call( cmd, close_fds = True, shell = True, stdout = out, stderr = out, stdin = input )
		except (OSError, subprocess.CalledProcessError) as e:
			self.error("command %s failed: %s" % (self.cmd, e))

	def scan(self, args = [], input = None, err = False):
		"""Launch a command and return the output if successful, a CommandException is raise else.
		input -- optional input stream.
		err -- if True, redirect also the standard error."""
		cmd = self.get()
		if not cmd:
			return
		errs = None
		if err:
			errs = subprocess.PIPE
		cmd = " ".join([ cmd ] + args)
		try:
			out = subprocess.check_output( cmd, close_fds = True, shell = True, stdout = subprocess.PIPE, stderr = errs, stdin = input )
			return out
		except (OSError, subprocess.CalledProcessError) as e:
			self.error("command %s failed: %s" % (self.cmd, e))


ESCAPES = [ '(', ')', '+', '.', '*', '/', '?', '^', '$', '\\', '|' ]
def escape_re(str):
	res = ""
	for c in str:
		if c in ESCAPES:
			res = res + "\\" + c
		else:
			res = res + c
	return res
	
