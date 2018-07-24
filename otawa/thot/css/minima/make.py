#!/usr/bin/python
# make -- Minima theme maker
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
import ConfigParser
import re

# check parameters
name = sys.argv[1]
if not name:
	name = "initial"


# read the keys
config = ConfigParser.ConfigParser()
config.readfp(open('ini/%s.ini' % name))
reps = { }
for (key, val) in config.items('replacements'):
	reps[key] = val[1:-1]

#for (key, val) in reps.items():
#	print "%s=%s" % (key, val)

# open input file
input = open('style.css')

# open the output file
out = open('%s.css' % name, 'w')

# process lines
rep = re.compile('__([a-zA-Z0-9]*_)+_')
for line in input:
	while line <> '':
		m = rep.search(line)
		if not m:
			out.write(line)
			line = ''
		else:
			out.write(line[:m.start()])
			out.write(reps[m.group()])
			line = line[m.end():]
