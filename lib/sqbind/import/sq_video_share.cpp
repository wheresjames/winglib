/*------------------------------------------------------------------
// sq_video_share.cpp
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
#define SQSVS_PREFIX		oexT( "oex_video_share_" )
#define SQSVS_CBID			0xBEBADDBA
#define SQSVS_PADDING		64
#define SQSVS_HEADERSIZE	( 20 * 4 )

SQBIND_REGISTER_CLASS_BEGIN( sqbind::CSqVideoShare, CSqVideoShare )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqVideoShare, Destroy )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqVideoShare, Create )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqVideoShare, Open )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqVideoShare, getName )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqVideoShare, setPrefix )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqVideoShare, getPrefix )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqVideoShare, setCbId )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqVideoShare, getCbId )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqVideoShare, setGlobal )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqVideoShare, getGlobal )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqVideoShare, getWritePtr )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqVideoShare, getBuffers )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqVideoShare, getWidth )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqVideoShare, getHeight )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqVideoShare, getFps )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqVideoShare, getFmt )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqVideoShare, getReads )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqVideoShare, getWrites )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqVideoShare, getTs )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqVideoShare, getImageSize )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqVideoShare, getNextImg )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqVideoShare, getLastImg )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqVideoShare, WriteFrame )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqVideoShare, setAllowFrameSkipping )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqVideoShare, getAllowFrameSkipping )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqVideoShare, getLastErrorStr )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqVideoShare, setLastErrorStr )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqVideoShare, getWriteImg )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqVideoShare, incWritePtr )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqVideoShare, getImg )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqVideoShare, incReadPtr )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqVideoShare, isOpen )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqVideoShare, setPadding )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqVideoShare, getPadding )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqVideoShare, getHeaderSize )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqVideoShare, getBufferGuid )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqVideoShare, getBufferGuidStr )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqVideoShare, getReset )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqVideoShare, setReset )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqVideoShare, setFpsDivider )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqVideoShare, getFpsDivider )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqVideoShare, setFpsDividerCount )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqVideoShare, getFpsDividerCount )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqVideoShare, getReadExtra )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqVideoShare, getWriteExtra )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqVideoShare, getExtraIdx )
//	SQBIND_MEMBER_FUNCTION(  sqbind::CSqVideoShare,  )
//	SQBIND_MEMBER_FUNCTION(  sqbind::CSqVideoShare,  )

SQBIND_REGISTER_CLASS_END()

void CSqVideoShare::Register( sqbind::VM vm )
{_STT();
	SQBIND_EXPORT( vm, CSqVideoShare );
}

CSqVideoShare::CSqVideoShare()
{
	m_uCbId = SQSVS_CBID;
	m_sPrefix = SQSVS_PREFIX;
	m_iRead = 0;
	m_bGlobal = 1;
	m_bWrite = 0;
	m_nPadding = SQSVS_PADDING;
	m_bAllowFrameSkipping = 0;
	m_uTs = 0;
	m_nFpsDivider = 1;
}

CSqVideoShare::CSqVideoShare( const sqbind::stdString &sPrefix, SQInteger nId, SQInteger nGlobal )
{
	m_uCbId = (oex::oexUINT)nId;
	m_sPrefix = sPrefix; 
	m_bGlobal = nGlobal;
	m_iRead = 0;
	m_bWrite = 0;
	m_nPadding = SQSVS_PADDING;
	m_bAllowFrameSkipping = 0;
	m_uTs = 0;
	m_nFpsDivider = 1;
	m_nFpsDividerCount = 0;
}


CSqVideoShare::CSqVideoShare( const CSqVideoShare &r ) 
{
	m_uCbId = r.m_uCbId;
	m_sPrefix = r.m_sPrefix; 
	m_bGlobal = r.m_bGlobal;
	m_iRead = 0;
	m_bWrite = 0;
	m_nPadding = SQSVS_PADDING;
	m_bAllowFrameSkipping = 0;
	m_uTs = 0;
	m_nFpsDivider = 1;
	m_nFpsDividerCount = 0;
}


void CSqVideoShare::Destroy()
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
	m_bAllowFrameSkipping = 0;
	m_uTs = 0;

	m_iRead = 0;
	m_nFpsDividerCount = 0;
}

int CSqVideoShare::Create( const sqbind::stdString &sName, const sqbind::stdString &sBufName, int nBuffers, int nImgSize, int nWidth, int nHeight, int nFps, int nFmt, int nExtra )
{_STT();

	// Lose the old share
	Destroy();

	// Unique name?
	if ( sName.length() )
		m_sName = sName;
	else
		m_sName = oex2std( oexUnique() );

	// Are we using a frame rate divider?
	if ( 1 < m_nFpsDivider )
	{	nFps = nFps / m_nFpsDivider;
		m_nFpsDividerCount = 0;
	} // end if

	// At least on fps
	if ( nFps < 1 ) 
		nFps = 1;
		
	// Are we sharing an image buffer?
	int bShareBuffer = 0;
	if ( sBufName.length() && sName != sBufName )
		bShareBuffer = 1;

	// Structure size
	int struct_size = SQSVS_HEADERSIZE;

	// Control block share name
	oex::oexGUID guidName;
	oex::CStr sgName = oexGuid( std2oex( sqbind::stdString( m_sPrefix ) + m_sName ) );
	sqbind::stdString sidName = ( m_bGlobal ? oexT( "Global\\" ) : oexT( "" ) ) + oex2std( sgName ); 
	sgName.StringToGuid( guidName );

	// Image buffer share name
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
		if ( ( m_cb.getINT( 2 ) && m_cb.getINT( 2 ) != nBuffers )
		     || ( m_cb.getINT( 3 ) && m_cb.getINT( 3 ) != nWidth )
		     || ( m_cb.getINT( 4 ) && m_cb.getINT( 4 ) != nHeight )
		     || ( m_cb.getINT( 5 ) && m_cb.getINT( 5 ) != nFps )
		     || ( m_cb.getINT( 6 ) && m_cb.getINT( 6 ) != nFmt )
		     || ( m_cb.getINT( 7 ) && m_cb.getINT( 7 ) != nImgSize )
		     || ( m_cb.getINT( 18 ) && m_cb.getINT( 18 ) != nExtra )
			)
		{	m_sLastErr = oex2std( oexMks( oexT( "Share control block already exists, and has changed : " ), std2oex( m_sName ), oexT( " : " ), std2oex( sidName ) ) );
			Destroy();
			return 0;
		} // end if

	} // end if

	// Set control block size
	m_cb.setUsed( struct_size );

	// Initialize control block
	m_cb.setINT(	1, -1 );
	m_cb.setINT(	2, nBuffers );
	m_cb.setINT(	3, nWidth );
	m_cb.setINT(	4, nHeight );
	m_cb.setINT(	5, nFps );
	m_cb.setINT(	6, nFmt );
	m_cb.setINT(	7, nImgSize );
	m_cb.setINT(	8, 0 );
	m_cb.setINT(	9, 0 );
	m_cb.setUINT(	10, oexPtrToInt( oex::os::CSys::GetInstanceHandle() ) );
	m_cb.setUINT(	11, ( m_uTs = oexGetUnixTime() ) );
	m_cb.setUINT(	12, SQSVS_HEADERSIZE );
	m_cb.setUINT(	13, 0 );
	m_cb.setUINT(	14, guidBuffer.u1 );
	m_cb.setUINT(	15, guidBuffer.u2 );
	m_cb.setUINT(	16, guidBuffer.u3 );
	m_cb.setUINT(	17, guidBuffer.u4 );
	m_cb.setUINT(	18, nExtra );
	m_cb.setUINT(	19, 0 );

	// Set image buffer info
	m_buf.SetName( sidBuffer );
	m_buf.PlainShare( 1 );
	
	// Attempt to allocate the image buffer
	int size = nBuffers * ( nImgSize + nExtra ) + m_nPadding;
	if ( !m_buf.Allocate( size ) )
	{	m_sLastErr = oex2std( oexMks( oexT( "Failed to allocate image buffer of " ), size, oexT( " bytes : " ), std2oex( ( bShareBuffer ? sBufName : sName ) ), oexT( " : " ), std2oex( sidBuffer ) ) );
		Destroy();
		return 0;
	} // end if

	// If it is shared, it must exist
	if ( bShareBuffer && !m_buf.Existing() )
	{	m_sLastErr = oex2std( oexMks( oexT( "Share image buffer doesn't exist : " ), std2oex( sBufName ), oexT( " : " ), std2oex( sidBuffer ) ) );
		Destroy();
		return 0;
	} // end if

	// Set size of image buffer
	m_buf.setUsed( nBuffers * ( nImgSize + nExtra ) );
	
	// Ok for others to read now
	m_cb.setUINT( 0, m_uCbId );

	// Share is writable
	m_bWrite = 1;	

	return 1;
}

int CSqVideoShare::Open( const sqbind::stdString &sName, int bAllowFrameSkipping )
{_STT();

	// Must have a valid name
	if ( !sName.length() )
		return 0;

	// Structure size
	int struct_size = SQSVS_HEADERSIZE;

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
	int nWidth = getWidth();
	int nHeight = getHeight();
	int nFps = getFps();
	int nBuffers = getBuffers();
	int nImgSize = getImageSize();
	int nExtra = getExtraSize();
	
	// Validate image parameters
	if ( 0 >= nWidth || 0 >= nHeight || 0 >= nFps || 0 >= nBuffers || 0 >= nImgSize )
	{	Destroy();
		return 0;
	} // end if

	// Image buffer share name
	oex::oexGUID guidBuffer;
	oex::CStr sgBuffer;
	if ( getHeaderSize() >= SQSVS_HEADERSIZE )
		sgBuffer = std2oex( getBufferGuidStr() );
	else
		sgBuffer = oexGuid( std2oex( sqbind::stdString( m_sPrefix ) + oexT( "buffer_" ) + sName ) );
	sqbind::stdString sidBuffer = ( m_bGlobal ? oexT( "Global\\" ) : oexT( "" ) ) + oex2std( sgBuffer ); 
	sgBuffer.StringToGuid( guidBuffer );
	
	// Set image buffer info
	m_buf.SetName( sidBuffer );
	m_buf.PlainShare( 1 );
	
	// Attempt to allocate the image buffer
	int size = nBuffers * ( nImgSize + nExtra ) + m_nPadding;
	if ( !m_buf.Allocate( size ) )
	{	m_sLastErr = oex2std( oexMks( oexT( "Failed to allocate image buffer of " ), size, oexT( " bytes : " ), std2oex( sName ), oexT( " : " ), std2oex( sidBuffer ) ) );
		Destroy();
		return 0;
	} // end if

	// Punt if it doesn't exist
	if ( !m_buf.Existing() )
	{	m_sLastErr = oex2std( oexMks( oexT( "Share image buffer does not exist : " ), std2oex( sName ), oexT( " : " ), std2oex( sidBuffer ) ) );
		Destroy();
		return 0;
	} // end if

	// Set size of image buffer
	m_buf.setUsed( nBuffers * ( nImgSize + nExtra ) );

	// Save share information
	m_sName = sName;
	m_bAllowFrameSkipping = bAllowFrameSkipping;
	m_uTs = getTs();

	return 1;
}

sqbind::CSqBinary CSqVideoShare::getNextImg()
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
	}

	// Get the current image index
	int i = m_cb.getINT( 1 );
	int nBuffers = getBuffers();
	int nImgSize = getImageSize();
	int nExtra = getExtraSize();
	
	// Use zero if it's invalid
	if ( 0 > i || i == m_iRead || i >= nBuffers )
		return sqbind::CSqBinary();
		
	// Use the latest if we're skipping frames
	if ( m_bAllowFrameSkipping ) 
		m_iRead = i;
		
	// Return each frame in turn
	else if ( ++m_iRead >= nBuffers ) 
		m_iRead = 0;

	// Count a frame read
	m_cb.setUINT( 9, m_cb.getUINT( 9 ) + 1 );

	// Return a wrapper to the image that's ready
	return m_buf.getSub( nExtra + m_iRead * ( nImgSize + nExtra ), nImgSize );
}

int CSqVideoShare::incReadPtr()
{_STT();

	if ( !m_cb.getUsed() || !m_buf.getUsed() )
	{	m_sLastErr = oexT( "No open share" );
		return 0;
	} // end if

	// Verify that writer is still connected
	if ( m_cb.getUINT( 0 ) != m_uCbId || m_cb.getUINT( 11 ) != m_uTs )
	{	m_sLastErr = oex2std( oexMks( oexT( "Writer has disconnected : " ), std2oex( m_sName ) ) );
		Destroy(); 
		return 0; 
	}

	// Get the current image index
	int i = m_cb.getINT( 1 );
	int nBuffers = getBuffers();
	
	// Use zero if it's invalid
	if ( 0 > i || i == m_iRead || i >= nBuffers )
		return 0;
		
	// Use the latest if we're skipping frames
	if ( m_bAllowFrameSkipping ) 
		m_iRead = i;
		
	// Return each frame in turn
	else if ( ++m_iRead >= nBuffers ) 
		m_iRead = 0;

	// Count a frame read
	m_cb.setUINT( 9, m_cb.getUINT( 9 ) + 1 );

	// Return a wrapper to the image that's ready
	return 1;
}

sqbind::CSqBinary CSqVideoShare::getImg()
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
	}

	// Get the current image index
	int i = m_cb.getINT( 1 );
	int nBuffers = getBuffers();
	int nImgSize = getImageSize();
	int nExtra = getExtraSize();
	
	// Use zero if it's invalid
	if ( 0 > i || i == m_iRead || i >= nBuffers )
		return sqbind::CSqBinary();
		
	// Return a wrapper to the image that's ready
	return m_buf.getSub( nExtra + m_iRead * ( nImgSize + nExtra ), nImgSize );
}

sqbind::CSqBinary CSqVideoShare::getReadExtra()
{_STT();
	return getExtraIdx( m_iRead );
}

sqbind::CSqBinary CSqVideoShare::getWriteExtra()
{_STT();
	return getExtraIdx( getWritePtr() );
}

sqbind::CSqBinary CSqVideoShare::getExtraIdx( int i )
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
	}

	// Get the current image index
	int nBuffers = getBuffers();
	int nImgSize = getImageSize();
	int nExtra = getExtraSize();
	
	// Use zero if it's invalid
	if ( 0 >= nExtra || 0 > i || i >= nBuffers )
		return sqbind::CSqBinary();
		
	// Return a wrapper to the image that's ready
	return m_buf.getSub( i * ( nImgSize + nExtra ), nExtra );
}

sqbind::CSqBinary CSqVideoShare::getLastImg()
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
	}

	// Get the current image index
	int i = m_cb.getINT( 1 );
	int nBuffers = getBuffers();
	int nImgSize = getImageSize();
	int nExtra = getExtraSize();
	if ( 0 > i || i >= nBuffers )
		return sqbind::CSqBinary();

	// Return the latest frame
	return m_buf.getSub( nExtra + i * ( nImgSize + nExtra ), nImgSize );
}

sqbind::CSqBinary CSqVideoShare::getWriteImg()
{_STT();

	if ( !m_cb.getUsed() || !m_buf.getUsed() )
	{	m_sLastErr = oexT( "No open share" );
		return sqbind::CSqBinary();
	} // end if

	// Must be in write mode
	if ( !m_bWrite )
	{	m_sLastErr = oexT( "Shared buffer is not open for writing" );
		return sqbind::CSqBinary();
	} // end if		

	// Don't return a buffer if it would be ignored
	if ( 1 < m_nFpsDivider && m_nFpsDividerCount )
		return sqbind::CSqBinary();

	// Get the next image index
	int i = m_cb.getINT( 1 ) + 1;
	int nBuffers = getBuffers();
	int nImgSize = getImageSize();
	int nExtra = getExtraSize();
	if ( 0 > i || i >= nBuffers )
		i = 0;

	// Return the next frame buffer
	return m_buf.getSub( nExtra + i * ( nImgSize + nExtra ), nImgSize );
}

int CSqVideoShare::incWritePtr()
{_STT();

	if ( !m_cb.getUsed() || !m_buf.getUsed() )
	{	m_sLastErr = oexT( "No open share" );
		return 0;
	} // end if

	if ( !m_bWrite )
	{	m_sLastErr = oexT( "Shared buffer is not open for writing" );
		return 0;
	} // end if		

	// Implement frame divider
	if ( 1 < m_nFpsDivider && m_nFpsDividerCount++ )
	{	if ( m_nFpsDividerCount >= m_nFpsDivider ) m_nFpsDividerCount = 0;
		return 1;
	} // end if

	// Get the next image index
	int i = m_cb.getINT( 1 ) + 1;
	int nBuffers = getBuffers();
	if ( 0 > i || i >= nBuffers )
		i = 0;

	// Count a frame written
	m_cb.setUINT( 8, m_cb.getUINT( 8 ) + 1 );

	// Next frame
	m_cb.setINT( 1, i );
	
	return 1;
}


int CSqVideoShare::WriteFrame( sqbind::CSqBinary *frame )
{_STT();

	if ( !frame || !frame->getUsed() )
	{	m_sLastErr = oexT( "Invalid frame" );
		return 0;
	} // end if		

	if ( !m_bWrite )
	{	m_sLastErr = oexT( "Shared buffer is not open for writing" );
		return 0;
	} // end if		

	if ( !m_cb.getUsed() || !m_buf.getUsed() )
	{	m_sLastErr = oexT( "No open share" );
		return 0;
	} // end if

	// Implement frame divider
	if ( 1 < m_nFpsDivider && m_nFpsDividerCount++ )
	{	if ( m_nFpsDividerCount >= m_nFpsDivider ) m_nFpsDividerCount = 0;
		return 1;
	} // end if

	// Get info from control block
	int i = m_cb.getINT( 1 ) + 1;
	int nBuffers = getBuffers();
	int nImgSize = getImageSize();
	int nExtra = getExtraSize();
	if ( 0 > i || i >= nBuffers ) 
		i = 0;
		
	// Ensure it's not too much data
	int nCopy = nImgSize;
	if ( m_buf.getUsed() < (unsigned int)nCopy )
		nCopy = m_buf.getUsed();
	
	// Copy the image data into the shared buffer
	m_buf.MemCpyNumAt( frame, nExtra + i * ( nImgSize + nExtra ), nCopy );

	// Count a frame written
	m_cb.setUINT( 8, m_cb.getUINT( 8 ) + 1 );

	// Next frame
	m_cb.setINT( 1, i );

	return 1;
}


