/*------------------------------------------------------------------
// property_bag.h
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

//==================================================================
// TPropertyBag
//
/// Implements a multi-dimensional property bag
/**

	This class provides functionality for a multi-dimensional
	property bag.

	\code

	TPropertyBag< oexINT, oexINT > pbii;

	pbii[ 2 ] = 22;


	TPropertyBag< oexINT, CStr > pbis;

	pbis[ 2 ] = "Testing";


	TPropertyBag< CStr, CStr > pbis;

	pbis[ "Hello" ] = "World";


	// Convert to std map of strings
	std::map< std::tstring, std::tstring > mapCnv;
	for ( oex::CPropertyBag::iterator it; pb.List().Next( it ); )
		mapCnv[ it.Node()->key.Ptr() ] = it->ToString().Ptr();

	// Different char size?
	std::map< std::tstring, std::tstring > mapCnv;
	for ( oex::CPropertyBag8::iterator it; pb.List().Next( it ); )
		mapCnv[ oexMbToStrPtr( it.Node()->key.Ptr() ) ] = oexMbToStrPtr( it->ToString().Ptr() );

	\endcode

	For a generic string based property bag use CPropertyBag

*/
//==================================================================
template < class T_L, class T_R = T_L > class TPropertyBag
{
public:

	/// Property bag flags
	enum eFlags
	{
		efList = 0x00000001
	};

public:

	/// Key type
	typedef T_L t_key;

	/// Value type
	typedef T_R t_val;

	/// Our multi-dimensional string array type
	typedef TAssoList< t_key, TPropertyBag< t_key, t_val > > t_PbArray;

	/// Iterator type
	typedef typename t_PbArray::iterator iterator;

public:

	/// Default constructor
	TPropertyBag() { m_flags = 0; }

    /// Assignment constructor
    TPropertyBag( oexCONST TPropertyBag &x_rPb )
    {	m_flags = 0; Assume( x_rPb ); }

	/// Destructor
	virtual ~TPropertyBag() { Destroy(); }

	//==============================================================
	// Destroy()
	//==============================================================
	/// Releases all memory resources and prepares class for reuse.
	void Destroy() { m_lstPb.Destroy(); }

	//==============================================================
	// List()
	//==============================================================
	/// Returns a reference to the encapsulated list
	t_PbArray& List() { return m_lstPb; }

	//==============================================================
	// Size()
	//==============================================================
	/// Returns the size of the sub array
	oexUINT Size() { return m_lstPb.Size(); }

	//==============================================================
	// operator []
	//==============================================================
	/// Indexes into sub array
	/**
		\param [in] x_key	-	Index key

		\return Reference to sub class.

		\see
	*/
	TPropertyBag& operator []( t_key x_key ) { return m_lstPb[ x_key ]; }

	//==============================================================
	// operator =
	//==============================================================
	/// Assignment operator
	/**
		\param [in] x_t	-	object

		\return Reference to sub class.

		\see
	*/
	TPropertyBag& operator = ( t_val x_t )
    {
        if ( !m_t.Ptr() )
            m_t.OexConstruct().Ptr();

        if ( m_t.Ptr() )
            *m_t.Ptr() = x_t;

        return *this;
	}

/*
#if defined( oexUNICODE )

	TPropertyBag& operator = ( CStr8 &v )
    {
        if ( !m_t.Ptr() )
            m_t.OexConstruct().Ptr();

        m_t = oexStr8ToStr( v );
        return *this;
    }

#else

	TPropertyBag& operator = ( CStrW &v )
    {
        if ( !m_t.Ptr() )
            m_t.OexConstruct().Ptr();

        m_t = oexStrWToStr( v );
        return *this;
    }

#endif
*/

	//==============================================================
	// at
	//==============================================================
	/// Parses string path
	/**
		\param [in] x_key	-	Index key

		pb.at( "k1.k2.k3" ).ToString()

		is equiv to

		pb[ "k1" ][ "k2" ][ "k3" ].ToString()

		\return Reference to sub class.

		\see
	*/
	template < typename T_CHAR >
		TPropertyBag& at( TStr< T_CHAR > x_key, TStr< T_CHAR > x_sep = oexTT( T_CHAR, "." ) )
		{
			t_key p = x_key.Parse( x_sep.Ptr() );
			x_key.Skip( x_sep.Ptr() );

			if ( p.Length() && x_key.Length() )
				return m_lstPb[ p ].at( x_key, x_sep );

			else if ( p.Length() )
				return m_lstPb[ p ];

			else if ( x_key.Length() )
				return m_lstPb[ x_key ];

			return *this;
		}

	template < typename T_CHAR >
		TPropertyBag& at( const T_CHAR *x_key, const T_CHAR *x_sep = oexTT( T_CHAR, "." ) )
		{
			// If valid data
			if ( oexCHECK_PTR( x_key ) && oexCHECK_PTR( x_sep ) )
			{
				t_key key = x_key;
				t_key p = key.Parse( x_sep );
				key.Skip( x_sep );

				if ( p.Length() && key.Length() )
					return m_lstPb[ p ].at( key, t_key( x_sep ) );

				else if ( p.Length() )
					return m_lstPb[ p ];

				else if ( key.Length() )
					return m_lstPb[ key ];

			} // end if

			return *this;
		}

	//==============================================================
	// find_at
	//==============================================================
	/// Parses string path
	/**
		\param [in] x_key	-	Index key

		pb.at( "k1.k2.k3" ).ToString()

		is equiv to

		pb[ "k1" ][ "k2" ][ "k3" ].ToString()

		\return Reference to sub class.

		\see
	*/
	template < typename T_CHAR >
		iterator find_at( TStr< T_CHAR > x_key, TStr< T_CHAR > x_sep = oexTT( T_CHAR, "." ) )
		{
			t_key p = x_key.Parse( x_sep.Ptr() );
			x_key.Skip( x_sep.Ptr() );

			if ( p.Length() && x_key.Length() )
				return m_lstPb[ p ].find_at( x_key, x_sep );

			else if ( p.Length() )
				return m_lstPb.Find( p );

			else if ( x_key.Length() )
				return m_lstPb.Find( x_key );

			return iterator();
		}

	template < typename T_CHAR >
		iterator find_at( const T_CHAR *x_key, const T_CHAR *x_sep = oexTT( T_CHAR, "." ) )
		{
			// If valid data
			if ( oexCHECK_PTR( x_key ) && oexCHECK_PTR( x_sep ) )
			{
				t_key key = x_key;
				t_key p = key.Parse( x_sep );
				key.Skip( x_sep );

				if ( p.Length() && key.Length() )
					return m_lstPb[ p ].find_at( key, t_key( x_sep ) );

				else if ( p.Length() )
					return m_lstPb.Find( p );

				else if ( key.Length() )
					return m_lstPb.Find( key );

			} // end if

			return iterator();
		}

	//==============================================================
	// erase_at
	//==============================================================
	/// Parses string path
	/**
		\param [in] x_key	-	Index key

		pb.at( "k1.k2.k3" ).ToString()

		is equiv to

		pb[ "k1" ][ "k2" ][ "k3" ].ToString()

		\return Reference to sub class.

		\see
	*/
	template < typename T_CHAR >
		iterator erase_at( TStr< T_CHAR > x_key, TStr< T_CHAR > x_sep = oexTT( T_CHAR, "." ) )
		{
			t_key p = x_key.Parse( x_sep.Ptr() );
			x_key.Skip( x_sep.Ptr() );

			if ( p.Length() && x_key.Length() )
				return m_lstPb[ p ].erase_at( x_key, x_sep );

			else if ( p.Length() )
				return m_lstPb.Unset( p );

			else if ( x_key.Length() )
				return m_lstPb.Unset( x_key );

			return iterator();
		}

	template < typename T_CHAR >
		iterator erase_at( const T_CHAR *x_key, const T_CHAR *x_sep = oexTT( T_CHAR, "." ) )
		{
			// If valid data
			if ( oexCHECK_PTR( x_key ) && oexCHECK_PTR( x_sep ) )
			{
				t_key key = x_key;
				t_key p = key.Parse( x_sep );
				key.Skip( x_sep );

				if ( p.Length() && key.Length() )
					return m_lstPb[ p ].erase_at( key, t_key( x_sep ) );

				else if ( p.Length() )
					return m_lstPb.Unset( p );

				else if ( key.Length() )
					return m_lstPb.Unset( key );

			} // end if

			return iterator();
		}

    //==============================================================
    // setFlags()
    //==============================================================
    /// Sets the flags
    void setFlags( long f ) { m_flags = f; }    

    //==============================================================
    // getFlags()
    //==============================================================
    /// Returns the flags
    long getFlags() const { return m_flags; }    

    //==============================================================
    // is_list()
    //==============================================================
    /// Returns non-zero if this is the list flag is set
    oexBOOL is_list() const { return 0 != ( m_flags & efList ); }    

    //==============================================================
    // is_list()
    //==============================================================
    /// Sets the list flag
	void is_list( oexBOOL b ) 
	{
		if ( b ) 
			m_flags |= efList;
		else
			m_flags &= ~efList;
	}		

	//==============================================================
	// Returns the number of differences between two property bags
	//==============================================================
	/**
		@param [in] x_rPb		-	Pointer to property bag to use with comparison.
		@param [in] added		-	Pointer to variable that keeps the added values count
		@param [in] removed		-	Pointer to variable that keeps the removed values count
		@param [in] modified	-	Pointer to variable that keeps the modified values count
	*/
	oexLONG diff_count( TPropertyBag &x_rPb, oexLONG *added, oexLONG *removed, oexLONG *modified )
	{	return diff_count( *this, x_rPb, added, removed, modified ); }

	static oexLONG diff_count( TPropertyBag &a, TPropertyBag &b, oexLONG *added, oexLONG *removed, oexLONG *modified )
	{
		oexLONG lRet = 0;

		// Check for added or modified keys
		for ( iterator it; a.List().Next( it ); )
			if ( it->IsArray() )
			{
				if ( !b.IsKey( it.Node()->key ) )
					lRet++, added ? (*added)++ : 0;
					
				else if ( !b[ it.Node()->key ].IsArray() )
					lRet++, modified ? (*modified)++ : 0;
					
				else
					lRet += diff_count( *it, b[ it.Node()->key ], added, removed, modified );

			} // end if

			else if ( !b.IsKey( it.Node()->key ) )
				lRet++, added ? (*added)++ : 0;

			else if ( b[ it.Node()->key ].IsArray() 
					  || it->ToString() != b[ it.Node()->key ].ToString() )
				lRet++, modified ? (*modified)++ : 0;

		// Check for removed keys
		for ( iterator it; b.List().Next( it ); )
			if ( !a.IsKey( it.Node()->key ) )
				lRet++, removed ? (*removed)++ : 0;

		return lRet;

	}

	//==============================================================
	// Returns the differences between two property bags
	//==============================================================
//	oexLONG diff( oexCONST TPropertyBag &x_rPb, TPropertyBag &x_rDiff );


	//==============================================================
	// operator =
	//==============================================================
    /// Assumes control of the contents of another property bag
    TPropertyBag& operator = ( oexCONST TPropertyBag &x_rPb )
    {   return Assume( x_rPb ); }

	//==============================================================
	// operator t_val&
	//==============================================================
	/// Returns our encapsulated object
    /**
        It was a nice feature, but I had to disable this so that we
        could efficiently assign and move property bags through
        function parameters and return values.  Also a nice way
        to move a property bag inside of another.
    */
//	operator t_val&() { return m_t.Obj(); }

	/// Returns our encapsulated thing-a-ma-jig reference
	t_val& Value() { return m_t; }

	//==============================================================
	// operator ->
	//==============================================================
	/// Pointer difference operator
	/**
		\return Pointer to object

		\see
	*/
//	t_val* operator -> () { return &m_t.Obj(); }

	//==============================================================
	// operator ==
	//==============================================================
	/// Equality operator
	oexBOOL operator == ( t_val x_t )
    {
        if ( !m_t.Ptr() )
            return oexFALSE;
        return *m_t.Ptr() == x_t;
    }

	//==============================================================
	// operator !=
	//==============================================================
	/// NEQ operator
	oexBOOL operator != ( t_val x_t )
    {
        if ( !m_t.Ptr() )
            return oexTRUE;
        return *m_t.Ptr() != x_t;
    }

	//==============================================================
	// IsArray()
	//==============================================================
	/// Returns non-zero if array elements are present
	oexBOOL IsArray() { return 0 < m_lstPb.Size(); }

	//==============================================================
	// IsSet()
	//==============================================================
	/// Returns non-zero if there is a valid data object
	oexBOOL IsSet() { return IsArray() || m_t.Ptr()->Length(); }

	//==============================================================
	// IsDefaultValue()
	//==============================================================
	// Returns non zero if there is a default value
	oexBOOL IsDefaultValue()
    { return ( m_t.Ptr() && m_t.Ptr()->Length() ) ? oexTRUE : oexFALSE; }

	//==============================================================
	// IsKey()
	//==============================================================
	/// Returns non-zero if the specified key exists
	/**
		\param [in] x_key

		\return

		\see
	*/
	oexBOOL IsKey( t_key x_key ) { return m_lstPb.IsKey( x_key ); }

	/// Converts to long
	oexLONG ToLong()
    {
        if ( !m_t.Ptr() )
            return 0;

        return m_t.Ptr()->ToLong();
    }

	/// Converts to unsigned long
	oexULONG ToULong()
    {
        if ( !m_t.Ptr() )
            return 0;
        return m_t.Ptr()->ToULong();
    }

	/// Converts to int
	oexINT ToInt()
    {
        if ( !m_t.Ptr() )
            return 0;

        return m_t.Ptr()->ToInt();
    }

	/// Converts to unsigned int
	oexUINT ToUInt()
    {
        if ( !m_t.Ptr() )
            return 0;
        return m_t.Ptr()->ToUInt();
    }

	/// Converts to int64
	oexINT64 ToInt64()
    {
        if ( !m_t.Ptr() )
            return 0;

        return m_t.Ptr()->ToInt64();
    }

	/// Converts to unsigned int64
	oexUINT64 ToUInt64()
    {
        if ( !m_t.Ptr() )
            return 0;
        return m_t.Ptr()->ToUInt64();
    }

	/// Converts to float
	oexFLOAT ToFloat()
    {
        if ( !m_t.Ptr() )
            return 0;
        return m_t.Ptr()->ToFloat();
    }

	/// Converts to double
	oexDOUBLE ToDouble()
    {
        if ( !m_t.Ptr() )
            return 0;
        return m_t.Ptr()->ToDouble();
    }

	/// Converts to a string
	t_key& ToString()
    {
        // Create an object if we don't have one
        if ( !m_t.Ptr() )
            m_t.OexConstruct();

        return *m_t.Ptr();
    }

    TPropertyBag& Assume( oexCONST TPropertyBag &x_pb )
    {
        // Swip values
        m_t.Assume( ( (TPropertyBag&)x_pb ).m_t );
        m_lstPb = ( (TPropertyBag&)x_pb ).m_lstPb;
		m_flags = ( (TPropertyBag&)x_pb ).m_flags;
        return *this;
    }

    // Returns a copy of the property bag
    TPropertyBag Copy()
    {   TPropertyBag pb;
        pb.m_t.Copy( m_t );
		for ( iterator it; m_lstPb.Next( it ); )
			pb[ it.Node()->key ] = it->Copy();
		pb.m_flags = m_flags;
        return pb;
    }

//#if defined( OEX_WINDOWS ) && !defined( OEX_ARM )
//#if defined( oexDEBUG )

	//==============================================================
	// PrintR()
	//==============================================================
	/// Writes the contents of the property bag into a human readable string
	/**
		\return Human readable representation of the contents
	*/
	t_key PrintR() { return PrintR( *this, t_key() ); }

	//==============================================================
	// PrintR()
	//==============================================================
	/// Writes the contents of the property bag into a human readable string
	/**
		\param [in] pb			-	Property bag reference
		\param [in] key			-	Key of interest
		\param [in] uDepth		-	Current depth, to track recursion

		\return Human readable representation of the contents

		\see
	*/
	template < typename T_CH >
		t_key PrintR( TPropertyBag< TStr< T_CH >, t_val > &pb, oexCONST TStr< T_CH > &key, oexUINT uDepth = 0 )
	{
		typedef TStr< T_CH > t_key;

		t_key tabs;
		for ( oexUINT t = 0; t < uDepth; t++ )
			tabs << oexTT( T_CH, "  " );
		uDepth++;

		t_key str;
		if ( pb.IsArray() )
		{
			if ( key.Length() )
			{
				str << tabs << oexTT( T_CH, "\'" ) << key << oexTT( T_CH, "\'" ) << oexTT( T_CH, " = " );

				if ( pb.IsDefaultValue() )
					str << oexTT( T_CH, "\'" ) << pb.ToString() << oexTT( T_CH, "\'" );

				str << oexNL;

				tabs << oexTT( T_CH, "  " ); uDepth++;

			} // end if

			str << tabs << oexTT( T_CH, "Array()" ) << oexNL << tabs << oexTT( T_CH, "{" ) << oexNL;

			for ( iterator it; pb.List().Next( it ); )
				str << PrintR( it.Obj(), it.Node()->key, uDepth );

			str << tabs << oexTT( T_CH, "}" ) << oexNL;

		} // end if

		else
			str << tabs << oexTT( T_CH, "\'" ) << key << oexTT( T_CH, "\'" ) << oexTT( T_CH, " = " ) << oexTT( T_CH, "\'" ) << pb.ToString() << oexTT( T_CH, "\'" ) << oexNL;

		return str;
	}
//#endif

    /// Erases the specified key
	iterator Unset( t_key key )
    {   return m_lstPb.Unset( key ); }

private:

	/// Our value
	TMem< t_val >				    m_t;

	/// Array of strings
	t_PbArray						m_lstPb;

	/// Propety bag flags
	oexUINT							m_flags;

};

//==================================================================
// TPropertyBag
//
/// Generic property bag type
/**

	This class provides functionality for a multi-dimensional
	property bag.

	Generic property bag use

	\code

	CPropertyBag arr1, arr2;

	arr1[ "A" ][ "AA" ] = "Hello World!";
	arr1[ "A" ][ "AB" ] = (long)1;
	arr1[ "B" ][ "BA" ] = (double)3.14159f;

	for ( long i = 0; i < 4; i++ )
		arr1[ "list" ][ i ] = i * 2;

	// Get long value
	long lVal = arr2[ "A" ][ "AB" ]->ToLong();

	// Get double
	double dVal = arr2[ "B" ][ "BA" ]->ToDouble();

	// Get string value
	LPCTSTR pString = arr2[ "list" ][ 0 ]->ToString();

	\endcode

	\see TPropertyBag

*/
//==================================================================
typedef TPropertyBag< CStr > CPropertyBag;

typedef TPropertyBag< CStrW > CPropertyBagW;

typedef TPropertyBag< CStr8 > CPropertyBag8;

typedef TPropertyBag< CStr16 > CPropertyBag16;

typedef TPropertyBag< CStr32 > CPropertyBag32;
