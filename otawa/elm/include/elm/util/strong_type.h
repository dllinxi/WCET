/*
 * $Id$
 * Copyright (c) 2006 - IRIT-UPS <casse@irit.fr>
 *
 * elm/util/StrongType.h -- interface of StrongType class.
 */
#ifndef ELM_UTIL_STRONG_TYPE_H
#define ELM_UTIL_STRONG_TYPE_H

// OTAWA_STRONG_TYPE macro
#define OTAWA_STRONG_TYPE(N, T) \
	typedef struct N { \
		typedef T _t; \
		_t v; \
		inline N(_t _v = 0): v(_v) { }; \
		inline operator const _t&(void) const { return v; } \
		inline operator _t&(void) { return v; } \
		inline struct N& operator=(const _t& _v) { v = _v; return *this; } \
		inline const _t& operator*(void) const { return v; } \
		inline _t& operator*(void) { return v; } \
	} N

// Shrotcut
#ifndef OTAWA_NO_SHORTCUT
#	define STRONG_TYPE(N, T) OTAWA_STRONG_TYPE(N, T)
#endif

#endif // ELM_UTIL_STRONG_TYPE_H

