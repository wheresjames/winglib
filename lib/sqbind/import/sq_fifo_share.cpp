/*------------------------------------------------------------------
// sq_fifo_share.cpp
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
#define SQSFS_PREFIX		oexT( "oex_fifo_share_" )
#define SQSFS_CBID			0xBEBAA5A5
#define SQSFS_PADDING		64
#define SQSFS_CBSIZE		( 16 * 4 )
#define SQSFS_PTSIZE		sizeof( sqbind::CSqFifoShare::SPtrInfo )

SQBIND_REGISTER_CLASS_BEGIN( sqbind::CSqFifoShare, CSqFifoShare )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFifoShare, Destroy )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFifoShare, Create )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFifoShare, Open )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFifoShare, isOpen )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFifoShare, isValid )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFifoShare, getName )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFifoShare, setPrefix )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFifoShare, getPrefix )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFifoShare, setCbId )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFifoShare, getCbId )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFifoShare, setGlobal )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFifoShare, getGlobal )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFifoShare, setPadding )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFifoShare, getPadding )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFifoShare, getLastErrorStr )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFifoShare, setLastErrorStr )
	
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFifoShare, getWritePtr )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFifoShare, getCbSize )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFifoShare, getTs )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFifoShare, getProcId )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFifoShare, getBufSize )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFifoShare, getBlocks )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFifoShare, getHeaderSize )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFifoShare, getWrites )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFifoShare, getReads )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFifoShare, getReset )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFifoShare, setReset )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFifoShare, getBufferGuid )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFifoShare, getBufferGuidStr )

	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFifoShare, ReadHeader )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFifoShare, ReadData )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFifoShare, ReadUser )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFifoShare, ReadTs )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFifoShare, ReadTsMin )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFifoShare, ReadTsMax )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFifoShare, incReadPtr )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFifoShare, decReadPtr )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFifoShare, isRead )
	
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFifoShare, Write )
	
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFifoShare, Reset )
//	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFifoShare,  )
//	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFifoShare,  )

SQBIND_REGISTER_CLASS_END()

void CSqFifoShare::Register( sqbind::VM vm )
{_STT();
	SQBIND_EXPORT( vm, CSqFifoShare );
}

CSqFifoShare::CSqFifoShare()
{
	m_sPrefix = SQSFS_PREFIX; 
	m_uCbId = SQSFS_CBID;
	m_iRead = -1;
	m_iWrite = 0;
	m_bGlobal = 1;
	m_bWrite = 0;
	m_nPadding = SQSFS_PADDING;
	m_uTs = 0;
}

CSqFifoShare::CSqFifoShare( const sqbind::stdString &sPrefix, SQInteger nId, SQInteger nGlobal )
{
	m_sPrefix = sPrefix; 
	m_uCbId = (oex::oexUINT)nId;
	m_bGlobal = nGlobal;
	m_iRead = -1;
	m_iWrite = 0;
	m_bWrite = 0;
	m_nPadding = SQSFS_PADDING;
	m_uTs = 0;
}


CSqFifoShare::CSqFifoShare( const CSqFifoShare &r ) 
{
 	m_sPrefix = r.m_sPrefix;
	m_uCbId = r.m_uCbId;
	m_bGlobal = r.m_bGlobal;
	m_iRead = -1;
	m_iWrite = 0;
	m_bWrite = 0;
	m_nPadding = SQSFS_PADDING;
	m_uTs = 0;
}


void CSqFifoShare::Destroy()
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
	m_iWrite = 0;
}

int CSqFifoShare::Create( const sqbind::stdString &sName, const sqbind::stdString &sBufName, int nBufSize, int nBlocks, const sqbind::stdString &sHeader )
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
	int struct_size = SQSFS_CBSIZE;
	
	// Convert header to multi-byte string if needed
	oex::CStr8 sMbHeader = oexStrToMb( sqbind::std2oex( sHeader ) );
	int nHeaderSize = sMbHeader.Length();

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
		     || ( m_cb.getINT( 3 ) && m_cb.getINT( 3 ) != nBlocks ) 
		     || ( m_cb.getINT( 4 ) && m_cb.getINT( 4 ) != nHeaderSize )
		   )
		{	m_sLastErr = oex2std( oexMks( oexT( "Share control block already exists, and has changed : " ), std2oex( m_sName ), oexT( " : " ), std2oex( sidName ) ) );
			Destroy();
			return 0;
		} // end if

	} // end if

	// Set control block size
	m_cb.setUsed( struct_size );

	// Initialize control block
	m_cb.setINT(	0, 0 );
	m_cb.setINT(	1, 0 );
	m_cb.setUINT(	2, SQSFS_CBSIZE );
	m_cb.setUINT(	3, ( m_uTs = oexGetUnixTime() ) );
	m_cb.setUINT(	4, oexPtrToInt( oex::os::CSys::GetInstanceHandle() ) );
	m_cb.setINT(	5, nBufSize );
	m_cb.setINT(	6, nBlocks );
	m_cb.setINT(	7, nHeaderSize );
	m_cb.setINT(	8, 0 );
	m_cb.setINT(	9, 0 );
	m_cb.setUINT(	10, 0 );
	m_cb.setUINT(	11, 0 );
	m_cb.setUINT(	12, guidBuffer.u1 );
	m_cb.setUINT(	13, guidBuffer.u2 );
	m_cb.setUINT(	14, guidBuffer.u3 );
	m_cb.setUINT(	15, guidBuffer.u4 );

	// Set buffer info
	m_buf.SetName( sidBuffer );
	m_buf.PlainShare( 1 );
	
	// Attempt to allocate the buffer
	int size = ( nBlocks * SQSFS_PTSIZE ) + nHeaderSize + nBufSize + m_nPadding;
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
	m_buf.setUsed( size );

	// Save the header data
	char *ph = (char*)m_buf.Ptr( nBlocks * SQSFS_PTSIZE );
	if ( ph && sMbHeader.Length() == (unsigned int)nHeaderSize )
		oexMemCpy( ph, sMbHeader.Ptr(), nHeaderSize );
	
	// Ok for others to read now
	m_cb.setUINT( 0, m_uCbId );

	// Share is writable
	m_bWrite = 1;	

	return 1;
}

int CSqFifoShare::Open( const sqbind::stdString &sName )
{_STT();

	// Must have a valid name
	if ( !sName.length() )
	{	m_sLastErr = oex2std( oexMks( oexT( "Invalid name : " ), std2oex( sName ) ) );
		return 0;
	} // end if

	// Structure size
	int struct_size = SQSFS_CBSIZE;

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
	{	m_sLastErr = oex2std( oexMks( oexT( "Invalid control block id : " ), m_cb.getUINT( 0 ), oexT( " != " ), m_uCbId ) );
		Destroy();
		return 0;
	} // end if

	/// Get buffer params
	int nBufSize = getBufSize();
	int nBlocks = getBlocks();
	int nHeaderSize = getHeaderSize();
	
	// Validate parameters
	if ( 0 >= nBufSize || 0 >= nBlocks || 0 > nHeaderSize )
	{	m_sLastErr = oex2std( oexMks( oexT( "Invalid control block values : " ), 
									  oexT( "nBufSize = " ), nBufSize,
									  oexT( ", nBlocks = " ), nBlocks,
									  oexT( ", nHeaderSize = " ), nHeaderSize
									  ) );
		Destroy();
		return 0;
	} // end if

	// Data buffer share name
	oex::oexGUID guidBuffer;
	oex::CStr sgBuffer;
	sgBuffer = std2oex( getBufferGuidStr() );
	sqbind::stdString sidBuffer = ( m_bGlobal ? oexT( "Global\\" ) : oexT( "" ) ) + oex2std( sgBuffer ); 
	sgBuffer.StringToGuid( guidBuffer );

	// Set buffer info
	m_buf.SetName( sidBuffer );
	m_buf.PlainShare( 1 );
	
	// Attempt to allocate the buffer
	int size = ( nBlocks * SQSFS_PTSIZE ) + nHeaderSize + nBufSize + m_nPadding;
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
	m_buf.setUsed( size );

	// Save share information
	m_sName = sName;
	m_uTs = getTs();

	return 1;
}

int CSqFifoShare::isValid()
{_STT();

	// Is the share open?
	if ( !m_cb.getUsed() || !m_buf.getUsed() )
	{	m_sLastErr = oexT( "No open share" );
		return 0;
	} // end if

	// Verify that writer is still connected
	if ( m_cb.getUINT( 0 ) != m_uCbId || m_cb.getUINT( 3 ) != m_uTs )
	{	m_sLastErr = oex2std( oexMks( oexT( "Writer has disconnected : " ), std2oex( m_sName ) ) );
		Destroy(); 
		return 0; 
	} // end if

	return 1;	
}

sqbind::stdString CSqFifoShare::getHeader()
{_STT();

	if ( !isValid() )
		return oexT( "" );

	int nBlocks = getBlocks();
	int nHeaderSize = getHeaderSize();
	char *ph = (char*)m_buf.Ptr( nBlocks * SQSFS_PTSIZE );

	// Return data portion
	return sqbind::oex2std( oexMbToStr( oex::CStr8( ph, nHeaderSize ) ) );
}

int CSqFifoShare::isRead()
{_STT();
	return ReadPtr() ? 1 : 0;
}

CSqFifoShare::SPtrInfo* CSqFifoShare::ReadPtr()
{_STT();

	// Ensure valid share
	if ( !isValid() )
		return oexNULL; 
	
	// Get the current buffer pointer, and make sure it's valid
	int i = getWritePtr();
	int nBlocks = getBlocks();

	// Initialize our read pointer
	if ( 0 > m_iRead || m_iRead >= nBlocks ) 
		m_iRead = i;

	// Any data?
	if ( 0 > i || i == m_iRead || i >= nBlocks )
		return oexNULL; 

	/// Pointer information	
	SPtrInfo *pi = (SPtrInfo*)m_buf.Ptr();

	// Return data portion
	return &pi[ m_iRead ];
}

sqbind::stdString CSqFifoShare::ReadHeader()
{_STT();

	SPtrInfo *pi = ReadPtr();
	if ( !pi )
		return oexT( "" ); 

	// Return data portion
	return m_buf.getSub( pi->uPtr, pi->uHeader ).getString();
}

sqbind::CSqBinary CSqFifoShare::ReadData()
{_STT();

	SPtrInfo *pi = ReadPtr();
	if ( !pi )
		return sqbind::CSqBinary(); 

	// Return data portion
	return m_buf.getSub( pi->uPtr + pi->uHeader, pi->uData );
}

int CSqFifoShare::ReadUser()
{_STT();

	SPtrInfo *pi = ReadPtr();
	if ( !pi )
		return 0; 

	// Return data portion
	return pi->uUser;
}

SQInteger CSqFifoShare::ReadTs()
{_STT();

	SPtrInfo *pi = ReadPtr();
	if ( !pi )
		return 0; 

	// Return data portion
	return pi->uTs;
}

SQInteger CSqFifoShare::CalculateTsRange( SQInteger *pMin, SQInteger *pMax )
{_STT();

	SQInteger min, max;
	if ( !pMin )
		pMin = &min;
	if ( !pMax )
		pMax = &max;

	*pMin = 0;
	*pMax = 0;

	// Ensure valid share
	if ( !isValid() )
		return oexNULL; 
	
	// Get the current buffer pointer, and make sure it's valid
	int w = getWritePtr(), r = m_iRead;
	int nBlocks = getBlocks();

	// Initialize our read pointer
	if ( 0 > r || r >= nBlocks ) 
		r = w;

	// Any data?
	if ( 0 > w || w == r || w >= nBlocks )
		return 0; 

	/// Pointer information	
	SPtrInfo *pi = (SPtrInfo*)m_buf.Ptr();

	// Initialize minimum / maximum
	*pMin = pi[ r ].uTs;
	*pMax = pi[ r ].uTs;
	
	while ( w != r )
	{
		// Track the minimum
		if ( *pMin > pi[ r ].uTs )
			*pMin = pi[ r ].uTs;

		// Track the maximum
		if ( *pMax < pi[ r ].uTs )
			*pMax = pi[ r ].uTs;

		if ( ++r >= nBlocks )
			r = 0;

	} // end while

	// Return data portion
	return *pMax - *pMin;
}


int CSqFifoShare::incReadPtr()
{
	// Ensure valid share
	if ( !isValid() )
		return 0; 
	
	// Get the current buffer pointer, and make sure it's valid
	int i = getWritePtr();
	int nBlocks = getBlocks();

	// Initialize our read pointer
	if ( 0 > m_iRead || m_iRead >= nBlocks ) 
		m_iRead = i;
		
	// Any more data?
	if ( 0 > i || i == m_iRead || i >= nBlocks )
		return 0; 

	// Next block		
	if ( ++m_iRead >= nBlocks )
		m_iRead = 0;

	// Count a read
	m_cb.setUINT( 9, m_cb.getUINT( 9 ) + 1 );

	return 1;
}

int CSqFifoShare::decReadPtr()
{
	// Ensure valid share
	if ( !isValid() )
		return 0; 
	
	// Get the current buffer pointer, and make sure it's valid
	int i = getWritePtr();
	int nBlocks = getBlocks();

	// Initialize our read pointer
	if ( 0 > m_iRead || m_iRead >= nBlocks ) 
		m_iRead = i;

	if ( 0 >= nBlocks )
		return 0;

	// Prev block
	if ( --m_iRead < 0 )
		m_iRead = nBlocks - 1;

	return 1;
}

int CSqFifoShare::Write( sqbind::CSqBinary *pData, const sqbind::stdString &sHeader, int nUser, SQInteger ts )
{
	if ( !pData )
	{	m_sLastErr = oexT( "Invalid parameter" );
		return 0;
	} // end if

	return WritePtr( pData->Ptr(), pData->getUsed(), sHeader, nUser, ts );
}

int CSqFifoShare::WritePtr( const void *pData, int nSize, const sqbind::stdString &sHeader, int nUser, SQInteger ts )
{
	if ( !pData )
	{	m_sLastErr = oexT( "Invalid parameter" );
		return 0;
	} // end if

	// Ensure valid share
	if ( !isValid() )
		return 0; 

	if ( !m_bWrite )
	{	m_sLastErr = oexT( "Share is read only" );
		return 0;
	} // end if
	
	if ( 0 >= nSize )
		return 0;

	// Get the current buffer pointer, and make sure it's valid
	int i = getWritePtr();
	int nBufSize = getBufSize();
	int nBlocks = getBlocks();
	int nHeaderSize = getHeaderSize();
	int nBuffer = nBlocks * SQSFS_PTSIZE + nHeaderSize;
	
	/// Pointer information	
	SPtrInfo *pi = (SPtrInfo*)m_buf.Ptr();

	// How much do we want to write
	oex::CStr8 sMbHeader = oexStrToMb( sqbind::std2oex( sHeader ) );
	int hsz = sMbHeader.Length();
	int w = hsz + nSize;

	// Is the buffer large enough?
	if ( w >= nBufSize )
		return 0;

	// Validate write pointer
	if ( 0 > m_iWrite || nBufSize <= m_iWrite )
		m_iWrite = 0;

	// We won't be wrapping any data
	if ( w >= ( nBufSize - m_iWrite ) )
		m_iWrite = 0;

	// Get piece of the buffer we will write too		
	char *pDst = (char*)m_buf.Ptr( nBuffer + m_iWrite );
	if ( !pDst )
		return 0;

	// Update write block pointer
	if ( 0 > i || i >= nBlocks )
		i = 0;
		
	// Save write info
	pi[ i ].uPtr = nBuffer + m_iWrite;
	pi[ i ].uHeader = hsz;
	pi[ i ].uData = nSize;
	pi[ i ].uUser = nUser;
	pi[ i ].uTs = ts;

	// Write the header and data
	if ( hsz )
		oexMemCpy( pDst, sMbHeader.Ptr(), hsz );
	if ( nSize )
		oexMemCpy( pDst + hsz, pData, nSize );

	// Count a frame written
	m_cb.setUINT( 8, m_cb.getUINT( 8 ) + 1 );

	// Loop pointer
	if ( ++i >= nBlocks )
		i = 0;

	// Update write buffer pointer
	m_iWrite += w;

	// Update write pointer
	m_cb.setINT( 1, i );
	
	return 1;
}

int CSqFifoShare::Reset() 
{
	// Reset the read pointer
	m_iRead = -1;

	return 1;
}


