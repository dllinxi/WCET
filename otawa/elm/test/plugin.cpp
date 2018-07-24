/*
 *	Plugin class implementation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2006-07, IRIT UPS.
 *
 *	OTAWA is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	OTAWA is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with OTAWA; if not, write to the Free Software
 *	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include <elm/io.h>
#include <elm/sys/Plugin.h>
#include "plugin.h"

using namespace elm;
using namespace elm::sys;

#if !defined(__WIN32) && !defined(__WIN64)
extern bool startup_flag;
extern bool cleanup_flag;
#endif

// MyPlugin class
void MyPlugin::startup(void) {
	cout << "Startup performed !\n";
#if !defined(__WIN32) && !defined(__WIN64)
	startup_flag = true;
	cleanup_flag = false;
#endif
}

void MyPlugin::cleanup(void) {
	cout << "Cleanup performed !\n";
#if !defined(__WIN32) && !defined(__WIN64)
	startup_flag = false;
	cleanup_flag = true;
#endif
}

elm::String MyPlugin::get(void) {
	return "hello";
}

/*MyPlugin::MyPlugin(void): Plugin("myplugin", elm::Version(0, 0, 0)) {
	_description = "myplugin description";
	_licence = "myplugin licence";
	_plugin_version = Version(1, 2, 3);
}*/
MyPlugin::MyPlugin(void): Plugin(
	make("myplugin", Version(0, 0, 0))
	.description("myplugin description")
	.license("myplugin licence")
	.version(Version(1, 2, 3))) { }

// Plugin hook
MyPlugin my_plugin;
#define HOOK	my_plugin
ELM_PLUGIN(my_plugin, HOOK);

