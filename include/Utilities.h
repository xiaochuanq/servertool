/*
 * Utilities.h
 *
 * @date Aug 18, 2011
 * @author xiaochuanq
 */

#ifndef UTILITIES_H_
#define UTILITIES_H_

#include <ostream>
#include <vector>

#ifdef CPP_0X
#include <unordered_map>
using std::unordered_map;
#else
#include <tr1/unordered_map>
using std::tr1::unordered_map;
#endif

#ifdef __DARWIN_UNIX03
	#ifndef _MACOS_X
		#define _MACOS_X 1
	#endif
#endif

#ifdef _MACOS_X
pid_t gettid();
#endif

/*!
 * @brief Pointer Deletion Functor type
 */
template<typename T>
struct del_fun_t {
	void operator()(T* p) {
		delete p;
	}
};

/*!
 * @brief Uncopyable type.
 */
class Uncopyable
{
protected:
	Uncopyable(){};
	~Uncopyable(){};
private:
	Uncopyable( const Uncopyable&);
	Uncopyable& operator=(const Uncopyable&);
};


/*
 * @brief Quick generator of an object of Pointer Deletion Functor type
 */
template<typename T>
del_fun_t<T> deletor() {
	return del_fun_t<T>();
}

/*
 * @brief Element Printing Functor type
 */
template<typename T>
struct print_elem_t {
	print_elem_t( std::ostream& outstream,
			const std::string& delimiter = "; " )
		: outs(outstream), delim( delimiter) {};
	void operator()(T obj) {
		outs << obj << delim;
	}
	std::ostream& outs;
	std::string delim;
};

/*
 * @brief Quick generator of an object of Element Printing type
 */
template<typename T>
print_elem_t<T> printor(std::ostream& outs, const char* delim){
	return print_elem_t<T>(outs, delim);
}

/*!
 * @brief struct SourcePoint
 *
 * SourcePoint is used to cooperate with a call stack to trace the raising points
 * (file name and line number) of exceptions. The initial design is credit to user
 * 6502 at stackoverflow.com. The original discussions can be found at:
 * http://stackoverflow.com/questions/4562135/how-to-get-error-line-number-in-c-program
 */

struct SourcePoint
{
    const char *file;
    int line;
    SourcePoint(const char *filename = 0, int linenum = 0)
      : file(filename), line(linenum) { };
    bool is_blockmarker() const { return file == 0; }
};

std::ostream& operator<<( std::ostream& outs, const SourcePoint& sp);

/*!
 * @brief class CallStack. A call stack is used to trace the last marked source point.
 *
 * Each thread should use its own call stack(s) to avoid synchronization problem. Defining
 * NDEBUG to opt out the operations on call stacks.
 *
 * Bugs: Cannot handle re-throw correctly.
 */

class CallStack{

public:
	void 				clear()							{ m_stack.clear(); }
	void 				push( const SourcePoint& sp )	{ m_stack.push_back( sp ); }
	void 				pop()							{  m_stack.pop_back(); }
	const SourcePoint& 	top()							{ return m_stack.back(); }
	size_t 				size() 							{ return m_stack.size(); }
	void 				dump(std::ostream& outs);
private:
	std::vector<SourcePoint> m_stack;
};

/*!
 * @brief Call stack hash table. Used in singleton pattern.
 * It is suggested to use thread id as the accessing key.
 */
typedef unordered_map< int, CallStack> CallstackTable;

CallstackTable& theCallstackTable(); ///< Get reference to the ONLY call stack hash table.

struct SourcePointMarker
{
    SourcePointMarker( int stack_id, const char *filename, int line)
    {
    	theCallstackTable()[stack_id].push( SourcePoint(filename, line) );
    }
};

#ifndef NDEBUG
#define BEGIN_TRYBLOCK do{ SourcePointMarker mark_a_block( gettid(), 0, 0 ); }while(0)
#define MARK_SOURCEPOINT do{ SourcePointMarker mark_a_point( gettid(), __FILE__, __LINE__); } while(0)

#define LAST_ERRORPOINT theCallstackTable()[gettid()].top()

#define END_TRYBLOCK do{ \
						if( theCallstackTable()[gettid()].size() ){ \
							while( !theCallstackTable()[gettid()].top().is_blockmarker() ) \
								theCallstackTable()[gettid()].pop(); \
							theCallstackTable()[gettid()].pop(); \
						} \
					}while(0)

#else
#define BEGIN_TRYBLOCK
#define MARK_SOURCEPOINT
#define LAST_ERRORPOINT
#define END_TRYBLOCK
#endif

#endif /* UTILITIES_H_ */
