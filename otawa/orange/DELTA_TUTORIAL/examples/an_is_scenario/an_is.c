/*Sans Mangle-It C++ Source Code Obfuscator*/
/*#include <general.h>
 #include <ecmath.h>
 #include <calm.h>
 #include <ensc.h>
 #include <airt.h>
 #include <comr.h>
 #include <coms.h>
 #include <drrq.h>
 #include <enlu.h>
 #include <enos.h>
 #include <ensd.h>
 #include <enss.h>
 #include <extc.h>
 #include <fmsp.h>
 #include <lasp.h>
 #include <sair.h>
 #include <sycf.h>
 #include <trsm.h>
 #include <vhmd.h>
 #include <pwsl.h>
 #include <ente.h>
 #include <stsy.h>
 #include <ca_data.h>*/
#include "neededFunctions.h"
#define MEM_DATA MEM_DATA_PUBLIC_16
//#include <gmem.h>
u16 n_sp_is;
u16 n_sp_is_2;
s16 n_dif;
s16 n_dif_cor;
s16 n_dif_st;
s16 n_dif_mmv;
u16 n_sp_is_ratio;
#define MEM_DATA MEM_DATA_PUBLIC_8
//#include <gmem.h>
u8 n_max_tol_st;
#define MEM_DATA MEM_DATA_PUBLIC_FLAG
//#include <gmem.h>
flag lv_n_sp_is_cs;
flag lv_n_sp_is_pste;
flag lv_n_sp_is_pste_2;
#define MEM_DATA MEM_DATA_PRIVATE_16
//#include <gmem.h>
u16 n_sp_is_pow;
u16 n_sp_is_3;
#define MEM_DATA MEM_DATA_PRIVATE_FLAG
//#include <gmem.h>
flag lv_req_heat_n_sp_is;
#define MEM_DATA MEM_DATA_PRIVATE_32
//#include <gmem.h>
static u32 n_sp_is_fine;
#define  MEM_DATA MEM_DATA_PRIVATE_16
//#include <gmem.h>
static u16 n_sp_is_1;
static u16 dri_n_sp_is;
static u16 acin_n_sp_is;
static u16 dri_acin_n_sp_is;
static u16 t_ast_min_n_sp_is_cs;
static u16 n_sp_is_lih;
static u16 dri_n_sp_is_lih;
static u16 n_sp_is_pste;
static u16 n_sp_is_pste_2;
static u16 n_sp_is_cs;
static u16 n_sp_is_toil;
static s16 n_dif_1;
static s16 n_dif_2;
static s16 n_dif_3;
#define MEM_DATA MEM_DATA_PRIVATE_8
//#include <gmem.h>
static u8 fac_n_sp_ofs_kwp;
static u8 t_dly_n_sp_dec_pste;
static u8 t_dly_n_sp_dec_pste_2;
#define MEM_DATA MEM_DATA_PRIVATE_FLAG
//#include <gmem.h>
static flag lv_cs_vs_in_hys;
static flag lv_n_sp_is_ramp_cs;
static flag lv_n_sp_is_ramp_pste;
static flag lv_n_sp_is_ramp_pste_2;
static flag lv_n_sp_is_ramp_end;
static flag lv_t_dly_pste_run;
static flag lv_t_dly_pste_2_run;
static flag lv_n_dif_trans_is;
static flag lv_n_dif_crlc_marc;

#define        NC_N_SP_IS_MAX          8160
#define        NC_N_DIF_CRLC_0         0x00
#define        NC_N_DIF_CRLC_1         0xFFFF
#define        NC_N_DIF_MIN_CRLC       (s16)(-150)
#define MEM_CODE MEM_CODE_SLOW
//#include <gmem.h>
void i_n_sp_is_rst(void)
{
//	c_n_sp_is_temp();
  n_sp_is_2 = ui_add_ui_si(n_sp_is_1,
                           si_mul_si_uc_fak1(((s16) n_sp_ofs_kwp * 2), fac_n_sp_ofs_kwp));
  n_sp_is_2 = MIN(n_sp_is_2, (u16) NC_N_SP_IS_MAX);
  n_sp_is_3 = n_sp_is_2;
  n_sp_is = n_sp_is_2;
  n_sp_is_fine = (u32) n_sp_is * 16;
  n_dif_st = (s16) n_max_tol_st * 32;
}
#define MEM_CODE MEM_CODE_10MS
//#include <gmem.h>
void c_n_sp_is(void)
{

  u16 tmp_u16;
  s16 tmp_s16;
  u16 tmp_u16_n_sp_is_cs;
  u16 tmp_u16_n_sp_is_var_pste_2;
  u16 tmp_u16_n_sp_is_ars;
  u16 tmp_u16_n_sp_is_req_heat;
  u16 tmp_u16_n_sp_is_pste;
  u16 tmp_u16_n_sp_is_pste_2;
  u32 tmp_u32_n_sp_is_3_fine;
  u16 tmp_u16_grd_lim = 0;
  u16 tmp_u16_grd_lim_up = c_n_sp_lgrd_is;
  u16 tmp_u16_grd_lim_down = c_n_sp_lgrd_is;
  if (lv_acin)
    {
      if (lv_dly_n_sp_is)
        {
          tmp_u16 = dri_acin_n_sp_is;
        }
      else
        {
          tmp_u16 = acin_n_sp_is;
        }
    }
  else
    {
      if (lv_dly_n_sp_is)
        {
          tmp_u16 = dri_n_sp_is;
        }
      else
        {
          tmp_u16 = n_sp_is_1;
        }
    }
  n_sp_is_2 = ui_add_ui_si(tmp_u16,
                           si_min_max_si_si_si(
                             si_mul_si_uc_fak1(((s16) n_sp_ofs_kwp * 2),
                                 fac_n_sp_ofs_kwp), (s16) c_n_sp_ofs_kwp_min,
                             (s16) c_n_sp_ofs_kwp_max));
  n_sp_is_2 = MIN(n_sp_is_2, (u16) NC_N_SP_IS_MAX);
  if (lv_n_sp_is_pow_act)
    {
      n_sp_is_pow = n_sp_is_pow_1;
    }
  else
    {
      n_sp_is_pow = 0;
    }
  if (lv_var_acin)
    {
      lv_req_heat_n_sp_is = lv_req_heat;
    }
  else
    {
      if (tam < c_tam_n_sp_heat)
        {
          lv_req_heat_n_sp_is = 1;
        }
      else
        {
          if (tam > uc_add_uc_uc(c_tam_n_sp_heat, c_tam_hys))
            {
              lv_req_heat_n_sp_is = 0;
            }
        }
    }
  if (lv_req_heat_n_sp_is)
    {
      if (lv_at)
        {
          tmp_u16_n_sp_is_req_heat = c_n_sp_heat_at;
        }
      else
        {
          tmp_u16_n_sp_is_req_heat = c_n_sp_heat_mt;
        }
    }
  else
    {
      tmp_u16_n_sp_is_req_heat = 0;
    }
  if (lv_var_pste_2)
    {
      if (lv_at)
        {
          tmp_u16_n_sp_is_var_pste_2 = c_n_sp_is_pste_2_at;
        }
      else
        {
          tmp_u16_n_sp_is_var_pste_2 = c_n_sp_is_pste_2_mt;
        }
    }
  else
    {
      tmp_u16_n_sp_is_var_pste_2 = 0;
    }
  if (lv_var_ars && (vs > c_vs_min_n_sp_is_ars)
      && (vs < c_vs_max_n_sp_is_ars))
    {
      if (lv_at)
        {
          tmp_u16_n_sp_is_ars = c_n_sp_is_ars_at;
        }
      else
        {
          tmp_u16_n_sp_is_ars = c_n_sp_is_ars_mt;
        }
    }
  else
    {
      tmp_u16_n_sp_is_ars = 0;
    }
  if (vs < uc_sub_uc_uc(c_vs_max_n_sp_cs, c_vs_hys_n_sp_cs))
    {
      lv_cs_vs_in_hys = 1;
    }
  else
    {
      if (vs > c_vs_max_n_sp_cs)
        {
          lv_cs_vs_in_hys = 0;
        }
    }
  if ((t_ast > t_ast_min_n_sp_is_cs) && (lv_is || lc_is_n_sp_cs_dis) && lv_cs
      && !lv_var_amt && lv_cs_vs_in_hys)
    {
      lv_n_sp_is_ramp_cs = 1;
      tmp_u16_n_sp_is_cs = n_sp_is_cs;
      lv_n_sp_is_cs = 1;
    }
  else
    {
      if (!lv_n_sp_is_cs && lv_n_sp_is_ramp_end)
        {
          lv_n_sp_is_ramp_cs = 0;
        }
      tmp_u16_n_sp_is_cs = 0;
      lv_n_sp_is_cs = 0;
    }
  tmp_u16_n_sp_is_pste = n_sp_is_pste;
  if (lv_pste_disable || lv_pste_2_disable || (lv_cs && !lv_var_amt)
      || (vs > c_vs_min_n_sp_pste) || (abs(ang_pste) > c_ang_pste_isc)
      || (abs(vel_ang_pste) > c_vel_ang_pste_isc))
    {
      t_dly_n_sp_dec_pste = c_dly_n_sp_dec_pste;
      lv_n_sp_is_ramp_pste = 1;
      lv_n_sp_is_pste = 1;
      lv_t_dly_pste_run = 1;
    }
  else
    {
      if (t_dly_n_sp_dec_pste)
        {
          t_dly_n_sp_dec_pste--;
        }
      else
        {
          if (!lv_t_dly_pste_run && lv_n_sp_is_ramp_end)
            {
              lv_n_sp_is_ramp_pste = 0;
            }
          lv_t_dly_pste_run = 0;
          tmp_u16_n_sp_is_pste = 0;
        }
      lv_n_sp_is_pste = 0;
    }
  tmp_u16_n_sp_is_pste_2 = n_sp_is_pste_2;
  if ((lv_pste_disable || lv_pste_2_disable
       || (abs(ang_pste) > c_ang_pste_isc)
       || (abs(vel_ang_pste) > c_vel_ang_pste_isc))
      && (vs > c_vs_min_n_sp_pste_2))
    {
      t_dly_n_sp_dec_pste_2 = c_dly_n_sp_dec_pste;
      lv_n_sp_is_ramp_pste_2 = 1;
      lv_n_sp_is_pste_2 = 1;
      lv_t_dly_pste_2_run = 1;
    }
  else
    {
      if (t_dly_n_sp_dec_pste_2)
        {
          t_dly_n_sp_dec_pste_2--;
        }
      else
        {
          if (!lv_t_dly_pste_2_run && lv_n_sp_is_ramp_end)
            {
              lv_n_sp_is_ramp_pste_2 = 0;
            }
          lv_t_dly_pste_2_run = 0;
          tmp_u16_n_sp_is_pste_2 = 0;
        }
      lv_n_sp_is_pste_2 = 0;
    }
  if (lv_n_sp_is_lih_act)
    {
      if (lv_dri)
        {
          n_sp_is_3 = dri_n_sp_is_lih;
        }
      else
        {
          n_sp_is_3 = n_sp_is_lih;
        }
    }
  else
    {
      if (lv_act_sa_eol)
        {
          n_sp_is_3 = c_n_sp_is_sa_eol;
        }
      else
        {
          if (lv_act_n_sp_is_ext_adj)
            {
              n_sp_is_3 = n_sp_is_ext_adj;
            }
          else
            {
              if (lv_act_n_sp_is_bas_ext_adj)
                {
                  n_sp_is_3 = n_sp_is_2;
                }
              else
                {
                  int tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
                  tmp0 = MAX(n_sp_is_brake, tmp_u16_n_sp_is_cs);
                  tmp1 = MAX(n_sp_is_toil, tmp0);
                  tmp2 = MAX(tmp_u16_n_sp_is_ars, tmp1);
                  tmp3 = MAX(tmp_u16_n_sp_is_var_pste_2, tmp2);
                  tmp4 = MAX(tmp_u16_n_sp_is_req_heat, tmp3);
                  tmp5 = MAX(n_sp_is_pow, tmp4);
                  tmp6 = MAX(N_sp_is_ch(), tmp5);
                  n_sp_is_3 = MAX(n_sp_is_2, tmp6);
                  tmp7 =  MAX(tmp_u16_n_sp_is_pste, tmp_u16_n_sp_is_pste_2);
                  n_sp_is_3 = MAX(n_sp_is_3, tmp7);
                }
            }
        }
    }
  n_sp_is_3 = MIN(n_sp_is_3, NC_N_SP_IS_MAX);
  tmp_u16 = 0;
  if (lv_n_sp_is_lih_act)
    {
      tmp_u16_grd_lim_up = tmp_u16_grd_lim_down = (u16) c_n_sp_lgrd_is_lih
                           * 4;
    }
  else
    {
      if (lv_n_sp_is_cs ? (n_sp_is_3 == n_sp_is_cs) : lv_n_sp_is_ramp_cs)
        {
          tmp_u16_grd_lim_up = c_n_sp_inc_cs;
          tmp_u16_grd_lim_down = c_n_sp_dec_cs;
        }
      else
        {
          if ((t_dly_n_sp_dec_pste ?
               (n_sp_is_3 == n_sp_is_pste) : lv_n_sp_is_ramp_pste)
              || (t_dly_n_sp_dec_pste_2 ?
                  (n_sp_is_3 == n_sp_is_pste_2) :
                  lv_n_sp_is_ramp_pste_2))
            {
              tmp_u16_grd_lim_up = c_n_sp_inc_pste;
              tmp_u16_grd_lim_down = c_n_sp_dec_pste;
              tmp_u16 = 1;
            }
        }
    }
  tmp_u32_n_sp_is_3_fine = (u32) n_sp_is_3 * (u32) 16;
  if (lv_st_end)
    {
      if (tmp_u32_n_sp_is_3_fine < n_sp_is_fine)
        {
          tmp_u16_grd_lim = tmp_u16_grd_lim_down;
        }
      else
        {
          if (lv_is && lv_dri && (vs < c_dri_vs_min_vb) && !tmp_u16)
            {
              tmp_u16_grd_lim = 0;
            }
          else
            {
              tmp_u16_grd_lim = tmp_u16_grd_lim_up;
            }
        }
      n_sp_is_fine = ul_limitnew_ul_ul_ul(tmp_u32_n_sp_is_3_fine,
                                          n_sp_is_fine, (u32) tmp_u16_grd_lim);
    }
  else
    {
      n_sp_is_fine = tmp_u32_n_sp_is_3_fine;
    }
  lv_n_sp_is_ramp_end = (flag)(n_sp_is_fine == tmp_u32_n_sp_is_3_fine);
  n_sp_is = (u16)(n_sp_is_fine / 16);
  n_sp_is = MIN(n_sp_is, NC_N_SP_IS_MAX);
  n_sp_is_ratio = ui_div_ui_ui(n, (n_sp_is * 8));
}
#define MEM_CODE MEM_CODE_10MS
//#include <gmem.h>
void c_n_dif(void)
{
  u16 tmp_u16_crlc;
  n_dif = (s16)(n_sp_is - n_fast);
  if (lv_is)
    {
      if (lv_n_dif_trans_is)
        {
          if (n_dif >= 0)
            {
              tmp_u16_crlc = NC_N_DIF_CRLC_0;
              lv_n_dif_crlc_marc = 0;
            }
          else
            {
              if (n_dif_3 >= n_dif)
                {
                  tmp_u16_crlc = NC_N_DIF_CRLC_1;
                  lv_n_dif_crlc_marc = 0;
                }
              else
                {
                  if (lv_n_dif_crlc_marc || (n_dif <= NC_N_DIF_MIN_CRLC))
                    {
                      tmp_u16_crlc = (u16) c_n_dif_crlc << 8;
                      lv_n_dif_crlc_marc = 1;
                    }
                  else
                    {
                      tmp_u16_crlc =
                        NC_N_DIF_CRLC_1;
                    }
                }
            }
          n_dif_mmv = SI_GMW_BER_SI_SI_UI(n_dif_mmv, n_dif, tmp_u16_crlc);
        }
      else
        {
          lv_n_dif_trans_is = 1;
          n_dif_mmv = n_dif;
          lv_n_dif_crlc_marc = 1;
        }
      if (n_dif_mmv > 0)
        {
          n_dif_mmv = 0;
        }
      else
        {
          if (n_dif_mmv < c_n_dif_min_mmv)
            {
              n_dif_mmv = c_n_dif_min_mmv;
            }
        }
    }
  else
    {
      n_dif_mmv = 0;
      lv_n_dif_trans_is = 0;
    }
  n_dif_3 = n_dif_2;
  n_dif_2 = n_dif_1;
  n_dif_1 = n_dif;
  n_dif_cor = si_sub_si_si(n_dif, si_mul_si_uc_fak1(n_dif_mmv, c_n_dif_fac));
  n_dif_st = si_sub_ui_ui((u16) n_max_tol_st << 5, n);
}
#define MEM_CODE MEM_CODE_SLOWER_100MS
//#include <gmem.h>
//void c_n_sp_is_temp(void) {
//	{
//		DPSBX(ip_n_sp_is, tco);
//		n_sp_is_1 = IP_1DW(ip_n_sp_is);
//		dri_n_sp_is = IP_1DW(ip_dri_n_sp_is);
//		acin_n_sp_is = IP_1DW(ip_acin_n_sp_is);
//		dri_acin_n_sp_is = IP_1DW(ip_dri_acin_n_sp_is);
//		fac_n_sp_ofs_kwp = IP_1DB(ip_fac_n_sp_ofs_kwp);
//		t_ast_min_n_sp_is_cs = IP_1DW(ip_t_ast_min_n_sp_is_cs);
//		n_sp_is_lih = IP_1DW(ip_n_sp_is_lih);
//		dri_n_sp_is_lih = IP_1DW(ip_dri_n_sp_is_lih);
//	}
//	{
//		DPSBX(ip_n_sp_is_pste, tco);
//		n_sp_is_pste = IP_1DW(ip_n_sp_is_pste);
//		n_sp_is_pste_2 = IP_1DW(ip_n_sp_is_pste_2);
//	}
//	{
//		DPSBX(ip_n_sp_is_cs, toil);
//		n_sp_is_cs = IP_1DW(ip_n_sp_is_cs);
//	}
//	{
//		DPSBX(ip_n_sp_is_toil, toil);
//		n_sp_is_toil = IP_1DW(ip_n_sp_is_toil);
//	}
//	{
//		DPSBX(ip_n_max_tol_st, tco);
//		n_max_tol_st = IP_1DB(ip_n_max_tol_st);
//	}
//}


int main()
{
  i_n_sp_is_rst();
  c_n_sp_is();
  c_n_dif();

  return (0);
}

