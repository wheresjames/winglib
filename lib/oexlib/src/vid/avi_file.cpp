/*------------------------------------------------------------------
// avi_file.cpp
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

#include "../../oexlib.h"
OEX_USING_NAMESPACE
using namespace OEX_NAMESPACE::vid;

CAviFile::CAviFile()
{
	m_llAmhOffset = 0;
	m_llAshOffset = 0;
	m_llBiOffset = 0;
	m_llWfeOffset = 0;

	m_bValidHeaders = oexFALSE;

	m_llIndex = 0;
	m_uIndexType = 0;
	m_llIndexSize = 0;
	m_llIndexBase = 0;
	m_llIndexFrameOffset = 0;

	m_llNextFrame = 0;
	m_llStreamOffset = 0;

	m_llFrameSize = 0;

	m_bRefreshIndex = oexFALSE;
}

CAviFile::~CAviFile()
{
	Destroy();
}

void CAviFile::Destroy()
{
	// Update the AVI headers if needed
	if ( m_llNextFrame )
		UpdateAviHeaders();

	// Close any open file
	m_file.Close();

	// Release headers
	m_amh.OexDelete();
	m_ash.OexDelete();
	m_bi.OexDelete();
	m_wfe.OexDelete();

	// Lose the index
	m_llIndexBase = 0;
	m_memAviIndex.Delete();

	m_memFrame.Delete();

	m_llAmhOffset = 0;
	m_llAshOffset = 0;
	m_llBiOffset = 0;
	m_llWfeOffset = 0;

	m_bValidHeaders = oexFALSE;

	m_llIndex = 0;
	m_uIndexType = 0;
	m_llIndexSize = 0;
	m_llIndexFrameOffset = 0;

	m_llNextFrame = 0;
	m_llStreamOffset = 0;

	m_llFrameSize = 0;

	m_bRefreshIndex = oexFALSE;
}

oexBOOL CAviFile::Create( oexCSTR pFile )
{
	// Lose current file
	Destroy();

	// Open new file
	if ( !m_file.CreateAlways( pFile ).IsOpen() )
	{	oexTRACE( oexT( " : %lu" ), (unsigned long)m_file.GetLastError() );
		return oexFALSE;
	} // end if

	if ( !WriteAviHeaders() )
	{	oexTRACE( oexT( "%s : Unable to write headers to file" ), __FUNCTION__ );
		return oexFALSE;
	} // end if

	return oexTRUE;
}

oexBOOL CAviFile::Open( oexCSTR pFile, oexBOOL bAllowAppend )
{
	// Lose current file
	Destroy();

	// Attempt to open existing file
	if ( !m_file.OpenExisting( pFile ).IsOpen() )
	{	oexTRACE( oexT( "%s : %lu" ), __FUNCTION__, (unsigned long)m_file.GetLastError() );
		return oexFALSE;
	} // end if

	if ( !m_file.Read( &m_rfh, sizeof( m_rfh ) ) )
	{	oexTRACE( oexT( "%s : %lu" ), __FUNCTION__, (unsigned long)m_file.GetLastError() );
		Destroy();
		return oexFALSE;
	} // end if

	// Verify four cc
	if ( eFccRiff != oexLittleEndian( m_rfh.fccRiff ) )
	{	oexTRACE( oexT( "%s : File does not start with 'RIFF'" ), __FUNCTION__ );
		Destroy();
		return oexFALSE;
	} // end if

	// Verify four cc
	if ( eFccAvi != oexLittleEndian( m_rfh.fccType ) )
	{	oexTRACE( oexT( "%s : RIFF is not an 'AVI ' type" ), __FUNCTION__ );
		Destroy();
		return oexFALSE;
	} // end if

	// Verify four cc
	if ( !ReadAviHeaders() )
	{	oexTRACE( oexT( "%s : Improper headers" ), __FUNCTION__ );
		Destroy();
		return oexFALSE;
	} // end if

	// Find the stream index
	if ( !FindIndex() )
	{	oexTRACE( oexT( "%s : Stream index not found" ), __FUNCTION__ );
		Destroy();
		return oexFALSE;
	} // end if

	// Ensure there is a stream in the file
	if ( !StartStream( 0, bAllowAppend ) )
	{	oexTRACE( oexT( "%s : Data stream not found" ), __FUNCTION__ );
		Destroy();
		return oexFALSE;
	} // end if

	return oexTRUE;
}

oexBOOL CAviFile::ReadChunkHeader( SRiffChunk *pRc )
{
	if ( !m_file.IsOpen() )
		return oexFALSE;

	// Restore file position after read
	oex::CFile::CRestoreFilePos rfp( &m_file );

	// Read chunk header
	return m_file.Read( pRc, sizeof( SRiffChunk ) );
}

oexBOOL CAviFile::SkipChunk()
{																								
	SRiffChunk rc;

	// Read the chunk header
	if ( !m_file.Read( &rc, sizeof( rc ) ) )
		return oexFALSE;

	// Attempt to seek past the end of the file
	m_file.SetPtrPosCur( oexLittleEndian( rc.lDataSize ) );

	return oexTRUE;
}

oexBOOL CAviFile::FindChunk( unsigned long fccType, SRiffChunk *pRc )
{
	SRiffChunk rc;
	if ( !pRc )
		pRc = &rc;

	// Read the chunk header
	while ( ReadChunkHeader( pRc ) )
	{	
		// Is this our guy?
		if ( oexLittleEndian( pRc->fccType ) == fccType )
			return oexTRUE;

		// Next chunk
		if ( !SkipChunk() )
			return oexFALSE;

	} // end while	

	return oexFALSE;
}

/// +++ I totally screwed this up, this won't really work right
///     Unless the file has ONLY one stream.  Please re-work!
oexBOOL CAviFile::ReadHeadersFromList()
{
	if ( !m_file.IsOpen() )
		return oexFALSE;

	SRiffChunk rc;
	if ( !m_file.Read( &rc, sizeof( rc ) ) )
		return oexFALSE;

	// Must at least have a four cc, also, ignore huge blocks
	if ( 4 > oexLittleEndian( rc.lDataSize ) )
		return oexFALSE;

	// Verify it is a list
	if ( eFccList != oexLittleEndian( rc.fccType ) )
		return oexFALSE;

	// Verify AVI header block
	unsigned long fcc = 0;
	if ( !m_file.Read( &fcc, 4 ) || eAviHeader != oexLittleEndian( fcc ) )
		return oexFALSE;

	// Next comes the avi header
	m_llAmhOffset = m_file.GetPtrPos();
	if ( !m_file.Read( &rc, sizeof( rc ) ) || eAviMainHeader != oexLittleEndian( rc.fccType ) )
		return oexFALSE;

	// Allocate space for avi header
	if ( sizeof( SAviMainHeader ) > ( rc.lDataSize + 8 ) || !m_amh.New( rc.lDataSize + 8 ).Ptr() )
		return oexFALSE;

	// Read in the header data
	m_file.SetPtrPosBegin( m_llAmhOffset );
	if ( !m_file.Read( m_amh.Ptr(), rc.lDataSize + 8 ) )
		return oexFALSE;

	// Verify list data containing stream info
	SRiffChunk rcStream;
	if ( !m_file.Read( &rcStream, sizeof( rcStream ) ) || eFccList != oexLittleEndian( rcStream.fccType ) )
		return oexFALSE;

	// Verify stream info block
	if ( !m_file.Read( &fcc, 4 ) || eAviStreamInfo != oexLittleEndian( fcc ) )
		return oexFALSE;

	// Stream header?
	m_llAshOffset = m_file.GetPtrPos();
	if ( !m_file.Read( &rc, sizeof( rc ) ) || eAviStreamHeader != oexLittleEndian( rc.fccType ) )
		return oexFALSE;

	// Allocate space for avi header
	if ( sizeof( SAviStreamHeader ) > ( rc.lDataSize + 8 ) || !m_ash.New( rc.lDataSize + 8 ).Ptr() )
		return oexFALSE;

	// Read the stream header
	m_file.SetPtrPosBegin( m_llAshOffset );
	if ( !m_file.Read( m_ash.Ptr(), rc.lDataSize + 8 ) )
		return oexFALSE;

	// Stream format info should be next
	if ( !m_file.Read( &rc, sizeof( rc ) ) || eAviStreamFormat != oexLittleEndian( rc.fccType ) )
		return oexFALSE;

	// Check for video stream
	if ( SAviStreamHeader::eAshStreamTypeVideo == m_ash.Ptr()->fccType )
	{
		// Allocate space for audio format information
		if ( sizeof( SBitmapInfo::SBitmapInfoHeader ) > rc.lDataSize || !m_bi.OexNew( rc.lDataSize ).Ptr() )
			return oexFALSE;

		// Read in the audio data
		m_llBiOffset = m_file.GetPtrPos();
		if ( !m_file.Read( m_bi.Ptr(), rc.lDataSize ) )
			return oexFALSE;
	} // end if

	// Check for audio stream
	else if ( SAviStreamHeader::eAshStreamTypeAudio == m_ash.Ptr()->fccType )
	{
		// Allocate space for audio format information
		if ( sizeof( SWaveFormatEx ) > rc.lDataSize || !m_wfe.OexNew( rc.lDataSize ).Ptr() )
			return oexFALSE;

		// Read in the audio data
		m_llWfeOffset = m_file.GetPtrPos();
		if ( !m_file.Read( m_wfe.Ptr(), rc.lDataSize ) )
			return oexFALSE;

	} // end else if

	else
		return oexFALSE;

	// We have valid headers
	m_bValidHeaders = oexTRUE;

	return oexTRUE;
}

oexBOOL CAviFile::ReadAviHeaders()
{
	if ( !m_file.IsOpen() )
		return oexFALSE;

	// Restore file position after this
	oex::CFile::CRestoreFilePos rfp( &m_file );

	// Forget the old headers
	m_bValidHeaders = oexFALSE;

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
			return oexTRUE;

		// Next chunk
		if ( !SkipChunk() )
			return oexFALSE;

	} // end while

	return oexFALSE;
}

oexBOOL CAviFile::UpdateAviHeaders()
{
	// Refresh the index
	WriteIndex();

	// Calculate values
	Amh()->dwMaxBytesPerSec = (oexUINT)( GetFrameRate() * Amh()->dwSuggestedBufferSize );

	// Update main avi header
	if ( m_amh.Ptr() && m_llAmhOffset )
	{	m_file.SetPtrPosBegin( m_llAmhOffset );
		if ( !m_file.Write( m_amh.Ptr(), m_amh.Size() ) )
			return oexFALSE;
	} // end if

	// Update AVI stream header
	if ( m_ash.Ptr() && m_llAshOffset )
	{	m_file.SetPtrPosBegin( m_llAshOffset );
		if ( !m_file.Write( m_ash.Ptr(), m_ash.Size() ) )
			return oexFALSE;
	} // end if

	// Update AVI format header
	if ( m_bi.Ptr() && m_llBiOffset )
	{	m_file.SetPtrPosBegin( m_llBiOffset );
		if ( !m_file.Write( m_bi.Ptr(), m_bi.Size() ) )
			return oexFALSE;
	} // end if

	// Update AVI format header
	if ( m_wfe.Ptr() && m_llWfeOffset )
	{	m_file.SetPtrPosBegin( m_llWfeOffset );
		if ( !m_file.Write( m_wfe.Ptr(), m_wfe.Size() ) )
			return oexFALSE;
	} // end if

	// Set pointer to start of file
	m_file.SetPtrPosEnd( 0 );
	oexINT64 llEnd = m_file.GetPtrPos();

	// Write RIFF header
	m_rfh.fccRiff = oexLittleEndian( eFccRiff );
	m_rfh.lFileDataSize = oexLittleEndian( (oexUINT)llEnd - 8 );
	m_rfh.fccType =  oexLittleEndian( eFccAvi );

	m_file.SetPtrPosBegin( 0 );
	if ( !m_file.Write( &m_rfh, sizeof( m_rfh ) ) )
		return oexFALSE;

	return oexTRUE;
}

oexBOOL CAviFile::WriteAviHeaders()
{
	// Set pointer to start of file
	m_file.SetPtrPosBegin( 0 );

	// Write RIFF header
	m_rfh.fccRiff = oexLittleEndian( eFccRiff );
	m_rfh.lFileDataSize = oexLittleEndian( 0 );
	m_rfh.fccType =  oexLittleEndian( eFccAvi );
	if ( !m_file.Write( &m_rfh, sizeof( m_rfh ) ) )
		return oexFALSE;

	// Write out list block
	oexINT64 llList = m_file.GetPtrPos();
	SRiffChunkEx rcList;
	rcList.fccType = oexLittleEndian( eFccList );
	rcList.lDataSize = oexLittleEndian( 0 );
	rcList.fccSub = oexLittleEndian( eAviHeader );
	if ( !m_file.Write( &rcList, sizeof( rcList ) ) )
		return oexFALSE;

	// Ensure header
	if ( !Amh() )
		return oexFALSE;

	// Zero structure
	m_amh->fcc = oexLittleEndian( eAviMainHeader );
	m_amh->cb = oexLittleEndian( m_amh.Size() - 8 );

	// Write to file
	m_llAmhOffset = m_file.GetPtrPos();
	if ( !m_file.Write( m_amh.Ptr(), m_amh.Size() ) )
		return oexFALSE;

	// Stream info block
	SRiffChunkEx rcList2;
	rcList2.fccType = oexLittleEndian( eFccList );
	rcList2.lDataSize = oexLittleEndian( 0 );
	rcList2.fccSub = oexLittleEndian( eAviStreamInfo );
	oexINT64 llList2 = m_file.GetPtrPos();
	if ( !m_file.Write( &rcList2, sizeof( rcList2 ) ) )
		return oexFALSE;

	// Ensure header
	if ( !Ash() )
		return oexFALSE;

	// Fill in info
	m_ash->fcc = oexLittleEndian( eAviStreamHeader );
	m_ash->cb = oexLittleEndian( m_ash.Size() - 8 );	
	m_ash->fccType = SAviStreamHeader::eAshStreamTypeVideo;

	// Write to file
	m_llAshOffset = m_file.GetPtrPos();
	if ( !m_file.Write( m_ash.Ptr(), m_ash.Size() ) )
		return oexFALSE;

	SRiffChunk rc;
	rc.fccType = oexLittleEndian( eAviStreamFormat );
	rc.lDataSize = sizeof( SBitmapInfo::SBitmapInfoHeader );
	if ( !m_file.Write( &rc, sizeof( rc ) ) )
		return oexFALSE;

	// Ensure header
	if ( !Bi() )
		return oexFALSE;

	// Set size
	m_bi->bmiHeader.biSize = sizeof( SBitmapInfo::SBitmapInfoHeader );

	// Write to file
	m_llBiOffset = m_file.GetPtrPos();
	if ( !m_file.Write( m_bi.Ptr(), m_bi.Size() ) )
		return oexFALSE;

	SRiffChunkEx rcStream;
	rcStream.fccType = oexLittleEndian( eFccList );
	rcStream.lDataSize = oexLittleEndian( 0 );
	rcStream.fccSub = oexLittleEndian( eAviStream );

	if ( !m_file.Write( &rcStream, sizeof( rcStream ) ) )
		return oexFALSE;

	// Save start of stream data
	m_llStreamOffset = m_file.GetPtrPos();
	m_llNextFrame = m_llStreamOffset;

	// End of the header data
	oexINT64 llEnd = m_llStreamOffset - sizeof( rcStream );

	// Update first list size
	rcList.lDataSize = (oexUINT)( llEnd - llList - 8 );
	m_file.SetPtrPosBegin( llList );
	if ( !m_file.Write( &rcList, sizeof( rcList ) ) )
		return oexFALSE;

	// Update second list size
	rcList2.lDataSize = (oexUINT)( llEnd - llList2 - 8 );
	m_file.SetPtrPosBegin( llList2 );
	if ( !m_file.Write( &rcList2, sizeof( rcList2 ) ) )
		return oexFALSE;

	return oexTRUE;
}

oexBOOL CAviFile::WriteIndex()
{
	if ( !m_bRefreshIndex )
		return oexTRUE;

	// Must have a file
	if ( !m_file.IsOpen() )
		return oexFALSE;

	// Find the offset
	if ( !m_llStreamOffset )
		StartStream( 0, oexTRUE );

	// Do we have an append point?
	if ( !m_llNextFrame )
		return oexFALSE;

	// Update the stream header
	SRiffChunkEx rcStream;
	rcStream.fccType = oexLittleEndian( eFccList );
	rcStream.lDataSize = oexLittleEndian( (oexUINT)( m_llNextFrame - m_llStreamOffset + 4 ) );
	rcStream.fccSub = oexLittleEndian( eAviStream );

	// Update the position
	m_file.SetPtrPosBegin( m_llStreamOffset - sizeof( rcStream ) );
	if ( !m_file.Write( &rcStream, sizeof( rcStream ) ) )
		return oexFALSE;
	
	// Allocate memory for index data
	if ( m_memAviIndex.Size() != eIndexCacheSize )
		if ( !m_memAviIndex.OexNew( eIndexCacheSize ).Ptr() )
			return oexFALSE;

	// Start the AVI index
	SRiffChunk rc;
	rc.fccType = oexLittleEndian( eAviIndex );
	rc.lDataSize = oexLittleEndian( 0 );
	m_file.SetPtrPosBegin( m_llNextFrame );
	if ( !m_file.Write( &rc, sizeof( rc ) ) )
		return oexFALSE;

	oexUINT uFrame = 0;
	oexUINT uTotalFrames = 0;
	oexINT64 llIdxPos = m_file.GetPtrPos();
	oexINT64 llPos = m_llStreamOffset;
	oexINT64 llMaxSize = 0;

	// For each frame
	SRiffChunk rcFrame;
	while ( llPos < m_llNextFrame )
	{
		// Count a frame
		uTotalFrames++;

		// Read this frame data
		m_file.SetPtrPosBegin( llPos );
		if ( !m_file.Read( &rcFrame, sizeof( rcFrame ) ) )
			return oexFALSE;

		// Fill in index data
		m_memAviIndex.Ptr( uFrame )->dwChunkId	= oexLittleEndian( rcFrame.fccType );
		m_memAviIndex.Ptr( uFrame )->dwFlags	= oexLittleEndian( SAviIndexEntry::eFlagKeyFrame );
		m_memAviIndex.Ptr( uFrame )->dwOffset	= oexLittleEndian( (oexUINT)( llPos - m_llStreamOffset + 4 ) );
		m_memAviIndex.Ptr( uFrame )->dwSize		= oexLittleEndian( rcFrame.lDataSize );

		if ( rcFrame.lDataSize > llMaxSize )
			llMaxSize = rcFrame.lDataSize;

		// Point to next frame
		llPos += sizeof( rcFrame ) + rcFrame.lDataSize + ( 1 & rcFrame.lDataSize );

		// Update index if cache is full
		if ( ++uFrame >= eIndexCacheSize )
		{
			m_file.SetPtrPosBegin( llIdxPos );
			if ( !m_file.Write( m_memAviIndex.Ptr(), m_memAviIndex.SizeInBytes() ) )
				return oexFALSE;

			uFrame = 0;
			llIdxPos += m_memAviIndex.SizeInBytes();

		} // end if

	} // end while

	// Write left over frames in the index
	if ( uFrame )
	{	m_file.SetPtrPosBegin( llIdxPos );
		if ( !m_file.Write( m_memAviIndex.Ptr(), m_memAviIndex.Size() ) )
			return oexFALSE;
	} // end if

	rc.fccType = oexLittleEndian( eAviIndex );
	rc.lDataSize = oexLittleEndian( (oexUINT)( m_file.GetPtrPos() - m_llNextFrame - sizeof( rc ) ) );
	m_file.SetPtrPosBegin( m_llNextFrame );
	if ( !m_file.Write( &rc, sizeof( rc ) ) )
		return oexFALSE;

	// Update header values
	Amh()->dwTotalFrames = uTotalFrames;
	Amh()->dwFlags |= SAviMainHeader::eAmhHasIndex;
	Amh()->dwSuggestedBufferSize = oexLittleEndian( (oexUINT)llMaxSize );	

	Ash()->dwLength = uTotalFrames;
	Ash()->dwSuggestedBufferSize = oexLittleEndian( (oexUINT)llMaxSize );

	m_bRefreshIndex = oexFALSE;

	return oexTRUE;
}

oexBOOL CAviFile::AddFrame( oexUINT x_uType, oexUINT x_uStream, oexCPVOID x_pData, oexUINT x_uSize )
{
	// Ensure valid pointers
	if ( !x_pData || !x_uSize )
		return oexFALSE;

	// Must have a file
	if ( !m_file.IsOpen() )
		return oexFALSE;

	// Do we have a next frame pointer
	if ( !m_llStreamOffset || !m_llNextFrame )
		return oexFALSE;

	SRiffChunk rc;
	rc.fccType = x_uType;
	rc.lDataSize = x_uSize;

	if ( x_uStream )
	{	oex::CStr sStream( oex::CStr().Fmt( "%lu", x_uStream ) );
		rc.chFccType[ 0 ] = sStream[ 0 ];
		rc.chFccType[ 1 ] = sStream[ 1 ];
	} // end if

	// Start of frame
	m_file.SetPtrPosBegin( m_llNextFrame );

	// Write the frame header
	if ( !m_file.Write( &rc, sizeof( rc ) ) )
		return oexFALSE;

	// Write the frame data
	if ( !m_file.Write( x_pData, x_uSize ) )
		return oexFALSE;

	// Pad to word boundry
	if ( 1 & x_uSize )
		if ( !m_file.Write( &x_uSize, 1 ) )
			return oexFALSE;

	// Remeber where the next frame goes
	m_llNextFrame += sizeof( rc ) + x_uSize + ( 1 & x_uSize );

	// We must update the index
	m_bRefreshIndex = oexTRUE;

	return oexTRUE;
}

oexBOOL CAviFile::StartStream( oexUINT uStream, oexBOOL bAllowAppend )
{
	// Lose stream info
	m_llStreamOffset = m_llNextFrame = 0;

	if ( !m_file.IsOpen() )
		return oexFALSE;

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
					if ( bAllowAppend )
						m_llNextFrame = m_llStreamOffset + rc.lDataSize;
					rfp.Cancel();
					return oexTRUE;
				} // end if

		} // end if

		// Next chunk
		if ( !SkipChunk() )
			return oexFALSE;

	} // end while

	return oexFALSE;
}

oexBOOL CAviFile::FindIndex()
{
	if ( !m_file.IsOpen() )
		return oexFALSE;

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
			return oexFALSE;

		// Did we get an index
		if ( m_uIndexType )
		{
			// Save index size
			m_llIndexSize = rc.lDataSize;

			// Save index position
			m_llIndex = m_file.GetPtrPos() + sizeof( SRiffChunk );

			SAviIndexEntry aie;
			m_file.SetPtrPosBegin( m_llIndex );
			while ( m_file.Read( &aie, sizeof( aie ) ) )
			{
				// Acceptable type?
				if ( ( 0xffff0000 & eFtUncompressedVideo ) == ( 0xffff0000 & aie.dwChunkId )
					 || ( 0xffff0000 & eFtCompressedVideo ) == ( 0xffff0000 & aie.dwChunkId )
					 || ( 0xffff0000 & eFtPaletteChange ) == ( 0xffff0000 & aie.dwChunkId )
					 || ( 0xffff0000 & eFtAudioData ) == ( 0xffff0000 & aie.dwChunkId ) )
					return oexTRUE;

				m_llIndexFrameOffset++;

			} // end while

			return oexTRUE;

		} // end if

	} // end while

	return oexFALSE;
}

// +++ File mapping would be better, I have to look into the cross
//     platform issues with that.
oexBOOL CAviFile::CacheFrame( oexINT64 x_llFrame, oexBOOL bForward )
{
	if ( !m_file.IsOpen() )
		return oexFALSE;

	// Version 1 index
	if ( eAviIndex != m_uIndexType )
		return oexFALSE;

	// Is the frame in memory?
	if ( m_memAviIndex.Size() == eIndexCacheSize
		 && x_llFrame >= m_llIndexBase 
		 && x_llFrame < ( m_llIndexBase + eIndexCacheSize ) )
		return oexTRUE;

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
			return oexFALSE;

	// File offset
	oexINT64 ll = ( m_llIndex + ( m_llIndexFrameOffset * sizeof( SAviIndexEntry ) ) ) 
		          + ( x_llFrame * sizeof( SAviIndexEntry ) );

	// Offset into index
	if ( !m_file.SetPtrPosBegin( ll ) )
		return oexFALSE;

	// Read in this portion of the index
	if ( !m_file.Read( m_memAviIndex.Ptr(), eIndexCacheSize * sizeof( SAviIndexEntry ) ) )
		return oexFALSE;

	return oexTRUE;
}

oexBOOL CAviFile::GetFrameData( oexINT64 llFrame, oexPVOID *pData, oexINT64 *pllSize, oexBOOL bForward )
{
	SAviIndexEntry* pAie = GetFrameInfo( llFrame, bForward );
	if ( !pAie || !pAie->dwSize )
		return oexFALSE;

	// Allocate memory for frame data
	if ( m_memFrame.Size() < pAie->dwSize )
		if ( !m_memFrame.OexNew( pAie->dwSize * 2 ).Ptr() )
			return oexFALSE;

	// Set pointer to start of frame
	if ( !m_file.SetPtrPosBegin( m_llStreamOffset + pAie->dwOffset ) )
		return oexFALSE;

	// Attempt to read the frame data
	if ( !m_file.Read( m_memFrame.Ptr(), pAie->dwSize ) )
		return oexFALSE;

	// Remember how many bytes we read
	m_llFrameSize = pAie->dwSize;

	if ( pData )
		*pData = m_memFrame.Ptr();

	if ( pllSize )
		*pllSize = m_llFrameSize;

	return oexTRUE;
}
