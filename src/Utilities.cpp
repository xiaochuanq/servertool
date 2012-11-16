/*
 * Utilities.cpp
 *
 * @date Aug 18, 2011
 * @author xiaochuanq
 */

#include <algorithm>
#include "Utilities.h"


#ifdef _MACOS_X
pid_t gettid()
{
	return pthread_mach_thread_np( pthread_self() );
}
#endif

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
	static CallstackTable* ptable = 0;
	if( !ptable )
		ptable = new CallstackTable();
	return *ptable;
}

