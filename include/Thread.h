/*
 * Thread.h
 *
 *  Created on: Jul 3, 2011
 *      Author: Xiaochuan Qin
 */

#ifndef THREAD_H_
#define THREAD_H_
#include <pthread.h>

#include "Exceptions.h"
#include "Utilities.h"

/*!
 * @brief Get calling thread's ID.
 */
pid_t gettid();


class Thread;

class Mutex
{
public:
	Mutex(const void* pdata = 0);
	~Mutex();
	void lock();
	void unlock();
	bool trylock();
	pthread_mutex_t* rawtype_ptr();
private:
	pthread_mutex_t m_mutex;
	const void *m_pdata; // pointer to the data to be protected.
};

class ReadWriterLock
{

};

class Synchronizable
{
public:
	Synchronizable();
protected:
	Mutex self(){ return m_mutex; }
	Mutex* mutex_ptr();
private:
	Mutex m_mutex;
};

//
class Lock : public Uncopyable
{
public:
	explicit Lock( Synchronizable& );
	explicit Lock( Synchronizable* );
	explicit Lock( Mutex* pm);
	explicit Lock( Mutex& mx){ m_pmutex = &mx; }
	~Lock();
private:
	Mutex* m_pmutex;
};

class Condition : public Uncopyable
{
public:
	Condition(Mutex*);
	~Condition();
	void wait(int timeout_sec = 0, long timeout_ns = 0);
	void signal();
	void broadcast();
protected:
	pthread_cond_t* rawtype_ptr();
private:
	pthread_cond_t m_cv;
	Mutex * m_pmutex;
};


class Thread {
public:
	enum CancelType{
		CANCEL_DEFERED = PTHREAD_CANCEL_DEFERRED,
		CANCEL_ASYNCHRONOUS = PTHREAD_CANCEL_ASYNCHRONOUS
	};
	enum SchedulePolicy{
		FIFO = SCHED_FIFO,
		ROUNDROBIN = SCHED_RR,
		OTHER = SCHED_OTHER
	};
	enum DetachState{
		DETACHED = PTHREAD_CREATE_DETACHED,
		JOINABLE = PTHREAD_CREATE_JOINABLE
	};

	static void test_cancel(){ pthread_testcancel(); }
protected:
	static void* thread_fun( void *);
public:
	Thread();
	virtual ~Thread();
	void start();
	void wait( Condition&, int sec = 0, long ns = 0 );
	void join(void** ret_val_ptr = 0);
	void cancel();
	void detach();
public:
	void set_stack_size( size_t stacksize );
	size_t stack_size();
	void set_schedule_policy( SchedulePolicy);
	SchedulePolicy schedule_policy();
	CancelType set_cancel_type( CancelType );
	bool enable_cancel( bool);
	void set_detach_state(DetachState);
	DetachState detach_state();
	pthread_t* rawtype_ptr();
protected:
    virtual void exec() = 0;
protected:
    pthread_attr_t m_attr;
    pthread_t m_thread;
};

#endif /* THREAD_H_ */
