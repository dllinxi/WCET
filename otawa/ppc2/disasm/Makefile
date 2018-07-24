
CFLAGS=-I../include -I../src -g -O3
LIBADD += $(shell bash ../src/ppc-config --libs)

all: ppc-disasm

ppc-disasm: ppc-disasm.o ../src/libppc.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBADD)

clean:
	rm -rf ppc-disasm.o

distclean: clean
	rm -rf ppc-disasm
