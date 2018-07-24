# doc -- Thot document escription module
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

import re
import common

# levels
L_DOC=0
L_HEAD=1
L_PAR=2
L_WORD=3
LEVELS = ["DOC", "HEAD", "PAR", "WORD"]

# identifiers
ID_NEW = "new"				# ObjectEvent for L_HEAD
ID_END = "end"

ID_TITLE = "title"

ID_NEW_ITEM = "new_item"	# ItemEvent
ID_END_ITEM = "end_item"
ID_NEW_DEF  = "new_def"		# definition event
ID_END_TERM = "end term"
ID_END_DEF  = "end_def"

ID_NEW_TAB = "new_tab"
ID_END_TAB = "end_tab"
ID_NEW_ROW = "new_row"
ID_END_ROW = "end_row"
ID_NEW_CELL = "new_cell"
ID_END_CELL = "end_cell"

ID_NEW_STYLE = "new_style"	# TypedEvent
ID_END_STYLE = "end_style"	# TypedEvent

ID_NEW_LINK = "new_link"	# ObjectEvent
ID_END_LINK = "end_link"

ID_NEW_QUOTE = "new_quote"
ID_END_QUOTE = "end_quote"

ID_NUM_HEADER = "header"
ID_NUM_TABLE = "table"
ID_NUM_FIGURE = "figure"
ID_NUM_LISTING = "listing"

ID_EMBED_RAW = "raw"
ID_EMBED_FILE = "file"
ID_EMBED_CODE = "code"
ID_EMBED_DOT = "dot"
ID_EMBED_GNUPLOT = "gnuplot"


# variable reduction
VAR_RE = "@\((?P<varid>[a-zA-Z_0-9]+)\)"
VAR_REC = re.compile(VAR_RE)


# alignment
ALIGN_NONE = 0
ALIGN_LEFT = 1
ALIGN_CENTER = 2	
ALIGN_RIGHT = 3
ALIGN_JUSTIFY = 4
ALIGN_TOP = 1
ALIGN_BOTTOM = 3


# standard styles
STYLE_BOLD = "bold"
STYLE_STRONG = "string"
STYLE_ITALIC = "italic"
STYLE_EMPHASIZED = "emphasized"
STYLE_MONOSPACE = "monospace"
STYLE_UNDERLINE = "underline"
STYLE_SUBSCRIPT = "subscript"
STYLE_SUPERSCRIPT = "superscript"
STYLE_STRIKE = "strike"
STYLE_BIGGER = "bigger"
STYLE_SMALLER = "smaller"
STYLE_CITE = "cite"
STYLE_CODE = "code"
STYLE_FOOTNOTE = "footnote"

# standard footnote
FOOTNOTE_EMBED = "embed"
FOOTNOTE_REF = "ref"
FOOTNOTE_DEF = "def"

# standard lists
LIST_ITEM = "ul"
LIST_NUMBER = "ol"


# external optional information
#	Back-end are free or not the information attributes.
INFO_CLASS = "thot:class"				# string (class in CSS)
INFO_CSS = "thot:css"					# string (CSS code)
INFO_ALIGN = "thot:align"				# one of ALIGN_LEFT, ALIGN_CENTER or ALIGN_RIGHT
INFO_VALIGN = "thot:valign"				# one of ALIGN_TOP, ALIGN_CENTER or ALIGN_BOTTOM
INFO_ID = "thot:id"						# string (id in XML format)
INFO_LANG = "thot:lang"					# ISO 639-1 language code
INFO_LEFT_PAD = "thot:left_pad"			# real (number of 1 em)
INFO_RIGHT_PAD = "thot:right_pad"		# real (number of 1 em)
INFO_HSPAN = "thot:hspan"				# integer (number of cells)
INFO_VSPAN = "thot:vspan"				# integer (number of cells)
INFO_PERCENT_SIZE = "thot:percent_size"	# real (percent)
INFO_WIDTH = "thot:width"				# integer (pixels)
INFO_HEIGHT = "thot:height"				# integer (pixels)


# supported events
class Event:
	"""Base class of all events."""
	level = None
	id = None

	def __init__(self, level, id):
		"""Build a new event with level and id."""
		self.level = level
		self.id = id

	def make(self):
		"""Make an object matching the event."""
		return None

	def __str__(self):
		return "event(" + LEVELS[self.level] + ", " + self.id + ")"


class ObjectEvent(Event):
	"""Event carrying an object to make."""
	object = None

	def __init__(self, level, id, object):
		Event.__init__(self, level, id)
		self.object = object

	def make(self):
		return self.object
	
	def __str__(self):
		return "event(%s, %s, %s)" % (LEVELS[self.level], self.id, self.object)


class TypedEvent(Event):
	"""Event containing a type."""
	type = None

	def __init__(self, level, id, type):
		Event.__init__(self, level, id)
		self.type = type


class StyleEvent(TypedEvent):
	"""Event for a simple style."""

	def __init__(self, type):
		TypedEvent.__init__(self, L_WORD, ID_NEW_STYLE, type)

	def make(self):
		return Style(self.type)


class OpenStyleEvent(TypedEvent):
	"""Event for an open/close style."""

	def __init__(self, type):
		TypedEvent.__init__(self, L_WORD, ID_NEW_STYLE, type)

	def make(self):
		return OpenStyle(self.type)


class CloseEvent(TypedEvent):
	"""An event for a closing tag."""

	def __init__(self, level, id, type):
		TypedEvent.__init__(self, level, id, type)

	def make(self):
		raise common.ParseException(self.type + " closed but not opened !")


class CloseStyleEvent(CloseEvent):
	"""Event for an open/close style."""

	def __init__(self, type):
		CloseEvent.__init__(self, L_WORD, ID_END_STYLE, type)


class ItemEvent(TypedEvent):
	"""Event for list item."""
	depth = None

	def __init__(self, type, depth):
		TypedEvent.__init__(self, L_PAR, ID_NEW_ITEM, type)
		self.depth = depth

	def make(self):
		return List(self.type, self.depth)


class DefEvent(Event):
	"""Event for list definition."""

	def __init__(self, id = ID_NEW_DEF, depth = -1):
		Event.__init__(self, L_PAR, id)
		self.depth = depth
	
	def make(self):
		return DefList(self.depth)


class QuoteEvent(Event):
	"""An event designing a quoted text."""
	depth = None

	def __init__(self, depth):
		Event.__init__(self, L_PAR, ID_NEW_QUOTE)
		self.depth = depth

	def make(self):
		return Quote(self.depth)


class Info:
	info = None

	def setInfo(self, id, val):
		"""Set an information value."""
		if not self.info:
			self.info = { }
		self.info[id] = val
	
	def appendInfo(self, id, val):
		"""Append the given value to identifier with identifier processed as a list."""
		if not self.info:
			return None
		try:
			self.info[id].append(val)
		except KeyError:
			self.info[id] = [ val ]
			
	def getInfo(self, id, dflt = None):
		"""Get an information value. None if it not defined."""
		if not self.info:
			return None
		try:
			return self.info[id]
		except KeyError:
			return dflt

	def mergeInfo(self, info):
		"""Merge the given information with the current one."""
		if info.info:
			for k in info.info.keys():
				self.setInfo(k, info.info[k])


# nodes
class Node(Info):
	"""Base definition of document nodes."""
	file = None
	line = None
	
	def __init__(self):
		pass

	def setFileLine(self, file, line):
		if self.file == None:
			self.file = file
			self.line = line

	def onError(self, msg):
		"""Called to display an error."""
		common.onError('%s:%d: %s' % (self.file, self.line, msg))

	def onWarning(self, msg):
		"""Called to display a warning."""
		common.onWarning('%s:%d: %s' % (self.file, self.line, msg))

	def onInfo(self, msg):
		"""Called to display an information to the user."""
		common.onInfo('%s:%d: %s' % (self.file, self.line, msg))

	def onEvent(self, man, event):
		"""Called each time a new word is found.
		man -- current parser manager
		event -- current event."""
		man.forward(event)

	def isEmpty(self):
		"""Test if the node is empty (and can be removed)."""
		return False

	def dump(self, tab = ""):
		"""Provided a textual output of the node."""
		pass

	def clean(self):
		"""Cleanup the node: remove all nodes an dsub-nodes not required."""
		pass

	def getHeaderLevel(self):
		"""For a heade node, get the level."""
		return -1

	def genTitle(self, gen):
		"""generate the title for a header node."""
		return None

	def getContent(self):
		"""Get the sub-nodes of the current nodes."""
		return []

	def gen(self, gen):
		"""Method to perform document generation.
		gen -- used generator."""
		pass

	def setCaption(self, text):
		"""Method called when a caption is found after the current element.
		Return True if the caption is accepted. """
		return False
		
	def getCaption(self):
		"""Return the associated caption. return none if there is no caption."""
		return None

	def acceptLabel(self):
		"""Method called called when a label is found. Nodes not supporting
		just return False (default behaviour) else node return True."""
		return False
	
	def visit(self, visitor):
		"""Visit the current node, that is, call the method in
		visitor that matches the node."""
		pass

	def numbering(self):
		"""Return the group of numbering the node belongs to.
		Return None if there is no numbering (default).
		"header" for header numbering, "figure" pour image and the like,
		"listing" for code, etc."""
		return None

	def toText(self):
		"""Produce only the raw text of the node."""
		return ""


class Container(Node):
	"""A container is an item containing other items."""
	content = None

	def __init__(self):
		Node.__init__(self)
		self.content = []

	def add(self, man, item):
		if item:
			self.content.append(item)
			man.push(item)

	def last(self):
		return self.content[-1]

	def isEmpty(self):
		return self.content == []

	def clean(self):
		toremove = []
		for item in self.content:
			item.clean()
			if item.isEmpty():
				toremove.append(item)
		for item in toremove:
			self.content.remove(item)

	def dumpHead(self, tab):
		pass

	def dump(self, tab):
		self.dumpHead(tab)
		for item in self.content:
			item.dump(tab + "  ")
		print tab + ")"

	def getContent(self):
		return self.content

	def gen(self, gen):
		for item in self.content:
			item.gen(gen)

	def toText(self):
		r = ""
		for item in self.content:
			r = r + item.toText()
		return r
	

# Word family
class Word(Node):
	text = None

	def __init__(self, text):
		Node.__init__(self)
		self.text = text

	def dump(self, tab):
		print(tab + self)

	def gen(self, gen):
		gen.genText(self.text)
	
	def __str__(self):
		return "word(%s)" % self.text

	def visit(self, visitor):
		visitor.onWord(self)

	def toText(self):
		return self.text

class Ref(Node):
	label = None
	
	def __init__(self, label):
		Node.__init__(self)
		self.label = label
	
	def dump(self, tab):
		print("%sref(%s)" % (tab, self.label))

	def gen(self, gen):
		gen.genRef(self)
	
	def __str__(self):
		return "ref(%s)" % self.label

	def visit(self, visitor):
		visitor.onRef(self)
	

class Image(Node):
	path = None
	caption = None

	def __init__(self, path, width = None, height = None, caption = None):
		Node.__init__(self)
		self.path = path
		self.caption = caption
		if width:
			self.setInfo(INFO_WIDTH, width)
		if height:
			self.setInfo(INFO_HEIGHT, height)

	def dump(self, tab):
		print "%simage(%s, %s, %s, %s)" % \
			(tab, self.path, str(self.width), str(self.height), self.caption)

	def gen(self, gen):
		gen.genImage(self.path, node = self, caption = self.caption)

	def visit(self, visitor):
		visitor.onImage(self)


class EmbeddedImage(Node):
	path = None
	caption = None

	def __init__(self, path, width = None, height = None, caption = None, align = ALIGN_NONE):
		Node.__init__(self)
		self.path = path
		if caption:
			self.caption = Par()
			self.caption.content.append(Word(caption))
		if width:
			self.setInfo(INFO_WIDTH, width)
		if height:
			self.setInfo(INFO_HEIGHT, height)
		if align:
			self.setInfo(INFO_ALIGN, align)

	def dump(self, tab):
		print "%sembedded-image(%s, %s)" % \
			(tab, self.path, self.caption)

	def gen(self, gen):
		gen.genImage(self.path, caption = self.caption, node = self)

	def visit(self, visitor):
		visitor.onEmbeddedImage(self)

	def numbering(self):
		return "figure"

	def getCaption(self):
		return self.caption

	def acceptLabel(self):
		return True
	
	def setCaption(self, caption):
		self.caption = caption
		return True


class Glyph(Node):
	code = None

	def __init__(self, code):
		Node.__init__(self)
		self.code = code

	def dump(self, tab):
		print "%sglyph(%x)" % (tab, self.code)

	def gen(self, gen):
		gen.genGlyph(self.code)

	def visit(self, visitor):
		visitor.onGlyph(self)

class LineBreak(Node):

	def __init__(self):
		Node.__init__(self)

	def dump(self, tab):
		print tab + "linebreak"

	def gen(self, gen):
		gen.genLineBreak()

	def visit(self, visitor):
		visitor.onLineBreak(self)


# Style family
class Style(Container):
	style = None

	def __init__(self, style):
		Container.__init__(self)
		self.style = style

	def onEvent(self, man, event):
		if event.level is not L_WORD:
			man.forward(event)
		elif event.id is not ID_NEW_STYLE:
			self.add(man, event.make())
		elif event.type == self.style:
			man.pop()
		else:
			self.add(man, event.make())

	def dumpHead(self, tab):
		print tab + "style(" + self.style + ","

	def gen(self, gen):
		gen.genStyleBegin(self.style)
		Container.gen(self, gen)
		gen.genStyleEnd(self.style)

	def visit(self, visitor):
		visitor.onStyle(self)


class OpenStyle(Container):
	style = None

	def __init__(self, style):
		Container.__init__(self)
		self.style = style

	def onEvent(self, man, event):
		if event.level is not L_WORD:
			man.forward(event)
		elif event.id is not ID_END_STYLE:
			self.add(man, event.make())
		elif event.type == self.style:
			man.pop()
		else:
			raise Exception("closing style without opening")

	def dumpHead(self, tab):
		print tab + "style(" + self.style + ","

	def gen(self, gen):
		gen.genStyleBegin(self.style)
		Container.gen(self, gen)
		gen.genStyleEnd(self.style)

	def visit(self, visitor):
		visitor.onStyle(self)


class FootNote(OpenStyle):
	"""A foot note. There a 3 kinds of note:
	* FOOTNOTE_EMBEDDED -- the note content is embedded in the text and the number automatically generated.
	* FOOTNOTE_REF -- only the note reference is given and the note content will be provide thereafter.
	* FOOTNOTE_DEF -- provide the content of a foot note whose reference is given by FOOTNOTE_REF.
	"""
	kind = None
	id = None		# footnote identifier as displayed to the user
	ref = None		# footnote identifier as used in the links

	def __init__(self, kind = FOOTNOTE_EMBED, ref = None, id = None):
		OpenStyle.__init__(self, 'footnote')
		self.kind = kind
		if not id:
			id = ref
		self.ref = ref
		self.id = id

	def dumpHead(self, tab):
		if self.kind == FOOTNOTE_EMBED:
			print '%sfootnote(' % tab
		else:
			print '%sfootnote#%s(' % (tab, self.ref)

	def isEmpty(self):
		return False

	def gen(self, gen):
		gen.genFootNote(self)

	def visit(self, visitor):
		visitor.onFootNote(self)

	def toText(self):
		return ""


class Link(Container):
	"""A link in a text."""
	ref = None

	def __init__(self, ref):
		Container.__init__(self)
		self.ref = ref

	def onEvent(self, man, event):
		if event.level is not L_WORD:
			man.forward(event)
		elif event.id is ID_END_LINK:
			man.pop()
		else:
			self.add(man, event.make())

	def dumpHead(self, tab):
		print tab + "link(" + self.ref + ","

	def gen(self, gen):
		gen.genLinkBegin(self.ref)
		Container.gen(self, gen)
		gen.genLinkEnd(self.ref)

	def visit(self, visitor):
		visitor.onLink(self)


# Par family
class Par(Container):

	def __init__(self):
		Container.__init__(self)

	def onEvent(self, man, event):
		if event.level is L_WORD:
			self.add(man, event.make())
		#elif event.level is L_PAR and event.id is ID_END:
		#	man.pop()
		else:
			man.forward(event)

	def dumpHead(self, tab):
		print tab + "par("

	def gen(self, gen):
		gen.genParBegin()
		Container.gen(self, gen)
		gen.genParEnd()

	def visit(self, visitor):
		visitor.onPar(self)


class Quote(Par):
	"""A quoted paragraph."""
	level = None

	def __init__(self, level):
		Par.__init__(self)
		self.level = level

	def onEvent(self, man, event):
		if event.id is ID_END_QUOTE:
			man.pop()
		elif event.id is ID_NEW_QUOTE:
			if event.level == self.level:
				pass
			else:
				man.forward(event)
		else:
			Par.onEvent(self, man, event)

	def dumpHead(self, tab):
		print tab + "quote("

	def gen(self, gen):
		gen.genQuoteBegin(self.level)
		Container.gen(self, gen)
		gen.genQuoteEnd(self.level)

	def visit(self, visitor):
		visitor.onQuote(self)


class Embedded(Node):
	"""Class representing document part not part of the main
	text like figures, listing, tables, etc.
	It defines mainly a label."""
	caption = None

	def __init__(self):
		Node.__init__(self)

	def setCaption(self, caption):
		self.caption = caption
		return True

	def getCaption(self):
		return self.caption

	def acceptLabel(self):
		return True

	def visit(self, visitor):
		visitor.onEmbedded(self)

	def getKind(self):
		"""Get the kind of embed object."""
		return "none"

	def numbering(self):
		return "figure"


class Block(Embedded):
	kind = None
	content = None

	def __init__(self, kind):
		self.kind = kind
		self.content = []

	def add(self, line):
		self.content.append(line)

	def dump(self, tab):
		self.dumpHead(tab)
		for line in self.content:
			print tab + "  " + line
		print tab + ")"

	def isEmpty(self):
		return False

	def toText(self):
		text = ''
		for line in self.content:
			text += line + '\n'
		return text


# List family
class ListItem(Container):
	"""Description of a list item."""

	def __init__(self):
		Container.__init__(self)
		self.content.append(Par())

	def dumpHead(self, tab):
		print tab + "item("

	def visit(self, visitor):
		visitor.onListItem(self)


class List(Container):
	"""Description of any kind of list (numbered, unnumbered)."""
	kind = None
	depth = None

	def __init__(self, kind, depth):
		Container.__init__(self)
		self.kind = kind
		self.depth = depth

	def onEvent(self, man, event):
		if event.level is L_WORD:
			if self.isEmpty():
				self.content.append(ListItem())
				self.last().content.append(Par())
			self.last().last().add(man, event.make())
		elif event.id is ID_NEW_ITEM:
			if event.depth < self.depth:
				man.forward(event)
			elif event.depth > self.depth:
				self.last().add(man, event.make())
			elif self.kind == event.type:
				self.content.append(ListItem())
			else:
				self.forward(event)
		elif event.id is ID_END_ITEM:
			man.pop()
		else:
			man.forward(event)

	def dumpHead(self, tab):
		print tab + "list(" + self.kind + "," + str(self.depth) + ", "

	def getItems(self):
		"""Get the list of items in the list."""
		return self.content

	def gen(self, gen):
		gen.genList(self)

	def visit(self, visitor):
		visitor.onList(self)


class DefItem(Container):
	"""Description of a definition list item."""

	def __init__(self):
		Container.__init__(self)
		self.term = Par()
		self.content.append(Par())

	def get_term(self):
		"""Get the defined term as a container of text-level items."""
		return self.term
		
	def get_def(self):
		"""Get the definition as a container of paragraph-level items."""
		return self

	def dumpHead(self, tab):
		print tab + "item(" + self.term + ", "

	def visit(self, visitor):
		visitor.onDefItem(self)


class DefList(Container):
	"""Description of definition list."""
	depth = None

	def __init__(self, depth):
		Container.__init__(self)
		self.depth = depth
		self.content.append(DefItem())

	def onEvent(self, man, event):
		if event.level is L_WORD:
			man.push(self.last().get_term())
			man.send(event)
		elif event.id is ID_END_TERM:
			man.push(self.last().last())
		elif event.id is ID_NEW_DEF:
			if event.depth < self.depth:
				man.forward(event)
			elif event.depth > self.depth:
				self.last().add(man, event.make())
			else:
				self.content.append(DefItem())
		elif event.id is ID_END_DEF:
			man.pop()
		else:
			man.forward(event)

	def dumpHead(self, tab):
		print tab + "deflist(" + str(self.depth) + ", "

	def getItems(self):
		"""Get the list of items in the list."""
		return self.content

	def gen(self, gen):
		gen.genDefList(self)
		pass

	def visit(self, visitor):
		visitor.onDefList(self)


# Table
TAB_CENTER = 0
TAB_LEFT = -1
TAB_RIGHT = 1
TAB_NORMAL = 0
TAB_HEADER = 1

TABLE_KINDS = [ 'normal', 'header' ]
TABLE_ALIGNS = [ 'left', 'center', 'right' ]

class Cell(Par):
	kind = None

	def __init__(self, kind, align = None, span = None, vspan = None):
		Par.__init__(self)
		self.kind = kind
		if align:
			self.setInfo(INFO_ALIGN, align)
		if span:
			self.setInfo(INFO_HSPAN, span)
		if vspan:
			self.setInfo(INFO_VSPAN, vspan)

	def isEmpty(self):
		return False

	def dumpHead(self, tab):
		print tab + 'cell(' + TABLE_KINDS[self.kind] + ', ' + TABLE_ALIGNS[self.align + 1] + ', ' + str(self.span) + ','

	def gen(self, gen):
		Container.gen(self, gen)

	def visit(self, visitor):
		visitor.onCell(self)


class Row(Container):
	kind = None

	def __init__(self, kind):
		Container.__init__(self)
		self.kind = kind

	def onEvent(self, man, event):
		if event.id is ID_NEW_CELL:
			self.add(man, event.make())
		elif event.id is ID_END_CELL:
			pass
		elif event.id is ID_END_ROW:
			man.pop()
		else:
			man.forward(event)

	def getWidth(self):
		width = 0
		for cell in self.content:
			width += cell.span
		return width

	def isEmpty(self):
		return False

	def dumpHead(self, tab):
		print tab + 'row('

	def getCells(self):
		"""Get the list of cells."""
		return self.content

	def visit(self, visitor):
		visitor.onRow(self)


class Table(Container):
	"""Repreentation of a table, that is composed or Rows that are
	composed, in turn, of cells."""
	width = None
	caption = None

	def __init__(self):
		Container.__init__(self)

	def getWidth(self):
		if self.width == None:
			self.width = self.content[0].getWidth()
		return self.width

	def onEvent(self, man, event):
		if event.id is ID_NEW_CELL:
			man.push(self.content[0])
			man.send(event)
		elif event.id is ID_NEW_ROW:
			self.add(man, event.make().content[0])
		else:
			man.forward(event)

	def gen(self, gen):
		gen.genTable(self)

	def isEmpty(self):
		return False

	def getRows(self):
		"""Get the list of rows."""
		return self.content

	def dumpHead(self, tab):
		print tab + 'table('

	def visit(self, visitor):
		visitor.onTable(self)

	def numbering(self):
		return "table"

	def acceptLabel(self):
		return True

	def setCaption(self, caption):
		self.caption = caption
		return True
	
	def getCaption(self):
		return self.caption


# main family
class HorizontalLine(Node):
	"""A simple horizontal line."""

	def __init__(self):
		Node.__init__(self)

	def dump(self, tab):
		print "%shorizontal-line()" % tab

	def gen(self, gen):
		gen.genHorizontalLine()

	def visit(self, visitor):
		visitor.onHorizontalLine(self)


class Header(Container):
	header_level = None
	title = None
	do_title = None

	def __init__(self, level):
		Container.__init__(self)
		self.level = L_HEAD
		self.header_level = level
		self.do_title = True
		self.title = Par()

	def onEvent(self, man, event):
		if event.level is L_WORD:
			if self.do_title:
				self.title.add(man, event.make())
			else:
				self.add(man, Par())
				man.send(event)
		elif event.level is L_PAR:
			self.add(man, event.make())
		elif event.level is not L_HEAD:
			man.forward(event)
		elif event.id is ID_TITLE:
			self.do_title = False
		elif event.object.header_level <= self.header_level:
			man.forward(event)
		else:
			self.add(man, event.make())

	def dumpHead(self, tab):
		print tab + "header" + str(self.header_level) + "("
		print tab + "  title("
		self.title.dump(tab + "    ")
		print tab + "  )"

	def isEmpty(self):
		return False

	def getLevel(self):
		return self.level

	def getHeaderLevel(self):
		return self.header_level

	def getTitle(self):
		return self.title

	def genTitle(self, gen):
		for item in self.title.getContent():
			item.gen(gen)

	def genBody(self, gen):
		Container.gen(self, gen)

	def gen(self, gen):
		if gen.genHeader(self):
			return
		gen.genHeaderBegin(self.header_level)
		gen.genHeaderTitleBegin(self.header_level)
		self.genTitle(gen)
		gen.genHeaderTitleEnd(self.header_level)
		gen.genHeaderBodyBegin(self.header_level)
		self.genBody(gen)
		gen.genHeaderBodyEnd(self.header_level)
		gen.genHeaderEnd(self.header_level)

	def isEmpty(self):
		return False

	def acceptLabel(self):
		return True

	def visit(self, visitor):
		visitor.onHeader(self)

	def numbering(self):
		return "header"


class Feature:
	"""A feature allows to add special services at generation time.
	Feature method are called at generation time."""

	def prepare(self, gen):
		pass


class Document(Container):
	"""This is the top object of the document, containing the headings
	and also the configuration environment."""
	env = None
	features = None
	labels = { }
	inv_labels = { }

	def __init__(self, env):
		Container.__init__(self)
		self.env = env
		self.features = []

	def onEvent(self, man, event):
		if event.level is L_WORD:
			self.add(man, Par())
			man.send(event)
		elif event.level is L_DOC and event.id is ID_END:
			pass
		else:
			self.add(man, event.make())

	def reduceVars(self, text):
		"""Reduce variables in the given text.
		- doc -- current document
		- text -- text to replace in."""

		m = VAR_REC.search(text)
		while m:
			val = str(self.getVar(m.group('varid')))
			text = text[:m.start()] + val + text[m.end():]
			m = VAR_REC.search(text)
		return text

	def getVar(self, id, default = ""):
		"""Get a variable and evaluates the variables in its content."""
		if self.env.has_key(id):
			return self.reduceVars(self.env[id])
		else:
			return default

	def setVar(self, name, val):
		self.env[name] = val

	def dumpHead(self, tab = ""):
		for k in iter(self.env):
			print k + "=" + self.env[k]
		print tab + "document("

	def addFeature(self, feature):
		"""Add a feature to the document."""
		if feature not in self.features:
			self.features.append(feature)

 	def pregen(self, gen):
		"""Call the prepare method of features of the document."""
 		for feature in self.features:
 			feature.prepare(gen)

	def addLabel(self, label, node):
		"""Add a label for the given node."""
		self.labels[label] = node
		self.inv_labels[node] = label
	
	def getLabel(self, label):
		"""Find the node matching the given label.
		Return None if there is no node matching the label."""
		if self.labels.has_key(label):
			return self.labels[label]
		else:
			return None

	def getLabelFor(self, node):
		"""Get the label, if any, for the given node."""
		if self.inv_labels.has_key(node):
			return self.inv_labels[node]
		else:
			return None

	def visit(self, visitor):
		visitor.onDocument(self)


class Visitor:
	"""Visitor default class."""
	
	def onDocument(self, doc):
		pass
	
	def onHeader(self, header):
		pass

	def onPar(self, par):
		pass
	
	def onQuote(self, quote):
		pass
	
	def onEmbedded(self, embeddded):
		pass
	
	def onList(self, list):
		pass
	
	def onDefList(self, list):
		pass
	
	def onTable(self, table):
		pass
	
	def onHeader(self, header):
		pass


class Factory:
	"""Factory to customize the building of objects."""
	
	def makeDocument(self, env):
		"""Build a document."""
		return Document(env)
	
	def makeHeader(self, level):
		"""Build a new header."""
		return Header(env)

	def makePar(self):
		"""Build a new paragraph."""
		return Par()
	
	def makeQuote(self, level):
		"""Build a new quote."""
		return Quote(level)
	
	def makeBlock(self, kind):
		"""Build a new block."""
		return Block(kind)
	
	def makeList(self, kind, depth):
		"""Build a new list."""
		return List(kind, depth)
	
	def makeListItem(self):
		"""Build a new list item."""
		return ListItem()

	def makeDefList(self, depth):
		"""Build a new definition list."""
		return DefList(depth)

	def makeDefItem(self):
		"""Build a definition list item."""
		return DefItem()
	
	def makeTable(self):
		"""Build a new table."""
		return Table()
	
	def makeRow(self, kind):
		"""Build a new table row."""
		return Row()

	def makeCell(self, kind, align = TAB_CENTER, span = 1):
		"""Build a new table cell."""
		return Cell(kind, align, span)

	def makeWord(self, text):
		"""Build a word."""
		return Word(text)

	def makeImage(self, path, width = None, height = None, caption = None):
		"""Build an image."""
		return Image(path, width, height, caption)

	def makeEmbeddedImage(self, path, width = None, height = None, caption = None, align = ALIGN_NONE):
		"""Build an embedded image."""
		return EmbeddedImage(path, width, height, caption, align)

	def makeGlyph(self, code):
		"""Build a glyph."""
		return Glyph(code)

	def makeStyle(self, style):
		"""Build a style."""
		return Style(style)

	def makeFootNote(self):
		"""Build a footnote."""
		return FootNote()

	def makeLink(self, ref):
		"""Make a link."""
		return Link(ref)
