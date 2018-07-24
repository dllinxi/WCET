static  __attribute__ ((__inline__)) void enable_external_irq();

int t(void) {
	asm volatile ("wrteei %0" :  : "i"(1));
	asm("stwbrx %0, %1, %2" :  : : "memory");
	asm("stwbrx %0, %1, %2" :  : "b"(val), "b"(addr), "b"(reg) : "memory");
}

register unsigned long int sp __attribute__ ((__asm__("sp"))) ;

__inline__
static void disable_external_irq()
{
	__asm__ __volatile__ (
			"wrteei %0" : : "i" (0)
	);
}

int main() {
	__asm__ (
			"mttbl %0	\n\t"
			"mttbu %1	\n\t"
			"mttbl %2	\n\t"
			: : "i" (0), "r" (tbu), "r" (tbl)
	);

	return 0;
}
