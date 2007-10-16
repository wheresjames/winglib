/*------------------------------------------------------------------
// parser.h
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

	\endcode

	For a generic string based property bag use CPropertyBag

*/
//==================================================================
template < class T_L, class T_R = T_L > class TPropertyBag
{
public:

	/// Our multi-dimensional string array type
	typedef TAssoList< T_L, TPropertyBag< T_L, T_R > > t_PbArray;

	/// Iterator type
	typedef typename TAssoList< T_L, TPropertyBag< T_L, T_R > >::iterator iterator;

public:

	/// Default constructor
	TPropertyBag() {}

    /// Assignment constructor
    TPropertyBag( oexCONST TPropertyBag &x_rPb )
    {   Assume( x_rPb ); }

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
	TPropertyBag& operator []( T_L x_key ) { return m_lstPb[ x_key ]; }

	//==============================================================
	// operator =
	//==============================================================
	/// Assignment operator
	/**
		\param [in] x_t	-	object
		
		\return Reference to sub class.
	
		\see 
	*/
	TPropertyBag& operator = ( T_R x_t ) 
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
	// operator =
	//==============================================================
    /// Assumes control of the contents of another property bag
    TPropertyBag& operator = ( oexCONST TPropertyBag &x_rPb )
    {   return Assume( x_rPb ); }

	//==============================================================
	// operator T_R&
	//==============================================================
	/// Returns our encapsulated object
    /**
        It was a nice feature, but I had to disable this so that we 
        could efficiently assign and move property bags through
        function parameters and return values.  Also a nice way
        to move a property bag inside of another.
    */
//	operator T_R&() { return m_t.Obj(); }

	/// Returns our encapsulated thing-a-ma-jig reference
	T_R& Value() { return m_t; }
    
	//==============================================================
	// operator ->
	//==============================================================
	/// Pointer difference operator
	/**
		\return Pointer to object
	
		\see 
	*/
//	T_R* operator -> () { return &m_t.Obj(); }

	//==============================================================
	// operator ==
	//==============================================================
	/// Equality operator
	oexBOOL operator == ( T_R x_t ) 
    { 
        if ( !m_t.Ptr() )
            return oexFALSE;
        return *m_t.Ptr() == x_t; 
    }

	//==============================================================
	// operator !=
	//==============================================================
	/// NEQ operator
	oexBOOL operator != ( T_R x_t ) 
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
	oexBOOL IsKey( T_L x_key ) { return m_lstPb.IsKey( x_key ); }

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

	/// Converts to double
	oexDOUBLE ToDouble() 
    {
        if ( !m_t.Ptr() )
            return 0;
        return m_t.Ptr()->ToDouble(); 
    }

	/// Converts to a string
	T_L& ToString() 
    { 
        // Create an object if we don't have one
        if ( !m_t.Ptr() )
            m_t.OexConstruct();

        return *m_t.Ptr(); 
    }

	//==============================================================
	// PrintR()
	//==============================================================
	/// Writes the contents of the property bag into a human readable string
	/**
		\return Human readable representation of the contents
	*/
	T_L PrintR() { return PrintR( *this, T_L() ); }

    TPropertyBag& Assume( oexCONST TPropertyBag &x_pb )
    {
        // Swip values
        m_t.Assume( ( (TPropertyBag&)x_pb ).m_t );
        m_lstPb = ( (TPropertyBag&)x_pb ).m_lstPb;
        return *this;
    }

    // Returns a copy of the property bag
    TPropertyBag Copy()
    {   TPropertyBag pb;
        pb.m_t.Copy( m_t );
		for ( t_PbArray::iterator it; m_lstPb.Next( it ); )
			pb[ it.Node()->key ] = it->Copy();
        return pb;
    }

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
	T_L PrintR( TPropertyBag &pb, T_L &key, oexUINT uDepth = 0 )
	{
		T_L tabs; 
		for ( oexUINT t = 0; t < uDepth; t++ )
			tabs << "  ";
		uDepth++;

		T_L str;
		if ( pb.IsArray() )
		{
			if ( key.Length() ) 
			{
				str << tabs << "\'" << key << "\'" << " = ";

				if ( pb.IsDefaultValue() )
					str << "\'" << pb.ToString() << "\'";

				str << oexNL; 

				tabs << "  "; uDepth++;

			} // end if

			str << tabs << "Array()" oexNL << tabs << "{" oexNL;

			for ( CPropertyBag::iterator it; pb.List().Next( it ); )
				str << PrintR( it, it.Node()->key, uDepth );

			str << tabs << "}" oexNL;

		} // end if

		else
			str << tabs << "\'" << key << "\'" << " = " << "\'" << pb.ToString() << "\'" << oexNL;

		return str;
	}

    /// Erases the specified key
    iterator Unset( T_L key )
    {   return m_lstPb.Unset( key ); }

private:

	/// Our value
	TMem< T_R >					    m_t;

	/// Array of strings
	t_PbArray						m_lstPb; 

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
