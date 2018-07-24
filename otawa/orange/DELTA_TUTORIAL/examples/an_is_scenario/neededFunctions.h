#include "missingVariables.h"
#define si_min_max_si_si_si s16_min_max_s16_s16_s16
inline s16 s16_min_max_s16_s16_s16(s16 value, s16 minimum, s16 maximum) {
		if (value > maximum) {
		return maximum;
	} else {
		if (value < minimum) {
			return minimum;
		} else {
			return value;
		}
	}
}

#define uc_add_uc_uc u8_add_u8_u8
inline u8 u8_add_u8_u8( u8 x_value, u8 y_value) {
		return (u8) (__satbu(x_value + y_value));
}

#define uc_sub_uc_uc u8_sub_u8_u8
#define U8_MIN         ((u8) 0U)
inline u8 u8_sub_u8_u8( u8 x_value, u8 y_value) {
	if (x_value > y_value) {
		return (x_value - y_value);
	} else {
		return U8_MIN;
	}
}

#define ul_limitnew_ul_ul_ul u32_limitnew_u32_u32_u32
inline u32 u32_limitnew_u32_u32_u32(u32 newval, u32 oldval, u32 maxdif) {
		if (newval > oldval) {
		if ((newval - oldval) > maxdif) {
			return (oldval + maxdif);
		} else {
			return (newval);
		}
	} else {
		if ((oldval - newval) > maxdif) {
			return (oldval - maxdif);
		} else {
			return (newval);
		}
	}
}

#define si_mul_si_uc_fak1 s16_mul_s16_u8_fac1
inline s16 s16_mul_s16_u8_fac1(s16 value, u8 factor) {
	return (s16) (((s32) value * (s32) factor) / 256);
}

#define ui_add_ui_si u16_add_u16_s16
#define U16_MIN        ((u16) 0U)
inline u16 u16_add_u16_s16( u16 x_value, s16 y_value) {
		s32 temp_value;
	temp_value = (s32) x_value + (s32) y_value;
	if (temp_value <= U16_MIN) {
		return U16_MIN;
	} else {
		return (u16) (__sathu(temp_value));
	}
}

#define SI_GMW_BER_SI_SI_UI s16_mmv_s16_s16_u16_fac1
inline s16 s16_mmv_s16_s16_u16_fac1(s16 mmv, s16 measure, u16 crlc) {
	u32 temp, quotient;
	s32 tmp_mmv = mmv, tmp_measure = measure;
	if (tmp_measure >= tmp_mmv) {
		/* Calculate increase in mmv value */
		temp = ((u32) (tmp_measure - tmp_mmv) * (u32) crlc);
		/* Rounding-off the mmv increase */
		quotient = temp / 65536 + (2 * (temp % 65536)) / 65536;
		/* Check for saturation */
		if (!quotient && temp) {
			/* Do Saturation correction*/
			return (s16) (tmp_mmv + 1);
		} else {
			/*Saturation not reached*/
			return (s16) (tmp_mmv + (s32) quotient);
		}
	} else {
		/* Calculate decrease in mmv value */
		temp = ((u32) (tmp_mmv - tmp_measure) * (u32) crlc);
		/* Rounding-off the mmv decrease */
		quotient = temp / 65536 + (2 * (temp % 65536)) / 65536;
		if (!quotient && temp) {
			/* Do Saturation correction */
			return (s16) (tmp_mmv - 1);
		} else {
			/* Saturation not reached*/
			return (s16) (tmp_mmv - (s32) quotient);
		}
	}
}

