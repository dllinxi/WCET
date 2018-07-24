# $Id$
include ../Makefile.head

SOURCES = \
        printc_bin.ml

OCAML_INC += ../frontc
OCAML_LIB += unix ../frontc/frontc

$(eval $(call ocaml_prog,printc,$(SOURCES),$(PROG_FLAGS)))

printc_bin.cmo: ../frontc/*.cmo

include ../Makefile.tail
