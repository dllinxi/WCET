
SUBDIRS=mods backs langs css doc test

SOURCES = \
	common.py \
	doc.py \
	highlight.py \
	i18n.py \
	tparser.py \
	thot.py \
	back.py \
	extern.py
OBJECTS = $(SOURCES:.py=.pyc)

ALL = $(OBJECTS)
INSTALL = custom-install
CLEAN_FILES = $(OBJECTS)
DIST_FILES=smileys Makefile.std Makefile.spec COPYING README

include Makefile.std

custom-install:
	cp -R smileys $(IDIR)
	cp -R pix $(IDIR)
	cp $(OBJECTS) $(IDIR)
	chmod +x $(IDIR)/thot.pyc
	-ln -s $(IDIR)/thot.pyc $(PREFIX)/bin/thot

