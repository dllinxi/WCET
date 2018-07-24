
CSS = \
	initial.css \
	style_blue.css \
	style_brown.css \
	style_gray.css  \
	style_green.css \
	style_pink.css \
	style_purple.css

ALL = $(CSS)
INSTALL = custom-install
CLEAN_FILES = $(CSS)
DIST_FILES=images ini make.py style.css

include ../../Makefile.std

MINIMA_DIR=$(IDIR)/css/minima

custom-install:
	mkdir -p $(MINIMA_DIR)
	cp -R $(CSS) images $(MINIMA_DIR)

%.css: ini/%.ini style.css
	./make.py $*
