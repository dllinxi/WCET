# ditaa -- Thot ditaa module
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


class DitaaBlock(extern.ExternalBlock):
	
	def __init__(self, meta):
		extern.ExternalBlock.__init__(self, meta)

	def prepare_input(self, gen, opts, input):
		tmp = self.dump_temporary(self.toText())
		opts.append(tmp);
		opts.append(self.get_path(gen))


def init(man):
	extern.ExternalModule(man, name = "ditaa",
	ext=".png",
	cmds=['ditaa'],
	maker = DitaaBlock, 
	options=[
		extern.SwitchOption("no-antilias", "-A"),
		extern.SwitchOption("no-separation", "-E"),
		extern.SwitchOption("round-corners", "-r"),
		extern.Option("scale", "-r"),
		extern.SwitchOption("no-shadows", "-S"),
		extern.Option("tabs", "-t")
	])
