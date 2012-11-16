/*
 * MessageCenter.h
 *
 *  Created on: Jul 26, 2011
 *      Author: xiaochuanq
 */

#ifndef MESSAGECENTER_H_
#define MESSAGECENTER_H_
#include <deque>
#include <string>
#include "Thread.h"
#include "Message.h"

class MessageCenter : public Synchronizable{
public:
	MessageCenter();
	~MessageCenter();
	void dispatch();
	void append();
	Timestamp sequence();
private:
	std::deque<Message> m_lstMsgQ;
	Timestamp m_tsSeqn;
};

#endif /* MESSAGECENTER_H_ */
