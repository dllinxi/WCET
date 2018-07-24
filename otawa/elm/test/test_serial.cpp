/*
 * $Id$
 * Copyright (c) 2006, IRIT-UPS.
 *
 * test/test_serial.cpp -- serial module test.
 */

#include <elm/util/test.h>
#include <elm/io.h>
#include <elm/xom.h>
#include <elm/serial/interface.h>
#include <elm/serial/implement.h>
#include <elm/serial/SerialTable.h>
#include <elm/serial/XOMUnserializer.h>
#include <elm/serial/TextSerializer.h>
#include <elm/genstruct/Table.h>

using namespace elm;

// MySubClass class
class MyClass;
class MySubClass {
	SERIALIZABLE
	char c;
	MyClass *back;
public:
	MySubClass(void) { }
	MySubClass(char _c, MyClass *_back): c(_c), back(_back) { };
	virtual ~MySubClass(void) { }
};



// MyClass class
class MyClass {
	SERIALIZABLE
	int x;
	MySubClass sub;
	MySubClass *sub2;
public:
	MyClass(void) { };
	MyClass(int _x): x(_x), sub('a', this), sub2(new MySubClass('b', 0)) { };
	virtual ~MyClass(void) { }
};



// ItemClass
class ItemClass {
	SERIALIZABLE
public:
	int x;
	virtual int getX(void) { return x; };
	virtual ~ItemClass(void) { }
};


// Item2Class
class Item2Class: public ItemClass {
	SERIALIZABLE
public:
	virtual int getX(void) { return 666; };
	virtual ~Item2Class(void) { }
};


// SimpleClass
class SimpleClass {
	SERIALIZABLE
public:
	typedef enum enum_t {
		VAL1 = 0,
		VAL2,
		VAL3
	} enum_t;

	ItemClass *ref;
	int x;
	char c;
	double f;
	CString str;
	serial::SerialTable<int> list;
	serial::SerialTable<ItemClass> list2;
	serial::SerialTable<ItemClass *> list3;
	enum_t en;
	SimpleClass(void): x(111), c('a'), f(0.1), str("ok"), en(VAL1) { }
	virtual ~SimpleClass(void) { }
};
//CString values[] = { "VAL1", "VAL2", "VAL3", "" };
SERIALIZABLE_ENUM(SimpleClass::enum_t);

/*namespace elm { namespace serial {
template <class T>
inline void elm::serial::Unserializer::read<T>(T& val) {
}
} }*/ // elm::serial

SERIALIZE_ENUM(SimpleClass::enum_t,
	ENUM_VALUE(SimpleClass::VAL1),
	ENUM_VALUE(SimpleClass::VAL2),
	ENUM_VALUE(SimpleClass::VAL3));


// Entry point
void test_serial(void) {
	
	try {
		
		// Serialize text
		elm::serial::TextSerializer serialize;
		MyClass my_object(666);
		serialize << my_object;
		serialize.close();
		cout << io::endl << io::endl;
		
		// Unserialize XML
		serial::XOMUnserializer unser("unser.xml");
		SimpleClass res;
		unser >> res;
		unser.close();
		cout << "res.x = " << res.x << io::endl;
		cout << "res.c = " << res.c << io::endl;
		cout << "res.f = " << res.f << io::endl;
		cout << "res.str = " << res.str << io::endl;
		cout << "address = " << &res.list[0] << io::endl;
		for(int i = 0; i < res.list.count(); i++)
			cout << "res.list[" << i << "] = " << res.list[i] << io::endl;
		for(int i = 0; i < res.list2.count(); i++)
			cout << "res.list2[" << i << "] = " << res.list2[i].x
			     << " (" << io::hex((int)&res.list2[i]) << ")" << io::endl;
		for(int i = 0; i < res.list3.count(); i++)
			cout << "res.list3[" << i << "] = " << res.list3[i]->getX()
				 << " = " << res.list3[i]->x
			     << " (" << io::hex((int)res.list3[i]) << ")" << io::endl;
		cout << "en = " << res.en << io::endl;
		cout << "ref = " << io::hex((int)res.ref) << io::endl;
	}
	catch(Exception& exn) {
		cerr << "ERROR: " << exn.message() << io::endl;
	}
}


// Avoid impact of template predefinition
SERIALIZE(MySubClass, FIELD(c); FIELD(back);)
SERIALIZE(MyClass, FIELD(x); FIELD(sub); FIELD(sub2))
SERIALIZE(ItemClass, FIELD(x))
SERIALIZE(Item2Class, SERIALIZE_BASE(ItemClass))
SERIALIZE(SimpleClass, FIELD(x); FIELD(c); FIELD(f); FIELD(str); FIELD(list);
	FIELD(en); FIELD(list2); FIELD(list3); FIELD(ref))
