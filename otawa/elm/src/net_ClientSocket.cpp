/*
 *	ClientsSocket class implementation
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

#if defined(__unix) || defined(__APPLE__)
#	include <sys/socket.h>
#	include <sys/types.h>
#	include <netinet/in.h>
#	include <netdb.h>
#	include <stdio.h>
#	include <string.h>
#	include <stdlib.h>
#	include <unistd.h>
#	include <errno.h>
#	include <elm/io/UnixInStream.h>
#	include <elm/io/UnixOutStream.h>
#elif defined(__WIN32) || defined(__WIN64)
#	include <windows.h>
#	include <winsock2.h>
#	include <ws2tcpip.h>
#	include <iphlpapi.h>
#	include <stdio.h>
#else
#	error "unsupported platform"
#endif
#include <elm/net/ClientSocket.h>
#include <elm/net/ServerSocket.h>
#include <elm/string.h>

namespace elm { namespace net {

// ClientSocket for Unix
#if defined(__unix) || defined(__APPLE__)
	class NativeClientSocket: public ClientSocket {
	public:

		NativeClientSocket(const string& host, int port)
			: _host(host), _port(port), _fd(-1), _in(0), _out(0) { }

		NativeClientSocket(const string& host, const string& service)
			: _host(host), _serv(service), _fd(-1), _in(0), _out(0) { }

		NativeClientSocket(int port)
			: _host("127.0.0.1"), _port(port), _fd(-1), _in(0), _out(0) { }

		virtual ~NativeClientSocket(void) { disconnect(); }
		virtual int port(void) const { return _port; }
		virtual const string& host(void) const { return _host; }
		virtual const string& service(void) const { return _serv; }

		virtual void connect(void) throw(Exception)  {

			// find the host
			struct addrinfo hints;
			memset(&hints, 0, sizeof(struct addrinfo));
			hints.ai_family = AF_UNSPEC;
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_flags = AI_NUMERICSERV;
			hints.ai_protocol = 0;
			struct addrinfo *info;
			string service = _serv ? _serv: (_ << _port);
			if(getaddrinfo(_host.toCString().chars(), service.toCString().chars(), &hints, &info) != 0)
				throw Exception(_ << "cannot get the host address: " << _host << ": " << service);

			// build the socket
			_fd = socket(AF_INET, SOCK_STREAM, 0);
			if(_fd < 0)
				throw Exception(_ << "cannot create the socket: " << strerror(errno));

			// perform the connection
			if(::connect(_fd, info->ai_addr, info->ai_addrlen) == -1) {
				disconnect();
				throw Exception(_ << "cannot connect: " << strerror(errno));
			}
		}

		virtual void disconnect(void) throw(Exception)  {
			if(_fd >= 0) {
				::close(_fd);
				_fd = -1;
				if(_in)
					delete _in;
				if(_out)
					delete _out;
			}
		}

		virtual io::InStream& in(void) throw(Exception)  {
			if(_fd < 0)
				throw Exception("client not connected");
			if(!_in)
				_in = new io::UnixInStream(_fd);
			return *_in;
		}

		virtual io::OutStream& out(void) throw(Exception)  {
			if(_fd < 0)
				throw Exception("client not connected");
			if(!_out)
				_out = new io::UnixOutStream(_fd);
			return *_out;
		}

	private:
		string _host;
		string _serv;
		int _port;
		int _fd;
		io::InStream *_in;
		io::OutStream *_out;
	};


// Windows client implementation
#elif defined(__WIN32) || (__WIN64)
	class NativeClientSocket: public ClientSocket {
	public:

		NativeClientSocket(const string& host, int port)
			: _host(host), _port(port), sock(INVALID_SOCKET), _in(this), _out(this) { }

		NativeClientSocket(const string& host, const string& service)
			: _host(host), _serv(service), sock(INVALID_SOCKET), _in(this), _out(this) { }

		NativeClientSocket(int port)
			: _host("127.0.0.1"), _port(port), sock(INVALID_SOCKET), _in(this), _out(this) { }

		virtual ~NativeClientSocket(void) { disconnect(); }
		virtual int port(void) const { return _port; }
		virtual const string& host(void) const { return _host; }
		virtual const string& service(void) const { return _serv; }

		virtual void connect(void) throw(Exception)  {

			// initialization
			struct addrinfo *result = 0, *ptr = 0, hints;
			ZeroMemory( &hints, sizeof(hints) );
			hints.ai_family = AF_UNSPEC;
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_protocol = IPPROTO_TCP;
			string service = _serv ? _serv: (_ << _port);
			if(getaddrinfo(_host.toCString().chars(), service.toCString().chars(), &hints, &result) != 0)
				throw Exception(_ << "cannot get the host address: " << _host << ": " << service);

			// build the socket
			sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
			_fd = socket(AF_INET, SOCK_STREAM, 0);
			if(sock == INVALID_SOCKET) {
				freeaddrinfo(result);
				throw Exception(_ << "cannot create the socket: " << WSAGetLastError());
			}

			// perform the connection
			if(::connect(sock, info->ai_addr, info->ai_addrlen) != 0) {
				disconnect();
				throw Exception(_ << "cannot connect: " << WSAGetLastError());
			}
		}

		virtual void disconnect(void) throw(Exception)  {
			if(sock != INVALID_SOCKET)
				closesocket(sock);
		}

		virtual io::InStream& in(void) throw(Exception)  {
			if(sock == INVALID_SOCKET)
				throw Exception("client not connected");
			else
				return _in;
		}

		virtual io::OutStream& out(void) throw(Exception)  {
			if(sock == INVALID_SOCKET)
				throw Exception("client not connected");
			else
				return _out;
		}

	private:

		class In: public InStream {
		public:
			inline In(NativeClientSocket& socket): sock(socket) { }
			virtual ~In(void) { }
			virtual int read (void *buffer, int size) { return recv(sock->sock, buffer, size, 0); }
			virtual CString lastErrorMessage(void) { return sock->lastError(); }
		private:
			NativeClientSocket& sock;
		};

		class Out: public OutStream {
		public:
			inline Out(NativeClientSocket& socket): sock(socket) { }
			virtual ~Out(void) { }
			virtual int write(const char *buffer, int size) { send(sock->sock, buffer, size, 0); }
			virtual CString lastErrorMessage(void) { return sock->lastError(); }
		private:
			NativeClientSocket& sock;
		};

		cstring lastError(void) {
			last_error = _ << "IO error: " << WSAGetLastError();
			return last_error.toCString();
		}

		string _host;
		int _port;
		SOCKET sock;
		In _in;
		Out _out;
		string last_error;
	};

// no client
#else
#	error "Unsupported platform"
#endif

/**
 * @class ClientSocket
 * Class implementing a client through a socket connection.
 */


/**
 * Build a client socket connecting to the given host on the given port.
 * @param host	Host to connect to (numeric or symbolic form).
 * @param port	Port to connect to.
 */
ClientSocket *ClientSocket::make(const string& host, int port) {
	return new NativeClientSocket(host, port);
}


/**
 * Build a client socket connecting to the local host on the given port.
 * @param port	Port to connect to.
 */
ClientSocket *ClientSocket::make(int port) {
	return new NativeClientSocket(port);
}


/**
 * Build a client socket connecting to the given host with the given service.
 * @param host		Host to connect to (numeric or symbolic form).
 * @param service	Service to connect to.
 */
ClientSocket *ClientSocket::make(const string& host, const string& service) {
	return new NativeClientSocket(host, service);
}


/**
 * @fn int ClientSocket::port(void) const;
 * Get the port of the client.
 * @return	Connection port.
 */


/**
 * @fn void ClientSocket::connect(void) throw(Exception);
 * Connect the client to the server.
 * @throw Exception		If there is an error.
 */


/**
 * @fn void ClientSocket::disconnect(void) throw(Exception);
 * Disconnect from the server.
 * @throw	Exception	If there is an error.
 */


/**
 * @fn io::InStream& ClientSocket::in(void) throw(Exception);
 * Obtain the input channel for the connection.
 * @return	Input channel.
 * @throw	Exception	If there is an error.
 */


/**
 * @fn io::OutStream& ClientSocket::out(void) throw(Exception);
 * Obtain the output channel for the connection.
 * @return	Output channel.
 * @throw	Exception	If there is an error.
 */


/**
 * @fn const string& ClientSocket::host(void) const;
 * Get the host name of the server the client has to connect to.
 * @return	Server host name.
 */


/**
 * @fn const string& ClientSocket::service(void) const;
 * If any, get the name of the service of connection.
 * @return	Connected service name.
 */

} }	// elm::net
