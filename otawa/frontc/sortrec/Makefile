# $Id$
include ../Makefile.head

SOURCES = \
	sortrec_bin.ml



OCAML_INC += ..//frontc
OCAML_LIB += unix ../frontc/frontc


$(eval $(call ocaml_prog,sortrec,$(SOURCES)))

include ../Makefile.tail
