# back -- Thot commmon back-end structure
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

"""Definition of the abstract class back.Generator to help generation
of output files.

The following variables are supported:
* FRIEND_RELOC - option to handle friend files ("local" (default): relocate all except relative
  files, "all": relocate all files)
"""

import i18n
import os.path
import shutil
import common
import doc as tdoc
import sys

class Generator:
	"""Abstract back-end generator."""
	doc = None
	friend_reloc = "local"
	path = None
	root = None
	out = None
	from_files = None
	to_files = None
	added_files = None

	def __init__(self, doc):
		"""Build the abstract generator.
		doc -- document to generate."""
		self.doc = doc
		self.trans  = i18n.getTranslator(self.doc)
		self.from_files = { }
		self.to_files = { }
		self.added_files = []
		
		# new friend system
		self.friend_reloc = doc.getVar("FRIEND_RELOC", "local")
		self.friends = []
		self.friend_map = { }

	def getType(self):
		"""Get type of the back-end: html, latex, xml."""
		return None

	def addedFiles(self):
		"""Get the added files."""
		return self.added_files

	def addFile(self, file):
		"""Add a file to the list of files linked to the document."""
		if file not in self.added_files:
			self.added_files.append(file)

	def friendFiles(self):
		"""Get the friend files of the document."""
		print self.from_files
		print self.to_files
		return self.to_files

	def getImportDir(self):
		"""Get the directory containing the imports."""
		return self.root + "-imports"

	def openMain(self, suff, out_name = None):
		"""Create and open an out file for the given document.
		suff -- suffix of the out file.
		out_name -- default file to output to (special "<stdout>" supported).
		Set path, root and out fields."""

		self.path = self.doc.getVar("THOT_OUT_PATH")
		if self.path:
			self.out = open(self.path, "w")
			if self.path.endswith(suff):
				self.root = self.path[:-5]
			else:
				self.root = self.path
		else:
			if out_name:
				in_name = out_name
			else:
				in_name = self.doc.getVar("THOT_FILE")
			if not in_name or in_name == "<stdout>":
				self.out = sys.stdout
				self.path = "stdout"
				self.root = "stdout"
			else:
				if in_name.endswith(".thot"):
					self.path = in_name[:-5] + suff
					self.root = in_name[:-5]
				else:
					self.path = in_name + suff
					self.root = self.path
				self.out = open(self.path, "w")


	def get_friend(self, path, base = ''):
		"""Test if a file is a friend file and returns its generation
		relative path. Return None if the the file is not part
		of the generation.
		path -- path of the file,
		base -- potential file base."""
		
		if not os.path.isabs(path):
			if base:
				apath = os.path.join(base, path)
			else:
				apath = os.path.abspath(path)
		else:
			apath = os.path.normpath(path)
		if self.from_files.has_key(apath):
			return self.from_files[apath]
		else:
			return None

	def prepare_friend(self, path):
		"""Prepare friend file to be created (ensuring uniqueness
		and existence of directories (maintain the same path suffix).
		Return the actual path."""
		
		# create directories
		dpath = os.path.dirname(path)
		if not os.path.exists(dpath):
			try:
				os.makedirs(dpath)
			except os.error, e:
				common.onError('cannot create directory "%s": %s' % (dpath, e))
		
		# ensure uniqueness
		file, ext = os.path.splitext(path)
		cnt = 0
		while self.to_files.has_key(path):
			path = "%s-%d.%s" % (file, cnt, ext)
			cnt = cnt + 1
		return path

	def new_friend(self, path):
		"""Allocate a place in friend files for the given path
		(that must be relative)."""
		fpath = self.prepare_friend(os.path.join(self.getImportDir(), path))
		self.addFile(fpath)
		self.to_files[fpath] = ""
		return fpath

	def copy_friend(self, spath, tpath):
		"""Load a friend file in the generation location.
		path -- relative path to write to.
		base -- absolute file to the file to copy."""
		tpath = self.prepare_friend(tpath)
		try:
			shutil.copyfile(spath, tpath)
			return tpath
		except shutil.Error, e:
			common.onError('can not copy "%s" to "%s": %s' % (spath, tpath, str(e)))
		except IOError, e:
			common.onError('can not copy "%s" to "%s": %s' % (spath, tpath, str(e)))


	def use_friend(self, path, base = ''):
		"""Ensure that a friend is available. If not, get it from
		the given path. If absolute, the file is loaded in the import
		directory and a relative path (to the document directory) is
		returned.
		path -- path to the file
		base -- base directory containing the file ('' for CWD files)"""

		# already declared?
		tpath = self.get_friend(path, base)
		if tpath:
			return tpath 

		# make target path
		if base:
			base = os.path.normpath(base)
		if os.path.isabs(path):
			apath = path
			if base:
				tpath = os.path.join(self.getImportDir(), os.path.relpath(path, base))
			else:
				tpath = os.path.join(self.getImportDir(), os.path.basename(path))
		elif base:
			apath = os.path.join(base, path)
			tpath = os.path.join(self.getImportDir(), path)
		else:
			apath = os.path.abspath(path)
			if self.friend_reloc == "local":
				tpath = path
			else:
				tpath = os.path.join(self.getImportDir(), path)

		# need to load?
		if tpath <> path:
			tpath = self.copy_friend(apath, tpath)

		# record all
		self.from_files[apath] = tpath
		self.to_files[tpath] = apath;
		self.addFile(tpath)		
		return tpath

	def relative_friend(self, fpath, bpath):
		"""Get the relative path of fpath reative to bpath."""
		r = os.path.relpath(fpath, bpath)
		return r

	def genFootNote(self, note):
		pass

	def genQuoteBegin(self, level):
		pass

	def genQuoteEnd(self, level):
		pass

	def genTable(self, table):
		"""Called when a table need to be generated."""
		pass

	def genHorizontalLine(self):
		pass

	def genVerbatim(self, line):
		"""Put the given line as is in the generated code.
		The output line must meet the conventions of the output language."""
		pass

	def genText(self, text):
		"""Put the given text as normal in the output, possibly escaping some
		character to maintain right display."""
		pass

	def genParBegin(self):
		pass

	def genParEnd(self):
		pass

	def genList(self, list):
		"""Generate output for a list."""
		pass

	def genDefList(self, deflist):
		"""Called to generate a definition list."""
		pass

	def genStyleBegin(self, kind):
		pass

	def genStyleEnd(self, kind):
		pass

	def genHeader(self, header):
		return False

	def genHeaderBegin(self, level):
		pass

	def genHeaderTitleBegin(self, level):
		pass

	def genHeaderTitleEnd(self, level):
		pass

	def genHeaderBodyBegin(self, level):
		pass

	def genHeaderBodyEnd(self, level):
		pass

	def genHeaderEnd(self, level):
		pass

	def genLinkBegin(self, url):
		pass

	def genLinkEnd(self, url):
		pass

	def genImage(self, url, width = None, height = None, caption = None, align = tdoc.ALIGN_NONE, node = None):
		pass

	def genGlyph(self, code):
		pass

	def genLineBreak(self):
		pass

	def genEmbeddedBegin(self, node):
		"""Start an embedded area with the given label (a paragraph).
		Usual kinds include "listing", "figure", "table", "algo"."""
		pass

	def genEmbeddedEnd(self, node):
		"""End of generation for an embedded."""
		pass

	def genRef(self, ref):
		"""Called to generate a reference."""
		pass

