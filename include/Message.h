/*
 * Message.h
 *
 *  Created on: Jul 3, 2011
 *      Author: Xiaochuan Qin
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_
#include <stdint.h>
#include <string>
/*
 *
#include <time.h>
struct Timestamp{
	time_t time;
	const Thread* pThread;
}; */

class Thread;
class User;

typedef uint64_t Timestamp;

class Message {
public:
	static const int ROOMMSG = 0;
	static const int USERMSG = 1;
public:
	Message(const char* msg, const Thread* senderThread);
	~Message();
	inline const Timestamp& timestamp();
private:
	std::string m_strFrom;
	std::string m_strTo;
	std::string m_strBody;
	Timestamp m_timestamp;
	int type;
};

#endif /* MESSAGE_H_ */
