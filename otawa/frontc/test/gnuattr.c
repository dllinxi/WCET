/* test of GNU attributes */

#define __attribute__ ((__const))

extern unsigned short int **__ctype_b_loc(void) __attribute__ ((__const));


extern int strerror_r(int __errnum, char *__buf, int __buflen) __asm__ ("" "__xpg_strerror_r") __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2)));
