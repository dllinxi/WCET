
 static inline int cc_sl_trylock(int *lock) {
	int r = 1;
	__asm("swap.w %0,[%2]0" : "=d" (r) : "0" (r), "a" ((int *) lock) : "memory");
	__asm("bl ok" : : );
	bof =;
	return r;
}
