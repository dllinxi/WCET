/*
 * $Id$
 * Copyright (c) 2005, IRIT-UPS.
 *
 * test/plugin.h -- MyPlugin class interface.
 */

#ifndef ELM_TEST_PLUGIN_H
#define ELM_TEST_PLUGIN_H

#include <elm/io.h>
#include <elm/sys/Plugin.h>



// MyPlugin class
class MyPlugin: public elm::sys::Plugin {
protected:
	virtual void startup(void);
	virtual void cleanup(void);
public:
	MyPlugin(void);
	virtual elm::String get(void);
};

#endif // ELM_TEST_PLUGIN_H

