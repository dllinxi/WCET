SOURCES = \
	dokuwiki.py \
	dot.py \
	gnuplot.py \
	latexmath.py \
	aafig.py
OBJECTS =  $(SOURCES:.py=.pyc)

ALL = $(OBJECTS)
INSTALL = custom-install
CLEAN = $(OBJECTS)

include ../Makefile.std

MODS_DIR = $(IDIR)/mods

custom-install:
	mkdir -p $(MODS_DIR)
	cp $(OBJECTS) $(MODS_DIR)


