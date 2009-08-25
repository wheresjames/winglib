/*------------------------------------------------------------------
// sq_sqlite.cpp
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

#include "../stdafx.h"

using namespace sqbind;

#if defined( OEX_ENABLE_SQLITE )

SQBIND_REGISTER_CLASS_BEGIN( sqbind::CSqSQLite, CSqSQLite )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSQLite, Open )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSQLite, Destroy )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSQLite, IsTable )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSQLite, Exec )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSQLite, Insert )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSQLite, Update )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSQLite, NumRows )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSQLite, Result )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSQLite, Escape )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSQLite, QueryTableInfo )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSQLite, QueryColumnInfo )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSQLite, GetLastQuery )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSQLite, GetLastError )
SQBIND_REGISTER_CLASS_END()

void CSqSQLite::Register( sqbind::VM vm )
{
	SQBIND_EXPORT( vm, CSqSQLite );
}

CSqSQLite::CSqSQLite()
{
}

int CSqSQLite::Destroy()
{
	// Lose the database
	CSQLite::Destroy();

	// Clear the last result
	Clear();

	return 1;
}

void CSqSQLite::OnClear()
{
	// Lose last result
	m_mResult.clear();
}


int CSqSQLite::Open( const stdString &sDbName )
{
	return CSQLite::Open( sDbName.c_str() ) ? 1 : 0;
}

int CSqSQLite::IsTable( const stdString &sTable )
{
	return CSQLite::IsTable( sTable.c_str() ) ? 1 : 0;
}

int CSqSQLite::Exec( const stdString &sQuery )
{
	Clear();

	return CSQLite::Exec( sQuery.c_str() ) ? 1 : 0;
}

int CSqSQLite::Insert( const stdString &sTable, CSqMap &mData )
{	oex::CPropertyBag pb;
	CSqMap::_serialize( pb, mData.list() );
	return CSQLite::Insert( sTable.c_str(), pb ); 
}

int CSqSQLite::Update( const stdString &sTable, const stdString &sWhere, CSqMap &mData )
{	oex::CPropertyBag pb;
	CSqMap::_serialize( pb, mData.list() );
	return CSQLite::Update( sTable.c_str(), sWhere.c_str(), pb ); 
}

int CSqSQLite::QueryTableInfo( const stdString &sTable )
{
	Clear();
	return CSQLite::QueryTableInfo( sTable.c_str() );
}

int CSqSQLite::QueryColumnInfo()
{
	Clear();
	return CSQLite::QueryColumnInfo();
}

stdString CSqSQLite::Escape( const stdString &sStr )
{	oex::CStr sEsc( sStr.c_str(), sStr.length() );
	sEsc = CSQLite::Escape( sEsc );
	return stdString().assign( sEsc.Ptr(), sEsc.Length() );
}

stdString CSqSQLite::GetLastError()
{
	return CSQLite::GetLastError().Ptr();
}

stdString CSqSQLite::GetLastQuery()
{
	return CSQLite::GetLastQuery().Ptr();
}

int CSqSQLite::OnCallback( int argc, char **argv, char **azColName )
{
	// Allocate row object
	CSqMulti &row = m_mResult[ oexMks( NumRows() ).Ptr() ];
	
	// Add rows
	for ( int i = 0; i < argc; i++ )
	{
		if ( oexCHECK_PTR( azColName[ i ] ) )
			row[ oexMbToStrPtr( azColName[ i ] ) ].set( oexCHECK_PTR( argv[ i ] ) 
														? oexMbToStrPtr( argv[ i ] ) 
														: oexT( "" ) );
		else
			row[ oexT( "*" ) ].set( oexCHECK_PTR( argv[ i ] ) 
													? oexMbToStrPtr( argv[ i ] ) 
													: oexT( "" ) );
	} // end for

	return 0;
}


#endif // defined( OEX_ENABLE_SQLITE )