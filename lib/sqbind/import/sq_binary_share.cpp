/*------------------------------------------------------------------
// sq_binary_share.cpp
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

#include "../stdafx.h"

using namespace sqbind;

// Key values
#define SQSBS_PREFIX		oexT( "oex_binary_share_" )
#define SQSBS_CBID			0xBEBAADBA
#define SQSBS_PADDING		64
#define SQSBS_HEADERSIZE	( 18 * 4 )

SQBIND_REGISTER_CLASS_BEGIN( sqbind::CSqBinaryShare, CSqBinaryShare )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinaryShare, Destroy )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinaryShare, Create )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinaryShare, Open )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinaryShare, getName )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinaryShare, setPrefix )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinaryShare, getPrefix )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinaryShare, setCbId )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinaryShare, getCbId )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinaryShare, setGlobal )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinaryShare, getGlobal )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinaryShare, getFps )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinaryShare, getFmt )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinaryShare, getReads )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinaryShare, getWrites )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinaryShare, getTs )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinaryShare, getBufSize )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinaryShare, getChannels )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinaryShare, getDataWidth )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinaryShare, getLastErrorStr )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinaryShare, setLastErrorStr )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinaryShare, isOpen )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinaryShare, setPadding )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinaryShare, getPadding )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinaryShare, getHeaderSize )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinaryShare, getBufferGuid )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinaryShare, getBufferGuidStr )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinaryShare, getReset )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinaryShare, setReset )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinaryShare, Read )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinaryShare, getMaxRead )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinaryShare, Write )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinaryShare, WriteFloat )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinaryShare, WriteDouble )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinaryShare, WriteChar )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinaryShare, WriteShort )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinaryShare, WriteInt )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinaryShare, WriteInt64 )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinaryShare, Reset )
//	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinaryShare,  )
//	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinaryShare,  )

SQBIND_REGISTER_CLASS_END()

void CSqBinaryShare::Register( sqbind::VM vm )
{_STT();
	SQBIND_EXPORT( vm, CSqBinaryShare );
}

CSqBinaryShare::CSqBinaryShare()
{
	m_sPrefix = SQSBS_PREFIX;
	m_uCbId = SQSBS_CBID;
	m_iRead = -1;
	m_bGlobal = 1;
	m_bWrite = 0;
	m_nPadding = SQSBS_PADDING;
	m_uTs = 0;
}

CSqBinaryShare::CSqBinaryShare( const sqbind::stdString &sPrefix, SQInteger nId, SQInteger nGlobal )
{
	m_sPrefix = sPrefix;
	m_uCbId = (oex::oexUINT)nId;
	m_bGlobal = nGlobal;
	m_iRead = -1;
	m_bWrite = 0;
	m_nPadding = SQSBS_PADDING;
	m_uTs = 0;
}


CSqBinaryShare::CSqBinaryShare( const CSqBinaryShare &r ) 
{
	m_sPrefix = r.m_sPrefix;
	m_uCbId = r.m_uCbId;
	m_bGlobal = r.m_bGlobal;
	m_iRead = -1;
	m_bWrite = 0;
	m_nPadding = SQSBS_PADDING;
	m_uTs = 0;
}


void CSqBinaryShare::Destroy()
{_STT();

	// Indicate that we have detached
	if ( m_bWrite )
	{	m_cb.setUINT( 0, 0 );
		m_cb.setUINT( 11, 0 );
	} // end if

	m_sName = oexT( "" );

	m_cb.Free();
	m_buf.Free();

	m_bWrite = 0;
	m_uTs = 0;

	m_iRead = -1;
}

int CSqBinaryShare::Create( const sqbind::stdString &sName, const sqbind::stdString &sBufName, int nBufSize, int nChannels, int nDataWidth, int nFps, int nFmt )
{_STT();

	// Lose the old share
	Destroy();

	// Unique name?
	if ( sName.length() )
		m_sName = sName;
	else
		m_sName = oex2std( oexUnique() );

	// Are we sharing a buffer?
	int bShareBuffer = 0;
	if ( sBufName.length() && sName != sBufName )
		bShareBuffer = 1;

	// Structure size
	int struct_size = SQSBS_HEADERSIZE;

	// Control block share name
	oex::oexGUID guidName;
	oex::CStr sgName = oexGuid( std2oex( sqbind::stdString( m_sPrefix ) + m_sName ) );
	sqbind::stdString sidName = ( m_bGlobal ? oexT( "Global\\" ) : oexT( "" ) ) + oex2std( sgName ); 
	sgName.StringToGuid( guidName );

	// Buffer share name
	oex::oexGUID guidBuffer;
	oex::CStr sgBuffer = oexGuid( std2oex( sqbind::stdString( m_sPrefix ) + oexT( "buffer_" ) + ( bShareBuffer ? sBufName : m_sName ) ) );
	sqbind::stdString sidBuffer = ( m_bGlobal ? oexT( "Global\\" ) : oexT( "" ) ) + oex2std( sgBuffer ); 
	sgBuffer.StringToGuid( guidBuffer );

	// Set share info
	m_cb.SetName( sidName ); 
	m_cb.PlainShare( 1 );
	
	// Attempt to allocate
	if ( !m_cb.Allocate( struct_size ) )
	{	m_sLastErr = oex2std( oexMks( oexT( "Failed to allocate control block " ), struct_size, oexT( " bytes : " ), std2oex( m_sName ), oexT( " : " ), std2oex( sidName ) ) );
		return 0;
	} // end if

	// Does it already exist
	if ( m_cb.Existing() )
	{
		// Is it still in use?
		if ( m_cb.getUINT( 0 ) )
		{	m_sLastErr = oex2std( oexMks( oexT( "Share control block already exists and is in use : " ), std2oex( m_sName ), oexT( " : " ), std2oex( sidName ) ) );
			Destroy();
			return 0;
		} // end if
		
		// We can only reuse it if the params haven't changed or are not initialized
		if ( ( m_cb.getINT( 2 ) && m_cb.getINT( 2 ) != nBufSize ) 
		     || ( m_cb.getINT( 3 ) && m_cb.getINT( 3 ) != nChannels ) 
		     || ( m_cb.getINT( 4 ) && m_cb.getINT( 4 ) != nDataWidth ) 
			 || ( m_cb.getINT( 5 ) && m_cb.getINT( 5 ) != nFps )
		     || ( m_cb.getINT( 6 ) && m_cb.getINT( 6 ) != nFmt ) )
		{	m_sLastErr = oex2std( oexMks( oexT( "Share control block already exists, and has changed : " ), std2oex( m_sName ), oexT( " : " ), std2oex( sidName ) ) );
			Destroy();
			return 0;
		} // end if

	} // end if

	// Set control block size
	m_cb.setUsed( struct_size );

	// Initialize control block
	m_cb.setINT(	1, -1 );
	m_cb.setINT(	2, nBufSize );
	m_cb.setINT(	3, nChannels );
	m_cb.setINT(	4, nDataWidth );
	m_cb.setINT(	5, nFps );
	m_cb.setINT(	6, nFmt );
	m_cb.setINT(	7, 0 );
	m_cb.setINT(	8, 0 );
	m_cb.setINT(	9, 0 );
	m_cb.setUINT(	10, oexPtrToInt( oex::os::CSys::GetInstanceHandle() ) );
	m_cb.setUINT(	11, ( m_uTs = oexGetUnixTime() ) );
	m_cb.setUINT(	12, SQSBS_HEADERSIZE );
	m_cb.setUINT(	13, 0 );
	m_cb.setUINT(	14, guidBuffer.u1 );
	m_cb.setUINT(	15, guidBuffer.u2 );
	m_cb.setUINT(	16, guidBuffer.u3 );
	m_cb.setUINT(	17, guidBuffer.u4 );

	// Set buffer info
	m_buf.SetName( sidBuffer );
	m_buf.PlainShare( 1 );
	
	// Attempt to allocate the buffer
	int size = nBufSize + m_nPadding;
	if ( !m_buf.Allocate( size ) )
	{	m_sLastErr = oex2std( oexMks( oexT( "Failed to allocate buffer of " ), size, oexT( " bytes : " ), std2oex( ( bShareBuffer ? sBufName : sName ) ), oexT( " : " ), std2oex( sidBuffer ) ) );
		Destroy();
		return 0;
	} // end if

	// If it is shared, it must exist
	if ( bShareBuffer && !m_buf.Existing() )
	{	m_sLastErr = oex2std( oexMks( oexT( "Share buffer doesn't exist : " ), std2oex( sBufName ), oexT( " : " ), std2oex( sidBuffer ) ) );
		Destroy();
		return 0;
	} // end if

	// Set size of buffer
	m_buf.setUsed( nBufSize );
	
	// Ok for others to read now
	m_cb.setUINT( 0, m_uCbId );

	// Share is writable
	m_bWrite = 1;	

	return 1;
}

int CSqBinaryShare::Open( const sqbind::stdString &sName )
{_STT();

	// Must have a valid name
	if ( !sName.length() )
		return 0;

	// Structure size
	int struct_size = SQSBS_HEADERSIZE;

	// Control block share name
	oex::oexGUID guidName;
	oex::CStr sgName = oexGuid( std2oex( sqbind::stdString( m_sPrefix ) + sName ) );
	sqbind::stdString sidName = ( m_bGlobal ? oexT( "Global\\" ) : oexT( "" ) ) + oex2std( sgName ); 
	sgName.StringToGuid( guidName );

	// Set share info
	m_cb.SetName( sidName ); 
	m_cb.PlainShare( 1 );

	// Attempt to open existing share
	if ( !m_cb.Allocate( struct_size ) )
	{	m_sLastErr = oex2std( oexMks( oexT( "Failed to allocate control block of " ), struct_size, oexT( " bytes : " ), std2oex( sName ) ) );
		Destroy();
		return 0;
	} // end if
	
	if ( !m_cb.Existing() )
	{	m_sLastErr = oex2std( oexMks( oexT( "Share control block does not exist : " ), std2oex( sName ) ) );
		Destroy();
		return 0;
	} // end if
	
	// Yay!  Set the share size.
	m_cb.setUsed( struct_size );

	// Verify control block id
	if ( m_cb.getUINT( 0 ) != m_uCbId )
	{	Destroy();
		return 0;
	} // end if

	/// Get buffer params
	int nFps = getFps();
	int nBufSize = getBufSize();
	
	// Validate parameters
	if ( 0 >= nFps || 0 >= nBufSize )
	{	Destroy();
		return 0;
	} // end if

	// Data buffer share name
	oex::oexGUID guidBuffer;
	oex::CStr sgBuffer;
	if ( getHeaderSize() >= SQSBS_HEADERSIZE )
		sgBuffer = std2oex( getBufferGuidStr() );
	else
		sgBuffer = oexGuid( std2oex( sqbind::stdString( m_sPrefix ) + oexT( "buffer_" ) + sName ) );
	sqbind::stdString sidBuffer = ( m_bGlobal ? oexT( "Global\\" ) : oexT( "" ) ) + oex2std( sgBuffer ); 
	sgBuffer.StringToGuid( guidBuffer );
	
	// Set buffer info
	m_buf.SetName( sidBuffer );
	m_buf.PlainShare( 1 );
	
	// Attempt to allocate the buffer
	int size = nBufSize + m_nPadding;
	if ( !m_buf.Allocate( size ) )
	{	m_sLastErr = oex2std( oexMks( oexT( "Failed to allocate buffer of " ), size, oexT( " bytes : " ), std2oex( sName ), oexT( " : " ), std2oex( sidBuffer ) ) );
		Destroy();
		return 0;
	} // end if

	// Punt if it doesn't exist
	if ( !m_buf.Existing() )
	{	m_sLastErr = oex2std( oexMks( oexT( "Share buffer does not exist : " ), std2oex( sName ), oexT( " : " ), std2oex( sidBuffer ) ) );
		Destroy();
		return 0;
	} // end if

	// Set size of buffer
	m_buf.setUsed( nBufSize );

	// Save share information
	m_sName = sName;
	m_uTs = getTs();

	return 1;
}

sqbind::CSqBinary CSqBinaryShare::Read()
{_STT();

	if ( !m_cb.getUsed() || !m_buf.getUsed() )
	{	m_sLastErr = oexT( "No open share" );
		return sqbind::CSqBinary();
	} // end if

	// Verify that writer is still connected
	if ( m_cb.getUINT( 0 ) != m_uCbId || m_cb.getUINT( 11 ) != m_uTs )
	{	m_sLastErr = oex2std( oexMks( oexT( "Writer has disconnected : " ), std2oex( m_sName ) ) );
		Destroy(); 
		return sqbind::CSqBinary(); 
	} // end if
	
	// Get the current buffer pointer, and make sure it's valid
	int i = m_cb.getINT( 1 );
	int nBufSize = m_cb.getINT( 2 );

	// Initialize our read pointer
	if ( 0 > m_iRead || m_iRead > nBufSize ) 
		m_iRead = i;

	// Any data?
	if ( 0 > i || i == m_iRead || i >= nBufSize )
		return sqbind::CSqBinary(); 

	// Find a new block of data
	int s = 0, e = 0;

	// End of buffer?
	if ( m_iRead < i )
		s = m_iRead, e = i, m_iRead = i;

	else
	{
		// Have we wrapped?
		if ( m_iRead < nBufSize )
			s = m_iRead, e = nBufSize, m_iRead = nBufSize;

		else
			s = 0, e = i, m_iRead = i;

	} // end else

	// Any data?
	if ( s >= e )
		return sqbind::CSqBinary(); 

	// Count a read
	m_cb.setUINT( 9, m_cb.getUINT( 9 ) + 1 );

	return m_buf.getSub( s, e - s );
}

int CSqBinaryShare::getMaxRead()
{
	if ( !m_cb.getUsed() || !m_buf.getUsed() )
	{	m_sLastErr = oexT( "No open share" );
		return 0;
	} // end if

	// Verify that writer is still connected
	if ( m_cb.getUINT( 0 ) != m_uCbId || m_cb.getUINT( 11 ) != m_uTs )
	{	m_sLastErr = oex2std( oexMks( oexT( "Writer has disconnected : " ), std2oex( m_sName ) ) );
		Destroy(); 
		return 0; 
	} // end if

	int i = m_cb.getINT( 1 );
	int nBufSize = m_cb.getINT( 2 );

	// Any data in the buffer?
	if ( 0 > i || i == m_iRead || i >= nBufSize )
		return 0;

	// How many bytes in the buffer?
	return ( i > m_iRead ) ? ( i - m_iRead ) : ( nBufSize - ( m_iRead - i ) );
}

int CSqBinaryShare::Write( sqbind::CSqBinary *pData )
{
	if ( !pData )
	{	m_sLastErr = oexT( "Invalid parameter" );
		return 0;
	} // end if

	return WritePtr( pData->Ptr(), pData->getUsed() );
}

int CSqBinaryShare::WritePtr( const void *pData, int nSize )
{
	if ( !m_cb.getUsed() || !m_buf.getUsed() )
	{	m_sLastErr = oexT( "No open share" );
		return 0;
	} // end if

	if ( !pData )
	{	m_sLastErr = oexT( "Invalid parameter" );
		return 0;
	} // end if

	// Verify that writer is still connected
	if ( m_cb.getUINT( 0 ) != m_uCbId || m_cb.getUINT( 11 ) != m_uTs )
	{	m_sLastErr = oex2std( oexMks( oexT( "Writer has disconnected : " ), std2oex( m_sName ) ) );
		Destroy(); 
		return 0; 
	} // end if

	if ( 0 >= nSize )
		return 0;

	// Get the current buffer pointer, and make sure it's valid
	int i = m_cb.getINT( 1 );
	int nBufSize = m_cb.getINT( 2 );
	int l = nBufSize - i;
	int w = nSize;

	// Is the buffer large enough?
	if ( w >= nBufSize )
		return 0;

	// Write what we can to the end of the buffer
	if ( l )
	{	if ( l >= w ) m_buf.Mem().MemCpyAt( (oex::CBin::t_byte*)pData, i, w ), i += w, w = 0;
		else m_buf.Mem().MemCpyAt( (oex::CBin::t_byte*)pData, i, l ), i = 0, w -= l;
	} // end if

	// Loop back to start if needed
	if ( w )
		m_buf.Mem().MemCpy( &((oex::CBin::t_byte*)pData)[ l ], w ), i += w;

	// Count a frame written
	m_cb.setUINT( 8, m_cb.getUINT( 8 ) + 1 );

	// Loop pointer
	if ( i >= nBufSize )
		i = 0;

	// Update write pointer
	m_cb.setINT( 1, i );

	return 1;
}

int CSqBinaryShare::Reset() 
{
	if ( !m_cb.getUsed() || !m_buf.getUsed() )
	{	m_sLastErr = oexT( "No open share" );
		return 0;
	} // end if

	// Verify that writer is still connected
	if ( m_cb.getUINT( 0 ) != m_uCbId || m_cb.getUINT( 11 ) != m_uTs )
	{	m_sLastErr = oex2std( oexMks( oexT( "Writer has disconnected : " ), std2oex( m_sName ) ) );
		Destroy(); 
		return 0; 
	} // end if

	// Reset the read pointer
	m_iRead = m_cb.getINT( 1 );
	int nBufSize = m_cb.getINT( 2 );
	if ( 0 > m_iRead || m_iRead > nBufSize ) 
		m_iRead = -1;
		
	return 1;
}


