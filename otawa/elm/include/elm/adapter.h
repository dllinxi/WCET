/*
 *	adapter module interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2008, IRIT UPS.
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
#ifndef ELM_ADAPTER_H_
#define ELM_ADAPTER_H_

#include <elm/util/Pair.h>

namespace elm {

// Id adapter
template <class T>
class IdAdapter {
public:
	typedef T key_t;
	typedef T val_t;
	typedef T data_t;
	static inline const T& key(const data_t& v) { return v; }
	static inline const T& value(const data_t& v) { return v; }
	static inline T& ref(data_t& v) { return v; }
};


// Pair adapter
template <class K, class T>
class PairAdapter {
public:
	typedef K key_t;
	typedef T val_t;
	typedef Pair<K, T> data_t;
	static inline const key_t& key(const data_t& v) { return v.fst; }
	static inline const val_t& value(const data_t& v) { return v.snd; }
	static inline val_t& ref(data_t& v) { return v.snd; }
};

}	// elm

#endif /* ELM_ADAPTER_H_ */
