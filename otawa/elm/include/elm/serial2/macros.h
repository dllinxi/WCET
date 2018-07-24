/*
 * $Id$
 * Copyright (c) 2006, IRIT UPS.
 *
 * Macros for serialization.
 */
#ifndef ELM_SERIAL2_MACROS_H
#define ELM_SERIAL2_MACROS_H

#include <elm/serial2/serial.h>

#define ELM_SERIALIZABLE(name, fields) \
	public: \
		static elm::serial2::Class<name> __class; \
		virtual elm::serial2::AbstractType& __getSerialClass(void) const \
			{ return __class; } \
		template <class T> void __visit(T& e) { e & fields; } \
	private:
#define ELM_SERIALIZE(name) elm::serial2::Class<name> name::__class(#name);
#define ELM_FIELD(name) elm::field(#name, name)
#define ELM_DFIELD(name, def) elm::field(#name, name, def)
#define ELM_BASE(name) ((name&)*this)


#ifndef ELM_NO_SHORTCUT
#	define SERIALIZABLE(name, fields) ELM_SERIALIZABLE(name, fields)
#	define SERIALIZE(name) ELM_SERIALIZE(name)
#	define FIELD(name) ELM_FIELD(name)
#	define DFIELD(name, def) ELM_DFIELD(name, def)
#	define BASE(name) ELM_BASE(name)
#endif

#endif // ELM_SERIAL2_MACROS_H
