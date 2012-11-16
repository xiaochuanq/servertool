/*!
 * @brief Socket.h
 * @date Jul 5, 2011
 * @author Xiaochuan Qin
 *
 * A thin wrap around UNIX socket. It is not thread-safe.
 */

#ifndef SOCKET_H_
#define SOCKET_H_

#include <errno.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <string>

/*!
 * @brief Class socket_exception
 *
 * Socket exception type.
 */

class socket_exception : public runtime_exception
{
public:
	socket_exception(const char* func_name, int errornum);
};



/*!
 * @brief SocketImpl Class
 *
 * Base class of classes Socket and ServerSocket
 */

class SocketImpl
{
protected:
    SocketImpl(int socket = -1) :
        socket_(socket),
	backlog_(0),
        closed_(true),
        bound_(false),
        non_blocking_(false)
    {
        timeout_.tv_sec = -1;
        timeout_.tv_usec = -1;
    }
public:
    void bind( const std::string& ip, int port);
    void listen();
    std::string read( size_t maxlength) const;
    void write( const std::string& msg) const;
    void close();
    // Status/option change functions
    void set_nonblocking( bool nonblocking = true );
    void set_timeout( int sec, int usec = 0);
public:
    int  fd() 			const
    {
        return socket_;    ///< File descriptor associated with a Socket Object.
    }
    bool bound() 		const
    {
        return bound_;
    }
    bool closed() 		const
    {
        return closed_;
    }
    bool nonblocking()	const
    {
        return non_blocking_;
    }
    const struct timeval& timeout() const
    {
        return timeout_;
    }
protected:
    void create_socket();
    void make_address(struct sockaddr_in* paddr, const std::string& ip, int port);
private:
    int  socket_;			///< File descriptor, i.e. the socket handle.=
    int  backlog_;
    bool closed_;			///< Flag of validity.
    bool bound_;			///< Flag of binding status.
    bool non_blocking_;	///< Flag of asynchronous/synchronous state.
    struct timeval timeout_;
};

#ifdef _LINUX
struct SocketKeepAliveOption
{
    int keepalive; 		///< Boolean flag of keep alive or not.
    int timer;			///< Max timer or timeout before disconnection. Unit: millisecond;
    int interval;		///< Max time between two keep-alive probes. Unit: millisecond;
    int count;			///< Max number of keep-alive probes to be sent before disconnection.
};
#endif

/*!
 * @brief Socket Class
 */
class Socket : public SocketImpl
{
    friend class ServerSocket;
public:
    // ctor and dtor
    Socket(); 									///< Default Constructor
    Socket(const std::string& ip, int port);	///< Constructor which creates a socket using IP and port. It binds socket in the mean time.
    ~Socket();
private:
    Socket(int socket); 						///< Constructor which creates a socket from a file descriptor. Used only by ServerSocket in accept();
public:
    void connect( const std::string& ip, int port);		///< Connect to a remote host be ip address and port.
    void enable_keepalive( bool );						///< Enable/Disable keep alive probes at TCP leve.
    void set_keepalive_timer(int timeout);				///< Set the keepalive idle timer. Unit: millisecond.
    int  get_keepalive_timer();							///< Get the keepalive idle timer. Unit: millisecond.
    void keepalive( int timeout);						///< An quick setup of keep alive options by specifying the (idle timer) timeout before considered disconnection.
    bool keepalive_enalbed();
#ifdef _LINUX
    void set_keepalive_interval(int tv);				///< Set the time between two keepalive probes. Unit: millisecond.
    void set_keepalive_cnt( int count);					///< Set the number of probes before timeout.
    void set_keepalive_option( const SocketKeepAliveOption&); ///< Set the three parameters mentioned above.
    int  get_keepalive_interval();				///< Get the time between two keepalive probes. Unit: millisecond.
    int  get_keepalive_cnt( );				///< Get the number of probes before timeout.
    void get_keepalive_option( SocketKeepAliveOption&);	///< Get the three parameters mentioned above.
#endif
    int byte_pending_read();		///< Get the number of bytes in the input buffer to be read.
    int byte_pending_write();		///< Get the number of bytes in the output buffer to be written.

    bool connected() const
    {
        return closed() && connected_;
    }
    bool alive()
    {
        return alive_;    ///< Check if the socket is disconnected remotedly.
    }
protected:
    bool connected_;		///< Flag of connection state.
    bool alive_;			///< Flag of aliveness/
};



/*!
 * @brief Class ServerSocket
 *
 *
 */

class ServerSocket : public SocketImpl
{
public:
    ServerSocket(const std::string& ip, int port, int backlog = -1);
    ~ServerSocket();
    Socket accept();

private:
    int backlog_;
};


#endif /* SOCKET_H_ */
