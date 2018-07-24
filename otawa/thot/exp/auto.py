"""Automata for Thot. In the hope, I will be more powerful and easier to implement."""

import sys

class ErrorState:
	"""Error state."""
	trans = []
	eps = []
	any = None
	min = 1
	max = 0
	
	def __init__(self):
		self.any = self
	
	def add_trans(self, char, state):
		pass
	
	def add_eps(self, state):
		pass
	
	def set_any(self, state):
		pass
	
	def set_data(self, data):
		pass
	
	def get_data(self):
		return None

	def dump(self, out):
		out.write("err")
	
	def __repr__(self):
		return "err";


ESTATE = ErrorState()

def join_state(s1, s2):
	"""Join two states (ESTATE has less priority, s1 the biggest)."""
	if s1 == ESTATE:
		print "join(%s, %s) = %s" % (s1, s2, s2)
		return s2
	else:
		print "join(%s, %s) = %s" % (s1, s2, s1)
		return s1


def join_data(d1, d2):
	"""Join final state data."""
	if d1 == None:
		return d2
	else:
		return d1


def char_to_dot(char):
	"""Convert a character to displayable string."""
	code = ord(char)
	if 32 <= code and code < 128 and char not in ['"', '\'', '{', '}']:
		return char
	else:
		return "_%2X" % code


def to_stream(out):
	if type(out) == str:
		return open(out, "w")
	else:
		return out
		

def group_trans(trans):
	"""Try to group transition to the same state and to build ranges."""
	map = { }
	if not trans:
		return map
	for i in xrange(len(trans)):
		t = trans[i]
		if t <> ESTATE:
			try:
				set = map[t]
				(l, u) = set[-1]
				if i == u + 1:
					map[t] = set[:-1] + [(l, i)]
				else:
					map[t] = set + [(i, i)]
			except KeyError, e:
				map[t] = [(i, i)]
	return map


class State:
	min = None
	max = None
	trans = None
	any = None
	data = None
	id = None
	eps = None

	def __init__(self, data = None):
		self.data = data
		self.min = 1
		self.max = 0
		self.any = ESTATE
	
	def next(self, char):
		"""Provide state after transition with the given character.
		Return None if there is no transition."""
		char = ord(char)
		if self.min <= char and char <= self.max:
			res = self.trans[char - self.min]
			if res == ESTATE:
				return self.any
			else:
				return res
		else:
			return self.any
	
	def is_final(self):
		"""Test if the state is final."""
		return self.data <> None

	def get_data(self):
		"""Get the data associated with the state."""
		return self.data

	def set_data(self, data):
		"""Set data."""
		self.data = data

	def set_any(self, state):
		"""Set transition on any other character."""
		for i in xrange(self.max - self.min + 1):
			if self.trans[i] == self.any:
				self.trans[i] = state
		self.any = state				
	
	def add_eps(self, state):
		"""Add an epsilon transition."""
		if state == ESTATE:
			pass
		elif self.eps == None:
			self.eps = [state]
		elif state in self.eps:
			pass
		else:
			self.eps.append(state)

	def is_defined(self, char):
		"""Test if the given character is defined."""
		char = ord(char)
		return self.min <= char and char <= self.max and (self.trans[char - self.min] <> ESTATE or self.any <> ESTATE)

	def close(self, auto):
		"""Perform closure along epsilon transitions."""
		print "Closing S%d" % self.id
		while self.eps:
			s = self.eps.pop()
			print "\tpopping S%d" % s.id
			
			# join remaining epsilon list
			if s.eps:
				self.eps = self.eps + s.eps

			# join end data
			self.data = join_data(self.data, s.data)

			# process any other transition
			ts = None
			if self.any == s.any:
				pass
			elif self.any == ESTATE:
				self.any = s.any
			elif s.any == ESTATE:
				pass
			else:
				ts = auto.state()
				ts.add_eps(self.any)
				ts.add_eps(self.s)
				self.any = ts
			
			# allocate new transition array
			if not self.trans:
				if s.trans:
					self.trans = [] + s.trans
					self.min = s.min
					self.max = s.max
					print "\t\tcopying from S%d" % s.id
				else:
					print "\t\tno transitions"
				continue
			else:
				if not s.trans:
					continue
				else:
					print "\t\tS%d: [%d, %d]" % (self.id, self.min, self.max)
					self.trans = [self.any] * (max(0, self.min - s.min)) + self.trans + [self.any] * (max(0, s.max - self.max))
					self.min = min(self.min, s.min)
					self.max = max(self.max, s.max)
					print "\t\tjoin with S%d: [%d, %d]" % (s.id, s.min, s.max)
					print "\t\tmutated to [%d, %d]" % (self.min, self.max)
				
			# merge transition
			for i in xrange(s.max - s.min + 1):
				j = i + s.min - self.min
				assert j >= 0
				print "\t\ti=%d, j=%d" % (i, j)
				if self.trans[j] == s.trans[i]:
					pass
				elif self.trans[j] == ESTATE:
					self.trans[j] = s.trans[i]
				elif s.trans[i] == ESTATE:
					pass
				else:
					if not ts:
						ts = auto.state()
					ts.add_eps(self.trans[j])
					ts.add_eps(s.trans[i])
					self.trans[j] = ts
			

	def propagate(self):
		if not self.eps:
			return
		todo = [] + self.eps
		while todo:
			s = todo.pop()
			self.any = join_state(self.any, s.any)
			self.data = join_data(self.data, s.data)
			for i in xrange(len(s.trans)):
				char = i + s.min
				to = s.trans[i]
				if s.eps and s not in todo:
					todo.push(s)
				if self.min <= char and char <= self.max:
					self.trans[char - self.min] = join_state(self.trans[char - self.min], to)	
				else:
					to = join_state(self.any, to)
					if to <> self.any:
						self.add_trans(chr(char), to)
			
	
	def add_trans(self, char, state):
		"""Set transition on specific character."""
		char = ord(char)
		if self.min > self.max:
			self.trans = [state]
			self.max = char
			self.min = char
		elif char < self.min:
			if state <> self.any:
				self.trans = [state] + [self.any] * (self.min - char - 1) + self.trans
				self.min = char
		elif self.max < char:
			if state <> self.any:
				self.trans = self.trans + [self.any] * (char - self.max - 1) + [state]
				self.max = char
		else:
			self.trans[char - self.min] = state
	
	def add_range(self, lo, up, state):
		"""Set a range of transitions."""
		lo = ord(lo)
		up = ord(up)
		if self.min > self.max:
			self.trans = [None] * (up - lo + 1)
		else:
			if lo < min:
				self.trans = [self.any] * (self.min - lo) + self.trans
				self.min = lo
			if up > max:
				self.trans = self.trans + [self.any] * (up - self.max)
				self.max = up
		for i in xrange(up - lo + 1):
			self.trans[i] = state
	
	def dump(self, out = sys.stdout):
		if self.data:
			out.write("STATE %d (%s)\n" % (self.id, self.data))
		else:
			out.write("STATE %d\n" % self.id)
		groups = group_trans(self.trans)
		for k in groups.keys():
			out.write('\t');
			fst = True
			for (l, u) in groups[k]:
				if fst:
					fst = False
				else:
					out.write(", ")
				if l == u:
					out.write("%c" % (l + self.min))
				else:
					out.write("%c-%c" % (l + self.min, u + self.min))
			out.write(" to %s\n" % k)
	
	def dumpDot(self, out = sys.stdout):
		"""Dump to dot format (useful for verification)."""
		out.write("S%d" % self.id)
		if self.data:
			out.write(" [ shape=doublecircle ]")
		out.write(";\n");
		groups = group_trans(self.trans)
		for k in groups.keys():
			fst = True
			out.write("S%d -> S%d [label=\"" % (self.id, k.id));
			for (l, u) in groups[k]:
				if fst:
					fst = False
				else:
					out.write(", ")
				if l == u:
					out.write("%s" % char_to_dot(chr(l + self.min)))
				else:
					out.write("%s-%s" % (char_to_dot(chr(l + self.min)), char_to_dot(chr(u + self.min))))
			out.write("\"];\n" % k)
		if self.eps:
			for s in self.eps:
				out.write("S%d -> S%d [label=\"eps\"];\n" % (self.id, s.id))
	

	def __repr__(self):
		return "S%d" % self.id


class Automaton:
	"""Basically a set of states with an initial state."""
	init = None
	states = None
	name = None

	def __init__(self, name = "noname"):
		self.init = State()
		self.init.id = 0
		self.states = [self.init]
		self.name = name
	
	def get_init(self):
		"""Get initial state."""
		return self.init
	
	def state(self, data = None):
		"""Build a new state."""
		s = State(data)
		s.id = len(self.states)
		self.states.append(s)
		return s
	
	def scan(self, word):
		"""Scan a word and return data of the final state.
		None return means that the word does not match."""
		s = self.init
		for c in word:
			os = s
			s = s.next(c)
			#print "%s - %s -> %s" % (os, c, s)
			if s == ESTATE:
				return None
		return s.get_data()
	
	def minimize(self):
		self.states = []
		todo = [self.init]
		while todo:
			s = todo.pop()
			if s <> ESTATE and s not in self.states:
				s.id = len(self.states)
				self.states.append(s)
				todo.append(s.any)
				if s.trans:
					todo = todo + s.trans
							
	def close(self):
		"""Perform transitive closure of epsilon transitions."""
		for s in self.states:
			s.close(self)
		self.minimize()

	
	def propagate(self):
		"""Transitive propagation along epsilong transitions."""
		for s in self.states:
			s.eps = None
		self.minimize()

	def dump(self, out = sys.stdout):
		out = to_stream(out)
		for s in self.states:
			s.dump(out)

	def dumpDot(self, out = sys.stdout):
		out = to_stream(out)
		out.write("digraph %s {\n" % self.name);
		for s in self.states:
			s.dumpDot(out);
		out.write("}\n");
		

class RE:
	"""Basic class of regular expressions, i.e. . """

	def make(self, auto, entry, exit):
		"""Build automaton from RE, between given states."""
		pass
	
	def __add__(self, r):
		return alt(self, r)
	
	def __radd__(self, r):
		return alt(r, self)
	
	def __or__(self, r):
		return alt(self, r)
	
	def __ror__(self, r):
		return alt(r, self)
	
	def __mul__(self, r):
		return seq(self, r)
	
	def __rmul__(self, r):
		return re(r, self)
	
	def __xor__(self, r):
		return re(self, r)
	
	def __rxor__(self, r):
		return re(r, self)
	
	def __and__(self, r):
		return re(self, r)
	
	def __rand__(self, r):
		return re(r, self)


class Char(RE):
	"""Single character RE."""
	char = None
	
	def __init__(self, char):
		self.char = char
	
	def make(self, auto, entry, exit):
		entry.add_trans(self.char, exit)


class Concat(RE):
	"""Concatenation of RE, i.e. ?"""
	re1 = None
	re2 = None
	
	def __init__(self, re1, re2):
		assert re1
		assert re2
		self.re1 = re1
		self.re2 = re2
	
	def make(self, auto, entry, exit):
		step = auto.state()
		self.re1.make(auto, entry, step)
		self.re2.make(auto, step, exit)


class Option(RE):
	"""Optional RE."""
	re = None
	
	def __init__(self, re):
		self.re = re
	
	def make(self, auto, entry, exit):
		self.re.make(auto, entry, exit)
		entry.add_eps(exit)


class Alt(RE):
	"""Alternative RE, i.e. | """
	re1 = None
	re2 = None
	
	def __init__(self, re1, re2):
		self.re1 = re1
		self.re2 = re2
	
	def make(self, auto, entry, exit):
		s1 = auto.state()
		s2 = auto.state()
		entry.add_eps(s1)
		entry.add_eps(s2)
		self.re1.make(auto, s1, exit)
		self.re2.make(auto, s2, exit)


class Loop(RE):
	"""Loop RE, i.e. +."""
	re = None
	
	def __init__(self, re):
		self.re = re
	
	def make(self, auto, entry, exit):
		self.re.make(auto, entry, entry)
		entry.add_eps(exit)
	

def CharSet(RE):
	"""Range of character RE, i.e. [l-u]."""
	chars = None

	def __init__(self, chars):
		self.chars = chars

	def make(self, auto, entry, exit):
		for i in self.chars:
			if type(i) == str:
				for c in i:
					auto.add_trans(c, exit)
			else:
				lo, up = i
				for c in xrange(up - lo + 1):
					auto.add_trans(c, exit)


def ExceptRange(RE):
	"""All except the give range."""
	chars = None
	
	def __init__(self, chars):
		self.chars = chars

	def make(self, auto, entry, exit):
		entry.set_any(exit)
		for i in self.chars:
			if type(i) == str:
				for c in i:
					auto.add_trans(c, ESTATE)
			else:
				lo, up = i
				for c in xrange(up - lo + 1):
					auto.add_trans(c, ESTATE)


def to_re(a):
	assert a
	if type(a) == str:
		return re(*[Char(c) for c in a])
	else:
		return a
	
def re(*args):
	"""Build a sequence."""
	if args[-1] == None:
		args = args[:-1]
	r = to_re(args[0])
	args = args[1:]
	for a in args:
		r = Concat(r, re(a))
	assert r
	return r

def alt(*args):
	"""Compose alternatives."""
	r = to_re(args[0])
	for a in args[1:]:
		r = Alt(r, re(a))
	return r

def many(*args):
	"""Loop RE."""
	return Loop(re(*args))

def opt(*args):
	"""Optional RE."""
	return Option(re(*args))

def make(name = "noname", data = None, *args):
	"""Build the automaton for a RE."""
	r = re(*args)
	auto = Automaton(name)
	exit = auto.state()
	exit.set_data(data)
	r.make(auto, auto.get_init(), exit)
	#auto.propagate()
	auto.close()
	return auto


# unit tests
#auto = make("ok", "success", "abcg" |  loop(re("abcd") | "abef"))
#auto = make("ok", "success", "abcd")
#auto = make("ok", "success", "a", loop("b"), "c")
auto = make("ok", "success", "a" + "b" * many("b"))
#auto = make("ok", "success", "a" + "b" * opt("b"))
auto.dumpDot("auto.dot");
#auto.dump()
