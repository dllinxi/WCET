/*
 * $Id$
 * Copyright (c) 2005, IRIT-UPS Corporation.
 *
 * util.VarArg.h -- VarArg class interface.
 */
#ifndef ELM_UTIL_VARARG_H
#define ELM_UTIL_VARARG_H

#include <stdarg.h>

namespace elm {

// Macros
#define VARARG_BEGIN(name, arg) \
	{ \
		va_list __args; \
		va_start(__args, arg); \
		elm::VarArg name(__args);
#define VARARG_END \
		va_end(__args); \
	}

// VarArg class
class VarArg {
	va_list& _args;
public:
	inline VarArg(va_list& args);
	inline VarArg(const VarArg& args);
	template <class T> T next(void);
	inline va_list& args(void);
};

// Inlines
inline VarArg::VarArg(va_list& args): _args(args) {
}

inline VarArg::VarArg(const VarArg& args): _args(args._args) {
}

template <class T>
T VarArg::next(void) {
	return va_arg(_args, T);
}

inline va_list& VarArg::args(void) {
	return _args;
}

// Useful overload
template <> inline bool VarArg::next(void) {
	return (bool)va_arg(_args, int);
};

template <> inline char VarArg::next(void) {
	return (char)va_arg(_args, int);
};

template <> inline short VarArg::next(void) {
	return (short)va_arg(_args, int);
};

template <> inline unsigned char VarArg::next(void) {
	return (unsigned char)va_arg(_args, unsigned int);
};

template <> inline unsigned short VarArg::next(void) {
	return (unsigned short)va_arg(_args, unsigned int);
};

} // elm

#endif // ELM_UTIL_VARARG_H
