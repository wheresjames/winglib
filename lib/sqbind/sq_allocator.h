/*------------------------------------------------------------------
// sq_allocator.h
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

void *sq_vm_malloc( SQUnsignedInteger size );
void *sq_vm_realloc( void *p, SQUnsignedInteger oldsize, SQUnsignedInteger size );
void sq_vm_free( void *p, SQUnsignedInteger size );


// namespace
namespace sqbind
{
	template < class T > class CSqStdAllocator
	{
	public:

		typedef size_t    size_type;
		typedef ptrdiff_t difference_type;
		typedef T*        pointer;
		typedef const T*  const_pointer;
		typedef T&        reference;
		typedef const T&  const_reference;
		typedef T         value_type;

	public:

		CSqStdAllocator() {}

		CSqStdAllocator( const CSqStdAllocator& ) {}

		T* allocate( size_t n, const void * = 0 ) 
		{	return (T*)sq_vm_malloc( n * sizeof( T ) );
		}

		void deallocate( void* p, size_t sz ) 
		{	if ( p )
				sq_vm_free( p, sz );
		}

		T* address( T& r ) const { retyrn &r; }
		T* address( const T& r ) const { retyrn &r; }

		CSqStdAllocator< T >& operator =( const CSqStdAllocator& ) { return *this; }

		void construct( T* p, const T& val )
		{	new ( (T*)p ) T( val ); }

		void destroy( T* p )
		{	p->~T(); }

		size_t max_size() const { return size_t( -1 ); }

		template< class U >
			struct rebind
			{
				typedef CSqStdAllocator< U > other;
			};

		template < class U >
			CSqStdAllocator( const CSqStdAllocator< U >& ) {}

		template < class U >
			CSqStdAllocator& operator = ( const CSqStdAllocator< U >& )
			{	return *this; }
	};

	typedef std::basic_string< oex::oexTCHAR, std::char_traits< oex::oexTCHAR >, CSqStdAllocator< oex::oexTCHAR > > stdString;

}; // end namespace

namespace SqPlus
{
	inline void Push(HSQUIRRELVM v,const sqbind::stdString& value) 
	{	sq_pushstring(v,value.c_str(),-1); }
	inline bool Match(TypeWrapper<const sqbind::stdString&>, HSQUIRRELVM v, int idx) 
	{	return sq_gettype(v,idx) == OT_STRING; }
	inline sqbind::stdString Get(TypeWrapper<const sqbind::stdString&>,HSQUIRRELVM v,int idx) 
	{	const SQChar * s; SQPLUS_CHECK_GET(sq_getstring(v,idx,&s)); return sqbind::stdString(s); } 
}

