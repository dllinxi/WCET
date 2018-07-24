#ifndef PPC_LINUX_ENV_H
#define PPC_LINUX_ENV_H

#include "api.h"
#include "loader.h"

#if defined(__cplusplus)
    extern  "C" {
#endif


/* module name: env */

#define PPC_ENV_STATE
#define PPC_ENV_INIT(s)
#define PPC_ENV_DESTROY(s)


/* system initialization (used internally during platform and state initialization) */
void ppc_stack_fill_env(ppc_loader_t *loader, ppc_platform_t *platform,  ppc_env_t *env);
void ppc_registers_fill_env(ppc_env_t *env, ppc_state_t *state);
void ppc_set_brk(ppc_platform_t *pf, ppc_address_t address);

#if defined(__cplusplus)
}
#endif

#endif /* PPC_LINUX_ENV_H */
