/*
 * $Id
 *
 * @file Plugin linkage in a shared context.
 */

#include "../config.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include "gel_mem.h"
#ifdef CMAKE_FLAG
#	ifdef _WIN32
#		include <windows.h>
#		include <errno.h>
#	else
#		include <dlfcn.h>
#	endif
#else
#	include <ltdl.h>
#endif

#include "gel_private.h"
#include <gel/gel_elf.h>

#define TRACE(stat) /*stat*/


/* externals */
extern arch_plugin_t null_plugin_arch;
extern sys_plugin_t null_plugin_sys;

#ifdef CMAKE_FLAG
#	ifdef _WIN32
#		define DLHANDLE		void *
#		define DLOPEN(P) (void*)LoadLibrary(P)
#		define DLSYM(D,F) (void*)GetProcAddress((HMODULE)D,F)
#		define DLCLOSE(D) FreeLibrary((HMODULE)D)
#	else
#		define	DLHANDLE	void *
#		define	DLOPEN(p)	dlopen(p, RTLD_LAZY)
#		define	DLSYM(h, i)	dlsym(h, i)
#		define	DLCLOSE(h)	dlclose(h)
#	endif
#else
#	define DLHANDLE		lt_dlhandle
#	define DLOPEN(p)	lt_dlopen(p)
#	define DLSYM(h, i)	lt_dlsym(h, i)
#	define DLCLOSE(h)	lt_dlclose(h)
#endif


/**
 * Current kernel version.
 */
int gel_ver = 1;

/**
 * Try to load the system plugin.
 * @param str 	Plugin path.
 * @return		Loaded plugin or NULL if there is an error.
 */
static sys_plugin_t *try_load_sys(char *str)
{
  DLHANDLE h;
  sys_plugin_t *tmp;

  TRACE(printf("try_load_sys(\"%s\")\n", str));

  /* Link the plugin  */
  h = DLOPEN(str);
  if (h == NULL) {
  	TRACE(printf("ERROR: %s\n", lt_dlerror()));
    return NULL;
  }

  /* Look for the hook symbol */
  tmp = DLSYM(h, "plugin_sys");
  if (tmp == NULL) {
  	TRACE(printf("ERROR: no plugin_sys: %s\n", lt_dlerror()));
    DLCLOSE(h);
    return NULL;
  }

  /* Check the version */
  if (tmp->version > gel_ver) {
  	TRACE(printf("ERROR: bad version\n"));
    DLCLOSE(h);
    return NULL;
  }

  /* Record the linkage */
  tmp->dlh = h;
  return tmp;
}


/**
 * Try to load an architecture plugin.
 * @param str	Path to the plugin.
 * @return		Loaded plugin or NULL if there is an error.
 */
static arch_plugin_t *try_load_arch(char *str) {
	DLHANDLE h;
	arch_plugin_t *tmp;

	/* Link the plugin */
	h = DLOPEN(str);
	if (h == NULL)
    	return NULL;

	/* Look for the hook symbol */
	tmp = DLSYM(h, "plugin_arch");
	if (tmp == NULL) {
    	DLCLOSE(h);
		return NULL;
	}

	/* Check the version */
	if (tmp->version > gel_ver) {
		DLCLOSE(h);
		return NULL;
	}

	/* Record the linkage */
	tmp->dlh = h;
	return tmp;
}


/**
 * Load the plugins required by the current binary.
 * @param f 		Platform to fill.
 * @param plugpath	Path list to find the plugins.
 * @param quiet		If not 0, doesn't display any warning.
 */
void gel_load_plugins(gel_platform_t *f, const char *plugpath, int quiet) {
	char *buf = NULL;
	static int ltdl_init = 0;

	TRACE(printf("gel_load_plugins(%p, %s) {\n", f, plugpath));

	/* default initialization */
	f->arch = &null_plugin_arch;
	f->sys = &null_plugin_sys;

	/* Initialize the LTDL library */
#	ifndef CMAKE_FLAG
		if(!ltdl_init)  {
			ltdl_init = 1;
			lt_dlinit();
			atexit((void (*)(void))lt_dlexit);
		}
#	endif

	/* Use default library path if required */
	if (plugpath == NULL || strlen(plugpath) == 0)
		plugpath = PLUGINPATH;

	/* Get the plugin */
	if (plugpath != NULL) {

		/* Allocate the buffer */
		buf = newv(char, strlen(plugpath) + 1 + 128);
		if (buf == NULL)
      		return;

		/* Plug the architecture */
		/* !!TOFIX!! */
#		ifndef CMAKE_FLAG
			sprintf(buf, "%s/libgel_arch_%u.la", plugpath, (int)f->machine);
#		else
			sprintf(buf, "%s/libgel_arch_%u.so", plugpath, (int)f->machine);
#		endif
		TRACE(printf("plugin %s.\n", buf));
	    f->arch = try_load_arch(buf);
    	if (f->arch == NULL) {
    		if(!quiet)
    			fprintf(stderr, "warning: reverting to default arch plugin\n");
	    	f->arch = &null_plugin_arch;
    	}

		/* Plug the system */
		/* !!TOFIX!! */
	    sprintf(buf, "%s/libgel_sys_%u.la", plugpath, (int)f->system);
		TRACE(printf("plugin %s.\n", buf));
    	f->sys = try_load_sys(buf);
	    if (f->sys == NULL) {
	    	if(!quiet)
	    		fprintf(stderr, "warning: reverting to default sys plugin\n");
    		f->sys = &null_plugin_sys;
    	}

		/* Free the buffer */
		delete(buf);
	}

}


/**
 * Unload the plugins.
 * @param pf	Platform to work on.
 */
void gel_unload_plugins(gel_platform_t *pf) {
	if (pf->sys && pf->sys->dlh)
    	DLCLOSE(pf->sys->dlh);
  	if (pf->arch && pf->sys->dlh)
   		DLCLOSE(pf->arch->dlh);
}
