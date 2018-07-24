# $Id: Makefile,v 1.2 2008/07/18 11:36:28 michiel Exp $
include ../Makefile.head

SOURCES = \
	conditionAnalysis.ml \
	cexptostr.ml \
	cvariables.ml \
	constante.ml\
	coutput.ml \
	rename.ml\
	util.ml\
	extractinfoPtr.ml\
	cvarabs.ml \
	interval.ml \
	increment.ml\
	covariance.ml \
	cextraireboucle.ml \
	tMap.ml \
	maxima.ml \
	flowfacts.ml \
	balance.ml \
	wcee.ml \
	orange.ml 

# absClassProperty.ml\

OCAMLC_CFLAGS += -g
OCAMLC_LDFLAGS += -g
OCAML_INC += ../../frontc/frontc ../xml-light
OCAML_LIB += unix ../../frontc/frontc/frontc ../xml-light/xml-light
OCAMLYACC_FLAGS = -v

$(eval $(call ocaml_prog,orange, $(SOURCES), no_install $(PROG_FLAGS)))
$(eval $(call ocaml_lib,orange,$(SOURCES), no_install))

include ../Makefile.tail
