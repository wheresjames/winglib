/*------------------------------------------------------------------
// debug.cpp
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

#include <crtdbg.h>

OEX_USING_NAMESPACE
using namespace OEX_NAMESPACE::os;

// http://www.codeguru.com/Cpp/W-P/dll/tips/article.php/c3635/
// http://www.dotnet247.com/247reference/msgs/13/65259.aspx

oexCPVOID CDebug::GetInstanceHandle()
{
#if defined( __ImageBase )

	return (oexCPVOID)&__ImageBase;

#else

	MEMORY_BASIC_INFORMATION mbi;
	static int dummy;
	VirtualQuery( &dummy, &mbi, sizeof( mbi ) );

	return (oexCPVOID)mbi.AllocationBase;

#endif
}

void CDebug::Trace( oexCSTR x_pStr )
{
/*
#if defined( oexUNICODE )
#   define TRACE0( s )  _CrtDbgReportW( _CRT_WARN, 0, 0, NULL, x_pStr )
#else
#   define TRACE0( s )  _CrtDbgReport( _CRT_WARN, 0, 0, NULL, x_pStr )
#endif
*/
    OutputDebugString( x_pStr );

}

void CDebug::FmtTrace( oexCSTR x_pFmt, ... )
{
	oexVaList ap; oexVaStart( ap, x_pFmt );	
	vFmtTrace( x_pFmt, ap );
	oexVaEnd( ap );
}

void CDebug::vFmtTrace( oexCSTR x_pFmt, oexVaList &x_pArgs )
{
    oexTCHAR tcMsg[ oexSTRSIZE ] = oexT( "" );
    os::CSys::vStrFmt( tcMsg, oexSTRSIZE, x_pFmt, x_pArgs );
    Trace( tcMsg );

//+++   Trace( CStr().vFmt( x_pFmt, x_pArgs ).Ptr() );
}

void CDebug::Break()
{
#ifdef _DEBUG
		
	_CrtDbgBreak();

#else

	DebugBreak();

#endif
}

void CDebug::Break( oexINT x_nType, oexCSTR x_pFile, oexUINT x_uLine, oexCSTR x_pStr )
{
	oexTCHAR tcModule[ oexSTRSIZE ] = oexT( "" );
	GetModuleFileName( (HMODULE)GetInstanceHandle(), tcModule, sizeof( tcModule ) );
	Break( x_nType, x_pFile, x_uLine, tcModule, x_pStr );
}

void CDebug::Break( oexINT x_nType, oexCSTR x_pFile, oexUINT x_uLine, oexCSTR x_pModule, oexCSTR x_pStr )
{
#if defined( _DEBUG )

#if defined( oexUNICODE )

	if ( 1 == _CrtDbgReport( _CRT_ASSERT, oexStrToStr8( x_pFile ).Ptr(), x_uLine, 
                             oexStrToStr8( x_pModule ).Replace( '%', '.' ).Ptr(), 
                             oexStrToStr8( x_pStr ).Ptr() ) )

#else

    if ( 1 == _CrtDbgReport( _CRT_ASSERT, x_pFile, x_uLine, x_pModule, 
                             ( (CStr)x_pStr ).Replace( '%', '.' ).Ptr() ), x_pStr )

#endif

#else	

	CStr str;

	str << oexT( "Module : " ) << x_pModule << oexNL;
	str << oexT( "File : " ) << x_pFile << oexNL;
	str << oexT( "Line : " ) << x_uLine << oexNL << oexNL;
	str << oexT( "Expression : " ) << x_pStr;

	// Simulate the _CrtDbgReport box
	int nRet = MessageBox( NULL, str.Ptr(), oexT( "Program Self Verification Error" ), MB_ICONSTOP | MB_ABORTRETRYIGNORE );

	// Does the user want to quit?
	if ( IDABORT == nRet )
		os::CSys::Exit( 0 );

	// Does the user want to debug?
	else if ( IDRETRY == nRet )

#endif
		Break();
}
