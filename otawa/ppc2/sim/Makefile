CFLAGS=-I../include -I../src -g -O3
LIBADD =  $(shell bash ../src/ppc-config --libs)

all: ppc-sim

ppc-sim: ppc-sim.o  ../src/libppc.a
	$(CC) $(CFLAGS) -o $@ $< $(LIBADD)

clean:
	rm -f ppc-sim.o

distclean: clean
	rm -f ppc-sim
