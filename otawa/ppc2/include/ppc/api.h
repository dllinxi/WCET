/* Generated by gep (117/07/07 15:08:27) copyright (c) 2008 IRIT - UPS */
#ifndef GLISS_PPC_INCLUDE_PPC_API_H
#define GLISS_PPC_INCLUDE_PPC_API_H

#include <stdint.h>
#include <stdio.h>

#if defined(__WIN32) || defined(__WIN64)
#	ifdef __cplusplus
#		define __BEGIN_DECLS	extern "C" {
#		define __END_DECLS		}
#	else
#		define __BEGIN_DECLS
#		define __END_DECLS
#	endif
#else
#	include <sys/cdefs.h>
#endif


#include "id.h"
#include "mem.h"
#include "decode.h"

#define PPC_PROC_NAME "PPC"
#define PPC_PC_NAME  NIA

/* return an instruction identifier as a string instead of the ppc_ident_t which is not very user friendly */
char *ppc_get_string_ident(ppc_ident_t id);

/* opaque types */
typedef struct ppc_platform_t ppc_platform_t;
typedef struct ppc_fetch_t ppc_fetch_t;
typedef struct ppc_decoder_t ppc_decoder_t;
struct ppc_loader_t;
typedef struct mask_t mask_t;

/* ppc_state_t type */
typedef struct ppc_state_t {
	ppc_platform_t *platform;
	uint32_t DMISS;
	uint32_t L2CR;
	uint32_t SDR1;
	uint32_t NIA;
	float TMP_FLOTS2;
	uint32_t CTR;
	uint32_t SIA;
	uint32_t IMISS;
	uint32_t SR[16];
	uint8_t CR[8];
	uint32_t IBAT[16];
	uint32_t ICMP;
	uint32_t EAR;
	uint8_t VLE;
	uint32_t DEC;
	uint32_t HASH[2];
	uint32_t DAR;
	uint32_t PMC[4];
	long double TMP_XFLOT;
	double TMP_FLOT1;
	double TMP_FLOT;
	uint32_t SPRG[8];
	uint32_t USIA;
	uint32_t L2PM;
	uint32_t HID[2];
	uint32_t DCMP;
	uint32_t MSR;
	uint32_t UMMCR[2];
	uint32_t UPMC[4];
	uint32_t MMCR[2];
	uint32_t LR;
	int32_t GPR[32];
	uint64_t TB;
	uint32_t RPA;
	uint32_t ICTC;
	uint32_t PVR;
	uint32_t THRM[3];
	uint32_t CIA;
	float TMP_FLOTS;
	uint32_t DBAT[16];
	double FPR[32];
	uint32_t PIA;
	uint32_t XER;
	uint32_t SRR[2];
	uint32_t DABR;
	uint32_t DSISR;
	uint32_t FPSCR;
	uint32_t IABR;
	uint32_t fp_clear;
	float TMP_FLOTS1;
	uint32_t SPEFSCR;

	ppc_memory_t *M;

} ppc_state_t;

/* ppc_sim_t type */
typedef struct ppc_sim_t {
	ppc_state_t *state;
	ppc_decoder_t *decoder;
	/* on libc stripped programs it is difficult to find the exit point, so we specify it */
	ppc_address_t addr_exit;
	/* anything else? */
	int ended;
} ppc_sim_t;


/* ppc_value_t type */
typedef union ppc_value_t {
	int8_t int8;
	uint8_t uint8;
	int16_t int16;
	uint16_t uint16;
	int32_t int32;

} ppc_value_t;


/* ppc_ii_t type */
typedef struct ppc_ii_t {
	ppc_value_t val;
} ppc_ii_t;

/* ppc_inst_t type */
typedef struct ppc_inst_t {
	ppc_ident_t   ident;
	ppc_address_t addr;
	ppc_ii_t instrinput[6];

} ppc_inst_t;

/* auxiliary vector */
typedef struct ppc_auxv_t {
	int	a_type;
	union {
		long a_val;
		void *a_ptr;
		void (*a_fcn)();
	} a_un;
} ppc_auxv_t;

/* environment description */
typedef struct ppc_env_t
{
	int argc;

	char **argv;
	ppc_address_t argv_addr;

	char **envp;
	ppc_address_t envp_addr;

	ppc_auxv_t *auxv;
	ppc_address_t auxv_addr;

	ppc_address_t stack_pointer;
	ppc_address_t brk_addr;
} ppc_env_t;

/* platform management */
#define PPC_MAIN_MEMORY		0
ppc_platform_t *ppc_new_platform(void);
ppc_memory_t *ppc_get_memory(ppc_platform_t *platform, int index);
struct ppc_env_t;
struct ppc_env_t *ppc_get_sys_env(ppc_platform_t *platform);
void ppc_lock_platform(ppc_platform_t *platform);
void ppc_unlock_platform(ppc_platform_t *platform);
int ppc_load_platform(ppc_platform_t *platform, const char *path);
void ppc_load(ppc_platform_t *platform, struct ppc_loader_t *loader);

/* fetching */
ppc_fetch_t *ppc_new_fetch(ppc_platform_t *pf, ppc_state_t *state);
void ppc_delete_fetch(ppc_fetch_t *fetch);

typedef union {
	uint32_t u32;
	mask_t *mask;
} code_t;
ppc_ident_t ppc_fetch(ppc_fetch_t *fetch, ppc_address_t address, code_t *code);

/* decoding */
typedef struct ppc_mode_t {
	const char *name;
	ppc_inst_t *(*decode)(ppc_decoder_t *decoder, ppc_address_t address);
} ppc_mode_t;
extern ppc_mode_t ppc_modes[];

ppc_decoder_t *ppc_new_decoder(ppc_platform_t *pf);
void ppc_delete_decoder(ppc_decoder_t *decoder);
ppc_inst_t *ppc_decode(ppc_decoder_t *decoder, ppc_address_t address);
void ppc_free_inst(ppc_inst_t *inst);
/* only used if several ISS defined to fully initialize decoder structure,
 * does nothing if one inst set only is defined */
void ppc_set_cond_state(ppc_decoder_t *decoder, ppc_state_t *state);
/* decoding functions for one specific instr set */
/* decoding function for instr set 0, named PPC */
ppc_inst_t *ppc_decode_PPC(ppc_decoder_t *decoder, ppc_address_t address);
/* decoding function for instr set 1, named VLE */
ppc_inst_t *ppc_decode_VLE(ppc_decoder_t *decoder, ppc_address_t address);
unsigned long ppc_get_inst_size(ppc_inst_t* inst);
unsigned long ppc_get_inst_size_from_id(ppc_ident_t id);


/* code execution */
void ppc_execute(ppc_state_t *state, ppc_inst_t *inst);

/* state management function */
ppc_state_t *ppc_new_state(ppc_platform_t *platform);
void ppc_delete_state(ppc_state_t *state);
ppc_state_t *ppc_copy_state(ppc_state_t *state);
ppc_state_t *ppc_fork_state(ppc_state_t *state);
void ppc_dump_state(ppc_state_t *state, FILE *out);
ppc_platform_t *ppc_platform(ppc_state_t *state);

/* simulation functions */
ppc_sim_t *ppc_new_sim(ppc_state_t *state, ppc_address_t start_addr, ppc_address_t exit_addr);
ppc_inst_t *ppc_next_inst(ppc_sim_t *sim);
uint64_t ppc_run_and_count_inst(ppc_sim_t *sim);
void ppc_run_sim(ppc_sim_t *sim);
void ppc_step(ppc_sim_t *sim);
void ppc_delete_sim(ppc_sim_t *sim);
ppc_address_t  ppc_next_addr(ppc_sim_t *sim);
void ppc_set_next_address(ppc_sim_t *sim, ppc_address_t address);
#define ppc_set_entry_address(sim, addr) ppc_set_next_address(sim, addr)
void ppc_set_exit_address(ppc_sim_t *sim, ppc_address_t address);
#define ppc_set_sim_ended(sim) (sim)->ended = 1
#define ppc_is_sim_ended(sim) ((sim)->ended)

/* disassemble function */
void ppc_disasm(char *buffer, ppc_inst_t *inst);
typedef char *(*ppc_label_solver_t)(ppc_address_t address);
extern ppc_label_solver_t ppc_solve_label;
char *ppc_solve_label_null(ppc_address_t address);

/* variable length functions */
uint32_t value_on_mask(mask_t *inst, mask_t *mask);

#endif /* GLISS_PPC_INCLUDE_PPC_API_H */
