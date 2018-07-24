/*
 * $Id$
 * Copyright (c) 2006, IRIT-UPS.
 *
 * test/test_serial.cpp -- serial module test.
 */

#include <elm/io.h>
#include <elm/xom.h>
#include <elm/serial2/serial.h>
#include <elm/serial2/XOMUnserializer.h>
#include <elm/genstruct/Table.h>
#include <elm/serial2/collections.h>
#include <elm/serial2/macros.h>

using namespace elm;

// MySubClass class
class MyClass;
class MySubClass {
	SERIALIZABLE(MySubClass, DFIELD(c, '!') & FIELD(back))
	char c;
	MyClass *back;
public:
	MySubClass(void) { }
	MySubClass(char _c, MyClass *_back): c(_c), back(_back) { };
	virtual ~MySubClass(void) { }
};
SERIALIZE(MySubClass);


// MyClass class
class MyClass {
	SERIALIZABLE(MyClass, FIELD(x) & FIELD(sub) & FIELD(sub2))
	int x;
	MySubClass sub;
	MySubClass *sub2;
public:
	MyClass(void) { };
	MyClass(int _x): x(_x), sub('a', this), sub2(new MySubClass('b', 0)) { };
	virtual ~MyClass(void) { }
};
SERIALIZE(MyClass);


// ItemClass
class ItemClass {
	SERIALIZABLE(ItemClass, DFIELD(x, 666))
public:
	int x;
	virtual ~ItemClass(void) { }
	virtual int getX(void) { return x; };
};
SERIALIZE(ItemClass);


// Item2Class
class Item2Class: public ItemClass {
	SERIALIZABLE(Item2Class, BASE(ItemClass))
public:
	~Item2Class(void) { }
	virtual int getX(void) { return 666; };
};
SERIALIZE(Item2Class);


// SimpleClass
class SimpleClass {
	SERIALIZABLE(SimpleClass, FIELD(x) & FIELD(c) & FIELD(f) & FIELD(str)
		  & FIELD(list) & FIELD(en) & FIELD(list2) & FIELD(list3) & FIELD(ref))
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
	genstruct::AllocatedTable<int> list;
	genstruct::AllocatedTable<ItemClass> list2;
	genstruct::AllocatedTable<ItemClass *> list3;
	enum_t en;
	SimpleClass(void): x(111), c('a'), f(0.1), str("ok"), en(VAL1) { }
	virtual ~SimpleClass(void) { }
};
SERIALIZE(SimpleClass);

ENUM(SimpleClass::enum_t)
ENUM_BEGIN(SimpleClass::enum_t) 
	VALUE(SimpleClass::VAL1),
	VALUE(SimpleClass::VAL2),
	VALUE(SimpleClass::VAL3)
ENUM_END


// Entry point
int main(void) {
	
	try {
		
		serial2::XOMUnserializer unser("unser.xml");
		SimpleClass res;
		unser >> res;
		unser.flush();
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
