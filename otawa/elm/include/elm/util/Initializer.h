/*
 * $Id$
 * Copyright (c) 2006 - IRIT-UPS <casse@irit.fr>
 *
 * Initializer class interface.
 */
#ifndef ELM_UTIL_INITIALIZER_H
#define ELM_UTIL_INITIALIZER_H

namespace elm {

// Initializer class
template <class T>
class Initializer {
	typedef struct node_t {
		struct node_t *next;
		T *object;
		inline node_t(T *_object, struct node_t *_next)
		: next(_next), object(_object) { }
	} node_t;
	
	static node_t *list;
	static bool initialized;
public:
	Initializer(bool start = true);
	~Initializer(void);
	void record(T *object);
	void startup(void);
};

// Statics
template <class T> typename Initializer<T>::node_t *Initializer<T>::list = 0;
template <class T> bool Initializer<T>::initialized = false;

// Initializer<T>::Initializer
template <class T>
Initializer<T>::Initializer(bool start) {
	if(start)
		startup();
}

// Initializer<T>::~Initializer
template <class T>
Initializer<T>::~Initializer(void) {
	for(node_t *node = list, *next; node; node = next) {
		next = node->next;
		delete node;
	}
}

// Initializer<T>::record
template <class T>
void Initializer<T>::record(T *object) {
	if(initialized)
		object->initialize();
	else {
		list = new node_t(object, list);
	}
}

// Initializer<T>::startup()
template <class T>
void Initializer<T>::startup(void) {
	if(!initialized) {
		initialized = true;
		for(node_t *node = list, *next; node; node = next) {
			next = node->next;
			node->object->initialize();
			delete node;
		}
		list = 0;
	}
}

} // elm

#endif // ELM_UTIL_INITIALIZER_H

