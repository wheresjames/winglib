
#include "../stdafx.h"
#include "riff_file.h"

CRiffFile::CRiffFile()
{
	m_llAmhOffset = 0;
	m_llAshOffset = 0;
	m_llWfeOffset = 0;
}

CRiffFile::~CRiffFile()
{
	Destroy();
}

void CRiffFile::Destroy()
{
	// Close any open file
	m_file.Close();

	// Release headers
	m_amh.OexDelete();
	m_ash.OexDelete();
	m_wfe.OexDelete();

	m_llAmhOffset = 0;
	m_llAshOffset = 0;
	m_llWfeOffset = 0;
}

BOOL CRiffFile::Create( LPCTSTR pFile )
{
	// Lose current file
	Destroy();

	// Open new file
	if ( !m_file.CreateNew( pFile ).IsOpen() )
	{	oexTRACE( oexT( " : %lu" ), (unsigned long)m_file.GetLastError() );
		return FALSE;
	} // end if

	return TRUE;
}

BOOL CRiffFile::Open( LPCTSTR pFile )
{
	// Lose current file
	Destroy();

	// Attempt to open existing file
	if ( !m_file.OpenExisting( pFile ).IsOpen() )
	{	oexTRACE( oexT( "%s : %lu" ), __FUNCTION__, (unsigned long)m_file.GetLastError() );
		return FALSE;
	} // end if

	if ( !m_file.Read( &m_rfh, sizeof( m_rfh ) ) )
	{	oexTRACE( oexT( "%s : %lu" ), __FUNCTION__, (unsigned long)m_file.GetLastError() );
		Destroy();
		return FALSE;
	} // end if

	// Verify four cc
	if ( eFccRiff != oexLittleEndian( m_rfh.fccRiff ) )
	{	oexTRACE( oexT( "%s : File does not start with 'RIFF'" ), __FUNCTION__ );
		Destroy();
		return FALSE;
	} // end if

	// Verify four cc
	if ( eFccAvi != oexLittleEndian( m_rfh.fccType ) )
	{	oexTRACE( oexT( "%s : RIFF is not an 'AVI ' type" ), __FUNCTION__ );
		Destroy();
		return FALSE;
	} // end if

	// Verify four cc
	if ( ReadAviHeaders() )
	{	oexTRACE( oexT( "%s : Improper headers" ), __FUNCTION__ );
		Destroy();
		return FALSE;
	} // end if


	return TRUE;
}

BOOL CRiffFile::ReadChunkHeader( SRiffChunk *pRc )
{
	if ( !m_file.IsOpen() )
		return FALSE;

	// Restore file position after read
	oex::CFile::CRestoreFilePos rfp( &m_file );

	// Read chunk header
	return m_file.Read( pRc, sizeof( SRiffChunk ) );
}

BOOL CRiffFile::SkipChunk()
{																								
	SRiffChunk rc;

	// Read the chunk header
	if ( !m_file.Read( &rc, sizeof( rc ) ) )
		return FALSE;

	// Attempt to seek past the end of the file
	m_file.SetPtrPosCur( oexLittleEndian( rc.lDataSize ) );

	return TRUE;
}

BOOL CRiffFile::FindChunk( unsigned long fccType, SRiffChunk *pRc )
{
	SRiffChunk rc;
	if ( !pRc )
		pRc = &rc;

	// Read the chunk header
	while ( ReadChunkHeader( pRc ) )
	{	
		// Is this our guy?
		if ( oexLittleEndian( pRc->fccType ) == fccType )
			return TRUE;

		// Next chunk
		if ( !SkipChunk() )
			return FALSE;

	} // end while	

	return FALSE;
}

BOOL CRiffFile::ReadList()
{
	if ( !m_file.IsOpen() )
		return FALSE;

	// Restore file position after this
	oex::CFile::CRestoreFilePos rfp( &m_file );

	SRiffChunk rc;
	if ( !m_file.Read( &rc, sizeof( rc ) ) )
		return FALSE;

	// Must at least have a four cc, also, ignore huge blocks
	if ( 4 > oexLittleEndian( rc.lDataSize ) || 1024 * 1024 < oexLittleEndian( rc.lDataSize ) )
		return FALSE;

	// Verify it is a list
	if ( eFccList != oexLittleEndian( rc.fccType ) )
		return FALSE;

	oex::TMem< char > buf;
	if ( !buf.OexNew( rc.lDataSize ).Ptr() )
		return FALSE;

	// Read the chunk header
	if ( !m_file.Read( buf.Ptr(), rc.lDataSize ) )
		return FALSE;

	// Check for header
	char *pBuf = buf.Ptr();
	if ( eAviHeader == oexLittleEndian( *(LPUINT)pBuf ) )
	{
		rc.lDataSize -= sizeof( UINT );
		pBuf += sizeof( UINT );

		if ( eAviMainHeader == oexLittleEndian( *(LPUINT)pBuf ) )
		{
			// Ensure valid header size
			if ( sizeof( SAviMainHeader ) > rc.lDataSize )
				return FALSE;

			if ( !m_amh.New( sizeof( SAviMainHeader ) ).Ptr() )
				return FALSE;

			// Copy data
			m_amh.MemCpy( pBuf, sizeof( SAviMainHeader ) );

			// Skip the header
			pBuf += oexLittleEndian( m_amh->cb ) + 8;
			rc.lDataSize -= oexLittleEndian( m_amh->cb ) + 8;

		} // end if

		if ( eFccList == oexLittleEndian( *(LPUINT)pBuf ) )
		{
			SRiffChunk rcList;
			memcpy( &rcList, pBuf, sizeof( SRiffChunk ) );
			pBuf += sizeof( SRiffChunk );

		} // end if

	} // end if

	return TRUE;
}

BOOL CRiffFile::ReadAviHeaders()
{
	SRiffChunk rc;

	while ( ReadChunkHeader( &rc ) )
	{
		// Look for a list
		if ( eFccList == oexLittleEndian( rc.fccType ) )
			ReadList();

		// Next chunk
		if ( !SkipChunk() )
			return FALSE;

	} // end while

	return TRUE;
}

