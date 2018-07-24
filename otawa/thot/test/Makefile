
include ../Makefile.std

THOT_FILES = \
	caption.thot \
	dot.thot \
	gnuplot.thot \
	ref.thot \
	simple.thot

DIST_FILES=\
	$(THOT_FILES) \
	include.thot \
	gravure-thot.gif

test: test-single test-chapter test-latex

test-single:
	for f in *.thot; do ../thot.py $$f || exit 1; done

test-chapter:
	for f in *.thot; do ../thot.py -D HTML_ONE_FILE_PER=chapter $$f || exit 1; done

test-clean:
	rm -rf *.html *-imports *.tex *.pdf

test-latex: $(THOT_FILES:.thot=.pdf)

%.pdf: %.tex
	pdflatex $<

%.tex: %.thot
	../thot.py -t latex $<
