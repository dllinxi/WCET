/*
 *	Tree class implementation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2013, IRIT UPS.
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

#include <elm/stree/MarkerBuilder.h>
#include <elm/stree/SegmentBuilder.h>

namespace elm { namespace stree  {

/**
 * @defgroup stree Segment Tree
 * Segmented tree allows to retrieve quickly data associated with a value range.
 * Look at http://en.m.wikipedia.org/wiki/Segment_tree for more details.
 *
 * The base class is the @ref elm::stree::Tree that allows to look up on the segments.
 *
 * To populate the tree, programmer can use one of the following:
 * @li @ref elm::stree::Builder -- very rough builder,
 * @li @ref elm::stree::MarkerBuilder -- defines segments by bounds,
 * @li @ref elm::stree::SegmentBuilder -- from the list of segments.
 */

/**
 * @class Tree
 * A segmented tree allows to retrieve items associated with a segment, that is, a value range.
 * The access is quite fast (log2(n)) but the creation of the tree is not dynamic: all segment
 * must be provided at creation time.
 *
 * To help to build such a structure, several builder are provided:
 * @li @ref elm::stree::Builder
 * @li @ref elm::stree::MakerBuilder
 *
 * @param K		Key type.
 * @param T		Retrieven item type.
 * @param C		Comparator to compare keys (default to Comparator<K>).
 * @ingroup stree
 */


/**
 * @fn Tree::Tree(void);
 * Build an empty tree.
 */


/**
 * @fn Tree::Tree(int _root, node_t *_nodes);
 * Build form the given list of nodes. The tree is responsible to release the node
 * array at destruction time.
 * @param _root		Index of root node in the node array.
 * @param _nodes	List of nodes.
 */

/**
 * @fn void Tree::set(int _root, node_t *_nodes);
 * Initialize the current tree with the current configuration.
 * @param _root		Index of root node in the node array.
 * @param _nodes	List of nodes.
 */

/**
 * @fn const T& Tree::get(const K& key, const T& def) const;
 * Find the value associated with the given key. If not found, return the default value.
 * @param key	Key to look for.
 * @param def	Default value.
 * @return		Found value or default value.
 */

/**
 * @fn const T& Tree::get(const K& key) const;
 * Find a value by its key or raise an assertion failure.
 * @param key	Key to look for.
 * @return		Found value.
 */

/**
 * @fn bool contains(const K& key) const;
 * Test if the key is contained in the tree.
 * @param key	Key to test.
 * @return		True if the key is contained, false else.
 */

/**
 * @class Builder
 * Very simple allocator that creates a power of 2 number of nodes for @ref elm::stree::Tree class.
 *
 * @param K		Key type.
 * @param T		Retrieven item type.
 * @param C		Comparator to compare keys (default to Comparator<K>).
 * @ingroup stree
 */

/**
 * @fn node_t *Builder::allocate(t::uint32 n);
 * Allocate an array of nodes whose number is the closest greater number of power of 2.
 * @param n		Power of the number of nodes.
 * @return		Allocated array of nodes (caller is responsible for releasing them).
 */

/**
 * @fn int Builder::make(node_t *nodes, int& s, int start, int end);
 * Initialize the structure of the tree.
 * @notice	The keys of the nodes should have been initialized first.
 * @param nodes		Array of nodes.
 * @param s			Parent node.
 * @param start		Start index.
 * @param end		End index.
 * @return			Index of the root node.
 */


/**
 * @class MarkerBuilder
 * Efficient and easy builder for @ref elm::stree::Tree class
 * thatjust records segments and their value.
 *
 * @param K		Key type.
 * @param T		Retrieven item type.
 * @param C		Comparator to compare keys (default to Comparator<K>).
 * @ingroup stree
 */


/**
 * @fn void MarkerBuilder::add(const K& mark, const T& val);
 * Add a segment starting at the given mark, containing the given value
 * and spanning the next upper given mark.
 * It must be noticed that the value of the greater mark is not used.
 *
 * @param mark	Mark to add.
 * @param val	Value of the segment.
 */

/**
 * @fn void MarkerBuilder::make(stree::Tree<K, T, C>& tree);
 * Build the segmented tree from the markers and values.
 * @param tree	Tree to initialize.
 */


/**
 * @class SegmentBuilder
 * Builder for @ref elm::stree::Tree based on a list of disjointed segments.
 * @warning	Unexpected results may occur if the segments are not disjointed.
 *
 * @param K		Key type.
 * @param T		Retrieven item type.
 * @param C		Comparator to compare keys (default to Comparator<K>).
 * @ingroup stree
 */

/**
 * SegmentBuilder::SegmentBuilder(const T& def);
 * Build a segment builder with a default value: the value returned when a look-up
 * select an area out of segments.
 * @param def	Default value.
 */

/**
 * @fn void SegmentBuilder::add(const K& low, const K& high, const T& val);
 * Add a segment for a range of keys between low (inclusive) and high (exclusive).
 * @param low	Low value of the segment (inclusive).
 * @param high	High value of the segment (exclusive).
 * @param val	Value associated with the segment.
 */

/**
 * @fn void SegmentBuilder::make(stree::Tree<K, T, C>& tree);
 * Build the segmented tree from the segments and values.
 * @param tree	Tree to initialize.
 */

} }	// elm::stree
