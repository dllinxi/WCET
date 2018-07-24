TESTS=\
	asm.c \
	gnuattr.c \
	melted.c \
	volatile.c \
	bitfield.c \
	isuffix.c \
	onlytdef.c \
	tricky.c
FAILURE=\
	bad.c
TOOL = ../printc/printc

all:

	@error=""; \
	for test in $(TESTS); do \
		echo -n "processing $$test ... "; \
		if $(TOOL) $$test > /dev/null 2> $$test.err ; then \
			echo "[OK]"; \
		else \
			echo "[FAILED]"; \
			error=ok; \
		fi; \
	done; \
	for test in $(FAILURE); do \
		echo -n "processing $$test ... "; \
		if $(TOOL) $$test > /dev/null 2> $$test.err ; then \
			echo "[FAILED]"; \
			error=ok; \
		else \
			echo "[OK]"; \
		fi; \
	done; \
	if [ -z "$$error" ]; then rm -rf *.err; fi
