/*
 * Thread.cpp
 *
 *  Created on: Jul 3, 2011
 *      Author: Xiaochuan Qin
 */
#include <time.h>
#include <unistd.h>

#include <iostream>
#include "Thread.h"

/*
 * Thread.h
 *
 *  Created on: Jul 3, 2011
 *      Author: Xiaochuan Qin
 */


Lock::Lock(Mutex* pm) :
	m_pmutex(pm)
{
	m_pmutex->lock();
}


Lock::~Lock()
{
	m_pmutex->unlock();
}



/************ Class Synchronizable **************/

Synchronizable::Synchronizable()
	: m_mutex(this)
	{}

Mutex* Synchronizable::mutex_ptr()
{
	return &m_mutex;
}

/************ Class Mutex ***********************/
Mutex::Mutex(const void* pdata) :
		m_pdata(pdata)
{
	int error = pthread_mutex_init(&m_mutex, NULL);
	if (error)
		throw pthread_exception("pthread_mutex_init())", error);
}

Mutex::~Mutex()
{
	pthread_mutex_destroy(&m_mutex);
}


pthread_mutex_t* Mutex::rawtype_ptr()
{
	return &m_mutex;
}

void Mutex::lock()
{
	int error = pthread_mutex_lock(&m_mutex);
	if ( error)
		throw pthread_exception("pthread_mutex_lock()", error);
}

void Mutex::unlock()
{
	int error = pthread_mutex_unlock(&m_mutex);
	if ( error)
		throw pthread_exception("pthread_mutex_unlock()", error);
}

bool Mutex::trylock()
{
	int error = pthread_mutex_trylock(&m_mutex);
	if ( error != 0 && error != EBUSY)
		throw pthread_exception("pthread_mutex_trylock()", error);
	return error == 0; // if it is locked by this call.
}

/************ Class Condition ******************/
Condition::Condition(Mutex* pm) : m_pmutex( pm )
{
	int error = pthread_cond_init( &m_cv, NULL);
	if(	error)
		throw pthread_exception("pthread_cond_destroy()", error);
}

Condition::~Condition()
{
	int error = pthread_cond_destroy( &m_cv);
	if(	error)
		std::cerr << " Error in destroying condition variable.";
}

pthread_cond_t* Condition::rawtype_ptr()
{
	return &m_cv;
}

void Condition::signal()
{
	int error = pthread_cond_signal( &m_cv);
	if(	error)
		throw pthread_exception("pthread_cond_signal()", error);
}

void Condition::broadcast()
{
	int error = pthread_cond_broadcast( &m_cv);
	if(	error)
		throw pthread_exception("pthread_cond_broadcast()", error);
}

void Condition::wait(int sec, long ns)
{
	int error;
	//m_pmutex->lock();
	Lock lock( m_pmutex );
	if( sec & ns){
		struct timespec timeout = { time(0) + sec, ns };
		error = pthread_cond_timedwait( &m_cv, m_pmutex->rawtype_ptr(), &timeout );
	}
	else{
		error = pthread_cond_wait( &m_cv, m_pmutex->rawtype_ptr());
	}
	if( error != 0 && error != ETIMEDOUT )
		throw pthread_exception("pthread_cond_timedwait()", error);

}

/******* Class Thread ************************/

Thread::Thread(){
	int error = pthread_attr_init(&m_attr);
	if( error)
		throw pthread_exception("pthread_attr_init(3)", error);
}

Thread::~Thread() {
	int error = pthread_attr_destroy(&m_attr);
	if( error){
		std::cerr << "Error in destroying pthread: " << strerror( error ) << std::endl;
	}
}

void* Thread::thread_fun( void* this_ptr)
{
	Thread* pthr = reinterpret_cast<Thread*> (this_ptr);
	pthr->exec();
	return 0;
}


pthread_t* Thread::rawtype_ptr()
{
	return &m_thread;
}

void Thread::start()
{
	int error = pthread_create( &m_thread, &m_attr, Thread::thread_fun,  (void *)this);
	if( error)
		throw pthread_exception("pthread_create(3)", error);
}

void Thread::join(void** ret_val_ptr)
{
	int error = pthread_join(m_thread, ret_val_ptr);
	if( error)
		throw pthread_exception("pthread_join(3)", error);
}

void Thread::wait(Condition& cond, int sec, long ns)
{
	cond.wait( sec, ns);
}

void Thread::exec()
{
	pthread_exit(0);
}

void Thread::cancel()
{
	int error = pthread_cancel( m_thread);
	if( error )
		throw pthread_exception("pthread_cancel()", error);
}

void Thread::detach()
{
	int error = pthread_detach( m_thread );
	if( error )
		throw pthread_exception("pthread_detach()", error);
}

void Thread::set_stack_size( size_t sz)
{
	int error = pthread_attr_setstacksize( &m_attr, sz);
	if( error )
		throw pthread_exception("pthread_attr_setstacksize()", error);
}

size_t Thread::stack_size()
{
	size_t sz;
	int error = pthread_attr_getstacksize( &m_attr, &sz);
	if( error )
		throw pthread_exception("pthread_attr_getstacksize()", error);
	return sz;
}

void Thread::set_schedule_policy( SchedulePolicy policy)
{
	int error = pthread_attr_setschedpolicy( &m_attr, policy);
	if( error )
		throw pthread_exception("pthread_attr_setschedpolicy()", error);
}

Thread::SchedulePolicy Thread::schedule_policy()
{
	SchedulePolicy policy;
	int error = pthread_attr_getschedpolicy( &m_attr, (int*)&policy);
	if( error )
		throw pthread_exception("pthread_attr_getschedpolicy()", error);
	return policy;
}


void Thread::set_detach_state(DetachState ds)
{
	int error = pthread_attr_setdetachstate( &m_attr, ds);
	if( error )
		throw pthread_exception("pthread_attr_setdetachstate()", error);
}

Thread::DetachState Thread::detach_state()
{
	DetachState ds;
	int error = pthread_attr_getdetachstate( &m_attr, (int*)&ds);
	if( error )
		throw pthread_exception("pthread_attr_getdetachstate()", error);
	return ds;
}


Thread::CancelType Thread::set_cancel_type(CancelType newtype)
{
	CancelType oldtype;
	int error = pthread_setcanceltype( (int)newtype, (int*)&oldtype);
	if( error )
		throw pthread_exception("pthread_setcanceltype()", error);
	return oldtype;
}


bool Thread::enable_cancel( bool enable)
{
	int oldstate;
	int newstate = enable ? PTHREAD_CANCEL_ENABLE : PTHREAD_CANCEL_DISABLE;
	int error = pthread_setcancelstate( newstate, &oldstate );
	if( error )
		throw pthread_exception("pthread_setcancelstate()", error);
	return oldstate;
}

