/*------------------------------------------------------------------
// sqeng_extern.hpp
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

namespace sqbind
{
	static SSqAllocator g_SqAllocator = { 0, 0, 0 };

	oex::oexRESULT SQBIND_SetAllocator( sqbind::SSqAllocator *x_pAllocator )
	{
		if ( x_pAllocator )
			oexMemCpy( &g_SqAllocator, x_pAllocator, sizeof( g_SqAllocator ) );
		else
			oexZeroMemory( &g_SqAllocator, sizeof( g_SqAllocator ) );

		return 0;
	}
}; // end namespace sqbind

void *sq_vm_malloc( SQUnsignedInteger size )
{
	if ( oexCHECK_PTR( sqbind::g_SqAllocator.fnMalloc ) )
		return sqbind::g_SqAllocator.fnMalloc( size );
	return oex::os::CMem::New( size, oexLINE, oexFILE );
}

void *sq_vm_realloc( void *p, SQUnsignedInteger oldsize, SQUnsignedInteger size )
{
	if ( oexCHECK_PTR( sqbind::g_SqAllocator.fnRealloc ) )
		return sqbind::g_SqAllocator.fnRealloc( p, size );

	return oex::os::CMem::Resize( p, size, oexLINE, oexFILE );
}

void sq_vm_free( void *p, SQUnsignedInteger size )
{
	if ( oexCHECK_PTR( sqbind::g_SqAllocator.fnFree ) )
	{	sqbind::g_SqAllocator.fnFree( p );
		return;
	} // end if

	return oex::os::CMem::Delete( p );
}


