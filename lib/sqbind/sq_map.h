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
		typedef stdString						t_Obj;

        /// List type
		typedef oexStdMap( t_Obj, t_Obj )		t_List;

		/// List iterator type
		typedef t_List::iterator				iterator;

    private:

        /// List
        t_List					m_lst;

    public:

		/// Default constructor
		CSqMap();

		/// Deserializing constructor
		CSqMap( const t_Obj &s );

		/// Deserializing constructor
		CSqMap( const oex::oexTCHAR *s );

        /// Returns a reference to the underlying vector
        t_List& list();

		/// Returns a reference to the list item
		t_Obj& operator []( const t_Obj &rObj );

		/// Returns a reference to the list item
		t_Obj& operator []( const oex::oexTCHAR *p );

        /// Registers the vector class for use with Squirrel
        static void Register( sqbind::VM vm );

		t_Obj serialize();

		void deserialize( const t_Obj &s );

		void merge( const t_Obj &s );

		void clear();

		t_Obj urlencode();

		void urldecode( const t_Obj &s );

		/// Returns non-zero if the specified key exists
		oex::oexBOOL isset( const CSqMap::t_Obj &k );

		/// returns the number of items in the list
		int size();

        /// Adds an element to the vector
        void set( const t_Obj &k, const t_Obj &v );

        /// Removes the specified element
        void unset( const t_Obj &k );

        /// Adds an element to the vector
        t_Obj get( const t_Obj &k );

        /// Adds an element to the vector
        t_Obj find_key( const t_Obj &k );

        /// Adds an element to the vector
        t_Obj find_value( const t_Obj &v );

		/// Returns the first item in the list
        iterator begin();

		/// Returns the last item in the list
        iterator end();

		/// Returns the specified list item
        iterator find ( const t_Obj &k );

		/// Displays the array in a human readable form
		t_Obj print_r( int nShowVals );

        /// Internal squirrel function, returns value of specified item
        SquirrelObject _get( HSQUIRRELVM v );

        /// Internal squirrel function used to iterate list items
        SquirrelObject _nexti( HSQUIRRELVM v );

    private:

        /// Returns non-zero if pPattern matches pString
        static bool match_pattern( const oex::oexTCHAR *pString, const oex::oexTCHAR *pPattern);

    };

}; // end namespace

// Declare type for use as squirrel parameters
_SQBIND_DECLARE_INSTANCE( sqbind::CSqMap, CSqMap )
