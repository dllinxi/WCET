#include <elm/util/test.h>
#include <elm/enum_info.h>

using namespace elm;

typedef enum {
	a,
	b,
	c
} my_enum;

namespace elm { template <> struct type_info<my_enum>: public enum_info<my_enum> { }; }

TEST_BEGIN(enum_info)

	// enumeration test
	type_info<my_enum>::iterator i = type_info<my_enum>::begin();
	CHECK(i != type_info<my_enum>::end());
	CHECK(i.name() == "a");
	CHECK(i.value() == int(a));
	i++;
	CHECK(i != type_info<my_enum>::end());
	CHECK(i.name() == "b");
	CHECK(i.value() == int(b));
	i++;
	CHECK(i != type_info<my_enum>::end());
	CHECK(i.name() == "c");
	CHECK(i.value() == int(c));
	i++;
	CHECK(i == type_info<my_enum>::end());

	// IO test
	{
		io::StringInput in("b");
		my_enum e;
		in >> e;
		CHECK(e == b);
	}
TEST_END

namespace elm {
	template <> cstring enum_info<my_enum>::name(void) { return "my_enum"; }
	template <> enum_info<my_enum>::value_t enum_info<my_enum>::values[] = {
		value("a", a),
		value("b", b),
		value("c", c),
		last()
	};
}

