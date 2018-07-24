struct f
{
    unsigned int  flag : 1;  /* a bit flag: can either be on (1) or off (0) */
    signed int    num  : 4;  /* a signed 4-bit field; range -7...7 or -8...7 */
    signed int         : 3;  /* 3 bits of padding to round out to 8 bits */
} g;

typedef int T;

struct f2 {
	T f1: 3, f2: 4, f3: 1;
};
