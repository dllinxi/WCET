#!/usr/bin/python

class Set:
	
	def __init__(self, name, s = ""):
		self.name = name
		self.words = [0, 0, 0, 0]
		self.set(s)
	
	def set(self, s):
		for c in s:
			n = ord(c)
			self.words[n / 64] |= 1 << (n % 64)

	def dump(self):
		print "\tstatic t::uint64 %s[] = { %s };" % \
			(self.name, ", ".join("0x%08x" % self.words[i] for i in xrange(0, 4)))


def crange(c1, c2):
	s = ""
	for c in xrange(ord(c1), ord(c2)+1):
		s = s + chr(c)
	return s

sets = [
	Set("bin_set", "01"),
	Set("dec_set", crange('0', '9')),
	Set("hex_set", crange('0', '9') + crange('a', 'f') + crange('A', 'F')),
	Set("space_set", " \n\t\r\v"),
	Set("letter_set", crange('a', 'z') + crange('A', 'Z')),
	Set("lower_set", crange('a', 'z')),
	Set("upper_set", crange('A', 'Z'))
]

for s in sets:
	s.dump()

