/* Generated by gep (117/07/07 15:08:27) copyright (c) 2011 IRIT - UPS */
#ifndef GLISS_PPC_PPC_DEBUG_H
#define GLISS_PPC_PPC_DEBUG_H

#include "api.h"

__BEGIN_DECLS

/* register type */
typedef enum {
	RTYPE_NONE = 0,
	RTYPE_BITS,
	RTYPE_ADDR,
	RTYPE_INT,
	RTYPE_FLOAT
} register_type_t;

/* register field description */
typedef struct {
	const char *name;
	int hbit, lbit;
} register_field_t;

/* register bank description */ 
typedef struct register_bank_t {
	int id;
	const char *name;
	const char *format;
	int size;
	register_type_t type;
	int tsize;
	union {
		register_field_t fields;
	} data;
} register_bank_t;

/* register value */
typedef union {
	int32_t iv;
	int64_t lv;
	float fv;
	double dv;
} register_value_t;

/* IRQ functions */
typedef struct ppc_exception_t {
	const char *name;
	void (*fun)(ppc_state_t *state);
	int is_irq;
} ppc_exception_t;
	

/* functions */
register_bank_t *ppc_get_registers(void);
register_value_t ppc_get_register(ppc_state_t *state, int id, int index);
void ppc_set_register(ppc_state_t *state, int id, int index, register_value_t value);
ppc_exception_t *ppc_exceptions(void);

/* macros avoiding customisation in debugger */
#define debug_get_registers()				ppc_get_registers()
#define debug_get_register(s, id, idx)		ppc_get_register(s, id, idx)
#define debug_set_register(s, id, idx, v)	ppc_set_register(s, id, idx, v)
#define debug_exceptions					ppc_exceptions()

__END_DECLS

#endif	// GLISS_PPC_PPC_DEBUG_H
