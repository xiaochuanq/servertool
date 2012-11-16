//============================================================================
// Name        : testWorkQ.cpp
// Author      : Xiaochuan Qin
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
#include <climits>
#include <ctime>
#include <iostream>
#include "WorkQueue.hpp"

using namespace std;
using namespace qths;

class MyTask
{
public:
	MyTask(int id ) : m_id(id){};
	~MyTask(){
		//std::cout <<" Task #"<< m_id << "has been destroyed." << endl;
	}
	void handle()
	{
		//std::cout << "Task #" << m_id <<" is being processed!" << endl;
		double a = 1.0;
		for(unsigned long i = 0; i < 200000; ++i){
			if( a < 1e22 )
				a*=2;
			else
				a = 1.0;
		}
	//	std::cout << "Task #" << m_id <<" has been processed!" << endl;
	}
private:
	int m_id;
};
int main() {
	WorkQueueManager<MyTask> wq(10);
	for( int i = 0; i < 1000; ++i){
		wq.add_work( new MyTask(i) ) ;
	}
	cout << "Threads begin" << endl;
	std::time_t t1 = std::time(0);
	wq.terminate();
	std::time_t t2 = std::time(0);
	cout << "It is done. time used" << t2 - t1 <<endl;
	return 0;
}
