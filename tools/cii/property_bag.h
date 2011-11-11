// property_bag.h

#pragma once

#include <map>
#include <string>
#include "typecnv.h"

//==================================================================
/// Implements a multi-dimensional property bag
/**

    This class provides functionality for a multi-dimensional
    property bag and type conversions.

    Typical use

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
    const char *pString = pb[ "list" ][ 0 ];

*/
//==================================================================
template < class T > class TPropertyBag
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
    template < class T > class CAutoMem
    {
        public:

            /// Default constructor
            CAutoMem() { m_p = 0; }

            /// Destructor
            ~CAutoMem() { release(); }

            /// Release allocated object
            void release() { if ( m_p ) { delete m_p; m_p = 0; } }

            /// Returns a pointer to encapsulated object
            T& Obj() { if ( !m_p ) m_p = new T; return *m_p; }

            /// Returns a pointer to the encapsulated object
            operator T&() { return Obj(); }

            /// Returns a pointer to the encapsulated object
            T* operator ->() { Obj(); return m_p; }
			
        private:

            /// Contains a pointer to the controlled object
            T       *m_p;
    };

    /// Unicode friendly string
    typedef std::basic_string< T > t_String;

    /// Our multi-dimensional string array type
    typedef std::map< t_String, CAutoMem< TPropertyBag< T > > > t_map;

public:

	/// Pass on iterator type
	typedef typename t_map::iterator iterator;

	/// First item
	iterator begin() { return m_lstSub.begin(); }

	/// Item beyond the last item, yeah, that's what it is
	iterator end() { return m_lstSub.end(); }
    
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

    /// Releases all memory resources and prepares class for reuse.
    void destroy() { m_lstSub.clear(); m_str.release(); }

    /// Releases all memory resources and prepares class for reuse.
    void clear() { destroy(); }

	/// Returns the number of items in the property bag
	long size() { return m_lstSub.size(); }
	
    //==============================================================
    // serialize()
    //==============================================================
    /// Serializes the array
    /**
        \return Serialized array.
    
        \see 
    */
/*	
    t_String serialize()
    {
        t_String sRes;
        
        // Just return our value if we're not an array
        if ( !IsArray() ) return m_str.Obj();

        // Iterator
        t_map::iterator pos = m_lstSub.begin();

        // For each array element
        while ( pos != m_lstSub.end() )
        {
            // Add separator if needed
            if ( sRes.length() ) sRes += _T( ',' );

            sRes += pos->first;

            // Is it an array?
            if ( pos->second.Obj().IsArray() )
            { 
                sRes += _T( '{' ); 
                sRes += pos->second.Obj().serialize(); 
                sRes += _T( '}' ); 
            }
                
            // Serialize the value
            else sRes += _T( '=' ), sRes += urlencode( (LPCTSTR)pos->second.Obj() );

            // Next array element
            pos++;

        } // end while

        return sRes;
    }
*/

    //==============================================================
    // deserialize()
    //==============================================================

    /// Deserializes an array from string
    /**
        \param [in] sStr    -   Serialized array string.
        \param [in] bMerge  -   Non-zero if array should be merged
                                into current data. Set to zero to
                                replace current array.
        \param [in] pLast   -   Receives the number of bytes decoded.
        \param [in] pPs     -   Property bag that receives any decoded
                                characters. We could also have just
                                called this function on the object,
                                but this way provides a little extra
                                flexibility for later.
        
        \return Number of items deserialized.
    
        \see 
    */
/*
    LONG deserialize(   t_String sStr, BOOL bMerge = FALSE, 
                        LONG *pLast = NULL, TPropertyBag *pPs = NULL )
    {
        // Ensure object
        if ( !pPs ) pPs = this;

        // Do we want to merge?

        if ( !bMerge ) pPs->destroy();

        LONG lItems = 0;
        long lLen = sStr.length(), s = 0, e = 0;

        while ( e < lLen )
        {
            switch( sStr[ e ] )
            {
                case ',' : case '}' :
                {
                    if ( 1 < e - s )
                    {
                       // Find '='

                       long a = s; while ( a < e && '=' != sStr[ a ] ) a++;

                       t_String sKey, sVal;

                       // First character is separator

                       if ( a == s ) 
                        sKey = urldecode( t_String( &sStr.c_str()[ s + 1 ], e - s - 1 ) );

                       else sKey = urldecode( t_String( &sStr.c_str()[ s ], a - s ) );
     
                       // Single token

                       if ( 1 >= e - a ) (*pPs)[ sKey ] = _T( "" );

                       // Both tokens present

                       else (*pPs)[ sKey ] = 
                        urldecode( t_String( &sStr.c_str()[ a + 1 ], e - a - 1 ) );

                       // Count one item

                       lItems++;

                    } // end if


                    // Next element

                    s = e + 1;

                    // Time to exit?

                    if ( '}' == sStr[ e ] )
                    {   if ( pLast ) *pLast = e + 1; return lItems; }

                } break;

                case '{' :
                {              
                    // Get key

                    t_String sKey = 
                        urldecode( t_String( &sStr.c_str()[ s ], e - s ) );

                    // Do we have a key?

                    if ( sKey.length() )
                    {
                        // This will point to the end of the array we're about to decode

                        LONG lEnd = 0;

                        // Get the sub array

                        lItems += deserialize(  t_String( &sStr.c_str()[ e + 1 ] ), 
                                                TRUE, &lEnd, &(*pPs)[ sKey ] );

                        // Skip the array we just decoded

                        e += lEnd;

                    } // end if


                    // Skip this token

                    s = e + 1; 

                } break;

            } // end switch


            // Next i

            e++;

        } // end while


        return lItems;
    }
*/

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
    {	return m_lstSub[ tcnv::ToString< T, t_String >( n ) ]; }

    //==============================================================
    // operator []()
    //==============================================================
    /// Indexes into sub array
    /**
        \param [in] n   -   Index key
        
        \return Reference to sub class.
    */
    TPropertyBag& operator []( unsigned int n ) 
    {	return m_lstSub[ tcnv::ToString< T, t_String >( n ) ]; }

    //==============================================================
    // operator []()
    //==============================================================
    /// Indexes into sub array
    /**
        \param [in] n   -   Index key
        
        \return Reference to sub class.
    */
    TPropertyBag& operator []( long n ) 
    {	return m_lstSub[ tcnv::ToString< T, t_String >( n ) ]; }

    //==============================================================
    // operator []()
    //==============================================================
    /// Indexes into sub array
    /**
        \param [in] n   -   Index key
    
        \return Reference to sub class.
    */
    TPropertyBag& operator []( unsigned long n ) 
    {	return m_lstSub[ tcnv::ToString< T, t_String >( n ) ]; }

    //==============================================================
    // operator []()
    //==============================================================
    /// Indexes into sub array
    /**
        \param [in] n   -   Index key
        
        \return Reference to sub class.
    */
    TPropertyBag& operator []( tcnv::tc_int64 n ) 
    {	return m_lstSub[ tcnv::ToString< T, t_String >( n ) ]; }

    //==============================================================
    // operator []()
    //==============================================================
    /// Indexes into sub array
    /**
        \param [in] n   -   Index key
    
        \return Reference to sub class.
    */
    TPropertyBag& operator []( tcnv::tc_uint64 n ) 
    {	return m_lstSub[ tcnv::ToString< T, t_String >( n ) ]; }

    //==============================================================
    // operator []()
    //==============================================================
    /// Indexes into sub array
    /**
        \param [in] n   -   Index key
        
        \return Reference to sub class.
    */
    TPropertyBag& operator []( double n ) 
    {	return m_lstSub[ tcnv::ToString< T, t_String >( n ) ]; }

    //==============================================================
    // operator = ()
    //==============================================================
    /// Conversion from string object
    t_String operator = ( t_String sStr ) 
    {   m_str.Obj() = sStr; return m_str.Obj(); }

    //==============================================================
    // operator = ()
    //==============================================================
    /// Conversion from string
    t_String operator = ( const T* pStr ) 
    {   if ( pStr ) m_str.Obj() = *pStr; return m_str.Obj(); }


    //==============================================================
    // operator = ()
    //==============================================================
    /// Conversion from int
    t_String operator = ( int n )
    {   return m_str.Obj() = tcnv::ToString< T, t_String >( n ); }

    //==============================================================
    // operator = ()
    //==============================================================
    /// Conversion from unsigned int
    t_String operator = ( unsigned int n )
    {   return m_str.Obj() = tcnv::ToString< T, t_String >( n ); }

    //==============================================================
    // operator = ()
    //==============================================================
    /// Conversion from long
    t_String operator = ( long n )
    {   return m_str.Obj() = tcnv::ToString< T, t_String >( n ); }

    //==============================================================
    // operator = ()
    //==============================================================
    /// Conversion from unsigned long
    t_String operator = ( unsigned long n )
    {   return m_str.Obj() = tcnv::ToString< T, t_String >( n ); }

    //==============================================================
    // operator = ()
    //==============================================================
    /// Conversion from long
    t_String operator = ( tcnv::tc_int64 n )
    {   return m_str.Obj() = tcnv::ToString< T, t_String >( n ); }

    //==============================================================
    // operator = ()
    //==============================================================
    /// Conversion from unsigned long
    t_String operator = ( tcnv::tc_uint64 n )
    {   return m_str.Obj() = tcnv::ToString< T, t_String >( n ); }

    //==============================================================
    // operator = ()
    //==============================================================
    /// Conversion from double
    t_String operator = ( float n )
    {   return m_str.Obj() = tcnv::ToString< T, t_String >( n ); }

    //==============================================================
    // operator = ()
    //==============================================================
    /// Conversion from double
    t_String operator = ( double n )
    {   return m_str.Obj() = tcnv::ToString< T, t_String >( n ); }

    //==============================================================
    // T*()
    //==============================================================
    /// Conversion to const T*
    operator const T*() { return c_str(); }

    //==============================================================
    // c_str()
    //==============================================================
    /// Returns local string object
    const T* c_str() { return m_str.Obj().c_str(); }

    //==============================================================
    // length()
    //==============================================================
    /// Returns local string object
    long length() { return m_str.Obj().length(); }

    //==============================================================
    // ToString()
    //==============================================================
    /// Returns local string object
    t_String ToString() { return m_str.Obj(); }

    //==============================================================
    // ToInt64()
    //==============================================================
    /// Converts to long
    tcnv::tc_int64 ToInt64() { return tcnv::StrToInt64( c_str() ); }

    //==============================================================
    // ToUInt64()
    //==============================================================
    /// Converts to long
    tcnv::tc_uint64 ToUInt64() { return tcnv::StrToUInt64( c_str() ); }

    //==============================================================
    // ToLong()
    //==============================================================
    /// Converts to long
    long ToLong() { return tcnv::StrToLong( c_str() ); }

    //==============================================================
    // ToULong()
    //==============================================================
    /// Converts to unsigned long
    unsigned long ToULong() { return tcnv::StrToULong( c_str() ); }

    //==============================================================
    // ToDouble()
    //==============================================================
    /// Converts to double
    double ToDouble() { return tcnv::StrToDouble( c_str() ); }

    //==============================================================
    // ToFloat()
    //==============================================================
    /// Converts to double
    double ToFloat() { return tcnv::StrToFloat( c_str() ); }

    //==============================================================
    // IsArray()
    //==============================================================
    /// Returns non-zero if array elements are present

    bool IsArray() { return 0 < m_lstSub.size(); }

private:

    /// Our value
    CAutoMem< t_String >			m_str;

    /// Array of string maps
    t_map							m_lstSub; 

};

/// Property bag types
/** \see TPropertyBag */
typedef TPropertyBag< char > CPropertyBag8;
#ifndef CII_NO_WCHAR
	typedef TPropertyBag< wchar_t > CPropertyBagW;
#endif
