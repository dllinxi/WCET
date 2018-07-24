# latex -- Thot latex back-end
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
import codecs
import unicodedata
import subprocess
import os.path
import doc
import sys


# USED VARIABLES
#	TITLE: document title
#	AUTHORS: document authors
#	LANG: document language
#	OUTPUT: one of 'latex' or 'pdf'
#	ENCODING: character encoding
#	SUBTITLE: sub-title of the document
#	ORGANIZATION: organization producing the document
#	LOGO: logos of the supporting organizations
#	LATEX_CLASS: latex class for document
#	LATEX_PREAMBLE: insert just after document definition
#	LATEX_PAPER: latex paper format (a4paper, letter, etc)

KOMA_STYLES = [
	"scrartcl",
	"scrreprt",
	"scrbook",
	"scrlttr2"
]

ESCAPES = {
	'&'	: '\\&',
	'$'	: '\\$',
	'%'	: '\\%',
	'#'	: '\\#',
	'_'	: '\\_',
	'{'	: '\\{',
	'}'	: '\\}',
	'~'	: '$\sim$',
	'\\': '$\\backslash$',
	'\U21D0'	: '$\Leftarrow$'
}

UNICODE_ESCAPES = {
	0x2013: '--',
	0x2014: '---',
	0x22ef: '\ldots',
	0x21D0: '$\Leftarrow$'
}

LANGUAGES = {
	'en': 'english',
	'en_au': 'english',
	'en_bw': 'english',
	'en_ca': 'english',
	'en_dk': 'english',
	'en_gb': 'english',
	'en_hk': 'english',
	'en_ie': 'english',
	'en_in': 'english',
	'en_ng': 'english',
	'en_nz': 'english',
	'en_ph': 'english',
	'en_sg': 'english',
	'en_us': 'english',
	'en_za': 'english',
	'en_zw': 'english',
	'fr': 'frenchb',
	'fr_ca': 'frenchb',
	'fr_fr': 'frenchb',
	'fr_be': 'frencb',
	'fr_ch': 'frenchb',
	'fr_lu': 'frenchb'
}

SECTIONS = {
	0: '\\chapter{',
	1: '\\section{',
	2: '\\subsection{',
	3: '\\subsubsection{',
	4: '\\paragraph{titre',
	5: '\\subparagraph{',
}

STYLES = {
	'bold': '\\textbf{',
	'italic': '\\textit{',
	'underline': '\\underline{',
	'subscript': '\\subscript{',
	'superscript': '\\superscript{',
	'monospace': '\\texttt{',
	'deleted': '{'
}

UNSUPPORTED_IMAGE = [ '.gif' ]

ALIGNMENT = ['l', 'c', 'r']

REF_PREFIXES = {
	"header": "sec",
	"figure": "fig",
	"table": "tab",
	"listing": "lst"
}

class UnicodeEncoder:
	"""Abstract for unicode character encoding."""

	def toText(self, code):
		"""Transform the given code to text."""
		pass


class UTF8Encoder(UnicodeEncoder):
	"""Unicode support by UTF8 encoding."""
	encoder = None

	def __init__(self):
		self.encoder = codecs.getencoder('UTF-8')

	def toText(self, code):
		"""Transform the given code to text."""
		str, _ = self.encoder(unichr(code))
		return str


class NonUnicodeEncoder(UnicodeEncoder):
	"""Encoder for non-unicode character encoding."""
	encoding = None
	encoder = None
	escaped = None

	def __init__(self, encoding):
		self.encoding = encoding
		self.encoder = codecs.getencoder(encoding)
		self.escaped = []

	def toText(self, code):
		"""Transform the given code to text."""
		try:
			str, _ = self.encoder(unichr(code))
			return str
		except UnicodeEncodeError,e:
			if UNICODE_ESCAPES.has_key(code):
				return UNICODE_ESCAPES[code]
			if not (code in self.escaped):
				self.escaped.append(code)
				common.onWarning('encoding %s cannot support character %x' % (self.encoding, code))
			return unicodedata.name(unichr(code))


class Generator(back.Generator):
	encoding = None
	encoder = UnicodeEncoder()
	first = False
	multi = False

	def __init__(self, doc):
		back.Generator.__init__(self, doc)

	def escape(self, text):
		res = ""
		for c in text:
			if ESCAPES.has_key(c):
				res += ESCAPES[c]
			else:
				res += c
		return res

	def unsupported(self, feature):
		common.onError('%s unsupported for Latex back-end')

	def getType(self):
		return 'latex'

	def run(self):
		self.openMain('.tex')
		self.doc.pregen(self)

		# get class
		cls = self.doc.getVar('LATEX_CLASS')
		if not cls:
			cls = 'book'
		preamble = self.doc.getVar('LATEX_PREAMBLE')

		# look for internationalization
		lang = self.doc.getVar('LANG').lower().replace('-', '_')
		if lang:
			if LANGUAGES.has_key(lang):
				lang = LANGUAGES[lang]
			else:
				pos = lang.find('_')
				if pos < 0:
					common.onError('cannot not support "%s"' % lang)
				else:
					lang = lang[:pos]
					if LANGUAGES.has_key(lang):
						lang = LANGUAGES[lang]
					else:
						common.onError('cannot not support "%s"' % lang)

		# look for encoding
		self.encoding = self.doc.getVar('ENCODING').lower().replace('-', '_')
		if self.encoding:
			if self.encoding == 'utf_8':
				preamble += '\\usepackage{ucs}\n'
				preamble += '\\usepackage[utf8x]{inputenc}\n'
				self.encoder = UTF8Encoder()
			elif self.encoding == 'iso_8859_1':
				preamble += '\\usepackage[latin1]{inputenc}\n'
				self.encoder = NonUnicodeEncoder(self.encoding)
			else:
				common.onWarning('%s encoding is just ignored for latex' % self.encoding)

		# look paper size
		paper = self.doc.getVar('LATEX_PAPER')
		if not paper:
			paper = 'a4paper'

		# preamble
		self.out.write('\\documentclass[oneside,%s,%s]{%s}\n' % (paper, lang, cls))
		self.out.write('\\usepackage[T1]{fontenc}\n')
		self.out.write('\\usepackage[pdftex]{graphicx}\n')
		self.out.write('\\usepackage{hyperref}\n')
		self.out.write('\\usepackage{verbatim}\n')
		#self.out.write('\\usepackage{fancyhdr}\n')
		self.out.write(preamble)
		if lang:
			self.out.write('\\usepackage[%s]{babel}\n' % lang)
		is_koma = cls in KOMA_STYLES

		# add custom definitions
		self.out.write('\\newcommand{\\superscript}[1]{\\ensuremath{^{\\textrm{#1}}}}\n')
		self.out.write('\\newcommand{\\subscript}[1]{\\ensuremath{_{\\textrm{#1}}}}\n')
		self.out.write('\\headheight=20pt\n')
		self.out.write('\\headsep=10pt\n')

		self.out.write('\\begin{document}\n')

		# write title
		latex_title = self.doc.getVar("LATEX_TITLE")
		if not latex_title:
			self.out.write('\\title{%s}\n' % self.escape(self.doc.getVar('TITLE')))
			subtitle = self.doc.getVar("SUBTITLE")
			if is_koma and subtitle:
				self.out.write("\\subtitle{%s}\n" % self.escape(subtitle))
			# NOTE: \thanks{...} allows to give the owner organization of an author
			self.out.write('\\author{%s}\n' % self.escape(self.doc.getVar('AUTHORS')).replace(",", " \\and "))
			organization = self.doc.getVar("ORGANIZATION")
			if is_koma and organization:
				self.out.write("\\publishers{%s}\n" % self.escape(organization))
			self.out.write('\\maketitle\n\n')
		else:
			
			self.out.write("\\begin{titlepage}\n")
			self.out.write("\\newcommand{\\thotorganization}{%s}\n" % self.escape(self.doc.getVar("ORGANIZATION")))
			self.out.write("\\newcommand{\\thottitle}{%s}\n" % self.escape(self.doc.getVar("TITLE")))
			self.out.write("\\newcommand{\\thotsubtitle}{%s}\n" % self.escape(self.doc.getVar("SUBTITLE")))
			self.out.write("\\newcommand{\\thotauthors}{%s}\n" % ("{" + self.escape(self.doc.getVar("AUTHORS")).replace(",", "} {") + "}"))
			logos = self.doc.getVar("LOGO")
			text = ""
			fst = True
			for logo in logos.split(","):
				if not fst:
					text = text + " \\hfill ";
				else:
					fst = False
				text = text + "\includegraphics{%s}" % logo.strip()
			self.out.write("\\newcommand{\\thotlogos}{%s}\n" % text)
			file = open(latex_title)
			for l in file:
				self.out.write(l)
			self.out.write("\\end{titlepage}\n")			
				
		# generate the content
		self.out.write('\\tableofcontents\n\n')
		self.out.write('\\pagebreak\n\n')

		# write body
		self.doc.gen(self)

		# write footer
		self.out.write('\\end{document}\n')
		self.out.close()

		# generate final format
		output = self.doc.getVar('OUTPUT')
		if not output or output == 'latex':
			print "SUCCESS: result in %s" % self.path
		elif output == 'pdf':

			# perform compilation
			for i in xrange(0, 2):	# two times for TOC (sorry)
				dir, file = os.path.split(self.path)
				cmd = 'pdflatex -halt-on-error %s' % file
				if dir == "":
					dir = "."
				process = subprocess.Popen(
					cmd,
					shell = True,
					stdout = subprocess.PIPE,
					stderr = subprocess.PIPE,
					cwd = dir
				)
				out, err = process.communicate('')
				if process.returncode <> 0:
					sys.stdout.write(out)
					sys.stderr.write(err)
					return

			# display result
			file, ext = os.path.splitext(self.path)
			if ext == ".tex":
				path = file
			else:
				path = self.path
			path = path + ".pdf"
			print "SUCCESS: result in %s" % path
		else:
			common.onError('unknown output: %s' % output)

	def genFootNote(self, note):
		self.out.write('\\footnote{')
		for item in note:
			item.gen(self)
		self.out.write('}')

	def genQuoteBegin(self, level):
		# Latex does not seem to support multi-quote...
		self.out.write('\\begin{quote}\n')

	def genQuoteEnd(self, level):
		self.out.write('\\end{quote}\n')

	def genTable(self, table):
		floatting = self.doc.getLabelFor(table) or table.getCaption()

		# output prolog
		if floatting:
			self.out.write("\\begin{table}[htbp]\n")
		self.out.write('\\vspace{4pt}\n')
		self.out.write('\\begin{tabular}{|')
		for i in xrange(0, table.getWidth()):
			self.out.write('c|')
		self.out.write('}\n')

		# compute vertical and horizontal lines
		hlines = []
		vlines = []
		rows = table.getRows()
		for i in xrange(0, len(rows)):
			row = rows[i]
			if i < len(rows) - 1 and row.kind <> rows[i + 1].kind:
				hlines += [i]
			if row.kind <> doc.TAB_HEADER:
				cells = row.getCells()
				pos = 0
				for i in xrange(0, len(cells) - 1):
					if pos not in vlines and cells[i].kind <> cells[i + 1].kind:
						vlines += [pos]
						pos += cells[i].span

		# generate the content
		for irow in xrange(0, len(rows)):
			self.out.write('\\hline\n')
			row = rows[irow]
			icol = 0
			for cell in row.getCells():

				multi = False
				if icol == 0:
					lbar = '|'
				else:
					self.out.write(' & ')
					lbar = ''
				if icol in vlines:
					rbar = '|'
					multi = True
				else:
					rbar = ''
				if cell.align <> doc.TAB_CENTER or cell.span <> 1:
					multi = True
				if multi:
					self.out.write('\\multicolumn{%d}{%s%s|%s}{' % (cell.span, lbar, ALIGNMENT[cell.align + 1], rbar))
				icol += cell.span
				if cell.kind == doc.TAB_HEADER:
					self.out.write('\\bf{')

				cell.gen(self)

				if cell.kind == doc.TAB_HEADER:
					self.out.write('}')
				if multi:
					self.out.write('}')

			self.out.write('\\\\\n')
			if irow in hlines:
				self.out.write('\\hline\n')

		# epilog
		self.out.write('\\hline\n')
		self.out.write('\\end{tabular}\n')
		self.out.write('\\vspace{4pt}\n')
		if floatting:
			self.genLabel(table)
			self.out.write("\end{table}\n")
		self.out.write("\n")

	def genHorizontalLine(self):
		self.out.write('\n\\vspace{4pt}\n')
		self.out.write('\\hrule\n')
		self.out.write('\\vspace{4pt}\n\n')

	def genVerbatim(self, line):
		self.out.write(line)

	def genText(self, text):
		self.out.write(self.escape(text))

	def genParBegin(self):
		pass

	def genParEnd(self):
		self.out.write('\n\n')

	def genList(self, list):
		if list.kind == 'ul':
			self.out.write('\\begin{itemize}\n')
		elif list.kind == 'ol':
			self.out.write('\\begin{enumerate}\n')
		else:
			self.unsupported('%s list' % list.kind)

		for item in list.getItems():
			self.out.write('\\item ')
			item.gen(self)

		if list.kind == 'ul':
			self.out.write('\\end{itemize}\n')
		else:
			self.out.write('\\end{enumerate}\n')

	def genDefList(self, deflist):
		self.out.write("\\begin{itemize}\n")
		for item in deflist.getItems():
			self.out.write("\item[")
			for text in item.get_term().getContent():
				text.gen(self)
			self.out.write("] ")
			item.get_def().gen(self)
		self.out.write("\\end{itemize}\n")


	def genStyleBegin(self, kind):
		if not kind in STYLES:
			self.unsupported('%s style' % kind)
		self.out.write(STYLES[kind])

	def genStyleEnd(self, kind):
		self.out.write('}')

	def genHeader(self, header):
		self.out.write(SECTIONS[header.level])
		header.genTitle(self)
		self.out.write('}\n')
		self.genLabel(header)
		header.genBody(self)
		return True

	def genHeaderEnd(self, level):
		pass

	def genLinkBegin(self, url):
		self.out.write('\href{%s}{' % self.escape(url))

	def genLinkEnd(self, url):
		self.out.write('}')

	def genImage(self, url, width = None, height = None, caption = None, align = None, node = None):
		# !!TODO!!
		# It should download the image if the URL is external

		# handle unsupported image format
		root, ext = os.path.splitext(url)
		if ext.lower() not in UNSUPPORTED_IMAGE:
			link = self.loadFriendFile(url)
		else:
			original = self.relocateFriendFile(url)
			root, ext = os.path.splitext(original)
			link = self.addFriendFile(os.path.abspath(root + ".png"))
			res = subprocess.call(['convert %s %s' % (original, link)], shell = True)
			if res <> 0:
				common.onError('cannot convert image "%s" to "%s"' % (original, link))
			link = self.getFriendRelativePath(link)

		# build the command
		args = ''
		if width:
			if args:
				args += ','
			args += 'width=%dpx' % width
		if height:
			if args:
				args += ','
			args += 'height=%dpx' % height
		if args:
			args = "[%s]" % args
		if align <> doc.ALIGN_NONE:
			self.out.write("\\begin{figure}[htbp]\n")
		self.out.write('\includegraphics%s{%s}' % (args, link))
		if align <> doc.ALIGN_NONE:
			self.genLabel(node)
			self.out.write("\n\\end{figure}\n")

	def genLabel(self, node):
		label = None
		caption = None
		if node:
			label = self.doc.getLabelFor(node)
			caption = node.getCaption()
		if label:
			pref = node.numbering()
			if REF_PREFIXES.has_key(pref):
				pref = REF_PREFIXES[pref]
			self.out.write("\\label{%s:%s}\n" % (pref, label))
		if caption:
			self.out.write("\\caption{")
			for item in caption.getContent():
				item.gen(self)
			self.out.write("}")

	def genEmbeddedBegin(self, node):
		floatting = node.getCaption() or self.doc.getLabelFor(self)
		if floatting:
			self.out.write("\\begin{figure}[htbp]\n")

	def genEmbeddedEnd(self, node):
		floatting = node.getCaption() or self.doc.getLabelFor(self)
		if floatting:
			self.genLabel(node)
			self.out.write("\\end{figure}\n")

	def genGlyph(self, code):
		if UNICODE_ESCAPES.has_key(code):
			self.out.write(UNICODE_ESCAPES[code])
		else:
			self.out.write(self.encoder.toText(code))

	def genLineBreak(self):
		self.out.write(' \\\\ ')

	def genRef(self, ref):
		node = self.doc.getLabel(ref.label)
		pref = node.numbering()
		if REF_PREFIXES.has_key(pref):
			pref = REF_PREFIXES[pref]
		self.out.write("\\ref{%s:%s} " % (pref, ref.label))


def output(doc):
	gen = Generator(doc)
	gen.run()
