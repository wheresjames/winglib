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

oexUINT CLog::Log( oexCSTR x_pFile, oexINT x_nLine, oexINT x_uLevel, oexCSTR x_pErr, oexINT x_uErr, oexUINT x_uSkip )
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
			os::CSys::printf( oexStrToMb( oexGetModulePath( oexT( "debug.log\n" ) ) ).Ptr() );
#endif

			// Open new log file
			if ( !m_file.CreateAlways( oexGetModulePath( oexT( "debug.log" ) ).Ptr() ).IsOpen() )
			{	os::CSys::InterlockedDecrement( &m_lInLog );
				return x_uErr;
			} // end if

			// Create log header
			m_file.Write( CStr().Fmt( oexT( "; Log file\r\n; Date : %s\r\n; Application : %s\r\n\r\n" ),
									  oexStrToMbPtr( oexLocalTimeStr( oexT( "%W, %B %D, %Y - %h:%m:%s %A" ) ).Ptr() ),
									  oexStrToMbPtr( oexGetModuleFileName().Ptr() ) ) );

		} // end if

		// Do we have a source file name?
		if ( oexCHECK_PTR( x_pFile ) )
			m_file.Write( CStr().Fmt( oexT( "%s:(%u) " ), oexStrToMbPtr( x_pFile ), x_nLine ) );

		// Ensure error is not null
		if ( !oexCHECK_PTR( x_pErr ) || !*x_pErr )
			x_pErr = oexT( "Unspecified" );

		// Write out the user error string
		m_file.Write( CStr().Fmt( oexT( " : %s\r\n" ), oexStrToMbPtr( x_pErr ) ) );

		// Write out the system error code and string if not zero
		if ( x_uErr )
			m_file.Write( CStr().Fmt( oexT( "> 0x%X (%d) : %s\r\n" ), x_uErr, x_uErr, oexStrToMbPtr( os::CTrace::GetErrorMsg( x_uErr ).Ptr() ) ) );

		// Write out the backtrace
		m_file.Write( CStr().Fmt( oexT( "\t%s\r\n" ), oexStrToMbPtr( os::CTrace::GetBacktrace( x_uSkip ).Replace( oexT( "\n" ), oexT( "\n\t" ) ).Ptr() ) ) );

	} // end if

	// Reduce log count
	os::CSys::InterlockedDecrement( &m_lInLog );

	return x_uErr;
}

