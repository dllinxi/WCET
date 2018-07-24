int p asm("r1");

static inline int cc_sl_trylock(int *lock) {
	int q __attribute__((bof("r1")));
	int p asm("r1");
	int r = 1;
	__asm("swap.w %0,[%2]0" : "=d" (r) : "0" (r), "a" ((int *) lock) : "memory");
	__asm("bl ok" : : );
	asm ( "mfmsr %0" : "=r" (msr) );
	asm volatile ( "mfmsr %0" : "=r" (msr) );
	return r;
}
