/*
 *	Test for ClientSocket.
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
#include <elm/net/ClientSocket.h>

using namespace elm;
using namespace elm::net;

int main(void) {
	ClientSocket *client = ClientSocket::make(8888);
	try {
		client->connect();
		cout << "CLIENT: waiting !\n";
		char c = client->in().read();
		while(c >= 0 && c != '\n') {
			cout << c;
			c = client->in().read();
		}
		cout << io::endl;

		if(c != io::InStream::FAILED) {
			cstring msg = "Bye bye !\n";
			client->out().write(msg.chars(), msg.length());
			cout << "CLIENT: success\n";
		}
		else
			cout << "CLIENT: error\n";
		client->disconnect();
	}
	catch(net::Exception& e) {
		cerr << "ERROR: exception thrown: " << e.message() << io::endl;
	}
	delete client;
	return 0;
}
