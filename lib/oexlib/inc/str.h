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

	/// Size type
	typedef oexSIZE_T			t_size;

	/// Char type
	typedef T					t_char;

	/// Buffer type
	typedef TMem< t_char >		t_buffer;

public:

    /// Default constructor
    TStr()
    {
        m_nLength = 0;
        m_nOffset = 0;

#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )
        m_pFile = oexNULL;
        m_uLine = 0;
#endif
    }

	TStr( oexCONST t_buffer &buf, t_size uLength )
	{	m_nLength = uLength;
        m_nOffset = 0;
#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )
        m_pFile = oexNULL;
        m_uLine = 0;
#endif
        m_mem.Share( buf );
	}


	TStr( oexCONST TStr &str )
	{	m_nLength = 0;
        m_nOffset = 0;
#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )
        m_pFile = oexNULL;
        m_uLine = 0;
#endif
		Set( str );
//		if ( str.Length() ) Set( str.Ptr() );
	}

	TStr( oexCONST T *pStr )
	{	m_nLength = 0;
        m_nOffset = 0;
#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )
        m_pFile = oexNULL;
        m_uLine = 0;
#endif
		Set( pStr );
	}

	TStr( oexCONST T *pStr, t_size uSize )
	{	m_nLength = 0;
        m_nOffset = 0;
#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )
        m_pFile = oexNULL;
        m_uLine = 0;
#endif
		Set( pStr, uSize );
	}

	TStr( oexCONST T *pStr, t_size nStart, t_size uLen )
	{	m_nLength = 0;
        m_nOffset = 0;
#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )
        m_pFile = oexNULL;
        m_uLine = 0;
#endif
		Sub( pStr, nStart, uLen );
	}

	TStr( oexCONST T *pStr, t_size uSize, t_size nStart, t_size uLen )
	{	m_nLength = 0;
        m_nOffset = 0;
#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )
        m_pFile = oexNULL;
        m_uLine = 0;
#endif
		Sub( pStr, uSize, nStart, uLen );
	}

	TStr( oexCONST oexINT nVal )
	{	m_nLength = 0;
        m_nOffset = 0;
#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )
        m_pFile = oexNULL;
        m_uLine = 0;
#endif
		SetNum( oexTT( T, "%i" ), (oexINT)nVal );
	}

	TStr( oexCONST oexUINT uVal )
	{	m_nLength = 0;
        m_nOffset = 0;
#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )
        m_pFile = oexNULL;
        m_uLine = 0;
#endif
		SetNum( oexTT( T, "%u" ), (oexUINT)uVal );
	}

	TStr( oexCONST oexLONG lVal )
	{	m_nLength = 0;
        m_nOffset = 0;
#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )
        m_pFile = oexNULL;
        m_uLine = 0;
#endif
		SetNum( oexTT( T, "%i" ), (oexINT64)lVal );
	}

	TStr( oexCONST oexULONG ulVal )
	{	m_nLength = 0;
        m_nOffset = 0;
#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )
        m_pFile = oexNULL;
        m_uLine = 0;
#endif
		SetNum( oexTT( T, "%u" ), (oexUINT64)ulVal );
	}

	TStr( oexCONST oexDOUBLE dStr )
	{	m_nLength = 0;
        m_nOffset = 0;
#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )
        m_pFile = oexNULL;
        m_uLine = 0;
#endif
		SetNumTrim( oexTT( T, "%f" ), oexNULL, oexTT( T, "0" ), (oexDOUBLE)dStr );
	}

	TStr( oexCONST oexINT64 llStr )
	{	m_nLength = 0;
        m_nOffset = 0;
#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )
        m_pFile = oexNULL;
        m_uLine = 0;
#endif
		SetNum( oexTT( T, "%lld" ), (oexINT64)llStr );
	}

	TStr( oexCONST oexUINT64 llStr )
	{	m_nLength = 0;
        m_nOffset = 0;
#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )
        m_pFile = oexNULL;
        m_uLine = 0;
#endif
		SetNum( oexTT( T, "%llu" ), (oexUINT64)llStr );
	}

	TStr( oexCONST T tVal )
	{	m_nLength = 0;
        m_nOffset = 0;
#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )
        m_pFile = oexNULL;
        m_uLine = 0;
#endif
		Set( tVal );
	}

	TStr( oexCONST oexGUID &guid )
	{	m_nLength = 0;
        m_nOffset = 0;
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
        m_nOffset = 0;

        // Lose the string memory
        m_mem.OexDelete();
    }

public:

    /// Shares memory of another object
    TStr& Share( oexCONST TStr &x_str )
    {
        // Share the memory
        m_nLength = x_str.m_nLength;
        m_nOffset = x_str.m_nOffset;
        m_mem.Share( x_str.m_mem );
        return *this;
    }

    /// Creates our own copy of a string if it's shared
	oexBOOL Unshare()
	{
		// Punt if not shared or offset
		if ( !m_nOffset && 1 == m_mem.GetRefCount() )
			return oexTRUE;

		// Figure out in use size of current buffer
		t_size nOldSize = m_mem.Size();

		// If it's one, it's just a NULL terminator
		if ( m_nOffset >= nOldSize || 1 >= ( nOldSize - m_nOffset ) )
			Destroy();

		else
		{
			// Size of the new buffer
			t_size nNewSize = nOldSize - m_nOffset;

            t_buffer mem;
            if ( !oexVERIFY_PTR( mem.OexNew( nNewSize ).Ptr() ) )
                return oexFALSE;

            // Copy string data
			os::CSys::MemCpy( mem.Ptr(), m_mem.c_Ptr( m_nOffset ), nNewSize * sizeof( T ) );

            // Use the new memory
			m_mem.Assume( mem );

			// Reset offset
			m_nLength = nNewSize - 1;
			m_nOffset = 0;

		} // end if

		return oexTRUE;
	}

	/// Allocates at least the number of bytes specified
	T* Allocate( t_size x_uSize )
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
        } // end if

		// Verify we got valid memory
        if ( !oexVERIFY_PTR( pMem ) )
        {	oexDSHOWVAL( (oexUINT)x_uSize, "%u : " );
        	oexDSHOWVAL( (oexUINT)oexPtrToInt( pMem ), "0x%x\n" );
		    return oexNULL;
		} // endi f

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
			m_nLength = oexMAXSIZE;

		return pMem;
    }

	/// Returns a const pointer to the internal string buffer
	oexCONST T* Ptr() const
	{
		// Ensure string
		if ( !m_mem.Size() )
			( (TStr*)this )->OexAllocate( 0 );

		return ( (TStr*)this )->m_mem.Ptr( m_nOffset );
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

		return ( (TStr*)this )->m_mem.Ptr( x_uOffset + m_nOffset );
	}

	/// Returns a writable pointer (use with care)
	T* _Ptr() const
	{	( (TStr*)this )->Unshare(); return (T*)Ptr(); }

	/// Returns a writable pointer (use with care)
	T* _Ptr( oexUINT x_uOffset ) const
	{	( (TStr*)this )->Unshare(); return (T*)Ptr( x_uOffset ); }

	/// Returns the specified character
	oexCONST T operator []( oexUINT x_uOffset ) const
	{	return *Ptr( x_uOffset ); }

	/// Returns the first character in the buffer
	oexCONST T& operator *() const
    {   return *Ptr(); }

	/// TMem object
	operator t_buffer&() const
	{	return m_mem; }

	/// Returns memory object reference
	t_buffer& Mem() { return m_mem; }

	/// Returns const memory object reference
	oexCONST t_buffer& c_Mem() oexCONST { return m_mem; }

	/// Calculates the length of the string by finding the NULL terminator
	t_size CalculateLength()
	{
		m_nLength = 0;
		t_size lMax = m_mem.Size();

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
	t_size Length() oexCONST
	{	if ( oexMAXSIZE == m_nLength )
			return ( (TStr*)this )->CalculateLength();
		return m_nLength - m_nOffset;
	}

    /// Manually sets the length
    /// !!! This allows NULL characters to be in the string.
    t_size SetLength( t_size x_nLength )
    {
        // Do we already have the correct length?
        if ( x_nLength == m_nLength )
            return m_nLength;

        // Ensure we have that much data
        if ( !OexAllocate( m_nOffset + x_nLength ) )
            return 0;

        // Just accept the callers size
        m_nLength = x_nLength;

        return m_nLength;
    }

    /// Manually sets the length
    /// This function is for buffer sharing
	/// !!! This function may cause a non null terminated string
    t_size _SetLength( t_size x_nLength )
    {
		// Ensure it's within reason
		if ( x_nLength > m_mem.Size() )
			x_nLength = m_mem.Size();

        // Just accept the callers size
        m_nLength = x_nLength;

        return m_nLength;
    }

	/// Returns the size of the underlying memory buffer
	t_size Size()
	{
		return (t_size)m_mem.Size();
	}

    /// Returns the size in bytes, of a single character
    t_size SizeOfChar() oexCONST
    {   return sizeof( T ); }

    /// Returns the length of the string in bytes
    /**
        \param x_uAdd   -   Number of *characters* to
                            add to the returned length
    */
    t_size LengthInBytes( t_size x_uAdd = 0 ) oexCONST
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

	TStr& operator = ( oexCONST oexLONG lVal )
	{	return SetNum( oexTT( T, "%lld" ), (oexINT64)lVal ); }

	TStr& operator = ( oexCONST oexULONG ulVal )
	{	return SetNum( oexTT( T, "%llu" ), (oexUINT64)ulVal ); }

	TStr& operator = ( oexCONST oexDOUBLE dStr )
	{	return SetNumTrim( oexTT( T, "%f" ), oexNULL, oexTT( T, "0" ), (oexDOUBLE)dStr ); }

	TStr& operator = ( oexCONST oexINT64 llStr )
	{	return SetNum( oexTT( T, "%lld" ), (oexINT64)llStr ); }

	TStr& operator = ( oexCONST oexUINT64 llStr )
	{	return SetNum( oexTT( T, "%llu" ), (oexUINT64)llStr ); }

	TStr& operator += ( oexCONST oexINT nVal )
	{	return AppendNum( oexTT( T, "%i" ), (oexINT)nVal ); }

	TStr& operator += ( oexCONST oexUINT uVal )
	{	return AppendNum( oexTT( T, "%u" ), (oexUINT)uVal ); }

	TStr& operator += ( oexCONST oexLONG lVal )
	{	return AppendNum( oexTT( T, "%lld" ), (oexINT64)lVal ); }

	TStr& operator += ( oexCONST oexULONG ulVal )
	{	return AppendNum( oexTT( T, "%llu" ), (oexUINT64)ulVal ); }

	TStr& operator += ( oexCONST oexDOUBLE dVal )
	{	return AppendNumTrim( oexTT( T, "%f" ), oexNULL, oexTT( T, "0" ), (oexDOUBLE)dVal ); }

	TStr& operator += ( oexCONST oexUINT64 llVal )
	{	return AppendNum( oexTT( T, "%llu" ), (oexUINT64)llVal ); }

	TStr& operator += ( oexCONST oexINT64 llVal )
	{	return AppendNum( oexTT( T, "%lld" ), (oexINT64)llVal ); }

	TStr& operator += ( oexCONST T chVal )
	{	return Append( &chVal, 1 ); }

	TStr& operator += ( oexCONST oexGUID &guid )
	{	return Append( TStr().GuidToString( guid ) ); }

	TStr& operator << ( oexCONST oexINT nVal )
	{	return AppendNum( oexTT( T, "%i" ), (oexINT)nVal ); }

	TStr& operator << ( oexCONST oexUINT uVal )
	{	return AppendNum( oexTT( T, "%u" ), (oexUINT)uVal ); }

	TStr& operator << ( oexCONST oexLONG lVal )
	{	return AppendNum( oexTT( T, "%lld" ), (oexINT64)lVal ); }

	TStr& operator << ( oexCONST oexULONG ulVal )
	{	return AppendNum( oexTT( T, "%llu" ), (oexUINT64)ulVal ); }

	TStr& operator << ( oexCONST oexDOUBLE dVal )
	{	return AppendNumTrim( oexTT( T, "%f" ), oexNULL, oexTT( T, "0" ), (oexDOUBLE)dVal ); }

	TStr& operator << ( oexCONST oexINT64 llVal )
	{	return AppendNum( oexTT( T, "%lld" ), (oexINT64)llVal ); }

	TStr& operator << ( oexCONST oexUINT64 llVal )
	{	return AppendNum( oexTT( T, "%llu" ), (oexUINT64)llVal ); }

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
             && rStr.m_nOffset == m_nOffset )
            return oexTRUE;

        // I suppose we'll have to actually compare the strings
        return !Compare( rStr.Ptr(), rStr.Length() );
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

    /// Slices chars and returns the sliced string
	TStr Slice( oexUINT x_uLen = 0 )
	{
		// Grab the slice
		TStr ret( Ptr(), Length(), 0, x_uLen );
		LTrim( x_uLen );
		return ret;
	}

    /// Compare to const string
    t_size Compare( oexCONST T *x_pPtr, t_size x_uLen )
    {   return str::Compare( Ptr(), Length(), x_pPtr, x_uLen ); }

    /// Extracts a sub string
	TStr& Sub( t_size x_uStart, t_size x_uLen = 0 )
	{
        // Current string length
		t_size uSize = Length();

		// Can't shorten the string if we don't own the memory
		if ( x_uLen && x_uLen < ( uSize - x_uStart ) && 1 != m_mem.GetRefCount() )
			return Set( SubStr( x_uStart, x_uLen ) );

        // Wiping out the string?
        if ( x_uStart >= uSize )
        {   Destroy(); return *this; }

        // Shift the start of the string
        m_nOffset += x_uStart;

        // Do we need to adjust the length?
        if ( x_uLen && x_uLen < ( uSize - x_uStart ) )
        {   OexAllocate( m_nOffset + x_uLen );
            m_nLength = m_nOffset + x_uLen;
        } // end if

		return *this;

	}

	TStr& Sub( TStr &str, t_size nStart, t_size uLen )
	{	return Sub( str.Ptr(), str.Length(), nStart, uLen ); }

	TStr& Sub( oexCONST T* pStr, t_size nStart, t_size uLen )
	{	return Sub( pStr, zstr::Length( pStr ), nStart, uLen ); }

	TStr& Sub( oexCONST T* pStr, t_size uSize, t_size nStart, t_size uLen )
	{
		t_size uStart = 0;
		if ( 0 <= nStart )
			uStart = (oexUINT)nStart;
		else
			uStart = (oexUINT)( (oexINT)uSize - nStart );

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
	TStr SubStr( t_size nStart, t_size uLen ) oexCONST
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
	TStr& Move( t_size uFrom, t_size uTo, t_size uSize )
	{	Move( _Ptr(), Length(), uFrom, uTo, uSize ); return *this; }
	t_size Move( T* pPtr, t_size uLen, t_size uFrom, t_size uTo, t_size uSize )
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

	TStr& Set( oexCONST T* x_pStr, t_size uSize )
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
        m_nOffset = 0;

		// Allocate space for new string
		if ( !oexVERIFY_PTR( OexAllocate( uSize ) ) )
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
        m_nOffset = 0;

		// Allocate space for new string
		T* pPtr = OexAllocate( 1 );
		if ( !oexVERIFY( pPtr ) )
			return *this;

		pPtr[ 0 ] = x_chVal;

		m_nLength = 1;

		return *this;
	}

    template< typename T_CHAR >
        TStr& Cnv( oexCONST T_CHAR *x_pStr, t_size x_uLen = 0 )
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
        TStr& Bin( oexCONST T_STR *x_pStr, t_size x_uLen = 0 )
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
        t_size uT = uBytes / sizeof( T );
        t_size uR = uBytes - ( uT * sizeof( T ) );

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
        t_size uLen = x_sStr.Length();

		if ( !uLen )
		{	OexAllocate( 0 );
            return *this;
        } // end if

        // Get length of in comming string in bytes
        t_size uBytes = uLen * sizeof( T_CHAR );

        // Calculate whole bytes and remainder
        t_size uT = uBytes / sizeof( T );
        t_size uR = uBytes - ( uT * sizeof( T ) );

        // Allocate enough space
        if ( !OexAllocate( uT + uR ) )
            return *this;

        // Binary copy
        os::CSys::MemCpy( _Ptr(), x_sStr.Ptr(), uBytes );

        // Save length
        m_nLength = uT + uR;

        return *this;
    }

#if !defined( OEX_NOWCHAR )

	TStr& ToWc( oexCONST TStr< oexCHAR8 > &x_sStr )
	{
		if ( !oexCHECK( sizeof( T ) == sizeof( oexCHARW ) ) )
			return *this;

        t_size uLen = x_sStr.Length();

		if ( !uLen )
		{	OexAllocate( 0 );
            return *this;
        } // end if

        // Get length of in comming string in bytes
        t_size uConv = os::CSys::MbsToWcs( oexNULL, 0, x_sStr.Ptr(), uLen );

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

        t_size uLen = x_sStr.Length();

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

#endif

	// Concatenation operator
	TStr& Append( oexCONST TStr &sStr )
	{	return Append( ( (TStr&)sStr ).Ptr(), ( (TStr&)sStr ).Length() ); }

	// Concatenation operator
	TStr& Append( oexCONST T* pStr )
	{	return Append( pStr, zstr::Length( pStr ) ); }

	// Concatenation operator
	TStr& Append( oexCONST T* pStr, t_size uSize )
	{
		// Allocate space for new string
		t_size uOldSize = Length();
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

	TStr& Trim( oexCONST T* pChars )
	{	return RTrim( pChars ).LTrim( pChars ); }

	TStr& operator >>= ( t_size uChars )
	{	return LTrim( uChars ); }

	TStr& operator <<= ( t_size uChars )
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

	TStr& vPrint( oexCONST T *x_pFmt, oexVaList x_pArgs )
	{
		// Verify input string
		if ( !oexVERIFY( x_pFmt ) )
		{	OexAllocate( 0 ); return *this; }

		oexRESULT res;
		t_size uSize = oexSTRSIZE;
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

		} while ( uSize && os::CSys::c_StrErr_INSUFFICIENT_BUFFER == (t_size)res );

		// Verfiy that the string was copied correctly
		oexASSERT( (oexRESULT)os::CSys::c_StrErr_OK == res );

		// Set the length of the string
		Length();

		return *this;
	}

	/// Formats a string
	TStr& Print( oexCONST T *pFmt, ... )
	{
		oexVaList ap;
		oexVaStart( ap, pFmt );
		vPrint( pFmt, ap );
		oexVaEnd( ap );
		return *this;
	}

	TStr& vAppendPrint( oexCONST T *x_pFmt, oexVaList x_pArgs )
	{
		// Verify input string
		if ( !oexCHECK_PTR( x_pFmt ) )
			return *this;

		oexRESULT res;
		t_size uSize = oexSTRSIZE;
		t_size uLen = Length();
		do
		{
			// Allocate buffer
			// The - 1 is a performance boost, this keeps us under the TMemory block size.
			T* pPtr = OexAllocate( uLen + uSize - 1 );
			if ( !oexCHECK_PTR( pPtr ) )
				return *this;

			// Attempt to format the string
			os::CSys::vStrFmt( &res, &pPtr[ uLen ], uSize - 1, x_pFmt, x_pArgs );

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
	TStr& AppendPrint( oexCONST T *pFmt, ... )
	{
		oexVaList ap;
		oexVaStart( ap, pFmt );
		vAppendPrint( pFmt, ap );
		oexVaEnd( ap );
		return *this;
	}

	template < typename T_P1 >
		TStr& Mks( T_P1 p1 )
		{	*this += p1;
			return *this;
		}

	template < typename T_P1, typename T_P2 >
		TStr& Mks( T_P1 p1, T_P2 p2 )
		{	*this += p1;
			*this += p2;
			return *this;
		}

	template < typename T_P1, typename T_P2, typename T_P3 >
		TStr& Mks( T_P1 p1, T_P2 p2, T_P3 p3 )
		{	*this += p1;
			*this += p2;
			*this += p3;
			return *this;
		}

	template < typename T_P1, typename T_P2, typename T_P3, typename T_P4 >
		TStr& Mks( T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4 )
		{	*this += p1;
			*this += p2;
			*this += p3;
			*this += p4;
			return *this;
		}

	template < typename T_P1, typename T_P2, typename T_P3, typename T_P4,
	           typename T_P5 >
		TStr& Mks( T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4, T_P5 p5 )
		{	*this += p1;
			*this += p2;
			*this += p3;
			*this += p4;
			*this += p5;
			return *this;
		}

	template < typename T_P1, typename T_P2, typename T_P3, typename T_P4,
	           typename T_P5, typename T_P6 >
		TStr& Mks( T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4, T_P5 p5, T_P6 p6 )
		{	*this += p1;
			*this += p2;
			*this += p3;
			*this += p4;
			*this += p5;
			*this += p6;
			return *this;
		}

	template < typename T_P1, typename T_P2, typename T_P3, typename T_P4,
	           typename T_P5, typename T_P6, typename T_P7 >
		TStr& Mks( T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4, T_P5 p5, T_P6 p6, T_P7 p7 )
		{	*this += p1;
			*this += p2;
			*this += p3;
			*this += p4;
			*this += p5;
			*this += p6;
			*this += p7;
			return *this;
		}

	template < typename T_P1, typename T_P2, typename T_P3, typename T_P4,
	           typename T_P5, typename T_P6, typename T_P7, typename T_P8 >
		TStr& Mks( T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4, T_P5 p5, T_P6 p6, T_P7 p7, T_P8 p8 )
		{	*this += p1;
			*this += p2;
			*this += p3;
			*this += p4;
			*this += p5;
			*this += p6;
			*this += p7;
			*this += p8;
			return *this;
		}

	template < typename T_P1, typename T_P2, typename T_P3, typename T_P4,
	           typename T_P5, typename T_P6, typename T_P7, typename T_P8,
	           typename T_P9 >
		TStr& Mks( T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4, T_P5 p5, T_P6 p6, T_P7 p7, T_P8 p8,
		           T_P9 p9 )
		{	*this += p1;
			*this += p2;
			*this += p3;
			*this += p4;
			*this += p5;
			*this += p6;
			*this += p7;
			*this += p8;
			*this += p9;
			return *this;
		}

	template < typename T_P1, typename T_P2, typename T_P3, typename T_P4,
	           typename T_P5, typename T_P6, typename T_P7, typename T_P8,
	           typename T_P9, typename T_P10 >
		TStr& Mks( T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4, T_P5 p5, T_P6 p6, T_P7 p7, T_P8 p8,
		           T_P9 p9, T_P10 p10 )
		{	*this += p1;
			*this += p2;
			*this += p3;
			*this += p4;
			*this += p5;
			*this += p6;
			*this += p7;
			*this += p8;
			*this += p9;
			*this += p10;
			return *this;
		}

	template < typename T_P1, typename T_P2, typename T_P3, typename T_P4,
	           typename T_P5, typename T_P6, typename T_P7, typename T_P8,
	           typename T_P9, typename T_P10, typename T_P11 >
		TStr& Mks( T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4, T_P5 p5, T_P6 p6, T_P7 p7, T_P8 p8,
		           T_P9 p9, T_P10 p10, T_P11 p11 )
		{	*this += p1;
			*this += p2;
			*this += p3;
			*this += p4;
			*this += p5;
			*this += p6;
			*this += p7;
			*this += p8;
			*this += p9;
			*this += p10;
			*this += p11;
			return *this;
		}

	template < typename T_P1, typename T_P2, typename T_P3, typename T_P4,
	           typename T_P5, typename T_P6, typename T_P7, typename T_P8,
	           typename T_P9, typename T_P10, typename T_P11, typename T_P12 >
		TStr& Mks( T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4, T_P5 p5, T_P6 p6, T_P7 p7, T_P8 p8,
		           T_P9 p9, T_P10 p10, T_P11 p11, T_P12 p12 )
		{	*this += p1;
			*this += p2;
			*this += p3;
			*this += p4;
			*this += p5;
			*this += p6;
			*this += p7;
			*this += p8;
			*this += p9;
			*this += p10;
			*this += p11;
			*this += p12;
			return *this;
		}

	template < typename T_P1, typename T_P2, typename T_P3, typename T_P4,
	           typename T_P5, typename T_P6, typename T_P7, typename T_P8,
	           typename T_P9, typename T_P10, typename T_P11, typename T_P12,
	           typename T_P13 >
		TStr& Mks( T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4, T_P5 p5, T_P6 p6, T_P7 p7, T_P8 p8,
		           T_P9 p9, T_P10 p10, T_P11 p11, T_P12 p12, T_P13 p13 )
		{	*this += p1;
			*this += p2;
			*this += p3;
			*this += p4;
			*this += p5;
			*this += p6;
			*this += p7;
			*this += p8;
			*this += p9;
			*this += p10;
			*this += p11;
			*this += p12;
			*this += p13;
			return *this;
		}

	template < typename T_P1, typename T_P2, typename T_P3, typename T_P4,
	           typename T_P5, typename T_P6, typename T_P7, typename T_P8,
	           typename T_P9, typename T_P10, typename T_P11, typename T_P12,
	           typename T_P13, typename T_P14 >
		TStr& Mks( T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4, T_P5 p5, T_P6 p6, T_P7 p7, T_P8 p8,
		           T_P9 p9, T_P10 p10, T_P11 p11, T_P12 p12, T_P13 p13, T_P14 p14 )
		{	*this += p1;
			*this += p2;
			*this += p3;
			*this += p4;
			*this += p5;
			*this += p6;
			*this += p7;
			*this += p8;
			*this += p9;
			*this += p10;
			*this += p11;
			*this += p12;
			*this += p13;
			*this += p14;
			return *this;
		}

	template < typename T_P1, typename T_P2, typename T_P3, typename T_P4,
	           typename T_P5, typename T_P6, typename T_P7, typename T_P8,
	           typename T_P9, typename T_P10, typename T_P11, typename T_P12,
	           typename T_P13, typename T_P14, typename T_P15 >
		TStr& Mks( T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4, T_P5 p5, T_P6 p6, T_P7 p7, T_P8 p8,
		           T_P9 p9, T_P10 p10, T_P11 p11, T_P12 p12, T_P13 p13, T_P14 p14, T_P15 p15 )
		{	*this += p1;
			*this += p2;
			*this += p3;
			*this += p4;
			*this += p5;
			*this += p6;
			*this += p7;
			*this += p8;
			*this += p9;
			*this += p10;
			*this += p11;
			*this += p12;
			*this += p13;
			*this += p14;
			*this += p15;
			return *this;
		}

	template < typename T_P1, typename T_P2, typename T_P3, typename T_P4,
	           typename T_P5, typename T_P6, typename T_P7, typename T_P8,
	           typename T_P9, typename T_P10, typename T_P11, typename T_P12,
	           typename T_P13, typename T_P14, typename T_P15, typename T_P16 >
		TStr& Mks( T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4, T_P5 p5, T_P6 p6, T_P7 p7, T_P8 p8,
		           T_P9 p9, T_P10 p10, T_P11 p11, T_P12 p12, T_P13 p13, T_P14 p14, T_P15 p15, T_P16 p16 )
		{	*this += p1;
			*this += p2;
			*this += p3;
			*this += p4;
			*this += p5;
			*this += p6;
			*this += p7;
			*this += p8;
			*this += p9;
			*this += p10;
			*this += p11;
			*this += p12;
			*this += p13;
			*this += p14;
			*this += p15;
			*this += p16;
			return *this;
		}

	template < typename T_P1, typename T_P2, typename T_P3, typename T_P4,
	           typename T_P5, typename T_P6, typename T_P7, typename T_P8,
	           typename T_P9, typename T_P10, typename T_P11, typename T_P12,
	           typename T_P13, typename T_P14, typename T_P15, typename T_P16,
	           typename T_P17 >
		TStr& Mks( T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4, T_P5 p5, T_P6 p6, T_P7 p7, T_P8 p8,
		           T_P9 p9, T_P10 p10, T_P11 p11, T_P12 p12, T_P13 p13, T_P14 p14, T_P15 p15, T_P16 p16,
		           T_P17 p17 )
		{	*this += p1;
			*this += p2;
			*this += p3;
			*this += p4;
			*this += p5;
			*this += p6;
			*this += p7;
			*this += p8;
			*this += p9;
			*this += p10;
			*this += p11;
			*this += p12;
			*this += p13;
			*this += p14;
			*this += p15;
			*this += p16;
			*this += p17;
			return *this;
		}

	template < typename T_P1, typename T_P2, typename T_P3, typename T_P4,
	           typename T_P5, typename T_P6, typename T_P7, typename T_P8,
	           typename T_P9, typename T_P10, typename T_P11, typename T_P12,
	           typename T_P13, typename T_P14, typename T_P15, typename T_P16,
	           typename T_P17, typename T_P18 >
		TStr& Mks( T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4, T_P5 p5, T_P6 p6, T_P7 p7, T_P8 p8,
		           T_P9 p9, T_P10 p10, T_P11 p11, T_P12 p12, T_P13 p13, T_P14 p14, T_P15 p15, T_P16 p16,
		           T_P17 p17, T_P18 p18 )
		{	*this += p1;
			*this += p2;
			*this += p3;
			*this += p4;
			*this += p5;
			*this += p6;
			*this += p7;
			*this += p8;
			*this += p9;
			*this += p10;
			*this += p11;
			*this += p12;
			*this += p13;
			*this += p14;
			*this += p15;
			*this += p16;
			*this += p17;
			*this += p18;
			return *this;
		}

	template < typename T_P1, typename T_P2, typename T_P3, typename T_P4,
	           typename T_P5, typename T_P6, typename T_P7, typename T_P8,
	           typename T_P9, typename T_P10, typename T_P11, typename T_P12,
	           typename T_P13, typename T_P14, typename T_P15, typename T_P16,
	           typename T_P17, typename T_P18, typename T_P19 >
		TStr& Mks( T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4, T_P5 p5, T_P6 p6, T_P7 p7, T_P8 p8,
		           T_P9 p9, T_P10 p10, T_P11 p11, T_P12 p12, T_P13 p13, T_P14 p14, T_P15 p15, T_P16 p16,
		           T_P17 p17, T_P18 p18, T_P19 p19 )
		{	*this += p1;
			*this += p2;
			*this += p3;
			*this += p4;
			*this += p5;
			*this += p6;
			*this += p7;
			*this += p8;
			*this += p9;
			*this += p10;
			*this += p11;
			*this += p12;
			*this += p13;
			*this += p14;
			*this += p15;
			*this += p16;
			*this += p17;
			*this += p18;
			*this += p19;
			return *this;
		}

	template < typename T_P1, typename T_P2, typename T_P3, typename T_P4,
	           typename T_P5, typename T_P6, typename T_P7, typename T_P8,
	           typename T_P9, typename T_P10, typename T_P11, typename T_P12,
	           typename T_P13, typename T_P14, typename T_P15, typename T_P16,
	           typename T_P17, typename T_P18, typename T_P19, typename T_P20 >
		TStr& Mks( T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4, T_P5 p5, T_P6 p6, T_P7 p7, T_P8 p8,
		           T_P9 p9, T_P10 p10, T_P11 p11, T_P12 p12, T_P13 p13, T_P14 p14, T_P15 p15, T_P16 p16,
		           T_P17 p17, T_P18 p18, T_P19 p19, T_P20 p20 )
		{	*this += p1;
			*this += p2;
			*this += p3;
			*this += p4;
			*this += p5;
			*this += p6;
			*this += p7;
			*this += p8;
			*this += p9;
			*this += p10;
			*this += p11;
			*this += p12;
			*this += p13;
			*this += p14;
			*this += p15;
			*this += p16;
			*this += p17;
			*this += p18;
			*this += p19;
			*this += p20;
			return *this;
		}

	template < typename T_P1, typename T_P2, typename T_P3, typename T_P4,
	           typename T_P5, typename T_P6, typename T_P7, typename T_P8,
	           typename T_P9, typename T_P10, typename T_P11, typename T_P12,
	           typename T_P13, typename T_P14, typename T_P15, typename T_P16,
	           typename T_P17, typename T_P18, typename T_P19, typename T_P20,
	           typename T_P21 >
		TStr& Mks( T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4, T_P5 p5, T_P6 p6, T_P7 p7, T_P8 p8,
		           T_P9 p9, T_P10 p10, T_P11 p11, T_P12 p12, T_P13 p13, T_P14 p14, T_P15 p15, T_P16 p16,
		           T_P17 p17, T_P18 p18, T_P19 p19, T_P20 p20, T_P21 p21 )
		{	*this += p1;
			*this += p2;
			*this += p3;
			*this += p4;
			*this += p5;
			*this += p6;
			*this += p7;
			*this += p8;
			*this += p9;
			*this += p10;
			*this += p11;
			*this += p12;
			*this += p13;
			*this += p14;
			*this += p15;
			*this += p16;
			*this += p17;
			*this += p18;
			*this += p19;
			*this += p20;
			*this += p21;
			return *this;
		}

	template < typename T_P1, typename T_P2, typename T_P3, typename T_P4,
	           typename T_P5, typename T_P6, typename T_P7, typename T_P8,
	           typename T_P9, typename T_P10, typename T_P11, typename T_P12,
	           typename T_P13, typename T_P14, typename T_P15, typename T_P16,
	           typename T_P17, typename T_P18, typename T_P19, typename T_P20,
	           typename T_P21, typename T_P22 >
		TStr& Mks( T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4, T_P5 p5, T_P6 p6, T_P7 p7, T_P8 p8,
		           T_P9 p9, T_P10 p10, T_P11 p11, T_P12 p12, T_P13 p13, T_P14 p14, T_P15 p15, T_P16 p16,
		           T_P17 p17, T_P18 p18, T_P19 p19, T_P20 p20, T_P21 p21, T_P22 p22 )
		{	*this += p1;
			*this += p2;
			*this += p3;
			*this += p4;
			*this += p5;
			*this += p6;
			*this += p7;
			*this += p8;
			*this += p9;
			*this += p10;
			*this += p11;
			*this += p12;
			*this += p13;
			*this += p14;
			*this += p15;
			*this += p16;
			*this += p17;
			*this += p18;
			*this += p19;
			*this += p20;
			*this += p21;
			*this += p22;
			return *this;
		}

	template < typename T_P1, typename T_P2, typename T_P3, typename T_P4,
	           typename T_P5, typename T_P6, typename T_P7, typename T_P8,
	           typename T_P9, typename T_P10, typename T_P11, typename T_P12,
	           typename T_P13, typename T_P14, typename T_P15, typename T_P16,
	           typename T_P17, typename T_P18, typename T_P19, typename T_P20,
	           typename T_P21, typename T_P22, typename T_P23 >
		TStr& Mks( T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4, T_P5 p5, T_P6 p6, T_P7 p7, T_P8 p8,
		           T_P9 p9, T_P10 p10, T_P11 p11, T_P12 p12, T_P13 p13, T_P14 p14, T_P15 p15, T_P16 p16,
		           T_P17 p17, T_P18 p18, T_P19 p19, T_P20 p20, T_P21 p21, T_P22 p22, T_P23 p23 )
		{	*this += p1;
			*this += p2;
			*this += p3;
			*this += p4;
			*this += p5;
			*this += p6;
			*this += p7;
			*this += p8;
			*this += p9;
			*this += p10;
			*this += p11;
			*this += p12;
			*this += p13;
			*this += p14;
			*this += p15;
			*this += p16;
			*this += p17;
			*this += p18;
			*this += p19;
			*this += p20;
			*this += p21;
			*this += p22;
			*this += p23;
			return *this;
		}

	template < typename T_P1, typename T_P2, typename T_P3, typename T_P4,
	           typename T_P5, typename T_P6, typename T_P7, typename T_P8,
	           typename T_P9, typename T_P10, typename T_P11, typename T_P12,
	           typename T_P13, typename T_P14, typename T_P15, typename T_P16,
	           typename T_P17, typename T_P18, typename T_P19, typename T_P20,
	           typename T_P21, typename T_P22, typename T_P23, typename T_P24 >
		TStr& Mks( T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4, T_P5 p5, T_P6 p6, T_P7 p7, T_P8 p8,
		           T_P9 p9, T_P10 p10, T_P11 p11, T_P12 p12, T_P13 p13, T_P14 p14, T_P15 p15, T_P16 p16,
		           T_P17 p17, T_P18 p18, T_P19 p19, T_P20 p20, T_P21 p21, T_P22 p22, T_P23 p23, T_P24 p24 )
		{	*this += p1;
			*this += p2;
			*this += p3;
			*this += p4;
			*this += p5;
			*this += p6;
			*this += p7;
			*this += p8;
			*this += p9;
			*this += p10;
			*this += p11;
			*this += p12;
			*this += p13;
			*this += p14;
			*this += p15;
			*this += p16;
			*this += p17;
			*this += p18;
			*this += p19;
			*this += p20;
			*this += p21;
			*this += p22;
			*this += p23;
			*this += p24;
			return *this;
		}

	// +++ sprintf functionality, to eventually be type safe
	TStr& vFmt( oexCONST T *x_pFmt, oexVaList x_pArgs )
	{	return vPrint( x_pFmt, x_pArgs );
	}

	// +++ sprintf functionality, to eventually be type safe
	TStr& Fmt( oexCONST T *pFmt, ... )
	{
		oexVaList ap;
		oexVaStart( ap, pFmt );
		vPrint( pFmt, ap );
		oexVaEnd( ap );
		return *this;
	}

/*
// +++ Sometime, it'd be great to have a type safe sprintf,
//     When I can find the time ;)

	TStr& Fmt( oexCONST T *pFmt )
	{
		if ( !oexCHECK_PTR( pFmt ) )
			Empty();
		else
			*this = pFmt;

		return *this;
	}

	oexCONST T* FindEndFmt( oexCONST T *pFmt, oexINT *pSize )
	{
		if ( _TT( T, '%' ) != *pFmt )
			return pFmt;

		// Skip format parameters
		while ( ( _TT( T, '0' ) <= *pFmt && _TT( T, '9' ) >= *pFmt )
		        || _TT( T, '+' ) == *pFmt
		        || _TT( T, '-' ) == *pFmt
		        || _TT( T, '.' ) == *pFmt
		        || _TT( T, '#' ) == *pFmt
		        || _TT( T, '*' ) == *pFmt )
			pFmt++;

		// Single character
		switch( *pFmt )
		{
			case _TT( T, 'c' ) :
				if ( pType )
					pType = eTypeChar;
				break;

			case _TT( T, 'd' ) :
			case _TT( T, 'i' ) :
				if ( pType )
					pType = eTypeInt8;
				break;


		} // end switch

	}

	template < typename T_P1 >
		oexCONST T* FmtAppend( oexCONST T *pFmt, T_P1 p1 )
		{
			if ( !oexCHECK_PTR( pFmt ) )
				return oexFALSE;

			// Find an escape sequence
			oexCONST T *pIns = pFmt;
			do
			{
				// Check for literal '%'
				if ( _TT( T, '%' ) == *pFmt && _TT( T, '%' ) == *( pFmt + 1 ) )
				{	Append( pFmt, ++pIns - pFmt );
					pFmt = ++pIns;
				} // end if

			} while ( *pIns && _TT( T, '%' ) != *pFmt );

			// Did we find an escape sequence?
			if ( !*pIns )
			{	Append( pFmt ); return pIns; }

			// Append the string up to the escape sequence if needed
			if ( pIns > pFmt )
				Append( pFmt, pIns - pFmt );

			oexINT nSize = 0;
			pFmt = FindEndFmt( pIns, &nSize );

			// Do it if the sizes match
			if ( obj::Size( &p1 ) == nSize )
				AppendPrint( TStr( pIns, pFmt - pIns ).Ptr(), p1 );

			// Just show the escape sequence
			else
				Append( pIns, pFmt - pIns );

			// Return next character after the escape sequence
			return pFmt;
		}
*/

	/// Sets a number into the string using rules of Fmt()
	/// Truncates results larger than 256 characters
	TStr& SetNum( oexCONST T *pFmt, ... )
	{
		T tBuf[ 256 ];

		oexVaList ap; oexVaStart( ap, pFmt );
		os::CSys::vStrFmt( tBuf, oexSizeOfArray( tBuf ), pFmt, ap );
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
		os::CSys::vStrFmt( tBuf, oexSizeOfArray( tBuf ), pFmt, ap );
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

	/// Converts the string to a GUID, returns oexNULL if this string does not contain a GUID
	oexGUID& StringToGuid( oexGUID &x_rGuid )
    {	guid::StringToGuid( &x_rGuid, Ptr(), Length() ); return x_rGuid; }

	/// Converts a GUID to a string, if pGuid is NULL, a unique GUID is created
	TStr& GuidToString( oexCONST oexGUID *x_pGuid = oexNULL )
	{	guid::GuidToString( OexAllocate( 36 /* + 1 is implicit*/ ), 37, x_pGuid );
		oexIGNORE oexVERIFY( Length() == 36 );
		return *this;
	}

	/// Converts a GUID to a string
	TStr& GuidToString( oexCONST oexGUID &x_rGuid )
	{	guid::GuidToString( OexAllocate( 36 /* + 1 is implicit*/ ), 37, &x_rGuid );
		oexIGNORE oexVERIFY( Length() == 36 );
		return *this;
	}

public:

    /// Converts to a number
    oexINT64 ToNum( t_size x_nMax = 0, oexUINT x_uRadix = 10, t_size *x_pnEnd = oexNULL, oexBOOL x_bTrim = oexFALSE )
    {   if ( !x_nMax || x_nMax > Length() ) x_nMax = Length();
        t_size nEnd = 0; oexINT64 llNum = str::StrToNum( Ptr(), x_nMax, x_uRadix, &nEnd );
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

	oexBOOL Cmp( oexCONST T* pStr, t_size uSize )
	{
        if ( (t_size)uSize != Length() )
            return oexFALSE;
		if ( !uSize )
            return oexTRUE;
		return !os::CSys::MemCmp( m_mem.Ptr(), pStr, uSize );
	}

	oexBOOL ICmp( TStr &str ) const
	{	return ICmp( str.Ptr(), str.Length() ); }

	oexBOOL ICmp( oexCONST T* pStr ) const
	{	return ICmp( pStr, zstr::Length( pStr ) ); }

	oexBOOL ICmp( oexCONST T* pStr, t_size uSize ) const
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

	oexBOOL CmpLen( oexCONST T* pStr, t_size uSize, oexINT x_lLen = -1 )
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

	oexBOOL ICmpLen( oexCONST T* pStr, t_size uSize, oexINT x_lLen = -1 )
    {   if ( 0 > x_lLen ) x_lLen = uSize;
        return !str::ICompareLen( Ptr(), Length(), pStr, uSize, x_lLen );
    }

	/// Returns the offset of pSub in the string, or
    /// -1 if a pStr is not found.
	oexINT Match( oexCONST T* pSub )
    {   return str::FindSubStr( Ptr(), Length(), pSub, zstr::Length( pSub ) ); }

	/// Returns the offset of pSub in the string, or
    /// -1 if a pStr is not found.
	oexINT IMatch( oexCONST T* pSub )
    {   return str::IFindSubStr( Ptr(), Length(), pSub, zstr::Length( pSub ) ); }

	/// Appends a size formatted string ( 1.3KB, 44.75GB, etc...)
	TStr& AppendSizeString( oexDOUBLE dSize, oexDOUBLE dDiv, oexINT nDigits, oexCONST T ** pSuffix = oexNULL )
	{	
		oexINT i = 0;
		static oexCONST T *sizes[] = 
		{	oexTT( T, "" ), 			//
			oexTT( T, "K" ), 			// Kilo				
			oexTT( T, "M" ), 			// Mega			
			oexTT( T, "G" ), 			// Giga
			oexTT( T, "T" ), 			// Tera
			oexTT( T, "P" ),			// Peta
			oexTT( T, "E" ),			// Exa
			oexTT( T, "Z" ),			// Zetta
			oexTT( T, "Y" ),			// Yotta
			oexTT( T, "B" ),			// Bronto
			0 							// Geop, but G already taken?
										// Segan, ...
		};

		// Use 1024 as the default divider
		if ( 0 >= dDiv )
			dDiv = double( 1024 );
		
		oexBOOL bNeg = 0 > dSize;
		if ( bNeg ) 
			dSize = -dSize;
		
		// Use default suffixes if non provided
		if ( !pSuffix || !*pSuffix || !**pSuffix )
			pSuffix = sizes;
		
		// Which size to use?
		while ( dSize > dDiv && pSuffix[ i + 1 ] )
			i++, dSize /= dDiv;

		// Is the number negative?
		if ( bNeg )
			Append( oexTT( T, "-" ) );
			
		// Special formating?
		if ( 0 > nDigits )
			Append( dSize );
		else if ( !nDigits )
			Append( (oexLONG)dSize );
		else
			AppendNum( ( TStr( oexTT( T, "%." ) ) << nDigits << oexT( "f" ) ).Ptr(), dSize );
			
		// Build the string
		Append( pSuffix[ i ] );

		return *this;
	}
	
	
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
			if ( m )
				str += SubStr( s, m );

			// Replace with other string if needed
			if ( lr )
				str += x_sReplace;

			// Next point
			s += m + lf;

		} while ( s < e );

		return str;
	}

	/// Replaces single characters in a string
	TStr& IReplace( oexCONST T x_tFind, oexCONST T x_tReplace )
    {   str::IReplace( _Ptr(), Length(), x_tFind, x_tReplace ); return *this; }

	/// Replaces sub strings with another
	TStr IReplace( oexCONST TStr &x_sFind, oexCONST TStr &x_sReplace ) oexCONST
	{
		TStr str;
		oexINT lf = x_sFind.Length();
		if ( !lf )
			return *this;

		oexINT s = 0, e = Length(), lr = x_sReplace.Length(), m;
		do
		{
			// See if we can find the string
			m = str::IFindSubStr( Ptr( s ), e - s, x_sFind.Ptr(), lf );

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
			if ( m )
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
	{	t_size uLen = Length();
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

	/// Escapes the specified characters
	TStr Escape( oexCONST T* pChars, oexCONST T cEscape )
	{
		if ( !pChars )
			return TStr();

		TStr s;
		oexSIZE_T lc = zstr::Length( pChars ), ls = Length();
		oexCONST T* pSrc = Ptr();

		// Can't be larger than twice the length
		T* pDst = s.OexAllocate( ls * 2 );

		// While we have a string
		while ( ls )
		{
			// Find escape character
			oexINT p = str::FindCharacters( pSrc, ls, pChars, lc );

			// Did we find an escape character
			if ( 0 <= p )
			{
				// Copy raw block
				if ( 0 < p )
					oexMemCpy( pDst, pSrc, sizeof( T ) * p ),
					pDst += p, pSrc += p, ls -= p;

				// Escape the character
				*pDst = cEscape, pDst++, *pDst = *pSrc, pDst++, pSrc++, ls--;

			} // end if

			// Are we done?
			else if ( ls )
				oexMemCpy( pDst, pSrc, ls ),
				pDst += ls, ls = 0;

		} // end while

		// Set length of escaped string
		s.SetLength( pDst - s.Ptr() );

		return s;
	}

	/// Escapes the specified characters
	TStr EscapeRange( oexCONST T tMin, oexCONST T tMax, oexBOOL bInclusive, oexCONST T cEscape )
	{
		/// +++ Please implement
		oexVERIFY( oexFALSE != bInclusive );

		TStr s;
		oexSIZE_T ls = Length();
		oexCONST T* pSrc = Ptr();

		// Can't be larger than twice the length
		T* pDst = s.OexAllocate( ls * 2 );

		// While we have a string
		while ( ls )
		{
			// Find escape character
			//oexINT p = str::FindCharacters( pSrc, ls, pChars, lc );
	        oexINT p = str::FindInRange( pSrc, ls, tMin, tMax );

			// Did we find an escape character
			if ( 0 <= p )
			{
				// Copy raw block
				if ( 0 < p )
					oexMemCpy( pDst, pSrc, sizeof( T ) * p ),
					pDst += p, pSrc += p, ls -= p;

				// Escape the character
				*pDst = cEscape, pDst++, *pDst = *pSrc, pDst++, pSrc++, ls--;

			} // end if

			// Are we done?
			else if ( ls )
				oexMemCpy( pDst, pSrc, ls ),
				pDst += ls, ls = 0;

		} // end while

		// Set length of escaped string
		s.SetLength( pDst - s.Ptr() );

		return s;
	}

	/// Escapes the specified characters
	TStr EscapeWs( oexCONST T cEscape )
	{
		TStr s;
		oexSIZE_T ls = Length();
		oexCONST T* pSrc = Ptr();

		// Can't be larger than twice the length
		T* pDst = s.OexAllocate( ls * 2 );

		// While we have a string
		while ( ls )
		{
			switch( *pSrc )
			{
				case oexTC( T, '\r' ) :
					*pDst = cEscape, pDst++;
					*pDst = oexTC( T, 'r' ), pDst++;
					break;

				case oexTC( T, '\n' ) :
					*pDst = cEscape, pDst++;
					*pDst = oexTC( T, 'n' ), pDst++;
					break;

				case oexTC( T, '\t' ) :
					*pDst = cEscape, pDst++;
					*pDst = oexTC( T, 't' ), pDst++;
					break;

				default :
					*pDst = *pSrc, pDst++;
					break;

			} // end switch

			pSrc++, ls--;

		} // end while

		// Set length of escaped string
		s.SetLength( pDst - s.Ptr() );

		return s;
	}

	/// Unescapes the specified characters
	TStr UnescapeWs( oexCONST T cEscape )
	{
		TStr s;
		oexSIZE_T ls = Length();
		oexCONST T* pSrc = Ptr();

		// Can't be larger than the length
		T* pDst = s.OexAllocate( ls );

		// While we have a string
		while ( ls )
		{
			if ( cEscape != *pSrc )
				*pDst = *pSrc, pDst++;

			else
			{
				pSrc++, ls--;

				switch( *pSrc )
				{
					case oexTC( T, 'r' ) :
						*pDst = oexTC( T, '\r' ), pDst++;
						break;

					case oexTC( T, 'n' ) :
						*pDst = oexTC( T, '\n' ), pDst++;
						break;

					case oexTC( T, 't' ) :
						*pDst = oexTC( T, '\t' ), pDst++;
						break;

					default :
						*pDst = *pSrc, pDst++;
						break;

				} // end switch

			} // end else

			pSrc++, ls--;

		} // end while

		// Set length of escaped string
		s.SetLength( pDst - s.Ptr() );

		return s;
	}

	/// Returns the offset of the first character in pChars found in the string
	/// Returns -1 if no characters found
	oexINT FindChars( oexCONST T* pChars )
	{	return str::FindCharacters( Ptr(), Length(), pChars, zstr::Length( pChars ) );
	}

	/// Returns the offset of the first character in pChars found in the string
	/// Returns -1 if no characters found
	oexINT FindChars( oexCONST T* pChars, t_size lnChars )
	{	return str::FindCharacters( Ptr(), Length(), pChars, lnChars );
	}

	/// Returns the offset of the first character in pChars found in the string
	/// Returns -1 if no characters found
	oexINT RFindChars( oexCONST T* pChars )
	{	return str::RFindCharacters( Ptr(), Length(), pChars, zstr::Length( pChars ) );
	}

	/// Skips any characters not in pChars and removes
	/// them from the string
	TStr& Find( oexCONST T* pChars )
	{	oexINT p = str::FindCharacters( Ptr(), Length(), pChars, zstr::Length( pChars ) );
		if ( 0 < p ) LTrim( p ); else if ( 0 > p ) Destroy();
		return *this;
	}

	/// Skips any characters not in pChars and removes
	/// them from the string
	TStr& Find( oexCONST T* pChars, t_size lnChars )
	{	oexINT p = str::FindCharacters( Ptr(), Length(), pChars, lnChars );
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
    {	oexIGNORE oexVERIFY_PTR_NULL_OK( pEscape );
    	if ( !Length() ) return TStr();
        oexINT i = str::ParseQuoted(    Ptr(), Length(),
                                        pOpen, zstr::Length( pOpen ),
                                        pClose, zstr::Length( pOpen ),
                                        pEscape, pEscape ? zstr::Length( pEscape ) : 0 );
		if ( 0 > i )
			return TStr();
		TStr str;
		if ( 1 < i )
			str = SubStr( 1, i - 1 );
		LTrim( i + 1 );
		return str;
	}

	/// Splits off a token and returns it
	TStr& Unquote( oexCONST T *pOpen, oexCONST T *pClose, oexCONST T *pEscape = oexNULL )
    {	oexIGNORE oexVERIFY_PTR_NULL_OK( pEscape );
    	if ( !Length() ) return *this;
        oexINT i = str::ParseQuoted(    Ptr(), Length(),
                                        pOpen, zstr::Length( pOpen ),
                                        pClose, zstr::Length( pOpen ),
                                        pEscape, pEscape ? zstr::Length( pEscape ) : 0 );
		if ( 0 > i )
			return *this;

		if ( 1 < i )
			Sub( 1, i - 1 );
		else
			Destroy();

		return *this;
	}
	
	/// Returns quoted string
	TStr Quote( oexCONST T *pOpen, oexCONST T *pClose, oexCONST T pEscape )
	{
		TStr s;
		T find[ 2 ] = { 0, 0 };
		T escp[ 3 ] = { 0, pEscape, 0 };
		
		// Escape escape chars
		find[ 0 ] = pEscape, escp[ 0 ] = pEscape, s = Replace( find, escp );
		
		// Escape open chars
		if ( pOpen )
			for ( oexINT i = 0; pOpen[ i ]; i++ )
				find[ 0 ] = pOpen[ i ], escp[ 0 ] = pOpen[ i ], s = Replace( find, escp );
		
		// Escape close chars
		if ( pClose )
			for ( oexINT i = 0; pClose[ i ]; i++ )
				find[ 0 ] = pClose[ i ], escp[ 0 ] = pClose[ i ], s = Replace( find, escp );

		find[ 0 ] = pEscape;
		return ( TStr() << find << s << find );
	}

	/// Splits off a token and returns it
	TStr ParseWithQuoted( oexCONST T *pOpen, oexCONST T *pTerm, oexCONST T *pClose, oexCONST T *pEscape = oexNULL )
    {	oexIGNORE oexVERIFY_PTR_NULL_OK( pEscape );
        oexINT i = str::ParseQuoted(    Ptr(), Length(),
                                        pTerm, zstr::Length( pTerm ),
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
	{	oexASSERT_PTR( pValid );
		oexINT i = 0;
		TStr str = Token( pValid, &i );
		if ( i ) LTrim( i );
		return str;
	}

	/// Splits off a token and returns it
	TStr ParseNextToken( oexCONST T* pValid )
	{	oexASSERT_PTR( pValid );
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

	/// Splits off a string up to and including any of the terminator characters,
	TStr& SkipPast( oexCONST T* pTerm )
	{	oexASSERT_PTR( pTerm );
		oexUINT nTerm = zstr::Length( pTerm );
		oexINT i = str::FindCharacters( Ptr(), Length(), pTerm, nTerm );
		if ( 0 > i ) { Destroy(); return *this; }
		oexINT nEnd = str::SkipCharacters( Ptr( i ), Length() - i, pTerm, nTerm );
		if ( 0 > nEnd ) { Destroy(); return *this; }
		LTrim( i + nEnd );
		return *this;
	}

	/// Splits off a string up to any of the terminator characters
	TStr RParse( oexCONST T* pTerm )
	{	oexASSERT_PTR( pTerm );
        oexINT lLen = Length();
        oexINT i = str::RFindTerm( Ptr(), lLen, pTerm, zstr::Length( pTerm ) );
		if ( 0 > i )
			return TStr();
		TStr str( Ptr( i ), lLen - i );
		RTrim( lLen - i );
		return str;
	}

	oexBOOL IsMatchAt( t_size i, oexCONST T* pChars )
	{	oexASSERT_PTR( pChars );
        return 0 <= str::FindCharacter( pChars, zstr::Length( pChars ), *Ptr( i ) );
	}

    oexBOOL IsInRangeAt( t_size i, T tMin, T tMax )
	{   T ch = *Ptr( i );
        return tMin <= ch && tMax >= ch;
	}

    oexBOOL IsWhiteSpaceAt( t_size i )
    {   return !IsInRangeAt( i, oexT( '!' ), oexT( '~' ) ); }

	/// Finds the first token in a string and returns it
	TStr NextToken( T tMin, T tMax, oexINT *puI = oexNULL )
	{	return NextToken( Ptr(), Length(), tMin, tMax, puI ); }

	static TStr NextToken( oexCONST T* pStr, T tMin, T tMax, oexINT *puI = oexNULL )
	{	return NextToken( pStr, zstr::Length( pStr ), tMin, tMax, puI ); }

	static TStr NextToken( TStr str, T tMin, T tMax, oexINT *puI = oexNULL )
	{	return NextToken( str.Ptr(), str.Length(), tMin, tMax, puI ); }

	static TStr NextToken( oexCONST T* pStr, t_size uSize, T tMin, T tMax, oexINT *puI  = oexNULL )
	{
		// Sanity check
		if ( !oexVERIFY_PTR( pStr ) )
			return TStr();

		t_size i = 0;

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
			nEnd = uSize;

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

	static TStr NextToken( oexCONST T* pStr, t_size uSize, oexCONST T *pValid, oexINT *puI  = oexNULL )
	{
		// Sanity check
		if ( !oexCHECK_PTR( pStr ) || !oexCHECK_PTR_NULL_OK( pValid ) )
			return TStr();

		if ( !pValid )
			return TStr( pStr, uSize );

		t_size i = 0;
		t_size uValid = zstr::Length( pValid );

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
			nEnd = uSize;

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
	static TStr Token( oexCONST T* pStr, t_size uSize, oexCONST T *pValid, oexINT *puI  = oexNULL )
	{
		// Sanity check
		if ( !oexCHECK_PTR( pStr ) || !oexCHECK_PTR_NULL_OK( pValid ) )
			return TStr();

		if ( !pValid )
			return TStr( pStr, uSize );

		t_size i = 0;
		t_size uValid = zstr::Length( pValid );

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
			nEnd = uSize;

		// Add offset
		if ( puI )
			*puI += nEnd;

		return TStr( pStr, 0, nEnd );
	}

public:

    /// Concatinates two strings into a path
    static TStr BuildPath( TStr x_sRoot, TStr x_sPath, T tSep = oexTCPathSep( T ) )
    {	if ( !x_sRoot.Length() ) return x_sPath;
		if ( !x_sPath.Length() ) return x_sRoot;
		return x_sRoot.RTrim( oexTT( T, "\\/" ) ) << tSep << x_sPath.LTrim( oexTT( T, "\\/" ) );
	}

    /// Concatinates two strings into a path
    TStr& BuildPath( TStr x_sPath, T tSep = oexTCPathSep( T ) )
    {   
		if ( !x_sPath.Length() ) 
			return *this;
			
		if ( !Length() ) 
		{	*this = x_sPath;
			return *this;
		} // end if

		// Build the path
		RTrim( oexTT( T, "\\/" ) );
        *this << tSep << x_sPath.LTrim( oexTT( T, "\\/" ) );
        RTrim( oexTT( T, "\\/" ) );
		
        return *this;
    }

	// +++ Make this more efficient by removing the double search for separators
    /// Returns the root path of str
    TStr GetPath() oexCONST
    {   TStr str( *this );
		oexINT nOffset = str.RFindChars( oexTT( T, "\\/" ) );
		if ( 0 > nOffset )
			return TStr();
        str.RParse( oexTT( T, "\\/" ) );
        return str.RTrim( oexTT( T, "\\/" ) );
    }

    /// Returns the root path of str
    TStr GetFileName()
    {   TStr str( *this );
		oexINT nOffset = str.RFindChars( oexTT( T, "\\/" ) );
		if ( 0 > nOffset )
			return *this;
		return str.LTrim( nOffset + 1 );
	}

    /// Returns the file extension of the file path
    TStr GetFileExtension()
    {	TStr sFilename = GetFileName();
		oexINT nOffset = sFilename.RFindChars( oexTT( T, "." ) );
		if ( 0 > nOffset )
			return TStr();
		return sFilename.LTrim( nOffset + 1 );
    }

    /// Returns the file without the extension
    TStr RemoveFileExtension()
    {	TStr sFilename = GetFileName();
		oexINT nOffset = sFilename.RFindChars( oexTT( T, "." ) );
		if ( 0 > nOffset )
			return *this;
		TStr sPath = GetPath();
		if ( !sPath.Length() )
			return sFilename.RTrim( sFilename.Length() - nOffset );
		return TStr().BuildPath( sPath, sFilename.RTrim( sFilename.Length() - nOffset ) );
    }

	TStr& DecorateName( oexBOOL bExe, oexBOOL bLib )
	{
		return DecorateName( *this, bExe, bLib );
	}

	TStr& DecorateName( TStr &sFile, oexBOOL bExe, oexBOOL bLib )
	{
#if defined( OEX_WINDOWS )
#	if defined( oexDEBUG )
		return DecorateName( sFile, bExe, bLib, oexTRUE, oexTRUE );
#	else
		return DecorateName( sFile, bExe, bLib, oexFALSE, oexTRUE );
#	endif
#else
#	if defined( oexDEBUG )
		return DecorateName( sFile, bExe, bLib, oexTRUE, oexFALSE );
#	else
		return DecorateName( sFile, bExe, bLib, oexFALSE, oexFALSE );
#	endif
#endif

	}

	TStr& DecorateName( TStr sFile, oexBOOL bExe, oexBOOL bLib, oexBOOL bDebug, oexBOOL bWindows )
	{
		Destroy();

		if ( bDebug )
			sFile << oexTT( T, "_d" );

		if ( bWindows )
		{
			if ( bExe && !bLib )
				*this << sFile << oexTT( T, ".exe" );
			else if ( bExe && bLib )
				*this << sFile << oexTT( T, ".dll" );
			else
				*this << sFile << oexTT( T, ".lib" );

		} // end if

		else
		{
			if ( bExe && !bLib )
				*this << sFile << oexTT( T, "" );
			else if ( bExe && bLib )
				*this << oexTT( T, "lib" ) << sFile << oexTT( T, ".so" );
			else
				*this << oexTT( T, "lib" ) << sFile << oexTT( T, ".a" );

		} // end if

		return *this;
	}

	// Match against file pattern
	oexBOOL MatchPattern( oexCONST TStr x_sPattern, oexBOOL x_bIgnoreCase )
	{	return 0 <= str::MatchPattern( Ptr(), Length(), x_sPattern.Ptr(), x_sPattern.Length(), x_bIgnoreCase );
	}

private:

    /// The string memory
    t_buffer       m_mem;

    /// The length of the string
    t_size       m_nLength;

    /// Offset into the string, this is invaluable for text parsing
    t_size       m_nOffset;

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
	template <> oexITS oexSIZE_T Size< CStrW >( oexCONST CStrW *x_obj )
    {   return ( (CStrW*)x_obj )->LengthInBytes(); }

	template <> oexITS oexSIZE_T Size< CStr8 >( oexCONST CStr8 *x_obj )
    {   return ( (CStr8*)x_obj )->LengthInBytes(); }

	template <> oexITS oexSIZE_T Size< CStr16 >( oexCONST CStr16 *x_obj )
    {   return ( (CStr16*)x_obj )->LengthInBytes(); }

//	template <> oexITS oexSIZE_T Size< CStr32 >( oexCONST CStr32 *x_obj )
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
