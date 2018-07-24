/*
 *	$Id$
 *	Plugin class implementation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2005-07, IRIT UPS.
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

#include <stdio.h>
#if defined(__unix) || defined(__APPLE__)
#	include "../config.h"
#elif defined(__WIN32) || defined(__WIN64)
#	include "../config.h"
#endif

#include <elm/assert.h>
#ifdef WITH_LIBTOOL
#	include <ltdl.h>
#elif defined(__WIN32) || defined(__WIN64)
#	include <windows.h>
#	undef min
#	undef max
#else
#	include <dlfcn.h>
#endif
#include <elm/sys/Plugin.h>
#include <elm/sys/Plugger.h>

namespace elm { namespace sys {

/* Maximal age for unused plugins before deletion
 * (expressed in plugin getting cycles).
 */
#define MAX_AGE	16


/**
 * @defgroup plugins Plugins
 *
 * @section use_plugins Using a Plugin
 *
 * ELM provides classes to implement plugins in an OS-indepedent way.
 * To work-around found on some OS, ELM provides also an independent way
 * to optionaly represent dependencies between plugins based on textual file
 * with ".ini" syntax.
 *
 * A plugin is a piece of code that
 * provides some service while the plugger allows to plug many different
 * plugins to implement this service.
 *
 * In ELM, the plugger is declared with a hook - an identifier allowing
 * an application to have different kinds of services and matching plugins -
 * and a version for the current implemented plugin interface. For example,
 * below, the plugger provides "net_plugin" services at version 1.0.0 (see
 * @ref elm::Version for more details). We may also gives a list of directories
 * to look to find the plugin.
 * @code
 * #define NET_HOOK	net_plugin
 * elm::system::Plugger net_plugger(#NET_HOOK, Version(1, 0, 0), ".:/usr/local/net_plugins");
 * @endcode
 *
 * To open a specific plugin named, for example, "my_net_plugin", we have only
 * to call the method Plugger::plug().
 * @code
 * elm::system::Plugin *plugin = net_plugger.plug("my_net_plugin");
 * @endcode
 * If the plugin is found, is loadable and has a compatible version, it is loaded,
 * initialized and its instance is returned. If there is an error, a null
 * pointer is returned and the method Plugger::lastErrorMessage() may give
 * some information about the error.
 * @code
 * if(plugin == NULL)
 * 	cout << "ERROR: " << plugger.lastErrorMessage() << io::endl;
 * @endcode
 *
 * Having just a plugin pointer does not provide any service. To do this, we have
 * usually to define an interface that is implemented by the plugin instance.
 * Let's call it "NetPlugin": it must be derived from the elm::system::Plugin class.
 * Notice that the interface pass the right name of the service but should not
 * pass the version. Otherwise, the service version would be ever compatible
 * while the actual binary would be compiled for an incompatible version.
 * @code
 * class NetPlugin: public Plugin {
 * public:
 *	NetPlugin(const make& maker): Plugin(make(name, version))
 *		{ }
 * 	virtual void performService(void) = 0;
 * };
 * @endcode
 * Then, we can cast the plugin to its required interface and we can call the
 * service:
 * @code
 * NetPlugin *net_plugin = static_cast<NetPlugin *>(plugin);
 * net_plugin->performService();
 * @endcode
 *
 * When the plugin is no more needed, we can simply unplug it:
 * @code
 * net_plugin->unplug();
 * @endcode
 *
 * @section creating_plugins Creating Plugins
 *
 * To create a plugin, we have to define a class implementing the service
 * interface of the plugger to plug to. Let's continue the example started
 * in the previous section. Notice that we must implement
 * the pure virtual functions of the interface in order to be able to create
 * an instance of the plugin class.
 * @code
 * class MyNetPlugin: public NetPlugin {
 * public:
 *		MyNetPlugin(void);
 * 		virtual void performService(void);
 * };
 * @endcode
 *
 * Then, the constructor must pass to the elm::system::Plugin class the name
 * of the service to be hooked to the right plugger with the service version
 * supported by the plugin.
 * @code
 * MyNetPlugin::MyNetPlugin(void): NetPlugin(make("my_net_plugin", Version(1, 0, 0))) {
 * }
 * @endcode
 * Notice how the version is encoded in the plugin code. If this plugin is then
 * used with in incompatible version of the plugger, say 1.2.0. The compatibility
 * comparison will compare the version 1.0.0 (from the plugin) with the version
 * 1.2.0 (from the plugger) and will detect possible binary incompatibility and
 * prevent the invalid linkage.
 *
 * Then we need to provide a hook to let the plugger found the plugin object.
 * @code
 * ELM_PLUGIN(MyNetPlugin, NET_HOOK);
 * @endcode
 * The result source must then be compiled to produce a shared code with
 * your preferred compiler For example, with GCC on a Unix OS.
 * @code
 * gcc -shared -o my_net_plugin.so my_net_plugin.cpp
 * @endcode
 *
 * @section eld_files ELD Files
 *
 * ELM Linkage Description (ELD) files allows to circumvent limitations found on some OSes.
 * Basically, ELD files are textual files with ".eld" extension implementing the ".ini" file
 * syntax. They must placed in the same directory as the plugin they apply to and get the same
 * name as the plugin with the plugin extension replaced by ".eld".
 *
 * As usual ".ini" files, they can contain as many sections as possible but ELM is only interested
 * in the "elm-plugin" section that may contain the following definitions:
 * @li "author" -- author of the plugin in the form "AUTHOR <EMAIL>",
 * @li "copyright" -- name and, optionally, link to the license,
 * @li "deps" -- ";" separated list of plugin names to load before the current plugin.
 * @li "description" -- description of the plugin for human user,
 * @li "libs" -- ";" separated list of library names or paths (absolute or relative) to load before the current plugin,
 * @li "name" -- alternative name of the plugin,
 * @li "path" -- absolute or relative path to the actual plugin (support for aliasing),
 * @li "rpaths" -- ";" separated list of paths to look for require plugins,
 * @li "site" -- URL to the website publising the plugin.
 *
 * "path" definition allows to cope with the lack of support for symnolic links on some OSes.
 *
 * "rpaths" and "libs" are used to handle easily dependencies between plugins and libraries when the OSes
 * does not support linkage lookup path encoded in executable.
 *
 * Paths found in "path", "libs" and "rpaths" can be prefixed by "$ORIGIN" that is replaced,
 * at run time, by the path of the directory containing the considered plugin.
 *
 * Below is an example of ELD files for our example requiring two other libraries that should be found
 * in the directory containing the plugin or in directory "/usr/lib".
 * It is named "net_plugin.eld" and put in the same directory
 * as "net_plugin.so".
 * @code
 * [elm-plugin]
 * libs=libxml2;libxslt
 * rpath=$ORIGIN;/usr/lib
 * @endcode
 *
 * Below another example allows to have an alias, "net_plugin_v2" to "net_plugin". The file is named
 * "net_plugin_v2.eld":
 * @code
 * [elm-plugin]
 * path=$ORIGIN/net_plugin
 * @endcode
 *
 * This last example links with library libxml2 from the OS path:
 * @code
 * [elm-plugin]
 * libs=libxml2
 * @endcode
 *
 * @section plugin_old_style Old-Style Plugin Creation
 *
 * The old-style plugin creation remains supported but is marked as deprecated.
 *
 * Here, the plugin of the example must be declared with:
 * @code
 * class MyNetPlugin: public NetPlugin {
 * public:
 *		MyNetPlugin(void): NetPlugin("my_net_plugin", Version(1, 0, 0)) {
 *			_description = "my plugin";
 *			_licence = "my licence";
 *		}
 * 		virtual void performService(void);
 * };
 * @endcode
 * Notice how description and license were passed in the constructor.
 *
 * The second difference stands in the way to declare the hook (it has proven to
 * no robust enough):
 * @code
 * MyNetPlugin NET_HOOK;
 * MyNetPlugin& my_net_plugin = NET_HOOK;
 * @endcode
 */

/**
 * @class Plugin <elm/system/Plugin.h>
 * The class Plugin allows implementing plug-ins in OS-independent way. See
 * @ref plugins for more details.
 * @ingroup plugins
 */


/**
 * List of static plugins.
 */
genstruct::Vector<Plugin *> Plugin::static_plugins;


/**
 * List of unused plugins.
 */
genstruct::Vector<Plugin *> Plugin::unused_plugins;


/**
 * Build a new plugin.
 * @param name				Plugin name.
 * @param plugger_version	Plugger version (used for checking compatibility
 * 							between plugin and user application API).
 * @param hook				Hook of a matching plugger
 * @param aliases			Name aliases for the plugin.
 */
Plugin::Plugin(
	string name,
	const Version& plugger_version,
	CString hook,
	const aliases_t& aliases
) :
	magic(MAGIC),
	_hook(hook),
	_name(name),
	per_vers(plugger_version),
	_handle(0),
	state(0)
{
	if(hook)
		static_plugins.add(this);
	_aliases.copy(aliases);
}


/**
 * New-style builder for plugin using @ref Plugin::Maker.
 * @param maker	Maker for plugin.
 */
Plugin::Plugin(const Plugin::make& maker)
:	magic(MAGIC),
 	_description(maker._description),
 	_licence(maker._license),
 	_plugin_version(maker._plugin_version),
	_hook(maker._hook),
 	_name(maker._name),
 	per_vers(maker._plugger_version),
 	_handle(0),
 	state(0)
{
	if(_hook)
		static_plugins.add(this);
	if(maker.aliases) {
		genstruct::Vector<string> as;
		as.addAll(maker.aliases);
		_aliases = as.detach();
	}
}

/**
 */
Plugin::~Plugin(void) {
}


/**
 * For internal use only.
 */
void Plugin::plug(void *handle) {
	// Usage incrementation
	if(state > 0)
		state++;

	// Initialization
	else if(state == 0) {
		startup();
		state = 1;
		if(handle) {
#if defined(__unix)
			_handle = handle;
#elif defined(__WIN32) || defined(__WIN64)
			_handle = reinterpret_cast<HMODULE&>(handle);
#endif

			static_plugins.remove(this);
		}
	}

	// Revival for unused plugins
	else if(state < 0) {
		unused_plugins.remove(this);
		state = 1;
	}

	// Make other older
	step();
}


/**
 * Call it when the plugin is no more used.
 */
void Plugin::unplug(void) {
	ASSERT(state);

	// Decrement usage
	state--;

	// Plugin become unused ?
	if(!state) {
		unused_plugins.add(this);
		state = -1;
	}

	// Unused plugin become too old ?
	else if(state < -MAX_AGE) {
		Plugger::leave(this);
		cleanup();
		unused_plugins.remove(this);
		if(_handle)
			Plugger::unlink(_handle);
	}
}


/**
 * This method is called just after the plugin has been plugged into the user
 * application. It may be overriden for performing specific initializations.
 */
void Plugin::startup(void) {
}


/**
 * This method is called just before the plugin removal from the user
 * application. It may be overriden for performing specific clean up.
 */
void Plugin::cleanup(void) {
}


/**
 * @fn string Plugin::name(void) const;
 * Get the plugin name.
 * @return	Plugin name.
 */


/**
 * @fn String Plugin::description(void) const;
 * Get the plugin description.
 * @return	Plugin description.
 */


/**
 * @fn String Plugin::licence(void) const;
 * Get the plugin licence.
 * @return Plugin licence.
 */


/**
 * @fn Version& Plugin::pluginVersion(void) const;
 * Get the plugin version.
 * @return Plugin version.
 */


/**
 * @fn Version& Plugin::pluggerVersion(void) const;
 * Get the plugger version.
 * @return Plugger version.
 */


/**
 * @fn String Plugin::hook(void) const;
 * Get the hook of the current plugin.
 * @return	Plugin hook.
 */


/**
 * For internal use only.
 */
void Plugin::step(void) {
	for(int i = unused_plugins.count() - 1; i >= 0; i--)
		unused_plugins[i]->unplug();
}


/**
 * For internal use only.
 */
Plugin *Plugin::get(cstring hook, const string& name) {

	// Find the plugin
	for(int i = 0; i < static_plugins.count(); i++)
		if(static_plugins[i]->hook() == hook
		&& static_plugins[i]->matches(name))
			return static_plugins[i];

	// Not found
	return 0;
}


/**
 * @fn const aliases_t& Plugin::aliases(void) const;
 * Get the table of aliases of the plugin.
 * @return	Table of aliases.
 */


/**
 * Test if the current plugin matches the given name, that is, if the name
 * or one of the aliases matches the name.
 * @param name	Name to test.
 * @return		True if the name is matched, false else.
 */
bool Plugin::matches(const string& name) const {
	if(_name == name)
		return true;
	for(int i = 0; i < _aliases.count(); i++)
		if(name == _aliases[i])
			return true;
	return false;
}

} } // elm::sys
