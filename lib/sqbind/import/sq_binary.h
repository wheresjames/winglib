/*------------------------------------------------------------------
// sq_binary.h
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
    class CSqBinary
    {
	public:

		/// Byte type
		typedef oex::oexBYTE t_byte;

		/// Buffer type
		typedef oex::TMem< t_byte > t_buffer;

		/// Size type
		typedef int t_size;

	public:

		SQBIND_CLASS_CTOR_BEGIN( CSqBinary )
		SQBIND_CLASS_CTOR_END( CSqBinary )

		/// Default constructor
		CSqBinary() { m_nUsed = 0; m_ptr = 0; }

		/// Copy constructor
		CSqBinary( const CSqBinary &r ) :
			m_buf( r.m_buf )
		{
			m_nUsed = r.m_nUsed;
			m_ptr = r.m_ptr;
		}

		/// Assignment operator
		CSqBinary& operator=( const CSqBinary &r )
		{	m_nUsed = r.m_nUsed;
			m_buf.Share( r.m_buf );
			m_ptr = r.m_ptr;
			return *this;
		}

		/// Copy constructor
		CSqBinary( const t_buffer &r, t_size nUsed = 0 ) :
			m_buf( r )
		{	m_nUsed = nUsed;
			m_ptr = 0;
		}

		/// Registers the class
		static void Register( sqbind::VM vm );

		/// Frees the buffer
		void Free()
		{	m_nUsed = 0;
			m_ptr = 0;
			m_buf.Destroy();
		}

		/// Allocate specified number of bytes
		int Allocate( t_size nSize )
		{	m_ptr = 0;
			m_buf.OexNew( nSize );
			return m_buf.Size() == (unsigned int)nSize ? 1 : 0;
		}

		/// Resize buffer to specified size
		int Resize( t_size nNewSize )
		{
			if ( m_ptr )
			{
				// Is the buffer getting smaller?
				if ( m_nUsed > nNewSize )
					m_nUsed = nNewSize;

				// Make a new buffer and copy data
				m_buf.OexNew( nNewSize );
				oexMemCpy( m_buf.Ptr(), m_ptr, m_nUsed );
				m_ptr = 0;

			} // end if

			// Simple resize
			else
				m_buf.Resize( nNewSize );

			return m_buf.Size() == (unsigned int)nNewSize ? 1 : 0;
		}

		/// Copy another buffer
		int Copy( CSqBinary *p )
		{
			if ( !p )
				return 0;

			m_ptr = 0;
			m_nUsed = p->m_nUsed;
			if ( !m_nUsed )
				Free();
			else
			{
				if ( !p->m_ptr )
					m_buf.Copy( p->m_buf );

				else
					m_buf.MemCpy( p->m_ptr, m_nUsed );

			} // end else
			return m_buf.Size();
		}

		/// Shares another buffer
		int Share( CSqBinary *p )
		{
			if ( !p )
				return 0;

			m_nUsed = p->m_nUsed;
			m_ptr = p->m_ptr;
			m_buf.Share( p->m_buf );

			return 1;
		}

		/// Returns the size of the current buffer
		t_size Size()
		{
			if ( m_ptr )
				return m_nUsed;

			return (t_size)m_buf.Size();
		}

		/// Returns character value at i
		int get( int i )
		{	if ( i >= Size() ) return -1;
			return (int)m_buf.Ptr( i );
		}

		/// Sets character value at i
		void set( int i, int v )
		{	if ( i >= Size() ) return;
			*m_buf.Ptr( i ) = (t_byte)v;
		}

		/// Sets the number of bytes in the buffer being used
		void setUsed( t_size n )
		{	m_nUsed = n; }

		/// Returns the number of bytes in the buffer being used
		t_size getUsed()
		{	if ( 0 > m_nUsed ) m_nUsed = 0;
			if ( m_ptr ) return m_nUsed;
			else if ( Size() < m_nUsed ) m_nUsed = Size();
			return m_nUsed;
		}

		/// Sets the share name
		void SetName( const stdString &s )
		{	m_buf.SetName( s.c_str() );
		}

		/// Returns the share name
		stdString GetName()
		{	return m_buf.GetName();
		}

		/// Returns a string representation of the buffer
		stdString getString()
		{	oex::CStr s = oexMbToStr( oex::CStr8( (const oex::CStr8::char_type*)Ptr(), getUsed() ) );
			return stdString( s.Ptr(), s.Length() );
		}

		/// Converts a string to a binary buffer
		// Returns the number of bytes in the buffer
		t_size setString( const stdString &s )
		{	m_ptr = 0;
			oex::CStr8 mb = oexStrToMb( oex::CStr( s.c_str(), s.length() ) );
			m_buf.MemCpy( (const t_byte*)mb.Ptr(), mb.Length() );
			return (t_size)m_buf.Size();
		}

		/// Converts a string to a binary buffer
		// Returns the number of bytes in the buffer
		// +++ This isn't quite right, should use getUsed()
		t_size appendString( const stdString &s )
		{	if ( m_ptr ) { m_buf.MemCpy( m_ptr, getUsed() ); m_ptr = 0; }
			oex::CStr8 mb = oexStrToMb( oex::CStr( s.c_str(), s.length() ) );
			m_buf.MemAppend( (const t_byte*)mb.Ptr(), mb.Length() );
			return (t_size)m_buf.Size();
		}

		/// Returns a pointer to the buffer
		const t_byte* Ptr() { if ( m_ptr ) return m_ptr; return m_buf.Ptr(); }

		/// Returns a pointer to the buffer
		t_byte* _Ptr() { if ( m_ptr ) return m_ptr; return m_buf.Ptr(); }

		/// Returns reference to buffer
		t_buffer& Obj() { return m_buf; }

		/// Sets a custom buffer pointer ( make sure it doesn't go away before this class! )
		void setBuffer( t_byte *ptr, t_size size )
		{	Free(); m_ptr = ptr; m_nUsed = size; }

	private:

		/// The number of bytes in the buffer actually being used
		t_size						m_nUsed;

		/// Buffer object
		t_buffer					m_buf;

		/// User buffer ( a bit more dangerous to use )
		t_byte						*m_ptr;

	};

}; // end namespace

// Declare type for use as squirrel parameters
SQBIND_DECLARE_INSTANCE( sqbind::CSqBinary, CSqBinary )
