.ml.cmo:
	ocamlc -c $<

.mli.cmi:
	ocamlc -c $<
	
lib_LIBRARIES= \
	libglue.a

libglue_a_SOURCES = \
	glue.cpp
bin_SCRIPTS = yacastop yacastop.real example
caml_SCRIPTS = cyacas.cma cyacas.cmi
CYACAS_OBJS = cyacas.cmo example.cmo example.cmi
CLEANFILES = $(caml_SCRIPTS) $(bin_SCRIPTS) $(CYACAS_OBJS)
EXTRA_DIST = cyacas.ml cyacas.mli example.ml

cyacas.cma: cyacas.cmo cyacas.cmi libglue.a
	ocamlc -custom -a -o cyacas.cma cyacas.cmo -cclib -lglue -cclib -lyacas -cclib -lyacasnumbers -cclib -lyacasplatform -cclib -lltdl -ccopt -L$(libdir) -ccopt -L$(yacas_libdir)

cyacas.cmo: cyacas.ml cyacas.cmi

cyacas.cmi: cyacas.mli

example: example.cmo example.cmi cyacas.cma libglue.a
	ocamlc -cc "g++" -custom -o example cyacas.cma example.cmo -ccopt -L./
	
example.cmo: example.ml cyacas.cma

yacastop.real: cyacas.cma libglue.a
	ocamlmktop -cc "g++" -custom -o yacastop.real cyacas.cma -ccopt -L./ 
