// property_bag.h

#pragma once

#include <map>
#include <string>
#include "str.h"

//==================================================================
/// Implements a multi-dimensional property bag
/**

    This class provides functionality for a multi-dimensional
    property bag and type conversions.

    Typical use

	@code

		TPropertyBag< char > pb;

		pb[ "A" ][ "AA" ] = "Hello World!";
		pb[ "A" ][ "AB" ] = (long)1;
		pb[ "B" ][ "BA" ] = (double)3.14159;
		
		for ( long i = 0; i < 4; i++ )
			pb[ "list" ][ i ] = i * 2;

		// Get long value
		long lVal = pb[ "A" ][ "AB" ].ToLong();

		// Get double
		double dVal = pb[ "B" ][ "BA" ].ToDouble();

		// Get string value
		const char *pString = pb[ "list" ][ 0 ].c_str();

	@endcode

*/
//==================================================================
template < typename T > class TPropertyBag
{
public:

    //==================================================================
    // CAutoMem
    //
    /// Just a simple auto pointer
    /**
        This class is a simple auto pointer. It has properties that I
        particularly like for this type of job. I'll quit making my
        own when boost comes with VC... 
    */
    //==================================================================
    template < typename T_OBJ > class CAutoMem
    {
        public:

            /// Default constructor
            CAutoMem() { m_p = 0; }

            /// Destructor
            ~CAutoMem() { release(); }

            /// Release allocated object
            void release() { if ( m_p ) { delete m_p; m_p = 0; } }

            /// Returns a pointer to encapsulated object
            T_OBJ& Obj() { if ( !m_p ) m_p = new T_OBJ; return *m_p; }

            /// Returns a pointer to encapsulated object
            T_OBJ& operator *() { return Obj(); }
            const T_OBJ& operator *() const { return ((CAutoMem*)(this))->Obj(); }

			/// Assignment operator
			T_OBJ& operator = ( const T_OBJ& r ) { return Obj() = r; }

            /// Returns a pointer to the encapsulated object
            operator T_OBJ&() { return Obj(); }

            /// Returns a pointer to the encapsulated object
            T_OBJ* operator ->() { Obj(); return m_p; }
			
        private:

            /// Contains a pointer to the controlled object
            T_OBJ       *m_p;
    };

    /// Unicode friendly string
    typedef typename std::basic_string< T > t_String;

    /// Our multi-dimensional string array type
    typedef typename std::map< t_String, CAutoMem< TPropertyBag< T > > > t_map;

public:

	/// Pass on iterator type
	typedef typename t_map::iterator iterator;
	typedef typename t_map::const_iterator const_iterator;

	/// First item
	iterator begin() { return m_lstSub.begin(); }
	const_iterator begin() const { return m_lstSub.begin(); }

	/// Item beyond the last item, yeah, that's what it is
	iterator end() { return m_lstSub.end(); }
	const_iterator end() const { return m_lstSub.end(); }
    
public:

    /// Default constructor
    TPropertyBag() { }

    //==============================================================
    // TPropertyBag()
    //==============================================================
    /// Constructos object from encoded string
    /**
        \param [in] sStr    -   Encoded array
    */
    TPropertyBag( t_String sStr )
    {   deserialize( sStr );
    }

    //==============================================================
    // TPropertyBag()
    //==============================================================
    /// Constructs object from another property bag
    /**
        \param [in] r    - Reference to property bag to be copied
    */
    TPropertyBag( const TPropertyBag &r )
    {	merge( r ); }

    //==============================================================
    // operator = ()
    //==============================================================
    /// Copies the contents of another property bag
    /**
        \param [in] r    - Reference to property bag to be copied
    */
    TPropertyBag& operator = ( const TPropertyBag &r ) { clear(); merge( r ); return *this; }

    //==============================================================
    // merge
    //==============================================================
    /// Merges the properties of the specified property bag
    /**
        \param [in] r    - Reference to property bag to be merged
    */
	void merge( const TPropertyBag &r )
	{
		// Copy string value
		m_str = r.m_str;

		// Copy array
		stdForeach( const_iterator, it, r )
			m_lstSub[ it->first ]->merge( *it->second );
	}

    /// Releases all memory resources and prepares class for reuse.
    void clear() { m_lstSub.clear(); m_str.clear(); }

	/// Returns the number of items in the property bag
	long size() { return m_lstSub.size(); }

    //==============================================================
    // operator []()
    //==============================================================
    /// Indexes into sub array
    /**
        \param [in] pKey    -   Index key
        
        \return Reference to sub class.
    
        \see 
    */
    TPropertyBag& operator []( const T* pKey ) { return m_lstSub[ pKey ]; }

    //==============================================================
    // operator []()
    //==============================================================
    /// Indexes into sub array
    /**
        \param [in] sKey    -   Index key
        
        \return Reference to sub class.
    
        \see 
    */
    TPropertyBag& operator []( t_String sKey ) { return m_lstSub[ sKey.c_str() ]; }

    //==============================================================
    // operator []()
    //==============================================================
    /// Indexes into sub array
    /**
        \param [in] n   -   Index key
        
        \return Reference to sub class.
    */
    TPropertyBag& operator []( int n ) 
    {	return m_lstSub[ str::ToString< T, t_String >( n ) ]; }

    //==============================================================
    // operator []()
    //==============================================================
    /// Indexes into sub array
    /**
        \param [in] n   -   Index key
        
        \return Reference to sub class.
    */
    TPropertyBag& operator []( unsigned int n ) 
    {	return m_lstSub[ str::ToString< T, t_String >( n ) ]; }

    //==============================================================
    // operator []()
    //==============================================================
    /// Indexes into sub array
    /**
        \param [in] n   -   Index key
        
        \return Reference to sub class.
    */
    TPropertyBag& operator []( long n ) 
    {	return m_lstSub[ str::ToString< T, t_String >( n ) ]; }

    //==============================================================
    // operator []()
    //==============================================================
    /// Indexes into sub array
    /**
        \param [in] n   -   Index key
    
        \return Reference to sub class.
    */
    TPropertyBag& operator []( unsigned long n ) 
    {	return m_lstSub[ str::ToString< T, t_String >( n ) ]; }

    //==============================================================
    // operator []()
    //==============================================================
    /// Indexes into sub array
    /**
        \param [in] n   -   Index key
        
        \return Reference to sub class.
    */
    TPropertyBag& operator []( str::tc_int64 n ) 
    {	return m_lstSub[ str::ToString< T, t_String >( n ) ]; }

    //==============================================================
    // operator []()
    //==============================================================
    /// Indexes into sub array
    /**
        \param [in] n   -   Index key
    
        \return Reference to sub class.
    */
    TPropertyBag& operator []( str::tc_uint64 n ) 
    {	return m_lstSub[ str::ToString< T, t_String >( n ) ]; }

    //==============================================================
    // operator []()
    //==============================================================
    /// Indexes into sub array
    /**
        \param [in] n   -   Index key
        
        \return Reference to sub class.
    */
    TPropertyBag& operator []( double n ) 
    {	return m_lstSub[ str::ToString< T, t_String >( n ) ]; }

    //==============================================================
    // operator = ()
    //==============================================================
    /// Conversion from string object
    t_String operator = ( t_String sStr ) 
    {   m_str = sStr; return m_str; }

    //==============================================================
    // operator = ()
    //==============================================================
    /// Conversion from string
    t_String operator = ( const T* pStr ) 
    {   return m_str = pStr; }


    //==============================================================
    // operator = ()
    //==============================================================
    /// Conversion from int
    t_String operator = ( int n )
    {   return m_str = str::ToString< T, t_String >( n ); }

    //==============================================================
    // operator = ()
    //==============================================================
    /// Conversion from unsigned int
    t_String operator = ( unsigned int n )
    {   return m_str = str::ToString< T, t_String >( n ); }

    //==============================================================
    // operator = ()
    //==============================================================
    /// Conversion from long
    t_String operator = ( long n )
    {   return m_str = str::ToString< T, t_String >( n ); }

    //==============================================================
    // operator = ()
    //==============================================================
    /// Conversion from unsigned long
    t_String operator = ( unsigned long n )
    {   return m_str = str::ToString< T, t_String >( n ); }

    //==============================================================
    // operator = ()
    //==============================================================
    /// Conversion from long
    t_String operator = ( str::tc_int64 n )
    {   return m_str = str::ToString< T, t_String >( n ); }

    //==============================================================
    // operator = ()
    //==============================================================
    /// Conversion from unsigned long
    t_String operator = ( str::tc_uint64 n )
    {   return m_str = str::ToString< T, t_String >( n ); }

    //==============================================================
    // operator = ()
    //==============================================================
    /// Conversion from double
    t_String operator = ( float n )
    {   return m_str = str::ToString< T, t_String >( n ); }

    //==============================================================
    // operator = ()
    //==============================================================
    /// Conversion from double
    t_String operator = ( double n )
    {   return m_str = str::ToString< T, t_String >( n ); }

    //==============================================================
    // T*()
    //==============================================================
    /// Conversion to const T*
    operator const T*() { return c_str(); }

    //==============================================================
    // c_str()
    //==============================================================
    /// Returns NULL terminated const pointer to string buffer
    const T* c_str() { return m_str.c_str(); }

    //==============================================================
    // data()
    //==============================================================
    /// Returns pointer to string buffer (may not be NULL terminated)
    const T* data() { return m_str.c_str(); }
	
    //==============================================================
    // length()
    //==============================================================
    /// Returns length of string
    long length() { return m_str.length(); }

    //==============================================================
    // ToString()
    //==============================================================
    /// Returns reference to string object
    t_String& str() { return m_str; }

    //==============================================================
    // ToString()
    //==============================================================
    /// Returns copy of the string object
    t_String ToString() { return m_str; }

    //==============================================================
    // ToInt64()
    //==============================================================
    /// Converts to long
    str::tc_int64 ToInt64() { return str::StrToInt64( c_str() ); }

    //==============================================================
    // ToUInt64()
    //==============================================================
    /// Converts to long
    str::tc_uint64 ToUInt64() { return str::StrToUInt64( c_str() ); }

    //==============================================================
    // ToLong()
    //==============================================================
    /// Converts to long
    long ToLong() { return str::StrToLong( c_str() ); }

    //==============================================================
    // ToULong()
    //==============================================================
    /// Converts to unsigned long
    unsigned long ToULong() { return str::StrToULong( c_str() ); }

    //==============================================================
    // ToDouble()
    //==============================================================
    /// Converts to double
    double ToDouble() { return str::StrToDouble( c_str() ); }

    //==============================================================
    // ToFloat()
    //==============================================================
    /// Converts to double
    double ToFloat() { return str::StrToFloat( c_str() ); }

    //==============================================================
    // IsArray()
    //==============================================================
    /// Returns non-zero if array elements are present
    bool IsArray() { return 0 < m_lstSub.size(); }

    //==============================================================
    // IsSet()
    //==============================================================
    /// Returns non-zero if the specified key exists
    bool IsSet( const t_String &k ) 
	{	return ( m_lstSub.end() == m_lstSub.find( k ) ) ? false : true; }

private:

    /// Our value
    t_String						m_str;

    /// Array of string maps
    t_map							m_lstSub; 

};

/// Property bag types
/** \see TPropertyBag */
typedef TPropertyBag< char > CPropertyBag8;
#ifndef CII_NO_WCHAR
	typedef TPropertyBag< wchar_t > CPropertyBagW;
#endif
