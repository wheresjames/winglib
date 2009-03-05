/*------------------------------------------------------------------
// sq_multi.cpp
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

#include "stdafx.h"

using namespace sqbind;

CSqMulti::CSqMulti()
{
}

CSqMulti::CSqMulti( const CSqMulti::t_Obj &s )
{
	deserialize( s );
}

CSqMulti::CSqMulti( const oex::oexTCHAR *s )
{
	if ( s )
		deserialize( t_Obj( s ) );
}

CSqMulti::t_List& CSqMulti::list()
{   return m_lst; }

CSqMulti::CSqMulti& CSqMulti::operator []( const CSqMulti::t_Obj &rObj )
{	return m_lst[ rObj ]; }

CSqMulti::CSqMulti& CSqMulti::operator []( const oex::oexTCHAR *p )
{	return m_lst[ p ]; }

CSqString* CSqMulti::value()
{	return &m_val; }

sqbind::stdString& CSqMulti::str()
{	return m_val.str(); }


void CSqMulti::Register( SquirrelVM &vm )
{
    SqPlus::SQClassDef< CSqMulti >( vm, oexT( "CSqMulti" ) )
            . func( &CSqMulti::serialize,     	oexT( "serialize" ) )
            . func( &CSqMulti::deserialize,   	oexT( "deserialize" ) )
            . func( &CSqMulti::merge,			oexT( "merge" ) )
            . func( &CSqMulti::urlencode,		oexT( "urlencode" ) )
            . func( &CSqMulti::urldecode,		oexT( "urldecode" ) )
            . func( &CSqMulti::isset,         	oexT( "isset" ) )
            . func( &CSqMulti::size,          	oexT( "size" ) )
            . func( &CSqMulti::set,           	oexT( "set" ) )
            . func( &CSqMulti::get,           	oexT( "get" ) )
            . func( &CSqMulti::move_up,     	oexT( "move_up" ) )
            . func( &CSqMulti::move_down,     	oexT( "move_down" ) )
            . func( &CSqMulti::value,      		oexT( "value" ) )
            . func( &CSqMulti::str,           	oexT( "str" ) )
            . func( &CSqMulti::unset,         	oexT( "unset" ) )
            . func( &CSqMulti::clear,         	oexT( "clear" ) )
            . func( &CSqMulti::find_key,    	oexT( "find_key" ) )
            . func( &CSqMulti::find_value,    	oexT( "find_value" ) )
            . func( &CSqMulti::_get,          	oexT( "_get" ) )
            . func( &CSqMulti::_nexti,        	oexT( "_nexti" ) )
            . func( &CSqMulti::print_r,       	oexT( "print_r" ) )
        ;
}

int CSqMulti::size()
{	return m_lst.size(); }

void CSqMulti::clear()
{	m_lst.clear(); }

void CSqMulti::_serialize( oex::CPropertyBag &pb, CSqMulti::t_List &lst )
{
	for ( t_List::iterator it = lst.begin(); it != lst.end(); it++ )
	{
		if ( it->second.m_val.str().length() )
			pb[ it->first.c_str() ] = it->second.m_val.str().c_str();

		if ( it->second.size() )
			_serialize( pb[ it->first.c_str() ], it->second.list() );

	} // end for
}

CSqParam::t_SqStr CSqMulti::serialize()
{
	oex::CPropertyBag pb;
	_serialize( pb, m_lst );
	return oex::CParser::Serialize( pb ).Ptr();
}

void CSqMulti::_deserialize( oex::CPropertyBag &pb, CSqMulti::t_List &lst )
{
	for ( oex::CPropertyBag::iterator it; pb.List().Next( it ); )
	{
		if ( it->ToString().Length() )
			lst[ it.Node()->key.Ptr() ].m_val.set( it->ToString().Ptr() );

		if ( it->Size() )
			_deserialize( *it, lst[ it.Node()->key.Ptr() ].list() );

	} // end for

}

void CSqMulti::deserialize( const CSqParam::t_SqStr &s )
{
	// Lose old data
	m_lst.clear();

	// Deserialize data
	oex::CPropertyBag pb = oex::CParser::Deserialize( s.c_str() );
	_deserialize( pb, m_lst );
}

void CSqMulti::merge( const CSqParam::t_SqStr &s )
{
	// Deserialize data
	oex::CPropertyBag pb = oex::CParser::Deserialize( s.c_str() );
	_deserialize( pb, m_lst );
}

CSqMulti::t_Obj CSqMulti::urlencode()
{
	oex::CPropertyBag pb;
	for ( t_List::iterator it = m_lst.begin(); it != m_lst.end(); it++ )
		pb[ it->first.c_str() ] = it->second.m_val.str().c_str();
	return oex::CParser::EncodeUrlParams( pb ).Ptr();
}

void CSqMulti::urldecode( const CSqMulti::t_Obj &s )
{
	// Deserialize data
	oex::CPropertyBag pb = oex::CParser::DecodeUrlParams( s.c_str() );;
	for ( oex::CPropertyBag::iterator it; pb.List().Next( it ); )
		m_lst[ it.Node()->key.Ptr() ].m_val.set( it->ToString().Ptr() );
}

void CSqMulti::unset( const CSqMulti::t_Obj &k )
{
	m_lst.erase( k );
}


void CSqMulti::set( const CSqMulti::t_Obj &v )
{	m_val = v; }

CSqMulti::iterator CSqMulti::begin()
{	return m_lst.begin(); }

CSqMulti::iterator CSqMulti::end()
{	return m_lst.end(); }

CSqMulti::iterator CSqMulti::find ( const CSqMulti::t_Obj &k )
{	return m_lst.find( k ); }

int CSqMulti::isset( const CSqMulti::t_Obj &k )
{	return ( m_lst.end() != m_lst.find( k ) ) ? 1 : 0; }

CSqMulti* CSqMulti::get( const CSqMulti::t_Obj &k )
{	return &m_lst[ k ];
}

void CSqMulti::move_up( const t_Obj &k )
{
	t_List::iterator it = m_lst.find( k );
	if ( m_lst.end() == it )
		return;

	if ( m_lst.begin() == it )
		return;

	// Swap the data
	t_List::iterator itB = it; itB--;
	t_List t = it->second.list();
	it->second.list() = itB->second.list();
	itB->second.list() = t;
}

void CSqMulti::move_down( const t_Obj &k )
{
	t_List::iterator it = m_lst.find( k );
	if ( m_lst.end() == it )
		return;

	// Swap the data
	t_List::iterator itB = it; itB++;
	if ( m_lst.end() == itB )
		return;

	t_List t = it->second.list();
	it->second.list() = itB->second.list();
	itB->second.list() = t;
}


/// Adds an element to the vector
CSqMulti::t_Obj CSqMulti::find_key( const CSqMulti::t_Obj &k )
{
    // For each item
    for ( t_List::iterator it = m_lst.begin();
          m_lst.end() != it;
          it++ )

        // Is the sub string in the key?
        if ( match_pattern( it->first.c_str(), k.c_str() ) )
            return it->first;

    return oexT( "" );
}

/// Adds an element to the vector
CSqMulti::t_Obj CSqMulti::find_value( const CSqMulti::t_Obj &v )
{
    // For each item
    for ( t_List::iterator it = m_lst.begin();
          m_lst.end() != it;
          it++ )

        // Is the sub string in the value
        if ( match_pattern( it->second.m_val.str().c_str(), v.c_str() ) )
            return it->first;

    return oexT( "" );
}

/// Internal squirrel function, returns value of specified item
SquirrelObject CSqMulti::_get( HSQUIRRELVM v )
{
    StackHandler sa( v );

    const SQChar *pKey = sa.GetString( 2 );
    if ( !pKey || !*pKey )
        return SquirrelObject( v );

//    t_List::iterator it = m_lst.find( pKey );
//   if ( m_lst.end() == it )
//        return SquirrelObject( v );

	SquirrelObject so( v );

	SqPlus::Push( v, &m_lst[ pKey ] );

	// Is it an array?
//	if ( m_lst.size() )
//		SqPlus::Push( v, &it->second );

	// Push value
//	else
//		sq_pushstring( v, it->second.m_val.get()->c_str(), (int)it->second.m_val.get()->length() );

	so.AttachToStackObject( -1 );
	sq_pop( v, 1 );

    return so;
}

/// Internal squirrel function used to iterate list items
SquirrelObject CSqMulti::_nexti( HSQUIRRELVM v )
{
    StackHandler sa( v );

    SQObjectType type = (SQObjectType)sa.GetType( 2 );

    switch( type )
    {
        case OT_NULL:
        {
            if ( !m_lst.size() )
                return SquirrelObject( v );

            SquirrelObject so( v );
            sq_pushstring( v, m_lst.begin()->first.c_str(), (int)m_lst.begin()->first.length() );
            so.AttachToStackObject( -1 );
            sq_pop( v, 1 );

            return so;

        } break;

        case OT_STRING:
        {
            const SQChar *pKey = sa.GetString( 2 );
            if ( !pKey || !*pKey )
                return SquirrelObject( v );

            t_List::iterator it = m_lst.find( pKey );
            if ( m_lst.end() == it )
                return SquirrelObject( v );

            it++;
            if ( m_lst.end() == it )
                return SquirrelObject( v );

            SquirrelObject so( v );

            sq_pushstring( v, it->first.c_str(), (int)it->first.length() );
            so.AttachToStackObject( -1 );
            sq_pop( v, 1 );

            return so;

        } break;

    } // end switch

    // ???
    sa.ThrowError( oexT( "Invalid index type" ) );
    return SquirrelObject( v );
}

/// Returns non-zero if pPattern matches pString
bool CSqMulti::match_pattern( const oex::oexTCHAR *pString, const oex::oexTCHAR *pPattern)
{
    if ( !pString || !pPattern )
        return false;

    unsigned int i = 0, p = 0;

    // Skip multiple '*'
    while ( oexT( '*' ) == pPattern[ p ] && oexT( '*' ) == pPattern[ p + 1 ] )
        p++;

    // Check for the 'any' pattern
    if ( pPattern[ p ] == oexT( '*' ) && pPattern[ p + 1 ] == 0 )
        return true;

    // While we're not at the end
    while ( pString[ i ] != 0 )
    {
        // Are we on a wildcard?
        if ( oexT( '*' ) == pPattern[ p ] )
        {
	        // Are we matching everything?
	        if ( pPattern[ p + 1 ] == 0 )
                return true;

	        // Check for pattern advance
	        if (	pString[ i ] == pPattern[ p + 1 ] ||

			        (
				        pString[ i ] >= oexT( 'a' ) && pString[ i ] <= oexT( 'z' ) &&
				        ( pString[ i ] - ( oexT( 'a' ) - oexT( 'A' ) ) ) == pPattern[ p + 1 ]
			        ) ||

			        (
				        pString[ i ] >= oexT( 'A' ) && pString[ i ] <= oexT( 'Z' ) &&
				        ( pString[ i ] + ( oexT( 'a' ) - oexT( 'A' ) ) ) == pPattern[ p + 1 ]
			        )

		        ) p += 2;

        } // end if

        // Just accept this character
        else if ( oexT( '?' ) == pPattern[ p ] )
            p++;

        // Otherwise advance if equal
        else if ( pString[ i ] == pPattern[ p ] )
            p++;

        // Case insensitive
        else if (	(
				        pString[ i ] >= oexT( 'a' ) && pString[ i ] <= oexT( 'z' ) &&
				        ( pString[ i ] - ( oexT( 'a' ) - oexT( 'A' ) ) ) == pPattern[ p ]
			        ) ||
			        (
				        pString[ i ] >= oexT( 'A' ) && pString[ i ] <= oexT( 'Z' ) &&
				        ( pString[ i ] + ( oexT( 'a' ) - oexT( 'A' ) ) ) == pPattern[ p ]
			        )
		        ) p++;

        // Back up in the pattern
        else while ( p && pPattern[ p ] != oexT( '*' ) ) p--;

        // Return true if we're at the end of the pattern
        if ( pPattern[ p ] == 0 )
            return true;

        // Next char
        i++;

    } // end while

    // Skip wild cards
    while ( pPattern[ p ] == oexT( '*' ) ) p++;

    // Did we match?
    return ( pPattern[ p ] == 0 );
}

static CSqMulti::t_Obj tab( int t )
{	CSqMulti::t_Obj str; while ( t-- ) str += oexT( "\t" ); return str; }


CSqMulti::t_Obj CSqMulti::_print_r( int nShowVals, t_List &rLst, int nDepth )
{
	t_Obj sRet;
	for ( iterator it = rLst.begin(); it != rLst.end(); it++ )
	{
		if ( nDepth ) sRet += tab( nDepth );
		sRet += it->first; sRet += oexT( " = " );

		// Is there an array?
		if ( it->second.size() )
		{
			// Is default value present?
			if ( it->second.m_val.str().length() )
				sRet += oexT( "[ " ),
				sRet += ( nShowVals ? it->second.m_val.str() : oexT( "..." ) ),
				sRet += oexT( "] " );
			sRet += oexNL;
			if ( nDepth ) sRet += tab( nDepth );
			sRet += oexT( " [ " );
			sRet += oexNL;

			sRet += _print_r( nShowVals, it->second.list(), nDepth + 1 );
			if ( nDepth ) sRet += tab( nDepth );
			sRet += oexT( " ]" );
			sRet += oexNL;

		} // end if
		else
		{
			sRet += ( nShowVals ? it->second.m_val.str() : oexT( "..." ) );
			sRet += oexNL;
		} // end else

	} // end for
	return sRet;
}

CSqMulti::t_Obj CSqMulti::print_r( int nShowVals )
{
	return _print_r( nShowVals, m_lst );
}


