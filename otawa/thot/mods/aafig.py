# aafig -- Thot aafig module
# Copyright (C) 2015  <hugues.casse@laposte.net>
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

"""A block containing aafig diagram.
See https://launchpad.net/aafigure for more details."""

import extern

class AafigBlock(extern.ExternalBlock):
	
	def __init__(self, meta):
		extern.ExternalBlock.__init__(self, meta)

	def prepare_input(self, gen, opts, input):
		opts.append("-o '%s'" % self.get_path(gen))
		input.append(self.toText())
		

def init(man):
	extern.ExternalModule(man,
		name = "aafig",
		ext=".png",
		cmds=['aafigure'],
		maker = AafigBlock,
		options=[
			extern.SwitchOption("wide-chars", "-w"),
			extern.SwitchOption("textual", "-T"),
			extern.Option("scale", "-s"),
			extern.Option("aspect", "-a"),
			extern.Option("linewidth", "-l"),
			extern.SwitchOption("proportional", "--proportional", True),
			extern.Option("foreground", "-f"),
			extern.Option("fill", "-x"),
			extern.Option("background", "-n"),
			extern.Option("option", "-O")
		])
