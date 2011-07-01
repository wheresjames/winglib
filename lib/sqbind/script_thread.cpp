/*------------------------------------------------------------------
// script_thread.cpp
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

using namespace sqbind;

CScriptThread::CScriptThread()
{_STT();
	m_bQuit = oex::oexFALSE;
	m_pModuleManager = oexNULL;
	m_bFile = oex::oexFALSE;
	m_bInline = oex::oexFALSE;
	m_pParentScript = oexNULL;
	m_tid = 0;
	m_uToFreq = 100;
	m_uToElapsed = 0;
	m_dToValue = 0;
	m_uLogFreq = 1000;
}

CScriptThread::~CScriptThread()
{_STT();
	Destroy();
}

// Application description pointers
oex::oexCSTR CScriptThread::m_pAppName = oexNULL;
oex::oexCSTR CScriptThread::m_pAppNameProc = oexNULL;
oex::oexCSTR CScriptThread::m_pAppLongName = oexNULL;
oex::oexCSTR CScriptThread::m_pAppDescription = oexNULL;

void CScriptThread::SetAppInfo( oex::oexCSTR pName, oex::oexCSTR pNameProc, oex::oexCSTR pLName, oex::oexCSTR pDesc )
{
	m_pAppName = pName;
	m_pAppNameProc = pNameProc;
	m_pAppLongName = pLName;
	m_pAppDescription = pDesc;
}

oex::oexCSTR CScriptThread::getAppName() 
{
	return m_pAppName; 
}

oex::oexCSTR CScriptThread::getAppNameProc() 
{
	return m_pAppNameProc; 
}

oex::oexCSTR CScriptThread::getAppLongName() 
{
	return m_pAppLongName; 
}

oex::oexCSTR CScriptThread::getAppDescription() 
{
	return m_pAppDescription; 
}

void CScriptThread::Destroy()
{_STT();
	m_bQuit = oex::oexTRUE;
	Stop();
	m_pModuleManager = oexNULL;
	m_pParentScript = oexNULL;
}

void CScriptThread::SetModuleManager( sqbind::CModuleManager *pMm )
{_STT();
	m_pModuleManager = pMm;
}

void CScriptThread::SetScript( oexCONST oex::CStr8 &sScript, oex::oexBOOL bFile )
{_STT();
	m_sScript = sScript; m_bFile = bFile;
}

void CScriptThread::SetParentScript( CSqMsgQueue *pParent )
{_STT();
	m_pParentScript = pParent;
}

void CScriptThread::SetExportFunction( PFN_SQBIND_Export_Symbols fn, sqbind::SSqAllocator *pa )
{_STT();
	m_cSqEngine.SetExportFunction( fn, pa );
}

oex::oexBOOL CScriptThread::InitEngine()
{
	// +++ Ensure script
	if ( !m_sScript.Length() )
		return oex::oexFALSE;

	// Save thread id
	SetOwnerThreadId( oexGetCurThreadId() );

	// Set module manager
	m_cSqEngine.SetModuleManager( m_pModuleManager );

	// Pointer to our message queue
	m_cSqEngine.SetMessageQueue( this );

	// Set script name for debugging
	if ( m_cSqEngine.GetScriptName().length() && m_sName.length() && m_cSqEngine.GetScriptName() != m_sName )
	{	_STT_SET_NAME( oex::CStr() << std2oex( m_cSqEngine.GetScriptName() ) << oexT( " : " ) <<  std2oex( m_sName ) ); }
	else if ( m_sName.length() ) 
	{	_STT_SET_NAME( oex::CStr() << std2oex( m_sName ) ); }
	else 
	{	_STT_SET_NAME( oex::CStr() << std2oex( m_cSqEngine.GetScriptName() ) ); }

	return oex::oexTRUE;
}

oex::oexBOOL CScriptThread::InitThread( oex::oexPVOID x_pData )
{_STT();

	// Initialize the engine
	if ( !InitEngine() )
		return oex::oexFALSE;

	// Start the script
	if ( !m_cSqEngine.Load( stdString(), &m_sScript, m_bFile, oex::oexFALSE, oex::oexTRUE, m_bInline ) )
	{
		stdString sErr;

		if ( m_bFile )
			sErr += oexT( "File : " ),
			sErr += oex2std( oexMbToStr( m_sScript ) ) + oexT( "\r\n\r\n" );
		else
		{
			if ( m_cSqEngine.GetScriptName().length() )
				sErr += oexT( "Name : " ),
				sErr += m_cSqEngine.GetScriptName(),
				sErr += oexT( "\r\n" );

			if ( m_sScript.Length() )
			{	oex::CBin bin( m_sScript );
				sErr += oex2std( oexBinToAsciiHexStr( &bin, 0, 16, 4 ) );
				sErr += oexT( "\r\n" );
			} // end if

			sErr += oexT( "\r\n" );
		}

		if ( m_cSqEngine.GetLastError().length() )
			sErr += m_cSqEngine.GetLastError();
		else
			sErr += oexT( "Error loading script" );

		oexERROR( 0, oexMks( oexT( "Script Error : " ), std2oex( sErr ) ) );

		return oex::oexFALSE;

	} // end if

	return oex::oexTRUE;
}

oex::oexBOOL CScriptThread::DoThread( oex::oexPVOID x_pData )
{_STT();

	// Inline scripts should not have idle loops
	if ( m_bInline )
		return oex::oexFALSE;

	oex::oexINT nDiv = 0, nDiv10 = 0;
	oex::os::CTimeout toIdle;

	// Until we're told to stop
	while ( GetStopEvent().Wait( 0 ) )
	{
		// Wait for a message
		CSqMsgQueue::Wait( m_uToFreq );

		// Have we gotten a quit flag?
		if ( m_bQuit )
			return oex::oexFALSE;

		// Process script messages
		ProcessMsgs();

		// Run timers
		RunTimers();

		// Punt if quit has been requested
		if ( WantQuit() )
			return oex::oexFALSE;

		// Time for idle processing?
		if ( !toIdle.IsValid() )
		{
			// About every 100 milli-seconds
			toIdle.SetMs( 100 );

			// Run logging
			LogVariables();

			// Idle processing
			int nRet = 0;
			if ( !m_cSqEngine.Execute( &nRet, SQEXE_FN_IDLE ) )
				return oex::oexFALSE;

			// Did idle function indicate exit?
			if ( nRet )
				return oex::oexFALSE;

			// Once per second
			if ( 10 <= ++nDiv )
			{	nDiv = 0;

				// Cleanup tasks
				Cleanup();

				// Every 10 seconds
				if ( 10 <= ++nDiv10 )
				{	nDiv10 = 0;

					oexAutoLock ll( m_lockPb );
					if ( !ll.IsLocked() )
						return oex::oexFALSE;

					if ( m_lstLog.size() )
						m_log.Flush();

				} // end if

			} // end if

		} // end if

	} // end if

	return oex::oexFALSE;
}


oex::oexINT CScriptThread::EndThread( oex::oexPVOID x_pData )
{_STT();

	// Lock the queue
//	while ( CSqMsgQueue::GetLock().Wait( 100 ) )
//		ProcessMsgs();

	// Let script cleanup
	m_cSqEngine.Exit();

	// No more owner thread
	SetOwnerThreadId( 0 );

	// Lose child scripts
	DestroyChildScripts();

	// Lose the engine
	m_cSqEngine.Destroy();

	// Lose timeouts
	m_to.clear();

	// Reset timeout frequency
	m_uToFreq = 100;

	m_dToValue = 0;

	// Let the user know we're ending a thread
//	oexPrintf( oexT( "Exiting : 0x%08x : %s\n" ),
//			   (unsigned int)oexGetCurThreadId(), m_sName.c_str() );

	return 0;
}

oex::oexBOOL CScriptThread::LogVariables()
{
	// Are we logging anything?
	if ( !m_lstLog.size() )
		return oex::oexFALSE;

	// Is it time?
	if ( m_toLog.IsValid() )
		return oex::oexFALSE;

	// Not too fast
	if ( 100 > m_uLogFreq )
		m_uLogFreq = 1000;

	m_toLog.SetMs( m_uLogFreq );

	// Lock the property bag
	oexAutoLock ll( m_lockPb );
	if ( !ll.IsLocked() )
		return oex::oexFALSE;

	// What time is it?
	oex::CSysTime st( oex::CSysTime::eGmtTime );
	oex::oexUINT gmt = st.GetUnixTime();
	oex::oexUINT gmtms = st.GetMilliSecond();

	// Start knocking out the list
	for ( t_LogList::iterator it = m_lstLog.begin(); it != m_lstLog.end(); it++ )
	{	oex::CStr &s = m_pb.at( std2oex( it->first ) ).ToString();
		m_log.Log( it->second, s.Ptr(), s.Length(), gmt, gmtms );
	} // end while

	return oex::oexTRUE;
}


oex::oexINT CScriptThread::RunTimers()
{
	oex::oexDOUBLE dTm = oex::os::CHqTimer::GetTimerSeconds();

	if ( !m_dToValue )
	{	m_dToValue = dTm;
		return 0;
	} // end if

	oex::oexUINT uE = (oex::oexUINT)( ( dTm - m_dToValue ) * 1000 );
	m_dToValue = dTm;

	oex::oexINT n = 0;
	for ( t_TimerList::iterator it = m_to.begin(); m_to.end() != it; )
	{
		oex::oexBOOL bErase = oex::oexFALSE;

		// Timeout?
		if ( it->second.uTimeout )
		{	int nRet = 0;
			if ( it->second.uTimeout < oexGetUnixTime() )
			{	n++, m_cSqEngine.Execute( &nRet, it->second.sCallback.c_str() );
				bErase = oex::oexTRUE;
			} // end if
		} // end if

		// Timer?
		else if ( it->second.uMsTimeout )
		{
			it->second.uTo += uE;
			if ( it->second.uTo >= it->second.uMsTimeout )
			{
				n++;
				it->second.uTo -= it->second.uMsTimeout;
				int nRet = 0;
				m_cSqEngine.Execute( &nRet, it->second.sCallback.c_str() );

				if ( 0 > nRet )
					bErase = oex::oexTRUE;

			} // end if

		} // end else if

		// ???
		else
			bErase = oex::oexTRUE;

		if ( bErase )
			m_to.erase( it++ );
		else
			it++;

	} // end for

	// Remove timers
	for ( t_RemoveTimers::iterator it = m_lstRemoveTo.begin(); it != m_lstRemoveTo.end(); )
	{	t_TimerList::iterator itErase = m_to.find( *it );
		if ( itErase != m_to.end() )
			m_to.erase( itErase );
		m_lstRemoveTo.erase( it++ );
	}

	return n;
}

oex::oexUINT CScriptThread::SetTimer( oex::oexUINT x_uTo, const stdString &x_sCallback )
{
	if ( !x_uTo || !x_sCallback.length() )
		return 0;

	STimerInfo ti;
	ti.uTo = x_uTo; // Schedule to run first tick right away
	ti.uTimeout = 0;
	ti.uMsTimeout = x_uTo;
	ti.sCallback = x_sCallback;
	m_to[ ++m_tid ] = ti;

	if ( m_uToFreq > x_uTo )
		m_uToFreq = oex::cmn::Range( x_uTo, (oex::oexUINT)1, (oex::oexUINT)100 );

	return m_tid;
}

oex::oexUINT CScriptThread::SetTimeout( oex::oexUINT x_uTo, const stdString &x_sCallback )
{
	if ( !x_uTo || !x_sCallback.length() )
		return 0;

	STimerInfo ti;
	ti.uTo = 0;
	ti.uTimeout = x_uTo;
	ti.uMsTimeout = 0;
	ti.sCallback = x_sCallback;

	m_to[ ++m_tid ] = ti;
	return m_tid;
}

oex::oexBOOL CScriptThread::KillTimer( oex::oexUINT x_uId )
{
	t_TimerList::iterator it = m_to.find( x_uId );
	if ( it == m_to.end() )
		return oex::oexFALSE;

	m_lstRemoveTo.push_back( x_uId );

	return oex::oexTRUE;
}


oex::oexBOOL CScriptThread::ExecuteMsg( stdString &sMsg, CSqMulti &mapParams, stdString *pReply, oexEvent *pReplyEvent )
{_STT();

	oex::oexBOOL bRet = oex::oexTRUE;

	// Just a path check?
	if ( sMsg == oexT( "is_path" ) )
		bRet = oex::oexTRUE;

	// Terminate thread?  
	else if ( sMsg == oexT( "terminate_thread" ) )
	{	if ( GetOwnerThreadId() == oexGetCurThreadId() ) EndThread( 0 );
		RequestQuit();
		bRet = CThread::Stop( 3000, oex::oexTRUE ) ? oex::oexFALSE : oex::oexTRUE;
		EndThread( 0 );
	} // end else if

	// Is it a script map?
	else if ( ( sMsg == oexT( "pb_get" ) || sMsg == oexT( "pb_set" ) )
			  && *mapParams[ oexT( "key" ) ].str().c_str() == oexT( '@' ) )
	{
		oexAutoLock ll( m_lockPb );
		if ( !ll.IsLocked() )
			return oex::oexFALSE;

		sqbind::stdString &key = mapParams[ oexT( "key" ) ];
		oex::CStr sKey = std2oex( key );

		// Skip '@'
		sKey++;

		// Where to map
		sqbind::stdString sFunction  = oex2std( sKey.Parse( oexT( ":" ) ) );
		if ( !sFunction.length() ) sFunction = oex2std( sKey ), sKey.Destroy();
		else sKey++;

		// Did we get a function?
		if ( sFunction.length() )
		{
			CSqEngine *pEngine = GetEngine();
			if ( pEngine )
			{
				// Get params
				sqbind::stdString sP[ 4 ]; int i = 0;

				// Use val as first parameter if set command
				if ( sMsg == oexT( "pb_set" ) )
					sP[ i++ ] = mapParams[ oexT( "val" ) ];

				while( i < 4 && sKey.Length() )
				{	sP[ i ] = oex2std( sKey.Parse( oexT( ":" ) ) );
					if ( !sP[ i ].length() )
						sP[ i ] = oex2std( sKey ), sKey.Destroy();
					else
						sKey++;
					i++;
				} // end if

				switch( i )
				{	case 0 : pEngine->Execute( pReply, sFunction.c_str() ); break;
					case 1 : pEngine->Execute( pReply, sFunction.c_str(), sP[ 0 ] ); break;
					case 2 : pEngine->Execute( pReply, sFunction.c_str(), sP[ 0 ], sP[ 1 ] ); break;
					case 3 : pEngine->Execute( pReply, sFunction.c_str(), sP[ 0 ], sP[ 1 ], sP[ 2 ] ); break;
					default : pEngine->Execute( pReply, sFunction.c_str(), sP[ 0 ], sP[ 1 ], sP[ 2 ], sP[ 3 ] ); break;
				} // end switch

			} // end if

		} // end if

	} // end if

	// Property bag set
	else if ( sMsg == oexT( "pb_set" ) )
	{
		oexAutoLock ll( m_lockPb );
		if ( !ll.IsLocked() )
			return oex::oexFALSE;

		sqbind::stdString &key = mapParams[ oexT( "key" ) ];
		if ( mapParams[ oexT( "val" ) ].len() )
			m_pb.at( std2oex( key ) ) = std2oex( mapParams[ oexT( "val" ) ].str() );

		else
			m_pb.erase_at( std2oex( key ) );

	} // end if

	// Property bag timeout
	else if ( sMsg == oexT( "pb_tset" ) )
	{
		oexAutoLock ll( m_lockTo );
		if ( !ll.IsLocked() )
			return oex::oexFALSE;

		// Set the key timeout value
		m_lstKeyTimeouts[ mapParams[ oexT( "key" ) ].str() ]
			= oexGmtTime().GetUnixTime() + std2oex( mapParams[ oexT( "to" ) ].str() ).ToUInt();

	} // end if

	// Property bag get
	else if ( sMsg == oexT( "pb_get" ) )
	{
		oexAutoLock ll( m_lockPb );
		if ( !ll.IsLocked() )
			return oex::oexFALSE;

		sqbind::stdString &key = mapParams[ oexT( "key" ) ];

		oex::CPropertyBag &pb = m_pb.at( std2oex( key ) );
		if ( pb.Size() )
			*pReply = oex2std( oex::CParser::Serialize( pb ) );
		else
			*pReply = oex2std( pb.ToString() );

	} // end else if

	// Does property value exist?
	else if ( sMsg == oexT( "pb_isset" ) )
	{
		oexAutoLock ll( m_lockPb );
		if ( !ll.IsLocked() )
			return oex::oexFALSE;

		*pReply = ( m_pb.find_at( mapParams[ oexT( "key" ) ].c_str() ).IsValid() ) ? oexT( "1" ) : oexT( "" );

	} // end else if

	// Property bag array size
	else if ( sMsg == oexT( "pb_asize" ) )
	{
		oexAutoLock ll( m_lockPb );
		if ( !ll.IsLocked() )
			return oex::oexFALSE;

		oex::CPropertyBag &pb = m_pb.at( std2oex( mapParams[ oexT( "key" ) ] ) );
		*pReply = oex2std( oexMks( pb.Size() ) );
	}

	// Property bag set	array
	else if ( sMsg == oexT( "pb_aset" ) )
	{
		oexAutoLock ll( m_lockPb );
		if ( !ll.IsLocked() )
			return oex::oexFALSE;

		sqbind::stdString &key = mapParams[ oexT( "key" ) ];
		if ( mapParams[ oexT( "val" ) ].length() )
			m_pb.at( std2oex( key ) ) = oex::CParser::Deserialize( std2oex( mapParams[ oexT( "val" ) ] ) );
		else
			m_pb.erase_at( std2oex( key ) );

	} // end if

	// Property bag merge array
	else if ( sMsg == oexT( "pb_mset" ) )
	{
		oexAutoLock ll( m_lockPb );
		if ( !ll.IsLocked() )
			return oex::oexFALSE;

		oex::CParser::Deserialize( std2oex( mapParams[ oexT( "val" ) ] ),
								   m_pb.at( std2oex( mapParams[ oexT( "key" ) ] ) ),
								   oex::oexTRUE );

	} // end if

	// Property bag get
	else if ( sMsg == oexT( "pb_kget" ) )
	{
		oexAutoLock ll( m_lockPb );
		if ( !ll.IsLocked() )
			return oex::oexFALSE;

		oex::CPropertyBag &pb = m_pb.at( std2oex( mapParams[ oexT( "key" ) ] ) );
		if ( pb.Size() )
			*pReply = oex2std( oex::CParser::Implode( oex::CParser::Keys( pb ), oexT( "," ) ) );

	} // end else if

	// Property bag JSON get
	else if ( sMsg == oexT( "pb_jget" ) )
	{
		oexAutoLock ll( m_lockPb );
		if ( !ll.IsLocked() )
			return oex::oexFALSE;

		oex::CPropertyBag &pb = m_pb.at( std2oex( mapParams[ oexT( "key" ) ] ) );
		if ( pb.Size() )
			*pReply = oex2std( oex::CParser::EncodeJSON( pb ) );
		else
			*pReply = oex2std( pb.ToString() );

	} // end else if

	// Property bag set	JSON
	else if ( sMsg == oexT( "pb_jset" ) )
	{
		oexAutoLock ll( m_lockPb );
		if ( !ll.IsLocked() )
			return oex::oexFALSE;

		sqbind::stdString &key = mapParams[ oexT( "key" ) ];
		if ( mapParams[ oexT( "val" ) ].length() )
			m_pb.at( std2oex( key ) ) = oex::CParser::DecodeJSON( std2oex( mapParams[ oexT( "val" ) ] ) );
		else
			m_pb.erase_at( std2oex( key ) );

	} // end if

	// Return serialized property bag
	else if ( sMsg == oexT( "pb_all" ) )
	{
		oexAutoLock ll( m_lockPb );
		if ( !ll.IsLocked() )
			return oex::oexFALSE;

		*pReply = oex2std( oex::CParser::Serialize( m_pb ) );

	} // end else if

	// Add key to be logged
	else if ( sMsg == oexT( "pb_addlog" ) )
	{
		oexAutoLock ll( m_lockPb );
		if ( !ll.IsLocked() )
			return oex::oexFALSE;

		oex::oexINT nKey = m_log.AddKey( mapParams[ oexT( "key" ) ].c_str(),
										 oexStrToULong( mapParams[ oexT( "type" ) ].c_str() ) );

		if ( 0 <= nKey )
			m_lstLog[ mapParams[ oexT( "key" ) ].str() ] = nKey;

		*pReply = oex2std( oexMks( nKey ) );

	} // end else if

	// Remove key from logging
	else if ( sMsg == oexT( "pb_removelog" ) )
	{
		oexAutoLock ll( m_lockPb );
		if ( !ll.IsLocked() )
			return oex::oexFALSE;

		oex::oexINT nKey = m_log.FindKey( mapParams[ oexT( "key" ) ].c_str() );
		if ( 0 <= nKey )
			m_log.RemoveKey( nKey );

		m_lstLog.erase( mapParams[ oexT( "key" ) ] );

		*pReply = oex2std( oexMks( nKey ) );

	} // end else if

	// pb_setlogroot
	else if ( sMsg == oexT( "pb_setlogroot" ) )
	{
		oexAutoLock ll( m_lockPb );
		if ( !ll.IsLocked() )
			return oex::oexFALSE;

		*pReply = oex2std( oexMks( m_log.SetRoot( mapParams[ oexT( "root" ) ].c_str() ) ) );

	} // end else if

	// pb_setlogroot
	else if ( sMsg == oexT( "pb_resetlog" ) )
	{
		oexAutoLock ll( m_lockPb );
		if ( !ll.IsLocked() )
			return oex::oexFALSE;

		m_log.Destroy();
		m_lstLog.clear();

	} // end else if

	// pb_getlogroot
	else if ( sMsg == oexT( "pb_getlogroot" ) )
	{
		oexAutoLock ll( m_lockPb );
		if ( !ll.IsLocked() )
			return oex::oexFALSE;

		*pReply = oex2std( m_log.GetRoot() );

	} // end else if

	// pb_setlogfreq
	else if ( sMsg == oexT( "pb_setlogfreq" ) )
	{
		oexAutoLock ll( m_lockPb );
		if ( !ll.IsLocked() )
			return oex::oexFALSE;

		// Set logging frequency
		m_uLogFreq = oexStrToULong( mapParams[ oexT( "freq" ) ].c_str() );
		*pReply = oex2std( oexMks( m_uLogFreq ) );

	} // end else if

	// pb_getlogfreq
	else if ( sMsg == oexT( "pb_getlogfreq" ) )
	{
		oexAutoLock ll( m_lockPb );
		if ( !ll.IsLocked() )
			return oex::oexFALSE;

		// Set logging frequency
		*pReply = oex2std( oexMks( m_uLogFreq ) );

	} // end else if

	// pb_getlogkeys
	else if ( sMsg == oexT( "pb_getlogkeys" ) )
	{
		oexAutoLock ll( m_lockPb );
		if ( !ll.IsLocked() )
			return oex::oexFALSE;

		// Get log keys
		oex::CPropertyBag pb = m_log.GetKeyList( oexStrToULong( mapParams[ oexT( "time" ) ].c_str() ) );

		// Return list of keys being logged
		*pReply = oex2std( oex::CParser::Serialize( pb ) );

	} // end else if

	// pb_getlog
	else if ( sMsg == oexT( "pb_getlog" ) )
	{
		oexAutoLock ll( m_lockPb );
		if ( !ll.IsLocked() )
			return oex::oexFALSE;

		// Write any pending data to the disk
		m_log.Flush();

		// Grab log
		oex::CPropertyBag pb
			= m_log.GetLog( mapParams[ oexT( "key" ) ].c_str(),
							oexStrToULong( mapParams[ oexT( "start" ) ].c_str() ),
							oexStrToULong( mapParams[ oexT( "stop" ) ].c_str() ),
							oexStrToULong( mapParams[ oexT( "interval" ) ].c_str() ),
							oexStrToULong( mapParams[ oexT( "type" ) ].c_str() ),
							oexStrToULong( mapParams[ oexT( "method" ) ].c_str() )
						  );

		// Return list of keys being logged
		*pReply = oex2std( oex::CParser::Serialize( pb ) );

	} // end else if

	// pb_getlog
	else if ( sMsg == oexT( "pb_getlogbin" ) )
	{
		oexAutoLock ll( m_lockPb );
		if ( !ll.IsLocked() )
			return oex::oexFALSE;

		// Write any pending data to the disk
		m_log.Flush();

		// Extract extra params
		oex::CPropertyBag ext = oex::CParser::Deserialize( mapParams[ oexT( "ext" ) ].c_str() );

		// Return list of keys being logged
		*pReply =
			oex2std( m_log.GetLogBin( mapParams[ oexT( "key" ) ].c_str(),
									  oexStrToULong( mapParams[ oexT( "start" ) ].c_str() ),
									  oexStrToULong( mapParams[ oexT( "stop" ) ].c_str() ),
									  oexStrToULong( mapParams[ oexT( "interval" ) ].c_str() ),
									  ext[ oexT( "type" ) ].ToInt(),
									  ext[ oexT( "method" ) ].ToInt(),
									  ext[ oexT( "scale" ) ].ToFloat()
									) );

	} // end else if

	// pb_getlog
	else if ( sMsg == oexT( "pb_logbin" ) )
	{
		oexAutoLock ll( m_lockPb );
		if ( !ll.IsLocked() )
			return oex::oexFALSE;

		// Log the data
		m_log.LogBin( mapParams[ oexT( "key" ) ].c_str(),
					  mapParams[ oexT( "bin" ) ].c_str(),
					  oexStrToULong( mapParams[ oexT( "type" ) ].c_str() ),
					  oexStrToULong( mapParams[ oexT( "time" ) ].c_str() ),
					  oexStrToULong( mapParams[ oexT( "timems" ) ].c_str() ),
					  oexStrToULong( mapParams[ oexT( "buffering" ) ].c_str() )
					);


	} // end else if

	// Our own thread must handle the rest
	else if ( GetOwnerThreadId() != oexGetCurThreadId() )
		return Msg( stdString(), sMsg, &mapParams, pReply, pReplyEvent );

	else if ( sMsg == oexT( "spawn" ) )
		OnSpawn( mapParams, pReply );

	// Call to send message to child script
	else if ( sMsg == oexT( "msg" ) )
		OnMsg( mapParams, pReply );

	// Return comma separated list of child scripts command
	else if ( sMsg == oexT( "get_children" ) )
	{	if ( oexCHECK_PTR( pReply ) )
		{	oex::TList< oex::CStr > strlst;
			for (	t_ScriptList::iterator it = m_lstScript.begin();
					m_lstScript.end() != it; it++ )
				if ( oexCHECK_PTR( it->second ) && it->second->IsRunning() && !it->second->WantQuit() )
					strlst << oex::CParser::UrlEncode( std2oex( it->second->GetName() ) );
			*pReply = oex2std( oex::CParser::Implode( strlst, oexT( "," ) ) );
		} // end if
	} // end else if

	// Quit command
	else if ( sMsg == oexT( "kill" ) )
		RequestQuit();

	// Set timer command
	else if ( sMsg == oexT( "set_timer" ) )
		*pReply = oex2std( oexMks( SetTimer( std2oex( mapParams[ oexT( "to" ) ] ).ToULong(),
						  					 mapParams[ oexT( "cb" ) ] ) ) );

	// Set time out command
	else if ( sMsg == oexT( "set_timeout" ) )
		*pReply = oex2std( oexMks( SetTimeout( std2oex( mapParams[ oexT( "to" ) ] ).ToULong(),
						  			  		   mapParams[ oexT( "cb" ) ] ) ) );

	// Set time out command
	else if ( sMsg == oexT( "kill_timer" ) )
		KillTimer( std2oex( mapParams[ oexT( "id" ) ] ).ToULong() );

	else
		bRet = oex::oexFALSE;

	// Signal message has been processed
	if ( pReplyEvent )
		pReplyEvent->Signal();

	return bRet;
}

oex::oexBOOL CScriptThread::ProcessMsg( const stdString &x_sPath, stdString &sMsg, CSqMulti &mapParams, stdString *pReply, oexEvent *pReplyEvent )
{_STT();
	stdString sPath = x_sPath;

//	if ( sMsg == oexT( "kill" ) )
//		oexPrintf( oexT( "ProcessMsg(): %s, To: %s, Caller: 0x%08x, Owner: 0x%08x\n" ), sMsg.c_str(), x_sPath.c_str(), (unsigned int)oexGetCurThreadId(), GetOwnerThreadId() );

	// Is it bound for another computer
	stdString::size_type pos = sPath.find_first_of( oexT( ":" ), (size_t)-1 );
	if ( stdString::npos != pos )
	{
		oexPrintf( "Remote routing not supported\n" );

		stdString sAddress = sPath.substr( 0, pos );
		if ( sAddress.length() )
		{
			// +++ Route to remote computer
			oexASSERT( 0 );

			return oex::oexFALSE;

		} // end if

	} // end if

	// Is it for us?
    if ( !sPath.length()
         || ( sPath[ 0 ] == oexT( '.' ) && 1 == sPath.length() ) // sPath == oexT( "." ) - !!! SIGSEGV in __strlen_sse2()
         || sPath == m_sName )
		return ExecuteMsg( sMsg, mapParams, pReply, pReplyEvent );

	// All the way to the top?
	else if ( sPath[ 0 ] == oexT( '/' ) || sPath[ 0 ] == oexT( '\\' ) )
	{
		// Route to parent if any
		if ( m_pParentScript )
//			return m_pParentScript->Msg( sPath, sMsg, &mapParams, pReply );
			return m_pParentScript->ProcessMsg( sPath, sMsg, mapParams, pReply, pReplyEvent );

		// Shave off parent sep
		while ( sPath.length()
				&& ( sPath[ 0 ] == oexT( '/' )
					 || sPath[ 0 ] == oexT( '\\' )
					 || sPath[ 0 ] == oexT( '.' ) 
					) )
			sPath = sPath.substr( 1 );

		// Is it ours?
		if ( !sPath.length() )
			return ExecuteMsg( sMsg, mapParams, pReply, pReplyEvent );

	} // end if

	// Find path separator
	stdString sToken;
	stdString::size_type nPos = sPath.find_first_of( oexT( '\\' ) );
	if ( stdString::npos == nPos ) nPos = sPath.find_first_of( oexT( '/' ) );
	if ( stdString::npos != nPos )
	{	sToken = sPath.substr( 0, nPos );
		sPath = sPath.substr( nPos + 1 );
	} // end if
	else
	{   sToken = sPath;
		sPath = oexT( "" );
	} // end if

	// Is it going up to the parent?
	if ( sToken == oexT( ".." ) )
	{
		if ( !m_pParentScript )
			return oex::oexFALSE;

//		return m_pParentScript->Msg( sPath, sMsg, &mapParams, pReply );
		return m_pParentScript->ProcessMsg( sPath, sMsg, mapParams, pReply, pReplyEvent );

	} // end if

	// Route to child
	t_ScriptList::iterator it = m_lstScript.find( sToken );
	if ( m_lstScript.end() != it && it->second )
//		return it->second->Msg( sPath, sMsg, &mapParams, pReply );
		return it->second->ProcessMsg( sPath, sMsg, mapParams, pReply, pReplyEvent );

	return oex::oexFALSE;
}

void CScriptThread::Cleanup()
{_STT();

	// For all child scripts
	t_ScriptList::iterator it = m_lstScript.begin();
	while ( m_lstScript.end() != it )
	{
		// Delete node if child has stopped
		if ( it->second && !it->second->IsRunning() )
		{	it->second->Stop();
			OexAllocDestruct( it->second );
			it->second = oexNULL;
		} // end if

		if ( !it->second )
		{	t_ScriptList::iterator nx = it; nx++;
			m_lstScript.erase( it );
			it = nx;
		} // end if

		else
			it++;

		// ???
//			it = m_lstScript.erase( it );

	} // end while

	// Any time limited keys?
	if ( m_lstKeyTimeouts.size() )
	{
		// Check key timeouts
		oexAutoLock ll( m_lockTo );
		if ( ll.IsLocked() )
		{	oex::oexUINT gmt = oexGmtTime().GetUnixTime();
			t_TimeoutList::iterator it = m_lstKeyTimeouts.begin();
			while ( m_lstKeyTimeouts.end() != it )
			{
				// Has this key timed out?
				if ( it->second < gmt )
				{
					oexAutoLock ll( m_lockPb );
					if ( ll.IsLocked() )
					{	m_pb.erase_at( std2oex( it->first ) );
						m_lstKeyTimeouts.erase( it++ );
					} // end if

				} // end if

				else
					it++;

			} // end while

		} // end if

	} // end if

}

void CScriptThread::DestroyChildScripts()
{_STT();
	// Kill our script
	t_ScriptList::iterator it = m_lstScript.begin();
	while ( m_lstScript.end() != it )
	{
		if ( it->second )
		{	it->second->Stop();
			OexAllocDestruct( it->second );
			it->second = oexNULL;
		} // end if

		t_ScriptList::iterator nx = it; nx++;
		m_lstScript.erase( it );
		it = nx;

		// ???
//			it = m_lstScript.erase( it );

	} // end while
}

void CScriptThread::OnSpawn( CSqMulti &mapParams, stdString *pReply )
{_STT();
	// Grab the path
	stdString sName = mapParams[ oexT( "name" ) ];
	stdString sScriptName = mapParams[ oexT( "script_name" ) ];
	if ( !sScriptName.length() )
		sScriptName = sName;

	// Lose current script engine at this tag if any
	t_ScriptList::iterator it = m_lstScript.find( sName );
	if ( m_lstScript.end() != it )
	{	if ( it->second )
		{	OexAllocDestruct( it->second );
			it->second = oexNULL;
		} // end if
		m_lstScript.erase( it );
	} // end if

	// Create new object
	CScriptThread *pSt = OexAllocConstruct< CScriptThread >();
	if ( pSt )
	{
		// Save away pointer for later
		m_lstScript[ sName ] = pSt;

		// Set include function
		pSt->SetIncludeScriptFunction( m_cSqEngine.GetIncludeScriptFunction() );

		// Let the script know it's name
		pSt->SetName( sName );
		pSt->SetScriptName( sScriptName );

		// Set us as the parent
		pSt->SetParentScript( this );

		// Allow child to have access to the module manager
		pSt->SetModuleManager( m_pModuleManager );

		// Import environment
		pSt->SetExportFunction( m_cSqEngine.GetExportFunction(), m_cSqEngine.GetAllocator() );

		// Is it a file?
		if ( 0 != std2oex( mapParams[ oexT( "file" ) ] ).ToULong() )
		{
			// Does the path specified exist?
			if ( oexExists( mapParams[ oexT( "script" ) ].c_str() ) )
				pSt->SetScript( std2oex8( mapParams[ oexT( "script" ) ] ), 1 );

			// Custom include engine?
			else if ( m_cSqEngine.GetIncludeScriptFunction() )
			{
				stdString sData, sName;
				if ( m_cSqEngine.GetIncludeScriptFunction()( mapParams[ oexT( "script" ) ], sData, sName ) )
					*pReply = oexT( "FAILED" );
				else
					pSt->SetScript( std2oex8( sData ), 0 );

				// Set the script name
				pSt->SetScriptName( sName );

			} // end if

			else
				pSt->SetScript( std2oex8( m_cSqEngine.path( mapParams[ oexT( "script" ) ] ) ), 1 );

		} // end if

		else
		{
			// Same root as us then
			pSt->SetRoot( GetRoot() );

			// Load script information
			pSt->SetScript( std2oex8( mapParams[ oexT( "script" ) ] ), 0 );

		} // end else

		// Create the thread
		pSt->Start();

		// Wait for thread to start if reply buffer
		if ( pReply )
			if ( pSt->GetInitEvent().Wait() )
				*pReply = oexT( "FAILED" );

	} // end if
}

void CScriptThread::OnMsg( CSqMulti &mapParams, stdString *pReply )
{_STT();

	int ret = 0;

	// Run script?
	if ( mapParams[ oexT( "run" ) ].length() )
		m_cSqEngine.Run( pReply, mapParams[ oexT( "name" ) ].c_str(), mapParams[ oexT( "run" ) ].c_str() );

	// shell
	else if ( mapParams[ oexT( "shell" ) ].length() )
	{	ret = m_cSqEngine.shell( mapParams[ oexT( "shell" ) ].c_str(), 
								 mapParams[ oexT( "params" ) ].c_str(),
								 mapParams[ oexT( "dir" ) ].c_str() );
		if ( pReply ) 
			*pReply = oex2std( oexMks( ret ) );
	} // end if

	// exec
	else if ( mapParams[ oexT( "exec" ) ].length() )
	{	ret = m_cSqEngine.exec( mapParams[ oexT( "exec" ) ].c_str(), 
								mapParams[ oexT( "params" ) ].c_str(),
								mapParams[ oexT( "dir" ) ].c_str() );
		if ( pReply ) 
			*pReply = oex2std( oexMks( ret ) );
	} // end if

	// sqexe
	else if ( mapParams[ oexT( "sqexe" ) ].length() )
	{	ret = m_cSqEngine.sqexe( mapParams[ oexT( "sqexe" ) ].c_str(), 
								 mapParams[ oexT( "dir" ) ].c_str() );
		if ( pReply ) 
			*pReply = oex2std( oexMks( ret ) );
	} // end if
		
	// sqexe_script
	else if ( mapParams[ oexT( "sqexe_script" ) ].length() )
	{	ret = m_cSqEngine.sqexe_script( mapParams[ oexT( "sqexe_script" ) ].c_str(), 
										mapParams[ oexT( "params" ) ].c_str(),
										mapParams[ oexT( "dir" ) ].c_str() );
		if ( pReply ) 
			*pReply = oex2std( oexMks( ret ) );
	} // end if
		
	// Execute function?
	else if ( mapParams.isset( oexT( "execute" ) ) )
		m_cSqEngine.Execute( pReply, mapParams[ oexT( "execute" ) ].c_str() );

	// Execute one param
	else if ( mapParams.isset( oexT( "execute1" ) ) )
		m_cSqEngine.Execute( pReply, mapParams[ oexT( "execute1" ) ].c_str(), mapParams[ oexT( "p1" ) ].str() );

	// Execute 2 params
	else if ( mapParams.isset( oexT( "execute2" ) ) )
		m_cSqEngine.Execute( pReply, mapParams[ oexT( "execute2" ) ].c_str(), mapParams[ oexT( "p1" ) ].str(), mapParams[ oexT( "p2" ) ].str() );

	// Execute 3 params
	else if ( mapParams.isset( oexT( "execute3" ) ) )
		m_cSqEngine.Execute( pReply, mapParams[ oexT( "execute3" ) ].c_str(), mapParams[ oexT( "p1" ) ].str(), mapParams[ oexT( "p2" ) ].str(), mapParams[ oexT( "p3" ) ].str() );

	// Execute 4 params
	else if ( mapParams.isset( oexT( "execute4" ) ) )
		m_cSqEngine.Execute( pReply, mapParams[ oexT( "execute4" ) ].c_str(), mapParams[ oexT( "p1" ) ].str(), mapParams[ oexT( "p2" ) ].str(), mapParams[ oexT( "p3" ) ].str(), mapParams[ oexT( "p4" ) ].str() );
}

CSqMsgQueue* CScriptThread::GetQueue( const stdString &x_sPath )
{_STT();
	stdString sPath = x_sPath;

	// Us?
	if ( !sPath.length() || sPath == oexT( "." ) || sPath == m_sName )
		return this;

	// Can't get remote objects
	int pos = sPath.find_first_of( oexT( ":" ), (size_t)-1 );
	if ( 0 <= pos )
		return oexNULL;

	// All the way to the top?
	else if ( sPath[ 0 ] == oexT( '/' ) || sPath[ 0 ] == oexT( '\\' ) )
	{
		// Route to parent if any
		if ( m_pParentScript )
			return m_pParentScript->GetQueue( sPath );

		// Shave off parent sep
		while ( sPath[ 0 ] == oexT( '/' )
				|| sPath[ 0 ] == oexT( '\\' )
				|| sPath[ 0 ] == oexT( '.' ) )
			sPath = sPath.substr( 1 );

		// Is it ours?
		if ( !sPath.length() )
			return this;

	} // end if

	// Find path separator
	stdString sToken;
	stdString::size_type nPos = sPath.find_first_of( oexT( '\\' ) );
	if ( stdString::npos == nPos ) nPos = sPath.find_first_of( oexT( '/' ) );
	if ( stdString::npos != nPos )
	{	sToken = sPath.substr( 0, nPos );
		sPath = sPath.substr( nPos + 1 );
	} // end if
	else
	{   sToken = sPath;
		sPath = oexT( "" );
	} // end if

	// Is it going up to the parent?
	if ( sToken == oexT( ".." ) )
	{
		if ( !m_pParentScript )
			return oex::oexFALSE;

		return m_pParentScript->GetQueue( sPath );

	} // end if

	// Route to child
	t_ScriptList::iterator it = m_lstScript.find( sToken );
	if ( m_lstScript.end() != it && it->second )
		return it->second->GetQueue( sPath );

	return oexNULL;
}

