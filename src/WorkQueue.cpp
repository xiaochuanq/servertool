/*
 * WorkQueue.cpp
 *
 *  Created on: Jul 6, 2011
 *      Author: Xiaochuan Qin
 */

#include "Thread.h"
#include "WorkQueue.h"

template<typename Work, typename WorkerThread>
WorkQueue<Work, WorkerThread>::WorkQueue( int minthread, int maxthread, int maxwait ) :
	m_mutexWorklist(&this->m_lstWork),
	m_condNotempty(&this->m_mutexWorklist),
	m_nMinthreads( minthread ),
	m_nMaxthreads( maxthread ),
	m_bQuit(false),
	m_bValid(true)
{
	int status;
	status = pthread_attr_init( & (this->m_threadattr) );
	if( status)
		throw pthread_exception( "Failed to initialize thread attribute (pthread_attr_init()).");
	status = pthread_attr_setdetachstate( &this->m_threadattr, PTHREAD_CREATE_DETACHED );
	if( status ){
		pthread_attr_destroy( & (this->m_threadattr) );
		throw pthread_exception( "Failed to set thread detach state (pthread_attr_setdetachstate()).");
	}
	status = pthread_cond_init(& (this->m_cv), NULL);
	if( status){
		pthread_attr_destroy(& (this->threadattr) );
		throw pthread_exception( "Failed to initialize conditional variable(pthread_cond_init()).");
	}

}

template<typename Work, typename WorkerThread>
WorkQueue<Work, WorkerThread>::~WorkQueue() {
	if( m_bValid )
		destroy();
}

template<typename Work, typename WorkerThread>
void WorkQueue<Work, WorkerThread>::destroy(){
	m_mutexWorklist.lock();
	m_bValid = false;
	int status;
	if( !m_lstWork.empty() ){
		m_bQuit = true;
		if( this->num_idle() > 0){
			m_condNotempty.broadcast();
			m_mutexWorklist.unlock();
		}
		while( this->num_work() > 0 ){
			m_condNotempty.wait();
			m_mutexWorklist.unlock();
		}
	}
	m_mutexWorklist.unlock();
}

template<typename Work, typename WorkerThread>
void WorkQueue<Work, WorkerThread>::add(Work* pwork) {
	m_mutexWorklist.lock();
	m_lstWork.push_back(pwork);
	if (!m_lstWork.empty()) {

		if (m_lstWork.size() < m_nMaxthreads) {

		}
	}
	m_mutexWorklist.unlock();
}

template<typename Work, typename WorkerThread>
void WorkQueue<Work, WorkerThread>::add(WorkerThread* pthread)
{
	m_lstThreadpool.push_back( pthread );
}
