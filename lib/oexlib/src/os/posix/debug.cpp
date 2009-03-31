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
//   HOWEVER CAUSED AND ON ANY #include "opc/breloc/basic.c"THEORY OF LIABILITY, WHETHER IN
//   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
//   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
//   EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//----------------------------------------------------------------*/

#include "../../../oexlib.h"
#include "std_os.h"

OEX_USING_NAMESPACE
using namespace OEX_NAMESPACE::os;

// http://www.codeguru.com/Cpp/W-P/dll/tips/article.php/c3635/
// http://www.dotnet247.com/247reference/msgs/13/65259.aspx

oexCPVOID CDebug::GetInstanceHandle()
{
	return (oexCPVOID)getpid();
}

void CDebug::Trace( oexCSTR x_pStr )
{
	puts( oexStrToMbPtr( x_pStr ) );
}

void CDebug::FmtTrace( oexCSTR x_pFmt, ... )
{
	oexVaList ap; oexVaStart( ap, x_pFmt );
	vFmtTrace( x_pFmt, ap );
	oexVaEnd( ap );
}

void CDebug::vFmtTrace( oexCSTR x_pFmt, oexVaList &x_pArgs )
{
//  This is just for when the string class is malfunctioning ;-)
//    oexTCHAR tcMsg[ oexSTRSIZE ] = oexT( "" );
//    os::CSys::vStrFmt( tcMsg, oexSTRSIZE, x_pFmt, x_pArgs );
//    Trace( tcMsg );

	Trace( CStr().vFmt( x_pFmt, x_pArgs ).Ptr() );
}

void CDebug::Break()
{
	// Flush the log file contents
	CLog::GlobalLog().Flush();

#ifndef OEX_ARM
//	asm ( "int $3" );
#endif
}

void CDebug::Break( oexINT x_nType, oexCSTR x_pFile, oexUINT x_uLine, oexCSTR8 x_pFunction, oexCSTR x_pStr, oexINT x_nRes, oexINT x_nErr )
{
	CStr sPath = oexGetModulePath();
	Break( x_nType, x_pFile, x_uLine, x_pFunction, sPath.Ptr(), x_pStr, x_nRes, x_nErr );
}

void CDebug::Break( oexINT x_nType, oexCSTR x_pFile, oexUINT x_uLine, oexCSTR8 x_pFunction, oexCSTR x_pModule, oexCSTR x_pStr, oexINT x_nRes, oexINT x_nErr )
{
	CStr str;

	str << oexNL;
	str << oexT( "==========================" ) << oexNL;
	str << oexT( "  CDebug::Break() Called  " ) << oexNL;
	str << oexT( "==========================" ) << oexNL;
	str << oexT( "Module     : " ) << ( oexCHECK_PTR( x_pModule ) ? x_pModule : oexT( "" ) ) << oexNL;
	str << oexT( "File       : " ) << ( oexCHECK_PTR( x_pFile ) ? x_pFile : oexT( "" ) ) << oexNL;
	str << oexT( "Line       : " ) << x_uLine << oexNL;
	str << oexT( "Function   : " ) << ( oexCHECK_PTR( x_pFunction ) ? oexMbToStrPtr( x_pFunction ) : oexT( "???" ) ) << oexT( "()" ) << oexNL;
	str << oexT( "Expression : " ) << CStr( x_pStr ).Replace( oexT( "%" ), oexT( "%%" ) ).Ptr() << oexNL;
	if ( x_nRes )
		str << oexT( "Result     : " ) << x_nRes << oexT( " sys:\"" ) << os::CTrace::GetErrorMsg( x_nRes ).Replace( oexT( "%" ), oexT( "%%" ) ).Ptr() << oexT( "\"" ) << oexNL;
	if ( x_nErr )
		str << oexT( "Error Code : " ) << x_nErr << oexT( " sys:\"" ) << os::CTrace::GetErrorMsg( x_nErr ).Replace( oexT( "%" ), oexT( "%%" ) ).Ptr() << oexT( "\"" ) << oexNL;

	puts( oexStrToMbPtr( str.Ptr() ) );
	oexERROR( 0, str.Ptr() );

#ifdef oexBACKTRACE_IN_LOG
	oexPrintf( oexStrToMbPtr( os::CTrace::GetBacktrace( 0 ).Ptr() ) );
#endif

	Break();
}

