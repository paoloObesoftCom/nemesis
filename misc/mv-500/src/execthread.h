#ifndef __EXECTHREAD_H__
#define __EXECTHREAD_H__

#include <lib/base/thread.h>
#include <lib/base/estring.h>
	
class ExecThread : public eThread
{
	eString command;
	int retVal;

	void thread( void );
	void thread_finished( void );
public:
	ExecThread( eString command );
	void start( void );
	Signal1<void,int> allDone;
};

#endif
