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

		/// Size type
		typedef oex::CBin::t_size		t_size;

		/// Byte type
		typedef oex::CBin::t_byte		t_byte;

		/// Buffer type
		typedef oex::CBin				t_buffer;

	public:

		SQBIND_CLASS_CTOR_BEGIN( CSqBinary )
		SQBIND_CLASS_CTOR_END( CSqBinary )

		/// Default constructor
		CSqBinary() {}

		/// Copy constructor
		CSqBinary( const CSqBinary &r ) : m_bin( r.m_bin ) { }

		/// Assignment operator
		CSqBinary& operator = ( const CSqBinary &r )
		{	m_bin = r.m_bin; return *this; }

		/// Copy constructor
		CSqBinary( const t_buffer &r ) : m_bin( r ) { }

		/// Assignment operator
		CSqBinary& operator = ( const t_buffer &r )
		{	m_bin = r; return *this; }

		/// Registers the class
		static void Register( sqbind::VM vm );

		/// Frees the memory
		void Free() { m_bin.Destroy(); }

		/// Allocate specified amount of memory
		t_size Allocate( t_size x_nSize ) { m_bin.Allocate( x_nSize ); return m_bin.Size(); }

		/// Resizes memory block
		t_size Resize( t_size x_nNewSize ) { m_bin.Resize( x_nNewSize ); return m_bin.Size(); }

		/// Copies memory object
		t_size Copy( CSqBinary *x_p ) { if ( !x_p ) return 0; return m_bin.Copy( &x_p->m_bin ); }

		/// Shares memory object
		t_size Share( CSqBinary *x_p ) { if ( !x_p ) return 0; return m_bin.Share( &x_p->m_bin ); }

		/// Returns the raw size of the buffer
		t_size Size() { return m_bin.Size(); }

		/// Returns the value of the byte at i
		int get( t_size x_i ) { return (int)m_bin.get( x_i ); }

		/// Sets the value of the byte at i
		void set( t_size x_i, int x_v ) { m_bin.set( x_i, (t_byte)x_v ); }

		/// Sets the number of bytes in the buffer being used
		void setUsed( t_size x_n ) { m_bin.setUsed( x_n ); }

		/// Returns the number of bytes in the buffer being used
		t_size getUsed() { return m_bin.getUsed(); }

		/// Sets the share name
		void SetName( const stdString &s ) { m_bin.SetName( s.c_str() ); }

		/// Returns the share name
		stdString GetName() { return m_bin.GetName(); }

		/// Sets plain share flag
		void PlainShare( int x_bPlain ) { m_bin.PlainShare( x_bPlain ? oex::oexTRUE : oex::oexFALSE ); }

		/// Returns non-zero if plain share
		int IsPlainShare() { return m_bin.IsPlainShare() ? 1 : 0; }

		/// Returns a string representation of the data
		stdString getString()
		{	oex::CStr s = oexMbToStr( m_bin.getString() );
			return stdString( s.Ptr(), s.Length() );
		}

		/// Initializes the binary buffer from a string
		t_size setString( const stdString &s )
		{	return m_bin.setString( oexStrToMb( oex::CStr( s.c_str(), s.length() ) ) ); }

		/// Appends the string data to the buffer
		t_size appendString( const stdString &s )
		{	return m_bin.appendString( oexStrToMb( oex::CStr( s.c_str(), s.length() ) ) ); }

		/// Return const pointer to raw buffer
		const t_byte* Ptr( t_size o = 0 ) { return m_bin.Ptr( o ); }

		/// Return writable pointer to raw buffer
		t_byte* _Ptr( t_size o = 0 ) { return m_bin._Ptr( o ); }

		/// Sets a raw buffer
		void setBuffer( t_byte *x_ptr, t_size x_size, int x_bFree = 1 )
		{	m_bin.setBuffer( x_ptr, x_size, x_bFree ? oex::oexTRUE : oex::oexFALSE ); }

		/// Copies data from a raw buffer
		t_size MemCpy( oexCONST t_byte *x_ptr, t_size x_size )
		{	return m_bin.MemCpy( x_ptr, x_size ); }

		/// Appends the specified buffer
		t_size Append( CSqBinary *x_p )
		{	if ( !x_p ) return 0; return m_bin.Append( &x_p->m_bin ); }

		/// Appends the specified raw buffer
		t_size AppendBuffer( const t_byte *x_pBuf, t_size x_nBytes )
		{	return m_bin.AppendBuffer( x_pBuf, x_nBytes ); }

		/// Shift data in buffer to the left
		t_size LShift( t_size x_nBytes )
		{	return m_bin.LShift( x_nBytes ); }

		/// Returns reference to buffer object
		t_buffer& Mem() { return m_bin; }

		int getCHAR( t_size x_nOffset ) { return m_bin.getCHAR( x_nOffset ); }
		void setCHAR( t_size x_nOffset, int val ) { m_bin.setCHAR( x_nOffset, val ); }

		unsigned int getUCHAR( t_size x_nOffset ) { return m_bin.getUCHAR( x_nOffset ); }
		void setUCHAR( t_size x_nOffset, unsigned int val ) { m_bin.setUCHAR( x_nOffset, val ); }

		int getINT( t_size x_nOffset ) { return m_bin.getINT( x_nOffset ); }
		void setINT( t_size x_nOffset, int val ) { m_bin.setINT( x_nOffset, val ); }

		unsigned int getUINT( t_size x_nOffset ) { return m_bin.getUINT( x_nOffset ); }
		void setUINT( t_size x_nOffset, unsigned int val ) { m_bin.setUINT( x_nOffset, val ); }

		long getLONG( t_size x_nOffset ) { return m_bin.getLONG( x_nOffset ); }
		void setLONG( t_size x_nOffset, long val ) { m_bin.setLONG( x_nOffset, val ); }

		unsigned long getULONG( t_size x_nOffset ) { return m_bin.getULONG( x_nOffset ); }
		void setULONG( t_size x_nOffset, unsigned long val ) { m_bin.setULONG( x_nOffset, val ); }

		float getFLOAT( t_size x_nOffset ) { return m_bin.getFLOAT( x_nOffset ); }
		void setFLOAT( t_size x_nOffset, float val ) { m_bin.setFLOAT( x_nOffset, val ); }

		double getDOUBLE( t_size x_nOffset ) { return m_bin.getDOUBLE( x_nOffset ); }
		void setDOUBLE( t_size x_nOffset, double val ) { m_bin.setDOUBLE( x_nOffset, val ); }

	private:

		t_buffer		m_bin;

	};

}; // end namespace

// Declare type for use as squirrel parameters
SQBIND_DECLARE_INSTANCE( sqbind::CSqBinary, CSqBinary )
