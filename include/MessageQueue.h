/*
 * MessageRoller.h
 *
 *  Created on: Apr 9, 2012
 *      Author: xiaochuan Qin
 */

#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H
#include <list>
#include <vector>

class Message;

class MessageQueue {
public:
	typedef std::list<const Message*>::const_iterator msg_iterator;
public:
	MessageQueue(size_t size);
	~MessageQueue();
	void push_back(const Message*);
	Message* pop_front();
private:
	std::vector<std::list<const Message*> > _msg_table;
	size_t _table_size;
};

#endif /* MESSAGEQUEUE_H */
