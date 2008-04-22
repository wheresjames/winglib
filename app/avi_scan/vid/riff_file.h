
#pragma once

// Creates a four cc from characters
#define MAKE_FOURCC( str )  (													  \
								  ( ( (unsigned long)(str) & 0x000000ff ) << 24 ) \
								| ( ( (unsigned long)(str) & 0x0000ff00 ) <<  8 ) \
								| ( ( (unsigned long)(str) & 0x00ff0000 ) >>  8 ) \
								| ( ( (unsigned long)(str) & 0xff000000 ) >> 24 ) \
							)


/// CRiffFile - Reading and writting RIFF (or avi) files
/**
	heres the general layout of an AVI riff file (new format)

	RIFF (3F??????) AVI       <- not more than 1 GB in size
		LIST (size) hdrl
			avih (0038)
			LIST (size) strl
				strh (0038)
				strf (????)
				indx (3ff8)   <- size may vary, should be sector sized
			LIST (size) strl
				strh (0038)
				strf (????)
				indx (3ff8)   <- size may vary, should be sector sized
			LIST (size) odml
				dmlh (????)
			JUNK (size)       <- fill to align to sector - 12
		LIST (7f??????) movi  <- aligned on sector - 12
			00dc (size)       <- sector aligned
			01wb (size)       <- sector aligned
			ix00 (size)       <- sector aligned
		idx1 (00??????)       <- sector aligned
	RIFF (7F??????) AVIX
		JUNK (size)           <- fill to align to sector -12
		LIST (size) movi
			00dc (size)       <- sector aligned
	RIFF (7F??????) AVIX      <- not more than 2GB in size
		JUNK (size)           <- fill to align to sector - 12
		LIST (size) movi
			00dc (size)       <- sector aligned
*/
class CRiffFile
{
public:

	enum
	{
		eFccRiff			= MAKE_FOURCC( 'RIFF' ),
		eFccAvi				= MAKE_FOURCC( 'AVI ' ),
		eFccList			= MAKE_FOURCC( 'LIST' ),

		eAviHeader			= MAKE_FOURCC( 'hdrl' ),
		eAviMainHeader		= MAKE_FOURCC( 'avih' ),
		eAviStreamInfo		= MAKE_FOURCC( 'strl' ),
		eAviStreamHeader	= MAKE_FOURCC( 'strh' ),
		eAviStreamFormat	= MAKE_FOURCC( 'strf' ),
		eAviExtraHeaderData	= MAKE_FOURCC( 'strd' ),
		eAviStreamName		= MAKE_FOURCC( 'strn' ),

		eAviIndex			= MAKE_FOURCC( 'idx1' ),
	};

// AVI structures are packed on word bounderies
#pragma pack( push, 2 )

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
		unsigned long		lFileDataSize;

		/// The file type / 'AVI ' for avi file
		unsigned long		fccType;
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
		unsigned long		lDataSize;
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
		unsigned long		cb;
		unsigned long		dwMicroSecPerFrame;
		unsigned long		dwMaxBytesPerSec;
		unsigned long		dwPaddingGranularity;
		unsigned long		dwFlags;
		unsigned long		dwTotalFrames;
		unsigned long		dwInitialFrames;
		unsigned long		dwStreams;
		unsigned long		dwSuggestedBufferSize;
		unsigned long		dwWidth;
		unsigned long		dwHeight;
		unsigned long		dwReserved[ 4 ];

    };

	struct SAviStreamHeader
	{
		enum
		{
			eAshStreamTypeVideo		= MAKE_FOURCC( 'vids' ),
			eAshStreamTypeAudio		= MAKE_FOURCC( 'auds' ),
			eAshStreamTypeMidi		= MAKE_FOURCC( 'mids' ),
			eAshStreamTypeText		= MAKE_FOURCC( 'txts' )
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
		unsigned long		cb;

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

		unsigned long		dwFlags;

		unsigned short		wPriority;
		unsigned short		wLanguage;
		unsigned long		dwInitialFrames;
		unsigned long		dwScale;
		unsigned long		dwRate;
		unsigned long		dwStart;
		unsigned long		dwLength;
		unsigned long		dwSuggestedBufferSize;
		unsigned long		dwQuality;
		unsigned long		dwSampleSize;

		struct 
		{
			short left;
			short top;
			short right;
			short bottom;

		} rcFrame;
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

#pragma pack( pop )

public:

	/// Default constructor
	CRiffFile();
	
	/// Destructor
	~CRiffFile();

	/// Closes open file and releases resources
	void Destroy();

	/// Creates a new file
	BOOL Create( LPCTSTR pFile );

	/// Opens an existing file
	BOOL Open( LPCTSTR pFile );

	/// Reads in a chunk header and returns the file pointer to the start of the chunk
	BOOL ReadChunkHeader( SRiffChunk *pRc );

	/// Skips the current chunk
	BOOL SkipChunk();

	/// Skips chunks looking for a matching fcc
	BOOL FindChunk( unsigned long fccFcc, SRiffChunk *pRc );

	/// Reads in the avi file headers
	BOOL ReadAviHeaders();

	/// Read headers from list
	BOOL ReadHeadersFromList();

private:

	/// File handle
	oex::CFile								m_file;

	/// Riff file header
	SRiffFileHeader							m_rfh;

	/// File offset of main header
	LONGLONG								m_llAmhOffset;

	/// Set to non-zero if header structures are valid
	BOOL									m_bValidHeaders;

	/// AVI main header
	oex::TMem< char, SAviMainHeader >		m_amh;
//	SAviMainHeader			*m_pAmh;

	/// File offset of stream header
	LONGLONG								m_llAshOffset;

	/// Stream header
	oex::TMem< char, SAviStreamHeader >		m_ash;
//	SAviStreamHeader		*m_pAsh;

	/// File offset of audio stream header
	LONGLONG								m_llWfeOffset;

	/// Audio stream header
	oex::TMem< char, SWaveFormatEx >		m_wfe;
//	SWaveFormatEx			*m_pWfe;	
};