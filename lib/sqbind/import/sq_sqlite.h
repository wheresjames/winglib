/*------------------------------------------------------------------
// sq_sqlite.h
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

// namespace
namespace sqbind
{
	class CSqSQLite : public oex::CSQLite
    {
	public:

		SQBIND_CLASS_CTOR_BEGIN( CSqSQLite )
		SQBIND_CLASS_CTOR_END( CSqSQLite )

		/// Register class for squirrel
		static void Register( sqbind::VM vm );

		/// Default constructor
		CSqSQLite();

		/// Destructor
		~CSqSQLite() { Destroy(); }

		// Copy semantics
		CSqSQLite( const CSqCapture &r ) { }
		CSqSQLite& operator=( const CSqSQLite &r ) { return *this; }

		/// Close previous device
		int Destroy(); 

		/// Opens the specified database
		int Open( const stdString &sDbName );

		/// Returns non-zero if the specified table exists
		int IsTable( const stdString &sTable );

		/// Executes a query on the database
		int Exec( const stdString &sQuery );

		/// Inserts the specified data
		int Insert( const stdString &sTable, CSqMap &mData );

		/// Updates the specified row
		int Update( const stdString &sTable, const stdString &sWhere, CSqMap &mData );

		/// Queries for table information
		int QueryTableInfo( const stdString &sTable );

		/// Queries for column information
		int QueryColumnInfo();

		/// Makes a string safe to use in a query
		stdString Escape( const stdString &sStr );

		/// Clears the last result
		virtual void OnClear();

		/// Returns a description of the last error
		stdString GetLastError();

		/// Returns the last query
		stdString GetLastQuery();

		/// Returns the result of the last query
		CSqMulti& Result() { return m_mResult; }

		/// The number of rows returned by the last query
		int NumRows() { return CSQLite::NumRows(); }

	protected:

		/// Query callback
		virtual int OnCallback( int argc, char **argv, char **azColName );

	private:

		/// Result of the last query
		sqbind::CSqMulti			m_mResult;
		
    };

}; // end namespace

// Declare type for use as squirrel parameters
SQBIND_DECLARE_INSTANCE( sqbind::CSqSQLite, CSqSQLite )

#endif // defined( OEX_ENABLE_SQLITE )