/*
 *	Path class interface
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
#ifndef ELM_SYS_PLUGGER_H
#define ELM_SYS_PLUGGER_H

#include <elm/PreIterator.h>
#include <elm/sys/Plugin.h>
#include <elm/genstruct/Vector.h>
#include <elm/sys/Directory.h>
#include <elm/sys/Path.h>
#include <elm/util/ErrorHandler.h>

namespace elm { namespace sys {

// Plugger class
class Plugger: public ErrorBase {
	friend class Plugin;
	friend class PluginManager;
public:
	typedef enum error_t {
		OK = 0,
		NO_PLUGIN,
		NO_HOOK,
		BAD_VERSION,
		NO_MAGIC,
		BAD_PLUGIN,
		MISSING_DEP
	} error_t;

public:
	Plugger(CString hook, const Version& plugger_version, String paths = "*");
	virtual ~Plugger(void);
	void addPath(String path);
	void removePath(String path);
	void resetPaths(void);
	Plugin *plug(const string& path);
	Plugin *plugFile(sys::Path path);
	inline String hook(void) const;
	string getLastError(void);
	inline bool quiet(void) const { return _quiet; }
	inline void setQuiet(bool quiet) { _quiet = quiet; }

	// deprecated
	virtual void onError(String message);
	virtual void onWarning(String message);
	error_t lastError(void);
	String lastErrorMessage(void);

	// Iterator class
	class Iterator: public PreIterator<Iterator, String> {
	public:
		Iterator(Plugger& _plugger);
		~Iterator(void);
		bool ended(void) const;
		String item(void) const;
		void next(void);
		Plugin *plug(void) const;
		Path path(void) const;
	private:
		Plugger& plugger;
		genstruct::Vector<Plugin *>& statics;
		int i, c;
		int _path;
		Directory::Iterator *file;
		void go(void);
	};

	class PathIterator: public genstruct::Vector<string>::Iterator {
	public:
		inline PathIterator(const Plugger& plugger)
			: genstruct::Vector<string>::Iterator(plugger.paths) { }
		inline PathIterator(const PathIterator& iter)
			: genstruct::Vector<string>::Iterator(iter) { }
	};

private:

	static genstruct::Vector<Plugger *> pluggers;
	CString _hook;
	Version per_vers;
	genstruct::Vector<Plugin *> plugins;
	genstruct::Vector<String> paths;
	error_t err;
	bool _quiet;
	static void leave(Plugin *plugin);
	Plugin *plug(Plugin *plugin, void *handle);
	inline genstruct::Vector<Plugin *>& statics(void);
	void onError(error_level_t level, const string& message);
	Plugin *lookELD(const Path& path, error_t& err, genstruct::Vector<Plugin *>& deps);

	// portability functions
	static void *link(sys::Path lib);
	static void unlink(void *handle);
	static void *lookSymbol(void *handle, cstring hook);
	static void *lookLibrary(sys::Path lib, genstruct::Vector<string> rpath);
	static string error(void);
};

// Inlines
inline String Plugger::hook(void) const {
	return _hook;
}

inline genstruct::Vector<Plugin *>& Plugger::statics(void) {
	return Plugin::static_plugins;
}

} } // elm::sys

#endif // ELM_SYS_PLUGGER_H
