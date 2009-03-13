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
    	m_uLevel = eNotice;
    }

    /// Destructor
    ~CLog()
    {
    	Destroy();
    }

    void Destroy()
    {  	m_file.Flush();
		m_file.Destroy();
		m_sPath.Destroy();
	}

	//==============================================================
	// Log()
	//==============================================================
	/// Writes a string to the file
	/**
		\param [in] x_pFile		- Source file name
		\param [in] x_uLine		- Source file line number
		\param [in] x_pFunction - Function that generated the call
		\param [in] x_uLevel	- Error level
		\param [in] x_pErr		- Error string
		\param [in] x_uErr		- Error code

		\warning x_uErr should be the last parameter, if it is
		         a global variable like 'errno', it could possibly,
		         be destroyed by side effects of other parameters.

		\return Returns x_uErr

		\see
	*/
    oexINT Log( oexCSTR x_pFile, oexINT x_nLine, oexCSTR x_pFunction, oexINT x_uLevel, oexCSTR x_pErr, oexINT x_nErr, oexUINT x_uSkip = 2 );

	//==============================================================
	// Log()
	//==============================================================
	/// Writes a string to the file
	/**
		\param [in] x_pFile		- Source file name
		\param [in] x_uLine		- Source file line number
		\param [in] x_pFunction - Function that generated the call
		\param [in] x_uLevel	- Error level
		\param [in] x_uErr		- Error code
		\param [in] x_pErr		- Error string

		This function returns x_uErr in all cases

		\warning x_uErr should be the last parameter, if it is
		         a global variable like 'errno', it could possibly,
		         be destroyed by side effects of other parameters.

		\return Returns x_uErr

		\see
	*/
    oexINT Log( oexCSTR x_pFile, oexINT x_nLine, oexCSTR x_pFunction, oexINT x_uLevel, CStr x_sErr, oexINT x_nErr )
    {	if ( x_uLevel < m_uLevel ) return x_nErr;
    	return Log( x_pFile, x_nLine, x_pFunction, x_uLevel, x_sErr.Ptr(), x_nErr, 3 );
    }

	//==============================================================
	// OpenLogFile()
	//==============================================================
	/// Writes a string to the file
	/**
		\param [in] x_pPath			- Path for the log file
		\param [in] x_pFile			- Log file name
		\param [in] x_pExtension	- Extension to add to log file

		This function opens the specified file and directs errors
		to this file.  It is not normally necesary to call this file,
		a file will automatically be created with the first call
		to log.  You may want to use this function to keep sequential
		logs or for some other purpose.

		\warning x_uErr should be the last parameter, if it is
		         a global variable like 'errno', it could possibly,
		         be destroyed by side effects of other parameters.

		\return Returns non-zero if success

		\see
	*/
    oexBOOL OpenLogFile( oexCSTR x_pPath = oexNULL, oexCSTR x_pFile = oexNULL, oexCSTR x_pExtension = oexNULL );

	//==============================================================
	// Open()
	//==============================================================
	/// Opens the specified file for writing
	/**
		\param [in] x_pPath			- Path for the log file
		Opens the specified path for logging

		\return Returns non-zero if success

		\see
	*/
    oexBOOL Open( oexCSTR x_pPath );

	//==============================================================
	// Resume()
	//==============================================================
	/// Opens the specified file and appends to the end
	/**
		\param [in] x_pPath			- Path for the log file

		Opens the specified path for logging

		\return Returns non-zero if success

		\see
	*/
    oexBOOL Resume( oexCSTR x_pPath );

	//==============================================================
	// SetReportingLevel()
	//==============================================================
	/// Writes a string to the file
	/**
		\param [in] x_uLevel	- Error level

		This function returns x_uErr in all cases
	*/
    void SetReportingLevel( oexUINT x_uLevel )
    {  	m_uLevel = x_uLevel; }

	/// Flushes the log to disk
	void Flush()
	{	m_file.Flush(); }

	/// Returns the path for the log file
	CStr GetPath()
	{	return m_sPath; }

	/// Returns non-zero if a log file is open
	oexBOOL IsOpen()
	{	return m_file.IsOpen(); }

private:

	/// Log file
	CFile								m_file;

	/// Log file path
	CStr								m_sPath;

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

