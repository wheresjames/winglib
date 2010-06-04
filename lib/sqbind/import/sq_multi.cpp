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

#include "../stdafx.h"

using namespace sqbind;

CSqMulti::~CSqMulti()
{_STT();
	if ( m_def )
	{	OexAllocDestruct( m_def );
		m_def = oexNULL;
	} // end if
}

CSqMulti::CSqMulti()
{_STT();
	m_def = oexNULL;
}

CSqMulti::CSqMulti( const CSqMulti &m )
{_STT();
	m_def = oexNULL;
	m_lst = m.m_lst;
	m_val = m.m_val;
}

CSqMulti::CSqMulti( const CSqMulti::t_Obj &s )
{_STT();
	m_def = oexNULL;
	deserialize( s );
}

CSqMulti::CSqMulti( const oex::oexTCHAR *s )
{_STT();
	m_def = oexNULL;
	if ( s )
		deserialize( t_Obj( s ) );
}

CSqMulti& CSqMulti::operator = ( const CSqMulti &m )
{_STT();
	m_def = oexNULL;
	m_lst = m.m_lst;
	m_val = m.m_val;
	return *this;
}

CSqMulti::t_List& CSqMulti::list()
{_STT();
	return m_lst;
}

CSqMulti& CSqMulti::operator []( const CSqMulti::t_Obj &rObj )
{_STT();
	return m_lst[ rObj ];
}

CSqMulti& CSqMulti::operator []( const oex::oexTCHAR *p )
{_STT();
	return m_lst[ p ];
}

CSqString* CSqMulti::value()
{_STT();
	return &m_val;
}

sqbind::stdString& CSqMulti::str()
{_STT();
	return m_val.str();
}

int CSqMulti::toint()
{_STT();
	return oexStrToLong( m_val.str().c_str() );
}

float CSqMulti::tofloat()
{_STT();
	return oexStrToFloat( m_val.str().c_str() );
}

int CSqMulti::len()
{_STT();
	return m_val.str().length();
}


_SQBIND_REGISTER_CLASS_BEGIN( sqbind::CSqMulti, CSqMulti )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqMulti, serialize )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqMulti, deserialize )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqMulti, serialize_filter )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqMulti, deserialize_filter )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqMulti, setJSON )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqMulti, getJSON )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqMulti, merge )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqMulti, urlencode )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqMulti, urldecode )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqMulti, isset )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqMulti, size )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqMulti, add )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqMulti, set )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqMulti, get )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqMulti, copy )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqMulti, move_up )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqMulti, move_down )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqMulti, value )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqMulti, str )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqMulti, toint )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqMulti, tofloat )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqMulti, len )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqMulti, unset )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqMulti, clear )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqMulti, find_key )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqMulti, find_value )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqMulti, filter )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqMulti, _get )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqMulti, _nexti )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqMulti, _newslot )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqMulti, print_r )
_SQBIND_REGISTER_CLASS_END()

void CSqMulti::Register( sqbind::VM vm )
{_STT();
	_SQBIND_EXPORT( vm, CSqMulti );
}

int CSqMulti::size()
{_STT();
	return m_lst.size();
}

void CSqMulti::clear()
{_STT();
	m_lst.clear();
}

CSqParam::t_SqStr CSqMulti::getJSON()
{_STT();
	oex::CPropertyBag pb;
	SQBIND_MultiToPropertyBag( m_lst, pb );
	return oex::CParser::EncodeJSON( pb ).Ptr();
}

void CSqMulti::setJSON( const CSqParam::t_SqStr &s )
{_STT();
	// Lose old data
	m_lst.clear();

	// Deserialize data
	oex::CStr sData( s.c_str(), s.length() );
	oex::CPropertyBag pb = oex::CParser::DecodeJSON( sData );
	SQBIND_PropertyBagToMulti( pb, m_lst );
}

void CSqMulti::fromPb( oex::CPropertyBag &pb )
{	m_lst.clear();
	SQBIND_PropertyBagToMulti( pb, m_lst );

}

void CSqMulti::toPb( oex::CPropertyBag &pb )
{	SQBIND_MultiToPropertyBag( m_lst, pb );
}

CSqParam::t_SqStr CSqMulti::serialize()
{_STT();
	oex::CPropertyBag pb;
	SQBIND_MultiToPropertyBag( m_lst, pb );
	return oex::CParser::Serialize( pb ).Ptr();
}

void CSqMulti::deserialize( const CSqParam::t_SqStr &s )
{_STT();
	// Lose old data
	m_lst.clear();

	// Deserialize data
	oex::CStr sData( s.c_str(), s.length() );
	oex::CPropertyBag pb = oex::CParser::Deserialize( sData );
	SQBIND_PropertyBagToMulti( pb, m_lst );
}

CSqParam::t_SqStr CSqMulti::serialize_filter( const t_SqStr &sFilter, int x_bInclude, int x_bIgnoreCase )
{_STT();
	oex::CPropertyBag pb;
	SQBIND_MultiToPropertyBag( m_lst, pb );
	return oex::CParser::SerializeFilter( pb, sFilter.c_str(), x_bInclude, x_bIgnoreCase ).Ptr();
}

void CSqMulti::deserialize_filter( const CSqParam::t_SqStr &s, const t_SqStr &sFilter, int x_bInclude, int x_bIgnoreCase )
{_STT();
	// Lose old data
	m_lst.clear();

	// Deserialize data
	oex::CStr sData( s.c_str(), s.length() );
	oex::CPropertyBag pb = oex::CParser::DeserializeFilter( sData, sFilter.c_str(), x_bInclude, x_bIgnoreCase );
	SQBIND_PropertyBagToMulti( pb, m_lst );
}

CSqMulti& CSqMulti::add( CSqMulti &m )
{_STT();
	for ( t_List::iterator it = m.m_lst.begin(); it != m.m_lst.end(); it++ )
	{
		if ( !it->second.size() )
			m_lst[ it->first ] = it->second.m_val.str();

		else
			m_lst[ it->first ].add( it->second );

	} // end for

	return *this;
}

void CSqMulti::merge( const CSqParam::t_SqStr &s )
{_STT();
	// Deserialize data
	oex::CStr sData( s.c_str(), s.length() );
	oex::CPropertyBag pb = oex::CParser::Deserialize( sData, oex::oexTRUE );
	SQBIND_PropertyBagToMulti( pb, m_lst );
}

CSqMulti::t_Obj CSqMulti::urlencode()
{_STT();
	oex::CPropertyBag pb;
	for ( t_List::iterator it = m_lst.begin(); it != m_lst.end(); it++ )
		pb[ it->first.c_str() ].ToString().Set( it->second.m_val.str().c_str(), it->second.m_val.str().length() );
	return oex::CParser::EncodeUrlParams( pb ).Ptr();
}

void CSqMulti::urldecode( const CSqMulti::t_Obj &s )
{_STT();
	// Deserialize data
	oex::CPropertyBag pb = oex::CParser::DecodeUrlParams( s.c_str() );;
	for ( oex::CPropertyBag::iterator it; pb.List().Next( it ); )
		m_lst[ it.Node()->key.Ptr() ].m_val.str().assign( it->ToString().Ptr(), it->ToString().Length() );
}

void CSqMulti::unset( const CSqMulti::t_Obj &k )
{_STT();
	m_lst.erase( k );
}


void CSqMulti::setstr( const oex::CStr &s )
{_STT();
	m_val = s;
}

void CSqMulti::set( const CSqMulti::t_Obj &v )
{_STT();
	if ( v.length() ) m_val = v;
}

void CSqMulti::copy( CSqMulti &m )
{_STT();
	// Copy string
	m_val = m.m_val;

	// Copy array
	for ( iterator it = m.begin(); it != m.end(); it++ )
		get( it->first )->copy( it->second );
}

CSqMulti::iterator CSqMulti::begin()
{_STT();
	return m_lst.begin();
}

CSqMulti::iterator CSqMulti::end()
{_STT();
	return m_lst.end();
}

CSqMulti::iterator CSqMulti::find ( const CSqMulti::t_Obj &k )
{_STT();
	return m_lst.find( k );
}

int CSqMulti::isset( const CSqMulti::t_Obj &k )
{_STT();
	return ( m_lst.end() != m_lst.find( k ) ) ? 1 : 0;
}

CSqMulti* CSqMulti::get( const CSqMulti::t_Obj &k )
{_STT();
	if ( !k.length() )
	{	if ( !m_def )
			m_def = OexAllocConstruct< CSqMulti >();
		return m_def;
	} // end if
	return &m_lst[ k ];
}

void CSqMulti::move_up( const t_Obj &k )
{_STT();
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
{_STT();
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
{_STT();

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
{_STT();

    // For each item
    for ( t_List::iterator it = m_lst.begin();
          m_lst.end() != it;
          it++ )

        // Is the sub string in the value
        if ( match_pattern( it->second.m_val.str().c_str(), v.c_str() ) )
            return it->first;

    return oexT( "" );
}

SquirrelObject CSqMulti::_newslot( HSQUIRRELVM v )
{_STT();

    StackHandler sa( v );

	// Get key
    const SQChar *pKey = sa.GetString( 2 );
    if ( !pKey || !*pKey )
        return SquirrelObject( v );

	// Get value
    const SQChar *pVal = sa.GetString( 3 );
    if ( !pVal )
        return SquirrelObject( v );

	// Add to list
	m_lst[ pKey ].set( pVal );

	// Find the new value
    t_List::iterator it = m_lst.find( pKey );
    if ( m_lst.end() == it )
        return SquirrelObject( v );

    // Stuff string
    SquirrelObject so( v );
    sq_pushstring( v, it->first.c_str(), (int)it->first.length() );
//    sq_pushstring( v, it->second.c_str(), (int)it->second.length() );
    so.AttachToStackObject( -1 );
    sq_pop( v, 1 );

    return so;

}

/// Internal squirrel function, returns value of specified item
SquirrelObject CSqMulti::_get( HSQUIRRELVM v )
{_STT();

	StackHandler sa( v );

    const SQChar *pKey = sa.GetString( 2 );
    if ( !oexCHECK_PTR( pKey ) || !*pKey )
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
{_STT();

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

        default:
        	break;

    } // end switch

    // ???
    sa.ThrowError( oexT( "Invalid index type" ) );
    return SquirrelObject( v );
}

int CSqMulti::filter( const t_Obj &sFilter, int bInclude )
{
	int nErased = 0;

    // For each item
    for ( t_List::iterator it = m_lst.begin();
          m_lst.end() != it;
          it++ )

		// Is the sub string in the key?
		if ( ( bInclude ? 0 : 1 ) == ( match_pattern( it->first.c_str(), sFilter.c_str() ) ? 0 : 1 ) )
            m_lst.erase( it++ ), nErased++;

	return nErased;
}

/// Returns non-zero if pPattern matches pString
bool CSqMulti::match_pattern( const oex::oexTCHAR *pString, const oex::oexTCHAR *pPattern)
{_STT();

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
{_STT();
	CSqMulti::t_Obj str; while ( t-- ) str += oexT( "\t" ); return str;
}


CSqMulti::t_Obj CSqMulti::_print_r( int nShowVals, t_List &rLst, int nDepth )
{_STT();

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
//				sRet += oexT( "[ " ),
				sRet += ( nShowVals ? it->second.m_val.str() : oexT( "..." ) );
//				sRet += oexT( " ]" );
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
{_STT();

	if ( !size() )
		return m_val.str();

	return _print_r( nShowVals, m_lst );
}


