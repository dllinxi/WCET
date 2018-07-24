# docbook -- Thot docbook back-end
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

import common
import back
import doc
import subprocess
import os.path
import shutil
import cgi
import re

# USED VARIABLES
#	TITLE: document title
#	AUTHORS: document authors
#	LANG: document language
#	ENCODING: character encoding
#	OUTPUT: the kind of output (pdf only supported for now)
#	DOCBOOK_BACKEND: may be openjade or dblatex
#
# NOTES
#	highlight col/row headers not implemented (impossible for col)

AUTHOR_RE = re.compile('^([^<]*)\s*<([^>]*)>\s*')

STYLES = {
	'bold': 'varname',
	'italic': 'emphasis',
	'subscript': 'subscript',
	'superscript': 'superscript',
	'monospace': 'literal'
}

TAB_ALIGN = ['left', 'center', 'right']

class Generator(back.Generator):
	"""Generator for docbook back-end."""
	backend = None
	sgml = False
	output = False
	
	def __init__(self, doc):
		back.Generator.__init__(self, doc)
		self.output = self.doc.getVar('OUTPUT')
		if self.output:
			if self.output == 'pdf':
				self.backend = self.doc.getVar('DOCBOOK_BACKEND')
				if not self.backend:
					self.backend = 'dblatex'
				if self.backend == 'openjade':
					self.sgml = True
				elif self.backend == 'dblatex':
					self.sgml = False
				else:
					common.onError('DocBook back-end %s not supported' % self.backend)
			else:
				common.onError('DocBook output %s not supported' % self.output)

	def getType(self):
		return 'docbook'

	def run(self):
		self.openMain('.docbook')

		# generate document header
		self.doc.pregen(self)
		self.out.write('<?xml version="1.0" encoding="%s" standalone="yes"?>\n' % self.doc.getVar('ENCODING'))
		self.out.write('<!DOCTYPE book PUBLIC "-//OASIS//DTD DocBook XML V4.5//EN" "http://www.oasis-open.org/docbook/xml/4.5/docbookx.dtd">\n')
		if self.sgml:
			self.out.write('<book>\n')
		else:
			self.out.write('<book xmlns="http://docbook.org/ns/docbook" xmlns:xlink="http://www.w3.org/1999/xlink">\n')
		self.out.write('<title>%s</title>\n' % cgi.escape(self.doc.getVar('TITLE')))
		self.out.write('<info>\n')
		
		# generator author 
		authors = self.doc.getVar('AUTHORS')
		if authors:
			for author in authors.split(','):
				self.out.write('<author>')
				m = AUTHOR_RE.match(author)
				if m:
					name = m.group(1)
					email = m.group(2)
				else:
					name = ''
					email = ''
				self.out.write('<personname>%s</personname>' % cgi.escape(name))
				if email:
					self.out.write('<email>%s</email>' % cgi.escape(email))
				self.out.write('</author>\n')
		self.out.write('</info>\n')
		
		# generate body
		self.doc.gen(self)
		self.out.write('</book>\n')
		
		# run the backend
		if self.output == 'pdf':
			self.out.close()
			name, ext = os.path.splitext(self.path)
			if self.backend == 'dblatex':
				cmd = 'dblatex %s -o %s' % (self.path, name + ".pdf")
			elif self.back == 'openjade':
				cmd = 'docbook2pdf %s' % self.path
			process = subprocess.Popen(
					[cmd],
					shell = True,
					stdout = subprocess.PIPE,
					stderr = subprocess.PIPE
				)
			out, err = process.communicate('')
			if process.returncode <> 0:
				sys.stdout.write(out)
				sys.stderr.write(err)
				print "ERROR: failed"
			else:
				if self.backend == 'openjade':
					shutil.move(self.path + ".pdf", name + ".pdf")
				print "SUCCESS: result in %s" % (name + ".pdf")
		else:
			print "SUCCESS: result in %s" % self.path

	def genFootNote(self, note):
		self.out.write('<footnote>')
		for item in note:
			item.gen(self)
		self.out.write('</footnote>')

	def genQuoteBegin(self, level):
		self.out.write('<blockquote><para>')

	def genQuoteEnd(self, level):
		self.out.write('</para></blockquote>')

	def genTable(self, table):

		# table prolog
		self.out.write('<table><tgroup cols="%d">"\n' % table.getWidth())
		for i in xrange(0, table.getWidth()):
			self.out.write('<colspec colname="%d"/>' % i)
		self.out.write('<tbody>\n')

		# output rows
		for row in table.getRows():
			self.out.write('<row>\n')
			icol = 0

			# output columns
			for cell in row.getCells():
				self.out.write('<entry')
				if cell.align <> doc.TAB_LEFT:
					self.out.write(' align="%s"' % TAB_ALIGN[cell.align + 1])
				if cell.span <> 1:
					self.out.write(' namest="%d" nameend="%d"' % (icol, icol + cell.span - 1))
				icol += cell.span
				self.out.write('>')
				cell.gen(self)
				self.out.write('</entry>')
				
			self.out.write('</row>\n')		
		
		# table epilog
		self.out.write('</tbody></tgroup></table>\n')
	

	def genHorizontalLine(self):
		pass
	
	def genVerbatim(self, line):
		self.out.write(line)
	
	def genText(self, text):
		self.out.write(cgi.escape(text))

	def genParBegin(self):
		self.out.write('<para>')
	
	def genParEnd(self):
		self.out.write('</para>\n')

	def genList(self, list):
		if list.kind == 'ul':
			self.out.write('<itemizedlist>\n')
		elif list.kind == 'ol':
			self.out.write('<orderedlist>\n')
		else:
			common.onWarning('docbook does not support %s list' % list.kind)
	
		for item in list.getItems():
			self.out.write('<listitem>')
			item.gen(self)
			self.out.write('</listitem>\n')

		if list.kind == 'ul':
			self.out.write('</itemizedlist>\n')
		elif list.kind == 'ol':
			self.out.write('</orderedlist>\n')

	def genStyleBegin(self, kind):
		if STYLES.has_key(kind):
			self.out.write('<%s>' % STYLES[kind])

	def genStyleEnd(self, kind):
		if STYLES.has_key(kind):
			self.out.write('</%s>' % STYLES[kind])

	def genHeaderBegin(self, level):
		if level == 0:
			self.out.write('<chapter>\n')
		else:
			self.out.write('<section>\n')
		
	def genHeaderTitleBegin(self, level):
		self.out.write('<title>')

	def genHeaderTitleEnd(self, level):
		self.out.write('</title>\n')
	
	def genHeaderBodyBegin(self, level):
		pass
	
	def genHeaderBodyEnd(self, level):
		pass
	
	def genHeaderEnd(self, level):
		if level == 0:
			self.out.write('</chapter>\n')
		else:
			self.out.write('</section>\n')

	def genLinkBegin(self, url):
		self.out.write('<link xlink:href="%s">' % cgi.escape(url, True))
	
	def genLinkEnd(self, url):
		self.out.write('</link>')
	
	def genImage(self, url, width = None, height = None, caption = None):
		name, ext = os.path.splitext(url)
		fpath = self.loadFriendFile(url)
		self.out.write('<inlinemediaobject>')
		if caption:
			self.out.write('<alt>%s</alt>' % cgi.escape(caption))
		self.out.write('<imageobject>')
		self.out.write('<imagedata format="%s" fileref="%s"' % (ext.upper(), cgi.escape(fpath, True)))
		if width:
			self.out.write(' width="%dpt"' % width)
		if height:
			self.out.write(' depth="%dpt"' % height)
		self.out.write('/>')
		self.out.write('</imageobject></inlinemediaobject>')
	
	def genGlyph(self, code):
		self.out.write('&#' + str(code) + ';')

	def genLineBreak(self):
		pass

def output(doc):
	gen = Generator(doc)
	gen.run()
