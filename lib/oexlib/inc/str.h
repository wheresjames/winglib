/*------------------------------------------------------------------
// str.h
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

/*
      _______________________END
     |     |        |        |
     |__   |_____   |  ______|
     |        |        |     |
     |  ___   |_____   |     |
     |  |  |           |  |  |
     |  |  |  ___      |  |  |
     |  |        |  |  |  |  |
     |  |_____   |__|__|__|  |
     |  |                    |
Start|__|____________________|

*/

/// Generic string class
/**

  \code

  \endcode

*/
template < typename T > class TStr
{
public:

    /// Default constructor
    TStr()
    {
        m_nLength = 0;
        m_uOffset = 0;

#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )
        m_pFile = oexNULL;
        m_uLine = 0;
#endif
    }

	TStr( oexCONST TStr &str )
	{	m_nLength = 0;
        m_uOffset = 0;
#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )
        m_pFile = oexNULL;
        m_uLine = 0;
#endif
		Set( str );
//		if ( str.Length() ) Set( str.Ptr() );
	}

	TStr( oexCONST T *pStr )
	{	m_nLength = 0;
        m_uOffset = 0;
#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )
        m_pFile = oexNULL;
        m_uLine = 0;
#endif
		Set( pStr );
	}

	TStr( oexCONST T *pStr, oexUINT uSize )
	{	m_nLength = 0;
        m_uOffset = 0;
#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )
        m_pFile = oexNULL;
        m_uLine = 0;
#endif
		Set( pStr, uSize );
	}

	TStr( oexCONST T *pStr, oexINT nStart, oexUINT uLen )
	{	m_nLength = 0;
        m_uOffset = 0;
#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )
        m_pFile = oexNULL;
        m_uLine = 0;
#endif
		Sub( pStr, nStart, uLen );
	}

	TStr( oexCONST T *pStr, oexUINT uSize, oexINT nStart, oexUINT uLen )
	{	m_nLength = 0;
        m_uOffset = 0;
#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )
        m_pFile = oexNULL;
        m_uLine = 0;
#endif
		Sub( pStr, uSize, nStart, uLen );
	}

	TStr( oexCONST oexINT nVal )
	{	m_nLength = 0;
        m_uOffset = 0;
#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )
        m_pFile = oexNULL;
        m_uLine = 0;
#endif
		SetNum( oexTT( T, "%i" ), (oexINT)nVal );
	}

	TStr( oexCONST oexUINT uVal )
	{	m_nLength = 0;
        m_uOffset = 0;
#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )
        m_pFile = oexNULL;
        m_uLine = 0;
#endif
		SetNum( oexTT( T, "%u" ), (oexUINT)uVal );
	}

	TStr( oexCONST oexDOUBLE dStr )
	{	m_nLength = 0;
        m_uOffset = 0;
#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )
        m_pFile = oexNULL;
        m_uLine = 0;
#endif
		SetNumTrim( oexTT( T, "%f" ), oexNULL, oexTT( T, "0" ), (oexDOUBLE)dStr );
	}

	TStr( oexCONST T tVal )
	{	m_nLength = 0;
        m_uOffset = 0;
#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )
        m_pFile = oexNULL;
        m_uLine = 0;
#endif
		Set( tVal );
	}

	TStr( oexCONST oexGUID &guid )
	{	m_nLength = 0;
        m_uOffset = 0;
#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )
        m_pFile = oexNULL;
        m_uLine = 0;
#endif
		GuidToString( &guid );
	}

    /// Destructor
    virtual ~TStr()
    {   Destroy();
    }

    /// Releases the string
    void Destroy()
    {
        // Zero length
        m_nLength = 0;

        // Reset the offset
        m_uOffset = 0;

        // Lose the string memory
        m_mem.OexDelete();
    }

public:

    /// Shares memory of another object
    TStr& Share( oexCONST TStr &x_str )
    {
        // Share the memory
        m_nLength = x_str.m_nLength;
        m_uOffset = x_str.m_uOffset;
        m_mem.Share( x_str.m_mem );
        return *this;
    }

    /// Creates our own copy of a string if it's shared
	oexBOOL Unshare()
	{
		// Create our own buffer if shared
		if ( 1 == m_mem.GetRefCount() )
			return oexTRUE;

		oexUINT uOldSize = m_mem.Size();

		if ( uOldSize )
		{
            TMem< T > mem;
            if ( !oexVERIFY_PTR( mem.OexNew( uOldSize ).Ptr() ) )
                return oexFALSE;

            // Copy string data
			os::CSys::MemCpy( mem.Ptr(), m_mem.c_Ptr(), uOldSize * sizeof( T ) );

            // Use the new memory
			m_mem.Assume( mem );

		} // end if

		else
            Destroy();

		return oexTRUE;

	}

	/// Allocates at least the number of bytes specified
	T* Allocate( oexUINT x_uSize )
	{
//		oexTRACE( "TStr::Allocate( %u )\n", (oexUINT)uSize );

		// Break any existing share
		Unshare();

        T* pMem;
        oexBOOL bExisting = oexNULL != m_mem.Ptr();

        // Do we already have the memory
        if ( m_mem.Size() == ( x_uSize + 1 ) )
            pMem = m_mem.Ptr();

        else
        {
#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )
            pMem = m_mem._Log( m_uLine, m_pFile ).Resize( x_uSize + 1 ).Ptr();
#else
            pMem = m_mem.Resize( x_uSize + 1 ).Ptr();
#endif
            if ( !oexVERIFY_PTR( pMem ) )
			    return oexNULL;

        } // end if

        // Set empty string if new buffer
        if ( !bExisting )
            pMem[ 0 ] = 0;

		// Null terminate our new buffer
		pMem[ x_uSize ] = 0;

		// User can't possibly copy any data
		if ( !x_uSize )
            m_nLength = 0;

		// We'll scan for the length of the buffer later
		else
            m_nLength = -1;

		return pMem;
    }

	/// Returns a const pointer to the internal string buffer
	oexCONST T* Ptr() const
	{
		// Ensure string
		if ( !m_mem.Size() )
			( (TStr*)this )->OexAllocate( 0 );

		return ( (TStr*)this )->m_mem.Ptr( m_uOffset );
	}

	/// Returns a const pointer to the internal string buffer
    /**
        \param [in] x_lOffset   -   Offset into the string
    */
	oexCONST T* Ptr( oexUINT x_uOffset ) const
    {
        oexUINT uSize = ( (TStr*)this )->m_mem.Size();

		// Ensure string
		if ( !uSize )
			( (TStr*)this )->OexAllocate( 0 );

        if ( x_uOffset )
        {
		    // Check for offset beyond the end of the buffer
		    oexASSERT( uSize >= x_uOffset );

            if ( x_uOffset >= uSize )
                x_uOffset = uSize - 1;

        } // end if

		return ( (TStr*)this )->m_mem.Ptr( x_uOffset + m_uOffset );
	}

	/// Returns a writable pointer (use with care)
	T* _Ptr() const
	{	( (TStr*)this )->Unshare(); return (T*)Ptr(); }

	/// Returns a writable pointer (use with care)
	T* _Ptr( oexUINT x_uOffset ) const
	{	( (TStr*)this )->Unshare(); return (T*)Ptr( x_uOffset ); }

	/// Returns the specified character
	T& operator []( oexUINT x_uOffset ) const
	{	( (TStr*)this )->Unshare(); return *_Ptr( x_uOffset ); }

	/// Returns the first character in the buffer
	oexCONST T& operator *() const
    {   return *Ptr(); }

	/// TMem object
	operator TMem< T >&() const
	{	return m_mem; }

	/// Calculates the length of the string by finding the NULL terminator
	oexINT CalculateLength()
	{
		m_nLength = 0;
		oexINT lMax = m_mem.Size();

		// Do we have a buffer?
		if ( 0 >= lMax )
			return m_nLength;

		// Get a pointer to the whole string buffer
		T* pStr = m_mem.Ptr();

		// NULL terminate the end of memory so we don't over-run the buffer
		pStr[ lMax - 1 ] = 0;

        // Get the length
        m_nLength = str::Length( pStr, lMax );

		// Resize the memory block ( this adds over-run protection )
		m_mem.Resize( m_nLength + 1 );

		// Return the length of the buffer
		return m_nLength;
	}

	/// Returns the length of the string, calculates if needed.
	oexINT Length() oexCONST
	{	if ( 0 > m_nLength )
			return ( (TStr*)this )->CalculateLength();
		return m_nLength - m_uOffset;
	}

    /// Manually sets the length
    /// !!! This allows NULL characters to be in the string.
    oexINT SetLength( oexINT x_nLength )
    {
        // Do we already have the correct length?
        if ( x_nLength == m_nLength )
            return m_nLength;

        // Ensure we have that much data
        if ( !OexAllocate( m_uOffset + x_nLength ) )
            return 0;

        // Just accept the callers size
        m_nLength = x_nLength;

        return m_nLength;
    }

    /// Returns the size in bytes, of a single character
    oexINT SizeOfChar() oexCONST
    {   return sizeof( T ); }

    /// Returns the length of the string in bytes
    /**
        \param x_uAdd   -   Number of *characters* to
                            add to the returned length
    */
    oexINT LengthInBytes( oexUINT x_uAdd = 0 ) oexCONST
    {
        return ( Length() + x_uAdd ) * SizeOfChar();
    }

public:

	/// Assignment operator
	TStr& operator = ( oexCONST T* pStr )
	{	return Set( pStr ); }

	/// Assignment operator
	TStr& operator = ( oexCONST TStr &x_str )
	{	return Set( x_str ); }

    /// Assignment operator
	TStr& operator = ( oexCONST T chVal )
	{	return Set( chVal ); }

    /// Assignment operator
	TStr& operator = ( oexCONST oexGUID &guid )
	{	return StringToGuid( &guid ); }

	/// Concatenation operator
	TStr& operator += ( oexCONST T* pStr )
	{	return Append( pStr ); }

	/// Concatenation operator
	TStr& operator += ( oexCONST TStr &str )
	{	return Append( str ); }

	TStr& operator = ( oexCONST oexINT nVal )
	{	return SetNum( oexTT( T, "%i" ), (oexINT)nVal ); }

	TStr& operator = ( oexCONST oexUINT uVal )
	{	return SetNum( oexTT( T, "%u" ), (oexUINT)uVal ); }

	TStr& operator = ( oexCONST oexDOUBLE dStr )
	{	return SetNumTrim( oexTT( T, "%f" ), oexNULL, oexTT( T, "0" ), (oexDOUBLE)dStr ); }

	TStr& operator += ( oexCONST oexINT nVal )
	{	return AppendNum( oexTT( T, "%i" ), (oexINT)nVal ); }

	TStr& operator += ( oexCONST oexUINT uVal )
	{	return AppendNum( oexTT( T, "%u" ), (oexUINT)uVal ); }

	TStr& operator += ( oexCONST oexDOUBLE dVal )
	{	return AppendNumTrim( oexTT( T, "%f" ), oexNULL, oexTT( T, "0" ), (oexDOUBLE)dVal ); }

	TStr& operator += ( oexCONST T chVal )
	{	return Append( &chVal, 1 ); }

	TStr& operator += ( oexCONST oexGUID &guid )
	{	return Append( TStr().GuidToString( guid ) ); }

	TStr& operator << ( oexCONST oexINT nVal )
	{	return AppendNum( oexTT( T, "%i" ), (oexINT)nVal ); }

	TStr& operator << ( oexCONST oexUINT uVal )
	{	return AppendNum( oexTT( T, "%u" ), (oexUINT)uVal ); }

	TStr& operator << ( oexCONST oexDOUBLE dVal )
	{	return AppendNumTrim( oexTT( T, "%f" ), oexNULL, oexTT( T, "0" ), (oexDOUBLE)dVal ); }

	TStr& operator << ( oexCONST T *pStr )
	{	return Append( pStr ); }

	TStr& operator << ( oexCONST TStr &str )
	{	return Append( str ); }

	TStr& operator << ( oexCONST T chVal )
	{	return Append( &chVal, 1 ); }

    /// Assignment operator
	TStr& operator << ( oexCONST oexGUID &guid )
	{	return Append( TStr().GuidToString( guid ) ); }

	TStr& Chr( oexUINT uCh )
	{	return Set( (T)uCh ); }

	TStr& ChrAppend( oexUINT uCh )
	{	return Append( &( (T)uCh ), 1 ); }


    /// Compare to const string
    oexBOOL operator == ( oexCONST T *x_pStr )
    {   return !Compare( x_pStr, zstr::Length( x_pStr ) ); }

    /// Compare to other object
    oexBOOL operator == ( oexCONST TStr &x_str )
    {
        TStr &rStr = (TStr&)x_str;

        // Lengths must match
        if ( Length() != rStr.Length() )
            return oexFALSE;

        // See if we're sharing the same memory
        if ( rStr.m_mem.c_Ptr() == m_mem.c_Ptr()
             && rStr.m_uOffset == m_uOffset )
            return oexTRUE;

        // I suppose we'll have to actually compare the strings
        return !Compare( rStr.m_mem.c_Ptr(), rStr.Length() );
    }

    /// Compare to other object
    oexBOOL operator != ( oexCONST TStr &x_str )
    {   return !( *this == x_str ); }


    oexBOOL operator == ( oexCONST oexINT x_nVal )
    {   return ToInt() == x_nVal; }

    oexBOOL operator == ( oexCONST oexUINT x_uVal )
    {   return ToUInt() == x_uVal; }

    oexBOOL operator == ( oexCONST oexLONG x_lVal )
    {   return ToLong() == x_lVal; }

    oexBOOL operator == ( oexCONST oexULONG x_ulVal )
    {   return ToULong() == x_ulVal; }

    oexBOOL operator == ( oexCONST oexFLOAT x_fVal )
    {   return ToFloat() == x_fVal; }

    oexBOOL operator == ( oexCONST oexDOUBLE x_dVal )
    {   return ToDouble() == x_dVal; }

    oexBOOL operator == ( oexCONST oexGUID &x_guid )
    {   oexGUID guid; return guid::CmpGuid( StringToGuid( &guid ), &x_guid ); }

    oexBOOL operator != ( oexCONST oexINT x_nVal )
    {   return ToInt() != x_nVal; }

    oexBOOL operator != ( oexCONST oexUINT x_uVal )
    {   return ToUInt() != x_uVal; }

    oexBOOL operator != ( oexCONST oexLONG x_lVal )
    {   return ToLong() != x_lVal; }

    oexBOOL operator != ( oexCONST oexULONG x_ulVal )
    {   return ToULong() != x_ulVal; }

    oexBOOL operator != ( oexCONST oexFLOAT x_fVal )
    {   return ToFloat() != x_fVal; }

    oexBOOL operator != ( oexCONST oexDOUBLE x_dVal )
    {   return ToDouble() != x_dVal; }

    oexBOOL operator != ( oexCONST oexGUID &x_guid )
    {   oexGUID guid; return !guid::CmpGuid( StringToGuid( &guid ), &x_guid ); }

    oexBOOL operator !()
    {   oexBOOL b = ( !Length()
                      || ( *Ptr() >= oexTC( T, '0' )
                           && *Ptr() <= oexTC( T, '9' )
                           && !ToDouble()
                         )
                    );
        return b;
    }

public:

    /// Compare to const string
    oexINT Compare( oexCONST T *x_pPtr, oexUINT x_uLen )
    {   return str::Compare( Ptr(), Length(), x_pPtr, x_uLen ); }

    /// Extracts a sub string
	TStr& Sub( oexUINT x_uStart, oexUINT x_uLen = 0 )
	{
        // Current string length
		oexUINT uSize = Length();

		// Can't shorten the string if we don't own the memory
		if ( x_uLen && x_uLen < ( uSize - x_uStart ) && 1 != m_mem.GetRefCount() )
			return Set( SubStr( x_uStart, x_uLen ) );

        // Wiping out the string?
        if ( x_uStart >= uSize )
        {   Destroy(); return *this; }

        // Shift the start of the string
        m_uOffset += x_uStart;

        // Do we need to adjust the length?
        if ( x_uLen && x_uLen < ( uSize - x_uStart ) )
        {   OexAllocate( m_uOffset + x_uLen );
            m_nLength = m_uOffset + x_uLen;
        } // end if

		return *this;

	}

	TStr& Sub( TStr &str, oexINT nStart, oexUINT uLen )
	{	return Sub( str.Ptr(), str.Length(), nStart, uLen ); }

	TStr& Sub( oexCONST T* pStr, oexINT nStart, oexUINT uLen )
	{	return Sub( pStr, zstr::Length( pStr ), nStart, uLen ); }

	TStr& Sub( oexCONST T* pStr, oexUINT uSize, oexINT nStart, oexUINT uLen )
	{
		oexUINT uStart;
		if ( 0 <= nStart ) uStart = (oexUINT)nStart;
		else nStart = (oexUINT)( (oexINT)uSize - nStart );

		// Sanity checks
		if ( !oexVERIFY_PTR( pStr ) || !uSize || uStart >= uSize )
		{	OexAllocate( 0 ); return *this; }

		// Was a length given?
		if ( !uLen )
			uLen = uSize - uStart;

        // Allocate memory for sub string
		if ( !oexVERIFY( OexAllocate( uLen ) ) )
		{	OexAllocate( 0 ); return *this; }

		// Copy the sub string
		m_mem.MemCpy( &pStr[ uStart ], uLen );

		// Save the new string length
		m_nLength = uLen;

		return *this;
	}

	/// Returns a sub string
	TStr SubStr( oexINT nStart, oexUINT uLen ) oexCONST
	{	return TStr( Ptr(), Length(), nStart, uLen );
    }

    /// Reverses the string
    TStr& Reverse()
    {   str::Reverse( _Ptr(), Length() );
        return *this;
    }

public:

	/// Moves part of the string from one place to another
	/// May destroy the string if there is overlap
	TStr& Move( oexUINT uFrom, oexUINT uTo, oexUINT uSize )
	{	Move( _Ptr(), Length(), uFrom, uTo, uSize ); return *this; }
	oexINT Move( T* pPtr, oexUINT uLen, oexUINT uFrom, oexUINT uTo, oexUINT uSize )
	{
		// Sanity check
		if ( uFrom >= uLen || uTo >= uLen )
			return 0;

		// Ensure no buffer over-running
		if ( ( uFrom + uSize ) > uLen )
			uSize = uLen - uFrom;
		if ( ( uTo + uSize ) > uLen )
			uSize = uLen - uTo;

		// Get buffer pointers
		T *pFrom = &pPtr[ uFrom ];
		T* pTo = &pPtr[ uTo ];

		// Copy the data
		for ( oexUINT i = 0; i < uSize; i++ )
			pTo[ i ] = pFrom[ i ];

		return uSize;
	}

	/// Removes characters in the specified range
	TStr& DropRange( T tMin, T tMax, oexBOOL bIn )
	{
		T* pPtr = _Ptr();
		oexINT lLen = Length();
		oexINT g = -1, b = -1, i = 0, l = 0;

		if ( bIn )
		{	for( i = 0; i < lLen; i++ )
			{	if ( pPtr[ i ] < tMin || pPtr[ i ] > tMax )
				{	if ( 0 > g ) g = i; l++; }
				else
				{	if ( 0 > b ) b = i, g = -1;
					else if ( 0 <= g ) Move( pPtr, lLen, g, b, i - g ), b = l, g = -1;
				} // end else
			} // end for
		} // end if

		else
		{	for( i = 0; i < lLen; i++ )
			{	if ( pPtr[ i ] >= tMin && pPtr[ i ] <= tMax )
				{	if ( 0 > g ) g = i; l++; }
				else
				{	if ( 0 > b ) b = i, g = -1;
					else if ( 0 <= g ) Move( pPtr, lLen, g, b, i - g ), b = l, g = -1;
				} // end else
			} // end for
		} // end if

		// Anything change?
		if ( 0 > b )
			return *this;

		// Move remainder if needed
		if ( 0 <= g )
			Move( pPtr, lLen, g, b, i - g ), b = i, g = -1;

		// Change the length of the string
		OexAllocate( l );
		m_nLength = l;

		return *this;
	}

	/// Removes characters in the specified array
	TStr& Drop( oexCONST T *pChars, oexBOOL bIn )
	{
		T* pPtr = _Ptr();
		oexINT lLen = Length();
		oexINT g = -1, b = -1, i = 0, l = 0;
        oexINT lChars = zstr::Length( pChars );

		if ( bIn )
		{	for( i = 0; i < lLen; i++ )
			{	if ( 0 > str::FindCharacter( pChars, lChars, pPtr[ i ] ) )
				{	if ( 0 > g ) g = i; l++; }
				else
				{	if ( 0 > b ) b = i, g = -1;
					else if ( 0 <= g ) Move( pPtr, lLen, g, b, i - g ), b = l, g = -1;
				} // end else
			} // end for
		} // end if

		else
		{	for( i = 0; i < lLen; i++ )
			{	if ( 0 <= str::FindCharacter( pChars, lChars, pPtr[ i ] ) )
				{	if ( 0 > g ) g = i; l++; }
				else
				{	if ( 0 > b ) b = i, g = -1;
					else if ( 0 <= g ) Move( pPtr, lLen, g, b, i - g ), b = l, g = -1;
				} // end else
			} // end for
		} // end if


		// Move remainder if needed
		if ( 0 <= g )
			Move( pPtr, lLen, g, b, i - g ), b = i, g = -1;

		// Change the length of the string
		OexAllocate( l );
		m_nLength = l;

		return *this;
	}


public:

	TStr& Set( oexCONST TStr &x_str )
	{
        // To share or not to share?
        if ( *this != x_str )
            Share( x_str );

		return *this;
	}

	TStr& Set( oexCONST T* x_pStr )
	{
        if ( !x_pStr || !*x_pStr )
            Destroy();
        else
            Set( x_pStr, zstr::Length( x_pStr ) );
        return *this;
    }

	TStr& Set( oexCONST T* x_pStr, oexUINT uSize )
	{
		// Verify pointer
        if ( !x_pStr || !oexVERIFY_PTR( x_pStr ) || !uSize )
        {   Destroy();
			return *this;
        } // end if

		// Verify pointer
		if ( !oexVERIFY_PTR( x_pStr ) )
		{	Destroy();
			return *this;
		} // end if

        // Ditch the offset
        m_uOffset = 0;

		// Allocate space for new string
		if ( !oexVERIFY_PTR( OexAllocate( uSize ) ) || !uSize )
			return *this;

		// Copy the string data
		m_mem.MemCpy( x_pStr, uSize );

		// Save the new string length
		m_nLength = uSize;

		return *this;
	}

	TStr& Set( oexCONST T x_chVal )
	{
		if ( !x_chVal )
		{	OexAllocate( 0 ); return *this; }

        // Ditch the offset
        m_uOffset = 0;

		// Allocate space for new string
		T* pPtr = OexAllocate( 1 );
		if ( !oexVERIFY( pPtr ) )
			return *this;

		pPtr[ 0 ] = x_chVal;

		m_nLength = 1;

		return *this;
	}

    template< typename T_CHAR >
        TStr& Cnv( oexCONST T_CHAR *x_pStr, oexUINT x_uLen = 0 )
    {
		if ( !x_pStr || !x_pStr )
		{	OexAllocate( 0 );
            return *this;
        } // end if

        if ( !x_uLen )
            x_uLen = zstr::Length( x_pStr );

        if ( !x_uLen )
		{	OexAllocate( 0 );
            return *this;
        } // end if

        // Allocate buffer
        if ( !OexAllocate( x_uLen ) )
            return *this;

        // Copy bytes one by one
        for ( oexUINT i = 0; i < x_uLen; i++ )
            *_Ptr( i ) = (T)x_pStr[ i ];

        // Save the length
        m_nLength = x_uLen;

        return *this;
    }

    template< typename T_CHAR>
        TStr& Cnv( oexCONST TStr< T_CHAR > &x_sStr )
    {
        oexUINT uLen = x_sStr.Length();

		if ( !uLen )
		{	OexAllocate( 0 );
            return *this;
        } // end if

        // Allocate buffer
        if ( !OexAllocate( uLen ) )
            return *this;

        // Copy bytes one by one
        for ( oexUINT i = 0; i < uLen; i++ )
            *_Ptr( i ) = (T)( *x_sStr.Ptr( i ) );

        // Save the length
        m_nLength = uLen;

        return *this;
    }

    template< typename T_STR >
        TStr& Bin( oexCONST T_STR *x_pStr, oexUINT x_uLen = 0 )
    {
		if ( !x_pStr || !x_pStr )
		{	OexAllocate( 0 );
            return *this;
        } // end if

        if ( !x_uLen )
            x_uLen = zstr::Length( x_pStr );

        if ( !x_uLen )
		{	OexAllocate( 0 );
            return *this;
        } // end if

        // Get length of in comming string in bytes
        oexUINT uBytes = x_uLen * sizeof( T_STR );

        // Calculate whole bytes and remainder
        oexUINT uT = uBytes / sizeof( T );
        oexUINT uR = uBytes - ( uT * sizeof( T ) );

        // Allocate enough space
        if ( !OexAllocate( uT + uR ) )
            return *this;

        // Binary copy
        os::CSys::MemCpy( _Ptr(), x_pStr, uBytes );

        // Save length
        m_nLength = uT + uR;

        return *this;
    }

    template< typename T_CHAR>
        TStr& Bin( oexCONST TStr< T_CHAR > &x_sStr )
    {
        oexUINT uLen = x_sStr.Length();

		if ( !uLen )
		{	OexAllocate( 0 );
            return *this;
        } // end if

        // Get length of in comming string in bytes
        oexUINT uBytes = uLen * sizeof( T_CHAR );

        // Calculate whole bytes and remainder
        oexUINT uT = uBytes / sizeof( T );
        oexUINT uR = uBytes - ( uT * sizeof( T ) );

        // Allocate enough space
        if ( !OexAllocate( uT + uR ) )
            return *this;

        // Binary copy
        os::CSys::MemCpy( _Ptr(), x_sStr.Ptr(), uBytes );

        // Save length
        m_nLength = uT + uR;

        return *this;
    }
/*
	// Converts the string to wide character
    template< typename T_CHAR>
        TStr& ToWc( oexCONST TStr< T_CHAR > &x_sStr )
    {
    	OexAllocate( 0 );
    	return *this;
    }

	template<>
		TStr& ToWc< oexCHARW >( oexCONST TStr< oexCHARW > &x_sStr )
	{
		*this = x_sStr;
		return *this;
	}

	template<>
		TStr& ToWc< oexCHAR8 >( oexCONST TStr< oexCHAR8 > &x_sStr )
*/
	TStr& ToWc( oexCONST TStr< oexCHAR8 > &x_sStr )
	{
		if ( !oexCHECK( sizeof( T ) == sizeof( oexCHARW ) ) )
			return *this;

        oexUINT uLen = x_sStr.Length();

		if ( !uLen )
		{	OexAllocate( 0 );
            return *this;
        } // end if

        // Get length of in comming string in bytes
        oexUINT uConv = os::CSys::MbsToWcs( oexNULL, 0, x_sStr.Ptr(), uLen );

        // Allocate enough space
        if ( !OexAllocate( uConv ) )
            return *this;

		// Do the conversion
		uConv = os::CSys::MbsToWcs( _Ptr(), uConv, x_sStr.Ptr(), uLen );

		// Length may not be the same
		if ( !oexCHECK( 0 <= uConv ) )
		{
			Destroy();
			return *this;
		}

        // Save length
        m_nLength = uConv;

        return *this;
    }

	TStr& ToMb( oexCONST TStr< oexCHARW > &x_sStr )
	{
		if ( !oexCHECK( sizeof( T ) == sizeof( oexCHAR8 ) ) )
			return *this;

        oexUINT uLen = x_sStr.Length();

		if ( !uLen )
		{	OexAllocate( 0 );
            return *this;
        } // end if

        // Get length of in comming string in bytes
        oexINT uConv = os::CSys::WcsToMbs( oexNULL, 0, x_sStr.Ptr(), uLen );

        // Allocate enough space
        if ( !OexAllocate( uConv ) )
            return *this;

		// Do the conversion
		uConv = os::CSys::WcsToMbs( _Ptr(), uConv, x_sStr.Ptr(), uLen );

		// Actually, it may not be the same length
		if ( !oexCHECK( 0 <= uConv ) )
		{
			Destroy();
			return *this;
		}


        // Save length
        m_nLength = uConv;

        return *this;
    }


	// Concatenation operator
	TStr& Append( oexCONST TStr &sStr )
	{	return Append( ( (TStr&)sStr ).Ptr(), ( (TStr&)sStr ).Length() ); }

	// Concatenation operator
	TStr& Append( oexCONST T* pStr )
	{	return Append( pStr, zstr::Length( pStr ) ); }

	// Concatenation operator
	TStr& Append( oexCONST T* pStr, oexUINT uSize )
	{
		// Allocate space for new string
		oexUINT uOldSize = Length();
		if ( !oexVERIFY( OexAllocate( uOldSize + uSize ) ) )
			return *this;

		// Append string
		os::CSys::MemCpy( m_mem.Ptr( uOldSize ), pStr, uSize * sizeof( T ) );

		// Add new string length
		m_nLength = uOldSize + uSize;

		return *this;
	}

public:

	/// Trims uChars from the left side of the buffer
	/*
		This is efficient, it does not re-allocate or copy.
	*/
	TStr& RTrim( oexINT nChars )
	{
		if ( !nChars )
			return *this;

		oexINT lLen = Length();
        if ( lLen <= nChars )
            Destroy();
        else
            Sub( 0, lLen - nChars );

		return *this;
	}

	/// Drops the specified characters from the end of the string
	TStr& RTrim( oexCONST T* pChars )
	{	oexASSERT_PTR( pChars ); oexINT lLen = Length();
        oexINT i = str::RSkipCharacters( Ptr(), lLen, pChars, zstr::Length( pChars ) );
		if ( !i || i >= lLen ) return *this;
		RTrim( lLen - i - 1 );
		return *this;
	}

	/// Trims uChars from the left side of the buffer
	/*
		This is efficient, it moves the string pointer
		without re-allocating or copying.
	*/
	TStr& LTrim( oexINT nChars )
	{
		if ( !nChars )
			return *this;

		oexINT lLen = Length();
        if ( lLen <= nChars )
            Destroy();
        else
            Sub( nChars );

		return *this;
	}

	/// Drops the specified characters from the start of the string
	TStr& LTrim( oexCONST T* pChars, oexCONST T* pEscape = oexNULL )
	{	oexASSERT_PTR( pChars ); oexASSERT_PTR_NULL_OK( pEscape );
        oexINT i = str::SkipCharacters( Ptr(), Length(), pChars, zstr::Length( pChars ) );
		if ( 0 > i ) return *this;
		LTrim( i );
		return *this;
	}

	TStr& operator >>= ( oexUINT uChars )
	{	return LTrim( uChars ); }

	TStr& operator <<= ( oexUINT uChars )
	{	return RTrim( uChars ); }

	TStr& operator ++( int ) { return LTrim( 1 ); }

	TStr& operator --( int ) { return RTrim( 1 ); }

public:

    /// Converts letters from upper case to lower case
    TStr& ToLower()
    {   if ( Length() ) str::ToLower( _Ptr(), Length() ); return *this; }

    /// Converts letters from upper case to lower case
    TStr& ToUpper()
    {   if ( Length() ) str::ToUpper( _Ptr(), Length() ); return *this; }

public:

	TStr& vFmt( oexCONST T *x_pFmt, oexVaList x_pArgs )
	{
		// Verify input string
		if ( !oexVERIFY( x_pFmt ) )
		{	OexAllocate( 0 ); return *this; }

		oexRESULT res;
		oexUINT uSize = oexSTRSIZE;
		do
		{
			// Allocate buffer
			// The - 1 is a performance boost, this keeps us under the TMemory block size.
			T* pPtr = OexAllocate( uSize - 1 );
			if ( !oexVERIFY_PTR( pPtr ) )
				return *this;

			// Attempt to format the string
			os::CSys::vStrFmt( &res, pPtr, uSize - 1, x_pFmt, x_pArgs );

			// Shift up size
			uSize <<= 1;

		} while ( uSize && os::CSys::c_StrErr_INSUFFICIENT_BUFFER == res );

		// Verfiy that the string was copied correctly
		oexASSERT( os::CSys::c_StrErr_OK == res );

		// Set the length of the string
		Length();

		return *this;
	}

	/// Formats a string
	TStr& Fmt( oexCONST T *pFmt, ... )
	{
		oexVaList ap;
		oexVaStart( ap, pFmt );
		vFmt( pFmt, ap );
		oexVaEnd( ap );
		return *this;
	}

	/// Sets a number into the string using rules of Fmt()
	/// Truncates results larger than 256 characters
	TStr& SetNum( oexCONST T *pFmt, ... )
	{
		T tBuf[ 256 ];

		oexVaList ap; oexVaStart( ap, pFmt );
		os::CSys::vStrFmt( tBuf, oexSizeofArray( tBuf ), pFmt, ap );
		oexVaEnd( ap );

		return Cnv( tBuf );

	}

	/// Sets a number into the string using rules of Fmt()
	/// Truncates results larger than 256 characters
    /// Optional pre and post trimming
	TStr& SetNumTrim( oexCONST T *pFmt, oexCONST T* pLTrim, oexCONST T* pRTrim, ... )
	{
		oexVaList ap; oexVaStart( ap, pRTrim );
	    os::CSys::vStrFmt( OexAllocate( 256 ), 256, pFmt, ap );
		oexVaEnd( ap );

        if ( pLTrim ) LTrim( pLTrim );
        if ( pRTrim ) RTrim( pRTrim );
        return *this;
	}

	/// Appends a formatted number to the string using rules of Fmt()
	/// Truncates results larger than 256 characters
	TStr& AppendNum( oexCONST T *pFmt, ... )
	{	T tBuf[ 256 ];
		oexVaList ap; oexVaStart( ap, pFmt );
		os::CSys::vStrFmt( tBuf, oexSizeofArray( tBuf ), pFmt, ap );
		oexVaEnd( ap );
		return Append( tBuf );
	}

    /// Sets a number into the string using rules of Fmt()
	/// Truncates results larger than 256 characters
    /// Optional pre and post trimming
	TStr& AppendNumTrim( oexCONST T *pFmt, oexCONST T* pLTrim, oexCONST T* pRTrim, ... )
	{   TStr str;
		oexVaList ap; oexVaStart( ap, pRTrim );
   		os::CSys::vStrFmt( str.OexAllocate( 256 ), 256, pFmt, ap );
   		oexVaEnd( ap );
        if ( pLTrim ) str.LTrim( pLTrim );
        if ( pRTrim ) str.RTrim( pRTrim );
        return Append( str );
	}

	/// Converts the string to a GUID, returns oexNULL if this string does not contain a GUID
	oexGUID * StringToGuid( oexGUID *x_pGuid )
    {	return guid::StringToGuid( x_pGuid, Ptr(), Length() ); }

	/// Converts a GUID to a string, if pGuid is NULL, a unique GUID is created
	TStr& GuidToString( oexCONST oexGUID *x_pGuid = oexNULL )
	{	guid::GuidToString( OexAllocate( 36 /* + 1 is implicit*/ ), 37, x_pGuid );
		oexVERIFY( Length() == 36 );
		return *this;
	}

public:

    /// Converts to a number
    oexINT64 ToNum( oexINT x_nMax = 0, oexUINT x_uRadix = 10, oexINT *x_pnEnd = oexNULL, oexBOOL x_bTrim = oexFALSE )
    {   if ( !x_nMax || x_nMax > Length() ) x_nMax = Length();
        oexINT nEnd = 0; oexINT64 llNum = str::StrToNum( Ptr(), x_nMax, x_uRadix, &nEnd );
        if ( x_bTrim ) LTrim( nEnd );
        if ( x_pnEnd ) *x_pnEnd = nEnd;
        return llNum;
    }

	/// Converts to long
	oexINT ToInt64( oexUINT uRadix = 10 ) { return os::CSys::StrToInt64( Ptr(), uRadix ); }

	/// Converts to unsigned long
	oexUINT ToUInt64( oexUINT uRadix = 10 ) { return os::CSys::StrToUInt64( Ptr(), uRadix ); }

	/// Converts to long
	oexINT ToInt( oexUINT uRadix = 10 ) { return (oexINT)os::CSys::StrToInt64( Ptr(), uRadix ); }

	/// Converts to unsigned long
	oexUINT ToUInt( oexUINT uRadix = 10 ) { return (oexUINT)os::CSys::StrToUInt64( Ptr(), uRadix ); }

    /// Converts to long
	oexLONG ToLong( oexUINT uRadix = 10 ) { return os::CSys::StrToLong( Ptr(), uRadix ); }

	/// Converts to unsigned long
	oexULONG ToULong( oexUINT uRadix = 10 ) { return os::CSys::StrToULong( Ptr(), uRadix ); }

	/// Converts to double
	oexFLOAT ToFloat() { return (oexFLOAT)os::CSys::StrToDouble( Ptr( 0 ) ); }

    /// Converts to double
	oexDOUBLE ToDouble() { return os::CSys::StrToDouble( Ptr( 0 ) ); }

    /// Converts to a string,
	oexCONST T* ToString() { return Ptr(); }

	oexBOOL Cmp( TStr &str )
	{	return Cmp( str.Ptr(), str.Length() ); }

	oexBOOL Cmp( oexCONST T* pStr )
	{	return Cmp( pStr, zstr::Length( pStr ) ); }

	oexBOOL Cmp( oexCONST T* pStr, oexUINT uSize )
	{
        if ( uSize != Length() )
            return oexFALSE;
		if ( !uSize )
            return oexTRUE;
		return !os::CSys::MemCmp( m_mem.Ptr(), pStr, uSize );
	}

	oexBOOL ICmp( TStr &str )
	{	return ICmp( str.Ptr(), str.Length() ); }

	oexBOOL ICmp( oexCONST T* pStr )
	{	return ICmp( pStr, zstr::Length( pStr ) ); }

	oexBOOL ICmp( oexCONST T* pStr, oexUINT uSize )
    {   return !str::ICompare( Ptr(), Length(), pStr, uSize ); }

	oexBOOL CmpLen( TStr &str, oexINT x_lLen = -1 )
	{	if ( 0 > x_lLen ) x_lLen = str.Length();
        return CmpLen( str.Ptr(), str.Length(), x_lLen );
    }

	oexBOOL CmpLen( oexCONST T* pStr, oexINT x_lLen = -1 )
	{	oexINT lLen = zstr::Length( pStr );
        if ( 0 > x_lLen ) x_lLen = lLen;
		return CmpLen( pStr, lLen, x_lLen );
    }

	oexBOOL CmpLen( oexCONST T* pStr, oexUINT uSize, oexINT x_lLen = -1 )
    {   if ( 0 > x_lLen ) x_lLen = uSize;
        return !str::CompareLen( Ptr(), Length(), pStr, uSize, x_lLen );
    }

	oexBOOL ICmpLen( TStr &str, oexINT x_lLen = -1 )
	{	if ( 0 > x_lLen ) x_lLen = str.Length();
	 	return ICmpLen( str.Ptr(), str.Length(), x_lLen );
    }

	oexBOOL ICmpLen( oexCONST T* pStr, oexINT x_lLen = -1 )
	{	oexINT lLen = zstr::Length( pStr );
        if ( 0 > x_lLen ) x_lLen = lLen;
		return ICmpLen( pStr, lLen, x_lLen );
    }

	oexBOOL ICmpLen( oexCONST T* pStr, oexUINT uSize, oexINT x_lLen = -1 )
    {   if ( 0 > x_lLen ) x_lLen = uSize;
        return !str::ICompareLen( Ptr(), Length(), pStr, uSize, x_lLen );
    }

	/// Returns the offset of pSub in the string, or
    /// -1 if a pStr is not found.
	oexINT Match( oexCONST T* pSub )
    {   return str::FindSubStr( Ptr(), Length(), pSub, zstr::Length( pSub ) ); }

public:

	/// Replaces single characters in a string
	TStr& Replace( oexCONST T x_tFind, oexCONST T x_tReplace )
    {   str::Replace( _Ptr(), Length(), x_tFind, x_tReplace ); return *this; }

	/// Replaces sub strings with another
	TStr Replace( oexCONST TStr &x_sFind, oexCONST TStr &x_sReplace ) oexCONST
	{
		TStr str;
		oexINT lf = x_sFind.Length();
		if ( !lf )
			return *this;

		oexINT s = 0, e = Length(), lr = x_sReplace.Length(), m;
		do
		{
			// See if we can find the string
			m = str::FindSubStr( Ptr( s ), e - s, x_sFind.Ptr(), lf );

			// Punt if we got nothing
			if ( 0 > m )
			{
				// Sub string never found?
				if ( !s )
					return *this;

				// Return final string
				str += SubStr( s, e - s );
				return str;

			} // end if

			// Append non-matching stuff
			if ( s != m )
				str += SubStr( s, m );

			// Replace with other string if needed
			if ( lr )
				str += x_sReplace;

			// Next point
			s += m + lf;

		} while ( s < e );

		return str;
	}

	//==============================================================
	// Compare functions
	//==============================================================
	static oexINT CmpCompare( TStr pStr1, TStr pStr2 )
    {	return str::Compare( pStr1.Ptr(), pStr1.Length(), pStr2.Ptr(), pStr2.Length() ); }

	static oexINT CmpSubStr( TStr pStr1, TStr pStr2 )
	{	return str::FindSubStr( pStr1.Ptr(), pStr1.Length(), pStr2.Ptr(), pStr2.Length() ); }


public:

    //==============================================================
	// Parsing functions
	//==============================================================

    /// Trims white-space from the front and end of the string
    TStr& TrimWhiteSpace()
    {   return SkipWhiteSpace().RSkipWhiteSpace(); }

	/// Skips white-space characters
	/// White space characters 'c' where ( '!' > c > '~' )
	TStr& SkipWhiteSpace()
	{	return FindInRange( oexT( '!' ), oexT( '~' ) ); }

	/// Skips white-space characters
	/// White space characters 'c' where ( '!' > c > '~' )
	TStr& RSkipWhiteSpace()
	{	return RFindInRange( oexT( '!' ), oexT( '~' ) ); }

	/// Skips any non-white space characters
	/// Non-white space characters 'c' where ( '!' <= c <= '~' )
	TStr& SkipNonWhiteSpace()
	{	return SkipInRange( oexT( '!' ), oexT( '~' ) ); }

	/// Skips any non-white space characters
	/// Non-white space characters 'c' where ( '!' <= c <= '~' )
	TStr& RSkipNonWhiteSpace()
	{	return RSkipInRange( oexT( '!' ), oexT( '~' ) ); }

	/// Skips to the next line, current line is removed from the string.
	TStr& NextLine()
	{	Find( oexTT( T, "\r\n" ) ); return Skip( oexTT( T, "\r\n" ) ); }

	/// Removes white space from the string
	TStr& DropWhiteSpace()
	{	return DropRange( oexT( '!' ), oexT( '~' ), oexFALSE ); }

	/// Skips any character 'c' where ( tMin > c > tMax )
	/// Out of range characters are removed from the string
	TStr& FindInRange( T tMin, T tMax )
	{	oexINT p = str::FindInRange( Ptr(), Length(), tMin, tMax );
		if ( 0 < p ) LTrim( p ); else if ( 0 > p ) Destroy();
		return *this;
	}

	/// Skips any character 'c' where ( tMin > c > tMax )
	/// Out of range characters are removed from the string
	TStr& RFindInRange( T tMin, T tMax )
	{	oexINT lLen = Length(); if ( !lLen ) return *this;
        oexINT p = str::RFindInRange( Ptr(), lLen, tMin, tMax );
		if ( 0 <= p && p < lLen ) RTrim( lLen - p - 1 ); else if ( 0 > p ) Destroy();
		return *this;
	}

	/// Skips any character 'c' where ( tMin <= c <= tMax )
	/// In range characters are removed from the string
	TStr& SkipInRange( T tMin, T tMax )
	{	oexUINT uLen = Length();
		oexINT p = str::SkipInRange( Ptr(), Length(), tMin, tMax );
		if ( 0 < p ) LTrim( p ); else if ( 0 > p ) Destroy();
		return *this;
	}

	/// Skips any character 'c' where ( tMin <= c <= tMax )
	/// In range characters are removed from the string
	TStr& RSkipInRange( T tMin, T tMax )
	{	oexINT lLen = Length(); if ( !lLen ) return *this;
		oexINT p = str::RSkipInRange( Ptr(), lLen, tMin, tMax );
		if ( 0 <= p && p < lLen ) RTrim( lLen - p - 1 ); else if ( 0 > p ) Destroy();
		return *this;
	}

	/// Returns the offset of the first character in pChars found in the string
	/// Returns -1 if no characters found
	oexINT FindChars( oexCONST T* pChars )
	{	return str::FindCharacters( Ptr(), Length(), pChars, zstr::Length( pChars ) );
	}

	/// Skips any characters not in pChars and removes
	/// them from the string
	TStr& Find( oexCONST T* pChars )
	{	oexINT p = str::FindCharacters( Ptr(), Length(), pChars, zstr::Length( pChars ) );
		if ( 0 < p ) LTrim( p ); else if ( 0 > p ) Destroy();
		return *this;
	}

	/// Returns the offset of the specified string, or less than zero if not found
	oexINT FindSubStr( TStr pStr )
	{	return str::FindSubStr( Ptr(), Length(), pStr.Ptr(), pStr.Length() ); }

	/// Returns the offset of the specified string, or less than zero if not found
	oexINT FindSubStr( oexCONST T* pStr )
	{	return str::FindSubStr( Ptr(), Length(), pStr, zstr::Length( pStr ) ); }

	/// Returns the offset of the specified string, or less than zero if not found
	oexINT FindSubStr( oexCONST T* pStr, oexINT nLen )
	{	return str::FindSubStr( Ptr(), Length(), pStr, nLen ); }

	/// Skips any characters in pChars and removes them from
	/// the string.
	TStr& Skip( oexCONST T* pChars )
    {	oexINT p = str::SkipCharacters( Ptr(), Length(), pChars, zstr::Length( pChars ) );
		if ( 0 < p ) LTrim( p ); else if ( 0 > p ) Destroy();
		return *this;
	}

	/// Splits off a token and returns it
	TStr ParseQuoted( oexCONST T *pOpen, oexCONST T *pClose, oexCONST T *pEscape = oexNULL )
    {	oexVERIFY_PTR_NULL_OK( pEscape );
        oexINT i = str::ParseQuoted(    Ptr(), Length(),
                                        pOpen, zstr::Length( pOpen ),
                                        pClose, zstr::Length( pOpen ),
                                        pEscape, pEscape ? zstr::Length( pEscape ) : 0 );
		if ( 0 >= i ) return TStr();
		TStr str = SubStr( 1, i - 1 );
		LTrim( i + 1 );
		return str;
	}

	/// Splits off a token and returns it
	TStr ParseToken( oexCONST T* pValid )
	{	oexVALIDATE_PTR( pValid );
		oexINT i = 0;
		TStr str = Token( pValid, &i );
		if ( i ) LTrim( i );
		return str;
	}

	/// Splits off a token and returns it
	TStr ParseNextToken( oexCONST T* pValid )
	{	oexVALIDATE_PTR( pValid );
		oexINT i = 0;
		TStr str = NextToken( pValid, &i );
		if ( i ) LTrim( i );
		return str;
	}

	/// Splits off a token and returns it
	TStr ParseNextToken( T tMin = oexT( '!' ), T tMax = oexT( '~' ) )
	{	oexINT i = 0;
		TStr str = NextToken( tMin, tMax, &i );
		if ( i ) LTrim( i );
		return str;
	}

	/// Splits off a string up any of the terminator characters,
	/// Ignores terminator characters if preceeded by any character
	/// in the escape string.
	TStr Parse( oexCONST T* pTerm, oexCONST T* pEscape = oexNULL )
	{	oexASSERT_PTR( pTerm ); oexASSERT_PTR_NULL_OK( pEscape );
        oexINT i = str::FindTerm( Ptr(), Length(), pTerm, zstr::Length( pTerm ), pEscape, pEscape ? zstr::Length( pEscape ) : 0 );
		if ( 0 > i ) return TStr();
		TStr str( Ptr(), 0, i );
		LTrim( i );
		return str;
	}

	/// Splits off a string up to any of the terminator characters
	TStr RParse( oexCONST T* pTerm )
	{	oexASSERT_PTR( pTerm );
        oexINT lLen = Length();
        oexINT i = str::RFindTerm( Ptr(), lLen, pTerm, zstr::Length( pTerm ) );
		if ( 0 > i )
			return TStr();
		TStr str( Ptr( i + 1 ), lLen - i - 1 );
		RTrim( lLen - i );
		return str;
	}

	oexBOOL IsMatchAt( oexUINT i, oexCONST T* pChars )
	{	oexASSERT_PTR( pChars );
        return 0 <= str::FindCharacter( pChars, zstr::Length( pChars ), *Ptr( i ) );
	}

    oexBOOL IsInRangeAt( oexUINT i, T tMin, T tMax )
	{   T ch = *Ptr( i );
        return tMin <= ch && tMax >= ch;
	}

    oexBOOL IsWhiteSpaceAt( oexUINT i )
    {   return !IsInRangeAt( i, oexT( '!' ), oexT( '~' ) ); }

	/// Finds the first token in a string and returns it
	TStr NextToken( T tMin, T tMax, oexINT *puI = oexNULL )
	{	return NextToken( Ptr(), Length(), tMin, tMax, puI ); }

	static TStr NextToken( oexCONST T* pStr, T tMin, T tMax, oexINT *puI = oexNULL )
	{	return NextToken( pStr, zstr::Length( pStr ), tMin, tMax, puI ); }

	static TStr NextToken( TStr str, T tMin, T tMax, oexINT *puI = oexNULL )
	{	return NextToken( str.Ptr(), str.Length(), tMin, tMax, puI ); }

	static TStr NextToken( oexCONST T* pStr, oexUINT uSize, T tMin, T tMax, oexINT *puI  = oexNULL )
	{
		// Sanity check
		if ( !oexVERIFY_PTR( pStr ) )
			return TStr();

		oexUINT i = 0;

		// Ensure valid index pointer
		if ( !oexVERIFY_PTR_NULL_OK( puI ) )
			puI = oexNULL;

		// Start at this index
		else if ( puI )
			i = *puI;

		// Ensure we're not beyond the buffer
		if ( i >= uSize )
			return TStr();

		// Skip to previous position
		pStr += i;
		uSize -= i;

		// Find the start of a token
        oexINT nStart = str::FindInRange( pStr, uSize, tMin, tMax );
		if ( 0 > nStart )
			return TStr();

		// Find the end of the token
		oexINT nEnd = str::SkipInRange( &pStr[ nStart ], uSize - nStart, tMin, tMax );
		if ( 0 >= nEnd )
			return TStr();

		// Add offset
		if ( puI )
			*puI += nStart + nEnd;

		return TStr( &pStr[ nStart ], 0, nEnd );
	}


	/// Finds the first token in a string and returns it
	TStr NextToken( oexCONST T* pValid, oexINT *puI = oexNULL )
	{	return NextToken( Ptr(), Length(), pValid, puI ); }

	static TStr NextToken( oexCONST T* pStr, oexCONST T* pValid, oexINT *puI = oexNULL )
	{	return NextToken( pStr, zstr::Length( pStr ), pValid, puI ); }

	static TStr NextToken( TStr str, oexCONST T* pValid, oexINT *puI = oexNULL )
	{	return NextToken( str.Ptr(), str.Length(), pValid, puI ); }

	static TStr NextToken( oexCONST T* pStr, oexUINT uSize, oexCONST T *pValid, oexINT *puI  = oexNULL )
	{
		// Sanity check
		if ( !oexCHECK_PTR( pStr ) || !oexCHECK_PTR_NULL_OK( pValid ) )
			return TStr();

		if ( !pValid )
			return TStr( pStr, uSize );

		oexUINT i = 0;
		oexUINT uValid = zstr::Length( pValid );

		// Ensure valid index pointer
		if ( !oexVERIFY_PTR_NULL_OK( puI ) )
			puI = oexNULL;

		// Start at this index
		else if ( puI )
			i = *puI;

		// Ensure we're not beyond the buffer
		if ( i >= uSize )
			return TStr();

		// Skip to previous position
		pStr += i;
		uSize -= i;

		// Find the start of a token
		oexINT nStart = str::FindCharacters( pStr, uSize, pValid, uValid );
		if ( 0 > nStart )
			return TStr();

		// Find the end of the token
		oexINT nEnd = str::SkipCharacters( &pStr[ nStart ], uSize - nStart, pValid, uValid );
		if ( 0 >= nEnd )
			return TStr();

		// Add offset
		if ( puI )
			*puI += nStart + nEnd;

		return TStr( &pStr[ nStart ], 0, nEnd );
	}

	/// Returns the token at the current point in the string
	TStr Token( oexCONST T* pValid, oexINT *puI = oexNULL )
	{	return Token( Ptr(), Length(), pValid, puI ); }
	static TStr Token( oexCONST T* pStr, oexCONST T* pValid, oexINT *puI = oexNULL )
	{	return Token( pStr, zstr::Length( pStr ), pValid, puI ); }
	static TStr Token( TStr str, oexCONST T* pValid, oexINT *puI = oexNULL )
	{	return Token( str.Ptr(), str.Length(), pValid, puI ); }
	static TStr Token( oexCONST T* pStr, oexUINT uSize, oexCONST T *pValid, oexINT *puI  = oexNULL )
	{
		// Sanity check
		if ( !oexCHECK_PTR( pStr ) || !oexCHECK_PTR_NULL_OK( pValid ) )
			return TStr();

		if ( !pValid )
			return TStr( pStr, uSize );

		oexUINT i = 0;
		oexUINT uValid = zstr::Length( pValid );

		// Ensure valid index pointer
		if ( !oexVERIFY_PTR_NULL_OK( puI ) )
			puI = oexNULL;

		// Start at this index
		else if ( puI )
			i = *puI;

		// Ensure we're not beyond the buffer
		if ( i >= uSize )
			return TStr();

		// Skip to previous position
		pStr += i;
		uSize -= i;

		// Find the end of the token
		oexINT nEnd = str::SkipCharacters( pStr, uSize, pValid, uValid );
		if ( 0 >= nEnd )
			return TStr();

		// Add offset
		if ( puI )
			*puI += nEnd;

		return TStr( pStr, 0, nEnd );
	}

public:

    /// Concatinates two strings into a path
    static TStr BuildPath( TStr x_sRoot, TStr x_sPath, T tSep = oexTC( T, '/' ) )
    {   return x_sRoot.RTrim( oexTT( T, "\\/" ) ) << tSep << x_sPath.LTrim( oexTT( T, "\\/" ) ); }

    /// Concatinates two strings into a path
    TStr& BuildPath( TStr x_sPath, T tSep = oexTC( T, '/' ) )
    {   RTrim( oexTT( T, "\\/" ) );
        *this << tSep << x_sPath.LTrim( oexTT( T, "\\/" ) );
        RTrim( oexTT( T, "\\/" ) );
        return *this;
    }

    /// Returns the root path of str
    TStr GetPath()
    {   TStr str( *this );
        str.RParse( oexTT( T, "\\/" ) );
        return str.RTrim( oexTT( T, "\\/" ) );
    }

    /// Returns the root path of str
    TStr GetFileName()
    {   TStr str( *this );
        return str.RParse( oexTT( T, "\\/" ) );
    }

    /// Returns the file extension of the file path
    TStr GetFileExtension()
    {	TStr sFilename = GetFileName();
		oexINT nOffset = sFilename.FindChars( oexTT( T, "." ) );
		if ( 0 > nOffset )
			return TStr();
		return sFilename.LTrim( nOffset + 1 );
    }

private:

    /// The string memory
    TMem< T >       m_mem;

    /// The length of the string
    oexINT          m_nLength;

    /// Offset into the string, this is invaluable for text parsing
    oexINT          m_uOffset;

#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )

public:

    /// Registers the source file and line number
    TStr& _Log( oexUINT x_uLine, oexCSTR x_pFile )
    {   m_uLine = x_uLine;
        m_pFile = x_pFile;
        return *this;
    }

private:

    /// Holds the line number of the allocating call
    oexUINT         m_uLine;

    /// Holds the file name of the allocating call
    oexCSTR         m_pFile;

#endif

};


/// Unicode / Multibyte string class
typedef TStr< oexTCHAR >    CStr;

/// Unicode string class
typedef TStr< oexCHARW >    CStrW;

/// 8 bit character string
typedef TStr< oexCHAR >     CStr8;

/// 16 bit character string
typedef TStr< oexINT16 >    CStr16;

/// 32 bit character string
typedef TStr< oexINT32 >    CStr32;

/// 64 bit character string
typedef TStr< oexINT64 >    CStr64;


/// Compare
namespace obj
{
    // iii Unfortunately, Doxygen craps out on these next
    //     function templates.

	//==============================================================
	// Size()
	//==============================================================
	/// Specialized for CStr
	/**
		\param [in] x_obj		-	The object whose size will be
								    returned.

		This just specializes for the case of CStr.

		+++ I don't like having the reference to CStr here, so I really
		should figure out another way.  Any suggestions?
	*/
	template <> oexITS oexUINT Size< CStrW >( oexCONST CStrW *x_obj )
    {   return ( (CStrW*)x_obj )->LengthInBytes(); }

	template <> oexITS oexUINT Size< CStr8 >( oexCONST CStr8 *x_obj )
    {   return ( (CStr8*)x_obj )->LengthInBytes(); }

	template <> oexITS oexUINT Size< CStr16 >( oexCONST CStr16 *x_obj )
    {   return ( (CStr16*)x_obj )->LengthInBytes(); }

//	template <> oexITS oexUINT Size< CStr32 >( oexCONST CStr32 *x_obj )
//    {   return ( (CStr32&)x_obj ).LengthInBytes(); }

	//==============================================================
	// Ptr()
	//==============================================================
	/// Specialized for CStr
	/**
		\param [in] x_ptr		-	The object whose size will be
									returned.

		This just specializes for the case of CStr.

		+++ It would be nice to eliminate the ref to CStr here.
	*/
	template <> oexITS oexPVOID Ptr< CStrW >( oexCONST CStrW *x_ptr )
    {   return (oexPVOID)( (CStrW*)x_ptr )->Ptr(); }

	template <> oexITS oexPVOID Ptr< CStr8 >( oexCONST CStr8 *x_ptr )
    {   return (oexPVOID)( (CStr8*)x_ptr )->Ptr(); }

	template <> oexITS oexPVOID Ptr< CStr16 >( oexCONST CStr16 *x_ptr )
    {   return (oexPVOID)( (CStr16*)x_ptr )->Ptr(); }

//	template <> oexITS oexPVOID Ptr< CStr32 >( oexCONST CStr32 *x_ptr )
//    {   return (oexPVOID)( (CStr32*)x_ptr )->Ptr(); }

	//==============================================================
	// Compare()
	//==============================================================
    /// Special overload for CStr

    template <> oexITS oexBOOL Compare< CStrW >( oexCONST CStrW &k1, oexCONST CStrW &k2 )
    {   return (CStrW&)k1 == (CStrW&)k2; }

    template <> oexITS oexBOOL Compare< CStr8 >( oexCONST CStr8 &k1, oexCONST CStr8 &k2 )
    {   return (CStr8&)k1 == (CStr8&)k2; }

    template <> oexITS oexBOOL Compare< CStr16 >( oexCONST CStr16 &k1, oexCONST CStr16 &k2 )
    {   return (CStr16&)k1 == (CStr16&)k2; }

//    template <> oexITS oexBOOL Compare< CStr32 >( oexCONST CStr32 &k1, oexCONST CStr32 &k2 )
//    {   return (CStr32&)k1 == (CStr32&)k2; }

};
