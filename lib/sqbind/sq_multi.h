/*------------------------------------------------------------------
// sq_multi.h
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
    class CSqMulti : CSqParam
    {
    public:

        /// List object type
		typedef stdString						t_Obj;

        /// List type
		typedef oexStdMap( t_Obj, CSqMulti )	t_List;

		/// List iterator type
		typedef t_List::iterator				iterator;

	public:

		// Must be implemented to be passed as thread param
		virtual t_SqStr serialize();
		virtual void deserialize( const t_SqStr &s );
		virtual void merge( const t_SqStr &s );

		static void _serialize( oex::CPropertyBag &pb, CSqMulti::t_List &lst );


		static void _deserialize( oex::CPropertyBag &pb, CSqMulti::t_List &lst );


    private:

        /// List
        t_List					m_lst;

		/// This objects value
        CSqString				m_val;

		/// Default object
		CSqMulti				*m_def;

    public:

		/// Destructor
		~CSqMulti();

		/// Default constructor
		CSqMulti();

		/// Copy constructor
		CSqMulti( const CSqMulti &m );

		/// Assignment operator
		CSqMulti& operator = ( const CSqMulti &m );

		/// Deserializing constructor
		CSqMulti( const t_Obj &s );

		/// Deserializing constructor
		CSqMulti( const oex::oexTCHAR *s );

        /// Returns a reference to the underlying vector
        t_List& list();

		/// Returns a reference to the list item
		CSqMulti& operator []( const t_Obj &rObj );

		/// Returns a reference to the list item
		CSqMulti& operator []( const oex::oexTCHAR *p );

		/// Returns the value of this node
		CSqString* value();

		/// Returns a string representation of the value
		stdString& str();

        /// Registers the vector class for use with Squirrel
        static void Register( SquirrelVM &vm );

		t_Obj urlencode();

		void urldecode( const t_Obj &s );

		/// Returns non-zero if the specified key exists
		int isset( const t_Obj &k );

		/// returns the number of items in the list
		int size();

		/// Clears the list
		void clear();

        /// Adds an element to the vector
        void set( const t_Obj &v );

        /// Removes the specified element
        void unset( const t_Obj &k );

        /// Adds an element to the vector
        CSqMulti* get( const t_Obj &k );

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

		// Moves the specified item up in the list
		void move_up( const t_Obj &k );

		// Moves the specified item down in the list
		void move_down( const t_Obj &k );

		/// Displays the array in a human readable form
		t_Obj print_r( int nShowVals );

		/// Recursive
		static t_Obj _print_r( int nShowVals, t_List &rLst, int nDepth = 0 );

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
DECLARE_INSTANCE_TYPE_NAME( sqbind::CSqMulti, CSqMulti )
