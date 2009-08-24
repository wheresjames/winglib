/*------------------------------------------------------------------
// sqlite.cpp
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

#include "../oexlib.h"

#if defined( OEX_ENABLE_SQLITE )

// Pull in sqlite
#include "../oss/sqlite/sqlite3.h"

// Assumptions
//oexSTATIC_ASSERT( sizeof( sqlite3* ) == sizeof( oexPVOID ) );

OEX_USING_NAMESPACE

CSQLite::CSQLite()
{
	m_sqobj = oexNULL;
	m_nRows = 0;
}

CSQLite::~CSQLite()
{
	Destroy();
}

int CSQLite::_Callback( void *pThis, int argc, char **argv, char **azColName )
{
	if ( !oexCHECK_PTR( pThis ) )
		return 0;

	return ((CSQLite*)pThis )->OnCallback( argc, argv, azColName );
}

int CSQLite::OnCallback( int argc, char **argv, char **azColName )
{
	// What row are we on
	CPropertyBag &row = m_pbResult[ m_nRows++ ];

	// Add rows
	for ( int i = 0; i < argc; i++ )
		row[ azColName[ i ] ] = argv[ i ];

	return 0;
}

void CSQLite::Destroy()
{
	// Lose results
	Clear();

	// Close any open database
	if ( m_sqobj )
	{	sqlite3_close( (sqlite3*)m_sqobj );
		m_sqobj = oexNULL;
	} // end if
}

void CSQLite::Clear()
{	m_nRows = 0;
	m_pbResult.Destroy();
}

oexBOOL CSQLite::Open( oexCSTR x_pDb )
{
	if ( !oexCHECK_PTR( x_pDb ) )
	{	oexERROR( 0, oexT( "Invalid parameter" ) );
		return oexFALSE;
	} // end if

	// Attempt to open database
	oexINT res = sqlite3_open( x_pDb, (sqlite3**)&m_sqobj );
	if ( res )
	{	m_sErr = oexMbToStr( sqlite3_errmsg( (sqlite3*)m_sqobj ) );
		oexERROR( res, m_sErr );
		Destroy();
		return oexFALSE;
	} // end if

	return oexTRUE;
}

oexBOOL CSQLite::Exec( oexCSTR x_pQuery )
{
	// Lose previous results
	Clear();

	// Sanity check params
	if ( !oexCHECK_PTR( m_sqobj ) || !oexCHECK_PTR( x_pQuery ) )
		return oexFALSE;

	// Execute the query
	char *pErr = oexNULL;
	oexINT res = sqlite3_exec( (sqlite3*)m_sqobj, x_pQuery, CSQLite::_Callback, this, &pErr );
	if ( SQLITE_OK != res )
	{ 	if ( oexCHECK_PTR( pErr ) )
			m_sErr = oexMbToStr( pErr );
		else
			m_sErr = oexT( "Invalid pointer returned from sqlite3_exec()" );
		oexERROR( res, m_sErr );
		return oexFALSE;
	} // end if

	return oexTRUE;
}

#endif // OEX_ENABLE_SQLITE
