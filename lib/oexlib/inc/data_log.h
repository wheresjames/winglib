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
		/// Maximum number of keys that can be logged
		eMaxKeys = 1024,

		/// Default size of logging buffer
		eInitBuffer = 1024,

		/// Index step size
		eIndexStep = 60 * 10,

		/// Per day
		eLogBase = 60 * 60 * 24
	};

	/// Index info on a value
	struct SValueIndex
	{
		/// Size of this structure
		oexUINT			uBytes;

		/// Timestamp
		oexUINT			uTime;

		/// Timestamp milliseconds
		oexUINT			uTimeMs;

		/// Size of the data in this block
		CFile::t_size	uSize;

		/// Offset to the next block of data for this value
		CFile::t_size	uNext;

		/// Offset to the previous block of data for this value
		CFile::t_size	uPrev;
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

		/// Data hash
		oex::oexGUID	hash;

		/// Pointer to last structure
		oexUINT			olast;

		/// Position of last header
		CFile::t_size	plast;
	};


public:

    /// Default constructor
    CDataLog();

    /// Destructor
    virtual ~CDataLog();

    /// Closes the file
    void Destroy();

	/// Sets the logging folder
	oexBOOL SetRoot( oexCSTR x_pRoot, oexBOOL x_bChangesOnly = oexTRUE );

	/// Creates a logging key for the specified value, return less than zero on failure
	oexINT AddKey( oexCSTR x_pKey );

	/// Removes a key from the logger
	oexINT RemoveKey( oexINT x_nKey );

	/// Returns the index of the named key if it exists, otherwise returns less than zero
	oexINT CDataLog::FindKey( oexCSTR x_pKey );

	/// Logs the specified value
	oexBOOL Log( oexINT x_nKey, oexCPVOID x_pValue, oexUINT x_uSize, oexUINT x_uTime, oexUINT x_uTimeMs );

	/// Flushes data to disk
	oexBOOL Flush( oexUINT x_uTime = 0 );

private:

	/// Logs only when a value changes
	oexBOOL			m_bChangesOnly;

	/// Logging root
	CStr			m_sRoot;

	/// Pointers to logging buffers
	SLogKey			*m_pLogs[ eMaxKeys ];

};

