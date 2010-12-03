#include "execthread.h"

ExecThread::ExecThread( eString command ) :
	command( command )
{
}

void ExecThread::start( void )
{
	run();
}

void ExecThread::thread( void )
{
	retVal = system( command.c_str() );
}

void ExecThread::thread_finished( void )
{
	/* emit */ allDone( retVal );
}
