/*------------------------------------------------------------------
// sq_map.h
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

// namespace
namespace sqbind
{
    /// Squirrel map adaptor class
    class CSqMap
    {
    public:

        /// List object type
		typedef std::tstring                t_Obj;

        /// List type
        typedef std::map< t_Obj, t_Obj >    t_List;

    private:

        /// List
        t_List                              m_lst;

    public:
        
        /// Returns a reference to the underlying vector
        t_List& list() 
        {   return m_lst; }

        /// Registers the vector class for use with Squirrel
        static void Register( SquirrelVM &vm )
        {
            SqPlus::SQClassDef< CSqMap >( vm, _T( "CSqMap" ) )
                    . func( &CSqMap::set,           _T( "set" ) )
                    . func( &CSqMap::get,           _T( "get" ) )
                    . func( &CSqMap::find_sub_k,    _T( "find_sub_k" ) )
                    . func( &CSqMap::find_sub_v,    _T( "find_sub_v" ) )
                    . func( &CSqMap::_get,          _T( "_get" ) )
                    . func( &CSqMap::_nexti,        _T( "_nexti" ) )
                ;
        }

        /// Adds an element to the vector
        void set( const t_Obj &k, const t_Obj &v )
        {
            m_lst[ k ] = v;
        }

        /// Adds an element to the vector
        t_Obj get( const t_Obj &k )
        {
            if ( m_lst.end() == m_lst.find( k ) )
                return _T( "" );

            return m_lst[ k ];
        }

        /// Adds an element to the vector
        t_Obj find_sub_k( const t_Obj &k )
        {
            // For each item
            for ( t_List::iterator it = m_lst.begin();
                  m_lst.end() != it;
                  it++ )

                // Is the sub string in the key?
//                if ( _tcsstr( it->first.c_str(), k.c_str() ) )
                if ( match_pattern( it->first.c_str(), k.c_str() ) )
                    return it->first;

            return _T( "" );
        }

        /// Adds an element to the vector
        t_Obj find_sub_v( const t_Obj &v )
        {
            // For each item
            for ( t_List::iterator it = m_lst.begin();
                  m_lst.end() != it;
                  it++ )

                // Is the sub string in the value
//                if ( _tcsstr( it->second.c_str(), v.c_str() ) )
                if ( match_pattern( it->second.c_str(), v.c_str() ) )
                    return it->first;

            return _T( "" );
        }

    private:

        /// Internal squirrel function, returns value of specified item
        SquirrelObject _get( HSQUIRRELVM v )
        {
            StackHandler sa( v );

            const SQChar *pKey = sa.GetString( 2 );
            if ( !pKey || !*pKey )
                return SquirrelObject( v );

            t_List::iterator it = m_lst.find( pKey );
            if ( m_lst.end() == it )
                return SquirrelObject( v );

            // Stuff string
            SquirrelObject so( v );
	        sq_pushstring( v, it->second.c_str(), (int)it->second.length() );
	        so.AttachToStackObject( -1 );
	        sq_pop( v, 1 );

            return so;
        }

        /// Internal squirrel function used to iterate list items
        SquirrelObject _nexti( HSQUIRRELVM v )
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
            sa.ThrowError( _T( "Invalid index type" ) );
            return SquirrelObject( v );
        }

        /// Returns non-zero if pPattern matches pString
        static bool match_pattern( const TCHAR *pString, const TCHAR *pPattern)
        {
	        if ( !pString || !pPattern ) 
                return false;

	        unsigned int i = 0, p = 0;

	        // Skip multiple '*'
	        while ( _T( '*' ) == pPattern[ p ] && _T( '*' ) == pPattern[ p + 1 ] ) 
                p++;

	        // Check for the 'any' pattern
	        if ( pPattern[ p ] == _T( '*' ) && pPattern[ p + 1 ] == 0 )
		        return true;

	        // While we're not at the end
	        while ( pString[ i ] != 0 )
	        {
		        // Are we on a wildcard?
		        if ( _T( '*' ) == pPattern[ p ] )
		        {
			        // Are we matching everything?
			        if ( pPattern[ p + 1 ] == 0 ) 
                        return true;

			        // Check for pattern advance
			        if (	pString[ i ] == pPattern[ p + 1 ] ||

					        (
						        pString[ i ] >= _T( 'a' ) && pString[ i ] <= _T( 'z' ) && 
						        ( pString[ i ] - ( _T( 'a' ) - _T( 'A' ) ) ) == pPattern[ p + 1 ] 
					        ) ||

					        (
						        pString[ i ] >= _T( 'A' ) && pString[ i ] <= _T( 'Z' ) && 
						        ( pString[ i ] + ( _T( 'a' ) - _T( 'A' ) ) ) == pPattern[ p + 1 ] 
					        ) 

				        ) p += 2;
        				
		        } // end if

		        // Just accept this character
		        else if ( _T( '?' ) == pPattern[ p ] ) 
                    p++;

		        // Otherwise advance if equal
		        else if ( pString[ i ] == pPattern[ p ] ) 
                    p++;

		        // Case insensitive
		        else if (	(
						        pString[ i ] >= _T( 'a' ) && pString[ i ] <= _T( 'z' ) && 
						        ( pString[ i ] - ( _T( 'a' ) - _T( 'A' ) ) ) == pPattern[ p ] 
					        ) ||
					        (
						        pString[ i ] >= _T( 'A' ) && pString[ i ] <= _T( 'Z' ) && 
						        ( pString[ i ] + ( _T( 'a' ) - _T( 'A' ) ) ) == pPattern[ p ] 
					        ) 
				        ) p++;

		        // Back up in the pattern
		        else while ( p && pPattern[ p ] != _T( '*' ) ) p--;

		        // Return true if we're at the end of the pattern
		        if ( pPattern[ p ] == 0 ) 
                    return true;

		        // Next char
		        i++;

	        } // end while

	        // Skip wild cards
	        while ( pPattern[ p ] == _T( '*' ) ) p++;

	        // Did we match?
	        return ( pPattern[ p ] == 0 );
        }

    };

}; // end namespace

// Declare type for use as squirrel parameters
DECLARE_INSTANCE_TYPE_NAME( sqbind::CSqMap, CSqMap )
