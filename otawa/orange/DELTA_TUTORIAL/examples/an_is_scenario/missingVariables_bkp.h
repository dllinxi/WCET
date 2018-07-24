#define u8 int
#define s8 int
#define s16	int
#define u16 int
#define s32 int
#define u32 int
#define s64 int
#define u64 int
#define flag int


// 30 


/* In function i_n_sp_is_rst */
static s8 n_sp_ofs_kwp ;				//from scenario

/* In function c_n_sp_is */

static u8 c_n_sp_lgrd_is; 				//from c_n_sp_lgrd_is_lih found in an_is_data
static flag lv_acin ; 				//from scenario
static flag lv_dly_n_sp_is ; 		//from scenario
static u16 c_n_sp_ofs_kwp_min ; 		//from scenario
static u16 c_n_sp_ofs_kwp_max ;	//from scenario 0xFF
static flag lv_n_sp_is_pow_act ;		//from scenario
static u16 n_sp_is_pow_1;				//from an_is_data
static flag lv_var_acin =0;			//from scenario
static flag lv_req_heat;				//from an_is_data
static u8 tam =22;						//from scenario 22 
static u8 c_tam_n_sp_heat =51;			//from scenario 0x33
static u8 c_tam_hys =7;				//from scenario 0x07
static flag lv_at ;					//from scenario
static u16 c_n_sp_heat_at;				//from an_is_data
static u16 c_n_sp_heat_mt ;		//from scenario
static flag lv_var_pste_2 ;			//from scenario
static u16 c_n_sp_is_pste_2_at;			//from an_is_data
static u16 c_n_sp_is_pste_2_mt ;	//from scenario 0x01f4
static flag lv_var_ars =0;				//from scenario
static u8 vs =0;						//from scenario
static u8 c_vs_min_n_sp_is_ars =0;		//from scenario
static u8 c_vs_max_n_sp_is_ars ;		//from scenario
static u16 c_n_sp_is_ars_at;			//from an_is_data
static u16 c_n_sp_is_ars_mt ;		//from scenario 0x01f4
static u8 c_vs_max_n_sp_cs =10;		//from scenario 0x0a
static u8 c_vs_hys_n_sp_cs =6;			//from scenario 0x06
static flag lv_is =1;					//from scenario ON in idle mode
static flag lc_is_n_sp_cs_dis;			//from an_is_data
static flag lv_cs =0;					//from scenario
static flag t_ast =0;					//from scenario
static flag lv_var_amt;					//from an_is_data
static flag lv_pste_disable =1;		//from scenario
static flag lv_pste_2_disable;			//from scenario
static u8 c_vs_min_n_sp_pste;			//from an_is_data
static u16 ang_pste;					//???
static u16 c_ang_pste_isc;				//from an_is_data
static u16 vel_ang_pste;				//???
static u16 c_vel_ang_pste_isc;			//from an_is_data
static u16 c_dly_n_sp_dec_pste;			//1..FFFF from an_i_data
static u8 c_vs_min_n_sp_pste_2;		//from an_is_data
static flag lv_n_sp_is_lih_act =0;		//from scenario
static flag lv_dri;						//from an_is_data
static flag lv_act_sa_eol = 0;			//from scenario
static u16 c_n_sp_is_sa_eol;			//from scenario
static flag lv_act_n_sp_is_ext_adj =0;	//from scenario
static u16 n_sp_is_ext_adj;				//???
static flag lv_act_n_sp_is_bas_ext_adj =0;	//Missing information in scenario, supposed to be "OFF"
											//because the contained n_sp_is_brake variable is not defined in scenario
static u16 n_sp_is_brake;				//from an_is_data
static u8 c_n_sp_lgrd_is_lih;			//1..FF from an_is_data
static u8 c_n_sp_inc_cs;				//1..FF from an_is_data
static u8 c_n_sp_dec_cs;				//1..FF from an_is_data
static u8 c_n_sp_inc_pste;				//1..FF from an_is_data
static u8 c_n_sp_dec_pste;				//1..FF from an_is_data
static flag lv_st_end =1;				//from scenario
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










