/*
 *	Test for ServerSocket.
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2012, IRIT UPS.
 *
 *	ELM is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	ELM is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with ELM; if not, write to the Free Software
 *	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <elm/io.h>
#include <elm/net/ServerSocket.h>

using namespace elm;
using namespace elm::net;

class MyServer: public Server {
public:
	MyServer(void): Server(8888) { }
protected:
	virtual void onConnection(Connection& connection) {
		cstring hello = "Hello !\n";
		int r = connection.out().write(hello.chars(), hello.length());
		if(r <= 0)
			cerr << "ERROR: when sending " << hello << io::endl;
		else {
			cerr << "INFO: success of sending hello !\n";
		}
		char c = connection.in().read();
		while(c != io::InStream::ENDED && c != '\n') {
			cout << c;
			c = connection.in().read();
		}
		cout << io::endl;
	}
};

int main(void) {
	MyServer server;
	try {
		server.open();
		cout << "INFO: port = " << server.port() << io::endl;
		server.manage();
	}
	catch(net::Exception& e) {
		cerr << "ERROR: exception: " << e.message() << io::endl;
	}
	return 0;
}
