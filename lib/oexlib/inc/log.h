/*------------------------------------------------------------------
// log.h
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
// CLog
//
/// Provides generic logging
/**

*/
//==================================================================
class CLog
{
public:

	enum
	{
		eNotice 		= 0x00000001,
		eWarning 		= 0x00000002,
		eError			= 0x00000004,
		eHalt			= 0x00000008
	};

public:

    /// Default constructor
    CLog()
    {
    	m_lInLog = 0;
    	m_uLevel = eWarning;
    }

    /// Destructor
    ~CLog()
    {
    }

	//==============================================================
	// Log()
	//==============================================================
	/// Writes a string to the file
	/**
		\param [in] x_pFile		- Source file name
		\param [in] x_uLine		- Source file line number
		\param [in] x_uLevel	- Error level
		\param [in] x_uErr		- Error code
		\param [in] x_pErr		- Error string

		\return Returns x_uErr

		\see
	*/
    oexUINT Log( oexCSTR x_pFile, oexINT x_nLine, oexINT x_uLevel, oexINT x_uErr, oexCSTR x_pErr, oexUINT x_uSkip = 2 );

	//==============================================================
	// Log()
	//==============================================================
	/// Writes a string to the file
	/**
		\param [in] x_pFile		- Source file name
		\param [in] x_uLine		- Source file line number
		\param [in] x_uLevel	- Error level
		\param [in] x_uErr		- Error code
		\param [in] x_pErr		- Error string

		This function returns x_uErr in all cases

		\return Returns x_uErr

		\see
	*/
    oexUINT Log( oexCSTR x_pFile, oexINT x_nLine, oexINT x_uLevel, oexINT x_uErr, CStr x_sErr )
    {	if ( x_uLevel < m_uLevel ) return x_uErr;
    	return Log( x_pFile, x_nLine, x_uLevel, x_uErr, x_sErr.Ptr(), 3 );
    }

	//==============================================================
	// Log()
	//==============================================================
	/// Writes a string to the file
	/**
		\param [in] x_uLevel	- Error level

		This function returns x_uErr in all cases
	*/
    void SetReportingLevel( oexUINT x_uLevel )
    {  	m_uLevel = x_uLevel; }

private:

	/// Log file
	CFile								m_file;

public:

	//==============================================================
	// GlobalLog()
	//==============================================================
	/// Returns a pointer to the global log object
	static CLog& GlobalLog()
	{	return m_logGlobal;
	}

private:

	/// Flag to prevent re-entrant logging
	oexLONG								m_lInLog;

	/// Error reporting level
	oexUINT								m_uLevel;

	/// Global log file
	static CLog							m_logGlobal;
};

