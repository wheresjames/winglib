/*------------------------------------------------------------------
// service.cpp
//
// Copyright (c) 1997
// Robert Umbehant
// winglib@wheresjames.com
// http://www.wheresjames.com
//
// Redistribution and use in source and binary forms, with or
// without modification, are permitted for commercial and
// non-commercial purposes, provided that the following
// conditions are met:
//
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// * The names of the developers or contributors may not be used to
//   endorse or promote products derived from this software without
//   specific prior written permission.
//
//   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
//   CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
//   INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
//   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
//   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
//   NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
//   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
//   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
//   EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//----------------------------------------------------------------*/

#include "../../../oexlib.h"
#include "std_os.h"

OEX_USING_NAMESPACE
using namespace OEX_NAMESPACE::os;

oexINT CService::Fork( CStr x_sWorkingDirectory )
{
	pid_t pid, sid;

	// Fork from the parent
	pid = fork();
	if ( 0 > pid )
	{	oexERROR( errno, oexT( "fork() failed" ) );
		return -1;
	} // end if

	// Exit parent process
	if ( 0 < pid )
	{	CStr sStr = CStr().Fmt( oexT( "fork() = %d (0x%x);" ), (int)pid, (int)pid );
		oexNOTICE( 0, sStr.Ptr() );
		CSys::printf( ( sStr << oexT( "\n" ) ).Ptr() );
		return pid;
	} // end if

	// Change file mode mask
	umask( 0 );

	// Child process needs a SID
	sid = setsid();
	if ( 0 > sid )
	{	oexERROR( errno, oexT( "setsid() failed" ) );
		return -1;
	} // end if

	// Switch to custom log file
	CLog::GlobalLog().OpenLogFile( oexNULL, oexNULL, oexT( ".fork.debug.log" ) );

	// Log child sid
	oexNOTICE( 0, CStr().Fmt( oexT( "Child fork() from %s : setsid() = %n" ), (int)sid ) );

	// Use the module path as the current working directory
	if ( x_sWorkingDirectory.Length() )
		if ( 0 > chdir( oexStrToMbPtr( x_sWorkingDirectory.Ptr() ) ) )
		{	oexERROR( errno, CStr().Fmt( oexT( "chdir( '%s' ) failed" ), oexStrToMbPtr( x_sWorkingDirectory.Ptr() ) ).Ptr() );
			return -1;
		} // end if

	// No more terminals
	if ( 0 > close( STDIN_FILENO ) )
		oexWARNING( errno, oexT( "Unable to close STDIN_FILENO" ) );
	if ( 0 > close( STDOUT_FILENO ) )
		oexWARNING( errno, oexT( "Unable to close STDIN_FILENO" ) );
	if ( 0 > close( STDERR_FILENO ) )
		oexWARNING( errno, oexT( "Unable to close STDIN_FILENO" ) );

	// Return from child process
	return 0;
}

oexINT CService::Run( CStr x_sModule )
{
	// Fork the process, return if parent or failure
	if ( oexINT nRet = Fork( x_sModule.GetPath() ) )
		return nRet;

	// We're in the child fork() now...

	// Load the module
	CModule mod;
	if ( !mod.Load( x_sModule.Ptr() ) )
		return -1;



	return 0;
}
