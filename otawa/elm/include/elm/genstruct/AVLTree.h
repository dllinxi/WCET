/*
 *	Deprecated header.
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2008-13, IRIT UPS.
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
#ifndef ELM_GENSTRUCT_AVLTREE_H
#define ELM_GENSTRUCT_AVLTREE_H

#include <elm/avl/Set.h>

namespace elm { namespace genstruct {

class AbstractAVLTree: public avl::AbstractTree {
protected:
	inline AbstractAVLTree(void) { }
};

template <class T, class K = IdAdapter<T>, class C = elm::Comparator<typename K::t> >
class GenAVLTree: public avl::GenTree<T, K, C> {
};

template <class T, class C = elm::Comparator<T> >
class AVLTree: public avl::Set<T, C> {
};

} }	// elm::genstruct

#endif	// ELM_AVLTREE_AVLTREE_H
