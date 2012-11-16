/*
 * @brief Exceptions.h
 *
 * @date Aug 12, 2011
 * @author Xiaochuan Qin
 */

#ifndef EXCEPTIONS_H_
#define EXCEPTIONS_H_
#include <errno.h>
#include <stdexcept>

/*!
 * @brief Class runtime_exception
 *
 * runtime exception type in this library.
 */
class runtime_exception : public std::runtime_error
{
public:
	runtime_exception( const std::string err_msg, const char* func_name, int err_num );
	int 		errnum();
	const char* errstr();
	const char* raiser();
protected:
	int errorNum_;
	const char* functionName_;
};

/*!
 * @brief Class socket_exception
 *
 * Socket exception type.
 */

class socket_exception : public runtime_exception
{
public:
	socket_exception(const char* func_name, int errornum);
};

/*!
 * @brief Class thread_exception
 *
 * Thread exception type.
 */

class pthread_exception : public runtime_exception
{
public:
	pthread_exception(const char* func_name, int errornum);
	pid_t thread_id();
protected:
	pid_t 		m_tid;
};

#endif /* EXCEPTIONS_H_ */
