/*
 * $Id$
 * Copyright (c) 2006 - IRIT-UPS <casse@irit.fr>
 *
 * HashKey class interface.
 */
#ifndef ELM_UTIL_HASH_KEY_H
#define ELM_UTIL_HASH_KEY_H

#include <elm/types.h>
#include <elm/string.h>
#include <elm/util/Equiv.h>
#include <elm/util/Option.h>

namespace elm {

namespace t { typedef t::intptr hash; }

// Useful hash functions
t::hash hash_string(const char *chars, int length);
t::hash hash_cstring(const char *chars);
t::hash hash_jenkins(const void *block, int size);
bool hash_equals(const void *p1, const void *p2, int size);

// HashKey class
template <class T> class HashKey {
public:
	static t::hash hash(const T& key) { return hash_jenkins(&key, sizeof(T)); };
	static inline bool equals(const T& key1, const T& key2) { return &key1 == &key2 || Equiv<T>::equals(key1, key2); }
};

template <class T> class HashKey<const T&> {
public:
	static t::hash hash(const T& key) { return hash_jenkins(&key, sizeof(T)); };
	static inline bool equals(const T& key1, const T& key2) { return &key1 == &key2 || Equiv<T>::equals(key1, key2); }
};

template <class T> class HashKey<T&> {
public:
	static t::hash hash(const T& key) { return hash_jenkins(&key, sizeof(T)); };
	static inline bool equals(const T& key1, const T& key2) { return &key1 == &key2 || Equiv<T>::equals(key1, key2); }
};

// Predefined hash keys
template <> class HashKey<int> {
public:
	static inline t::hash hash(int key) { return t::hash(key); }
	static inline bool equals(int key1, int key2) { return key1 == key2; }
};

template <> class HashKey<void *> {
public:
	static inline t::hash hash(void *key) { return t::hash(key); }
	static inline bool equals(void *key1, void *key2) { return key1 == key2; }
};

template <> class HashKey<const void *> {
public:
	static inline t::hash hash(const void *key) { return t::hash(key); }
	static inline bool equals(const void *key1, const void *key2)
		{ return key1 == key2; }
};

template <> class HashKey<CString> {
public:
	static t::hash hash(CString key) { return hash_cstring(&key); }
	static inline bool equals(CString key1, CString key2) { return key1 == key2; }
};

template <> class HashKey<String> {
public:
	static t::hash hash(const String& key) { return hash_string(key.chars(), key.length()); };
	static inline bool equals(const String& key1, const String& key2) { return key1 == key2; };
};

// Hasher class
class Hasher {
public:
	inline Hasher(void): h(0) { }
	template <class T> void add(const T& value) { h = h ^ HashKey<T>::hash(value); }
	template <class T> Hasher& operator+=(const T& value) { add<T>(value); return *this; }
	template <class T> Hasher& operator<<(const T& value) { add<T>(value); return *this; }
	inline t::hash hash(void) const { return h; }
	inline operator t::hash(void) const { return h; }
private:
	t::hash h;
};

// SelfHashKey class
template <class T>
class SelfHashKey {
public:
	static t::hash hash(const T& v) { return v.hash(); }
	static bool equals(const T& v1, const T& v2) { return v1 == v2; }
};

};	// elm

#endif	// ELM_UTIL_HASH_KEY_H
