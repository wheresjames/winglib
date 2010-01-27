/*------------------------------------------------------------------
// bin_share.h
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

#define OEX_CBIN_DECLARE_TYPE_ACCESS( t )							\
	oex##t get##t( t_size x_nOffset )								\
	{	x_nOffset = x_nOffset * sizeof( oex##t );					\
		if ( x_nOffset + sizeof( oex##t ) >= Size() ) return 0;		\
		return *( (oex##t*)Ptr( x_nOffset ) );						\
	}																\
	void set##t( t_size x_nOffset, oex##t val )						\
	{	x_nOffset = x_nOffset * sizeof( oex##t );					\
		if ( x_nOffset + sizeof( oex##t ) >= Size() ) return;		\
		*( (oex##t*)Ptr( x_nOffset ) ) = val;						\
	}

/// Shared memory block
class CBin
{
public:

	/// Byte type ( Want this to match CStr8 )
	typedef CStr8::t_char		t_byte;

	/// Size type
	typedef CStr8::t_size		t_size;

	/// Buffer type
	typedef CStr8::t_buffer		t_buffer;

public:

	/// Default constructor
	CBin()
	{
		m_nUsed = 0;
		m_ptr = oexNULL;
		m_bFree = oexFALSE;
	}

	/// Copy constructor
	CBin( const CBin &r ) :
		m_buf( r.m_buf )
	{
		m_nUsed = 0;
		m_ptr = oexNULL;
		m_bFree = oexFALSE;

		if ( r.m_ptr )
			setBuffer( r.m_ptr, r.m_nUsed, r.m_bFree );
		else
			m_nUsed = r.m_nUsed;
	}

	/// Assignment operator
	CBin& operator = ( const CBin &x_r )
	{
		// Lose current
		Destroy();

		// Ptr?
		if ( x_r.m_ptr )
			setBuffer( x_r.m_ptr, x_r.m_nUsed, x_r.m_bFree );

		else
		{	m_buf.Share( x_r.m_buf );
			m_nUsed = x_r.m_nUsed;
		} // end else

		return *this;
	}

	/// Copy constructor
	/**
		Creates from buffer
	*/
	CBin( const t_buffer &x_r, t_size x_nUsed = 0 ) : m_buf( x_r )
	{
		m_nUsed = x_nUsed;
		m_ptr = oexNULL;
		m_bFree = oexFALSE;
	}

	/// Destructor
	~CBin() { Destroy(); }

	/// Frees the buffer
	void Destroy()
	{	m_nUsed = 0;
		FreePtr();
		m_buf.Destroy();
	}

	void FreePtr()
	{	if ( m_ptr && m_bFree )
			OexAllocDelete< t_byte >( m_ptr );
		m_ptr = oexNULL;
		m_bFree = oexFALSE;
	}

	/// Allocate specified number of bytes
	const t_byte* Allocate( t_size x_nSize )
	{	FreePtr();
		m_buf.OexNew( x_nSize );
		return Ptr();
	}

	/// Resize buffer to specified size
	const t_byte* Resize( t_size x_nNewSize );

	/// Unshare the internal buffer
	t_size Unshare()
	{	return Copy(); }

	/// Copy another buffer
	t_size Copy( CBin *x_p = oexNULL );

	/// Copy another buffer
	t_size Copy( CBin &x_r ) { return Copy( &x_r ); }

	/// Shares another buffer
	t_size Share( CBin *x_p = oexNULL );

	/// Shares another buffer
	t_size Share( CBin &x_r ) { return Share( &x_r ); }

	/// Returns the size of the current buffer
	t_size Size()
	{
		// Ptr buffer?
		if ( m_ptr )
			return m_nUsed;

		// Native buffer size
		return (t_size)m_buf.Size();
	}

	/// Returns character value at i
	t_byte get( t_size i )
	{
		if ( i >= m_nUsed )
			return 0;

		if ( m_ptr )
			return m_ptr[ i ];

		return *m_buf.Ptr( i );
	}

	/// Sets character value at i
	void set( t_size i, t_byte v )
	{
		if ( i >= m_nUsed )
			return;

		if ( m_ptr )
			m_ptr[ i ] = v;

		*m_buf.Ptr( i ) = v;
	}

	/// Sets the number of bytes in the buffer being used
	void setUsed( t_size n )
	{
		// Set new used size
		m_nUsed = n;

		// Ensure valid
		if ( 0 > m_nUsed )
			m_nUsed = 0;

		// Ensure valid
		else if ( m_nUsed > Size() )
			m_nUsed = Size();
	}

	/// Returns the number of bytes in the buffer being used
	t_size getUsed()
	{
		if ( 0 > m_nUsed )
			m_nUsed = 0;

		if ( m_ptr )
			return m_nUsed;

		else if ( Size() < m_nUsed )
			m_nUsed = Size();

		return m_nUsed;
	}

	/// Sets the share name
	void SetName( oexCSTR s )
	{	m_buf.SetName( s ); }

	/// Returns the share name
	oexCSTR GetName()
	{	return m_buf.GetName(); }

	/// Sets plain share flag
	void PlainShare( oexBOOL x_bPlain )
	{	m_buf.PlainShare( x_bPlain ); }

	/// Returns non-zero if plain share
	oexBOOL IsPlainShare()
	{	return m_buf.IsPlainShare(); }

	/// Returns a string representation of the buffer
	/**
		\warning Careful, this string may *not* be null terminated
				 If you use TStr::Ptr(), you may have issues
	*/
	CStr8 getString()
	{
		// Must copy if outside ptr
		if ( m_ptr )
			return getSafeString();

		// Return share
		CStr8 s;
		s.Mem().Share( m_buf );
		s._SetLength( getUsed() );
		return s;
	}

	/// Returns a string representation of the buffer, it will be null terminated
	/**
		At the cost of copying the buffer, this string will always be null terminated

	*/
	CStr8 getSafeString()
	{	return CStr8( Ptr(), getUsed() );
	}

	/// Converts a string to a binary buffer
	// Returns the number of bytes in the buffer
	t_size setString( const CStr8 &s )
	{	FreePtr();
		m_buf.Share( s.c_Mem() );
		m_nUsed = s.Length();
		return getUsed();
	}

	/// Appends a string to the end of the buffer
	// Returns the number of bytes in the buffer
	t_size appendString( const CStr8 &s )
	{	AppendBuffer( s.Ptr(), s.Length() );
		return getUsed();
	}

	/// Returns a pointer to the buffer
	const t_byte* Ptr( t_size o = 0 ) { if ( m_ptr ) return m_ptr + o; return m_buf.Ptr( o ); }

	/// Returns a writable pointer to the buffer
	t_byte* _Ptr( t_size o = 0 ) { if ( m_ptr ) return m_ptr + o; return m_buf.Ptr( o ); }

	/// Sets a Ptr buffer pointer ( make sure it doesn't go away before this class! )
	void setBuffer( t_byte *x_ptr, t_size x_size, oexBOOL x_bFree = oexTRUE )
	{
		Destroy();

		if ( !x_size )
			return;

		// Save Ptr buffer info
		m_nUsed = x_size;
		m_ptr = x_ptr;
		m_bFree = x_bFree;
		if ( m_ptr && m_bFree )
			oexAddRef( m_ptr );
	}

	/// Sets a Ptr buffer pointer ( make sure it doesn't go away before this class! )
	t_size MemCpy( oexCONST t_byte *ptr, t_size size )
	{
		// Free any Ptr buffer
		FreePtr();

		// Copy data
		if ( m_buf.MemCpy( ptr, size ).Size() < size )
		{	m_nUsed = 0;
			return 0;
		} // end if

		// Update used count
		m_nUsed = size;

		return 1;
	}

	/// Append buffer
	t_size Append( CBin *x_p )
	{
		if ( !x_p )
			return m_nUsed;

		return AppendBuffer( x_p->Ptr(), x_p->getUsed() );
	}

	/// Append data to buffer
	t_size AppendBuffer( const t_byte *x_pBuf, t_size x_nBytes );

	/// Shift the data in the buffer to the left
	t_size LShift( t_size x_nBytes );

	/// Declare access types
	OEX_CBIN_DECLARE_TYPE_ACCESS( CHAR );
	OEX_CBIN_DECLARE_TYPE_ACCESS( UCHAR );
	OEX_CBIN_DECLARE_TYPE_ACCESS( INT );
	OEX_CBIN_DECLARE_TYPE_ACCESS( UINT );
	OEX_CBIN_DECLARE_TYPE_ACCESS( LONG );
	OEX_CBIN_DECLARE_TYPE_ACCESS( ULONG );
	OEX_CBIN_DECLARE_TYPE_ACCESS( INT64 );
	OEX_CBIN_DECLARE_TYPE_ACCESS( UINT64 );
	OEX_CBIN_DECLARE_TYPE_ACCESS( FLOAT );
	OEX_CBIN_DECLARE_TYPE_ACCESS( DOUBLE );

	/// Returns buffer object reference
	t_buffer& Mem() { return m_buf; }

private:

	/// Number of bytes in the buffer being used
	t_size			m_nUsed;

	/// Size of the buffer
	t_buffer		m_buf;

	/// Outside buffer pointer
	t_byte			*m_ptr;

	/// Non-zero if the Ptr buffer needs to be freed
	oexBOOL			m_bFree;
};


/// Allocator
/**
    Memory repository

  \code


  \endcode

*/
class CBinShare
{
	enum
	{
		/// Amount of time in seconds to retain unaccessed buffers
		eBufferTimeout = 60
	};

	class CBinHolder
	{
	public:

		/// Default constructor
		CBinHolder() {}

		/// Copy constructor
		CBinHolder( const CBinHolder &c )
			: bin( c.bin ), to( to ) {}

		/// Assignment operator
		CBinHolder& operator = ( const CBinHolder &c )
		{	bin = c.bin; to = to; return *this; }

		/// Buffer
		CBin			bin;

		/// Expires
		oexUINT			to;
	};


public:

	/// Buffer list type
    typedef TAssoList< CStr, CBinHolder >	t_buffers;

	/// Size type
	typedef CBin::t_size					t_size;


public:

    /// Constructor
    CBinShare()
    {
#if defined( oexDEBUG )
		m_uTime = 0;
#endif
    }

    /// Destructor
    virtual ~CBinShare()
	{
		Destroy();
	}

	/// Releases all resources
	void Destroy();

	/// Returns the specified buffer
	CBin GetBuffer( CStr sName, CBinShare::t_size uSize = 0 );

	/// Returns the specified buffer
	oexBOOL SetBuffer( CStr sName, CBin *pBin );

	/// Returns non-zero if the specified binary buffer exists
	oexBOOL IsBuffer( CStr sName );

	/// Releases unused buffers
	oexINT Cleanup();

public:

	/// Thread lock
	oexLock					m_lock;

	/// List of active buffers
	t_buffers				m_buffers;

#if defined( oexDEBUG )
	/// Last cleanup time
	oexUINT					m_uTime;
#endif

};

