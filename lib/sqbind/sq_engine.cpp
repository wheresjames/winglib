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


CSqEngineExport::CSqEngineExport()
{_STT();

	m_bFile = oex::oexFALSE;
}

CSqEngineExport::~CSqEngineExport()
{
}


int CSqEngineExport::isDebugBuild()
{_STT();
#if defined( oexDEBUG )
	return 1;
#else
	return 0;
#endif
}

stdString CSqEngineExport::getCompiler()
{_STT();

#if defined( OEX_GCC )
	return oexT( "gcc" );
#endif

#if defined( OEX_MSC )
	return oexT( "msvc" );
#endif

	return "";
}

stdString CSqEngineExport::getPlatform()
{_STT();

#if defined( _WIN32_WCE ) || defined( _WIN64_WCE )
	return oexT( "wince" );
#endif

#if defined( OEX_WINDOWS )
	return oexT( "windows" );
#endif

#if defined( OEX_IPHONE )
	return oexT( "iphone" );
#endif

	return oexT( "posix" );
}

int CSqEngineExport::getCpuSize()
{_STT();
#if defined( OEX_CPU_64 )
	return 64;
#else
	return 32;
#endif
}

stdString CSqEngineExport::getCpuType()
{_STT();

#if defined( CPU_ARM )
	return oexT( "arm" );
#endif

#if defined( OEX_CPU_64 )
	return oexT( "x64" );
#endif

	return oexT( "x86" );

}

int CSqEngineExport::ctrl_computer( int nCmd, int nForce, const sqbind::stdString &sMsg )
{_STT();
	return oexCtrlComputer( nCmd, nForce, sMsg.c_str() );
}

int CSqEngineExport::reboot( int nForce, const sqbind::stdString &sMsg )
{_STT();
	return oexCtrlComputer( 1, nForce, sMsg.c_str() );
}

int CSqEngineExport::shutdown( int nForce, const sqbind::stdString &sMsg )
{_STT();
	return oexCtrlComputer( 2, nForce, sMsg.c_str() );
}

int CSqEngineExport::logoff( int nForce, const sqbind::stdString &sMsg )
{_STT();
	return oexCtrlComputer( 4, nForce, sMsg.c_str() );
}

int CSqEngineExport::set_root()
{_STT();
	return oexSetRoot();
}

int CSqEngineExport::is_root()
{_STT();
	return oexIsRoot();
}

int CSqEngineExport::alert( const stdString &sMsg )
{_STT();
	return oex::os::CSys::ShowMessageBox( oexT( "Script Message" ), sMsg.c_str() );
}

int CSqEngineExport::print( const stdString &sMsg )
{_STT();
	oex::CUtil::AddOutput( sMsg.c_str(), sMsg.length(), oex::oexTRUE );
	return oexPrintf( sMsg.c_str() );
}

int CSqEngineExport::echo( const stdString &sMsg )
{_STT();
	return oexEcho( sMsg.c_str() );
}

stdString CSqEngineExport::read_stdin( int nMax )
{_STT();
	return oex2std( oexReadStdin( nMax ) );
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

stdString CSqEngineExport::include_inline( const stdString &sScript, CSqMulti *mParams )
{_STT();
	push_stack( mParams );
	OnIncludeInline( sScript );
	return pop_stack();
}

stdString CSqEngineExport::run_inline( const stdString &sScript, CSqMulti *mParams )
{_STT();
	push_stack( mParams );
	OnRunInline( sScript );
	return pop_stack();
}

stdString CSqEngineExport::get_script_return_value()
{_STT();
	return m_sReturnData;
}


int CSqEngineExport::push_stack( CSqMulti *mParams )
{_STT();
	m_lstScriptData.push_back( mParams );
	return m_lstScriptData.size();
}

stdString CSqEngineExport::pop_stack()
{_STT();
	if ( !m_lstScriptData.size() )
		return oexT( "" );
	stdString sOutput = m_lstScriptData.rbegin()->sOutput;
	m_lstScriptData.pop_back();
	return sOutput;
}

CSqMulti CSqEngineExport::get_stack_params()
{_STT();
	if ( !m_lstScriptData.size() ) return oexT( "" );
	return m_lstScriptData.rbegin()->mParams;
}

int CSqEngineExport::set_stack_params( CSqMulti *mParams )
{_STT();
	if ( !mParams || !m_lstScriptData.size() ) return 0;
	m_lstScriptData.rbegin()->mParams = *mParams;
	return m_lstScriptData.size();
}

stdString CSqEngineExport::get_stack_output()
{_STT();
	if ( !m_lstScriptData.size() ) return oexT( "" );
	return m_lstScriptData.rbegin()->sOutput;
}

int CSqEngineExport::set_stack_output( const stdString &sOutput )
{_STT();
	if ( !m_lstScriptData.size() ) return 0;
	m_lstScriptData.rbegin()->sOutput = sOutput;
	return m_lstScriptData.size();
}

int CSqEngineExport::append_stack_output( const stdString &sOutput )
{_STT();
	if ( !m_lstScriptData.size() ) return 0;
	m_lstScriptData.rbegin()->sOutput += sOutput;
	return m_lstScriptData.size();
}

int CSqEngineExport::get_stack_size()
{_STT();
	return m_lstScriptData.size();
}


int CSqEngineExport::load_module( const stdString &sModule, const stdString &sPath )
{_STT();
	return OnLoadModule( sModule, sPath );
}

int CSqEngineExport::set_thread_priority( int nPriority )
{_STT();
	return oex::os::CSys::SetThreadPriority( nPriority );
}

int CSqEngineExport::set_thread_affinity( int nAffinity )
{_STT();
	return oex::os::CSys::SetThreadAffinity( nAffinity );
}


int CSqEngineExport::kill( const stdString &sPath )
{_STT();
	CSqMsgQueue *q = queue();
	if ( !q ) return -1;
	return q->kill( oexNULL, sPath );
}

int CSqEngineExport::kill_wait( const stdString &sPath, int nTimeout, int bTerminate )
{_STT();

	CSqMsgQueue *q = queue();
	if ( !q ) return -1;

	// Send thread the kill signal
	q->kill( oexNULL, sPath );

	// Use default timeout
	if ( 0 >= nTimeout )
		nTimeout = ( oexDEFAULT_WAIT_TIMEOUT / 1000 );

	// Wait for thread to shutdown
	oex::oexUINT uStart = oexGetUnixTime() + nTimeout;
	while ( is_path( sPath ) )
	{ 
		// Wait a bit
		oexSleep( oexWAIT_RESOLUTION / 1000 );

		// Call cleanup in case it's our own thread
		q->Cleanup();

		// Are we timed out?
		if ( uStart < oexGetUnixTime() )
		{
			// Are we killing the thread?
			if ( !bTerminate )
				return 0;

			// Kill it and wait again
			bTerminate = 0;
			q->terminate_thread( oexNULL, sPath );
			uStart = oexGetUnixTime() + nTimeout;

		} // end if

	} // end while

	return 1;
}

int CSqEngineExport::terminate_thread( const stdString &sPath )
{_STT();
	CSqMsgQueue *q = queue();
	if ( !q ) return -1;
	int ret = q->terminate_thread( oexNULL, sPath );
	q->Cleanup();
	return ret;
}

void CSqEngineExport::exit( int nExitCode )
{_STT();
	oex::os::CSys::Quit( nExitCode );
}

void CSqEngineExport::terminate( int nExitCode )
{_STT();
	oex::os::CSys::Exit( nExitCode );
}

int CSqEngineExport::get_key()
{_STT();
	return oexGetKey();
}

int CSqEngineExport::is_key()
{_STT();
	return oexIsKey();
}


stdString CSqEngineExport::get_build()
{_STT();
	return oex2std( oexBuild() );
}

stdString CSqEngineExport::get_version()
{_STT();
	return oex2std( oexVersion() );
}

int CSqEngineExport::compare_version( const stdString &sV1, const stdString &sV2, const stdString &sSep )
{
	return oexCompareVersion( std2oex( sV1 ), std2oex( sV2 ), std2oex( sSep ) );
}

stdString CSqEngineExport::get_app_name()
{_STT();
	oex::oexCSTR p = CScriptThread::getAppName();
	return p ? p : oexT( "" );
}

stdString CSqEngineExport::get_service_name()
{_STT();
	oex::oexCSTR p = CScriptThread::getAppNameProc();
	return p ? p : oexT( "" );
}

stdString CSqEngineExport::get_app_long_name()
{_STT();
	oex::oexCSTR p = CScriptThread::getAppLongName();
	return p ? p : oexT( "" );
}

stdString CSqEngineExport::get_app_description()
{_STT();
	oex::oexCSTR p = CScriptThread::getAppDescription();
	return p ? p : oexT( "" );
}

stdString CSqEngineExport::get_name()
{_STT();
	return m_sScriptName;
}

stdString CSqEngineExport::get_script()
{_STT();
	return m_sScript;
}

stdString CSqEngineExport::path( const stdString &sPath )
{_STT();
	return OnPath( sPath );
}

stdString CSqEngineExport::decorate( const stdString &sPath, int bExe, int bLib )
{_STT();
	return oex2std( std2oex( sPath ).DecorateName( bExe, bLib ) );
}

stdString CSqEngineExport::build_path( const stdString &sS1,  const stdString &sS2 )
{_STT();
	return oex2std( std2oex( sS1 ).BuildPath( std2oex( sS2 ) ) );
}

stdString CSqEngineExport::root( const stdString &sPath )
{_STT();
	return oex2std( oexGetModulePath( sPath.c_str() ) );
}

stdString CSqEngineExport::get_sys_path( int bShared, const stdString &sId, const stdString &sPath )
{_STT();
	return build_path( get_sys_folder( bShared, sId ), sPath );
}


stdString CSqEngineExport::get_sys_folder( int bShared, const stdString &sId )
{_STT();

	oex::oexINT nId = oexStrToLong( sId.c_str() );
	if ( !nId )
	{
		if ( sId == oexT( "temp" ) )
			nId = oex::os::CBaseFile::eFidTemp;

		else if ( sId == oexT( "sys" ) )
			nId = oex::os::CBaseFile::eFidSystem;

		else if ( sId == oexT( "os" ) )
			nId = oex::os::CBaseFile::eFidUserOs;

		else if ( sId == oexT( "current" ) )
			nId = oex::os::CBaseFile::eFidCurrent;

		else if ( sId == oexT( "root" ) )
			nId = oex::os::CBaseFile::eFidRoot;

		else if ( sId == oexT( "default_drive" ) )
			nId = oex::os::CBaseFile::eFidDefDrive;

		else if ( sId == oexT( "fonts" ) )
			nId = oex::os::CBaseFile::eFidFonts;

		else if ( sId == oexT( "desktop" ) )
			nId = oex::os::CBaseFile::eFidDesktop;

		else if ( sId == oexT( "downloads" ) )
			nId = oex::os::CBaseFile::eFidDownloads;

		else if ( sId == oexT( "settings" ) )
			nId = oex::os::CBaseFile::eFidSettings;

		else if ( sId == oexT( "recycle" ) )
			nId = oex::os::CBaseFile::eFidRecycle;

		else if ( sId == oexT( "templates" ) )
			nId = oex::os::CBaseFile::eFidTemplates;

		else if ( sId == oexT( "public" ) )
			nId = oex::os::CBaseFile::eFidPublic;

		else if ( sId == oexT( "documents" ) )
			nId = oex::os::CBaseFile::eFidDocuments;

		else if ( sId == oexT( "music" ) )
			nId = oex::os::CBaseFile::eFidMusic;

		else if ( sId == oexT( "pictures" ) )
			nId = oex::os::CBaseFile::eFidPictures;

		else if ( sId == oexT( "video" ) )
			nId = oex::os::CBaseFile::eFidVideo;

		else if ( sId == oexT( "favorites" ) )
			nId = oex::os::CBaseFile::eFidFavorites;

		else if ( sId == oexT( "start_menu" ) )
			nId = oex::os::CBaseFile::eFidStartMenu;

		else if ( sId == oexT( "startup" ) )
			nId = oex::os::CBaseFile::eFidStartup;

		else if ( sId == oexT( "cookies" ) )
			nId = oex::os::CBaseFile::eFidCookies;

		else if ( sId == oexT( "network" ) )
			nId = oex::os::CBaseFile::eFidNetwork;

		else if ( sId == oexT( "printers" ) )
			nId = oex::os::CBaseFile::eFidPrinters;

		else if ( sId == oexT( "recent" ) )
			nId = oex::os::CBaseFile::eFidRecent;

		else if ( sId == oexT( "history" ) )
			nId = oex::os::CBaseFile::eFidHistory;

	} // end if

	return sqbind::oex2std( oexGetSysFolder( bShared, nId ) );
}

stdString CSqEngineExport::module_name()
{_STT();
	return oex2std( oexGetModuleFileName() );
}

stdString CSqEngineExport::module_path()
{_STT();
	return oex2std( oexGetModulePath() );
}

CSqMulti CSqEngineExport::find_resource( const stdString &sName, int bIgnoreCase, int bFileOverrideOk )
{_STT();

	// Get a list of resources matching the pattern
	oex::CStrList l = oexFindResource( std2oex( sName ), bIgnoreCase );
	if ( l.Size() )
	{	CSqMulti m; 
		SQBIND_StrListToStd( l, m.list(), oexT( "f" ) );	
		return m;
	} // end if
	
	// If none, check file path
	if ( !bFileOverrideOk )
		return CSqMulti();

	// Attempt to get the list from disk
	stdString sSub = CSqFile::get_path( sName );
	CSqMulti files = CSqFile::get_dirlist( path( build_path( oexT( ".." ), sSub ) ), 
										   CSqFile::get_filename( sName ), 1, 0 );
	if ( !files.size() )
		return CSqMulti();
		
	// Make the paths look right
	CSqMulti m;
	for ( CSqMulti::iterator it = files.begin(); it != files.end(); it++ )
		m[ build_path( sSub, it->first ) ] = it->second.str();
	
	return m;
}

stdString CSqEngineExport::get_resource( const stdString &sRes, int bFileOverrideOk )
{_STT();
	return get_resource_bin( sRes, bFileOverrideOk ).getString();
}

CSqBinary CSqEngineExport::get_resource_bin( const stdString &sRes, int bFileOverrideOk )
{_STT();
	// Data container
	oex::CBin b;

	if ( bFileOverrideOk )
	{
		// +++ This really isn't the way to handle the paths.
		//     I can't think of a good approach atm, so will
		//     think this through later.

		// Look in script path
		stdString sSub = path( build_path( oexT( ".." ), sRes ) );
		if ( oexExists( sSub.c_str() ) )
			b = oexFileGetContents( sSub.c_str() );

		// Look in binary path
		else
		{	sSub = root( sRes );
			if ( oexExists( sSub.c_str() ) )
				b = oexFileGetContents( sSub.c_str() );
		} // end else

	} // end if

	// Check for resource
	if ( !b.getUsed() )
		b = oexGetResource( sRes.c_str() );

	return b;
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

int CSqEngineExport::enable_output_file_capture( const stdString &sName, unsigned int uTimestampFreq )
{_STT();
	return oexEnableOutputFileCapture( sName.c_str(), uTimestampFreq );
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
	return oex2std( oexMbToStr( s ) );
#else
	return oex2std( s );
#endif
}

/// Returns the buffer for a binary share
unsigned int CSqEngineExport::get_instance_handle()
{_STT();
	return oexPtrToInt( oex::os::CSys::GetInstanceHandle() );
}

stdString CSqEngineExport::build_url( CSqMulti *pUrl )
{_STT();
	if ( !pUrl )
		return oexT( "" );
	oex::CPropertyBag pb;
	SQBIND_MultiToPropertyBag( *pUrl, pb );
	return oex2std( oex::os::CIpAddress::BuildUrl( pb ) );
}

CSqMulti CSqEngineExport::parse_url( const stdString &sUrl )
{_STT();
	CSqMulti m;
	oex::CPropertyBag pb = oex::os::CIpAddress::ParseUrl( sUrl.c_str(), sUrl.length() );
	SQBIND_PropertyBagToMulti( pb, m );
	return m;
}

stdString CSqEngineExport::base64_encode( const stdString &sStr )
{_STT();
	return oex2std( oexMbToStr( oex::CBase64::Encode( oexStrToMb( std2oex( sStr ) ) ) ) );
}

stdString CSqEngineExport::base64_decode( const stdString &sStr )
{_STT();
	return oex2std( oexMbToStr( oex::CBase64::Decode( oexStrToMb( std2oex( sStr ) ) ) ) );
}

stdString CSqEngineExport::base16_encode( const stdString &sStr )
{_STT();
	return oex2std( oexMbToStr( oex::CBase16::Encode( oexStrToMb( std2oex( sStr ) ) ) ) );
}

stdString CSqEngineExport::base16_decode( const stdString &sStr )
{_STT();
	return oex2std( oexMbToStr( oex::CBase16::Decode( oexStrToMb( std2oex( sStr ) ) ) ) );
}

stdString CSqEngineExport::md5( const stdString &sStr )
{_STT();
	return oex2std( oexMd5( std2oex( sStr ) ) );
}

stdString CSqEngineExport::guid( const stdString &sStr )
{_STT();
	return oex2std( oexGuid( std2oex( sStr ) ) );
}

// mandelbrot( 64, 16, 0, -15, 40, 15 );
stdString CSqEngineExport::mandelbrot( int w, int h, int x1, int y1, int x2, int y2 )
{_STT();
	return oex2std( oexMandelbrot( w, h, x1, y1, x2, y2 ) );
}


stdString CSqEngineExport::unique()
{_STT();
	oex::oexGUID hash;
	return oex2std( oexMbToStr( oex::CBase16::Encode( oexUniqueGuid( &hash ), sizeof( hash ) ) ) );
}

int CSqEngineExport::local_time()
{_STT();
	return oexLocalTime().GetUnixTime();
}

int CSqEngineExport::gmt_time()
{_STT();
	return oexGmtTime().GetUnixTime();
}

SQInteger CSqEngineExport::local_time_useconds()
{_STT();
	return oexLocalTime().GetUnixTimeUs();
}

SQInteger CSqEngineExport::gmt_time_useconds()
{_STT();
	return oexGmtTime().GetUnixTimeUs();
}

stdString CSqEngineExport::local_timestr( const stdString &fmt )
{_STT();
	return oex2std( oexLocalTimeStr( fmt.c_str() ) );
}

stdString CSqEngineExport::gmt_timestr( const stdString &fmt )
{_STT();
	return oex2std( oexGmtTimeStr( fmt.c_str() ) );
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
	return oex2std( std2oex( sS ).LTrim( sChs.c_str() ) );
}

stdString CSqEngineExport::rtrim( const stdString &sS, const stdString &sChs )
{_STT();
	return oex2std( std2oex( sS ).RTrim( sChs.c_str() ) );
}

stdString CSqEngineExport::trim( const stdString &sS, const stdString &sChs )
{_STT();
	return oex2std( std2oex( sS ).Trim( sChs.c_str() ) );
}

stdString CSqEngineExport::trimws( const stdString &sS )
{_STT();
	return oex2std( std2oex( sS ).TrimWhiteSpace() );
}

int CSqEngineExport::find( const stdString &sS, const stdString &sSub )
{_STT();
	return std2oex( sS ).Match( sSub.c_str() );
}

int CSqEngineExport::ifind( const stdString &sS, const stdString &sSub )
{_STT();
	return std2oex( sS ).IMatch( sSub.c_str() );
}

stdString CSqEngineExport::create_size_string( double d, double dDiv, int nDigits, const stdString &sSuffix )
{_STT();
	oex::CStr s = std2oex( sSuffix );	
	oexCONST oex::CStr::t_char *suf[ 128 ], sep[] = { oexT( ',' ) };
	suf[ oex::str::InplaceSplit( s._Ptr(), s.Length(), suf, oexSizeOfArray( suf ), sep, sizeof( sep ) ) ] = 0;
	return oex2std( oex::CStr().AppendSizeString( d, dDiv, nDigits, suf ) );
}

stdString CSqEngineExport::replace( const stdString &sS, const stdString &sFind, const stdString &sReplace )
{_STT();
	return oex2std( std2oex( sS ).Replace( std2oex( sFind ), std2oex( sReplace ) ) );
}

stdString CSqEngineExport::str_limit( const stdString &sS, int maxchars, const stdString &sAppend )
{_STT();
	return oex2std( std2oex( sS ).Clip( maxchars, std2oex( sAppend ) ) );
}

stdString CSqEngineExport::ireplace( const stdString &sS, const stdString &sFind, const stdString &sReplace )
{_STT();
	return oex2std( std2oex( sS ).Replace( std2oex( sFind ), std2oex( sReplace ) ) );
}

stdString CSqEngineExport::drop( const stdString &sS, const stdString &sDrop, int bInclusive )
{_STT();
	return oex2std( std2oex( sS ).Drop( sDrop.c_str(), bInclusive ) );
}

stdString CSqEngineExport::drop_range( const stdString &sS, const stdString &sBegin, const stdString &sEnd, int bInclusive )
{_STT();
	return oex2std( std2oex( sS ).DropRange( sBegin.c_str()[ 0 ], sEnd.c_str()[ 0 ], bInclusive ) );
}

stdString CSqEngineExport::urlencode( const stdString &sS )
{_STT();
	return oex2std( oexUrlEncode( std2oex( sS ) ) );
}

stdString CSqEngineExport::urldecode( const stdString &sS )
{_STT();
	return oex2std( oexUrlDecode( std2oex( sS ) ) );
}

stdString CSqEngineExport::json_encode( const stdString &sS )
{_STT();
	return oex2std( oexJsonEncode( std2oex( sS ) ) );
}

stdString CSqEngineExport::json_decode( const stdString &sS )
{_STT();
	return oex2std( oexJsonDecode( std2oex( sS ) ) );
}

stdString CSqEngineExport::htmlencode( const stdString &sS )
{_STT();
	return oex2std( oexHtmlEncode( std2oex( sS ) ) );
}

stdString CSqEngineExport::htmlencode_limit( const stdString &sS, int nMaxChars, const stdString &sAppend )
{_STT();
	return oex2std( oexHtmlEncode( std2oex( sS ) ).Clip( nMaxChars, std2oex( sAppend ) ) );
}

stdString CSqEngineExport::htmldecode( const stdString &sS )
{_STT();
	return oex2std( oexHtmlDecode( std2oex( sS ) ) );
}

stdString CSqEngineExport::compress( const stdString &sS )
{_STT();
	return oex2std( oexCompressStr( std2oex( sS ) ) );
}

stdString CSqEngineExport::uncompress( const stdString &sS )
{_STT();
	return oex2std( oexUncompressStr( std2oex( sS ) ) );
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

int CSqEngineExport::spawn2( int nRet, const stdString &sPath, const stdString &sName, const stdString &sScriptName, const stdString &sScript, int bFile )
{_STT();
	CSqMsgQueue *q = queue();
	if ( !q ) return -1;

	if ( !nRet )
		return q->spawn2( oexNULL, sPath, sName, sScriptName, sScript, bFile );

	stdString sRet;
	return q->spawn2( &sRet, sPath, sName, sScriptName, sScript, bFile );
}

double CSqEngineExport::get_cpu_load()
{_STT();
	return oexGetCpuLoad();
}

double CSqEngineExport::boot_time()
{_STT();
//	oexSHOW( oex::os::CHqTimer::GetTimerSeconds() );
	return oex::os::CHqTimer::GetTimerSeconds();
}

unsigned int CSqEngineExport::get_timer_seconds()
{_STT();
	oex::oexINT64 nVal = 0;
	if ( !oex::os::CHqTimer::osGetCounts( &nVal, oexNULL ) )
		return 0;
	return (unsigned int)nVal;
}

SQInteger CSqEngineExport::get_timer_useconds()
{_STT();
	oex::oexINT64 nVal = 0;
	if ( !oex::os::CHqTimer::osGetCounts( oexNULL, &nVal ) )
		return 0;
	return (oex::oexINT64)nVal;
}

SQInteger CSqEngineExport::get_total_useconds()
{_STT();
	oex::oexINT64 nS = 0, nU = 0;
	if ( !oex::os::CHqTimer::osGetCounts( &nS, &nU ) )
		return 0;
	return (SQInteger)( ( nS * 1000000 ) + nU );
}

stdString CSqEngineExport::run( int nRet, const stdString &sPath, const stdString &sName, const stdString &sScript )
{_STT();

	CSqMsgQueue *q = queue();
	if ( !q )
		return oexT( "" );

	if ( !nRet )
	{	q->run( oexNULL, sPath, sName, sScript );
		return oexT( "" );
	} // end if

	stdString sRet;
	q->run( &sRet, sPath, sName, sScript );
	return sRet;
}

SquirrelObject CSqEngineExport::run2( const stdString &sName, const stdString &sScript )
{_STT();
	return OnRun( sName, sScript );
}


stdString CSqEngineExport::prepare_inline( const stdString &sScript, int bFile )
{_STT();
	return sqbind::prepare_inline( sScript, bFile );
}

void CSqEngineExport::set( const stdString &sPath, const stdString &sKey, const stdString &sVal )
{_STT();
	CSqMsgQueue *q = queue();
	if ( q )
		q->set( sPath, sKey, sVal );
}

void CSqEngineExport::tset( const stdString &sPath, const stdString &sKey, unsigned int uTo )
{_STT();
	CSqMsgQueue *q = queue();
	if ( q )
		q->tset( sPath, sKey, uTo );
}

stdString CSqEngineExport::get( const stdString &sPath, const stdString &sKey )
{_STT();
	CSqMsgQueue *q = queue();
	if ( !q ) return stdString();
	return q->get( sPath, sKey );
}

void CSqEngineExport::jset( const stdString &sPath, const stdString &sKey, const stdString &sVal )
{_STT();
	CSqMsgQueue *q = queue();
	if ( q )
		q->jset( sPath, sKey, sVal );
}

stdString CSqEngineExport::jget( const stdString &sPath, const stdString &sKey )
{_STT();
	CSqMsgQueue *q = queue();
	if ( !q ) return stdString();
	return q->jget( sPath, sKey );
}

int CSqEngineExport::asize( const stdString &sPath, const stdString &sKey )
{_STT();
	CSqMsgQueue *q = queue();
	if ( !q ) return 0;
	return q->asize( sPath, sKey );
}

void CSqEngineExport::aset( const stdString &sPath, const stdString &sKey, const stdString &sVal )
{_STT();
	CSqMsgQueue *q = queue();
	if ( q )
		q->aset( sPath, sKey, sVal );
}

void CSqEngineExport::mset( const stdString &sPath, const stdString &sKey, const stdString &sVal )
{_STT();
	CSqMsgQueue *q = queue();
	if ( q )
		q->mset( sPath, sKey, sVal );
}

stdString CSqEngineExport::kget( const stdString &sPath, const stdString &sKey )
{_STT();
	CSqMsgQueue *q = queue();
	if ( !q ) return stdString();
	return q->kget( sPath, sKey );
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

int CSqEngineExport::addlog( const stdString &sPath, const stdString &sKey, int nType )
{_STT();
	CSqMsgQueue *q = queue();
	if ( !q ) return 0;
	return q->addlog( sPath, sKey, nType );
}

int CSqEngineExport::removelog( const stdString &sPath, const stdString &sKey )
{_STT();
	CSqMsgQueue *q = queue();
	if ( !q ) return 0;
	return q->removelog( sPath, sKey );
}

int CSqEngineExport::setlogroot( const stdString &sPath, const stdString &sRoot )
{_STT();
	CSqMsgQueue *q = queue();
	if ( !q ) return 0;
	return q->setlogroot( sPath, sRoot );
}

stdString CSqEngineExport::getlogroot( const stdString &sPath )
{_STT();
	CSqMsgQueue *q = queue();
	if ( !q ) return oexT( "" );
	return q->getlogroot( sPath );
}

int CSqEngineExport::setlogfreq( const stdString &sPath, int nFreq )
{_STT();
	CSqMsgQueue *q = queue();
	if ( !q ) return 0;
	return q->setlogfreq( sPath, nFreq );
}

int CSqEngineExport::getlogfreq( const stdString &sPath )
{_STT();
	CSqMsgQueue *q = queue();
	if ( !q ) return 0;
	return q->getlogfreq( sPath );
}

stdString CSqEngineExport::getlogkeys( const stdString &sPath, int nTime )
{_STT();
	CSqMsgQueue *q = queue();
	if ( !q ) return stdString();
	return q->getlogkeys( sPath, nTime );
}

stdString CSqEngineExport::getlog( const stdString &sPath, const stdString &sKey, int nStart, int nStop, int nInterval, int nDataType, int nMethod )
{
	CSqMsgQueue *q = queue();
	if ( !q ) return stdString();
	return q->getlog( sPath, sKey, nStart, nStop, nInterval, nDataType, nMethod );
}

stdString CSqEngineExport::getlogbin( const stdString &sPath, const stdString &sKey, int nStart, int nStop, int nInterval, const stdString &sExt )
{	CSqMsgQueue *q = queue();
	if ( !q ) return oexT( "" );
	return q->getlogbin( sPath, sKey, nStart, nStop, nInterval, sExt );
}

int CSqEngineExport::logbin( const stdString &sPath, const stdString &sKey, const stdString &sBin, int nType, int nTime, int nTimeMs, int nBuffering )
{	CSqMsgQueue *q = queue();
	if ( !q ) return 0;
	q->logbin( sPath, sKey, sBin, nType, nTime, nTimeMs, nBuffering );
	return 1;
}


int CSqEngineExport::resetlog( const stdString &sPath )
{	CSqMsgQueue *q = queue();
	if ( !q ) return 0;
	return q->resetlog( sPath );
}

int CSqEngineExport::shell( const stdString &sFile, const stdString &sParams, const stdString &sDirectory )
{_STT();
	return oexShell( sFile.c_str(), sParams.c_str(), sDirectory.c_str() );
}

int CSqEngineExport::exec( const stdString &sFile, const stdString &sParams, const stdString &sDirectory )
{_STT();
	return oexExec( sFile.c_str(), sParams.c_str(), sDirectory.c_str() );
}

int CSqEngineExport::process_system_messages()
{
	return oex::os::CSys::PumpThreadMessages();
}

int CSqEngineExport::service_install( const stdString &sName, const stdString &sDesc, const stdString &sExe, int bAutoRestart )
{_STT();
	return oex::os::CServiceImpl::InstallService( sName.c_str(), sDesc.c_str(), sExe.c_str(), bAutoRestart );
}

int CSqEngineExport::service_remove( const stdString &sName )
{_STT();
	return oex::os::CServiceImpl::RemoveService( sName.c_str() );
}

int CSqEngineExport::service_start( const stdString &sName )
{_STT();
	return oex::os::CServiceImpl::Start( sName.c_str() );
}

int CSqEngineExport::service_stop( const stdString &sName )
{_STT();
	return oex::os::CServiceImpl::Stop( sName.c_str() );
}

int CSqEngineExport::service_restart( const stdString &sName )
{_STT();
	return oex::os::CServiceImpl::Restart( sName.c_str() );
}

CSqMulti CSqEngineExport::get_system_drive_info( const stdString &sDrive )
{_STT();
	CSqMulti m;
	oex::CPropertyBag pb;
	
	if ( sDrive.length() )
		pb[ sqbind::std2oex( sDrive ) ] = oex::os::CSysUtil::GetDiskInfo( std2oex( sDrive ) );
	else
		pb = oex::os::CSysUtil::GetDisksInfo( oex::oexTRUE );
		
	SQBIND_PropertyBagToMulti( pb, m );
	
	return m;
}

SQInteger CSqEngineExport::get_file_size( const stdString &sFile )
{
	return oexGetFileSize( sFile.c_str() );
}

CSqMulti CSqEngineExport::get_file_info( const stdString &sFile )
{_STT();
	CSqMulti m;
	oex::CPropertyBag pb = oexGetFileInfo( sFile.c_str() );
	SQBIND_PropertyBagToMulti( pb, m );
	return m;
}

int CSqEngineExport::reg_is_key( const stdString &sKey, const stdString &sPath, const stdString &sName )
{_STT();
	return oex::os::CSysUtil::IsRegValue( std2oex( sKey ), std2oex( sPath ), std2oex( sName ) );
}

stdString CSqEngineExport::reg_get_str( const stdString &sKey, const stdString &sPath, const stdString &sName )
{_STT();
	return oex2std( oex::os::CSysUtil::GetRegString( std2oex( sKey ), std2oex( sPath ), std2oex( sName ) ) );
}

int CSqEngineExport::reg_set_str( const stdString &sKey, const stdString &sPath, const stdString &sName, const stdString &sValue )
{_STT();
	return oex::os::CSysUtil::SetRegString( std2oex( sKey ), std2oex( sPath ), std2oex( sName ), std2oex( sValue ) );
}

int CSqEngineExport::reg_del_key( const stdString &sKey, const stdString &sPath, int bSubKeys )
{_STT();
	return oex::os::CSysUtil::DeleteRegKey( std2oex( sKey ), std2oex( sPath ), bSubKeys );
}

int CSqEngineExport::reg_del_sub_keys( const stdString &sKey, const stdString &sPath )
{_STT();
	return oex::os::CSysUtil::DeleteRegSubKeys( std2oex( sKey ), std2oex( sPath ) );
}

int CSqEngineExport::is_path( const stdString &sPath )
{_STT();
	CSqMsgQueue *q = queue();
	if ( !q ) return 0;
	return q->is_path( sPath );
}

void CSqEngineExport::error( int e, const stdString &sStr )
{_STT();
	oexERROR( 0, std2oex( sStr ) );
}

void CSqEngineExport::warning( int e, const stdString &sStr )
{_STT();
	oexWARNING( 0, std2oex( sStr ) );
}

void CSqEngineExport::notice( int e, const stdString &sStr )
{_STT();
	oexNOTICE( 0, std2oex( sStr ) );
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

int CSqEngineExport::sqexe_script( const stdString &sScript, const stdString &sParams, const stdString &sDir )
{_STT();
	stdString p = stdString( oexT( " -b:\"" ) ) + base64_encode( sScript ) + oexT( "\"" );
	if ( sParams.length() ) p = p + oexT( " " ) + sParams;
	return sqexe( p, sDir );
}

int CSqEngineExport::sqexe( const stdString &sParams, const stdString &sDir )
{_STT();

	// Ensure a valid script was supplied
	if ( !sParams.length() )
		return -1;

	// What's the name of the squirrel exe?
	stdString sFull, sExeName = oex2std( oex::CStr( oexT( "sqrl" ) ).DecorateName( oex::oexTRUE, oex::oexFALSE ) );

	if ( oexExists( path( sExeName ).c_str() ) )
		sFull = path( sExeName );

	else if ( oexExists( root( sExeName ).c_str() ) )
		sFull = root( sExeName );

#if defined( OEX_WINDOWS )

	else
	{
		#	if defined( OEX_GCC )
		#		define SQBUILD "gcc"
		#	elif defined( OEX_MSC )
		#		define SQBUILD "vs"
		#	else
		#		define SQBUILD "unk"
		#	endif

		#	if defined( OEX_SQENGINE )
		#		define SQKEYNAME OEX_SQENGINE
		#	elif defined( OEX_GCC )
		#		define SQKEYNAME "WinglibScriptEngine"
		#	else
		#		define SQKEYNAME "SquirrelScript"
		#	endif
		#	if defined( OEX_CPU_64 )
		#		define SQKEYCPU "x64"
		#	else
		#		define SQKEYCPU "x86"
		#	endif

		// Posible squirrel engines
		oex::oexCSTR sKeys[] = 
		{
			// Check for an engine like us
			oexT( "SOFTWARE\\" SQKEYNAME "_" SQKEYCPU "_" SQBUILD ),
			
			// +++ Really? wtf?
			// Any engine will probably do...
			oexT( "SOFTWARE\\SquirrelScript_x86" ),
			oexT( "SOFTWARE\\SquirrelScript_x86_gcc" ),
			oexT( "SOFTWARE\\SquirrelScript_x86_vs" ),
			oexT( "SOFTWARE\\SquirrelScript_x64" ),
			oexT( "SOFTWARE\\SquirrelScript_x64_gcc" ),
			oexT( "SOFTWARE\\SquirrelScript_x64_vs" ),
			oexT( "SOFTWARE\\WinglibScriptEngine_x86" ),
			oexT( "SOFTWARE\\WinglibScriptEngine_x86_gcc" ),
			oexT( "SOFTWARE\\WinglibScriptEngine_x86_vs" ),
			oexT( "SOFTWARE\\WinglibScriptEngine_x64" ),
			oexT( "SOFTWARE\\WinglibScriptEngine_x64_gcc" ),
			oexT( "SOFTWARE\\WinglibScriptEngine_x64_vs" ),
			0
		};

		// Find a squirrel engine install
		stdString sRoot; oex::oexINT i = 0;
		while ( !sRoot.length() && sKeys[ i ] )
			sRoot = reg_get_str( oexT( "HKLM" ), sKeys[ i++ ], oexT( "Install_Dir" ) );

		// Did we find an installed engine?
		if ( sRoot.length() )
			sFull = build_path( sRoot, oexT( "sqrl.exe" ) );

	} // end else

#endif

	// Did we find the file?
	if ( !sFull.length() )
		return -2;
	
	// Let's try and execute
	return oexShell( sFull.c_str(), sParams.c_str(), sDir.length() ? sDir.c_str() : path( stdString() ).c_str() );
}

int CSqEngineExport::type_size( int type )
{	return oex::obj::StaticSize( type );
}


void CSqEngineExport::sleep( int nMsTime )
{_STT();
	oexSleep( nMsTime );
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

int CSqEngineExport::OnIncludeInline( const stdString &sClass )
{_STT();
	return 0;
}

int CSqEngineExport::OnRunInline( const stdString &sClass )
{_STT();
	return 0;
}

int CSqEngineExport::OnLoadModule( const stdString &sModule, const stdString &sPath )
{_STT();
	return 0;
}

SquirrelObject CSqEngineExport::OnRun( const stdString &sName, const stdString &sScript )
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
	return oex2std( oexBuildPath( std2oex( m_sRoot ), std2oex( sPath ) ) );
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
	pSqEngine->m_sOutput += oex2std( sErr );
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
	m_uThreadId = 0;

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

	// Only the smame thread can delete the vm
//	if ( m_uThreadId == oexGetCurThreadId() )
		m_vm.Shutdown();
//	else
//		m_vm.Detach();

	m_mapIncludeScriptCache.clear();

	m_uThreadId = 0;
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

SQBIND_REGISTER_CLASS_BEGIN( CSqMsgQueue, CSqMsgQueue )
SQBIND_REGISTER_CLASS_END()

SQBIND_REGISTER_CLASS_BEGIN( CSqEngineExport, CSqEngineExport )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, isDebugBuild )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, getCompiler )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, getCpuSize )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, getCpuType )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, getPlatform )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, alert )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, print )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, echo )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, read_stdin )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, get_key )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, is_key )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, get_build )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, get_version )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, compare_version )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, flush )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, import )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, include )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, include_once )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, include_inline )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, run_inline )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, get_script_return_value )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, push_stack )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, pop_stack )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, get_stack_size )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, get_stack_params )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, set_stack_params )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, get_stack_output )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, set_stack_output )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, append_stack_output )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, load_module )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, get_children )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, sleep )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, clock )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, ticks )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, spawn )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, get )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, set )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, tset )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, jget )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, jset )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, kget )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, aset )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, asize )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, mset )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, isset )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, pb )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, addlog )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, removelog )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, setlogroot )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, getlogroot )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, setlogfreq )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, getlogfreq )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, getlogkeys )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, getlog )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, getlogbin )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, logbin )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, resetlog )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, run )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, run2 )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, prepare_inline )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, shell )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, exec )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, reboot )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, shutdown )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, logoff )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, ctrl_computer )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, is_root )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, set_root )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, process_system_messages )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, sqexe )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, sqexe_script )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, service_install )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, service_remove )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, service_start )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, service_stop )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, service_restart )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, get_system_drive_info )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, get_file_size )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, get_file_info )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, get_cpu_load )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, get_instance_handle )
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
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, terminate_thread )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, set_thread_priority )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, set_thread_affinity )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, queue )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, path )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, module_name )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, module_path )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, build_path )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, decorate )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, root )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, get_sys_folder )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, get_sys_path )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, md5 )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, guid )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, parse_url )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, build_url )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, base64_decode )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, base64_encode )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, base16_decode )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, base16_encode )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, unique )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, mandelbrot )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, local_time )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, gmt_time )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, local_time_useconds )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, gmt_time_useconds )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, get_timer_seconds )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, get_timer_useconds )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, get_total_useconds )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, local_timestr )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, gmt_timestr )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, set_timer )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, set_timeout )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, kill_timer )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, tolong )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, toulong )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, toint64 )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, touint64 )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, strtol )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, strtoul )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, tofloat )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, todouble )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, trim )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, ltrim )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, rtrim )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, trimws )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, find )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, ifind )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, str_limit )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, replace )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, ireplace )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, create_size_string )	
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, drop )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, drop_range )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, json_encode )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, json_decode )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, urlencode )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, urldecode )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, htmlencode )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, htmlencode_limit )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, htmldecode )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, compress )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, uncompress )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, find_resource )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, get_resource )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, get_resource_bin )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, get_name )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, get_app_name )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, get_service_name )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, get_app_long_name )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, get_app_description )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, get_script )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, get_binshare )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, set_binshare )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, is_binshare )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, cleanup_binshare )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, enable_output_capture )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, enable_output_file_capture )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, get_output )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, reg_is_key )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, reg_get_str )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, reg_set_str )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, reg_del_key )
	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, reg_del_sub_keys )

	SQBIND_MEMBER_FUNCTION(  CSqEngineExport, type_size )

	SQBIND_ENUM( oex::obj::tInt,		tInt )
	SQBIND_ENUM( oex::obj::tUInt,		tUInt )
	SQBIND_ENUM( oex::obj::tInt8,		tInt8 )
	SQBIND_ENUM( oex::obj::tUInt8,		tUInt8 )
	SQBIND_ENUM( oex::obj::tInt16,		tInt16 )
	SQBIND_ENUM( oex::obj::tUInt16,		tUInt16 )
	SQBIND_ENUM( oex::obj::tInt32,		tInt32 )
	SQBIND_ENUM( oex::obj::tUInt32,		tUInt32 )
	SQBIND_ENUM( oex::obj::tInt64,		tInt64 )
	SQBIND_ENUM( oex::obj::tUInt64,		tUInt64 )

	SQBIND_ENUM( oex::obj::tChar,		tChar )
	SQBIND_ENUM( oex::obj::tUChar,		tUChar )
	SQBIND_ENUM( oex::obj::tChar8,		tChar8 )
	SQBIND_ENUM( oex::obj::tUChar8,		tUChar8 )
	SQBIND_ENUM( oex::obj::tChar16,		tChar16 )
	SQBIND_ENUM( oex::obj::tUChar16,	tUChar16 )
	SQBIND_ENUM( oex::obj::tChar32,		tChar32 )
	SQBIND_ENUM( oex::obj::tUChar32,	tUChar32 )
	SQBIND_ENUM( oex::obj::tChar64,		tChar64 )
	SQBIND_ENUM( oex::obj::tUChar64,	tUChar64 )

	SQBIND_ENUM( oex::obj::tFloat,		tFloat )
	SQBIND_ENUM( oex::obj::tDouble,		tDouble )
	SQBIND_ENUM( oex::obj::tLongDouble,	tLongDouble )
	SQBIND_ENUM( oex::obj::tStr,		tStr )
	SQBIND_ENUM( oex::obj::tStrW,		tStrW )
	SQBIND_ENUM( oex::obj::tStr8,		tStr8 )
	SQBIND_ENUM( oex::obj::tStr16,		tStr16 )
	SQBIND_ENUM( oex::obj::tStr32,		tStr32 )
	SQBIND_ENUM( oex::obj::tStr64,		tStr64 )
	SQBIND_ENUM( oex::obj::tGuid,		tGuid )

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
		// Out with the old
//		Destroy();

		// Save away the id of the thread that created the vm
		m_uThreadId = oexGetCurThreadId();

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

static SQInteger str_write(SQUserPointer str,SQUserPointer p,SQInteger size)
{
	( (oex::CStr8*)str )->Append( (oex::CStr8::t_char*)p, size );
	return size;
}

oex::CStr8 CSqEngine::GetCompiledScript()
{
	oex::CStr8 buf;
	sq_pushobject( m_vm.GetVMHandle(), m_script.GetObjectHandle() );
	sq_writeclosure( m_vm.GetVMHandle(), str_write, &buf );
	sq_pop( m_vm.GetVMHandle(), 1 );
	return buf;
}

static SQInteger str_read(SQUserPointer str,SQUserPointer buf,SQInteger size)
{
	SQInteger len = ( (oex::CStr8*)str )->Length();
	if ( 0 >= len )
		return -1;

	if ( len > size )
		len = size;

	oexMemCpy( buf, ( (oex::CStr8*)str )->Ptr(), len );
	( (oex::CStr8*)str )->LTrim( len );
	return len;
}

oex::oexBOOL CSqEngine::SetCompiledScript( oex::CStr8 buf )
{
	if ( !buf.Length() )
		return oex::oexFALSE;

	sq_readclosure( m_vm.GetVMHandle(), str_read, &buf );
	sq_getstackobj( m_vm.GetVMHandle(), -1, &m_script.GetObjectHandle() );
	sq_addref( m_vm.GetVMHandle(), &m_script.GetObjectHandle() );

	return oex::oexTRUE;

} // end if

oex::oexBOOL CSqEngine::Load( const stdString &sScript, oex::CStr8 *pbScript,
							  oex::oexBOOL bFile, oex::oexBOOL bRelative,
							  oex::oexBOOL bStart, oex::oexBOOL bInline )
{_STT();

	if ( !sScript.length() && ( !pbScript || !pbScript->Length() ) )
		return oex::oexFALSE;

	if ( !Init() )
		return oex::oexFALSE;

	stdString sFile;

	_oexTRY
	{
		oex::CStr sFull;

		if ( bFile )
		{
			// +++ Change this to current working directory
			//     Add oexWorkingDirectory()

			if ( sScript.length() )
				sFile = sScript;
			else if ( pbScript && pbScript->Length() )
				sFile = oex2std( oexMbToStr( *pbScript ) );

			// Does it point to a valid file?
			if ( oexExists( sFile.c_str() ) )

				m_sRoot = oex2std( std2oex( sFile ).GetPath() );

			else
			{
				// Check relative to exe path
				sFull = oexGetModulePath();
				sFull.BuildPath( std2oex( sFile ) );
				if ( oexExists( sFull.Ptr() ) )
				{	m_sRoot = oex2std( sFull.GetPath() );
					sFile = oex2std( sFull );
				} // end if

				else
				{
					// Check relative to scripts path
					sFull = oexGetModulePath();
					sFull.BuildPath( oexT( "scripts" ) );
					sFull.BuildPath( std2oex( sFile ) );
					if ( oexExists( sFull.Ptr() ) )
					{	m_sRoot = oex2std( sFull.GetPath() );
						sFile = oex2std( sFull );
					} // end if

					else
					{	oexERROR( 0, oexMks( oexT( "Script not found : " ), std2oex( sFile ) ) );
						return oex::oexFALSE;
					} // end else

				} // end else

			} // end else

		} // end if

		// Save away root path
		else if ( !m_sRoot.length() )
			m_sRoot = oex2std( oexGetModulePath() );

		// Check for pre-compiled script
		if ( pbScript && 2 <= pbScript->Length() && ( *(oex::oexUSHORT*)pbScript->Ptr() ) == SQ_BYTECODE_STREAM_TAG )
			SetCompiledScript( *pbScript );

		else if ( bFile && bInline )
			m_script = m_vm.CompileBuffer( prepare_inline( sFile.c_str(), 1 ).c_str() );

		else if ( bFile )
			m_script = m_vm.CompileScript( sFile.c_str() );

		else if ( pbScript && pbScript->Length() )
			m_script = m_vm.CompileBuffer( bInline ? prepare_inline( oex82std( *pbScript ), 0 ).c_str() : pbScript->Ptr() );

		else
			m_script = m_vm.CompileBuffer( bInline ? prepare_inline( sScript, 0 ).c_str() : sScript.c_str() );

		if ( bStart )

			// Initialize the script
			m_sReturnData = obj2str( m_vm.RunScript( m_script ) );

	} // end try

	_oexCATCH( SScriptErrorInfo &e )
	{	return LogError( oex::oexFALSE, e ); }
	_oexCATCH( SquirrelError &e )
	{	return LogErrorM( oex::oexFALSE, e.desc );
	} // end catch

	if ( bStart )
	{
		_oexTRY
		{

			// Execute init function
			Execute( WSQBIND_NOREPLY, SQEXE_FN_INIT );

		} // end try

		_oexCATCH( SScriptErrorInfo &e )
		{	return LogError( oex::oexFALSE, e ); }
		_oexCATCH( SquirrelError &e )
		{	return LogErrorM( oex::oexFALSE, e.desc ); }

	} // end if

	// Save script source information
	m_bFile = bFile;
	m_sScript = bFile ? sFile : sScript;

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

int CSqEngine::OnIncludeInline( const stdString &sScript )
{_STT();

	// Push script name
	int nRet = 0;
	stdString sScriptName = m_sScriptName;
	oex::oexBOOL bFile = oex::oexTRUE;
	stdString sData;
	stdString sUseScript = sScript;

	_oexTRY
	{
		// See if there is an embedded include handler
		if ( m_fIncludeScript )
		{
			if ( m_fIncludeScript( sScript, sData, m_sScriptName ) || !sData.length() )
				return -1;

			sUseScript = sData;
			bFile = oex::oexFALSE;

		} // end if

		// Check for pre-compiled script
		if ( 2 <= sUseScript.length() && ( *(oex::oexUSHORT*)sUseScript.c_str() ) == SQ_BYTECODE_STREAM_TAG )
		{	oex::CStr8 buf( sUseScript.c_str(), sUseScript.length() );
			SetCompiledScript( buf );
		} // end if

		else
			m_script = m_vm.CompileBuffer( bFile
										   ? prepare_inline( std2oex( m_sRoot ).BuildPath( std2oex( sUseScript ) ).Ptr(), 1 ).c_str()
										   : prepare_inline( sUseScript.c_str(), 0 ).c_str() );

		m_sReturnData = obj2str( m_vm.RunScript( m_script ) );

	} // end try

	_oexCATCH( SScriptErrorInfo &e )
	{	nRet = LogError( -2, e );
	}
	_oexCATCH( SquirrelError &e )
	{	nRet = LogErrorM( -3, oexMks( e.desc, oexT( " : " ),
							  ( bFile && sScript.length() ) ? sScript.c_str() : oexT( "N/A" ) ).Ptr() );
	}

	return nRet;
}

int CSqEngine::OnRunInline( const stdString &sScript )
{_STT();

	// Push script name
	int nRet = 0;
	stdString sScriptName = m_sScriptName;
	stdString sData;

	_oexTRY
	{
		// Check for pre-compiled script
		if ( 2 <= sScript.length() && ( *(oex::oexUSHORT*)sScript.c_str() ) == SQ_BYTECODE_STREAM_TAG )
		{	oex::CStr8 buf( sScript.c_str(), sScript.length() );
			SetCompiledScript( buf );
		} // end if

		else
			m_script = m_vm.CompileBuffer( prepare_inline( sScript.c_str(), 0 ).c_str() );

		m_sReturnData = obj2str( m_vm.RunScript( m_script ) );

	} // end try

	_oexCATCH( SScriptErrorInfo &e )
	{	nRet = LogError( -2, e );
	}
	_oexCATCH( SquirrelError &e )
	{	nRet = LogErrorM( -3, oexMks( e.desc, oexT( " : " ),
							  ( sScript.length() ) ? sScript.c_str() : oexT( "N/A" ) ).Ptr() );
	}

	return nRet;
}

int CSqEngine::OnInclude( const stdString &sScript )
{_STT();
	// Push script name
	int nRet = 0;
	stdString sScriptName = m_sScriptName;
	oex::oexBOOL bFile = oex::oexTRUE;
	stdString sData, sUseScript = sScript;

	_oexTRY
	{
		// See if there is an embedded include handler
		if ( m_fIncludeScript )
		{
			if ( m_fIncludeScript( sScript, sData, m_sScriptName ) || !sData.length() )
				return -1;

			sUseScript = sData;
			bFile = oex::oexFALSE;

		} // end if

		// Check for pre-compiled script
		if ( 2 <= sUseScript.length() && ( *(oex::oexUSHORT*)sUseScript.c_str() ) == SQ_BYTECODE_STREAM_TAG )
		{	oex::CStr8 buf( sUseScript.c_str(), sUseScript.length() );
			SetCompiledScript( buf );
		} // end if

		else
		{
			// Don't return an error if the file simply doesn't exist
			oex::CStr sFile;
			if ( bFile )
			{	sFile = std2oex( m_sRoot ).BuildPath( std2oex( sUseScript ) );
				if ( !oexExists( sFile.Ptr() ) )
					return -1;
			} // end if

			// Load the script
			m_script = bFile ? m_vm.CompileScript( sFile.Ptr() )
							 : m_vm.CompileBuffer( sUseScript.c_str() );
							 
		} // end else

		// Run the script
		m_sReturnData = obj2str( m_vm.RunScript( m_script ) );

	} // end try

	_oexCATCH( SScriptErrorInfo &e )
	{	nRet = LogError( -2, e );
	}
	_oexCATCH( SquirrelError &e )
	{	nRet = LogErrorM( -3, oexMks( e.desc, oexT( " : " ),
							  ( bFile && sScript.length() ) ? sScript.c_str() : oexT( "N/A" ) ).Ptr() );
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
		m_sReturnData = obj2str( m_vm.RunScript( m_script ) );

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

oex::oexBOOL CSqEngine::Run( sqbind::stdString *pReply, oex::oexCSTR pName, oex::oexCSTR pScript )
{_STT();
	if ( !oexCHECK_PTR( pScript ) || !*pScript )
		return oex::oexFALSE;

	_oexTRY
	{
		SquirrelObject script( m_vm.GetVMHandle() );
		script = m_vm.CompileBuffer( pScript );
//		SquirrelObject o = m_vm.RunScript( script );
		m_sReturnData = obj2str( m_vm.RunScript( script ) );
		if ( pReply )
			*pReply = m_sReturnData;

	} // end try

	_oexCATCH( SScriptErrorInfo &e )
	{	return LogError( oex::oexFALSE, e, pName ); }
	_oexCATCH( SquirrelError &e )
	{	return LogErrorME( oex::oexFALSE, e.desc, pName ); }

	return oex::oexTRUE;
}

SquirrelObject CSqEngine::OnRun( const stdString &sName, const stdString &sScript )
{
	_oexTRY
	{
		return m_vm.RunScript( m_vm.CompileBuffer( sScript.c_str() ) );
	} // end try

	_oexCATCH( SScriptErrorInfo &e )
	{	LogError( oex::oexFALSE, e, sName.c_str() ); }
	_oexCATCH( SquirrelError &e )
	{	LogErrorME( oex::oexFALSE, e.desc, sName.c_str() ); }

	return SquirrelObject( m_vm.GetVMHandle() );
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

oex::oexINT CSqEngine::LogError( oex::oexINT x_nReturn, SScriptErrorInfo &x_e, oex::oexCSTR x_pExtra )
{_STT();
	oex::CStr sErr;
	if ( x_e.sSource == oexT( "console_buffer" ) )
		sErr = oex::CStr().Fmt( oexT( "%s(%u)\r\n   %s" ), m_sScriptName.c_str(), x_e.uLine, x_e.sDesc.c_str() );
	else
		sErr = oex::CStr().Fmt( oexT( "%s(%u)\r\n   %s" ), x_e.sSource.c_str(), x_e.uLine, x_e.sDesc.c_str() );

	if ( x_pExtra && *x_pExtra )
		sErr << oexNL << x_pExtra;

	oexERROR( 0, sErr );

	m_sErr = oex2std( sErr );

#if defined( OEX_WINDOWS )
	oexRTRACE( oexT( "%s\n" ), m_sErr.c_str() );
#endif
	oexEcho( m_sErr.c_str() );

	return x_nReturn;
}

oex::oexINT CSqEngine::LogError( oex::oexINT x_nReturn, oex::oexCSTR x_pErr, oex::oexCSTR x_pExtra, oex::oexCSTR x_pFile, oex::oexUINT x_uLine )
{_STT();
	if ( !oexCHECK_PTR( x_pFile ) )
		x_pFile = oexT( "" );

	if ( !oexCHECK_PTR( x_pErr ) )
		x_pErr = oexT( "" );

	oex::CStr sErr;
	if ( !x_pFile || oex::CStr( x_pFile ) == oexT( "console_buffer" ) )
		sErr = oex::CStr().Fmt( oexT( "%s(%u)\r\n   %s" ), m_sScriptName.c_str(), x_uLine, x_pErr );
	else
		sErr = oex::CStr().Fmt( oexT( "%s(%u)\r\n   %s" ), oex::CStr( x_pFile ).GetFileName().Ptr(), x_uLine, x_pErr );

	if ( x_pExtra && *x_pExtra )
		sErr << oexNL << x_pExtra;

	oexERROR( 0, sErr );

	m_sErr = oex2std( sErr );

#if defined( OEX_WINDOWS )
	oexRTRACE( oexT( "%s\n" ), m_sErr.c_str() );
#endif
	oexEcho( m_sErr.c_str() );

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

static oex::CStr FindFile( oex::CStr sRoot, oex::CStrList &lstSubs, oex::CStr &sFile )
{_STT();

	// Check all possible sub folders
	for ( oex::CStrList::iterator it; lstSubs.Next( it ); )
	{	oex::CStr sFull = sRoot;
		sFull.BuildPath( *it ).BuildPath( sFile );
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
	oex::CStr sFile = ( oex::CStr() << oexT( "sqmod_" ) <<  std2oex( sModule ) ).DecorateName( oex::oexTRUE, oex::oexTRUE );
	oex::CStr sFull;

	// User path?
	if ( sPath.length() )
		sFull = FindFile( std2oex( sPath ), lstSubs, sFile );

	else
	{
		// First search our local directory
		sFull = FindFile( oexGetModulePath(), lstSubs, sFile );

#if !defined( oexDEBUG )

		// Check Install directory
		if ( !sFull.Length() )
		{
		
// Build type
#	if defined( OEX_GCC )
#		define SQBUILD "gcc"
#	elif defined( OEX_MSC )
#		define SQBUILD "vs"
#	else
#		define SQBUILD "unk"
#	endif

// Engine name
#	if defined( OEX_SQENGINE )
#		define SQKEYNAME OEX_SQENGINE
#	elif defined( OEX_GCC )
#		define SQKEYNAME "WinglibScriptEngine"
#	else
#		define SQKEYNAME "SquirrelScript"
#	endif

// CPU type
#	if defined( OEX_CPU_64 )
#		define SQKEYCPU "x64"
#	else
#		define SQKEYCPU "x86"
#	endif

			oex::CStr sInstallRoot = oex::os::CSysUtil::GetRegString( oexT( "HKLM" ), oexT( "SOFTWARE\\" SQKEYNAME "_" SQKEYCPU "_" SQBUILD ), oexT( "Install_Dir" ) );
			if ( sInstallRoot.Length() )
				sFull = FindFile( sInstallRoot, lstSubs, sFile );
			else
			{	sInstallRoot = oex::os::CSysUtil::GetRegString( oexT( "HKLM" ), oexT( "SOFTWARE\\" SQKEYNAME "_" SQKEYCPU ), oexT( "Install_Dir" ) );
				if ( sInstallRoot.Length() )
					sFull = FindFile( sInstallRoot, lstSubs, sFile );
				else
					oexNOTICE( 0, oexT( "!!! Script Engine is Not Installed !!!" ) );
			} // end else
		} // end if
#endif

	} // end else

	if ( !sFull.Length() )
	{	oexERROR( 0, oexMks( oexT( "Module not found : " ), sModule.c_str() ) );
		return -3;
	} // end if

	// See if the module is already loaded
	oex::oexBOOL bExists = m_pModuleManager->Exists( sFull.Ptr() );

	// Attempt to load the module
	CModuleInstance *pMi = m_pModuleManager->Load( sFull.Ptr() );
	if ( !pMi )
	{	oexERROR( 0, oexMks( oexT( "Failed to load module : " ), sFull ) );
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

