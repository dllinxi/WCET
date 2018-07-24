/*
 * $Id$
 * Copyright (c) 2006, IRIT-UPS.
 *
 * test/test_option.cpp -- option module test.
 */

#include <elm/util/test.h>
#include <elm/options.h>
#include <elm/option/BoolOption.h>
#include <elm/option/ListOption.h>

using namespace elm;
using namespace elm::option;

// EnumOption test
typedef enum val_t {
	one,
	two,
	three
} val_t;
EnumOption<int>::value_t vals[] = {
	{ "value", one },
	{ "one", one },
	{ "two", two },
	{ "three", three },
	{ "", one }
};


// manager initialization
static const cstring program = "my-command",
					 copyright = "copyright (c) 2010",
					description = "a test application",
					author = "H. Cassé <casse@irit.fr",
					arg_free = "free argument";
static const Version version(1, 2, 3);

class MyCommand: public Manager {
public:

	MyCommand(void)
	: Manager(
		option::program, &::program,
		option::version, new Version(::version),
		option::copyright, &::copyright,
		option::description, &::description,
		option::author, &::author,
		option::free_arg, &::arg_free,
		end
	) {
	}
};


// options
MyCommand man;

// new style options
ValueOption<string> ns(ValueOption<string>::Make(man).cmd("--ns"));

// old style options
BoolOption b(man, 'b', "boolean", "boolean test", false);
EnumOption<int> enum_opt(man, 'e', "enum", "", vals);
StringOption s(man, 's', "string", "string test", "", "");
SwitchOption c(man, cmd, "command", end);
SwitchOption sw(man, cmd, "-S", help, "switch option", end);
ListOption<int> l(man, cmd, "-l", end);


class MyCommand2: public option::Manager {
public:
	MyCommand2(void): option::Manager(Make(::program, ::version)
		.author(::author)
		.copyright(::copyright)
		.description(::description)
		.free_argument(arg_free)),
	c(SwitchOption::Make(this).cmd("command")),
	sw(SwitchOption::Make(this).cmd("-S").description("switch option"))
	{ }

private:
	SwitchOption c;
	SwitchOption sw;
	/*ListOption<int> l;*/
};

// test_option()
TEST_BEGIN(option)

	try {

		// manager test
		{
			CHECK_EQUAL(man.getProgram(), ::program);
			CHECK_EQUAL(man.getAuthor(), ::author);
			CHECK_EQUAL(man.getDescription(), ::description);
			CHECK_EQUAL(man.getCopyright(), ::copyright);
			CHECK_EQUAL(man.getVersion(), ::version);
			CHECK_EQUAL(man.getFreeArgumentDescription(), ::arg_free);
		}

		// new style value option
		{
			const char *argv[] = { "command", "--ns", "ok", 0 };
			man.parse(3, argv);
			CHECK_EQUAL(*ns,string("ok"));
		}

		// boolean option
		{
			const char *argv[] = { "command", "-b", 0 };
			FAIL_ON_EXCEPTION(OptionException, man.parse(2, argv));
			CHECK_EQUAL(*b, true);
			b = false;
		}
		{
			const char *argv[] = { "command", 0 };
			FAIL_ON_EXCEPTION(OptionException, man.parse(1, argv));
			CHECK_EQUAL(*b, false);
			b = false;
		}
		{
			const char *argv[] = { "command", "--boolean", 0 };
			FAIL_ON_EXCEPTION(OptionException, man.parse(2, argv));
			CHECK_EQUAL(*b, true);
			b = false;
		}

		// string option
		{
			cstring value = "hello, world !";
			const char *argv[] = { "command", "--string", &value, 0 };
			FAIL_ON_EXCEPTION(OptionException, man.parse(3, argv));
			CHECK_EQUAL(*s, string(value));
			s.set("");
		}
		{
			cstring value = "hello, world !";
			const char *argv[] = { "command", "--string=hello, world !", 0 };
			FAIL_ON_EXCEPTION(OptionException, man.parse(2, argv));
			CHECK_EQUAL(*s, string(value));
			s.set("");
		}
		{
			cstring value = "hello, world !";
			const char *argv[] = { "command", "-s", &value, 0 };
			FAIL_ON_EXCEPTION(OptionException, man.parse(3, argv));
			CHECK_EQUAL(*s, string(value));
			s.set("");
		}
		{
			cstring value = "hello, world !";
			const char *argv[] = { "command", "-s=hello, world !", 0 };
			FAIL_ON_EXCEPTION(OptionException, man.parse(2, argv));
			CHECK_EQUAL(*s, string(value));
			s.set("");
		}

		// SwicthOption test
		{
			const char *argv[] = { "command", "-S", 0 };
			FAIL_ON_EXCEPTION(OptionException, man.parse(2, argv));
			CHECK_EQUAL(*sw, true);
		}

		// free command
		{
			const char *argv[] = { "command", "command", 0 };
			FAIL_ON_EXCEPTION(OptionException, man.parse(2, argv));
			CHECK_EQUAL(*c, true);
		}

		// Unknown option
		{
			const char *argv[] = { "command", "--bad", 0 };
			CHECK_EXCEPTION(OptionException, man.parse(2, argv));
		};

		// EnumOption test
		{
			CHECK(enum_opt == one);
			enum_opt = two;
			CHECK(enum_opt == two);
			{
				const char *argv[] = { "command", "-e", "one", 0 };
				man.parse(3, argv);
				CHECK(enum_opt == one);
			}
			{
				const char *argv[] = { "command", "-e", "two", 0 };
				man.parse(3, argv);
				CHECK(enum_opt == two);
			}
			{
				const char *argv[] = { "command", "-e", "four", 0 };
				CHECK_EXCEPTION(OptionException, man.parse(3, argv));
			}
		}

		// multi test
		{
			const char *argv[] = { "command", "-l", "0", "-l", "1", "-l", "2", 0 };
			FAIL_ON_EXCEPTION(OptionException, man.parse(7, argv));
			CHECK_EQUAL(l.count(), 3);
			CHECK_EQUAL(l[0], 0);
			CHECK_EQUAL(l[1], 1);
			CHECK_EQUAL(l[2], 2);
		}

	}
	catch(OptionException& e) {
		__case.failed();
		cerr << "FAILURE: " << e.message() << io::endl;
	}

TEST_END

