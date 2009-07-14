/*------------------------------------------------------------------
// oex_debugging.h
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

#pragma once

// No reason to switch this off, there is no runtime hit
// http://www.codeguru.com/forum/showthread.php?t=404495
// Thanks Jeff ;)
template < const int T > class oex_static_assert{};
#define oexSTATIC_ASSERT( s )										\
	typedef oex_static_assert< sizeof( int[ ( s ) ? 1 : -1 ] ) >	\
		oex_typedef_static_assert;

#define oexSHOW( v )					oexPrintf( oexMks( oexT( #v ), oexT( " = " ), ( v ), oexNL ).Ptr() )
#define oexSHOWL( v )					oexPrintf( oexMks( oexT( #v ), oexT( " = " ), ( v ), oexNL ).Ptr() ), oexNOTICE( 0, oexMks( #v oexT( " = " ), ( v ) ) )
#define oexSHOWVAL( v, t )				oexPrintf( oexT( #v " = " t ), ( v ) )
#define oexM()							oexPrintf( oexT( "%s:(%d) : **** Marker ****" oexNL8 ), oexFILE, oexLINE )
#define oexLM()							oexM(), oexNOTICE( 0, oexT( "**** Marker ****" ) )
#define oexS( s )						OEX_NAMESPACE::os::CSys::Printf( oexT( "%s:(%d) (%u): " #s oexNL8 ), oexFILE, oexLINE, (oex::oexUINT)oexGetCurrentThreadId() )

#ifdef oexDEBUG

#	define oexRTRACE			        OEX_NAMESPACE::os::CDebug::FmtTrace
#	define oexTRACE( s )		        oexEcho( s )
#	define oexUNUSED( s )
#	define oexBREAK( s )		        ( OEX_NAMESPACE::os::CDebug::Break( 0, oexTEXT( oexFILE ), oexLINE, oexFUNCTION, s ), OEX_NAMESPACE::oexFALSE )
#	define oexDEBUGVAL( v, t )			oexSHOWVAR( v, t )

#else

#	define oexRTRACE			        OEX_NAMESPACE::os::CDebug::FmtTrace
#	define oexTRACE( s )
#	define oexUNUSED( s )
#	define oexBREAK( s )
#	define oexDEBUGVAL( v, t )

#endif

// \note __FILE__ seems to always be type char*

// !!! e has to be last in the parameter list to Log(), that way, it is
//     evaulated first.  This ensures that in case e is a global
//     variable like 'errno', it is not destroyed by the other
//     arguments.
#define oexLOG( l, e, s )			OEX_NAMESPACE::CLog::GlobalLog().Log( oexTEXT( oexFILE ), oexLINE, oexFUNCTION, l, s, e )
#define oexNOTICE( e, s )			oexLOG( OEX_NAMESPACE::CLog::eNotice, e, s )
#define oexWARNING( e, s )			oexLOG( OEX_NAMESPACE::CLog::eWarning, e, s )
#define oexERROR( e, s )			oexLOG( OEX_NAMESPACE::CLog::eError, e, s )
#define oexHALT( e, s )				oexLOG( OEX_NAMESPACE::CLog::eHalt, e, s )

/**
    Macro behavior

    - Macro -               - Release -             - Debug -

    oexASSERT               <removed>               1 = 1
                            <removed>               0 = Shows debug string

    oexCHECK                1 = 1                   1 = 1
                            0 = 0                   0 = 0

    oexVERIFY               1 = 1                   1 = 1
                            0 = Shows debug string  0 = Shows debug string


    These macros have no side effects.

*/

// Verify macros show a dialog box even in release mode
#	define oexVERIFY_PTR_NULL_OK( ptr )     oexVERIFY( oexVerifyPtrNullOk( (OEX_NAMESPACE::oexCPVOID)ptr ) )
#	define oexVERIFY_PTR( ptr )             oexVERIFY( OEX_NAMESPACE::oexVerifyPtr( (OEX_NAMESPACE::oexCPVOID)ptr ) )
#	define oexVERIFY( s )		            ( ( s ) ? OEX_NAMESPACE::oexTRUE : ( OEX_NAMESPACE::os::CDebug::Break( 0, oexTEXT( oexFILE ), oexLINE, oexFUNCTION, oexT( #s ) ), OEX_NAMESPACE::oexFALSE ) )
#	define oexVERIFYPOS( s )	            ({ OEX_NAMESPACE::oexRESULT r = s; ( 0 <= r ) ? OEX_NAMESPACE::oexTRUE : ( OEX_NAMESPACE::os::CDebug::Break( 0, oexTEXT( oexFILE ), oexLINE, oexFUNCTION, oexT( #s ), r ), OEX_NAMESPACE::oexFALSE ); })
#	define oexVERIFYZERO( s )	            ({ OEX_NAMESPACE::oexRESULT r = s; ( !r ) ? OEX_NAMESPACE::oexTRUE : ( OEX_NAMESPACE::os::CDebug::Break( 0, oexTEXT( oexFILE ), oexLINE, oexFUNCTION, oexT( #s ), r ), OEX_NAMESPACE::oexFALSE ); })

#	define oexCHECK_PTR_NULL_OK( ptr )      oexCHECK( oexVerifyPtrNullOk( (OEX_NAMESPACE::oexCPVOID)ptr ) )
#	define oexCHECK_PTR( ptr )              oexCHECK( OEX_NAMESPACE::oexVerifyPtr( (OEX_NAMESPACE::oexCPVOID)ptr ) )
#	define oexCHECK( s )		            ( ( s ) ? OEX_NAMESPACE::oexTRUE :  OEX_NAMESPACE::oexFALSE )

#ifdef oexDEBUG

#	define oexASSERT_PTR_NULL_OK( ptr )     oexIGNORE oexVERIFY( oexVerifyPtrNullOk( (OEX_NAMESPACE::oexCPVOID)ptr ) )
#	define oexASSERT_PTR( ptr )             oexIGNORE oexVERIFY( oexVerifyPtr( (OEX_NAMESPACE::oexCPVOID)ptr ) )
#	define oexASSERT( s )		            oexIGNORE ( ( s ) ? OEX_NAMESPACE::oexTRUE : ( OEX_NAMESPACE::os::CDebug::Break( 0, oexTEXT( oexFILE ), oexLINE, oexFUNCTION, oexT( #s ) ), OEX_NAMESPACE::oexFALSE ) )

// +++ Only works for 32-bit processors
//
// 0xABABABAB	-	LocalAlloc()
// 0xBAADF00D	-	LocalAlloc( LMEM_FIXED )
// 0xCCCCCCCC	-	Uninitialized stack memory
// 0xCDCDCDCD	-	Uninitialized heap memory ( allocated with new )
// 0xDDDDDDDD	-	Released heap memory ( after delete )
// 0xFDFDFDFD	-	Debugging gaurd bytes for heap memory ( buffer overrun? )
// 0xFEEEFEEE	-	HeapFree()
// 0xDEADDEAD	-	Windows Stop error
// 0xDEADC0DE	-	Linux / Unix
// 0xDEADBEEF	-	Linux / Unix
// 0xDEADBABE	-	Linux / Unix
// 0xA5A5A5A5	-	Embedded
// 0xFFFFFFFF	-	Invalid
static inline oexBOOL oexVerifyPtrNullOk( oexCPVOID ptr )
{	if ( !ptr ) return oexTRUE;
    return (       ( (oexTYPEOF_PTR)ptr >  oexPtrToPtr( 0x00000032 ) )
                && ( (oexTYPEOF_PTR)ptr != oexPtrToPtr( 0xABABABAB ) )
				&& ( (oexTYPEOF_PTR)ptr != oexPtrToPtr( 0xBAADF00D ) )
				&& ( (oexTYPEOF_PTR)ptr != oexPtrToPtr( 0xCCCCCCCC ) )
				&& ( (oexTYPEOF_PTR)ptr != oexPtrToPtr( 0xCDCDCDCD ) )
				&& ( (oexTYPEOF_PTR)ptr != oexPtrToPtr( 0xDDDDDDDD ) )
				&& ( (oexTYPEOF_PTR)ptr != oexPtrToPtr( 0xFDFDFDFD ) )
				&& ( (oexTYPEOF_PTR)ptr != oexPtrToPtr( 0xFEEEFEEE ) )
				&& ( (oexTYPEOF_PTR)ptr != oexPtrToPtr( 0xDEADDEAD ) )
				&& ( (oexTYPEOF_PTR)ptr != oexPtrToPtr( 0xDEADC0DE ) )
				&& ( (oexTYPEOF_PTR)ptr != oexPtrToPtr( 0xDEADBEEF ) )
				&& ( (oexTYPEOF_PTR)ptr != oexPtrToPtr( 0xDEADBABE ) )
				&& ( (oexTYPEOF_PTR)ptr != oexPtrToPtr( 0xA5A5A5A5 ) )
				&& ( (oexTYPEOF_PTR)ptr != oexPtrToPtr( 0xFFFFFFFF ) ) );
}

static inline oexBOOL oexVerifyPtr( oexCPVOID ptr )
{	return	( (oexTYPEOF_PTR)oexNULL != (oexTYPEOF_PTR)ptr ) &&
			oexVerifyPtrNullOk( ptr );
}

#else

#	define oexASSERT_PTR( ptr )
#	define oexASSERT_PTR_NULL_OK( ptr )
#	define oexASSERT( s )

static inline oexBOOL oexVerifyPtrNullOk( oexCPVOID ptr )
{	return oexTRUE; }

static inline oexBOOL oexVerifyPtr( oexCPVOID ptr )
{	return	( (oexTYPEOF_PTR)oexNULL != (oexTYPEOF_PTR)ptr ); }

#endif

