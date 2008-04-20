
#include "../stdafx.h"
#include "avi_file.h"

// Gee, never seen this macro before...
#define CAviFile_Delete( p ) ( ( p ) ? delete p, p = NULL : NULL )

CAviFile::CAviFile()
{
	m_fh = NULL;
	m_pAmh = NULL;
	m_pAsh = NULL;
	m_pWfe = NULL;

	m_llAmhOffset = 0;
	m_llAshOffset = 0;
	m_llWfeOffset = 0;
}

CAviFile::~CAviFile()
{
	Destroy();
}

void CAviFile::Destroy()
{
	// Close any open file
	if ( m_fh )
	{	fclose( m_fh );
		m_fh = NULL;
	} // end if

	// Release headers
	CAviFile_Delete( m_pAmh );
	CAviFile_Delete( m_pAsh );
	CAviFile_Delete( m_pWfe );

	m_llAmhOffset = 0;
	m_llAshOffset = 0;
	m_llWfeOffset = 0;
}

BOOL CAviFile::Create( LPCTSTR pFile )
{
	// Lose current file
	Destroy();

	// Open new file
	m_fh = fopen( pFile, "wb" );

	if ( !m_fh )
	{	//TRACE( __FUNCTION__ " : %lu", (unsigned long)errno );
		return FALSE;
	} // end if

	return TRUE;
}

BOOL CAviFile::Open( LPCTSTR pFile )
{
	// Lose current file
	Destroy();

	// Open new file
	m_fh = fopen( pFile, "rb" );

	if ( !m_fh )
	{	//TRACE( __FUNCTION__ " : %lu", (unsigned long)errno );
		return FALSE;
	} // end if

	if ( sizeof( m_rfh ) != fread( &m_rfh, 1, sizeof( m_rfh ), m_fh ) )
	{	//TRACE( __FUNCTION__ " : %lu", (unsigned long)errno );
		Destroy();
		return FALSE;
	} // end if

	// Verify four cc
	if ( eFccRiff != FIX_LONG( m_rfh.fccRiff ) )
	{	//TRACE( __FUNCTION__ " : File does not start with 'RIFF'" );
		Destroy();
		return FALSE;
	} // end if

	// Verify four cc
	if ( eFccAvi != FIX_LONG( m_rfh.fccType ) )
	{	//TRACE( __FUNCTION__ " : RIFF is not an 'AVI ' type" );
		Destroy();
		return FALSE;
	} // end if

	// Verify four cc
	if ( ReadAviHeaders() )
	{	//TRACE( __FUNCTION__ " : Improper headers" );
		Destroy();
		return FALSE;
	} // end if


	return TRUE;
}

BOOL CAviFile::ReadChunkHeader( SRiffChunk *pRc )
{
	if ( !m_fh )
		return FALSE;

	CRestoreFilePos rfp( m_fh );

	// Read the chunk header
	if ( sizeof( SRiffChunk ) != fread( pRc, 1, sizeof( SRiffChunk ), m_fh ) )
		return FALSE;

	// Restore the file position
    return rfp.Restore();
}

BOOL CAviFile::SkipChunk()
{																								
	SRiffChunk rc;

	// Read the chunk header
	if ( sizeof( SRiffChunk ) != fread( &rc, 1, sizeof( SRiffChunk ), m_fh ) )
		return FALSE;

	// Attempt to seek this far forward in the file
	return !_fseeki64( m_fh, FIX_LONG( rc.lDataSize ), SEEK_CUR );	
}

BOOL CAviFile::FindChunk( unsigned long fccType, SRiffChunk *pRc )
{
	SRiffChunk rc;
	if ( !pRc )
		pRc = &rc;

	// Read the chunk header
	while ( ReadChunkHeader( pRc ) )
	{	
		// Is this our guy?
		if ( FIX_LONG( pRc->fccType ) == fccType )
			return TRUE;

		// Next chunk
		if ( !SkipChunk() )
			return FALSE;

	} // end while	

	return FALSE;
}

BOOL CAviFile::ReadList( SRiffChunk *pRc )
{
	if ( !pRc || !m_fh )
		return FALSE;

	// Restore file position after this
	CRestoreFilePos rfp( m_fh );

	SRiffChunk rc;
	if ( sizeof( rc ) != fread( &rc, 1, sizeof( rc ), m_fh ) )
		return FALSE;

	// Must at least have a four cc
	if ( 4 > rc.lDataSize )
		return FALSE;

	// Verify it is a list
	if ( eFccList != FIX_LONG( rc.fccType ) )
		return FALSE;

	char *pBuf = new char[ pRc->lDataSize ];
	if ( !pBuf )
		return FALSE;

	// Read the chunk header
	if ( pRc->lDataSize != fread( pBuf, 1, pRc->lDataSize, m_fh ) )
		return FALSE;

	// Check for header
	if ( eAviHeader == FIX_LONG( *(LPUINT)pBuf ) )
	{
		rc.lDataSize -= sizeof( UINT );
		pBuf += sizeof( UINT );

		if ( eAviMainHeader == FIX_LONG( *(LPUINT)pBuf ) )
		{
			// Ensure valid header size
			if ( sizeof( SAviMainHeader ) > rc.lDataSize )
				return FALSE;

			CAviFile_Delete( m_pAmh );
			m_pAmh = (SAviMainHeader*)new BYTE[ sizeof( SAviMainHeader ) ];
			if ( !m_pAmh )
				return FALSE;

			// Copy main header data
			memcpy( m_pAmh, pBuf, sizeof( SAviMainHeader ) );

			// Skip the header
			pBuf += FIX_LONG( m_pAmh->cb ) + 8;
			rc.lDataSize -= FIX_LONG( m_pAmh->cb ) + 8;

		} // end if

		if ( eFccList == FIX_LONG( *(LPUINT)pBuf ) )
		{
			SRiffChunk rcList;
			memcpy( &rcList, pBuf, sizeof( SRiffChunk ) );
			pBuf += sizeof( SRiffChunk );

		} // end if

	} // end if

	return TRUE;
}

BOOL CAviFile::ReadAviHeaders()
{
	SRiffChunk rc;

	while ( ReadChunkHeader( &rc ) )
	{
		// Look for a list
		if ( eFccList == FIX_LONG( rc.fccType ) )
			ReadList( &rc );

		// Next chunk
		if ( !SkipChunk() )
			return FALSE;

	} // end while

	return TRUE;
}

