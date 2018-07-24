# html -- Thot html back-end
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
import backs.abstract_html


# supported variables
#	TITLE: title of the document
#	AUTHORS: authors of the document
#	LANG: lang of the document
#	ENCODING: charset for the document
#	THOT_OUT_PATH:	HTML out file
#	THOT_FILE: used to derivate the THOT_OUT_PATH if not set
#	HTML_STYLES: CSS styles to use (':' separated)
#	HTML_SHORT_ICON: short icon for HTML file
#	HTML_ONE_FILE_PER: one of document (default), chapter, section.
#	HTML_TEMPLATE:  template used to generate pages.

def makeRef(nums):
	"""Generate a reference from an header number array."""
	return ".".join([str(i) for i in nums])

class PageHandler:
	"""Provide support for generating pages."""

	def gen_header(self, gen):
		"""Called to generate header part of HTML file."""
		pass

	def gen_title(self, gen):
		"""Called to generate the title."""
		pass
	
	def gen_authors(self, gen):
		"""Called to generate list of authors."""
		pass
		
	def gen_menu(self, gen):
		"""Called to generate the menu."""
		pass
	
	def gen_content(self, gen):
		"""Called to generate the content."""
		pass


class Page:
	"""Abstract class for page generation."""

	def apply(self, handler, gen):
		"""Called to generate a page."""
		pass


class PlainPage(Page):
	"""Simple plain page."""
	
	def apply(self, handle, gen):
		out = gen.out
		
		# output header
		out.write('<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">\n')
		out.write('<html>\n')
		out.write('<head>\n')
		out.write("	<title>")
		handle.gen_title(gen)
		out.write("</title>\n")
		out.write('	<meta name="AUTHOR" content="' + cgi.escape(gen.doc.getVar('AUTHORS'), True) + '">\n')
		out.write('	<meta name="GENERATOR" content="Thot - HTML">\n');
		out.write('	<meta http-equiv="Content-Type" content="text/html; charset=' + cgi.escape(gen.doc.getVar('ENCODING'), True) + '">\n')
		handle.gen_header(gen)
		out.write('</head>\n<body>\n<div class="main">\n')
		
		# output the title
		out.write('<div class="header">\n')
		out.write('	<div class="title">')
		handle.gen_title(gen)
		out.write('</div>\n')
		out.write('	<div class="authors">')
		handle.gen_authors(gen)
		out.write('</div>\n')
		out.write('</div>')
		
		# output the menu
		handle.gen_menu(gen)
		
		# output the content
		out.write('<div class="page">\n')
		handle.gen_content(gen)
		gen.genFootNotes()
		out.write('</div>\n')		

		# output the footer
		out.write("</div>\n</body>\n</html>\n")

template_re = re.compile("<thot:([^/]+)\/>")

class TemplatePage(Page):
	"""Page supporting template in HTML. The template may contain
	the following special elements:
	* <thot:title> -- document title,
	* <thot:authors> -- list of authors,
	* <thot:menu> -- table of content of the document,
	* <thot:content> -- content of the document.
	"""
	path = None
	
	def __init__(self, path):
		self.path = path
	
	def apply(self, handler, gen):
		map = {
			"authors": handler.gen_authors,
			"content": handler.gen_content,
			"header":  handler.gen_header,
			"title":   handler.gen_title,
			"toc":    handler.gen_menu
		}
		global template_re

		try:
			tpl = open(self.path, "r")
			n = 0
			for line in tpl.xreadlines():
				n = n + 1
				f = 0
				for m in template_re.finditer(line):
					gen.out.write(line[f:m.start()])
					f = m.end()
					try:
						kw = m.group(1)
						map[kw](gen)
					except KeyError, e:
						common.onError("unknown element %s at %d" % (kw, n))					
				gen.out.write(line[f:])
			
		except IOError, e:
			common.onError(str(e))

	
class PagePolicy:
	"""A page policy allows to organize the generated document
	according the preferences of the user."""
	gen = None
	page = None

	def __init__(self, gen, page):
		self.gen = gen
		self.page = page

	def onHeaderBegin(self, header):
		pass

	def onHeaderEnd(self, header):
		pass

	def unfolds(self, header):
		return True

	def ref(self, header, number):
		return	"#" + number

	def gen_header(self, gen):
		out = gen.out
		styles = gen.doc.getVar("HTML_STYLES")
		if styles:
			for style in styles.split(':'):
				new_style = gen.importCSS(style)
				out.write('	<link rel="stylesheet" type="text/css" href="' + new_style + '">\n')
		short_icon = gen.doc.getVar('HTML_SHORT_ICON')
		if short_icon:
			out.write('<link rel="shortcut icon" href="%s"/>' % short_icon)

	
class AllInOne(PagePolicy):
	"""Simple page policy doing nothing: only one page."""

	def __init__(self, gen, page):
		PagePolicy.__init__(self, gen, page)

	def genRefs(self):
		"""Generate and return the references for the given generator."""
		self.gen.refs = { }
		self.makeRefs([1], { }, self.gen.doc)

	def makeRefs(self, nums, others, node):
		"""Traverse the document tree and generate references in the given map."""
		
		# number for header
		num = node.numbering()
		if num == 'header':
			r = makeRef(nums)
			self.gen.refs[node] = ("#%s" % r, r)
			nums.append(1)
			for item in node.getContent():
				self.makeRefs(nums, others, item)
			nums.pop()
			nums[-1] = nums[-1] + 1
		
		# number for embedded
		else:
			
			# set number
			if self.gen.doc.getLabelFor(node):
				if num:
					if not others.has_key(num):
						others[num] = 1
						n = 1
					else:
						n = others[num] + 1
					self.gen.refs[node] = ("#%s-%d" % (num, n), str(n))
					others[num] = n
		
			# look in children
			for item in node.getContent():
				self.makeRefs(nums, others, item)
	
	def gen_title(self, gen):
		gen.genTitleText()
	
	def gen_authors(self, gen):
		gen.genAuthors()

	def gen_menu(self, gen):
		self.gen.genContent([], 100)
		
	def gen_content(self, gen):
		self.gen.genBody()		

	def run(self):
		self.gen.openMain('.html')
		self.genRefs()
		self.gen.doc.pregen(self.gen)
		self.page.apply(self, self.gen)


class PerSection(PagePolicy):
	"""This page policy ensures there is one page per section."""

	def __init__(self, gen, page):
		PagePolicy.__init__(self, gen, page)

	def genRefs(self):
		"""Generate and return the references for the given generator."""
		self.gen.refs = { }
		self.makeRefs([1], { }, self.gen.doc, 0)

	def makeRefs(self, nums, others, node, page):
		"""Traverse the document tree and generate references in the given map."""
		my_page = page
		
		# number for header
		num = node.numbering()
		if num == 'header':
			page = page + 1
			self.gen.refs[node] = ("%s-%d.html" % (self.gen.root, my_page), ".".join([str(n) for n in nums]))
			nums.append(1)
			for item in node.getContent():
				page = self.makeRefs(nums, others, item, page)
			nums.pop()
			nums[-1] = nums[-1] + 1
		
		# number for embedded
		else:
			
			# set number
			if num and self.gen.doc.getLabelFor(node):
				if not others.has_key(num):
					others[num] = 1
					n = 1
				else:
					n = others[num] + 1
				r = str(n)
				self.gen.refs[node] = ("%s-%s#%s-%s" % (self.gen.root, my_page, page, num, r), r)
				others[num] = n
		
			# look in children
			for item in node.getContent():
				page = self.makeRefs(nums, others, item, page)

		return page

	def process(self, header):

		# generate the page
		self.gen.openPage(header)
		self.path = self.path + [header]
		self.page.apply(self, self.gen)
		print "generated %s" % (self.gen.getPage(header))
		
		# generate the su-headers
		for child in header.getContent():
			if child.getHeaderLevel() >= 0:
				self.process(child)
		self.path.pop()
		
	path = []
	
	def gen_title(self, gen):
		gen.genTitleText()
	
	def gen_authors(self, gen):
		gen.genAuthors()

	def gen_menu(self, gen):
		if not self.path:
			gen.genContent([], 0)
		else:
			gen.genContent(self.path, 100)
		
	def gen_content(self, gen):
		if not self.path:		
			for node in self.gen.doc.getContent():
				if node.getHeaderLevel() <> 0:
					node.gen(gen)
		else:
			for h in self.path:
				gen.genHeaderTitle(h)
			for child in self.path[-1].getContent():
				if child.getHeaderLevel() < 0:
					child.gen(gen)

	def run(self):
		
		# preparation
		self.gen.openMain('.html')
		self.gen.doc.pregen(self.gen)
		self.genRefs()

		# collect chapters
		#chapters = []
		#for node in self.gen.doc.getContent():
		#	if node.getHeaderLevel() == 0:
		#		chapters.append(node)
		
		# generate page
		self.page.apply(self, self.gen)
		print "generated %s" % self.gen.path

		# generate chapter pages
		for node in self.gen.doc.getContent():
			if node.getHeaderLevel() == 0:
				self.process(node)


class PerChapter(PagePolicy):
	"""This page policy ensures there is one page per chapter."""

	def __init__(self, gen, page):
		PagePolicy.__init__(self, gen, page)

	def genRefs(self):
		"""Generate and return the references for the given generator."""
		self.gen.refs = { }
		self.makeRefs([1], { }, self.gen.doc, '')

	def makeRefs(self, nums, others, node, page):
		"""Traverse the document tree and generate references in the given map."""
		
		# number for header
		num = node.numbering()
		if num == 'header':
			if node.header_level == 0:
				page = "%s-%d.html" % (self.gen.root, nums[0] - 1)
				self.gen.refs[node] = ("%s" % page, str(nums[0]))
			else:
				r = makeRef(nums)
				self.gen.refs[node] = ("%s#%s" % (page, r), r)
			nums.append(1)
			for item in node.getContent():
				self.makeRefs(nums, others, item, page)
			nums.pop()
			nums[-1] = nums[-1] + 1
		
		# number for embedded
		else:
			
			# set number
			if num and self.gen.doc.getLabelFor(node):
				if not others.has_key(num):
					others[num] = 1
					n = 1
				else:
					n = others[num] + 1
				r = str(n)
				self.gen.refs[node] = ("%s#%s-%s" % (page, num, r), r)
				others[num] = n
		
			# look in children
			for item in node.getContent():
				self.makeRefs(nums, others, item, page)

	node = None
	
	def gen_title(self, gen):
		gen.genTitleText()
	
	def gen_authors(self, gen):
		gen.genAuthors()

	def gen_menu(self, gen):
		if not self.node:
			self.gen.genContent([], 0)
		else:
			self.gen.genContent([self.node], 100)
		
	def gen_content(self, gen):
		if not self.node:		
			pass
		else:
			self.node.gen(self.gen)

	def run(self):
		chapters = []

		# generate main page
		self.gen.openMain('.html')
		self.genRefs()
		self.gen.doc.pregen(self.gen)
		for node in self.gen.doc.getContent():
			if node.getHeaderLevel() == 0:
				chapters.append(node)
		self.page.apply(self, self.gen)
		print "generated %s" % self.gen.path

		# generate chapter pages
		for node in chapters:
			self.gen.openPage(node)
			self.node = node
			self.page.apply(self, self.gen)
			self.gen.closePage()
			print "generated %s" % (self.gen.getPage(node))


class Generator(backs.abstract_html.Generator):
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
		backs.abstract_html.Generator.__init__(self, doc)
		self.footnotes = []
		self.pages = { }
		self.pages = { }
		self.page_count = 0
		self.stack = []
		self.refs = { }

	def getType(self):
		return 'html'

	def genDocHeader(self):
		self.out.write('<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">\n')
		self.out.write('<html>\n')
		self.out.write('<head>\n')
		self.out.write("	<title>" + cgi.escape(self.doc.getVar('TITLE')) + "</title>\n")
		self.out.write('	<meta name="AUTHOR" content="' + cgi.escape(self.doc.getVar('AUTHORS'), True) + '">\n')
		self.out.write('	<meta name="GENERATOR" content="Thot - HTML">\n');
		self.out.write('	<meta http-equiv="Content-Type" content="text/html; charset=' + cgi.escape(self.doc.getVar('ENCODING'), True) + '">\n')
		styles = self.doc.getVar("HTML_STYLES")
		if styles:
			for style in styles.split(':'):
				new_style = self.importCSS(style)
				self.out.write('	<link rel="stylesheet" type="text/css" href="' + new_style + '">\n')
		short_icon = self.doc.getVar('HTML_SHORT_ICON')
		if short_icon:
			self.out.write('<link rel="shortcut icon" href="%s"/>' % short_icon)
		self.out.write('</head>\n<body>\n<div class="main">\n')

	def genTitleText(self):
		"""Generate the text of the title."""
		self.out.write(cgi.escape(self.doc.getVar('TITLE')))
	
	def genTitle(self):
		self.out.write('<div class="header">\n')
		self.out.write('	<div class="title">')
		self.genTitleText()
		self.out.write('</div>\n')
		self.out.write('	<div class="authors">')
		self.genAuthors()
		self.out.write('</div>\n')
		self.out.write('</div>')

	def genBodyHeader(self):
		self.out.write('<div class="page">\n')

	def genBodyFooter(self):
		self.out.write('</div>\n')

	def genBody(self):
		self.genBodyHeader()
		self.doc.gen(self)
		self.genFootNotes()
		self.genBodyFooter()

	def genFooter(self):
		self.out.write("</div>\n</body>\n</html>\n")

	def genContentEntry(self, node, indent):
		"""Generate a content entry (including numbering, title and link)."""
		self.out.write('%s<a href="%s">' % (indent, self.refs[node][0]))
		self.out.write(self.refs[node][1])
		self.out.write(' ')
		node.genTitle(self)
		self.out.write('</a>\n')

	def expandContent(self, node, level, indent):
		"""Expand recursively the content and to the given level."""
		if node.getHeaderLevel() >= level:
			return
		one = False
		for child in node.getContent():
			if child.getHeaderLevel() >= 0:
				if not one:
					one = True
					self.out.write('%s<ul class="toc">\n' % indent)
				self.out.write("%s<li>\n" % indent)
				self.genContentEntry(child, indent)
				self.expandContent(child, level, indent + "  ")
				self.out.write("%s</li>\n" % indent)
		if one:
			self.out.write('%s</ul>\n' % indent)

	def expandContentTo(self, node, path, level, indent):
		"""Expand, not recursively, the content until reaching the end of the path.
		From this, expand recursively the sub-nodes."""
		if not path:
			self.expandContent(node, level, indent)
		else:
			one = False
			for child in node.getContent():
				if child.getHeaderLevel() >= 0:
					if not one:
						one = True
						self.out.write('%s<ul class="toc">\n' % indent)
					self.out.write("%s<li>\n" % indent)
					self.genContentEntry(child, indent)
					if path[0] == child:
						self.expandContentTo(child, path[1:], level, indent + '  ')
					self.out.write("%s</li>\n" % indent)
			if one:
				self.out.write('%s</ul>\n' % indent)
				
	def genContent(self, path, level):
		"""Generate the content without expanding until ending the path
		(of headers) with an expanding maximum level.
		"""
		self.out.write('<div class="toc">\n')
		self.out.write('<h1><a name="toc">' + cgi.escape(self.trans.get(i18n.ID_CONTENT)) + '</name></h1>\n')
		self.expandContentTo(self.doc, path, level, '  ')
		self.out.write('</div>\n')

	def getPage(self, header):
		if not self.pages.has_key(header):
			self.pages[header] = "%s-%d.html" % (self.root, self.page_count)
			self.page_count += 1
		return self.pages[header]

	def openPage(self, header):
		path = self.getPage(header)
		self.stack.append((self.out, self.footnotes))
		self.out = open(path, 'w')
		self.footnotes = []

	def closePage(self):
		self.out.close()
		self.out, self.footnotes = self.stack.pop()

	def run(self):

		# select the page
		self.template = self.doc.getVar('HTML_TEMPLATE')
		if self.template:
			page = TemplatePage(self.template)
		else:
			page = PlainPage()

		# select the policy
		self.struct = self.doc.getVar('HTML_ONE_FILE_PER')
		if self.struct == 'document' or self.struct == '':
			policy = AllInOne(self, page)
		elif self.struct == 'chapter':
			policy = PerChapter(self, page)
		elif self.struct == 'section':
			policy = PerSection(self, page)
		else:
			common.onError('one_file_per %s structure is not supported' % self.struct)

		# generate the document
		policy.run()
		print "SUCCESS: result in %s" % self.path


def output(doc):
	gen = Generator(doc)
	gen.run()
