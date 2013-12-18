/*------------------------------------------------------------------
// avi_file.h
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

#pragma once

// Creates a four cc from characters
/*
#define MAKE_FOURCC( str )  (													  \
								  ( ( (unsigned long)(str) & 0x000000ff ) << 24 ) \
								| ( ( (unsigned long)(str) & 0x0000ff00 ) <<  8 ) \
								| ( ( (unsigned long)(str) & 0x00ff0000 ) >>  8 ) \
								| ( ( (unsigned long)(str) & 0xff000000 ) >> 24 ) \
							)
*/
#define MAKE_FOURCC( a, b, c, d )  (													  \
								  ( ( (unsigned long)a ) << 24 ) \
								| ( ( (unsigned long)b ) <<  8 ) \
								| ( ( (unsigned long)c ) >>  8 ) \
								| ( ( (unsigned long)d ) >> 24 ) \
							)


/// CAviFile - Reading and writting RIFF (or avi) files
/**

*/
class CAviFile
{
public:

	enum
	{
		/// How many frames to cache from the index
		eIndexCacheSize		= 128
	};

	enum
	{
		eFccRiff				= MAKE_FOURCC( 'R', 'I', 'F', 'F' ),
		eFccAvi					= MAKE_FOURCC( 'A', 'V', 'I', ' ' ),
		eFccList				= MAKE_FOURCC( 'L', 'I', 'S', 'T' ),

		eAviHeader				= MAKE_FOURCC( 'h', 'd', 'r', 'l' ),
		eAviMainHeader			= MAKE_FOURCC( 'a', 'v', 'i', 'h' ),
		eAviStreamInfo			= MAKE_FOURCC( 's', 't', 'r', 'l' ),
		eAviStreamHeader		= MAKE_FOURCC( 's', 't', 'r', 'h' ),
		eAviStreamFormat		= MAKE_FOURCC( 's', 't', 'r', 'f' ),
		eAviExtraHeaderData		= MAKE_FOURCC( 's', 't', 'r', 'd' ),
		eAviStreamName			= MAKE_FOURCC( 's', 't', 'r', 'n' ),

		eAviStream				= MAKE_FOURCC( 'm', 'o', 'v', 'i' ),

		/// Version 1 AVI index
		eAviIndex				= MAKE_FOURCC( 'i', 'd', 'x', '1' ),

		/// Version 2 AVI index
		eAviIndex2				= MAKE_FOURCC( 'i', 'n', 'd', 'x' ),
	};

	enum
	{
		eFtUncompressedVideo	= MAKE_FOURCC( '0', '0', 'd', 'b' ),
		eFtCompressedVideo		= MAKE_FOURCC( '0', '0', 'd', 'c' ),
		eFtPaletteChange		= MAKE_FOURCC( '0', '0', 'p', 'c' ),
		eFtAudioData			= MAKE_FOURCC( '0', '0', 'w', 'b' )
	};

// AVI structures are packed on word bounderies
#ifndef OEX_NOPACK
#	pragma pack( push, 2 )
#endif

	/// Riff file header
	struct SRiffFileHeader
	{
		/// Must be set to RIFF
		union
		{
			unsigned long		fccRiff;
			char				chFccRiff[ 4 ];
		};

		/// Size of the data in the file
		unsigned long			lFileDataSize;

		/// The file type / 'AVI ' for avi file
		unsigned long			fccType;
	};

	/// RIFF chunk header
	struct SRiffChunk
	{
		/// Chunk type
		union
		{
			unsigned long		fccType;
			char				chFccType[ 4 ];
		};

		/// Size of the data in the chunk
		unsigned long			lDataSize;
	};

	/// RIFF chunk header
	struct SRiffChunkEx
	{
		/// Chunk type
		union
		{
			unsigned long		fccType;
			char				chFccType[ 4 ];
		};

		/// Size of the data in the chunk
		unsigned long			lDataSize;

		/// Chunk type
		union
		{
			unsigned long		fccSub;
			char				chFccSub[ 4 ];
		};
	};

	struct SAviMainHeader
	{
		enum
		{
			eAmhHasIndex		= 0x00000010,
			eAmhMustUseIndex	= 0x00000020,
			eAmhIsInterleaved	= 0x00000100,
			eAmhTrustCkType		= 0x00000800,
			eAmhWasCaptureFile	= 0x00010000,
			eAmhCopyrighted		= 0x00020000
		};

		/// Must be 'avih'
		union
		{
			unsigned long		fcc;
			char				chFcc[ 4 ];
		};

		unsigned long			cb;
		unsigned long			dwMicroSecPerFrame;
		unsigned long			dwMaxBytesPerSec;
		unsigned long			dwPaddingGranularity;
		unsigned long			dwFlags;
		unsigned long			dwTotalFrames;
		unsigned long			dwInitialFrames;
		unsigned long			dwStreams;
		unsigned long			dwSuggestedBufferSize;
		unsigned long			dwWidth;
		unsigned long			dwHeight;
		unsigned long			dwReserved[ 4 ];

    };

	struct SAviStreamHeader
	{
		enum
		{
			eAshStreamTypeVideo		= MAKE_FOURCC( 'v', 'i', 'd', 's' ),
			eAshStreamTypeAudio		= MAKE_FOURCC( 'a', 'u', 'd', 's' ),
			eAshStreamTypeMidi		= MAKE_FOURCC( 'm', 'i', 'd', 's' ),
			eAshStreamTypeText		= MAKE_FOURCC( 't', 'x', 't', 's' )
		};

		enum
		{
			eAshFlagDisabled		= 0x00000001,
			eAshFlagVideoPalChanges = 0x00010000
		};

		union
		{
			unsigned long		fcc;
			char				chFcc[ 4 ];
		};
		unsigned long			cb;

		union
		{
			unsigned long		fccType;
			char				chFccType[ 4 ];
		};
		union
		{
			unsigned long		fccHandler;
			char				chFccHandler[ 4 ];
		};

		unsigned long			dwFlags;

		unsigned short			wPriority;
		unsigned short			wLanguage;
		unsigned long			dwInitialFrames;
		unsigned long			dwScale;
		unsigned long			dwRate;
		unsigned long			dwStart;
		unsigned long			dwLength;
		unsigned long			dwSuggestedBufferSize;
		unsigned long			dwQuality;
		unsigned long			dwSampleSize;

		struct 
		{
			short				left;
			short				top;
			short				right;
			short				bottom;

		} rcFrame;
	};

	/// Structure of a video frame
	struct SBitmapInfo
	{
		// *** Bitmap header info
		struct SBitmapInfoHeader
		{
			unsigned long		biSize; 
			long				biWidth; 
			long				biHeight; 
			unsigned short		biPlanes; 
			unsigned short		biBitCount; 
			unsigned long		biCompression; 
			unsigned long		biSizeImage; 
			long				biXPelsPerMeter; 
			long				biYPelsPerMeter; 
			unsigned long		biClrUsed; 
			unsigned long		biClrImportant; 

		} bmiHeader;

		// *** Color information
		unsigned long			bmiColors[ 1 ];		// Just know it's there
	};

	/// The wave format structure
	struct SWaveFormatEx
	{
		unsigned short			wFormatTag;
		unsigned short			nChannels;
		unsigned long			nSamplesPerSec;
		unsigned long			nAvgBytesPerSec;
		unsigned short			nBlockAlign;
		unsigned short			wBitsPerSample;
		unsigned short			cbSize;
	};


	/// Format of version 1 avi index entry
	struct SAviIndex 
	{
		union
		{
			unsigned long		fcc;
			char				chFcc[ 4 ];
		};
		unsigned long			cb;

	};

	struct SAviIndexEntry 
	{
		enum
		{
			eFlagList		= 0x00000001,
			eFlagKeyFrame	= 0x00000010,
			eFlagNoTime		= 0x00000100,
			eFlagCompressor = 0x0fff0000
		};

		unsigned long			dwChunkId;
		unsigned long			dwFlags;
		unsigned long			dwOffset;
		unsigned long			dwSize;
	};

#ifndef OEX_NOPACK
#	pragma pack( pop )
#endif

public:

	/// Default constructor
	CAviFile();
	
	/// Destructor
	~CAviFile();

	/// Closes open file and releases resources
	void Destroy();

	/// Creates a new file
	oexBOOL Create( oexCSTR pFile );

	/// Opens an existing file
	oexBOOL Open( oexCSTR pFile, oexBOOL bAllowAppend );

	/// Reads in a chunk header and returns the file pointer to the start of the chunk
	oexBOOL ReadChunkHeader( SRiffChunk *pRc );

	/// Skips the current chunk
	oexBOOL SkipChunk();

	/// Skips chunks looking for a matching fcc
	oexBOOL FindChunk( unsigned long fccFcc, SRiffChunk *pRc );

	/// Reads in the avi file headers
	oexBOOL ReadAviHeaders();

	/// Read headers from list
	oexBOOL ReadHeadersFromList();

	/// Writes headers to file
	oexBOOL WriteAviHeaders();

	/// Updates the avi headers
	oexBOOL UpdateAviHeaders();

	/// Locates the start of the data stream
	oexBOOL StartStream( oexUINT uStream, oexBOOL bAllowAppend );

	/// Locates the index within the file
	oexBOOL FindIndex();

	/// Rebases the memory index cache to include the specified frame
	/**
		\param [in] x_llFrame	- The new index base for the cache
		\param [in] x_bForward	- If non-zero, frames including and
								  after x_llFrame will be cached.  If
								  zero, x_llFrame and preceding frames
								  will be cached.
	*/
	oexBOOL CacheFrame( oexINT64 x_llFrame, oexBOOL x_bForward = oexTRUE );

	/// Returns information about the specified frame.
	/**
		\param [in] x_llFrame	- The new index base for the cache
		\param [in] x_bForward	- If non-zero, frames including and
								  after x_llFrame will be cached.  If
								  zero, x_llFrame and preceding frames
								  will be cached.

		\return Pointer to SAviIndex structure if frame info
		        is available, otherwise zero.
	*/
	SAviIndexEntry* GetFrameInfo( oexINT64 llFrame, oexBOOL bForward = oexTRUE )
	{	if ( !CacheFrame( llFrame, bForward ) )
			return (SAviIndexEntry*)0;
		oexASSERT( llFrame >= m_llIndexBase && llFrame < ( m_llIndexBase + eIndexCacheSize ) );
		return m_memAviIndex.Ptr( (oexUINT)( llFrame - m_llIndexBase ) );
	}

	/// Returns the data for the specified frame
	oexBOOL GetFrameData( oexINT64 llFrame, oexPVOID *pData, oexINT64 *pllSize, oexBOOL bForward = oexTRUE );

	/// Writes a new index into the file
	oexBOOL WriteIndex();

	template < typename T >
		oexBOOL AddFrame( oexUINT x_uType, oexUINT x_uStream, oex::TStr< T > &x_sMem )
		{	return AddFrame( x_uType, x_uStream, x_sMem.Ptr(), x_sMem.LengthInBytes() ); }

	/// Adds a video frame to the AVI
	oexBOOL AddFrame( oexUINT x_uType, oexUINT x_uStream, oexCPVOID x_pData, oexUINT x_uSize );

	/// Returns a pointer to the AVI main header
	SAviMainHeader* Amh()
	{	if ( !m_amh.Ptr() )
		{	if ( !oexVERIFY( m_amh.OexNew( sizeof( SAviMainHeader ) ).Ptr() ) )
				return oexNULL;
			else
				oexZeroMemory( m_amh.Ptr(), m_amh.Size() );
		} // end if
		return m_amh.Ptr();
	}

	/// Returns a pointer to the AVI stream header
	SAviStreamHeader* Ash()
	{	if ( !m_ash.Ptr() )
		{	if ( !oexVERIFY( m_ash.OexNew( sizeof( SAviStreamHeader ) ).Ptr() ) )
				return oexNULL;
			else
				oexZeroMemory( m_ash.Ptr(), m_ash.Size() );
		} // end if
		return m_ash.Ptr();
	}

	/// Returns a pointer to the AVI bitmap info structure
	SBitmapInfo* Bi()
	{	if ( !m_bi.Ptr() )
		{	if ( !oexVERIFY( m_bi.OexNew( sizeof( SBitmapInfo::SBitmapInfoHeader ) ).Ptr() ) )
				return oexNULL;
			else
				oexZeroMemory( m_bi.Ptr(), m_bi.Size() );
		} // end if
		return m_bi.Ptr();
	}

	/// Returns a pointer to the AVI stream header
	SWaveFormatEx* Wfe()
	{	if ( !m_wfe.Ptr() )
		{	if ( !oexVERIFY( m_wfe.OexNew( sizeof( SWaveFormatEx ) ).Ptr() ) )
				return oexNULL;
			else
				oexZeroMemory( m_wfe.Ptr(), m_wfe.Size() );
		} // end if
		return m_wfe.Ptr();
	}

	/// Sets the AVI frame rate
	/**
		\param [in] fRate	- Frame rate in frame per second
	*/
	void SetFrameRate( float fRate )
	{	Ash()->dwRate = oexLittleEndian( 1000000 );
		Ash()->dwScale = oexLittleEndian( (oexUINT)( (float)Ash()->dwRate / fRate ) );
		Amh()->dwMicroSecPerFrame = Ash()->dwScale;
	}

	/// Returns the frame rate in frames per second
	float GetFrameRate()
	{	return (float)Ash()->dwRate / (float)Ash()->dwScale;
	}

	/// Returns the last file error
	oexUINT GetLastError()
	{	return m_file.GetLastError(); }

	/// Returns non-zero if an AVI file is open
	oexBOOL IsOpen()
	{	return m_file.IsOpen(); }

private:

	/// File handle
	oex::CFile								m_file;

	/// Riff file header
	SRiffFileHeader							m_rfh;

	/// Type of the index
	oexUINT									m_uIndexType;

	/// Offset of the start of the index
	oexINT64								m_llIndex;

	/// Size of the index chunk
	oexINT64								m_llIndexSize;

	/// Offset to start of data stream
	oexINT64								m_llStreamOffset;

	/// Offset to next frame
	oexINT64								m_llNextFrame;

	/// Offset into the index of the first frame
	oexINT64								m_llIndexFrameOffset;

	/// Set to non-zero if header structures are valid
	oexBOOL									m_bValidHeaders;
	
	/// File offset of main header
	oexINT64								m_llAmhOffset;

	/// AVI main header
	oex::TMem< char, SAviMainHeader >		m_amh;

	/// File offset of stream header
	oexINT64								m_llAshOffset;

	/// Stream header
	oex::TMem< char, SAviStreamHeader >		m_ash;

	/// File offset of audio stream header
	oexINT64								m_llBiOffset;

	/// Audio stream header
	oex::TMem< char, SBitmapInfo >			m_bi;

	/// File offset of audio stream header
	oexINT64								m_llWfeOffset;

	/// Audio stream header
	oex::TMem< char, SWaveFormatEx >		m_wfe;

	/// Index base
	oexINT64								m_llIndexBase;

	/// Index version 1
	oex::TMem< SAviIndexEntry >				m_memAviIndex;

	/// Size of the data in m_memFrame
	oexINT64								m_llFrameSize;

	/// Holds data for a single frame
	oex::TMem< oexCHAR >					m_memFrame;

	/// Set when the index needs refreshing
	oexBOOL									m_bRefreshIndex;

};
