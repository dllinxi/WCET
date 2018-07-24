# Thot slidy back-end
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

""" This back-end generates HTML file usable for a presentation
based on the [[https://www.w3.org/Talks/Tools/Slidy2|Slidy]] styles and
scripts. The following variables are used:

* TITLE - document title,
* SUBTITLE - sub-title for the title page,
* AUTHORS - author list,
* ORGANIZATION - organization,
* STYLE - Slidy style to use (default style slidy),
* COPYRIGHT - document copyright.
* COVER_IMAGE - picture used on the cover page.
* DURATION - duration of the presentation to display a time count.
* ORG_LOGO - logo of organization,
* DOC_LOGO -- logo of the document.
* OUTLINE -- none to not generate outline pages, top for outline only 
  with top-level entries (default).
"""

import backs.abstract_html
import common
import doc
import i18n
import os
import os.path
import re
import shutil
import types

import sys
import traceback

class Templater:

	def __init__(self, env):
		self.env = env
	
	def gen(self, temp, out):
		temp = open(temp, "r")
		for line in temp.readlines():
			while line:
				p = line.find("@(")

				# no variable: dump line
				if p < 0:
					out.write(line)
					break
					
				# write the prefix
				out.write(line[:p])
				line = line[p+2:]
					
				# find closing variable
				q = line.find(")")
				if q < 0:
					continue
				id = line[:q]
				line = line[q+1:]

				# process identifier
				try:
					v = self.env[id]
					if hasattr(v, "__call__"):
						v(self.env, out)
					else:
						out.write(backs.abstract_html.escape(v))
				except KeyError:
					pass
		temp.close()


class Marker(doc.Node):
	"""New slide marker."""

	def __init__(self, type):
		doc.Node.__init__(self)
		self.type = type

	def dump(self, tab):
		print "%smaker(%s)" % (tab, self.type)


class Generator(backs.abstract_html.Generator):
	
	def __init__(self, doc):
		backs.abstract_html.Generator.__init__(self, doc)
		self.inc = False
		self.trans = i18n.getTranslator(doc)
		self.outline = doc.getVar("OUTLINE", "top")
		self.top_headers = None
		self.last = None
		self.css = None
		self.template = None
		self.rel_css = False

	def get_top_headers(self):
		if not self.top_headers:
			self.top_headers = [n for n in self.doc.content if n.getHeaderLevel() == 0]
		return self.top_headers

	def gen_outline(self, cur):
		if self.outline == "none":
			return
		if cur.getHeaderLevel() > 0:
			return
		
		self.out.write('\n<div class="slide outline">\n')
		self.out.write('\t<h1>%s</h1>\n' % self.trans.get(i18n.ID_OUTLINE))
		self.out.write('\t<ul>\n')
		
		cls = "header-done"
		for h in self.get_top_headers():
			if h == self.last:
				cls = "header-last"
			elif h == cur:
				cls = "header-current"
			self.out.write('\t\t<li class="%s">' % cls)
			h.genTitle(self)
			self.out.write('</li>\n')
			if h == cur:
				cls = "header-todo"
		self.last = cur
		
		self.out.write('\t</ul>\n')
		self.out.write('</div>\n')

	def run(self):

		try:
		
			# prepare the files to process
			(base, css) = self.get_css()
			tpath = self.get_template()
			tool_base = os.path.join(self.doc.getVar("THOT_BASE"), "slidy")
			tool_css = os.path.join("styles", "slidy.css")
	
			# add CSS
			self.openMain(".html")
			tool_rel_css = self.importCSS(tool_css, tool_base)
			if css:
				self.rel_css = self.importCSS(css, base)
			
			# add scripts
			ipath = self.getImportDir()
			spath = os.path.join(tool_base, "scripts")
			path = os.path.join(ipath, "scripts")
			if not os.path.exists(path):
				shutil.copytree(spath, path)
			
			# write the output
			env = dict(self.doc.env)
			env["IMPORTS"] = ipath
			env["IF_IMPORTED_STYLE"] = self.gen_imported_style
			env["SLIDES"] = self.gen_slides
			env["IF_COVER_IMAGE"] = self.gen_cover_image
			env["IF_DURATION"] = self.gen_duration
			env["IF_DOC_LOGO"] = self.gen_doc_logo
			env["IF_ORG_LOGO"] = self.gen_org_logo
			templater = Templater(env)
			templater.gen(tpath, self.out)

		except IOError as e:
			common.onError("error during generation: %s" % e)

	def gen_imported_style(self, env, out):
		if self.rel_css:
			out.write('<link rel="stylesheet" href="%s" type="text/css" />\n' % self.rel_css)

	def gen_doc_logo(self, env, out):
		try:
			path = self.use_friend(env["DOC_LOGO"])
			out.write('<img id="head-icon" alt="document logo" src="%s"/>' % path) 
		except KeyError:
			pass
	
	def gen_org_logo(self, env, out):
		try:
			path = self.use_friend(env["ORG_LOGO"])
			out.write('<img src="%s" alt="W3C logo" id="head-logo-fallback" />' % path) 
		except KeyError:
			pass
	
	def gen_cover_image(self, env, out):
		try:
			path = self.use_friend(env["COVER_IMAGE"])
			out.write('<img src="%s"  alt="cover picture" class="cover"/><br clear="all" />' % path)
		except KeyError:
			pass
	
	def gen_duration(self, env, out):
		if env.has_key("DURATION"):
			out.write('<meta name="duration" content="%s"/>' % env["DURATION"])
	
	def gen_slides(self, env, out):
		stack = []
		started = False
		header = self.trans.get(i18n.ID_INTRODUCTION)
		i = iter(self.doc.content)
		inc = False
				
		while True:
			try:
				node = i.next()
				
				# marker processing
				if isinstance(node, Marker):
					if node.type == "slice":
						if started:
							self.out.write("</div>\n")
						self.out.write("\n<div class=\"slide\">\n")
						self.genHeaderTitle(header)
						started = True
					elif node.type == "inc":
						self.out.write('<div class="incremental">')
						self.inc = True
					elif node.type == "non-inc":
						self.out.write('</div>')
						self.inc = False
				
				# header processing
				if node.getHeaderLevel() >= 0:
					self.gen_outline(node)
					stack.append((node, i))
					if started:
						if self.inc:
							self.out.write("</div>\n")
						self.out.write("</div>\n")
					header = node
					i = iter(node.content)
					started = False
				
				# other paragraph processing
				else:
					if not started:
						self.out.write("\n<div class=\"slide\">\n")
						self.genHeaderTitle(header)
						started = True
					node.gen(self)

			except StopIteration:
				if started:
					if self.inc:
						self.out.write("</div>\n")
						self.inc = False
					self.out.write("</div>\n")
				started = False
				if not stack:
					break
				header, i = stack.pop()
		
	def genHeaderTitle(self, header):
		"""Generate the title of a header."""
		self.out.write("<h1>")
		self.out.write('<a name="%s"></a>' % id(header))
		header.genTitle(self)
		self.out.write('</h1>\n')

	def get_slide_template(self, name):
		"""Find a HTML template file for a particular slide type."""
		(base, css) = self.get_css()
		path = os.path.join(base, "%s.html" % name)
		if os.path.exists(path):
			return path
		else:
			return None

	def get_template(self):
		"""Get the HTML template to generate presentation.
		It is derived from the CSS."""
		
		# already computed?
		if self.template:
			return self.template
		(base, css) = self.get_css()
		
		# look in base / template.html
		if base:
			path = os.path.join(base, "template.html")
			if os.path.exists(path):
				self.template = path
		
		# else fallback to default blank
		if not self.template:
			self.template = os.path.join(self.doc.getVar("THOT_BASE"), "slidy", "blank.html")
		
		# return result
		return self.template
	

	def get_css(self):
		"""Get the CSS style from the current configuration.
		Return (CSS base, CSS path) or None"""

		# CSS already computed?
		if self.css:
			return self.css

		# get STYLE
		style = self.doc.getVar("STYLE")
		if not style:
			return (None, None)
		
		# absolute path
		if os.path.isabs(style):
			if os.path.isdir(style):
				self.css = (style, "style.css")
			else:
				self.css = (os.path.dirname(style), os.path.basename(style))
		
		# in the current directory
		elif os.path.exists(style):
			if os.path.isdir(style):
				self.css = (style, "style.css")
			else:
				self.css = (os.path.dirname(style), os.path.basename(style))

		# look in the THOT directory
		else:
			base = os.path.join(self.doc.getVar("THOT_BASE"), "slidy")
			path = os.path.join(base, style)
			if os.path.isdir(path):
				self.css = (path, "style.css")
			else:
				path = os.path.join(base, "styles", style + ".css")
				if os.path.exists(path):
					self.css = (base, os.path.join("styles", style + ".css"))
				else:
					common.onError("cannot find style '%s'" % style)
		
		# return result
		return self.css

	def genList(self, list, attrs = ""):
		if self.inc:
			attrs = attrs + ' class="incremental"'
		backs.abstract_html.Generator.genList(self, list, attrs)


def handle_slide(man, match):
	man.send(doc.ObjectEvent(doc.L_PAR, doc.ID_NEW, Marker("slice")))

def handle_inc(man, match):
	man.send(doc.ObjectEvent(doc.L_PAR, doc.ID_NEW, Marker("inc")))

def handle_not_inc(man, match):
	man.send(doc.ObjectEvent(doc.L_PAR, doc.ID_NEW, Marker("non-inc")))

def output(doc):
	gen = Generator(doc)
	gen.run()

def init(man):
	man.addLine((handle_slide, re.compile("<slide>")))
	man.addLine((handle_inc, re.compile("<inc>")))
	man.addLine((handle_not_inc, re.compile("<non-inc>")))
	
