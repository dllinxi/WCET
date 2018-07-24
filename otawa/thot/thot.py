#!/usr/bin/python
# thot -- Thot command
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

import doc
import os
import tparser
import locale
import sys
import optparse
import datetime
import os.path
import common

# Prepare environment
env = { } # os.environ.copy()
env["THOT_VERSION"] = "0.8"
env["ENCODING"] = locale.getpreferredencoding()
base = os.path.realpath(os.path.abspath(__file__))
env["THOT_BASE"] = os.path.dirname(base) + '/'
env["THOT_USE_PATH"] = env["THOT_BASE"] + "mods/"
env["THOT_DATE"] = str(datetime.datetime.today())

# Prepare arguments
oparser = optparse.OptionParser()
oparser.add_option("-t", "--type", action="store", dest="out_type",
	default="html", help="output type (xml, html, latex, ...)")
oparser.add_option("-o", "--out", action="store", dest="out_path",
	help="output path")
oparser.add_option("-D", "--define", action="append", dest="defines",
	help="add the given definition to the document environment.")
oparser.add_option("--dump", dest = "dump", action="store_true", default=False,
	help="only for debugging purpose, dump the database of Thot")
oparser.add_option("-u", "--use", action="append", dest="uses",
	help="given module is loaded before the generation.")
oparser.add_option("--verbose", "-v", dest = "verbose", action="store_true", default=False,
	help="display verbose messages about the processing")
oparser.add_option("--encoding", "-e", dest="encoding", action="store",
	type="string", help="select the encoding of the input files (default UTF-8)")

# Parse arguments
(options, args) = oparser.parse_args()
common.IS_VERBOSE = options.verbose
if options.encoding:
	common.ENCODING = options.encoding
env["THOT_OUT_TYPE"] = options.out_type
if not options.out_path:
	env["THOT_OUT_PATH"] = ""
else:
	env["THOT_OUT_PATH"] = options.out_path
if args == []:
	input = sys.__stdin__
	env["THOT_FILE"] = "<stdin>"
else:
	input = file(args[0])
	env["THOT_FILE"] = args[0]
if options.defines:
	for d in options.defines:
		p = d.find('=')
		if p == -1:
			common.onError('-D' + d + ' must follow syntax -Didentifier=value')
		else:
			env[d[:p]] = d[p+1:]

# open the output
document = doc.Document(env)
out_name = env["THOT_OUT_TYPE"]
out_path = os.path.join(document.env["THOT_BASE"], "backs")
out_driver = common.loadModule(out_name,  out_path)
if not out_driver:
	common.onError('cannot find %s back-end' % out_name)

# Parse the file
man = tparser.Manager(document)
if out_driver.__dict__.has_key("init"):
	out_driver.init(man)
if options.uses:
	for u in options.uses:
		man.use(u)
man.parse(input, env['THOT_FILE'])
if options.dump:
	document.dump("")

# Output the result
out_driver.output(document)

