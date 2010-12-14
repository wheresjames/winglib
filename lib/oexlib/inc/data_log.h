/*------------------------------------------------------------------
// data_log.h
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


//==================================================================
// CDataLog
//
/// File based logging
/**

*/
//==================================================================
class CDataLog
{
public:
		/// Time type
		typedef CSysTime::t_time		t_time;
		
		/// Interval base = microseconds
		static const oexINT64 c_IntervalBase = 1000000ll;

public:

	enum
	{
		eMethodNone			= 0x00,
		
		eMethodDiscrete		= 0x01,

		eMethodReverse		= 0x02,

		eMethodAverage		= 0x04,

		eMethodNoRead		= 0x10
	};

	enum
	{
		/// Maximum number of keys that can be logged
		eMaxKeys = 1024,

		/// Default size of logging buffer
		eInitBuffer = 1024,

		/// Default maximum number of values to return
		eDefaultFetchLimit = 1024,

		/// Index step size
		eIndexStep = 60 * 5,

		/// Per day
		eLogBase = 60 * 60 * 24,

	};

	/// Index info on a value
	struct SValueIndex
	{
		/// Size of this structure
		oexUINT32			uBytes;

		/// Key ID
		oexUINT32			uHash;

		/// Timestamp
		oexUINT32			uTime;

		/// Timestamp microseconds
		oexUINT32			uTimeMs;

		/// Size of the data in this block
		CFile::t_size		uSize;

		/// Offset to the next block of data for this value
		CFile::t_size		uNext;

		/// Offset to the previous block of data for this value
		CFile::t_size		uPrev;

		/// Timestamp microseconds
		oexUINT32			uType;
	};

	/// Holds info on a logging buffer
	struct SLogKey
	{
		/// Key name
		CStr			sName;

		/// Key hash
		CStr			sHash;

		/// Key data file
		CStr			sFile;

		/// Data buffer
		CBin			bin;

		/// Data type
		oexUINT			uType;

		/// Key hash
		oexGUID			hash;

		/// Root index
		oexUINT			uRi;

		/// Data hash
		oexGUID			changed;

		/// Every so often, write the data anyway
		oexUINT			valid;

		/// Pointer to last structure
		oexUINT			olast;

		/// Position of last header
		CFile::t_size	plast;

		/// Key parameters
		CPropertyBag	params;
	};

	struct SIterator
	{
		// Constructor
		SIterator()
		{	uB = uI = oexMAXLONG;
			pos = 0;
			npos = 0;
			oexZero( vi );
			oexZero( viNext );
			nValue = 0;
			fValue = 0.f;
			dValue = 0;
			nCount = 0;
			uType = 0;
			uElements = 0;
			uEi = 0;
		}

		~SIterator()
		{	Destroy();
		}

		void Destroy()
		{	uB = uI = oexMAXLONG;
			pos = 0;
			npos = 0;
			oexZero( vi );
			oexZero( viNext );
			nValue = 0;
			fValue = 0.f;
			dValue = 0;
			nCount = 0;
			uType = 0;
			uElements = 0;
			uEi = 0;
			fIdx.Destroy();
			fData.Destroy();			
		}

		oexFLOAT getFloat()
		{	
			if ( uElements && uEi >= uElements )
				return 0.f;

			switch( vi.uType )
			{
				case obj::tDouble :
					return (oexFLOAT)getDouble();

				case obj::tInt :
					return (oexFLOAT)getInt();

				case obj::tFloat :
					return ( (oexFLOAT*)sValue.Ptr() )[ uElements ? uEi++ : 0 ];

			} // end switch

			return sValue.ToFloat();
		}

		oexINT getInt()
		{	
			if ( uElements && uEi >= uElements )
				return 0.f;

			switch( vi.uType )
			{
				case obj::tDouble :
					return (oexINT)getDouble();

				case obj::tInt :
					return ( (oexINT*)sValue.Ptr() )[ uElements ? uEi++ : 0 ];

				case obj::tFloat :
					return (oexINT)getFloat();

			} // end switch

			return sValue.ToInt();
		}

		oexINT getDouble()
		{	
			if ( uElements && uEi >= uElements )
				return 0.f;

			else if ( uElements ) switch( vi.uType )
			{
				case obj::tDouble :
					return ( (oexDOUBLE*)sValue.Ptr() )[ uElements ? uEi++ : 0 ];

				case obj::tInt :
					return (oexDOUBLE)getInt();

				case obj::tFloat :
					return (oexDOUBLE)getFloat();

			} // end switch

			return sValue.ToDouble();
		}

		oexFLOAT getAvgFloat()
		{	
			switch( vi.uType )
			{
				case obj::tDouble :
					return (oexFLOAT)getAvgDouble();

				case obj::tInt :
					return (oexFLOAT)getAvgInt();

				case obj::tFloat :
				{	oexFLOAT fAcc = 0.f, *p = (oexFLOAT*)sValue.Ptr();					
					oexUINT nSize = sValue.Length() / obj::StaticSize( uType );
					if ( !nSize )
						return 0.f;
					for ( oexUINT i = 0; i < nSize; i++ )
						fAcc += p[ i ];
					return fAcc / nSize;
				} break;

			} // end switch

			return sValue.ToFloat();
		}

		oexDOUBLE getAvgDouble()
		{	
			switch( vi.uType )
			{
				case obj::tFloat :
					return (oexDOUBLE)getAvgFloat();

				case obj::tInt :
					return (oexDOUBLE)getAvgInt();

				case obj::tDouble :
				{	oexDOUBLE dAcc = 0.f, *p = (oexDOUBLE*)sValue.Ptr();					
					oexUINT nSize = sValue.Length() / obj::StaticSize( uType );
					if ( !nSize )
						return 0.f;
					for ( oexUINT i = 0; i < nSize; i++ )
						dAcc += p[ i ];
					return dAcc / nSize;
				} break;

			} // end switch

			return sValue.ToDouble();
		}

		oexINT getAvgInt()
		{	
			switch( vi.uType )
			{
				case obj::tFloat :
					return (oexINT)getAvgFloat();

				case obj::tDouble :
					return (oexINT)getAvgDouble();

				case obj::tInt :
				{	oexINT nAcc = 0, *p = (oexINT*)sValue.Ptr();					
					oexUINT nSize = sValue.Length() / obj::StaticSize( uType );
					if ( !nSize )
						return 0;
					for ( oexUINT i = 0; i < nSize; i++ )
						nAcc += p[ i ];
					return nAcc / nSize;
				} break;

			} // end switch

			return sValue.ToInt();
		}

		// Read value from the data file into the specified string
		oexBOOL getValue( CStr8 &s )
		{
			if ( !fData.IsOpen() ) 
				return oexFALSE;

			if ( !vi.uSize ) 
			{ s.Destroy(); return oexTRUE; }

			oexSTR p = s.OexAllocate( vi.uSize );
			if ( !p ) 
				return oexFALSE;

			fData.Read( p, vi.uSize );
			s.SetLength( vi.uSize );

			// Is it an array?
			uEi = 0; uElements = 0;
			switch( vi.uType )
			{	case obj::tInt :
				case obj::tFloat :
				case obj::tDouble :
					oexINT uSz = oex::obj::StaticSize( vi.uType );
					if ( vi.uSize >= uSz ) uElements = vi.uSize / uSz;
			} // end switch

			return oexTRUE;

		} // end if

		CBin getValueBin()
		{	if ( !fData.IsOpen() ) return CBin();
			if ( !vi.uSize ) return CBin();
			return fData.Read( vi.uSize );
		} // end if

		// Sets the key and calculates the hash
		oexBOOL Init( CStr x_sRoot, oexCSTR x_pKey )
		{	if ( !x_sRoot.Length() || !x_pKey || !*x_pKey )
				return oexFALSE;
			sRoot = x_sRoot;
			sKey = oexStrToMb( x_pKey );
			sHash = oexMbToStr( CBase16::Encode( oss::CMd5::Transform( &hash, sKey.Ptr(), sKey.Length() ), sizeof( hash ) ) );
			return oexTRUE;
		}

		// Returns non-zero if there is data for the iterator
		oexBOOL IsData( oexINT nLogBase, t_time x_tTime = 0 )
		{
			if ( 0 >= nLogBase )
				return oexFALSE;

			if ( !x_tTime )
				x_tTime = oexGetUnixTime();

			// Build root to data based on starting timestamp
			if ( !oexExists( ( oex::CStr( sRoot ).BuildPath( x_tTime / nLogBase ).BuildPath( sHash ) << oexT( ".bin" ) ).Ptr() ) )
				return oexFALSE;

			// Get type from file
			uType = CParser::Deserialize( CFile().OpenExisting( ( CStr( sRoot ).BuildPath( x_tTime / nLogBase ).BuildPath( CStr( sHash ) << oexT( ".txt" ) ) ).Ptr() ).Read() )[ "type" ].ToUInt();

			return oexTRUE;
		}

		// Variables
		CStr			sRoot;
		CStr8			sKey;
		CStr			sHash;
		oexUINT			uType;
		oexGUID			hash;
		oexULONG		uB;
		oexULONG		uI;
		SValueIndex		vi;
		SValueIndex		viNext;
		CFile::t_size	pos;
		CFile::t_size	npos;
		CFile			fData;
		CFile			fIdx;
		CStr8			sValue;
		oexINT			nValue;
		oexFLOAT		fValue;
		oexDOUBLE		dValue;
		oexINT			nCount;
		oexUINT			uElements;
		oexUINT			uEi;
	};

public:

    /// Default constructor
    CDataLog();

    /// Destructor
    virtual ~CDataLog();

    /// Closes the file
    void Destroy();

	/// Sets the logging folder
	oexBOOL SetRoot( oexCSTR x_pRoot );

	/// Returns the root logging folder
	CStr GetRoot() { return m_sRoot; }

	/// Creates a logging key for the specified value, return less than zero on failure
	oexINT AddKey( oexCSTR x_pKey, oexUINT x_uType, t_time x_tTime = 0, oexCSTR x_pParams = oexNULL );

	/// Removes a key from the logger
	oexINT RemoveKey( oexINT x_nKey );

	/// Returns the number of bytes currently being buffered
	oexINT GetBufferSize( oexINT x_nKey );

	/// Returns the index of the named key if it exists, otherwise returns less than zero
	oexINT FindKey( oexCSTR x_pKey );

	/// Logs the specified value from a shared buffer
	oexBOOL LogBin( oexCSTR x_pKey, oexCSTR x_pBin, oexINT x_nType, t_time x_tTime, t_time x_tTimeMs, oexBOOL x_bBuffering );

	/// Logs the specified value
	oexBOOL Log( oexINT x_nKey, oexCPVOID x_pValue, oexUINT x_uSize, t_time x_tTime, t_time x_tTimeMs, oexBOOL x_bBuffering = oexTRUE );

	/// Flushes all buffered data to disk
	oexBOOL Flush( t_time x_tTime = 0 );

	/// Flushes a single buffer value
	oexBOOL FlushBuffer( oexINT x_nKey, SValueIndex *pVi, oexCPVOID pBuf, oexUINT uSize );

	/// Returns a list of key names and hashes
	CPropertyBag GetKeyList( t_time x_tTime = 0 );

	/// Returns the log for the specified key and time range
	CPropertyBag GetLog( oexCSTR x_pKey, t_time x_tStart, t_time x_tEnd, t_time x_tInterval, oexINT x_nDataType, oexINT x_nMethod );

	/// Returns the log for the specified key and time range in a shared binary buffer
	CStr GetLogBin( oexCSTR x_pKey, t_time x_tStart, t_time x_tEnd, t_time x_tInterval, oexINT x_nDataType, oexINT x_nMethod, float x_fScale );

	/// Sets up log params
	void SetLogParams( t_time tBase, t_time tStep );

	/// Returns the root base for the logs
	t_time GetBase() { return m_tLogBase; }

	/// Returns the index step
	t_time GetStep() { return m_tIndexStep; }

	/// Returns the base interval in counts per second
	t_time GetInterval() { return c_IntervalBase; }

	/// Returns default fetch limit
	oexUINT GetLimit() { return m_uLimit; }

	/// Sets the default fetch limit
	void SetLimit( oexUINT uLimit ) { m_uLimit = uLimit; }

public:

	/// Returns non-zero if there is an index file for the specified key hash
	static oexBOOL IsKeyData( CStr x_sRoot, CStr x_sHash, t_time x_tTime, t_time tLogBase );

	/// Opens / Creates specified database files
	static oexBOOL OpenDb( oexBOOL x_bCreate, CStr x_sRoot, CStr x_sHash, t_time x_tTime, CFile *x_pIdx, CFile *x_pData, t_time tLogBase, t_time tIndexStep, oexBOOL &bNew );

	/// Finds the value for the specified time in an open database
	static oexBOOL FindValue( SIterator &x_it, t_time x_tTime, t_time x_tTimeMs, t_time x_tInterval, oexINT x_nDataType, oexINT x_nMethod, t_time tLogBase, t_time tIndexStep );

private:

	/// Logs only when a value changes
	oexBOOL			m_bChangesOnly;

	/// Logging root
	CStr			m_sRoot;

	/// Pointers to logging buffers
	SLogKey			*m_pLogs[ eMaxKeys ];

	/// Index step
	t_time			m_tIndexStep;

	/// Base for log root
	t_time			m_tLogBase;

	/// Maximum value valid time
	t_time			m_tMaxValid;

	/// Maximum return limit
	oexUINT			m_uLimit;

};

