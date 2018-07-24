# $Id: Makefile,v 1.3 2008/07/18 11:36:31 michiel Exp $


include ../Makefile.head

SOURCES = \
	main.ml \
	version.ml
OCAMLC_FLAGS += -g
OCAMLC_LDFLAGS += -g

NATIVE := 1

ifdef NATIVE
OPTIONS += native
endif
ifdef CUSTOM
OPTIONS += custom
endif

OCAML_INC += ../../frontc/frontc ../xml-light ../O_Range ../inst/lib/ ../graph 
OCAML_LIB += unix str ../../frontc/frontc/frontc  ../xml-light/xml-light ../O_Range/orange ../graph/resumeforgraph 

# Patch for a "missing" dependency
main.cmx: ../O_Range/orange.cmxa
main.cmo: ../O_Range/orange.cma

$(eval $(call ocaml_prog,orange,$(SOURCES),$(OPTIONS)))

VERSION=$(shell hg log -l 1 | head -1 | cut -c 14-30)
DATE=$(shell date +%Y%m%d)
version.ml:
	echo "let version=\"\\n!!VERSION=$(VERSION) ($(DATE))!!\\n\"" > version.ml

include ../Makefile.tail
