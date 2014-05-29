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
		enum eFlags
		{
			efList = 0x00000001
		};
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

		CSqMulti* mset( CSqMulti *m );
		CSqMulti* mmerge( CSqMulti *m );

		t_SqStr serialize_filter( const t_SqStr &sFilter, int x_bInclude, int x_bIgnoreCase );
		CSqMulti* deserialize_filter( const t_SqStr &s, const t_SqStr &sFilter, int x_bInclude, int x_bIgnoreCase );

		t_SqStr getJSON();
		CSqMulti* setJSON( const t_SqStr &s );
		CSqMulti* mergeJSON( const t_SqStr &s );

		t_SqStr getMIME( int bCaseSensitive );
		CSqMulti* setMIME( const t_SqStr &s, int bCaseSensitive );
		CSqMulti* mergeMIME( const t_SqStr &s, int bCaseSensitive );

		t_SqStr getURL();
		CSqMulti* setURL( const t_SqStr &s );
		CSqMulti* mergeURL( const t_SqStr &s );

	private:

		/// List
		t_List					m_lst;

		/// This objects value
		CSqString				m_val;

		/// Default object
		CSqMulti				*m_def;

		/// Property bag flags
		long					m_flags;

	public:

		SQBIND_CLASS_CTOR_BEGIN( CSqMulti )
			_SQBIND_CLASS_CTOR1( CSqMulti, OT_STRING ) ( sa.GetString( 2 ), sq_getsize( x_v, 2 ) )
		SQBIND_CLASS_CTOR_END( CSqMulti )

		/// Destructor
		virtual ~CSqMulti();

		/// Default constructor
		CSqMulti();

		/// Copy from pointer
		CSqMulti( const CSqMulti *m )
		{	m_flags = 0; if ( m ) *this = *m; }

		/// Copy constructor
		CSqMulti( const CSqMulti &m );

		/// Assignment operator
		CSqMulti& operator = ( const CSqMulti &m );

		/// Deserializing constructor
		CSqMulti( const t_Obj &s );

		/// Deserializing constructor
		CSqMulti( const oex::oexTCHAR *s, SQINT sz = 0 );

		/// Deserializing constructor
		CSqMulti( const oex::CStrList &l );

		/// Assignment operator
		CSqMulti& operator = ( const oex::CStrList &l );

		/// Returns a reference to the underlying vector
		t_List& list();

		/// Returns a reference to the list item
		CSqMulti& operator []( const CSqMulti &m );

		/// Returns a reference to the list item
		CSqMulti& operator []( const t_Obj &rObj );

		/// Returns a reference to the list item
		CSqMulti& operator []( const oex::oexTCHAR *p );

		/// Assignment operator
		CSqMulti& operator = ( const stdString &s );

		/// Assignment operator
		CSqMulti& operator = ( const oex::oexTCHAR *p );

		/// Returns non-zero if this is the list flag is set
		int getList() { return 0 != ( m_flags & efList ); }    

		/// Sets the list flag
		void setList( int b ) 
		{
			if ( b ) 
				m_flags |= efList;
			else
				m_flags &= ~efList;
		}

		/// Returns the flags
		long getFlags() { return m_flags; }

		/// Sets the flags
		void setFlags( long f ) { m_flags = f; }

		/// Returns a reference to the string object
		operator stdString&() { return str(); }

		/// Returns a const char pointer
		const stdChar* c_str() const { return m_val.c_str().c_str(); }

		/// Returns the length of the string
		int length() const { return m_val.c_str().length(); }

		/// Returns the value of this node
		CSqString* value();

		/// Returns a string representation of the value
		stdString& str();

		/// Returns a string representation of the value, if empty, returns def
		stdString def( const sqbind::stdString &k, const sqbind::stdString &def );

		/// Returns a string representation of the value
		stdString str_limit( int maxchars, const stdString &sAppend );

		/// Returns a string representation of the value, if empty, returns def
		stdString str_def( const sqbind::stdString &def )
		{	sqbind::stdString v = str(); if ( !v.length() ) return def; return v; }

		/// Returns a binary representation of the value
		CSqBinary bin();

		/// Returns a integer representation of the value
		SQInteger toint();

		/// Returns a integer representation of the value within the specified range
		SQInteger toint_range( SQInteger min, SQInteger max )
		{	return oex::cmn::Range( toint(), min, max ); }

		/// Returns a integer representation of the value
		int toint32();

		/// Returns a integer representation of the value
		oex::oexINT64 toint64();

		/// Returns a float representation of the value
		float tofloat();

		/// Returns a float representation of the value
		float tofloat_range( float min, float max )
		{	return oex::cmn::Range( tofloat(), min, max ); }

		/// Returns a double representation of the value
		double todouble();

		/// Returns a double representation of the value
		double todouble_range( double min, double max )
		{	return oex::cmn::Range( todouble(), min, max ); }

		/// Returns the length of the string value
		int len();

		/// Returns url encoded string
		stdString str_urlenc();

		/// Returns url decoded string
		stdString str_urldec();

		/// Return html encoded string
		stdString str_htmlenc();

		/// Return html encoded string and limits the length
		stdString str_htmlenc_limit( int maxchars, const stdString &sAppend );

		/// Returns html decoded string
		stdString str_htmldec();

		/// Returns zlib compressed string
		stdString str_compress();

		/// Returns zlib decompressed string
		stdString str_uncompress();

		/// Returns json encoded string
		stdString str_json_encode();

		/// Returns json decoded string
		stdString str_json_decode();

		/// Returns base16 encoded string
		stdString str_base16_encode();

		/// Returns base16 decoded string
		stdString str_base16_decode();

		/// Returns base64 encoded string
		stdString str_base64_encode();

		/// Returns base64 decoded string
		stdString str_base64_decode();

		/// Creates a size string like 1.3MB, 44.5GB, etc...
		/**
			@param [in] dDiv	- Divisor, default is 1024
			@param [in] nDigits	- Number of digits trailing the decimal
			@param [in] sSuffix	- Optional comma sperated suffix array
								  If empty string, defaults are "Bytes,KB,MB,GB,TB,PB,EB,ZB,YB,BB"
		*/
		stdString str_size( double dDiv, int nDigits, const stdString &sSuffix );
		
		/// Returns url encoded string
		CSqBinary bin_urlenc();

		/// Returns url decoded string
		CSqBinary bin_urldec();

		/// Return html encoded string
		CSqBinary bin_htmlenc();

		/// Returns html decoded string
		CSqBinary bin_htmldec();

		/// Returns zlib compressed string
		CSqBinary bin_compress();

		/// Returns zlib decompressed string
		CSqBinary bin_uncompress();

		/// Returns base16 encoded string
		CSqBinary bin_base16_encode();

		/// Returns base16 decoded string
		CSqBinary bin_base16_decode();

		/// Returns base64 encoded string
		CSqBinary bin_base64_encode();

		/// Returns base64 decoded string
		CSqBinary bin_base64_decode();

		/// Returns non-zero if string value matches pattern
		int match( const t_Obj &sPattern );
		
		/// Registers the vector class for use with Squirrel
		static void Register( sqbind::VM vm );

		t_Obj urlencode();

		void urldecode( const t_Obj &s );

		/// Merges in the specified array
		CSqMulti& add( CSqMulti &m );

		/// Formats value according to template
		t_Obj tmpl( const t_Obj &tmpl );

		/// Joins all elements in the array
		t_Obj join( const t_Obj &glue, const t_Obj &tmpl );

		/// Parses a key/value string
		int parse( const t_Obj &s, const t_Obj &glue, const t_Obj &sep, int bTrimWs );

		/// Returns non-zero if the specified key exists
		int isset( const t_Obj &k );

		/// returns the number of items in the list
		int size();

		/// Clears the list
		void clear();

		/// Copys a key
		void copy( CSqMulti &m );

		/// Adds an element
		void set( const t_Obj &v );

		/// Adds a string element
		void setstr( const oex::CStr &s );

		/// Removes the specified element
		void unset( const t_Obj &k );

		/// Filters items
		int filter( const t_Obj &sFilter, int bInclude );

		/// Extracts the comma separated items sItems = "item1,item2,..."
		CSqMulti extract( const t_Obj &sItems );
		
		/// Gets an element
		CSqMulti* get( const t_Obj &k );

		/// Gets an element using a path
		CSqMulti* at( const t_Obj &path );
		
		/// Erases the element at the specified path
		int erase_at( const t_Obj &path );

		/// Gets an element using a path
		CSqMulti* ats( const t_Obj &path, const t_Obj &sep );
		
		/// Erases the element at the specified path
		int erase_ats( const t_Obj &path, const t_Obj &sep );

		/// Gets the first element after the skip count
		CSqMulti* first( int skip );
		
		/// Gets last element after the skip count
		CSqMulti* last( int skip );
		
		/// Gets the first key after the skip count
		stdString first_key( int skip );
		
		/// Gets last key after the skip count
		stdString last_key( int skip );
		
		/// Finds an element
		t_Obj find_key( const t_Obj &k );

		/// Finds an element by value
		t_Obj find_value( const t_Obj &v );

		/// Returns the array element with the corrisponding key value matches
		CSqMulti* find_obj( const CSqMulti::t_Obj &k, const CSqMulti::t_Obj &v );

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

		// Left shifts an array returning the item shifted out
		CSqMulti lshift();
		
		/// Displays the array in a human readable form
		t_Obj print_r( int nShowVals );
		
		/// Get the number of differences between two arrays
		CSqMulti diff_count( CSqMulti *p );

		/// Recursive
		static t_Obj _print_r( int nShowVals, t_List &rLst, int nDepth = 0 );

		/// Internal squirrel function, returns value of specified item
		SquirrelObject _get( HSQUIRRELVM v );

		/// Internal squirrel function used to iterate list items
		SquirrelObject _nexti( HSQUIRRELVM v );

		/// Internal squirrel function used to add a new item
		SquirrelObject _newslot( HSQUIRRELVM v );

	public:

		/// Converts from oex property bag
		void fromPb( oex::CPropertyBag &pb );

		/// Converts to oex property bag
		void toPb( oex::CPropertyBag &pb );

	private:

		/// Returns non-zero if pPattern matches pString
		static bool match_pattern( const oex::oexTCHAR *pString, const oex::oexTCHAR *pPattern);

	};

}; // end namespace

// Declare type for use as squirrel parameters
SQBIND_DECLARE_INSTANCE( sqbind::CSqMulti, CSqMulti )
