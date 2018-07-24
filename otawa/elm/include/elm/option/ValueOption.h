/*
 *	$Id$
 *	ValueOption class interface
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
#ifndef ELM_OPTION_VALUEOPTION_H_
#define ELM_OPTION_VALUEOPTION_H_

#include <elm/option/StandardOption.h>
#include <elm/type_info.h>

namespace elm { namespace option {

// AbstractValueOption class
class AbstractValueOption: public Option {
public:
	class Make: public Option::Make {
		friend class AbstractValueOption;
	public:
		inline Make(Manager *man): Option::Make(man), _usage(arg_required) { }
		inline Make(Manager& man): Option::Make(man), _usage(arg_required) { }
		inline Make& argDescription(cstring s) { arg_desc = s; return *this; }
		inline Make& usage(usage_t u) { _usage = u; return *this; }
		inline Make& cmd(string c) { Option::Make::cmd(c); return *this; }
		inline Make& description(cstring d) { Option::Make::description(d); return *this; }
	private:
		cstring arg_desc;
		usage_t _usage;
	};

	AbstractValueOption(const Make& make);
	AbstractValueOption(Manager& man);
	AbstractValueOption(Manager& man, int tag, ...);
	AbstractValueOption(Manager& man, int tag, VarArg& args);

	// Option overload
	virtual cstring description(void);
	virtual usage_t usage(void);
	virtual cstring argDescription(void);

protected:
	virtual void configure(Manager& manager, int tag, VarArg& args);

private:
	cstring arg_desc;
	usage_t use;
};


// useful inline
template <class T>
inline T read(string arg) { T val; arg >> val; return val; }


// ValueOption<T> class
template <class T>
class ValueOption: public AbstractValueOption {
public:
	class Make: public AbstractValueOption::Make {
		friend class ValueOption<T>;
	public:
		inline Make(Manager *man): AbstractValueOption::Make(man) { }
		inline Make(Manager& man): AbstractValueOption::Make(man) { }
		inline Make& argDescription(cstring s) { AbstractValueOption::Make::argDescription(s); return *this; }
		inline Make& usage(usage_t u) { AbstractValueOption::Make::usage(u); return *this; }
		inline Make& cmd(string c) { Option::Make::cmd(c); return *this; }
		inline Make& description(cstring d) { Option::Make::description(d); return *this; }
		inline Make& def(const T& d) { _def = d; return *this; }
	private:
		T _def;
	};

	inline ValueOption(void) { }

	inline ValueOption(const Make& make): AbstractValueOption(make), val(make._def) { }

	inline ValueOption(Manager& man, int tag ...)
		: AbstractValueOption(man)
		{ VARARG_BEGIN(args, tag) AbstractValueOption::init(man, tag, args); VARARG_END }

	inline ValueOption(Manager& man, int tag, VarArg& args)
		: AbstractValueOption(man)
		{ AbstractValueOption::init(man, tag, args); }

	inline ValueOption(Manager& man, char s, cstring desc, cstring adesc, const T& val = type_info<T>::null)
		: AbstractValueOption(man)
		{ init(man, short_cmd, s, option::description, &desc, option::arg_desc, &adesc, def, val, end); }

	inline ValueOption(Manager& man, cstring l, cstring desc, cstring adesc, const T& _val = type_info<T>::null)
		: AbstractValueOption(Make(man).cmd(l).description(desc).argDescription(adesc)), val(_val) { }

	inline ValueOption(Manager& man, char s, cstring l, cstring desc, cstring adesc, const T& _val = type_info<T>::null)
		: AbstractValueOption(Make(man).cmd(l).cmd(_ << '-' << s).description(desc).argDescription(adesc)), val(_val) { }

	inline const T& get(void) const { return val; };
	inline void set(const T& value) { val = value; };

	// Operators
	inline operator const T&(void) const { return get(); };
	inline ValueOption<T>& operator=(const T& value) { set(value); return *this; };
	inline const T& operator*(void) const { return get(); }
	inline operator bool(void) const { return get(); }

	// Option overload
	virtual void process(String arg) { val = read<T>(arg); }

	// deprecated
	inline const T& value(void) const { return val; };

protected:
	virtual void configure(Manager& manager, int tag, VarArg& args) {
		switch(tag) {
		case def: val = get(args); break;
		default: AbstractValueOption::configure(manager, tag, args); break;
		}
	}

private:
	T val;
	inline T get(VarArg& args) { return args.next<T>(); }
};


// alias
template <class T> class Value: public ValueOption<T> {
public:
	inline Value(const typename ValueOption<T>::Make& make): ValueOption<T>(make) { }
};

// string specialization
template <>
inline ValueOption<string>::ValueOption(Manager& man, char s, cstring desc, cstring adesc, const string& value)
	: AbstractValueOption(man)
	{ AbstractValueOption::init(man, short_cmd, s, option::description, &desc, option::arg_desc, &adesc, def, value.chars(), end); }

template <>
inline ValueOption<string>::ValueOption(Manager& man, cstring l, cstring desc, cstring adesc, const string& val)
	: AbstractValueOption(man)
	{ init(man, long_cmd, &l, option::description, &desc, option::arg_desc, &adesc, def, &val, end); }

template <>
inline ValueOption<string>::ValueOption(Manager& man, char s, cstring l, cstring desc, cstring adesc, const string& val)
	: AbstractValueOption(man)
	{ init(man, short_cmd, s, long_cmd, &l, option::description, &desc, option::arg_desc, &adesc, def, &val, end); }


// get specialization
template <> inline string ValueOption<string>::get(VarArg& args) { return args.next<const char *>(); }


// read specializations
template <> string read<string>(string arg);
template <> cstring read<cstring>(string arg);

} }	// elm::option

#endif /* ELM_OPTION_VALUEOPTION_H_ */
