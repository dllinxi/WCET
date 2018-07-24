/*
 * data.h
 *
 *  Created on: 16 janv. 2016
 *      Author: casse
 */

#ifndef ELM_DATA_H_
#define ELM_DATA_H_

namespace elm {

/**
 * @defgroup data Data Structures
 *
 * Includes also function as:
 * @li @ref void quicksort(A<T>& array, const C& c)
 * @li @ref void quicksort(A<T>& array)
 *
 * This module groups together classes implementing storage data structure. It is new in ELM 2.0
 * and try to extend and to make easier the use of classes from @ref gen_datastruct module.
 * The main differences includes:
 * @li shorter names for classes and for iterators,
 * @li include in the @ref main elm name space,
 * @li more STL compliance,
 * @li more operators overloaded.
 *
 * In addition, their documentation includes several topic allowing a better understanding
 * of their work to help choosing the better data structure:
 * @li complexity (average and worst) of basic operations like add and remove,
 * @li basic memory and element relative memory foot print.
 *
 * As for old data structures, the following topics applies:
 * @li concepts implemented by the data structure are provided,
 * @li a data structure must not be modified while an iterator is active on it unless specific methods are provided.
 */

/**
 * @fn template <class T, template <class> class A, class C> void quicksort(A<T>& array, const C& c);
 * Sort the given array using quicksort algorithm (complexity O(N log(N)) ).
 *
 * @param array		Array containing the values to sort.
 * @param c			Comparator to use (rely on ELM default comparator if not provided).
 * @param T			Type of values.
 * @param A			Type of array (must implement @ref Array concept).
 * @param C			Type of comparator (must implement @ref Comparator or @ref Compare concept).
 *
 * @ingroup data
 */

/**
 * @fn template <class T, template <class> class A> void quicksort(A<T>& array);
 * Sort the given array using quicksort algorithm (complexity O(N log(N)) ).
 * The used comparator is @ref Comparator<T>.
 *
 * @param array		Array containing the values to sort.
 * @param T			Type of values.
 * @param A			Type of array (must implement @ref Array concept).
 *
 * @ingroup data
 */

/**
 * @fn template <class I> int count(I i);
 * Count the number of items accessible from iterator i.
 * @param i		Iterator.
 * @return		Counter of item to iterate on.
 *
 * @ingroup data
 */

/**
 * @fn template <class I, class P> bool forall(I i, P p);
 * Test if the given predicate t is true for all items of the iterator i.
 * @param i		Iterator on the items to test.
 * @param p		Predicate to test (type bool (*)(I)).
 * @return		True if all item are accepted by predicate, false else.
 *
 * @ingroup	data
 */

/**
 * @fn template <class I, class P, class A> bool forall(I i, P p, A a);
 * Test if the given predicate t is true for all items of the iterator i and supports an extra argument.
 * @param i		Iterator on the items to test.
 * @param p		Predicate to test (type bool (*)(I, A)).
 * @param a		Extra argument.
 * @return		True if all item are accepted by predicate, false else.
 *
 * @ingroup	data
 */

/**
 * @fn template <class I, class P, class A> bool exists(I i, P p, A a);
 * Test if the given predicate t is true for one of items of the iterator i and supports an extra argument.
 * @param i		Iterator on the items to test.
 * @param p		Predicate to test (type bool (*)(I, A)).
 * @param a		Extra argument.
 * @return		True if one item is accepted by predicate, false else.
 *
 * @ingroup	data
 */

}	// elm

#endif /* ELM_DATA_H_ */
