# html -- Thot dot module
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
import doc
import subprocess
import sys

mimetex = common.CommandRequirement("mimetex", 'mimetex not found but required by latexmath module: ignoring latexmath tags')

count = 0
formulae = { }

class LatexMath(doc.Word):
	
	def __init__(self, text):
		doc.Word.__init__(self, text)
	
	def dump(self, tab = ""):
		print "%slatexmath(%s)" % (tab, self.text)
	
	def gen(self, gen):
		global mimetex
		global count
		global formulae
		
		if gen.getType() == "latex":
			gen.genVerbatim("$%s$" % self.text)
		else:
			cmd = mimetex.get()
			if not cmd:
				return
			rpath = ''
			if formulae.has_key(self.text):
				rpath = formulae[self.text]
			else:
				rpath = gen.new_friend("latexmath/latexmath-%s.gif" % count);
				count += 1
				try:
					proc = subprocess.Popen(
						["%s -d '%s' -e %s" % (cmd, self.text, rpath)],
						stdout = subprocess.PIPE,
						stderr = subprocess.PIPE,
						shell = True
					)
					out, err = proc.communicate()
					if proc.returncode <> 0:
						sys.stderr.write(out)
						sys.stderr.write(err)
						self.onWarning("bad latexmath formula.")
					else:
						formulae[self.text] = rpath
				except OSError, e:
					MIMETEX_AVAILABLE = False
					self.onWarning("mimetex is not available: no latexmath !")
			if rpath:
				gen.genImage(rpath, None, self)


def handleMath(man, match):
	text = match.group("latexmath")
	if text == "":
		man.send(doc.ObjectEvent(doc.L_WORD, doc.ID_NEW, doc.Word("$")))
	else:
		man.send(doc.ObjectEvent(doc.L_WORD, doc.ID_NEW, LatexMath(text)))

MATH_WORD = (handleMath, "\$(?P<latexmath>[^$]*)\$")

def init(man):
	man.addWord(MATH_WORD)
