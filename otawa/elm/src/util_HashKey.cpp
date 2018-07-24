/*
 *	$Id$
 *	HashKey class implementation
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

#include <string.h>
#include <elm/util/HashKey.h>

namespace elm {

/**
 * @class HashKey
 * This is the default implementation of the Hash concept in ELM. It provides
 * a generic Jenkins hash function but provides also easier or more adapted
 * hash functions for curren types like integers, void pointer or strings.
 * 
 * Refer to @ref hash concept for more details.
 * 
 * @ingroup utility
 */ 


/**
 * @class JenkinsHasher
 * Perform compositional hashing of data (for hash table for example) combining hash
 * of simple data using the XOR operation.
 *
 * Although the function @ref hash_jenkins() does the same, it is not recommended
 * to use it as is on custom classes. Depending on the type of attributes,
 * padding bytes may be inserted and not initialized. When the @ref hash_jenkins
 * is called on such an object, this padding bytes will be involved in the hash
 * and therefore, two equal objects (from the logicial point of view of the application)
 * may produce a different hash values.
 *
 * Instead, the best approach is to propose your own hash function and to use this
 * object as an accumulator of the field hash as below:
 *
 * @code
 * 	class MyClass {
 * 		bool f1;
 * 		int f2;
 * 		string f3;
 * 	};
 *
 *	namespace elm {
 * 	template class<> class HashKey<MyClass> {
 * 	public:
 * 		static bool equals(const MyClass& o1, const MyClass& o2) { return o1 == o2; }
 * 		static t::hash hash(const MyClass& o) { return Hasher() << f1 << f2 << f3; }
 * 	};
 * 	}	// elm
 * @endcode
 *
 *  @ingroup utility
 */


/**
 * @class SelfHashKey
 * Simple hash key that looks in the object itself for the hash function
 * and for the quality operator. The T must have the following methods:
 * @li t::hash hash(void);
 * @li bool operator==(const T& v);
 *
 * @param T		Type of hashed objects.
 * @ingroup utility
 */

/**
 * Perform hashing according Jenkins approach
 * (https://en.wikipedia.org/wiki/Jenkins_hash_function).
 * @ingroup utility
 */
t::hash hash_jenkins(const void *block, int size) {
	t::hash hash = 0;
	const unsigned char *p = (const unsigned char *)block;
	for(int i = 0; i < size; i++) {
		hash += p[i];
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}
	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);
	return hash;
}


/**
 * Use a classical compiler string hashing algorithm (see "The Compilers"
 * by Aho, Sethi, Ullman).
 * @ingroup utility
 */
t::hash hash_string(const char *chars, int length) {
	t::hash h = 0, g;
	for(int i = 0; i < length; i++) {
		h = (h << 4) + chars[i];
		if((g = h & 0xf0000000)) {
			h = h ^ (g >> 24);
			h = h ^ g;
		}
	}
	return h;
}


/**
 * Use a classical compiler string hashing algorithm (see "The Compilers"
 * by Aho, Sethi, Ullman).
 * @ingroup utility
 */
t::hash hash_cstring(const char *chars) {
	t::hash h = 0, g;
	for(; *chars; chars++) {
		h = (h << 4) + *chars;
		if((g = h & 0xf0000000)) {
			h = h ^ (g >> 24);
			h = h ^ g;
		}
	}
	return h;
}


/**
 * Test equality of two memory blocks.
 * @param p1	First memory block.
 * @param p2	Second memory block.
 * @param size	Block size.
 * @return		True if they equals, byte to byte, false else.
 * @ingroup utility
 */
bool hash_equals(const void *p1, const void *p2, int size) {
	return !memcmp(p1, p2, size);
}

};	// elm
