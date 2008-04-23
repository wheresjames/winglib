
#include "../stdafx.h"
#include "riff_file.h"

// S M T W T F S
// 1 2 3 4 5 6 7

CRiffFile::CRiffFile()
{
	m_llAmhOffset = 0;
	m_llAshOffset = 0;
	m_llWfeOffset = 0;

	m_bValidHeaders = FALSE;

	m_llIndex = 0;
	m_uIndexType = 0;
	m_llIndexSize = 0;
	m_llIndexBase = 0;

	m_llStreamOffset = 0;

	m_llFrameSize = 0;
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

	// Lose the index
	m_llIndexBase = 0;
	m_memAviIndex.Delete();

	m_memFrame.Delete();

	m_llAmhOffset = 0;
	m_llAshOffset = 0;
	m_llWfeOffset = 0;

	m_bValidHeaders = FALSE;

	m_llIndex = 0;
	m_uIndexType = 0;
	m_llIndexSize = 0;

	m_llStreamOffset = 0;

	m_llFrameSize = 0;
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

	if ( !WriteAviHeaders() )
	{	oexTRACE( oexT( "%s : Unable to write headers to file" ), __FUNCTION__ );
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
	if ( !ReadAviHeaders() )
	{	oexTRACE( oexT( "%s : Improper headers" ), __FUNCTION__ );
		Destroy();
		return FALSE;
	} // end if

	// Find the stream index
	if ( !FindIndex() )
	{	oexTRACE( oexT( "%s : Stream index not found" ), __FUNCTION__ );
		Destroy();
		return FALSE;
	} // end if

	// Ensure there is a stream in the file
	if ( !StartStream() )
	{	oexTRACE( oexT( "%s : Data stream not found" ), __FUNCTION__ );
		Destroy();
		return FALSE;
	} // end if

	LPVOID pData = NULL;
	LONGLONG llSize = 0;

	for ( DWORD i = 0; i < 10; i++ )
	{
		BOOL bRet = GetFrameData( i, &pData, &llSize );

		oex::CFile f;

		if ( f.CreateNew( oex::CStr().Fmt( oexT( "Frame %lu.jpg" ), i ).Ptr() ).IsOpen() )
			f.Write( pData, llSize );

	} // end for


//	SAviIndex::SAviIndexEntry *pAi = GetFrameInfo( 0 );
//	pAi = GetFrameInfo( 1 );
//	pAi = GetFrameInfo( 2 );

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

BOOL CRiffFile::ReadHeadersFromList()
{
	if ( !m_file.IsOpen() )
		return FALSE;

	SRiffChunk rc;
	if ( !m_file.Read( &rc, sizeof( rc ) ) )
		return FALSE;

	// Must at least have a four cc, also, ignore huge blocks
	if ( 4 > oexLittleEndian( rc.lDataSize ) || ( 16 * 1024 ) < oexLittleEndian( rc.lDataSize ) )
		return FALSE;

	// Verify it is a list
	if ( eFccList != oexLittleEndian( rc.fccType ) )
		return FALSE;

	// Verify AVI header block
	unsigned long fcc = 0;
	if ( !m_file.Read( &fcc, 4 ) || eAviHeader != oexLittleEndian( fcc ) )
		return FALSE;

	// Next comes the avi header
	m_llAmhOffset = m_file.GetPtrPos();
	if ( !m_file.Read( &rc, sizeof( rc ) ) || eAviMainHeader != oexLittleEndian( rc.fccType ) )
		return FALSE;

	// Allocate space for avi header
	if ( sizeof( SAviMainHeader ) > ( rc.lDataSize + 8 ) || !m_amh.New( rc.lDataSize + 8 ).Ptr() )
		return FALSE;

	// Read in the header data
	m_file.SetPtrPosBegin( m_llAmhOffset );
	if ( !m_file.Read( m_amh.Ptr(), rc.lDataSize + 8 ) )
		return FALSE;

	// Verify list data containing stream info
	SRiffChunk rcStream;
	if ( !m_file.Read( &rcStream, sizeof( rcStream ) ) || eFccList != oexLittleEndian( rcStream.fccType ) )
		return FALSE;

	// Verify stream info block
	if ( !m_file.Read( &fcc, 4 ) || eAviStreamInfo != oexLittleEndian( fcc ) )
		return FALSE;

	// Stream header?
	m_llAshOffset = m_file.GetPtrPos();
	if ( !m_file.Read( &rc, sizeof( rc ) ) || eAviStreamHeader != oexLittleEndian( rc.fccType ) )
		return FALSE;

	// Allocate space for avi header
	if ( sizeof( SAviStreamHeader ) > ( rc.lDataSize + 8 ) || !m_ash.New( rc.lDataSize + 8 ).Ptr() )
		return FALSE;

	// Read the stream header
	m_file.SetPtrPosBegin( m_llAshOffset );
	if ( !m_file.Read( m_ash.Ptr(), rc.lDataSize + 8 ) )
		return FALSE;

	// Stream format info should be next
	if ( !m_file.Read( &rc, sizeof( rc ) ) || eAviStreamFormat != oexLittleEndian( rc.fccType ) )
		return FALSE;

	// Allocate space for audio format information
	if ( sizeof( SWaveFormatEx ) > rc.lDataSize || !m_wfe.OexNew( rc.lDataSize ).Ptr() )
		return FALSE;

	// Read in the audio data
	m_llWfeOffset = m_file.GetPtrPos();
	if ( !m_file.Read( m_wfe.Ptr(), rc.lDataSize ) )
		return FALSE;

	// We have valid headers
	m_bValidHeaders = TRUE;

	return TRUE;
}

BOOL CRiffFile::ReadAviHeaders()
{
	if ( !m_file.IsOpen() )
		return FALSE;

	// Restore file position after this
	oex::CFile::CRestoreFilePos rfp( &m_file );

	// Forget the old headers
	m_bValidHeaders = FALSE;

	// Start at the beginning of the file
	m_file.SetPtrPosBegin( sizeof( m_rfh ) );

	// Iterate chunks
	SRiffChunk rc;
	while ( ReadChunkHeader( &rc ) )
	{
		// Look for a LIST chunk, it might contain headers
		if ( eFccList == oexLittleEndian( rc.fccType ) )
			ReadHeadersFromList();

		// Quit if we got headers
		if ( m_bValidHeaders )
			return TRUE;

		// Next chunk
		if ( !SkipChunk() )
			return FALSE;

	} // end while

	return FALSE;
}

BOOL CRiffFile::WriteAviHeaders()
{
	// Set pointer to start of file
	m_file.SetPtrPosBegin( 0 );

	// Write RIFF header
	m_rfh.fccRiff = oexLittleEndian( eFccRiff );
	m_rfh.lFileDataSize = oexLittleEndian( 0 );
	m_rfh.fccType =  oexLittleEndian( eFccAvi );
	if ( !m_file.Write( &m_rfh, sizeof( m_rfh ) ) )
		return FALSE;

	// Write out list block
	LONGLONG llList = m_file.GetPtrPos();
	SRiffChunkEx rcList;
	rcList.fccType = oexLittleEndian( eFccList );
	rcList.lDataSize = oexLittleEndian( 0 );
	rcList.fccSub = oexLittleEndian( eAviHeader );
	if ( !m_file.Write( &rcList, sizeof( rcList ) ) )
		return FALSE;

/*	// Write out riff file header block info
	SRiffChunk rc;
	rc.fccType = oexLittleEndian( eAviMainHeader );
	rc.lDataSize = oexLittleEndian( sizeof( SAviMainHeader ) - 8 );
	if ( !m_file.Write( &rc, sizeof( rc ) ) )
		return FALSE;
*/
	// Ensure header
	if ( !m_amh.Ptr() )
		if ( !m_amh.OexNew( sizeof( SAviMainHeader ) ).Ptr() )
			return FALSE;

	// Zero structure
	oexZeroMemory( m_amh.Ptr(), m_amh.Size() );
	m_amh->fcc = oexLittleEndian( eAviMainHeader );
	m_amh->cb = oexLittleEndian( m_amh.Size() - 8 );

	// Write to file
	m_llAmhOffset = m_file.GetPtrPos();
	if ( !m_file.Write( m_amh.Ptr(), m_amh.Size() ) )
		return FALSE;

	// Stream info block
	SRiffChunkEx rcList2;
	rcList2.fccType = oexLittleEndian( eFccList );
	rcList2.lDataSize = oexLittleEndian( 0 );
	rcList2.fccSub = oexLittleEndian( eAviStreamInfo );
	LONGLONG llList2 = m_file.GetPtrPos();
	if ( !m_file.Write( &rcList2, sizeof( rcList2 ) ) )
		return FALSE;
/*
	rc.fccType = oexLittleEndian( eAviStreamHeader );
	rc.lDataSize = sizeof( SAviStreamHeader );
	if ( !m_file.Write( &rc, sizeof( rc ) ) )
		return FALSE;
*/
	// Ensure header
	if ( !m_ash.Ptr() )
		if ( !m_ash.OexNew( sizeof( SAviStreamHeader ) ).Ptr() )
			return FALSE;

	// Zero structure
	oexZeroMemory( m_ash.Ptr(), m_ash.Size() );
	m_amh->fcc = oexLittleEndian( eAviStreamHeader );
	m_amh->cb = oexLittleEndian( m_ash.Size() - 8 );

	// Write to file
	m_llAshOffset = m_file.GetPtrPos();
	if ( !m_file.Write( m_ash.Ptr(), m_ash.Size() ) )
		return FALSE;

	SRiffChunk rc;
	rc.fccType = oexLittleEndian( eAviStreamFormat );
	rc.lDataSize = sizeof( SWaveFormatEx );
	if ( !m_file.Write( &rc, sizeof( rc ) ) )
		return FALSE;

	// Ensure header
	if ( !m_wfe.Ptr() )
		if ( !m_wfe.OexNew( sizeof( SWaveFormatEx ) ).Ptr() )
			return FALSE;

	// Zero structure
	oexZeroMemory( m_wfe.Ptr(), m_wfe.Size() );

	// Write to file
	m_llWfeOffset = m_file.GetPtrPos();
	if ( !m_file.Write( m_wfe.Ptr(), m_wfe.Size() ) )
		return FALSE;

	LONGLONG llEnd = m_file.GetPtrPos();

	// Update first list size
	rcList.lDataSize = llEnd - llList;
	m_file.SetPtrPosBegin( llList );
	if ( !m_file.Write( &rcList, sizeof( rcList ) ) )
		return FALSE;

	// Update second list size
	rcList2.lDataSize = llEnd - llList2;
	m_file.SetPtrPosBegin( llList2 );
	if ( !m_file.Write( &rcList2, sizeof( rcList2 ) ) )
		return FALSE;

	return TRUE;
}


BOOL CRiffFile::StartStream()
{
	if ( !m_file.IsOpen() )
		return FALSE;

	// Start at the beginning of the file
	m_file.SetPtrPosBegin( sizeof( m_rfh ) );

	// Iterate chunks
	SRiffChunk rc;
	while ( ReadChunkHeader( &rc ) )
	{
		// Look for a LIST chunk
		if ( eFccList == oexLittleEndian( rc.fccType ) )
		{
			oex::CFile::CRestoreFilePos rfp( &m_file );

			// Is this the start of a stream
			SRiffChunk rc;
			if ( m_file.Read( &rc, sizeof( rc ) ) && eFccList == oexLittleEndian( rc.fccType ) )
				if ( m_file.Read( &rc, sizeof( rc ) ) && eAviStream == oexLittleEndian( rc.fccType ) )	
				{	m_llStreamOffset = m_file.GetPtrPos();
					rfp.Cancel();
					return TRUE;
				} // end if

		} // end if

		// Next chunk
		if ( !SkipChunk() )
			return FALSE;

	} // end while

	return FALSE;
}

BOOL CRiffFile::FindIndex()
{
	if ( !m_file.IsOpen() )
		return FALSE;

	// No index
	m_uIndexType = 0;

	// Start at the beginning of the file
	m_file.SetPtrPosBegin( sizeof( m_rfh ) );
	
	// Iterate chunks
	SRiffChunk rc;
	while ( ReadChunkHeader( &rc ) )
	{
		// Index version 1
		if ( eAviIndex == oexLittleEndian( rc.fccType ) )
			m_uIndexType = eAviIndex;

		// Index version 2
		else if ( eAviIndex2 == oexLittleEndian( rc.fccType ) )
			m_uIndexType = 0; // eAviIndex2; // +++ Add support for V2

		// Next chunk
		else if ( !SkipChunk() )
			return FALSE;

		// Did we get an index
		if ( m_uIndexType )
		{
			// Save index size
			m_llIndexSize = rc.lDataSize;

			// Save index position
			m_llIndex = m_file.GetPtrPos() + sizeof( SRiffChunk );

			return TRUE;

		} // end if

	} // end while

	return FALSE;
}

// +++ File mapping would be better, I have to look into the cross
//     platform issues with that.
BOOL CRiffFile::CacheFrame( LONGLONG x_llFrame, BOOL bForward )
{
	if ( !m_file.IsOpen() )
	{	m_memAviIndex.Delete();
		return FALSE;
	} // end if

	// Version 1 index
	if ( eAviIndex != m_uIndexType )
		return FALSE;

	// Is the frame in memory?
	if ( m_memAviIndex.Size() == eIndexCacheSize
		 && x_llFrame >= m_llIndexBase 
		 && x_llFrame < ( m_llIndexBase + eIndexCacheSize ) )
		return TRUE;

	// Going backward?
	if ( !bForward )
	{
		if ( x_llFrame > eIndexCacheSize )
			x_llFrame -= eIndexCacheSize;
		else
			x_llFrame = 0;

	} // end if

	// New base
	m_llIndexBase = x_llFrame;

	// Allocate space for index cache
	if ( m_memAviIndex.Size() != eIndexCacheSize )
		if ( !m_memAviIndex.OexNew( eIndexCacheSize ).Ptr() )
			return FALSE;

	// File offset
	LONGLONG ll = m_llIndex + ( x_llFrame * sizeof( SAviIndex::SAviIndexEntry ) );

	// Offset into index
	if ( !m_file.SetPtrPosBegin( ll ) )
		return FALSE;

	// Read in this portion of the index
	if ( !m_file.Read( m_memAviIndex.Ptr(), eIndexCacheSize * sizeof( SAviIndex::SAviIndexEntry ) ) )
		return FALSE;

	return TRUE;
}

BOOL CRiffFile::GetFrameData( LONGLONG llFrame, LPVOID *pData, LONGLONG *pllSize, BOOL bForward )
{
	SAviIndex::SAviIndexEntry* pAie = GetFrameInfo( llFrame, bForward );
	if ( !pAie )
		return FALSE;

	// Allocate memory for frame data
	if ( m_memFrame.Size() < pAie->dwSize )
		if ( !m_memFrame.OexNew( pAie->dwSize * 2 ).Ptr() )
			return FALSE;

	// Set pointer to start of frame
	if ( !m_file.SetPtrPosBegin( m_llStreamOffset + pAie->dwOffset ) )
		return FALSE;

	// Attempt to read the frame data
	if ( !m_file.Read( m_memFrame.Ptr(), pAie->dwSize ) )
		return FALSE;

	// Remember how many bytes we read
	m_llFrameSize = pAie->dwSize;

	if ( pData )
		*pData = m_memFrame.Ptr();

	if ( pllSize )
		*pllSize = m_llFrameSize;

	return TRUE;
}
