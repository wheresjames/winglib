/*------------------------------------------------------------------
// sq_engine.cpp
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

#include "stdafx.h"

/*
	got the house, but not the deed,
	got the horn, but not the reed.

	got the cards, but not the luck,
	got the wheel, but not the truck.

*/

using namespace sqbind;

int CSqEngineExport::alert( const stdString &sMsg )
{_STT();
	return oex::os::CSys::ShowMessageBox( oexT( "Script Message" ), sMsg.c_str() );
}

int CSqEngineExport::print( const stdString &sMsg )
{_STT();
	return oexPrintf( sMsg.c_str() );
}

int CSqEngineExport::echo( const stdString &sMsg )
{_STT();
	return oexEcho( sMsg.c_str() );
}

int CSqEngineExport::flush()
{_STT();
	return oex::os::CSys::Flush_stdout();
}

int CSqEngineExport::import( const stdString &sClass )
{_STT();
	return OnImport( sClass );
}

int CSqEngineExport::include( const stdString &sScript )
{_STT();
	return OnInclude( sScript );
}

int CSqEngineExport::include_once( const stdString &sScript )
{_STT();
	return OnIncludeOnce( sScript );
}

int CSqEngineExport::load_module( const stdString &sModule, const stdString &sPath )
{_STT();
	return OnLoadModule( sModule, sPath );
}

int CSqEngineExport::kill( const stdString &sPath )
{_STT();
	CSqMsgQueue *q = queue();
	if ( !q ) return -1;
	return q->kill( oexNULL, sPath );
}

int CSqEngineExport::kill_wait( const stdString &sPath )
{_STT();
	CSqMsgQueue *q = queue();
	if ( !q ) return -1;
	stdString sRet;
	return q->kill( &sRet, sPath );
}

void CSqEngineExport::exit( int nExitCode )
{_STT();
	oex::os::CSys::Quit( nExitCode );
}

void CSqEngineExport::terminate( int nExitCode )
{_STT();
	oex::os::CSys::Exit( nExitCode );
}

stdString CSqEngineExport::get_build_version()
{_STT();
	return oexVersion().Ptr();
}


stdString CSqEngineExport::get_name()
{_STT();
	return m_sScriptName;
}

stdString CSqEngineExport::path( const stdString &sPath )
{_STT();
	return OnPath( sPath );
}

stdString CSqEngineExport::decorate( const stdString &sPath, int bExe, int bLib )
{_STT();
	return oex::CStr( sPath.c_str() ).DecorateName( bExe, bLib ).Ptr();
}

stdString CSqEngineExport::build_path( const stdString &sS1,  const stdString &sS2 )
{_STT();
	return oexBuildPath( sS1.c_str(), sS2.c_str() ).Ptr();
}

stdString CSqEngineExport::root( const stdString &sPath )
{_STT();
	return oexGetModulePath( sPath.c_str() ).Ptr();
}

stdString CSqEngineExport::module_name()
{_STT();
	return oexGetModuleFileName().Ptr();
}

stdString CSqEngineExport::module_path()
{_STT();
	return oexGetModulePath().Ptr();
}

stdString CSqEngineExport::get_resource( const stdString &sRes, int bFileOverrideOk )
{_STT();
	// Data container
	oex::CStr s;

	if ( bFileOverrideOk )
	{
		// Look in script path
		stdString sSub = path( sRes );
		if ( oexExists( sSub.c_str() ) )
			s = oexMbToStr( oexFileGetContents( sSub.c_str() ) );

		// Look in binary path
		else
		{	stdString sSub = root( sRes );
			if ( oexExists( sSub.c_str() ) )
				s = oexMbToStr( oexFileGetContents( sSub.c_str() ) );
		} // end else

	} // end if

#if defined( OEX_RESOURCES )

	// Check for resource
	if ( !s.Length() )
		s = oexGetResource( sRes.c_str() );

#endif

	// Punt if no data
	if ( !s.Length() )
		return stdString();

	return stdString( s.Ptr(), s.Length() );
}

/// Returns the buffer for a binary share
CSqBinary CSqEngineExport::get_binshare( const stdString &sName )
{_STT();
	return oexGetBin( sName.c_str() );
}

/// Returns the buffer for a binary share
int CSqEngineExport::set_binshare( const stdString &sName, CSqBinary *pBin )
{_STT();
	if ( !pBin ) return 0;
	return oexSetBin( sName.c_str(), &pBin->Mem() );
}

/// Returns the buffer for a binary share
int CSqEngineExport::is_binshare( const stdString &sName )
{_STT();
	return oexIsBin( sName.c_str() );
}

/// Runs garbage collection on binary shares
int CSqEngineExport::cleanup_binshare()
{_STT();
	return oexCleanupBin();
}

int CSqEngineExport::enable_output_capture( int buffers, int buf_size )
{_STT();
	return oexEnableOutputCapture( buffers, buf_size );
}

stdString CSqEngineExport::get_output( int max )
{_STT();

	oex::CFifoSync *p = oex::CUtil::getOutputBuffer();
	if ( !p )
		return oexT( "" );

	oex::CStr8 s;
	oex::CFifoSync::t_size b = 0;
	while ( p->Peek( b++, s, 0, oex::oexTRUE ) )
		s << oexNL8;
#if defined( oexUNICODE )
	oex::CStr w = oexMbToStr( s );
	return stdString( w.Ptr(), w.Length() );
#else
	return stdString( s.Ptr(), s.Length() );
#endif

/*
	oex::CCircBuf *p = oex::CUtil::getOutputBuffer();
	if ( !p )
		return oexT( "" );
	oex::CStr s = oexMbToStr( p->Peek( max ) );
	return stdString( s.Ptr(), s.Length() );
*/
}

stdString CSqEngineExport::base64_encode( const stdString &sStr )
{_STT();
	oex::CStr8 sMb = oexStrToMb( oex::CStr( sStr.c_str(), sStr.length() ) );
	oex::CStr sRes = oexMbToStr( oex::CBase64::Encode( sMb ) );
	return stdString().assign( sRes.Ptr(), sRes.Length() );
}

stdString CSqEngineExport::base64_decode( const stdString &sStr )
{_STT();
	oex::CStr8 sMb = oexStrToMb( oex::CStr( sStr.c_str(), sStr.length() ) );
	oex::CStr sRes = oexMbToStr( oex::CBase64::Decode( sMb ) );
	return stdString().assign( sRes.Ptr(), sRes.Length() );
}

stdString CSqEngineExport::base16_encode( const stdString &sStr )
{_STT();
	oex::CStr8 sMb = oexStrToMb( oex::CStr( sStr.c_str(), sStr.length() ) );
	oex::CStr sRes = oexMbToStr( oex::CBase16::Encode( sMb ) );
	return stdString().assign( sRes.Ptr(), sRes.Length() );
}

stdString CSqEngineExport::base16_decode( const stdString &sStr )
{_STT();
	oex::CStr8 sMb = oexStrToMb( oex::CStr( sStr.c_str(), sStr.length() ) );
	oex::CStr sRes = oexMbToStr( oex::CBase16::Decode( sMb ) );
	return stdString().assign( sRes.Ptr(), sRes.Length() );
}

stdString CSqEngineExport::md5( const stdString &sStr )
{_STT();
	oex::oexGUID hash;
	oex::CStr8 sMb = oexStrToMb( oex::CStr( sStr.c_str(), sStr.length() ) );
	oex::CStr sRes = oexMbToStr( oex::CBase16::Encode( oex::oss::CMd5::Transform( &hash, sMb.Ptr(), sMb.Length() ), sizeof( hash ) ) );
	return stdString().assign( sRes.Ptr(), sRes.Length() );
}

// mandelbrot( 64, 16, 0, -15, 40, 15 );
stdString CSqEngineExport::mandelbrot( int w, int h, int x1, int y1, int x2, int y2 )
{_STT();
	oex::CStr s = oexMandelbrot( w, h, x1, y1, x2, y2 );
	return sqbind::stdString( s.Ptr(), s.Length() );
}


stdString CSqEngineExport::unique()
{_STT();
	oex::oexGUID hash;
	oex::CStr sRes = oexMbToStr( oex::CBase16::Encode( oexUniqueGuid( &hash ), sizeof( hash ) ) );
	return stdString().assign( sRes.Ptr(), sRes.Length() );
}

int CSqEngineExport::local_time()
{_STT();
	return oexLocalTime().GetUnixTime();
}

int CSqEngineExport::gmt_time()
{_STT();
	return oexGmtTime().GetUnixTime();
}

stdString CSqEngineExport::local_timestr( const stdString &fmt )
{_STT();
	return oexLocalTimeStr( fmt.c_str() ).Ptr();
}

stdString CSqEngineExport::gmt_timestr( const stdString &fmt )
{_STT();
	return oexGmtTimeStr( fmt.c_str() ).Ptr();
}

int CSqEngineExport::set_timer( const stdString &sPath, int to, const stdString &sCallback )
{
	CSqMsgQueue *q = queue();
	if ( !q ) return 0;
	return oexStrToLong( q->set_timer( sPath, oexMks( to ).Ptr(), sCallback ).c_str() );
}

int CSqEngineExport::set_timeout( const stdString &sPath, int to, const stdString &sCallback )
{
	CSqMsgQueue *q = queue();
	if ( !q ) return 0;
	return oexStrToLong( q->set_timeout( sPath, oexMks( to ).Ptr(), sCallback ).c_str() );
}

int CSqEngineExport::kill_timer( const stdString &sPath, int id )
{
	CSqMsgQueue *q = queue();
	if ( !q ) return 0;
	return oexStrToLong( q->kill_timer( sPath, oexMks( id ).Ptr() ).c_str() );
}


stdString CSqEngineExport::ltrim( const stdString &sS, const stdString &sChs )
{_STT();
	oex::CStr s( sS.c_str(), sS.length() );
	s.LTrim( sChs.c_str() );
	return stdString( s.Ptr(), s.Length() );
}

stdString CSqEngineExport::rtrim( const stdString &sS, const stdString &sChs )
{_STT();
	oex::CStr s( sS.c_str(), sS.length() );
	s.RTrim( sChs.c_str() );
	return stdString( s.Ptr(), s.Length() );

}

stdString CSqEngineExport::trim( const stdString &sS, const stdString &sChs )
{_STT();
	oex::CStr s( sS.c_str(), sS.length() );
	s.Trim( sChs.c_str() );
	return stdString( s.Ptr(), s.Length() );
}

stdString CSqEngineExport::trimws( const stdString &sS )
{_STT();
	oex::CStr s( sS.c_str(), sS.length() );
	s.TrimWhiteSpace();
	return stdString( s.Ptr(), s.Length() );
}

int CSqEngineExport::find( const stdString &sS, const stdString &sSub )
{_STT();
	oex::CStr s( sS.c_str(), sS.length() );
	return s.Match( sSub.c_str() );
}

stdString CSqEngineExport::replace( const stdString &sS, const stdString &sFind, const stdString &sReplace )
{_STT();
	oex::CStr s = oex::CStr( sS.c_str(), sS.length() )
					.Replace( sFind.c_str(), sReplace.c_str() );
	return stdString( s.Ptr(), s.Length() );
}

stdString CSqEngineExport::drop( const stdString &sS, const stdString &sDrop, int bInclusive )
{_STT();
	oex::CStr s = oex::CStr( sS.c_str(), sS.length() )
					.Drop( sDrop.c_str(), bInclusive );
	return stdString( s.Ptr(), s.Length() );
}

stdString CSqEngineExport::drop_range( const stdString &sS, const stdString &sBegin, const stdString &sEnd, int bInclusive )
{_STT();
	oex::CStr s = oex::CStr( sS.c_str(), sS.length() )
					.DropRange( sBegin.c_str()[ 0 ], sEnd.c_str()[ 0 ], bInclusive );
	return stdString( s.Ptr(), s.Length() );
}

stdString CSqEngineExport::urlencode( const stdString &sS )
{_STT();
	oex::CStr s = oexUrlEncode( oex::CStr( sS.c_str(), sS.length() ) );
	return stdString( s.Ptr(), s.Length() );
}

stdString CSqEngineExport::urldecode( const stdString &sS )
{_STT();
	oex::CStr s = oexUrlDecode( oex::CStr( sS.c_str(), sS.length() ) );
	return stdString( s.Ptr(), s.Length() );
}

stdString CSqEngineExport::htmlencode( const stdString &sS )
{_STT();
	oex::CStr s = oexHtmlEncode( oex::CStr( sS.c_str(), sS.length() ) );
	return stdString( s.Ptr(), s.Length() );
}

stdString CSqEngineExport::htmldecode( const stdString &sS )
{_STT();
	oex::CStr s = oexHtmlDecode( oex::CStr( sS.c_str(), sS.length() ) );
	return stdString( s.Ptr(), s.Length() );
}

stdString CSqEngineExport::compress( const stdString &sS )
{_STT();
	oex::CStr c = oexCompressStr( oex::CStr( sS.c_str(), sS.length() ) );
	return stdString( c.Ptr(), c.Length() );
}

stdString CSqEngineExport::uncompress( const stdString &sS )
{_STT();
	oex::CStr c = oexUncompressStr( oex::CStr( sS.c_str(), sS.length() ) );
	return stdString( c.Ptr(), c.Length() );
}

int CSqEngineExport::spawn( int nRet, const stdString &sPath, const stdString &sName, const stdString &sScript, int bFile )
{_STT();
	CSqMsgQueue *q = queue();
	if ( !q ) return -1;

	if ( !nRet )
		return q->spawn( oexNULL, sPath, sName, sScript, bFile );

	stdString sRet;
	return q->spawn( &sRet, sPath, sName, sScript, bFile );
}

double CSqEngineExport::get_cpu_load()
{_STT();
	return oexGetCpuLoad();
}

double CSqEngineExport::boot_time()
{_STT();
	oexSHOW( oex::os::CHqTimer::GetTimerSeconds() );
	return oex::os::CHqTimer::GetTimerSeconds();
}

unsigned int CSqEngineExport::get_timer_seconds()
{_STT();
	oex::oexINT64 nVal = 0;
	if ( !oex::os::CHqTimer::osGetCounts( &nVal, oexNULL ) )
		return 0;
	return (unsigned int)nVal;
}

unsigned int CSqEngineExport::get_timer_useconds()
{_STT();
	oex::oexINT64 nVal = 0;
	if ( !oex::os::CHqTimer::osGetCounts( oexNULL, &nVal ) )
		return 0;
	return (unsigned int)nVal;
}

int CSqEngineExport::run( const stdString &sPath, const stdString &sScript )
{_STT();
	CSqMsgQueue *q = queue();
	if ( !q ) return -1;
	return q->run( oexNULL, sPath, sScript );
}

void CSqEngineExport::set( const stdString &sPath, const stdString &sKey, const stdString &sVal )
{_STT();
	CSqMsgQueue *q = queue();
	if ( q )
		q->set( sPath, sKey, sVal );
}

stdString CSqEngineExport::get( const stdString &sPath, const stdString &sKey )
{_STT();
	CSqMsgQueue *q = queue();
	if ( !q ) return stdString();
	return q->get( sPath, sKey );
}

int CSqEngineExport::isset( const stdString &sPath, const stdString &sKey )
{_STT();
	CSqMsgQueue *q = queue();
	if ( !q ) return 0;
	return q->isset( sPath, sKey ).length();
}

CSqMulti CSqEngineExport::pb( const stdString &sPath )
{_STT();
	CSqMsgQueue *q = queue();
	if ( !q )
		return CSqMulti();
	return CSqMulti( q->pb( sPath ) );
}

int CSqEngineExport::shell( const stdString &sFile, const stdString &sParams, const stdString &sDirectory )
{_STT();
	return oexShell( sFile.c_str(), sParams.c_str(), sDirectory.c_str() );
}


int CSqEngineExport::is_path( const stdString &sPath )
{_STT();
	CSqMsgQueue *q = queue();
	if ( !q ) return 0;
	return q->is_path( sPath );
}

void CSqEngineExport::error( int e, const stdString &sStr )
{_STT();
	oexERROR( 0, sStr.c_str() );
}

void CSqEngineExport::warning( int e, const stdString &sStr )
{_STT();
	oexWARNING( 0, sStr.c_str() );
}

void CSqEngineExport::notice( int e, const stdString &sStr )
{_STT();
	oexNOTICE( 0, sStr.c_str() );
}


stdString CSqEngineExport::get_children( const stdString &sPath )
{_STT();
	CSqMsgQueue *q = queue();
	if ( !q ) return oexT( "" );
	stdString sRet;
	if ( !q->get_children( &sRet, sPath ) )
		return oexT( "" );
	return sRet;
}

stdString CSqEngineExport::execute( int nRet, const stdString &sPath, const stdString &sFunction )
{_STT();
	CSqMsgQueue *q = queue();
	if ( !q )
		return oexT( "" );

	if ( !nRet )
	{	q->execute( oexNULL, sPath, sFunction );
		return oexT( "" );
	} // end if

	stdString sRet;
	q->execute( &sRet, sPath, sFunction );
	return sRet;
}

stdString CSqEngineExport::execute1( int nRet, const stdString &sPath, const stdString &sFunction, const stdString &sP1 )
{_STT();
	CSqMsgQueue *q = queue();
	if ( !q )
		return oexT( "" );

	if ( !nRet )
	{	q->execute( oexNULL, sPath, sFunction, sP1 );
		return oexT( "" );
	} // end if

	stdString sRet;
	q->execute( &sRet, sPath, sFunction, sP1 );
	return sRet;
}

stdString CSqEngineExport::execute2( int nRet, const stdString &sPath, const stdString &sFunction, const stdString &sP1, const stdString &sP2 )
{_STT();
	CSqMsgQueue *q = queue();
	if ( !q )
		return oexT( "" );

	if ( !nRet )
	{	q->execute( oexNULL, sPath, sFunction, sP1, sP2 );
		return oexT( "" );
	} // end if

	stdString sRet;
	q->execute( &sRet, sPath, sFunction, sP1, sP2 );
	return sRet;
}

stdString CSqEngineExport::execute3( int nRet, const stdString &sPath, const stdString &sFunction, const stdString &sP1, const stdString &sP2, const stdString &sP3 )
{_STT();
	CSqMsgQueue *q = queue();
	if ( !q )
		return oexT( "" );

	if ( !nRet )
	{	q->execute( oexNULL, sPath, sFunction, sP1, sP2, sP3 );
		return oexT( "" );
	} // end if

	stdString sRet;
	q->execute( &sRet, sPath, sFunction, sP1, sP2, sP3 );
	return sRet;
}

stdString CSqEngineExport::execute4( int nRet, const stdString &sPath, const stdString &sFunction, const stdString &sP1, const stdString &sP2, const stdString &sP3, const stdString &sP4 )
{_STT();
	CSqMsgQueue *q = queue();
	if ( !q )
		return oexT( "" );

	if ( !nRet )
	{	q->execute( oexNULL, sPath, sFunction, sP1, sP2, sP3, sP4 );
		return oexT( "" );
	} // end if

	stdString sRet;
	q->execute( &sRet, sPath, sFunction, sP1, sP2, sP3, sP4 );
	return sRet;
}

void CSqEngineExport::sleep( int nMsTime )
{_STT();
	oex::os::CSys::Sleep( nMsTime );
}

float CSqEngineExport::clock()
{_STT();
	return oexGetBootSeconds();
}

unsigned int CSqEngineExport::ticks()
{_STT();
	return oexGetBootCount();
}

CSqMsgQueue* CSqEngineExport::queue()
{_STT();
	return OnGetQueue();
}

int CSqEngineExport::OnImport( const stdString &sClass )
{_STT();
	return 0;
}

int CSqEngineExport::OnInclude( const stdString &sClass )
{_STT();
	return 0;
}

int CSqEngineExport::OnIncludeOnce( const stdString &sClass )
{_STT();
	return 0;
}

int CSqEngineExport::OnLoadModule( const stdString &sModule, const stdString &sPath )
{_STT();
	return 0;
}

SquirrelVM* CSqEngineExport::GetVmPtr()
{_STT();
	return 0;
}

CSqEngine* CSqEngineExport::GetEnginePtr()
{_STT();
	return 0;
}

stdString CSqEngineExport::OnPath( stdString sPath )
{_STT();
	return oexBuildPath( m_sRoot.c_str(), sPath.c_str() ).Ptr();
}

CSqMsgQueue* CSqEngineExport::OnGetQueue()
{_STT();
	return oexNULL;
}

BOOL CSqEngine::IsScript()
{_STT();
	return m_bLoaded;
}

stdString& CSqEngine::GetLastError()
{_STT();
	return m_sErr;
}

stdString& CSqEngine::GetOutput()
{_STT();
	return m_sOutput;
}

SquirrelVM& CSqEngine::GetVM()
{_STT();
	return m_vm;
}

CSqEngine::operator SquirrelVM&()
{_STT();
	return m_vm;
}

SquirrelObject& CSqEngine::GetScriptObj()
{_STT();
	return m_script;
}

const SquirrelObject& CSqEngine::GetRootTable()
{_STT();
	return m_vm.GetRootTable();
}

SquirrelVM* CSqEngine::GetVmPtr()
{_STT();
	return &m_vm;
}


void CSqEngine::SqPrint( HSQUIRRELVM v, const SQChar* s, ... )
{_STT();
	StackHandler sa( v );
	SquirrelObject root( v, sa.GetObjectHandle( 1 ) );

	// Retrieve our class pointer
	SquirrelObject ptr = root.GetValue( SQEXE_SELF );
	CSqEngine *pSqEngine = (CSqEngine*)(CSqEngineExport*)ptr.GetInstanceUP( SqPlus::ClassType< CSqEngineExport >::type() );
	if ( !pSqEngine ) return;

	// Grab string from the command line
	oex::CStr sErr;
	oexVaList ap; oexVaStart( ap, s );
	sErr.vFmt( s, ap );
	oexVaEnd( ap );

	// Save the output string
	pSqEngine->m_sOutput += sErr.Ptr();
}

SQInteger CSqEngine::SqErrorHandler( HSQUIRRELVM v )
{_STT();
	if( 0 >= sq_gettop( v ) )
		return 0;

	const SQChar *sErr = 0;
	if( SQ_SUCCEEDED( sq_getstring( v, -1, &sErr ) ) )
		_oexTHROW( SScriptErrorInfo( sErr, oexT( "" ), 0, 0 ) );

	else
		_oexTHROW( SScriptErrorInfo( oexT( "Unknown error" ), oexT( "" ), 0, 0 ) );

	return 0;
}

void CSqEngine::SqCompilerErrorHandler( HSQUIRRELVM v, const SQChar *sErr, const SQChar *sSource, SQInteger line, SQInteger column )
{_STT();
	throw SScriptErrorInfo( sErr, sSource, line, column );
}

SQInteger CSqEngine::SqAuxErrorHandler( HSQUIRRELVM v )
{_STT();
	if( 0 >= sq_gettop( v ) )
		return 0;

	stdString sErr;
	const SQChar *pErr = 0;
	if( SQ_SUCCEEDED( sq_getstring( v, -1, &pErr ) ) )
		sErr = pErr;
	else
		sErr = oexT( "Unknown Error" );

	// Get stack trace
	SQStackInfos si;
	if ( SQ_SUCCEEDED( sq_stackinfos( v, 1, &si ) ) )
	{   stdString sMsg;
		if ( si.funcname )
		{   sMsg += si.funcname;
			sMsg += oexT( "() : " );
		} // end if
		sMsg += sErr;
		_oexTHROW( SScriptErrorInfo( sMsg.c_str(), si.source ? si.source : oexT( "" ), si.line, 0 ) );
	} // end if

	else
		_oexTHROW( SScriptErrorInfo( sErr.c_str(), oexT( "" ), 0, 0 ) );

	return 0;
}


CSqEngine::CSqEngine() :
	m_vm( SquirrelVM::StdLib_All ),
	m_script( m_vm.GetVMHandle() )
{_STT();
	m_bLoaded = oex::oexFALSE;
	m_fExportSymbols = oexNULL;
	m_pSqAllocator = oexNULL;
	m_fIncludeScript = oexNULL;
	m_bCallExit = oex::oexFALSE;
}

CSqEngine::~CSqEngine()
{_STT();
	Destroy();
}

void CSqEngine::Exit()
{_STT();
	if ( !m_bCallExit )
		return;

	m_bCallExit = oex::oexFALSE;

	int nRet = 0;
	Execute( &nRet, SQEXE_FN_EXIT );
}

void CSqEngine::Destroy()
{_STT();
	Exit();

	m_script.Reset();

	m_vm.Shutdown();

	m_mapIncludeScriptCache.clear();

	m_sErr = oexT( "" );
	m_sOutput = oexT( "" );
	m_sRoot = oexT( "" );

	m_bLoaded = oex::oexFALSE;

}

// Message box and logging functions / Mostly for debugging ;)
static int _msg( const stdString &sTitle, const stdString &sMsg )
{_STT();
	return oex::os::CSys::ShowMessageBox( sTitle.c_str(), sMsg.c_str() );
}
static int _log( int nErr, const stdString &sMsg )
{_STT();
	return oexNOTICE( nErr, sMsg.c_str() );
}
static int _break( const stdString &sError )
{_STT();
	oexBREAK( sError.c_str() ); return 0;
}

/*
class CTestClass
{
public:

	_SQBIND_CLASS_CTOR_BEGIN( CTestClass )
	_SQBIND_CLASS_CTOR_END( CTestClass )

	int msg( const stdString &sTitle, const stdString &sMsg )
	{	return oex::os::CSys::ShowMessageBox( sTitle.c_str(), sMsg.c_str() ); }

	int num( int num )
	{	return msg( oexT( "Number" ), oexMks( num ).Ptr() ); }
};

SQBIND_REGISTER_CLASS_BEGIN( CTestClass, CTestClass )
	SQBIND_MEMBER_FUNCTION(  CTestClass, msg )
	SQBIND_MEMBER_FUNCTION(  CTestClass, num )
SQBIND_REGISTER_CLASS_END()
*/

SQBIND_REGISTER_CLASS_BEGIN( CSqMsgQueue, CSqMsgQueue )
SQBIND_REGISTER_CLASS_END()

SQBIND_REGISTER_CLASS_BEGIN( CSqEngineExport, CSqEngineExport )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, alert )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, print )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, echo )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, get_build_version )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, flush )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, import )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, include )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, include_once )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, load_module )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, get_children )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, sleep )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, clock )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, ticks )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, spawn )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, get )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, set )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, isset )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, pb )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, run )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, shell )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, get_cpu_load )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, boot_time )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, error )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, warning )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, notice )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, is_path )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, execute )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, execute1 )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, execute2 )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, execute3 )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, execute4 )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, exit )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, terminate )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, kill )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, kill_wait )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, queue )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, path )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, module_name )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, module_path )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, build_path )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, decorate )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, root )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, md5 )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, base64_decode )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, base64_encode )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, base16_decode )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, base16_encode )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, unique )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, mandelbrot )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, local_time )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, gmt_time )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, get_timer_seconds )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, get_timer_useconds )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, local_timestr )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, gmt_timestr )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, set_timer )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, set_timeout )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, kill_timer )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, tolong )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, tofloat )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, todouble )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, trim )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, ltrim )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, rtrim )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, trimws )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, find )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, replace )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, drop )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, drop_range )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, urlencode )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, urldecode )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, htmlencode )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, htmldecode )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, compress )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, uncompress )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, get_resource )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, get_name )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, get_binshare )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, set_binshare )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, is_binshare )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, cleanup_binshare )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, enable_output_capture )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, get_output )

SQBIND_REGISTER_CLASS_END()

void CSqEngine::SetExportFunction( PFN_SQBIND_Export_Symbols fn, sqbind::SSqAllocator *pa )
{_STT();
	m_fExportSymbols = fn;
	m_pSqAllocator = pa;
}

oex::oexBOOL CSqEngine::Init()
{_STT();

	_oexTRY
	{
		// Initialize the virtual machine
		m_vm.Init();

		// Set compiler error handler
		sq_setcompilererrorhandler( m_vm.GetVMHandle(), &CSqEngine::SqCompilerErrorHandler );

		// Set print function
		sq_setprintfunc( m_vm.GetVMHandle(), CSqEngine::SqPrint );

		// Set run time error handler
//        sq_newclosure( m_vm.GetVMHandle(), &CSqEngine::SqErrorHandler, 0 );

		// Set auxiliary error handler
		sq_newclosure( m_vm.GetVMHandle(), &CSqEngine::SqAuxErrorHandler, 0 );

		sq_seterrorhandler( m_vm.GetVMHandle() );

		// Debugging functions
		BindRootFunction( _msg, oexT( "_msg" ) );
		BindRootFunction( _log, oexT( "_log" ) );
		BindRootFunction( _break, oexT( "_break" ) );

		// Bind Squirrel variables
#if defined( SQBIND_SQBIND )
		sqbind::SqBindAll( m_vm.GetVMHandle() );
		SQBIND_EXPORT( m_vm.GetVMHandle(), CSqMsgQueue );
		SQBIND_EXPORT( m_vm.GetVMHandle(), CSqEngineExport );
#else
		sqbind::SqBindAll( &m_vm );
		SQBIND_EXPORT( &m_vm, CSqMsgQueue );
		SQBIND_EXPORT( &m_vm, CSqEngineExport );
#endif
//		SQBIND_EXPORT( &m_vm, CTestClass );
/*

		sq_pushroottable(v);
		sq_pushstring(v,p_constant,-1);
		sq_pushinteger(v,p_value);
		sq_newslot(v,-3,false);
		sq_pop(v,1); // pop roottable


		sqbind::VM v = m_vm.GetVMHandle();
		sq_pushobject( v, SqBind< CSqEngineExport >::get_id() ); // push class
		sq_pushstring( v, SQEXE_SELF, -1 );
		sqbind_push_method_userdata( v, (CSqEngineExport*)this );
		sq_newclosure(v,_sqbind_sqmethod_1<T,P1>,1);
		sq_newslot(v,-3,false);
		sq_pop(v,1); // pop class
*/

		BindRootVariable( (CSqEngineExport*)this, SQEXE_SELF );

		// Allow derived class to register it's stuff
		OnRegisterVariables();

		// Custom registrations
		if ( m_fExportSymbols )
#if defined( SQBIND_SQBIND )
			m_fExportSymbols( m_vm.GetVMHandle(), m_pSqAllocator );
#else
			m_fExportSymbols( &m_vm, m_pSqAllocator );
#endif

		// Want the exit function called
		m_bCallExit = oex::oexTRUE;

	} // end try

	_oexCATCH( SScriptErrorInfo &e )
	{	return LogError( oex::oexFALSE, e ); }
	_oexCATCH( SquirrelError &e )
	{	return LogErrorM( oex::oexFALSE, e.desc ); }

	m_bLoaded = TRUE;

	return oex::oexTRUE;
}

oex::oexBOOL CSqEngine::Load( oex::oexCSTR pScript, oex::oexBOOL bFile, oex::oexBOOL bRelative, oex::oexBOOL bStart )
{_STT();
	if ( !oexCHECK_PTR( pScript ) || !*pScript )
		return oex::oexFALSE;

	if ( !Init() )
		return oex::oexFALSE;

	_oexTRY
	{
		oex::CStr sFull;

		if ( bFile )
		{
			// +++ Change this to current working directory
			//     Add oexWorkingDirectory()

			// Does it point to a valid file?
			if ( oexExists( pScript ) )

				m_sRoot = oex::CStr( pScript ).GetPath().Ptr();

			else
			{
				// Check relative to exe path
				sFull = oexGetModulePath();
				sFull.BuildPath( pScript );
				if ( oexExists( sFull.Ptr() ) )
				{	m_sRoot = sFull.GetPath().Ptr();
					pScript = sFull.Ptr();
				} // end if

				else
				{
					// Check relative to scripts path
					sFull = oexGetModulePath();
					sFull.BuildPath( oexT( "scripts" ) );
					sFull.BuildPath( pScript );
					if ( oexExists( sFull.Ptr() ) )
					{	m_sRoot = sFull.GetPath().Ptr();
						pScript = sFull.Ptr();
					} // end if

					else
					{	oexERROR( 0, oexMks( oexT( "Script not found : " ), pScript ) );
						return oex::oexFALSE;
					} // end else

				} // end else

			} // end else

		} // end if

		// Save away root path
		else
			m_sRoot = oexGetModulePath().Ptr();

		// Load the script
		m_script = bFile ? m_vm.CompileScript( pScript )
						 : m_vm.CompileBuffer( pScript );

		if ( bStart )
		{
			// Initialize the script
			m_vm.RunScript( m_script );

			// Execute init function
			Execute( WSQBIND_NOREPLY, SQEXE_FN_INIT );

		} // end if

	} // end try

	_oexCATCH( SScriptErrorInfo &e )
	{	return LogError( oex::oexFALSE, e ); }
	_oexCATCH( SquirrelError &e )
	{	return LogErrorM( oex::oexFALSE, e.desc ); }

	// Save script source information
	m_bFile = bFile;
	m_sScript = pScript;

	return oex::oexTRUE;
}

int CSqEngine::OnIncludeOnce( const stdString &sScript )
{_STT();
	// Has it already been included
	if ( m_mapIncludeScriptCache.end() != m_mapIncludeScriptCache.find( sScript ) )
		return 0;

	// Save the time we included it
	m_mapIncludeScriptCache[ sScript ] = oexGetUnixTime();

	// Include the thing
	return OnInclude( sScript );
}

int CSqEngine::OnInclude( const stdString &sScript )
{_STT();
	// Push script name
	int nRet = 0;
	stdString sScriptName = m_sScriptName;

	oex::oexBOOL bFile = oex::oexTRUE;
	oex::oexCSTR pScript = sScript.c_str();
	stdString sData;

	_oexTRY
	{
		// See if there is an embedded include handler
		if ( m_fIncludeScript )
		{
			if ( m_fIncludeScript( sScript, sData, m_sScriptName ) || !sData.length() )
				return -1;

			pScript = sData.c_str();
			bFile = oex::oexFALSE;

		} // end if

		// Load the script
		m_script = bFile ? m_vm.CompileScript( oexBuildPath( m_sRoot.c_str(), sScript.c_str() ).Ptr() )
						 : m_vm.CompileBuffer( pScript );

		// Run the script
		m_vm.RunScript( m_script );

	} // end try

	_oexCATCH( SScriptErrorInfo &e )
	{	nRet = LogError( -2, e );
	}
	_oexCATCH( SquirrelError &e )
	{	nRet = LogErrorM( -3, oexMks( e.desc, oexT( " : " ),
							  ( bFile && pScript ) ? pScript : oexT( "N/A" ) ).Ptr() );
	}

	if ( sScriptName.length() )
		m_sScriptName = sScriptName;

	return nRet;
}

oex::oexBOOL CSqEngine::Start()
{_STT();
	if ( !IsScript() )
		return oex::oexFALSE;

	_oexTRY
	{
		// Initialize the script
		m_vm.RunScript( m_script );

		// Execute init function
		Execute( WSQBIND_NOREPLY, SQEXE_FN_INIT );

	} // end try
	_oexCATCH( SScriptErrorInfo &e )
	{	return LogError( oex::oexFALSE, e ); }
	_oexCATCH( SquirrelError &e )
	{	return LogErrorM( oex::oexFALSE, e.desc ); }

	return oex::oexTRUE;
}

oex::oexBOOL CSqEngine::Idle()
{_STT();
	return Execute( WSQBIND_NOREPLY, SQEXE_FN_IDLE );
}

oex::oexBOOL CSqEngine::Run( oex::oexCSTR pScript )
{_STT();
	if ( !oexCHECK_PTR( pScript ) || !*pScript )
		return oex::oexFALSE;

	_oexTRY
	{
		SquirrelObject script( m_vm.GetVMHandle() );
		script = m_vm.CompileBuffer( pScript );
		m_vm.RunScript( script );

	} // end try

	_oexCATCH( SScriptErrorInfo &e )
	{	return LogError( oex::oexFALSE, e ); }
	_oexCATCH( SquirrelError &e )
	{	return LogErrorM( oex::oexFALSE, e.desc ); }

	return oex::oexTRUE;
}

void CSqEngine::SetScriptName( oex::oexCSTR pScriptName )
{_STT();
	if ( oexCHECK_PTR( pScriptName ) )
		m_sScriptName = pScriptName;
	else
		m_sScriptName = oexT( "" );
}

stdString CSqEngine::GetScriptName()
{_STT();
	return m_sScriptName;
}

oex::oexINT CSqEngine::LogError( oex::oexINT x_nReturn, SScriptErrorInfo &x_e )
{_STT();
	oex::CStr sErr;
	if ( x_e.sSource == oexT( "console_buffer" ) )
		sErr = oex::CStr().Fmt( oexT( "%s(%u)\r\n   %s" ), m_sScriptName.c_str(), x_e.uLine, x_e.sDesc.c_str() );
	else
		sErr = oex::CStr().Fmt( oexT( "%s(%u)\r\n   %s" ), x_e.sSource.c_str(), x_e.uLine, x_e.sDesc.c_str() );

	oexERROR( 0, sErr );

	m_sErr = sErr.Ptr();

#if defined( OEX_WINDOWS )
	oexRTRACE( oexT( "%s\n" ), m_sErr.c_str() );
#endif
	oexPrintf( oexT( "%s\n" ), m_sErr.c_str() );

	return x_nReturn;
}

oex::oexINT CSqEngine::LogError( oex::oexINT x_nReturn, oex::oexCSTR x_pErr, oex::oexCSTR x_pFile, oex::oexUINT x_uLine )
{_STT();
	if ( !oexCHECK_PTR( x_pFile ) )
		x_pFile = oexT( "" );

	if ( !oexCHECK_PTR( x_pErr ) )
		x_pErr = oexT( "" );

	oex::CStr sErr;
	if ( !x_pFile || oex::CStr( x_pFile ) == oexT( "console_buffer" ) )
		sErr = oex::CStr().Fmt( oexT( "%s(%u)\r\n   %s" ), m_sScriptName.c_str(), x_uLine, x_pErr );
	else
		sErr = oex::CStr().Fmt( oexT( "%s(%u)\r\n   %s" ), x_pFile, x_uLine, x_pErr );

	oexERROR( 0, sErr );

	m_sErr = sErr.Ptr();

#if defined( OEX_WINDOWS )
	oexRTRACE( oexT( "%s\n" ), m_sErr.c_str() );
#endif
	oexPrintf( oexT( "%s\n" ), m_sErr.c_str() );

	return x_nReturn;
}

void CSqEngine::SetModuleManager( CModuleManager *x_pMm )
{_STT();
	m_pModuleManager = x_pMm;
}

int CSqEngine::OnImport( const stdString &sClass )
{_STT();
	return -1;
}

static oex::CStr FindFile( const stdString &sRoot, oex::CStrList &lstSubs, oex::CStr &sFile )
{_STT();
	// Check all possible sub folders
	for ( oex::CStrList::iterator it; lstSubs.Next( it ); )
	{	oex::CStr sFull = oexBuildPath( sRoot.c_str(), *it );
		sFull.BuildPath( sFile );
		if ( oexExists( sFull.Ptr() ) )
			return sFull;
	} // end for

	return oex::CStr();
}

int CSqEngine::OnLoadModule( const stdString &sModule, const stdString &sPath )
{_STT();
	if ( !sModule.length() )
		return -1;

	if ( !m_pModuleManager )
		return -2;

	oex::CStrList lstSubs;
	lstSubs << oexT( "sqmod" ) << oexT( "_sqmod" ) << oexT( "modules" );

	// Build module file name
	oex::CStr sFile = ( oex::CStr() << oexT( "sqmod_" ) <<  sModule.c_str() ).DecorateName( oex::oexTRUE, oex::oexTRUE );
	oex::CStr sFull;

	// User path?
	if ( sPath.length() )
		sFull = FindFile( sPath, lstSubs, sFile );

	else
	{
#if !defined( oexDEBUG )

		// Check install folder
		oex::CStr sInstallRoot = oex::os::CSysUtil::GetRegString( oexT( "HKLM" ), oexT( "Software\\SquirrelScript" ), oexT( "Install_Dir" ) );
		if ( sInstallRoot.Length() )
			sFull = FindFile( sInstallRoot.Ptr(), lstSubs, sFile );

#endif

		// Check exe folder
		if ( !sFull.Length() )
			sFull = FindFile( oexGetModulePath().Ptr(), lstSubs, sFile );

	} // end else

	if ( !sFull.Length() )
		return -3;

	// See if the module is already loaded
	oex::oexBOOL bExists = m_pModuleManager->Exists( sFull.Ptr() );

	// Attempt to load the module
	CModuleInstance *pMi = m_pModuleManager->Load( sFull.Ptr() );
	if ( !pMi )
	{	oexERROR( 0, oexMks( oexT( "Failed to load module " ), sFull ) );
		return -4;
	} // end if

	// Export functionality
#if defined( SQBIND_SQBIND )
	if ( !pMi->Export( m_vm.GetVMHandle() ) )
#else
	if ( !pMi->Export( &m_vm ) )
#endif
	{	oexERROR( 0, oexMks( oexT( "Failed to export squirrel symbols from module " ), sFull ) );
		return -5;
	} // end if

	// Log the fact that we loaded said module
	if ( !bExists )
		oexNOTICE( 0, oexMks( oexT( "Module loaded : " ), sFull ) );

	return 0;
}

void CSqEngine::SetMessageQueue( CSqMsgQueue *pMq )
{_STT();
	m_pMsgQueue = pMq;
}

CSqMsgQueue* CSqEngine::OnGetQueue()
{_STT();
	return m_pMsgQueue;
}

