/*------------------------------------------------------------------
// log.cpp
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

#include "../oexlib.h"

OEX_USING_NAMESPACE

CLog CLog::m_logGlobal;

oexUINT CLog::Log( oexCSTR x_pFile, oexINT x_nLine, oexCSTR x_pFunction, oexINT x_uLevel, oexCSTR x_pErr, oexINT x_uErr, oexUINT x_uSkip )
{
	// Ensure valid reporting level
	if ( x_uLevel < m_uLevel )
		return x_uErr;

	// Log count
	if ( 1 == os::CSys::InterlockedIncrement( &m_lInLog ) )
	{
		// Create log file if needed
		if ( !m_file.IsOpen() )
		{
#if defined( oexDEBUG )
			// Show log file location
			os::CSys::printf( oexStrToMb( ( oexGetModuleFileName() += oexT( ".debug.log\n" ) ) ).Ptr() );
#endif
			// Open new log file
			if ( !m_file.CreateAlways( ( oexGetModuleFileName() += oexT( ".debug.log" ) ).Ptr() ).IsOpen() )
			{	os::CSys::InterlockedDecrement( &m_lInLog );
				return x_uErr;
			} // end if

			// Create log header
			m_file.Write( CStr().Fmt( oexT( ";====================================================================" ) oexNL
									  oexT( "; Log file" ) oexNL
									  oexT( "; Date : %s" ) oexNL
									  oexT( "; Application : %s" ) oexNL oexNL,
									  oexStrToMbPtr( oexLocalTimeStr( oexT( "%W, %B %D, %Y - %h:%m:%s %A" ) ).Ptr() ),
									  oexStrToMbPtr( oexGetModuleFileName().Ptr() ) ) );

		} // end if


		if ( !oexCHECK_PTR( x_pFile ) )
			x_pFile = oexT( "???" );

		if ( !oexCHECK_PTR( x_pFunction ) )
			x_pFunction = oexT( "???" );

		CStr sLevel;
		oexCSTR pLevel = oexT( "" );
		if ( eHalt == x_uLevel )
			pLevel = oexT( "Halt" );
		else if ( eError == x_uLevel )
			pLevel = oexT( "Error" );
		else if ( eWarning == x_uLevel )
			pLevel = oexT( "Warning" );
		else if ( eNotice == x_uLevel )
			pLevel = oexT( "Notice" );
		else
			pLevel = sLevel.Fmt( "%d", (int)x_uLevel ).Ptr();

		CStr sLog;

		// Add file / line number
		sLog << x_pFile << oexT( ":(" ) << x_nLine << oexT( ")" ) oexNL;

		// Write out the time
		sLog << oexGmtTimeStr( oexT( " -> %w, %d %b %Y %g:%m:%s GMT" ) )
		     << oexLocalTimeStr( oexT( " -- Local: %Y/%c/%d - %g:%m:%s.%l" ) oexNL );

		// Add error level
		sLog << oexT( " -> " ) << pLevel;

		// Add system error info
		if ( x_uErr )
			sLog << CStr().Fmt( oexT( " : 0x%X (%d) : " ), x_uErr, x_uErr )
			 	 << os::CTrace::GetErrorMsg( x_uErr );

		sLog << oexNL;

		// Add function name if available
		if ( x_pFunction && *x_pFunction )
			sLog << oexT( " -> " ) << x_pFunction << oexT( "()" ) oexNL;

		// Write out the user error string if available
		if ( oexCHECK_PTR( x_pErr ) )
			sLog << oexT( " -> " ) << CStr( x_pErr ).Replace( oexNL, oexNL oexT( " -> " ) ) << oexNL;

#ifdef oexBACKTRACE_IN_LOG
		// Write out the backtrace
		sLog << oexT( " -> " )
		     << os::CTrace::GetBacktrace( x_uSkip ).Replace( oexNL, oexNL oexT( " -> " ) )
		     << oexNL;
#endif

		// Just to space things out
		sLog << oexNL;

		// Write out the string to the file
		m_file.Write( oexStrToMb( sLog ) );

	} // end if

	// Reduce log count
	os::CSys::InterlockedDecrement( &m_lInLog );

	return x_uErr;
}

