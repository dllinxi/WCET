# highlight -- highlight call module
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

import subprocess
import sys
import doc
import os.path
import common

LANGS=[
  '4gl',
  'abp',
  'ada',
  'agda',
  'ampl',
  'amtrix',
  'applescript',
  'arm',
  'as',
  'asm',
  'asp',
  'aspect',
  'au3',
  'avenue',
  'awk',
  'bat',
  'bb',
  'bib',
  'bms',
  'c',
  'cb',
  'cfc',
  'clipper',
  'clp',
  'cob',
  'cs',
  'css',
  'd',
  'dot',
  'dylan',
  'e',
  'erl',
  'euphoria',
  'exp',
  'f77',
  'f90',
  'flx',
  'frink',
  'haskell',
  'hcl',
  'html',
  'httpd',
  'icn',
  'idl',
  'ini',
  'inp',
  'io',
  'j',
  'java',
  'js',
  'jsp',
  'lbn',
  'ldif',
  'lisp',
  'lotos',
  'ls',
  'lua',
  'm',
  'make',
  'mel',
  'mib',
  'ml',
  'mo',
  'mod3',
  'mpl',
  'ms',
  'mssql',
  'n',
  'nas',
  'nice',
  'nsi',
  'nut',
  'oberon',
  'objc',
  'octave',
  'os',
  'pas',
  'php',
  'pike',
  'pl',
  'pl1',
  'pov',
  'pro',
  'progress',
  'ps',
  'psl',
  'py',
  'pyx',
  'q',
  'qu',
  'r',
  'rb',
  'rexx',
  'rnc',
  's',
  'sas',
  'sc',
  'scala',
  'scilab',
  'sh',
  'sma',
  'sml',
  'snobol',
  'spec',
  'spn',
  'sql',
  'sybase',
  'tcl',
  'tcsh',
  'test_re',
  'tex',
  'ttcn3',
  'txt',
  'vb',
  'verilog',
  'vhd',
  'xml',
  'xpp',
  'y'
]

BACKS = {
	'html'	: '',
	'ansi'	: '--out-format=ansi',
	'latex'	: '--out-format=latex',
	'rtf'	: '--out-format=rtf',
	'tex'	: '--out-format=tex',
	'xhtml'	: '--out-format=xhtml'
}

DOCBOOK_LANGS = {
	'py': 'python',
	'c': 'c',
	'c++': 'c++'
}

CSS_BACKS = [ 'html', 'xhtml' ]

unsupported = []
unsupported_backs = []
checked = False
command = None

def getCommand():
	global command
	global checked
	if not checked:
		checked = True
		(id, release) = common.getLinuxDistrib()
		if id == "LinuxMint":
			command = "/usr/bin/highlight"
			common.onWarning("LinuxMint detected. Workaround to find 'highlight' command in /usr/bin/")
		else:
			command = common.which("highlight")
			if not command:
				common.onWarning("no highlight command found: code will not be colored.")
	return command
	

def genCode(gen, lang, text, type, line):
	"""Generate colorized code.
	gen -- back-end generator
	lang -- code language
	lines -- lines of the code"""
	global COMMAND
	
	type = gen.getType()
	if lang in LANGS and type in BACKS:
		command = getCommand()
		
		# default behaviour if no command
		if not command:
			if type == 'html':
				gen.genText(text)
			elif type == 'latex':
				gen.genVerbatim("\\begin{verbatim}\n")
				gen.genTex(text)
				gen.genVerbatim("\n\\end{verbatim}\n")
			return
		
		# other options
		opts = ""
		if line <> None:
			opts = opts + " -l"
			if line <> 1:
				opts = opts + " -m %s" % line
		
		# perform the command
		try:
			cfd = True
			if os.name == "nt":
				cfd = False
			process = subprocess.Popen(
				['%s -f --syntax=%s %s %s' % (command, lang, BACKS[type], opts)],
				stdin = subprocess.PIPE,
				stdout = subprocess.PIPE,
				close_fds = cfd,
				shell = True
			)
			res, _ = process.communicate(text)
			
			# generate the source
			gen.genVerbatim(res)
			
		except OSError, e:
			sys.stderr.write("ERROR: can not call 'highlight'\n")
			sys.exit(1)
	else:
		if lang and lang not in LANGS and lang not in unsupported:
			sys.stderr.write('WARNING: ' + lang + ' unsupported highglight language\n')
			unsupported.append(lang)
		if gen.getType() not in BACKS and gen.getType() not in unsupported_backs:
			sys.stderr.write('WARNING: ' + gen.getType() + ' unsupported highlight back-end\n')
			unsupported_baks.append(lang)
		if type == 'latex':
			gen.genVerbatim('\\begin{verbatim}\n')
		gen.genText(text)
		if type == 'latex':
			gen.genVerbatim('\\end{verbatim}\n\n')


class Feature(doc.Feature):

	def prepare(self, gen):
		type = gen.getType()
		command = getCommand()
		if not command:
			return
			
		# parse list of languages
		try:
			global LANGS
			ans = subprocess.check_output("%s -p" % command, shell = True)
			LANGS = []
			for line in ans.split("\n"):
				try:
					p = line.index(":")
					if p >= 0:
						line = line[p+1:]
						for w in line.split():
							if w != '(' and w != ')':
								LANGS.append(w)
				except ValueError, e:
					pass
		except subprocess.CalledProcessError,e :
			common.onWarning("cannot get supported languages from %s, falling back to default list." % command)
		
		# build the CSS file
		if type in CSS_BACKS:
			try:
				css = gen.new_friend('highlight/highlight.css')
				cfd = True
				if os.name == "nt":
					cfd = False
				process = subprocess.Popen(
					['%s -f --syntax=c --style-outfile=%s' % (command, css)],
					stdin = subprocess.PIPE,
					stdout = subprocess.PIPE,
					close_fds = cfd,
					shell = True
				)
				_ = process.communicate("")
			except OSError, e:
				sys.stderr.write("ERROR: can not call 'highlight'\n")
				sys.exit(1)

			# add the file to the style
			styles = gen.doc.getVar('HTML_STYLES')
			if styles:
				styles += ':'
			styles += css
			gen.doc.setVar('HTML_STYLES', styles)

		# build .sty
		if type == 'latex':
			try:
				css = gen.new_friend('highlight/highlight.sty')
				process = subprocess.Popen(
					['%s -f --syntax=c --style-outfile=%s %s' % (command, css, BACKS[type])],
					stdin = subprocess.PIPE,
					stdout = subprocess.PIPE,
					close_fds = True,
					shell = True
				)
				_ = process.communicate("")
			except OSError, e:
				sys.stderr.write("ERROR: can not call 'highlight'\n")
				sys.exit(1)

			# build the preamble
			preamble = gen.doc.getVar('LATEX_PREAMBLE')
			preamble += '\\usepackage{color}\n'
			preamble += '\\usepackage{alltt}\n'
			preamble += '\\input {%s}\n' % gen.getFriendRelativePath(css)
			gen.doc.setVar('LATEX_PREAMBLE', preamble)


FEATURE = Feature()

class CodeBlock(doc.Block):
	lang = None

	def __init__(self, man, lang, line = None):
		doc.Block.__init__(self, "code")
		self.lang = lang
		self.line_number = line
		man.doc.addFeature(FEATURE)

	def dumpHead(self, tab):
		print tab + "code(" + self.lang + ","

	def gen(self, gen):

		# aggregate code
		text = ""
		for line in self.content:
			if text <> "":
				text += '\n'
			text += line

		# generate the code
		type = gen.getType()
		if type == 'html':
			gen.genEmbeddedBegin(self)
			gen.genVerbatim('<pre class="code">\n')
			genCode(gen, self.lang, text, type, self.line_number)
			gen.genVerbatim('</pre>')
			gen.genEmbeddedEnd(self)
		elif type == 'latex':
			gen.genEmbeddedBegin(self)
			genCode(gen, self.lang, text, type, self.line_number)
			gen.genEmbeddedEnd(self)
		elif type == 'docbook':
			gen.genVerbatim('<programlisting xml:space="preserve" ')
			if DOCBOOK_LANGS.has_key(self.lang):
				gen.genVerbatim(' language="%s"' % DOCBOOK_LANGS[self.lang])
			gen.genVerbatim('>\n')
			gen.genText(self.toText())
			gen.genVerbatim('</programlisting>\n')
		else:
			common.onWarning('backend %s unsupported for code block' % type)

	def kind(self):
		return "listing"

	def numbering(self):
		return "listing"
