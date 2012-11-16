/*!
 * Exceptions.cpp
 *
 * @date 	Aug 13, 2011
 * @author 	Xiaochuan Qin
 */


#include <pthread.h>
#include <string.h>
#include <sys/types.h>

#include <stdexcept>

#include "Exceptions.h"
#include "Utilities.h"

runtime_exception::runtime_exception(const std::string err_msg,
		const char* func_name, int err_num) :
		std::runtime_error( err_msg ),
		errorNum_( err_num ),
		functionName_( func_name )
{

}

int runtime_exception::errnum()
{
	return errorNum_;
}

const char* runtime_exception::errstr()
{
	return strerror(errorNum_);
}

const char* runtime_exception::raiser()
{
	return functionName_;
}

socket_exception::socket_exception(const char* func_name, int errornum) :
		runtime_exception("Socket Exception", func_name, errornum)
{

}

pthread_exception::pthread_exception(const char* func_name, int errornum) :
		runtime_exception("POSIX Thread Exception", func_name, errornum),
		m_tid( gettid() )
{

}

inline pid_t pthread_exception::thread_id() {
	return m_tid;
}

