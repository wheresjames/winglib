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
    virtual ~CSQLite();

	/// Frees any open database resources
	void Destroy();

	/// Frees the last query result
	void Clear();

	/// Opens the specified database
	oexBOOL Open( oexCSTR x_pDb );

	/// Executes a query on the database
	oexBOOL Exec( CStr x_sQuery );

	/// Inserts property bag values into the database
	oexBOOL Insert( oexCSTR pTable, CPropertyBag &pb );

	/// Updates a row in the table
	oexBOOL Update( oexCSTR pTable, oexCSTR pWhere, CPropertyBag &pb );

	/// Deletes based on specified conditions
	oexBOOL Delete( oexCSTR pTable, CPropertyBag &pb, oexCSTR pCond );

	/// Returns the last error string
	CStr& GetLastError() { return m_sErr; }

	/// Returns the last error string
	CStr& GetLastQuery() { return m_sQuery; }

	/// Returns the result of the last query
	CPropertyBag& Result() { return m_pbResult; }

	/// Returns the specified row
	CPropertyBag& Row( oexINT nRow ) { return m_pbResult[ nRow ]; }

	/// Returns the number of rows returned by the last query
	oexINT NumRows() { return m_nRows; }

	/// Returns table information
	oexBOOL QueryTableInfo( oexCSTR x_pTable );

	/// Queries column info / since this is db specific
	oexBOOL QueryColumnInfo();

	/// Returns non zero if the specified table exists
	oexBOOL IsTable( oexCSTR x_pTable );

	/// If enabled, echo's all querys to stdout
	void DebugMode( oexBOOL x_b ) { m_bDebugMode = x_b; }

public:

	/// Escapes a string so that it is safe to insert in the database
	static CStr Escape( CStr sStr );

protected:

	/// Dispatch function for OnCallback()
	static int _Callback( void *pThis, int argc, char **argv, char **azColName );

	/// Query callback
	virtual int OnCallback( int argc, char **argv, char **azColName );

	/// Called to clear result data
	virtual void OnClear() {}

private:

	/// SQLite object
	oexPVOID		*m_sqobj;

	/// Last error message
	CStr			m_sErr;

	/// The last query
	CStr			m_sQuery;

	/// Number of rows returned by the last query
	oexINT			m_nRows;

	/// The result
	CPropertyBag	m_pbResult;

	/// Non-zero to enable debug mode
	oexBOOL			m_bDebugMode;

};

#endif // OEX_ENABLE_ZIP
