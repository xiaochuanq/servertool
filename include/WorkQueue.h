/*
 * WorkQueue.h
 *
 *  Created on: Jul 6, 2011
 *      Author: Xiaochuan Qin
 */

#ifndef WORKQUEUE_H_
#define WORKQUEUE_H_
#include "Thread.h"
#include <list>

template<typename Work, typename WorkerThread>
class WorkQueue
{
public:
	WorkQueue( int minthread, int maxthread, int maxwait);
	~WorkQueue();
	void add(Work*);
	void add(WorkerThread*);
protected:
	int num_work(){ return m_lstWork.size(); } // number of waiting work
	int num_idle(){ return m_lstThreadpool.size(); }// number of idle threads
	void destroy();
private:
	std::list<Work*> m_lstWork;
	std::list<WorkerThread*> m_lstThreadpool;
	Mutex m_mutexWorklist;
	Mutex m_mutexTheadpool;
	Condition m_condNotempty;
	int m_nThreadCounter;
	int m_nMinthreads; //minimum parallelism
	int m_nMaxthreads; //maximum parallelism
	bool m_bQuit; // flag on quit or not
	bool m_bValid; // flag on validity
};

#endif /* WORKQUEUE_H_ */
