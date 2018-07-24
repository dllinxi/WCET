/*
 *	elm-plugin tool
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

#include <elm/options.h>
#include <elm/genstruct/Vector.h>
#include <elm/sys/Plugin.h>
#include <elm/sys/System.h>

using namespace elm;

class ElmPlugin: public option::Manager {
public:
	ElmPlugin(void):
	Manager(
		option::program, "elm-plugin",
		option::version, new Version(1, 0, 0),
		option::copyright, "GPL (c) 2013 IRIT - UPS",
		option::author, "H. Cassé <casse@irit.fr>",
		option::end),
	info(*this, option::cmd, "-i", option::cmd, "--info", option::help, "Display the content of the plugin information string", option::end),
	help(*this, option::cmd, "-h", option::cmd, "--help", option::help, "Display this summary of options.", option::end)

	{ }

	virtual void process(string arg) {
		args.add(arg);
	}

	void run(void) throw(elm::Exception) {

		// process help
		if(help) {
			displayHelp();
			return;
		}

		// process the identification
		if(info) {
			performInfo();
			return;
		}
	}

private:

	void performInfo(void) {
		static const char id[] = ELM_PLUGIN_ID_PREFIX;
		for(int i = 0; i < args; i++) {

			// open the file
			io::InStream *in = sys::System::readFile(args[i]);

			// read it
			const char *p = id;
			int c = in->read();
			while(c != io::InStream::ENDED) {
				if(c == io::InStream::FAILED)
					throw MessageException(in->lastErrorMessage());
				if(c != *p)
					p = id;
				else {
					p++;
					if(!*p)
						break;
				}
				c = in->read();
			}

			// read the info string
			if(!*p) {
				StringBuffer buf;
				c = in->read();
				while(c) {
					if(c == io::InStream::FAILED)
						throw MessageException(in->lastErrorMessage());
					else if(c == io::InStream::ENDED)
						throw MessageException("file end found before information ended");
					buf << char(c);
					c = in->read();
				}
				cout << args[i] << ": " << buf.toString() << io::endl;
			}

			// no info string found !
			else {
				cout << args[i] << ": no info string found !\n";
			}

			// cleanup
			delete in;
		}
	}

	option::SwitchOption info, help;
	genstruct::Vector<string> args;
};

int main(int argc, char **argv) {
	ElmPlugin inst;
	try {
		inst.parse(argc, argv);
		inst.run();
	}
	catch(elm::Exception& e) {
		cerr << "ERROR: " << e.message() << io::endl;
	}
}

