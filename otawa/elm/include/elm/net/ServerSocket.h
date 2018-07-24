/*
 *	ServerSocket class interface
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
#ifndef ELM_NET_SERVERSOCKET_H_
#define ELM_NET_SERVERSOCKET_H_

#include <elm/io/InStream.h>
#include <elm/io/OutStream.h>
#include <elm/net/Exception.h>

namespace elm { namespace net {

class Connection {
public:
	virtual ~Connection(void);
	virtual io::InStream& in(void) = 0;
	virtual io::OutStream& out(void) = 0;
};

class ServerSocket {
public:
	static ServerSocket *make(void);
	static ServerSocket *make(int port);
	virtual ~ServerSocket(void);
	virtual int port(void) const = 0;
	virtual void open(void) throw(Exception) = 0;
	virtual Connection *listen(void) throw(Exception) = 0;
	virtual void close(void) = 0;
};

class Server {
public:
	Server(void);
	Server(int port);
	virtual ~Server(void);
	inline int port(void) const { if(sock) return sock->port(); else return -1; }
	void open(void) throw(Exception);
	void manage(void) throw(Exception);
	void close(void);
protected:
	virtual void onConnection(Connection& connection) = 0;
private:
	int _port;
	ServerSocket *sock;
	bool on;
};

} }	// elm::net

#endif /* ELM_NET_SERVERSOCKET_H_ */
