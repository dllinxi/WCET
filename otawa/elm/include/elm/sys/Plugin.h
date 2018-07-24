/*
 *	Plugin class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2005-12, IRIT UPS.
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
#ifndef ELM_SYS_PLUGIN_H
#define ELM_SYS_PLUGIN_H

#include <elm/types.h>
#include <elm/string.h>
#include <elm/data/List.h>
#include <elm/genstruct/Vector.h>
#include <elm/genstruct/Table.h>
#include <elm/util/Version.h>
#include <elm/sys/Path.h>

namespace elm { namespace sys {

#define ELM_PLUGIN_ID_PREFIX		"@@ELM-PLUGIN-ID@@:"
#define ELM_PLUGIN_CONCAT_AUX(x, y)	x ## y
#define ELM_PLUGIN_CONCAT(x, y)		ELM_PLUGIN_CONCAT_AUX(x, y)
#define ELM_PLUGIN_ID(hook, info)	const char ELM_PLUGIN_CONCAT(__plugin_id_, __LINE__)[] = ELM_PLUGIN_ID_PREFIX hook ":" info;
#define ELM_PLUGIN(plugin, hook) 	extern "C" { \
	static ELM_PLUGIN_ID(#hook, #plugin); \
	elm::sys::Plugin *ELM_PLUGIN_CONCAT(hook, _fun)(void) { return static_cast<elm::sys::Plugin *>(&plugin); } }

// Plugin class
class Plugin {
	t::uint32 magic;
public:
	static const t::uint32 MAGIC = 0xCAFEBABE;
	typedef genstruct::Table<string> aliases_t;

	class make {
		friend class Plugin;
	public:
		inline make(string name, const Version& plugger_version)
			: _name(name), _plugger_version(plugger_version) { }
		inline make& description(cstring d) { _description = d; return *this; }
		inline make& license(cstring l) { _license = l; return *this; }
		inline make& version(const Version& v) { _plugin_version = v; return *this; }
		inline make& alias(string a) { aliases.add(a); return *this; }
		inline make& hook(cstring h) { _hook = h; return *this; }
	private:
		string _name;
		cstring _description, _license, _hook;
		Version _plugin_version, _plugger_version;
		genstruct::Vector<string> aliases;
	};

	Plugin(string name, const Version& plugger_version, CString hook = "", const aliases_t& aliases = aliases_t::EMPTY);
	Plugin(const make& maker);
	virtual ~Plugin(void);
	inline string name(void) const { return _name; }
	inline CString description(void) const;
	inline CString licence(void) const;
	inline const Version& pluginVersion(void) const;
	inline const Version& pluggerVersion(void) const;
	inline CString hook(void) const;
	inline const aliases_t& aliases(void) const;
	bool matches(const string& name) const;
	void unplug(void);
	inline const Path& path(void) const { return _path; }

	typedef List<Plugin *>::Iter DepIter;
	inline DepIter dependencies(void) const { return DepIter(deps); }

protected:
	virtual void startup(void);
	virtual void cleanup(void);

	CString _description;
	CString _licence;
	Version _plugin_version;

private:
	void plug(void *handle);
	static void step(void);
	static Plugin *get(cstring hook, const string& name);
	void setPath(const Path& path) { _path = path; }

	friend class Plugger;
	static genstruct::Vector<Plugin *> static_plugins;
	static genstruct::Vector<Plugin *> unused_plugins;
	CString _hook;
	string _name;
	Version per_vers;
	void *_handle;
	int state;
	genstruct::DeletableTable<string> _aliases;
	Path _path;
	List<Plugin *> deps;
};

// Inlines
inline CString Plugin::description(void) const {
	return _description;
}

inline CString Plugin::licence(void) const {
	return _licence;
}

inline const Version& Plugin::pluginVersion(void) const {
	return _plugin_version;
}

inline const Version& Plugin::pluggerVersion(void) const {
	return per_vers;
}

inline CString Plugin::hook(void) const {
	return _hook;
}

inline const Plugin::aliases_t& Plugin::aliases(void) const {
	return _aliases;
}

} } // elm::sys

#endif // ELM_SYS_PLUGIN_H
