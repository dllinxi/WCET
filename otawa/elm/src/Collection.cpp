/*
 * $Id$
 * Copyright (c) 2003, Alfheim Corporation.
 *
 * src/Collection.cpp -- Collection class implementation.
 */

 #include <elm/Collection.h>
 
 namespace elm {


/**
 * @class Collection
 * A collection is an interface to any composite object containing a set of
 * other data. It provides standard methods for processing this set of object.
 * Remark that collection are immutable. If you require mutable collection,
 * use the MutableCollection class.
 */


/**
 * @fn int Collection::count(void) const;
 * Count the number of items in the collection.
 * @return	Item count.
 */


/**
 * @fn bool Collection::isEmpty(void) const;
 * Test if the collection is empty.
 * @return	True if collection is empty, false else.
 */


/**
 * @fn bool Collection::contains(const T value) const;
 * Test if the collection contains the given item.
 * @param value	Item to look for.
 * @return		True if value is in the collection, false else.
 */


/**
 * @fn bool Collection::containsAll(const Collection<T> *values) const;
 * Test if a collection is contained in the current collection.
 * @param values	Collection to test.
 * @return			True if the current collection contains all values of the
 * 					given collection, false else.
 */


/**
 * @fn bool Collection::equals(const Collection<T> *values) const;	
 * Test if the current collection is equal to the given one.
 * @param values	Collection to test.
 * @return			True if both collections are equal, false else.
 */


/**
 * @fn IteratorInst<T> *Collection::visit(void) const;
 * Get an iterator on the collection.
 * @return	Collection iterator.
 */


/**
 * @fn Collection::operator IteratorInst<T> *(void) const;
 * Same as visit().
 */


/**
 * @fn MutableCollection<T> *Collection::empty(void) const;
 * Get an empty mutable collection (used for implementing set operations).
 * @return	Empty mutable collection.
 */


/**
 * @fn MutableCollection<T> *Collection::copy(void) const;
 * Copy the current collection to a mutable one.
 * @return	Copy of the current collection.
 */


/**
 * @fn Collection<T> *Collection::merge(const Collection<T> *values) const;
 * Make the union of current collection with argument collection.
 * @param values	Collection to merge with current one.
 * @return			Result of merge.
 */


/**
 * @fn Collection<T> *Collection::retain(const Collection<T> *values) const;
 * Return current collection without items in the argument collection
 * (set difference).
 * @param values	Collection retain.
 * @return			Result of difference.
 */


/**
 * @fn Collection<T> *Collection::meet(const Collection<T> *values) const;
 * Perform the intersection of current collection with argument collection.
 * @param values	Collection to intersect with.
 * @return			Intersection result.
 */


/**
 * @fn Collection::operator bool(void)
 * Same as not isEmpty().
 */


/**
 * @fn Collection<T>& Collection::operator+(const Collection<T>& values);
 * Same as @ref merge().
 */


/**
 * @fn Collection<T>& Collection::operator-(const Collection<T>& values);
 * Same as @ref retain().
 */
 

/**
 * @fn Collection<T>& Collection::operator*(const Collection<T>& values);
 * Same as @ref meet().
 */


/**
 * @fn bool Collection::operator==(const Collection<T>& values);
 * Same as @ref equals().
 */


/**
 * @fn bool Collection::operator!=(const Collection<T>& values);
 * Perform not @ref equals().
 */


/**
 * @fn bool Collection::operator<(const Collection<T>& values);
 * Test of inclusion with equality of current collection in the argument collection.
 */


/**
 * @fn bool Collection::operator<=(const Collection<T>& values);
 * Test of inclusion of current collection in the argument collection.
 */


/**
 * @fn bool Collection::operator>(const Collection<T>& values);
 * Test if the current includes the given one but is not equal.
 */


/**
 * @fn bool Collection::operator>=(const Collection<T>& values);
 * Test if the current collection includes the given one.
 */


/**
 * @fn bool Collection::operator()(const T value);
 * Same as @ref contains().
 */


/**
 * @class MutableCollection
 * This classe inherits from Collection class but provides facilities for
 * modifying the items contained inside.
 */


/**
 * @fn MutableIteratorInst<T> *MutableCollection::edit(void);
 * Get a mutable iterator.
 * @return	Mutable iterator.
 */


/**
 * @fn void MutableCollection::add(const T value);
 * Add an item to the collection.
 * @param value	Value to add.
 */


/**
 * @fn void MutableCollection::addAll(const Collection<T> *values);
 * Add a collection to the current collection.
 * @param values	Collection to add.
 */


/**
 * @fn void MutableCollection::remove(const T value);
 * Remove an item from the collection.
 * @param value	Value to remove.
 */


/**
 * @fn void MutableCollection::removeAll(const Collection<T> *values);
 * Remove a collection from the current collection.
 * @param values	Collection to remove.
 */


/**
 * @fn void MutableCollection::clear(void);
 * Clear the collection.
 */


/**
 * @fn MutableCollection<T>& MutableCollection::operator+=(const T value);
 * Same as @ref add().
 */


/**
 * @fn MutableCollection<T>& MutableCollection::operator+=(const Collection<T>& values);
 * Same as @ref addAll().
 */


/**
 * @fn MutableCollection<T>& MutableCollection::operator-=(const T value);
 * Same as @ref remove().
 */
 

/**
 * @fn MutableCollection<T>& MutableCollection::operator-=(const Collection<T>& values);
 * Same as @ref removeAll().
 */

}	// elm
 
