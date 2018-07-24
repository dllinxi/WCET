/*
 * $Id
 *
 * @file Plugin linkage for a specialized PPC library.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
/*#include <ltdl.h>*/

#include "gel_private.h"
#include <gel/gel_elf.h>
#include "../config.h"

#define TRACE(stat) /*{ printf("%s:%d: ", __FILE__, __LINE__); stat; }*/

extern arch_plugin_t *plugin_arch;
extern sys_plugin_t *plugin_sys;


/**
 */
void gel_load_plugins(gel_platform_t *f, const char *plugpath, int quiet) {
	TRACE(printf("gel_load_plugins(%p, \"%s\")\n", f, plugpath));
	if(plugin_arch->machine == f->machine)
		f->arch = plugin_arch;
	else
		f->arch = 0;
	f->sys = plugin_sys;
}


/**
 */
void gel_unload_plugins(gel_platform_t *pf) {
	pf->arch = 0;
	pf->sys = 0;
}
