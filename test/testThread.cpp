//============================================================================
// Name        : testThread.cpp
// Author      : Xiaochuan Qin
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>

#include "Thread.h"

using namespace std;

Condition* pcond = 0;
Mutex* cout_mx;

class MyThread: public Thread {
public:
	MyThread(int x) :
			m_x(x) {
	}
	;
	virtual void exec() {

		wait(*pcond);
		m_x *= m_x;
		{
			Lock lock(cout_mx);
			cout << m_x << " calculated by thread " << gettid() << endl;
		}
		int i = 1;
		while (i) {
		//	sleep(1); // sleep function is a cancel point.
			//see http://publib.boulder.ibm.com/infocenter/iseries/v5r3/index.jsp?topic=%2Fapis%2Fusers_39.htm
			{
				Lock lock(cout_mx);
				cout << m_x << " has been kept " << i << " times." << endl;
				++i;
			}
			test_cancel(); // Use ASYNCHRONOUS type cancel can make a thread be cancelled even without any cancel point.
			/*
			 * The following functions are cancellation points:

    pthread_cond_timedwait()
    pthread_cond_wait()
    pthread_delay_np()
    pthread_join()
    pthread_join_np()
    pthread_extendedjoin_np()
    pthread_testcancel()
			 *
			 */
		}
		pthread_exit(&m_x);
	}

	int m_x;

};

int main() {
	MyThread* threads[5];
	int sum = 0;
	BEGIN_TRYBLOCK;
	try {
		Mutex mx(pcond);
		cout_mx = new Mutex();
		pcond = new Condition(&mx);
		for (int i = 0; i < 5; ++i) {
			threads[i] = new MyThread(i + 1);
			threads[i]->enable_cancel(true);
			threads[i]->set_cancel_type(Thread::CANCEL_DEFERED);
			threads[i]->start();
			sum += (i + 1) * (i + 1);
		}
		int *square, ssum = 0;
		::sleep(3);
		{
			Lock lock(cout_mx);
			cout << "Now sending a signal to every thread" << endl;
		}
		/*
		 * Sleep for a while is necessary. Because if signal is sent before thread starting, they will never be
		 * able to join the main thread.
		 */
		for (int i = 0; i < 5; ++i)
			pcond->signal();
		sleep(3);
		{
		Lock lock(cout_mx);
		cout << "Five signals sent. Now canceling threads one by one every 2 seconds." << endl;
		}
		for (int i = 0; i < 5; ++i) {
			threads[i]->cancel();
			{
				Lock lock(cout_mx);
				cout << " Canceling thread " << i << endl;
			}
			sleep(2);
		}

/*		cout << "All threads have been canceled. Now are let them join and collecting results" << endl;
		for (int i = 0; i < 5; ++i) {
			threads[i]->join((void**) &square);
			ssum += *square;
		}
			cout << "Sum Squared = " << ssum << endl;
			cout << "Supposed to be " << sum;
*/
		sleep(3600);
		cout << " Now deleting all threads." << endl;
		for (int i = 0; i < 5; ++i)
			delete threads[i];
		delete pcond;
		delete cout_mx;
	} catch (pthread_exception& pe) {
		cout << "At " << LAST_ERRORPOINT;
		cout << pe.what() << " raised by " << pe.raiser() << "; Error: "
				<< pe.errstr() << endl;
	}
	END_TRYBLOCK;
	return 0;
}
