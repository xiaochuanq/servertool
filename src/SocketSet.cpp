/*
 * SocketSet.cpp
 *
 *  Created on: Mar 4, 2012
 *      Author: xiaochuanq
 */

#include "SocketSet.h"

namespace qths{
/************* SocketSet Class **********************/
void SocketSet::clear_flag() {
	FD_ZERO( &m_fdSet_r);
	FD_ZERO( &m_fdSet_w);
	FD_ZERO( &m_fdSet_e);
	for (socket_map::const_iterator iter = m_sockets.begin();
			iter != m_sockets.end(); ++iter) {
		int fd = iter->first;
		FD_SET( fd, &m_fdSet_r);
		FD_SET( fd, &m_fdSet_w);
		FD_SET( fd, &m_fdSet_e);
	}
}

void SocketSet::poll(socket_ptr_vector* readables_ptr,
		socket_ptr_vector* writables_ptr,
		socket_ptr_vector* exceptions_ptr)
		{
	if (m_sockets.empty())
		return;
	if (readables_ptr)
		readables_ptr->clear();
	if (writables_ptr)
		writables_ptr->clear();
	if (exceptions_ptr)
		exceptions_ptr->clear();
	int max_fd_plus1 = m_sockets.rbegin()->first + 1;
	clear_flag();
	int n;
	if (timeout_.tv_sec < 0)
		n = ::select(max_fd_plus1, &m_fdSet_r, &m_fdSet_w, &m_fdSet_e, 0);
	else
		n = ::select(max_fd_plus1, &m_fdSet_r, &m_fdSet_w, &m_fdSet_e,
				&timeout_);
	if (n > 0) {
		for (socket_map::const_iterator iter = m_sockets.begin();
				iter != m_sockets.end(); ++iter) {
			int sn = iter->first;
			Socket* ps = iter->second;
			if (readables_ptr && FD_ISSET( sn, &m_fdSet_r ))
				readables_ptr->push_back(ps);
			if (writables_ptr && FD_ISSET( sn, &m_fdSet_w ))
				writables_ptr->push_back(ps);
			if (exceptions_ptr && FD_ISSET( sn, &m_fdSet_e ))
				exceptions_ptr->push_back(ps);
		}
	} else if (n < 0)
		throw socket_exception("select()", errno);
}

SocketSet& SocketSet::merge(const SocketSet& rhs) {
	//TODO: It is possible that two different sockets (at different memory addresses) maintain the
	// same socket handle. The new one may contain duplicated sockets.
	if (this != &rhs) {
		this->m_sockets.insert(rhs.m_sockets.begin(), rhs.m_sockets.end());
	}
	return *this;
}



/************* SocketSet Class **********************/
void SocketSet::clear_flag()
{
	FD_ZERO( &m_fdSet_r);
	FD_ZERO( &m_fdSet_w);
	FD_ZERO( &m_fdSet_e);
	for( socket_map::const_iterator iter = m_sockets.begin();
			iter != m_sockets.end(); ++iter ){
		int fd = iter->first;
		FD_SET( fd, &m_fdSet_r);
		FD_SET( fd, &m_fdSet_w);
		FD_SET( fd, &m_fdSet_e);
	}
}

void SocketSet::poll( socket_ptr_list* readable_list_ptr,
					  socket_ptr_list* writable_list_ptr,
					  socket_ptr_list* exception_list_ptr )
{
	if( m_sockets.empty() )
		return;
	if(readable_list_ptr)
		readable_list_ptr->clear();
	if(writable_list_ptr)
		writable_list_ptr->clear();
	if(exception_list_ptr)
		exception_list_ptr->clear();
	int max_fd_plus1 = m_sockets.rbegin()->first + 1;
	clear_flag();
	int n;
	if( timeout_.tv_sec < 0 )
		n = ::select( max_fd_plus1, &m_fdSet_r, &m_fdSet_w, &m_fdSet_e, 0);
	else
		n = ::select( max_fd_plus1, &m_fdSet_r, &m_fdSet_w, &m_fdSet_e, &timeout_);
	if( n > 0 ){
		for( socket_map::const_iterator iter = m_sockets.begin();
				iter != m_sockets.end(); ++iter ){
			if( readable_list_ptr && FD_ISSET( iter->first, &m_fdSet_r ) )
				readable_list_ptr->push_back( iter->second );
			if( writable_list_ptr && FD_ISSET( iter->first, &m_fdSet_w ) )
				writable_list_ptr->push_back( iter->second );
			if( exception_list_ptr && FD_ISSET( iter->first, &m_fdSet_e ) )
				exception_list_ptr->push_back( iter->second );
		}
	}
	else if( n < 0)
		throw socket_exception("select(2)", errno);
}



SocketSet& SocketSet::merge( const SocketSet& rhs)
{
	if( this != &rhs){
		this->m_sockets.insert( rhs.m_sockets.begin(), rhs.m_sockets.end());
	}
	return *this;
}

}//end of namespace qths
