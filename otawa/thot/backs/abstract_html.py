# abstract_html -- Thot html back-end
# Copyright (C) 2016  <hugues.casse@laposte.net>
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
import cgi
import i18n
import highlight
import doc
import shutil
import re
import urlparse
import common
import back
import doc as tdoc

CSS_URL_RE = re.compile('url\(([^)]*)\)')

LISTS = {
	'ul': ('<ul %s>', '<li>', '</li>', '</ul>'),
	'ol': ('<ol %s>', '<li>', '</li>', '</ol>'),
}
def getList(list):
	if LISTS.has_key(list):
		return LISTS[list]
	else:
		raise Exception('list ' + list + ' not supported')

STYLES = {
	doc.STYLE_BOLD: 		('<b>', '</b>'),
	doc.STYLE_STRONG: 		('<strong>', '</strong>'),
	doc.STYLE_ITALIC: 		('<i>', '</i>'),
	doc.STYLE_EMPHASIZED:	('<em>', '</em>'),
	doc.STYLE_UNDERLINE: 	('<u>', '</u>'),
	doc.STYLE_SUBSCRIPT: 	('<sub>', '</sub>'),
	doc.STYLE_SUPERSCRIPT: 	('<sup>', '</sup>'),
	doc.STYLE_MONOSPACE: 	('<tt>', '</tt>'),
	doc.STYLE_STRIKE: 		('<strike>', '</strike>'),
	doc.STYLE_BIGGER:		('<big>', '</big>'),
	doc.STYLE_SMALLER:		('<small>', '</small>'),
	doc.STYLE_CITE:			('<cite>', '</cite>'),
	doc.STYLE_CODE:			('<code>', '</code>')
}

ESCAPE_MAP = {
	'<'	: "&lt;",	
	'>'	: "&gt;",
	'&'	: "&amp;"
}
def escape(s):
	r = ""
	for c in s.decode(common.ENCODING):
		if ord(c) >= 128:
			r = r + ("&#%d;" % ord(c))
		elif ESCAPE_MAP.has_key(c):
			r = r + ESCAPE_MAP[c]
		else:
			r = r + c
	return r

def getStyle(style):
	if STYLES.has_key(style):
		return STYLES[style]
	else:
		raise Exception('style ' + style + ' not supported')


class Generator(back.Generator):
	"""Generator for HTML output."""
	trans = None
	doc = None
	path = None
	root = None
	from_files = None
	to_files = None
	footnotes = None
	struct = None
	pages = None
	page_count = None
	stack = None
	label = None
	refs = None

	def __init__(self, doc):
		back.Generator.__init__(self, doc)
		self.footnotes = []
		self.pages = { }
		self.pages = { }
		self.page_count = 0
		self.stack = []
		self.refs = { }

	def getType(self):
		return "html"

	def importCSS(self, spath, base = ""):
		"""Perform import of files found in a CSS stylesheet.
		spath -- path to the original CSS stylesheet.
		base -- base path of the source."""

		# get target path
		if spath.startswith(self.getImportDir()):
			return spath
		path = self.get_friend(spath, base)
		if path:
			return path
		if os.path.isabs(spath):
			base = os.path.dirname(spath)
			spath = os.path.basename(spath)
		tpath = self.new_friend(spath)
		spath = os.path.join(base, spath)

		# open files
		input = open(spath)
		output = open(tpath, "w")
		rbase = os.path.dirname(spath)

		# perform the copy
		for line in input:
			m = CSS_URL_RE.search(line)
			while m:
				output.write(line[:m.start()])
				url = m.group(1)
				res = urlparse.urlparse(url)
				if res[0]:
					output.write(m.group())
				else:
					rpath = os.path.relpath(os.path.join(rbase, res[2]), base)
					rpath = self.use_friend(rpath, base)
					output.write("url(%s)" % self.relative_friend(rpath, os.path.dirname(tpath)))
				line = line[m.end():]
				m = CSS_URL_RE.search(line)
			output.write(line)

		# return path
		return tpath

	def genFootNote(self, note):
		if note.kind <> doc.FOOTNOTE_REF:
			self.footnotes.append(note)
		if note.kind <> doc.FOOTNOTE_DEF:
			if note.ref:
				id = note.id
				ref = "#footnote-custom-%s" % note.ref
			else:
				id = str(len(self.footnotes))
				ref = "#footnote-%s" % id
			self.out.write('<a class="footnumber" href="%s">%s</a>' % (ref, id))

	def genFootNotes(self):
		if not self.footnotes:
			return
		num = 1
		self.out.write('<div class="footnotes">\n')
		for note in self.footnotes:
			self.out.write('<p class="footnote">\n')
			if note.ref:
				id = note.id 
				ref = "footnote-custom-%s" % note.ref
			else:
				id = str(num)
				ref = "footnote-%d" % num
			self.out.write('<a class="footnumber" name="%s">%s</a> ' % (ref, id))
			num = num + 1
			for item in note.content:
				item.gen(self)
			self.out.write('</p>\n')
		self.out.write('</div>')

	def genQuoteBegin(self, level):
		for i in range(0, level):
			self.out.write('<blockquote>')
		self.out.write('\n')

	def genQuoteEnd(self, level):
		for i in range(0, level):
			self.out.write('</blockquote>')
		self.out.write('\n')

	def genTable(self, table):
		self.out.write('<div class="table">')
		self.out.write('<table>\n')
		for row in table.getRows():
			self.out.write('<tr>\n')
			for cell in row.getCells():

				if cell.kind == doc.TAB_HEADER:
					self.out.write('<th')
				else:
					self.out.write('<td')
				align = cell.getInfo(doc.INFO_ALIGN)
				if not align or align == doc.TAB_LEFT:
					pass
				elif align == doc.TAB_RIGHT:
					self.out.write(' align="right"')
				else:
					self.out.write(' align="center"')
				hspan = cell.getInfo(doc.INFO_HSPAN)
				if hspan:
					self.out.write(' colspan="' + str(hspan) + '"')
				vspan = cell.getInfo(doc.INFO_VSPAN)
				if vspan:
					self.out.write(' rowspan="' + str(vspan) + '"')
				self.out.write('>')
				cell.gen(self)
				if cell.kind == doc.TAB_HEADER:
					self.out.write('</th>\n')
				else:
					self.out.write('</td>\n')

			self.out.write('</tr>\n')
		self.out.write('</table>\n')
		self.genLabel(table)
		self.out.write('</div>')

	def genHorizontalLine(self):
		self.out.write('<hr/>')

	def genVerbatim(self, line):
		self.out.write(line)

	def genText(self, text):
		self.out.write(escape(text))

	def genParBegin(self):
		self.out.write('<p>\n')

	def genParEnd(self):
		self.out.write('</p>\n')

	def genList(self, list, attrs = ""):
		list_begin, item_begin, item_end, list_end = getList(list.kind)
		self.out.write((list_begin % attrs) + '\n')

		for item in list.getItems():
			self.out.write(item_begin)
			item.gen(self)
			self.out.write(item_end + '\n')

		self.out.write(list_end + '\n')

	def genDefList(self, deflist):
		self.out.write("<dl>\n")
		for item in deflist.getItems():
			self.out.write("<dt>")
			item.get_term().gen(self)
			self.out.write("</dt><dd>")
			item.get_def().gen(self)
			self.out.write("</dd>")
		self.out.write("</dl>\n")

	def genStyleBegin(self, kind):
		tag, _ = getStyle(kind)
		self.out.write(tag)

	def genStyleEnd(self, kind):
		_, tag = getStyle(kind)
		self.out.write(tag)

	def genHeaderTitle(self, header):
		"""Generate the title of a header."""
		number = self.refs[header][1]
		self.out.write('<h' + str(header.getLevel() + 1) + '>')
		self.out.write('<a name="' + number + '"></a>')
		self.out.write(number)
		header.genTitle(self)
		self.out.write('</h' + str(header.getLevel() + 1) + '>\n')

	def genHeader(self, header):
		"""Generate a whole header element (title + content)."""
		self.genHeaderTitle(header)
		header.genBody(self)
		return True

	def genLinkBegin(self, url):
		self.out.write('<a href="' + url + '">')

	def genLinkEnd(self, url):
		self.out.write('</a>')

	def genImage(self, url, caption = None, node = None):
		assert node
		align = node.getInfo(tdoc.INFO_ALIGN)
		if align:
			self.out.write("<div class=\"figure\">")
		if align == tdoc.ALIGN_CENTER:
			self.out.write('<center>')
		new_url = self.use_friend(url)
		self.out.write('<img src="' + new_url + '"')
		width = node.getInfo(tdoc.INFO_WIDTH)
		if width <> None:
			self.out.write(' width="' + str(width) + '"')
		height = node.getInfo(tdoc.INFO_WIDTH)
		if height <> None:
			self.out.write(' height="' + str(height) + '"')
		if caption <> None:
			self.out.write(' alt="' + cgi.escape(str(caption), True) + '"')
		if align == tdoc.ALIGN_RIGHT:
			self.out.write(' align="right"')
		elif align == tdoc.ALIGN_LEFT:
			self.out.write(' align="left"')
		self.out.write('/>')
		if node:
			self.genLabel(node)
		if align == tdoc.ALIGN_CENTER:
			self.out.write('</center>')
		if align:
			self.out.write("</div>")

	def genGlyph(self, code):
		self.out.write('&#' + str(code) + ';')

	def genLineBreak(self):
		self.out.write('<br/>')

	def genAuthors(self):
		"""Generate the text of the authors."""
		authors = common.scanAuthors(self.doc.getVar('AUTHORS'))
		first = True
		for author in authors:
			if first:
				first = False
			else:
				self.out.write(', ')
			email = ""
			if author.has_key('email'):
				email = author['email']
				self.out.write('<a href="mailto:' + cgi.escape(email) + '">')
			self.out.write(cgi.escape(author['name']))
			if email:
				self.out.write('</a>')		

	def genLabel(self, node):
		caption = node.getCaption()
		if caption or self.refs.has_key(node):
			self.out.write('<div class="label">')
			if self.refs.has_key(node):
				r = self.refs[node]
				self.out.write("<a name=\"%s\" class=\"label-ref\">%s</a>" % (r[1], self.trans.caption(node.numbering(), r[1])))
			if caption:
				for item in caption.getContent():
					item.gen(self)
			self.out.write('</div>')

	def genEmbeddedBegin(self, node):
		self.out.write('<div class="%s">' % node.numbering())
		self.genLabel(node)

	def genEmbeddedEnd(self, node):
		if self.label:
			self.genLabel(self.label)
		self.out.write('</div>')

	def genRef(self, ref):
		node = self.doc.getLabel(ref.label)
		if not node:
			common.onWarning("label\"%s\" cannot be resolved" % ref.label)
		else:
			r = self.refs[node]
			self.out.write("<a href=\"%s\">%s</a>" % (r[0], r[1]))
