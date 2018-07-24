# scorm -- Scorm support module
# Copyright (C) 2013  <hugues.casse@laposte.net>
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

import tparser
import common
import re

PREFIX = "@scorm:"

CONTROL_MODES = {
	"+choice" : 'choice="true"',
	"-choice" : 'choice="false"',
	"+choiceExit" : 'choiceExit="true"',
	"-choiceExit" : 'choiceExit="false"',
	"+flow": 'flow="true"',
	"-flow": 'flow="false"',
	"+forwardOnly": 'forwardOnly="true"',
	"-forwardOnly": 'forwardOnly="false"',
	"+useCurrentAttemptObjectiveInfo": 'useCurrentAttemptObjectiveInfo="true"',
	"-useCurrentAttemptObjectiveInfo": 'useCurrentAttemptObjectiveInfo="false"',
	"+useCurrentAttemptProgressInfo": 'useCurrentAttemptProgressInfo="true"',
	"-useCurrentAttemptProgressInfo": 'useCurrentAttemptProgressInfo="false"'
}


class Test:

	def __init__(self, obj, inv, attr, arg):
		self.obj = obj
		self.inv = inv
		self.attr = attr
		self.arg = arg
		
	def gen(self, indent, out):
		args = ""
		if self.obj:
			args = '%s referencedObjective="%s"' % (args, self.obj)
		if self.inv:
			args = '%s operator="not"' % args
		if self.arg:
			args = '%s measureThreshold="%s"' % (args, self.arg)
		out.write('%s\t\t<imsss:ruleCondition condition ="%s"%s/>\n' % (indent, self.attr, args))
		

class Condition:
	
	def __init__(self, type, any, tests, action):
		self.type = type
		self.any = any
		self.tests = tests
		self.action = action
	
	def gen(self, indent, out):
		out.write("%s<imsss:%sConditionRule>\n" % (indent, self.type))
		if self.any:
			arg = ' conditionCombination="any"'
		else:
			arg = ''
		out.write("%s\t<imsss:ruleConditions%s>\n" % (indent, arg))
		for test in self.tests:
			test.gen(indent, out)
		out.write("%s\t</imsss:ruleConditions>\n" % (indent))
		out.write('%s\t<imsss:ruleAction action="%s"/>\n' % (indent, self.action))
		out.write("%s</imsss:%sConditionRule>\n" % (indent, self.type))
	

def getHeader(man):
	last = None
	for item in man.iter():
		if item.getHeaderLevel() == 0:
			return item
		last = item
	return last

IF_RE = re.compile("\s*if(\s+(all|any))?\s(.*)\s+then\s(.*)")
TEST_RE = re.compile("(\s*(\S+)\s+is(\s+not)?\s+)?([^( \t]+)(\(([^)]+)\))?\s*")

TESTS = {
	"satisfied": False,
	"objectiveStatusKnown": False,
	"objectiveMeasureKnown": False,
	"objectiveMeasureGreaterThan": True,
	"objectiveMeasureLessThan": True,
	"completed": False,
	"activityProgressKnown": False,
	"attempted": False,
	"attemptLimitExceeded": False,
	"timeLimitExceeded": False,
	"outsideAvailableTimeRange": False,
	"always": False
}

ACTIONS = [
	"skip",
	"disabled",
	"hiddenFromChoice",
	"stopForwardTraversal",
	"exitParent",
	"exitAll",
	"retry",
	"retryAll",
	"continue",
	"previous",
	"exit"
]

def handleCondition(man, type, header, line):
	match = IF_RE.match(line)
	if not match:
		man.warn("'if' syntax error")
		return
	any = False
	if match.group(2) == 'any':
		any = True
	test = match.group(3)
	action = match.group(4)
	if action not in ACTIONS:
		man.warn("action '%s' unknown" % action)
		return
	tests = []
	for t in test.split(","):
		match = TEST_RE.match(t)
		if not match:
			man.warn("'test' syntax error")
			return
		obj = match.group(2)
		inv = False
		if match.group(3):
			inv = True
		attr = match.group(4)
		arg = match.group(6)
		try:
			has_args = TESTS[attr]
			if (has_args and not arg) or (not has_args and arg):
				man.warn("test '%s' bad argument" % attr)
				return
			tests.append(Test(obj, inv, attr, arg))
		except KeyError:
			man.warn("test '%s' unknown" % attr)
			return
	header.appendInfo("scorm:conds", Condition(type, any, tests, action))


LIMITS = [
	"attemptLimit",
	"attemptAbsoluteDurationLimit"
]
DEF_RE = re.compile("\s*(\S+)\s*=\s*(\S+)\s*")

def handleCommand(man, match):
	header = getHeader(man)
	args = match.group(1)
	if args.startswith("controlMode:"):
		attrs = []
		for arg in args[12:].split():
			try:
				attrs.append(CONTROL_MODES[arg])
			except KeyError:
				man.warn("bad controlMode %s" % arg)
		header.setInfo("scorm:controlMode", attrs)
	elif args.startswith("pre:"):
		handleCondition(man, "pre", header, args[4:])
	elif args.startswith("post:"):
		handleCondition(man, "post", header, args[4:])
	elif args.startswith("exit:"):
		handleCondition(man, "exit", header, args[4:])
	elif args.startswith("limit:"):
		limits = []
		for arg in args[6:].split(","):
			match = DEF_RE.match(arg)
			if not match or match.group(1) not in LIMITS:
				man.warn("bad limit value")
				return
			limits.append('%s="%s"' % (match.group(1), match.group(2)))
		header.setInfo("scorm:limits", limits)
			
	else:
		man.error("unknown SCORM command: %s" % args)
	

def init(man):
	man.addLine((handleCommand, re.compile(PREFIX + "(.*)")))

