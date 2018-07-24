#define u8 int
#define s8 int
#define s16	int
#define u16 int
#define s32 int
#define u32 int
#define s64 int
#define u64 int
#define flag int

// 30 scenario values

// 2034
static flag lv_act_sa_eol = 0;			//from scenario
// 2034 + 16
static flag lv_n_sp_is_lih_act = 0;		//from scenario
// 2033
static flag lv_act_n_sp_is_ext_adj = 0;	//from scenario
// 2032
static flag lv_act_n_sp_is_bas_ext_adj = 0;	//Missing information in scenario, supposed to be "OFF"
// 230
static flag lv_is  = 1;					//from scenario ON in idle mode
// 215
static flag lv_st_end = 1;				//from scenario
// 184
static flag lv_var_acin = 0;			//from scenario
// 182 + 2 = 184
// Delta 182 at an_is.c:162 in c_n_sp_is (total count=1) : then=2; else=184;  // tam < c_tam_n_sp_heat
// Delta 2 at an_is.c:168 in c_n_sp_is (total count=1) : then=2; else=0;  // tam > u8_add_u8_u8(c_tam_n_sp_heat, c_tam_hys)
static u8 tam = 22;						//from scenario 22 
static u8 c_tam_n_sp_heat = 51;			//from scenario 0x33
static u8 c_tam_hys = 7;				//from scenario 0x07
//
// 4 + 4 + 3 = 11
static flag lv_pste_disable = 1;		//from scenario
static u8 vs = 0;						//from scenario
static flag lv_cs = 0;					//from scenario
static flag t_ast = 0;					//from scenario
//Delta 4 at an_is.c:275 in c_n_sp_is (total count=1) : then=9; else=13;  // 	((lv_pste_disable || lv_pste_2_disable || abs(ang_pste) > c_ang_pste_isc
//|| abs(vel_ang_pste) > c_vel_ang_pste_isc)) && vs > c_vs_min_n_sp_pste_2
//Delta 4 at an_is.c:248 in c_n_sp_is (total count=1) : then=9; else=13;  // 	lv_pste_disable || lv_pste_2_disable || lv_cs && !lv_var_amt || vs >
//	c_vs_min_n_sp_pste || abs(ang_pste) > c_ang_pste_isc ||
//Delta 3 at an_is.c:231 in c_n_sp_is (total count=1) : then=7; else=10;  // 	t_ast > t_ast_min_n_sp_is_cs && ((lv_is || lc_is_n_sp_cs_dis)) && lv_cs abs(vel_ang_pste) > c_vel_ang_pste_isc

// D 3 + 2 + 2 + 1 = 8
static u8 c_vs_max_n_sp_cs = 10;		//from scenario 0x0a
static u8 c_vs_hys_n_sp_cs = 6;			//from scenario 0x06
static flag lv_var_ars  =0;				//from scenario
static u8 c_vs_min_n_sp_is_ars = 0;		//from scenario
// Delta 3 at an_is.c:220 in c_n_sp_is (total count=1) : then=2; else=5;  // vs < u8_sub_u8_u8(c_vs_max_n_sp_cs, c_vs_hys_n_sp_cs)
// Delta 2 at an_is.c:226 in c_n_sp_is (total count=1) : then=2; else=0;  // vs > c_vs_max_n_sp_cs
//Delta 2 at an_is.c:204 in c_n_sp_is (total count=1) : then=4; else=2;  // lv_var_ars && vs > c_vs_min_n_sp_is_ars && vs < c_vs_max_n_sp_is_ars
//Delta 1 at an_is.c:385 in c_n_sp_is (total count=1) : then=2; else=3;  // lv_is && lv_dri && vs < c_dri_vs_min_vb && !tmp_u16

// DELTA 2
static flag lv_var_pste_2  ;//=0;			//from scenario
//Delta 2 at an_is.c:189 in c_n_sp_is (total count=1) : then=4; else=2;  // lv_var_pste_2

// DELTA 1
static flag lv_n_sp_is_pow_act  ;//=0;		//from scenario
//Delta 1 at an_is.c:148 in c_n_sp_is (total count=1) : then=3; else=2;  // lv_n_sp_is_pow_act

/* NONE 
Delta 218 at an_is.c:414 in c_n_dif (total count=1) : then=225; else=7;  // lv_n_dif_trans_is
Delta 11 at an_is.c:357 in c_n_sp_is (total count=1) : then=6; else=17;  // lv_n_sp_is_cs ? n_sp_is_3 == n_sp_is_cs : lv_n_sp_is_ramp_cs
Delta 10 at an_is.c:379 in c_n_sp_is (total count=1) : then=3; else=13;  // tmp_u32_n_sp_is_3_fine < n_sp_is_fine
8 Delta at an_is.c:364 in c_n_sp_is (total count=1) : then=8; else=0;  // ((t_dly_n_sp_dec_pste ? n_sp_is_3 == n_sp_is_pste : lv_n_sp_is_ramp_pste)) || ((t_dly_n_sp_dec_pste_2 ? n_sp_is_3 == n_sp_is_pste_2 : lv_n_sp_is_ramp_pste_2))
Delta 4 at an_is.c:450 in c_n_dif (total count=1) : then=2; else=6;  // n_dif_mmv > 0 && !lv_var_amt && lv_cs_vs_in_hys
Delta 3 at an_is.c:430 in c_n_dif (total count=1) : then=5; else=2;  // lv_n_dif_crlc_marc || n_dif <= (int)((-150))
Delta 3 at an_is.c:456 in c_n_dif (total count=1) : then=3; else=0;  // n_dif_mmv < c_n_dif_min_mmv
Delta 3 at neededFunctions.h:18 in s16_min_max_s16_s16_s16 (total count=1) : then=6; else=9;  // value > maximum
Delta 3 at neededFunctions.h:65 in u8_sub_u8_u8 (total count=1) : then=8; else=5;  // x_value_0 > y_value_0
Delta 2 at an_is.c:240 in c_n_sp_is (total count=1) : then=2; else=0;  // !lv_n_sp_is_cs && lv_n_sp_is_ramp_end
Delta 2 at an_is.c:293 in c_n_sp_is (total count=1) : then=2; else=0;  // !lv_t_dly_pste_2_run && lv_n_sp_is_ramp_end
Delta 2 at an_is.c:265 in c_n_sp_is (total count=1) : then=2; else=0;  // !lv_t_dly_pste_run && lv_n_sp_is_ramp_end
Delta 2 at neededFunctions.h:96 in u32_limitnew_u32_u32_u32 (total count=1) : then=8; else=6;  // newval - oldval > maxdif
Delta 2 at neededFunctions.h:102 in u32_limitnew_u32_u32_u32 (total count=1) : then=8; else=6;  // oldval - newval > maxdif
Delta 2 at an_is.c:174 in c_n_sp_is (total count=1) : then=4; else=2;  // lv_req_heat_n_sp_is
Delta 8 at an_is.c:416 in c_n_dif (total count=1) : then=4; else=12;  // n_dif >= 0
Delta 8 at an_is.c:259 in c_n_sp_is (total count=1) : then=2; else=10;  // t_dly_n_sp_dec_pste
Delta 8 at an_is.c:287 in c_n_sp_is (total count=1) : then=2; else=10;  // t_dly_n_sp_dec_pste_2
Delta 5 at an_is.c:423 in c_n_dif (total count=1) : then=4; else=9;  // n_dif_3 >= n_dif
*/



/* VARS WITH NO CORRESPONDENCE */
static s8 n_sp_ofs_kwp = 0;				//from scenario
static flag lv_acin = 0; 				//from scenario
static flag lv_dly_n_sp_is = 0; 		//from scenario
static u16 c_n_sp_ofs_kwp_min = 0; 		//from scenario
static u16 c_n_sp_ofs_kwp_max = 255;	//from scenario 0xFF
static flag lv_at = 0;					//from scenario
static u16 c_n_sp_heat_mt = 850;		//from scenario
static u16 c_n_sp_is_pste_2_mt = 500;	//from scenario 0x01f4
static u8 c_vs_max_n_sp_is_ars = 0;		//from scenario
static u16 c_n_sp_is_ars_mt = 500;		//from scenario 0x01f4


static u8 c_n_sp_lgrd_is; 				//from c_n_sp_lgrd_is_lih found in an_is_data
static u16 n_sp_is_pow_1;				//from an_is_data
static flag lv_req_heat;				//from an_is_data
static u16 c_n_sp_heat_at;				//from an_is_data
static u16 c_n_sp_is_pste_2_at;			//from an_is_data
static u16 c_n_sp_is_ars_at;			//from an_is_data
static flag lc_is_n_sp_cs_dis;			//from an_is_data
static flag lv_var_amt;					//from an_is_data
static flag lv_pste_2_disable;			//from scenario
static u8 c_vs_min_n_sp_pste;			//from an_is_data
static u16 ang_pste;					//???
static u16 c_ang_pste_isc;				//from an_is_data
static u16 vel_ang_pste;				//???
static u16 c_vel_ang_pste_isc;			//from an_is_data
static u16 c_dly_n_sp_dec_pste;			//1..FFFF from an_i_data
static u8 c_vs_min_n_sp_pste_2;		//from an_is_data
static flag lv_dri;						//from an_is_data
static u16 c_n_sp_is_sa_eol;			//from scenario
static u16 n_sp_is_ext_adj;				//???
											//because the contained n_sp_is_brake variable is not defined in scenario
static u16 n_sp_is_brake;				//from an_is_data
static u8 c_n_sp_lgrd_is_lih;			//1..FF from an_is_data
static u8 c_n_sp_inc_cs;				//1..FF from an_is_data
static u8 c_n_sp_dec_cs;				//1..FF from an_is_data
static u8 c_n_sp_inc_pste;				//1..FF from an_is_data
static u8 c_n_sp_dec_pste;				//1..FF from an_is_data
static u8 c_dri_vs_min_vb;			//from an_is_data
static u16 n;							//???
static u16 n_fast;						//???
static u8 c_n_dif_crlc;					//from an_is_data
static s16 c_n_dif_min_mmv;				//from an_is_data
static u8 c_n_dif_fac;					//from an_is_data



int MAX(int i1, int i2){
	return (i1>i2?i1:i2);
}
int MIN(int i1, int i2){
	return (i1<i2?i1:i2);
}

int N_sp_is_ch(){
	return (0);
}
/*Missing Function ui_div_ui_ui()*/
int ui_div_ui_ui(int i1, int i2){
	//return (i2!=0?i1/i2:0);
	return 0;
}

int abs(int i){
	return (i<0? -i : i);
}

/*Missing Function si_sub_si_si()*/
int si_sub_si_si(int i1, int i2){
	return (i1>i2?i1-i2:0);
}

/*Missing Function si_sub_si_si()*/
int si_sub_ui_ui(int i1, int i2){
	return (i1>i2?i1-i2:0);
}

/*Missing Function __sathu()*/
inline s32 __sathu(s32 val){
	return (val>65535?65535:val);
}

inline s32 __satbu(s32 val){
	return val;
}










