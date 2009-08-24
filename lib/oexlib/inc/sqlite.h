/*------------------------------------------------------------------
// sqlite.h
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

#if defined( OEX_ENABLE_SQLITE )

class CSQLite
{
public:

	/// Default constructor
    CSQLite();

	/// Destructor
    ~CSQLite();

	/// Frees any open database resources
	void Destroy();

	/// Frees the last query result
	void Clear();

	/// Opens the specified database
	oexBOOL Open( oexCSTR x_pDb );

	/// Executes a query on the database
	oexBOOL Exec( oexCSTR x_pQuery );

	/// Returns the last error string
	CStr& GetLastError() { return m_sErr; }

	/// Returns the result of the last query
	CPropertyBag& Result() { return m_pbResult; }

	/// Returns the specified row
	CPropertyBag& Row( oexINT nRow ) { return m_pbResult[ nRow ]; }

	/// Returns the number of rows returned by the last query
	oexINT NumRows() { return m_nRows; }

protected:

	/// Dispatch function for OnCallback()
	static int _Callback( void *pThis, int argc, char **argv, char **azColName );

	/// Query callback
	int OnCallback( int argc, char **argv, char **azColName );

private:

	/// SQLite object
	oexPVOID		*m_sqobj;

	/// Last error message
	CStr			m_sErr;

	/// Number of rows returned by the last query
	oexINT			m_nRows;

	/// The result
	CPropertyBag	m_pbResult;

};

#endif // OEX_ENABLE_ZIP
