# $Id$

include ../Makefile.head

SOURCES = \
	calipso_bin.ml
	
STAT_SOURCES = \
	stat.ml

OCAML_INC += ../frontc
OCAML_LIB += unix ../frontc/frontc
DIST += test/*.c test/*.out

$(eval $(call ocaml_prog,calipso,$(SOURCES),$(PROG_FLAGS)))

$(eval $(call ocaml_prog,calipso_stat,$(STAT_SOURCES),$(PROG_FLAGS)))

test: test_rk
TMP = /tmp/calipso.out

test_rk:
	@echo -n "TEST: regular switches ... "
	@./calipso -rk test/switch_reg.c > $(TMP)
	@diff test/switch_reg.out $(TMP)
	@rm $(TMP)
	@echo "DONE"

include ../Makefile.tail
