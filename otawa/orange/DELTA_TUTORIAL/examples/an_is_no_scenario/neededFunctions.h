#include "missingVariables.h"
#define si_min_max_si_si_si s16_min_max_s16_s16_s16
inline s16 s16_min_max_s16_s16_s16(s16 value, s16 minimum, s16 maximum) {
	/*
	 FUNCTION: s16_min_max_s16_s16_s16
	 Parameter         type      Hex Range       Physical Range
	 Input:
	 value              s16      8000..7FFF       -32768..32767
	 minimum            s16      8000..7FFF       -32768..32767
	 maximum            s16      8000..7FFF       -32768..32767
	 Output:
	 res                s16      8000..7FFF       -32768..32767
	 Functionality:
	 res    = value
	 If value> maximum ,    value = maximum
	 else if value<minimum value = minimum
	 */
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
	/*
	 FUNCTION: u8_add_u8_u8
	 Parameter     type       Hex Range       Physical Range
	 Input:
	 x_value        u8         0..FFH            0..255
	 y_value        u8         0..FFH            0..255
	 Output:
	 res            u8         0..FFH            0..255
	 Functionality:
	 res = x_value + y_value
	 overflow : limit to FFH
	 */
	/*Functional Description of Saturated Intrinsic function __sathu*/
	/*Returns Saturated unsigned half word as the result.*/
	/*When parameter passed to the intrinsic function is > 65535 result is 65535.*/
	/*The lower limit is explicitly checked because the function returns 65535(instead of zero) if the passed parameter is negative.*/
	return (u8) (__satbu(x_value + y_value));
}

#define uc_sub_uc_uc u8_sub_u8_u8
#define U8_MIN         ((u8) 0U)
inline u8 u8_sub_u8_u8( u8 x_value, u8 y_value) {
	/*
	 FUNCTION: u8_sub_u8_u8
	 Parameter         type      Hex Range       Physical Range
	 Input:
	 x_value            u8         0..FF            0..255
	 y_value            u8         0..FF            0..255
	 Output:
	 res                u8         0..FF            0..255
	 Functionality:
	 res = x_value - y_value
	 underflow: limit to 0H
	 */
	if (x_value > y_value) {
		return (x_value - y_value);
	} else {
		return U8_MIN;
	}
}

#define ul_limitnew_ul_ul_ul u32_limitnew_u32_u32_u32
inline u32 u32_limitnew_u32_u32_u32(u32 newval, u32 oldval, u32 maxdif) {
	/*
	 FUNCTION: u32_limitnew_u32_u32_u32
	 Parameter    type       Hex Range       Physical Range
	 Input:
	 newval       u32       0..FFFFFFFF      0..4294967295
	 oldval       u32       0..FFFFFFFF      0..4294967295
	 maxdif       u32       0..FFFFFFFF      0..4294967295
	 Output:
	 value        u32       0..FFFFFFFF      0..4294967295
	 Functionality:
	 if (newval > oldval)
	 if (newval-oldval) > maxdif
	 value = oldval+maxdif
	 else
	 value = newval
	 else
	 if (oldval-newval) > maxdif
	 value = oldval-maxdif
	 else
	 value = newval
	 */
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
	/*
	 FUNCTION: s16_mul_s16_u8_fac1
	 Parameter         type      Hex Range         Physical Range
	 Input:
	 value             s16       8000..7FFF        -32768..32767
	 factor            u8        0..FF              0..0.996
	 Output:
	 res               s16       8000..7FFF        -32768..32767
	 Functionality:
	 res =   value*factor>>8
	 */
	return (s16) (((s32) value * (s32) factor) / 256);
}

#define ui_add_ui_si u16_add_u16_s16
#define U16_MIN        ((u16) 0U)
inline u16 u16_add_u16_s16( u16 x_value, s16 y_value) {
	/*
	 FUNCTION: u16_add_u16_s16
	 Parameter         type      Hex Range       Physical Range
	 Input:
	 x_value            u16         0..FFFFH          0..65535
	 y_value            s16      8000..7FFFH     -32768..32767
	 Output:
	 res                u16         0..FFFFH           0..65535
	 Functionality:
	 res = x_value + y_value
	 overflow : limit to FFFFH
	 underflow: limit to 0000H
	 */
	/*Functional Description of Saturated Intrinsic function __sathu*/
	/*Returns Saturated unsigned half word as the result.*/
	/*When parameter passed to the intrinsic function is > 65535 result is 65535.*/
	/*The lower limit is explicitly checked because the function returns 65535(instead of zero) if the passed parameter is negative.*/
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
	/*
	 FUNCTION:  s16_mmv_s16_s16_u16_fac1
	 Parameter         type      Hex Range     Physical Range
	 Input:
	 mmv                s16    8000....7FFF   -32768....32767
	 measure            s16    8000....7FFF   -32768....32767
	 crlc               u16       0....FFFF       0....0.9999
	 Output:
	 result             s16    8000....7FFF   -32768....32767
	 Functionality:
	 mmv+(((measure-mmv)*crlc)>>16)  with round off
	 If  ( ((( messwert - gmw) * mitko) >> 16)==0)
	 If  ((( messwert - gmw) * mitko) > 0)
	 result++
	 Elseif  ((( messwert - gmw) * mitko) < 0)
	 result--
	 Endif
	 Endif
	 */
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

