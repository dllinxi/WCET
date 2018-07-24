/*
 * $Id$
 * Copyright (c) 2006, IRIT-UPS.
 *
 * Meta-programming definitions.
 */
#ifndef ELM_META_H
#define ELM_META_H


namespace elm {

// Value pattern
template <int x> struct _n { enum { _ = x }; };


// If pattern
template <bool c, class T, class E> struct _if { typedef T _; };
template <class T, class E> struct _if<false, T, E> { typedef E _; };

}	// elm

#endif // ELM_META_H

