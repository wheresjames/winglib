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
        m_lLength = 0;
    }

    /// Destructor
    ~TStr()
    {   Destroy();
    }

    /// Releases the string
    void Destroy()
    {
        // Zer length
        m_lLength = 0;

        // Lose the string memory
        m_mem.Delete();
    }

public:

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
//		oexTRACE( "TStr::Allocate( %lu )\n", uSize );

		// Break any existing share
		Unshare();

        T* pMem;

        // Do we already have the memory
        if ( m_mem.Size() == ( x_uSize + 1 ) )
            pMem = m_mem.Ptr();

        else
        {
            pMem = m_mem.Resize( x_uSize + 1 ).Ptr();
		    if ( !oexVERIFY_PTR( pMem ) )
			    return oexNULL;

        } // end if

		// Null terminate our new buffer
		pMem[ x_uSize ] = 0;

		// User can't possibly copy any data
		if ( !x_uSize ) m_lLength = 0;

		// We'll scan for the length of the buffer later
		else m_lLength = -1;

		return pMem;
    }

	/// Returns a const pointer to the internal string buffer
	oexCONST T* Ptr()
	{	
		// Ensure string
		if ( !m_mem.Size() )
			Allocate( 0 );

		return m_mem.Ptr();
	}

	/// Returns a const pointer to the internal string buffer
    /**
        !!! Possibly strange behavior, this function evaluates
            the length of the string if lOffset is not zero, so
            the open buffer you get form Allocate() will be
            canceled if you call this function with a non-zero
            offset.  I decided not to have it evaluate the string
            length when lOffset is zero to avoid needing an external 
            pointer when using open buffers and Allocate().
    */
	oexCONST T* Ptr( oexLONG x_lOffset )
    {	
		// Ensure string
		if ( !m_mem.Size() )
			Allocate( 0 );

        if ( x_lOffset )
        {
		    // Check for offset beyond the end of the buffer
		    oexASSERT( Length() >= x_lOffset );

    		// Don't point beyond the end of the buffer
	    	if ( x_lOffset > Length() ) 
                x_lOffset = m_lLength;

        } // end if

		return m_mem.Ptr( x_lOffset );
	}

	/// Returns a writable pointer (use with care)
	T* _Ptr( oexLONG x_lOffset = 0 )
	{	return (T*)Ptr( x_lOffset ); }

	/// Returns the specified character
	T& operator []( oexLONG x_lOffset )
	{	return *_Ptr( x_lOffset ); }

	/// Returns the first character in the buffer
	oexCONST T& operator *() 
    { return *Ptr(); }

	/// Calculates the length of the string by finding the NULL terminator
	oexLONG CalculateLength()
	{
		m_lLength = 0;
		oexLONG lMax = m_mem.Size();

		// Do we have a buffer?
		if ( 0 >= lMax )
			return m_lLength;

		/// Get a writable pointer to the string
		T* pStr = _Ptr();

		// NULL terminate the end of memory so we don't over-run the buffer
		pStr[ lMax - 1 ] = 0;

        // Get the length
        m_lLength = str::Length( pStr, lMax );

		// Resize the memory block ( this adds over-run protection )
		m_mem.Resize( m_lLength + 1 );

		// Return the length of the buffer
		return m_lLength;
	}

	/// Returns the length of the string, calculates if needed.
	oexLONG Length() 
	{	if ( 0 > m_lLength ) 
			return CalculateLength();
		return m_lLength; 
	}

    /// Manually sets the length
    /// !!! This allows NULL characters to be in the string.
    oexLONG SetLength( oexLONG x_lLength )
    {
        // Ensure we have that much data
        if ( !Allocate( x_lLength ) )
            return 0;

        // Just accept the callers size
        m_lLength = x_lLength;

        return m_lLength;
    }

public:

	TStr& Set( TStr &x_str )
	{
		if ( x_str.Length() )
		{	m_lLength = x_str.m_lLength;
			m_mem = x_str.m_mem;
		} // end if
		else 
			Destroy();

		return *this;
	}

	TStr& Set( oexCONST T* x_pStr )
	{	return Set( x_pStr, zstr::Length( x_pStr ) ); }

	TStr& Set( oexCONST T* x_pStr, oexUINT uSize )
	{
		// Verify pointer
		if ( !oexVERIFY_PTR( x_pStr ) )
			return *this;

		// Allocate space for new string
		if ( !oexVERIFY_PTR( Allocate( uSize ) ) || !uSize )
			return *this;
		
		// Copy the string data
		m_mem.MemCpy( x_pStr, uSize );

		// Save the new string length
		m_lLength = uSize;

		return *this;
	}

	TStr& Set( oexCONST T x_chVal )
	{
		if ( !x_chVal )
		{	Allocate( 0 ); return *this; }

		// Allocate space for new string
		T* pPtr = Allocate( 1 );
		if ( !oexVERIFY( pPtr ) )
			return *this;

		pPtr[ 0 ] = x_chVal;

		m_lLength = 1;

		return *this;
	}

	/// Assignment operator
	TStr& operator = ( oexCONST T* pStr )
	{	return Set( pStr ); }


	// Concatenation operator
	TStr& Append( TStr &sStr )
	{	return Append( sStr.Ptr(), sStr.Length() ); }

	// Concatenation operator
	TStr& Append( oexCONST T* pStr )
	{	return Append( pStr, zstr::Length( pStr ) ); }

	// Concatenation operator
	TStr& Append( oexCONST T* pStr, oexUINT uSize )
	{
		// Allocate space for new string
		oexUINT uOldSize = Length();
		if ( !oexVERIFY( Allocate( uOldSize + uSize ) ) )
			return *this;

		// Append string
		os::CSys::MemCpy( m_mem.Ptr( uOldSize ), pStr, uSize );

		// Add new string length
		m_lLength = uOldSize + uSize;

		return *this;
	}

	/// Concatenation operator
	TStr& operator += ( oexCONST T* pStr )
	{	return Append( pStr ); }

	/// Concatenation operator
	TStr operator += ( TStr &str )
	{	return Append( str ); }


    /// Compare to const string
    oexBOOL operator == ( oexCONST T *x_pStr )
    {   return !Compare( x_pStr, zstr::Length( x_pStr ) ); }

    /// Compare to other object
    oexBOOL operator == ( TStr &str )
    {   return !Compare( str.Ptr(), str.Length() ); }

    /// Compare to const string
    oexINT Compare( oexCONST T *x_pPtr, oexUINT x_uLen )
    {   return str::Compare( Ptr(), Length(), x_pPtr, x_uLen ); }

public:

	TStr& vFmt( oexCONST T* x_pFmt, oexCPVOID x_pArgs )
	{
		// Verify input string
		if ( !oexVERIFY( x_pFmt ) )
		{	Allocate( 0 ); return *this; }

		T* pPt = oexNULL;
		oexRESULT res;
		oexUINT uSize = oexSTRSIZE;
		do
		{	
			// Allocate buffer
			// The - 1 is a performance boost, this keeps us under the TMemory block size.
			T* pPtr = Allocate( uSize - 1 ); 
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
	TStr& Fmt( oexCONST T* pFmt, ... )
	{	return vFmt( pFmt, ( (oexPVOID*)&pFmt ) + 1 ); }

	/// Converts the string to a GUID, returns oexNULL if this string does not contain a GUID
	oexCONST oexGUID * StringToGuid( oexGUID *x_pGuid )
    {	return guid::StringToGuid( x_pGuid, Ptr(), Length() ); }

	/// Converts a GUID to a string, if pGuid is NULL, a unique GUID is created
	TStr& GuidToString( oexCONST oexGUID *x_pGuid = oexNULL )
	{	guid::GuidToString( Allocate( 36 /* + 1 is implicit*/ ), 37, x_pGuid ); 
		oexVERIFY( Length() == 36 );
		return *this; 		
	}


public:

	/// Replaces single characters in a string
	TStr& Replace( oexCONST T x_tFind, oexCONST T x_tReplace )
    {   str::Replace( _Ptr(), Length(), x_tFind, x_tReplace ); return *this; }



private:

    /// The string memory
    TMem< T >       m_mem;

    /// The length of the string
    oexINT          m_lLength;
    
};


/// Unicode friendly string class
typedef TStr< oexTCHAR >    CStr;

/// 8 bit character string
typedef TStr< oexCHAR >     CStr8;

/// 16 bit character string
typedef TStr< oexINT16 >    CStr16;

/// 32 bit character string
typedef TStr< oexINT32 >    CStr32;

/// 64 bit character string
typedef TStr< oexINT64 >    CStr64;
