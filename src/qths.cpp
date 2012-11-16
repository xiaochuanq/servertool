/*!
 * @brief qths.cpp
 *
 * @date Sep 1, 2011
 * @author Xiaochuan Qin
 */

#include "qths.h"

namespace qths{

void get_realtime_usec( struct timeval& tv)
{
    gettimeofday(&tv, NULL);
}

void get_realtime_nsec(struct timespec& ts)
{
#if defined(_POSIX_TIMERS) && _POSIX_TIMERS > 0
    clock_gettime(CLOCK_REALTIME, &ts);
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    ts.tv_sec = tv.tv_sec;
    ts.tv_nsec = tv.tv_usec * 1000;
#endif
}


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


std::ostream& operator<<( std::ostream& outs, const SourcePoint& sp)
{
	outs << "line " << sp.line << " in file " << sp.file;
	return outs;
}

void CallStack::dump(std::ostream& outs){
	std::for_each( m_stack.begin(), m_stack.end(), printor<SourcePoint>(outs, "\n"));
}

CallstackTable& theCallstackTable()
{
#ifndef NDEBUG
	static CallstackTable* ptable = 0;
	if( !ptable )
		ptable = new CallstackTable();
	return *ptable;
#else
	return 0;
#endif
}


}//End of namesapce qths
