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

	enum
	{
		eDtNone = 0,

		eDtString = 1,

		eDtInt = 2,

		eDtFloat = 3

	};

	enum
	{
		eMethodNone = 0,

		eMethodDiscrete = 1,

		eMethodAverage = 2,
	};

	enum
	{
		/// Maximum number of keys that can be logged
		eMaxKeys = 1024,

		/// Default size of logging buffer
		eInitBuffer = 1024,

		/// Index step size
		eIndexStep = 60 * 5,

		/// Per day
		eLogBase = 60 * 60 * 24,

		/// Write data every half step
		eMaxValid = eIndexStep / 2

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

		/// Timestamp milliseconds
		oexUINT32			uTimeMs;

		/// Size of the data in this block
		CFile::t_size		uSize;

		/// Offset to the next block of data for this value
		CFile::t_size		uNext;

		/// Offset to the previous block of data for this value
		CFile::t_size		uPrev;
	};

	/// Holds info on a logging buffer
	struct SLogKey
	{
		/// Key name
		CStr			sName;

		/// Key hash
		CStr			sHash;

		/// Data buffer
		CBin			bin;

		/// Key hash
		oex::oexGUID	hash;

		/// Data hash
		oex::oexGUID	changed;

		/// Every so often, write the data anyway
		oexUINT			valid;

		/// Pointer to last structure
		oexUINT			olast;

		/// Position of last header
		CFile::t_size	plast;
	};

	struct SIterator
	{
		// Constructor
		SIterator()
		{	uB = uI = oexMAXUINT;
			pos = 0;
			oexZero( vi );
			oexZero( viNext );
			nValue = 0;
			fValue = 0.f;
			nCount = 0;
		}

		// Read value from the data file into the specified string
		oexBOOL getValue( CStr &s )
		{	if ( !fData.IsOpen() ) return oexFALSE;
			if ( !vi.uSize ) { s.Destroy(); return oexTRUE; }
			oexSTR p = s.OexAllocate( vi.uSize );
			if ( !p ) return oexFALSE;
			fData.Read( p, vi.uSize );
			s.SetLength( vi.uSize );
			return oexTRUE;
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
		oexBOOL IsData( oexUINT x_uTime = 0 )
		{
			if ( !x_uTime )
				x_uTime = oexGetUnixTime();

			// Build root to data based on starting timestamp
			if ( !oexExists( ( oex::CStr( sRoot ).BuildPath( x_uTime / eLogBase ).BuildPath( sHash ) << oexT( ".bin" ) ).Ptr() ) )
				return oexFALSE;
			
			return oexTRUE;
		}

		// Variables
		CStr			sRoot;
		CStr8			sKey;
		CStr			sHash;
		oexGUID			hash;
		oexUINT			uB;
		oexUINT			uI;
		SValueIndex		vi;
		SValueIndex		viNext;
		CFile::t_size	pos;
		CFile			fData;
		CFile			fIdx;
		CStr			sValue;
		oexINT			nValue;
		oexFLOAT		fValue;
		oexINT			nCount;
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
	oexINT AddKey( oexCSTR x_pKey, oexUINT x_uTime = 0 );

	/// Removes a key from the logger
	oexINT RemoveKey( oexINT x_nKey );

	/// Returns the index of the named key if it exists, otherwise returns less than zero
	oexINT FindKey( oexCSTR x_pKey );

	/// Logs the specified value
	oexBOOL Log( oexINT x_nKey, oexCPVOID x_pValue, oexUINT x_uSize, oexUINT x_uTime, oexUINT x_uTimeMs );

	/// Flushes data to disk
	oexBOOL Flush( oexUINT x_uTime = 0 );

	/// Returns a list of key names and hashes
	CPropertyBag GetKeyList( oexUINT x_uTime = 0 );

	/// Returns the log for the specified key and time range
	CPropertyBag GetLog( oexCSTR x_pKey, oexUINT x_uStart, oexUINT x_uEnd, oexUINT x_uInterval, oexINT x_nDataType, oexINT x_nMethod );

	/// Returns the log for the specified key and time range in a shared binary buffer
	CStr GetLogBin( oexCSTR x_pKey, oexUINT x_uStart, oexUINT x_uEnd, oexUINT x_uInterval, oexINT x_nDataType, oexINT x_nMethod, float x_fScale );

public:

	/// Returns non-zero if there is an index file for the specified key hash
	static oexBOOL IsKeyData( CStr x_sRoot, CStr x_sHash, oexUINT x_uTime );

	/// Opens / Creates specified database files
	static oexBOOL OpenDb( oexBOOL x_bCreate, CStr x_sRoot, CStr x_sHash, oexUINT x_uTime, CFile *x_pIdx, CFile *x_pData );

	/// Finds the value for the specified time in an open database
	static oexBOOL FindValue( SIterator &x_it, oexUINT x_uTime, oexUINT x_uTimeMs, oexINT x_nDataType, oexINT x_nMethod );

private:

	/// Logs only when a value changes
	oexBOOL			m_bChangesOnly;

	/// Logging root
	CStr			m_sRoot;

	/// Pointers to logging buffers
	SLogKey			*m_pLogs[ eMaxKeys ];

};

