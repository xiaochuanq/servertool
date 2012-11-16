/*
 * Socket.cpp
 *
 * @date Jul 5, 2011
 * @author Xiaochuan Qin
 */

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>
#include <sys/select.h>

#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>

#include "Exceptions.h"
#include "Socket.h"

#ifndef SOL_TCP
#define SOL_TCP IPPROTO_TCP
#endif

#ifndef TCP_KEEPIDLE
#define TCP_KEEPIDLE TCP_KEEPALIVE
#endif

void SocketImpl::create_socket()
{
	socket_ = ::socket(PF_INET, SOCK_STREAM, 0);
	if( -1 == socket_ )
		throw socket_exception( "socket(7)", errno);
	closed_ = false;
}

void SocketImpl::make_address(struct sockaddr_in* paddr, const std::string& ip, int port)
{
	memset(paddr, 0, sizeof( struct sockaddr_in));
	paddr->sin_family = AF_INET;
	paddr->sin_port = htons(port);
	paddr->sin_addr.s_addr = inet_addr( ip.c_str() );
	if( paddr->sin_addr.s_addr == INADDR_NONE)
		throw socket_exception( "inet_addr()", errno);
}

void SocketImpl::write( const std::string& msg) const
{
	int n = ::write( socket_, msg.data(), msg.size());
	if( n < (int) msg.size() )
		throw socket_exception("write(2)", errno);
}

std::string SocketImpl::read( size_t maxLength ) const // better used in
{
	char buffer[maxLength];
	int	nBytesRead = ::read( socket_, buffer, maxLength );
	if( nBytesRead < 0)
		throw socket_exception( "read(2)", errno);
	return std::string( buffer, nBytesRead );
}

void SocketImpl::bind( const std::string& ip, int port)
{
	struct sockaddr_in addr;
	make_address(&addr, ip, port);
	if( -1 == ::bind(socket_, (struct sockaddr*)&addr, sizeof( addr) ) )
		throw socket_exception( "bind(2)", errno);
	bound_ = true;
}

void SocketImpl::close()
{
	if( closed_)
		return;
	if( -1 == ::close( socket_) )
		throw socket_exception( "close(2)", errno);
	closed_ = true;
	bound_ = false;
}

void SocketImpl::set_nonblocking(bool nonblocking)
{
    int flag = ::fcntl(socket_, F_GETFL, 0);
    if (-1 == (flag) )
        flag = 0;
	if( nonblocking)
		flag |= O_NONBLOCK; // set as non-block
	else
		flag &= ~O_NONBLOCK; //remove non-block bit
	if( -1 == ::fcntl (socket_, F_SETFL, flag) )
		throw socket_exception( "fcntl()", errno);
	non_blocking_ = nonblocking;
}

void SocketImpl::set_timeout(int sec, int usec)
{
	timeout_.tv_sec = sec;
	timeout_.tv_usec = usec;
	int status = ::setsockopt(socket_, SOL_SOCKET, SO_SNDTIMEO, &timeout_, sizeof( timeout_) );
	if( status < 0)
		throw socket_exception("setsockopt(3).", errno);
	status = ::setsockopt(socket_, SOL_SOCKET, SO_RCVTIMEO, &timeout_, sizeof( timeout_) );
	if( status < 0)
		throw socket_exception("setsockopt(3).", errno);
}


//////////// Class Socket ////////////////////////////////////
Socket::Socket() :
	SocketImpl(-1),
	connected_(false)
{
	create_socket();
}

Socket::Socket(int socket) :
	SocketImpl(socket),
	connected_(false)
{
}

Socket::Socket(const std::string& ip, int port) :
	SocketImpl(-1),
	connected_(false)
{
	create_socket();
	bind(ip, port);
}

Socket::~Socket()
{
	try{
		close();
		connected_ = false;
	}
	catch( socket_exception& se){
		std::cerr << se.what() << "; Error: " << se.errstr() << "." << std::endl;
	}
}

void Socket::connect(const std::string& ip, int port)
{
	struct sockaddr_in addr;
	make_address(&addr, ip, port);
		// connect to the remote host/server
	if( -1 == ::connect(fd(), (struct sockaddr*)&addr, sizeof( addr)) )
		throw socket_exception( "connect(2)", errno);
	connected_ = true;
}

int Socket::byte_pending_read()
{
	int n2read = 0;
	if( -1 == ::ioctl( fd(), FIONREAD, &n2read) )
		throw socket_exception( "itocl(2)", errno);
	return n2read;
}

int Socket::byte_pending_write()
{
	int n2write = 0;
	if( -1 == ::ioctl( fd(), TIOCOUTQ, &n2write) )
		throw socket_exception( "itocl(2)", errno);
	return n2write;
}

void Socket::set_keepalive_timer(int timeout)
{
	if( -1 == ::setsockopt( fd(), SOL_TCP, TCP_KEEPIDLE, &timeout, sizeof(timeout)))
		throw socket_exception("setsockopt( TCP_KEEPIDLE )", errno);
}

int Socket::get_keepalive_timer()
{
	int timeout;
	if( -1 == ::getsockopt( fd(), SOL_TCP, TCP_KEEPIDLE, &timeout, 0) )
		throw socket_exception("getsockopt( TCP_KEEPIDLE )", errno);
	return timeout;
}

void Socket::enable_keepalive( bool ka)
{
	int optval = (int)ka;
	if( -1 == ::setsockopt( fd(), SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval)) )
		throw socket_exception("setsockopt( SO_KEEPALIVE )", errno);
}

void Socket::keepalive( int timeout )
{
	if( timeout > 0)
	{
		enable_keepalive( true );
		set_keepalive_timer( timeout );
	}
	else{
		enable_keepalive( false);
	}
}

bool Socket::keepalive_enalbed()
{
	int alive;
	if( -1 == ::getsockopt( fd(), SOL_SOCKET, SO_KEEPALIVE, &alive, 0) )
		throw socket_exception("getsockopt( TCP_KEEPIDLE )", errno);
	return (bool)alive;
}

#ifdef _LINUX
void Socket::set_keepalive_interval(int tv)
{
	if( -1 == ::setsockopt( fd(), SOL_TCP, TCP_KEEPINTVL, &tv, sizeof(tv)))
			throw socket_exception("setsockopt( TCP_KEEPINTVL )", errno);
}

void Socket::set_keepalive_cnt( int count)
{
	if( -1 == ::setsockopt( fd(), SOL_TCP, TCP_KEEPCNT, &count, sizeof(count)))
			throw socket_exception("setsockopt( TCP_KEEPCNT )", errno);
}

void Socket::set_keepalive_option( const SocketKeepAliveOption& option)
{
	enable_keepalive( option.keepalive);
	if( option.keepalive ){
		set_keepalive_timer( option.timer);
		set_keepalive_interval( option.interval);
		set_keepalive_cnt( option.count );
	}
}

int Socket::get_keepalive_interval()
{
	int interval;
	if( -1 == ::getsockopt( fd(), SOL_TCP, TCP_KEEPINTVL, &interval, 0) )
	throw socket_exception("getsockopt( TCP_KEEPINTVL )", errno);
	return interval;
}

int Socket::get_keepalive_cnt( )
{
	int count;
	if( -1 == ::getsockopt( fd(), SOL_TCP, TCP_KEEPCNT, &count, 0) )
	throw socket_exception("getsockopt( TCP_KEEPCNT )", errno);
	return count;
}

void Socket::get_keepalive_option( SocketKeepAliveOption& option)
{
	option.keepalive = keepalive_enalbed();
	if( option.keepalive){ // the following fields are valid only if keepalive is true.
		option.timer = get_keepalive_timer();
		option.interval = get_keepalive_interval();
		option.count = get_keepalive_cnt();
	}
}

#endif
/************ ServerSocket Class ***********************/

ServerSocket::ServerSocket(const std::string& ip, int port, int backlog)
: SocketImpl(-1)
{
	if( backlog < 0)
			backlog = 256;
	create_socket();
	bind(ip, port);
	int status = ::listen( fd(), backlog);
	if( status < 0)
		throw socket_exception("listen(2)", errno);
}

ServerSocket::~ServerSocket()
{
	try{
		close();
	}
	catch( socket_exception& se){
		std::cerr << se.what() << "; Error: " << se.errstr() << "." << std::endl;
	}
}

Socket ServerSocket::accept()
{
	struct sockaddr_in client_addr;
	socklen_t len_inet;
	int nSock = ::accept(fd(), (struct sockaddr*)&client_addr, &len_inet);
	if( nSock < 0)
		throw socket_exception(
				"accept(2)", errno);
	return Socket( nSock );
}
