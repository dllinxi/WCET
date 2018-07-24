# $Id$
include ../Makefile.head

SOURCES = \
		mergec_bin.ml

OCAML_INC += ../frontc
OCAML_LIB += unix ../frontc/frontc

$(eval $(call ocaml_prog,mergec,$(SOURCES),$(PROG_FLAGS)))

mergec_bin.cmo: ../frontc/*.cmo

include ../Makefile.tail


