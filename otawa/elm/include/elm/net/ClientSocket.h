/*
 *	ClientsSocket class interface
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
#ifndef ELM_NET_CLIENTSOCKET_H_
#define ELM_NET_CLIENTSOCKET_H_

#include <elm/net/Exception.h>
#include <elm/io/InStream.h>
#include <elm/io/OutStream.h>

namespace elm { namespace net {

class ClientSocket {
public:
	static ClientSocket *make(const string& host, int port);
	static ClientSocket *make(const string& host, const string& service);
	static ClientSocket *make(int port);

	virtual ~ClientSocket(void) { }
	virtual int port(void) const = 0;
	virtual const string& host(void) const = 0;
	virtual const string& service(void) const = 0;
	virtual void connect(void) throw(Exception) = 0;
	virtual void disconnect(void) throw(Exception) = 0;
	virtual io::InStream& in(void) throw(Exception) = 0;
	virtual io::OutStream& out(void) throw(Exception) = 0;
};

} }	// elm::net

#endif /* ELM_NET_CLIENTSOCKET_H_ */
