SOURCES = \
	fr.py
OBJECTS =  $(SOURCES:.py=.pyc)

ALL = $(OBJECTS)
INSTALL = custom-install
CLEAN = $(OBJECTS)

include ../Makefile.std

MODS_DIR = $(IDIR)/langs

custom-install:
	mkdir -p $(MODS_DIR)
	cp $(OBJECTS) $(MODS_DIR)


