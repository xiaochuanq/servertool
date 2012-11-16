/*
 * SocketSet.h
 *
 *  Created on: Mar 4, 2012
 *      Author: Xiaochuan qin
 */

#ifndef SOCKETSET_H_
#define SOCKETSET_H_
#include <vector>
#ifdef _CPP_11
#include <unordered_map>
using std::unordered_map;
#else
#include <tr1/unordered_map>
using std::tr1::unordered_map;
#endif
#include <Socket.h>

namespace qths{
/*!
 * @brief Class SocketSet
 *
 */

typedef std::vector<Socket*> socket_ptr_vector;
typedef std::vector<const Socket*> const_socket_ptr_vector;

class SocketSet {
#ifdef _CPP_11
	typedef std::unordered_map<int, Socket*> socket_map;
#else
	typedef std::tr1::unordered_map<int, Socket*> socket_map;
#endif
public:
	SocketSet(int timedout_sec = 0, int timedout_usec = 0 ){
		timeout_.tv_sec = timedout_sec;
		timeout_.tv_usec = timedout_usec;
	}
	void insert(Socket* ps) {
		m_sockets[ps->fd()] = ps;
	} ///< Insert a socket; Note: No check on the validity of the socket.
	int erase(Socket* ps) {
		return m_sockets.erase(ps->fd());
	} ///< Erase a socket to be deleted and returns the number of socket that has been deleted.
	void clear() {
		m_sockets.clear();
	} ///< Clear the set
	void set_timeout(int sec, int usec = 0) {
		timeout_.tv_sec = sec;
		timeout_.tv_usec = usec;
	}
	void poll(socket_ptr_vector* readables_ptr,
			socket_ptr_vector* writables_ptr = 0,
			socket_ptr_vector* exceptions_ptr = 0);
	SocketSet& merge(const SocketSet&);
	size_t size() {
		return m_sockets.size();
	}
	const struct timeval& timeout() {
		return timeout_;
	}
private:
	void clear_flag();
private:
	fd_set m_fdSet_r;
	fd_set m_fdSet_w;
	fd_set m_fdSet_e;
	socket_map m_sockets;
	struct timeval timeout_;
};



/*!
 * @brief Class SocketSet
 */
typedef std::list<Socket*> socket_ptr_list;
typedef std::list<const Socket*> const_socket_ptr_list;

class SocketSet
{
    typedef std::map< int, Socket*> socket_map;
public:
    SocketSet()
    {
        timeout_.tv_sec = timeout_.tv_usec = 0;
    }
    void insert( Socket* ps)
    {
        m_sockets[ps->fd()] = ps;    ///< Insert a socket; Note: No check on the validity of the socket.
    }
    int  erase( Socket* ps)
    {
        return m_sockets.erase( ps->fd() );    ///< Erase a socket to be deleted and returns the number of socket that has been deleted.
    }
    void clear()
    {
        m_sockets.clear();    ///< Clear the set
    }
    void set_timeout(int sec, int usec = 0)
    {
        timeout_.tv_sec = sec;
        timeout_.tv_usec = usec;
    }
    void poll( 	socket_ptr_list* readable_list_ptr,
                socket_ptr_list* writable_list_ptr = 0,
                socket_ptr_list* exception_list_ptr = 0  );
    SocketSet& merge( const SocketSet& );
    size_t size()
    {
        return m_sockets.size();
    }
    const struct timeval& timeout()
    {
        return timeout_;
    }
private:
    void clear_flag();
private:
    fd_set m_fdSet_r;
    fd_set m_fdSet_w;
    fd_set m_fdSet_e;
    socket_map m_sockets;
    struct timeval timeout_;
};}//end of namespace qths

#endif /* SOCKETSET_H_ */
