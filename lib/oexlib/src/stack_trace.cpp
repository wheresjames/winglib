/*------------------------------------------------------------------
// stack_trace.cpp
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

#include "../oexlib.h"

OEX_USING_NAMESPACE

/// Set to non-zero to enable stack tracing
oexBOOL CStackTrace::m_bEnable = oexFALSE;

/// Instance of stack trace
CStackTrace *CStackTrace::m_pst = oexNULL;

#if defined( OEXLIB_USING_TLSAPI )

/// Tls API index
oexUINT CStackTrace::m_tls_dwIndex = ( (oexUINT)-1 );

#elif defined( OEXLIB_USING_TLS )

/// Thread specific stack pointer
oexTLS CStackTrace::CStack *CStackTrace::m_tls_pStack = oexNULL;

#endif

CStackTrace::CStack* CStackTrace::GetStack()
{
	// Current thread id
	oexUINT dwThreadId = oexGetCurThreadId();
	if ( !dwThreadId )
		return oexNULL;

	// Create stack pool if needed
	if ( !m_poolStack.isValid() )
		m_poolStack.Create( eMaxThreadsBits );

	// Attempt to find in pool
	CStack *p = m_poolStack.Ptr( (t_stack::t_key)dwThreadId );
	if ( p )
		return p;

	// Punt if no more free slots
	if ( !m_poolStack.getFreeSlots() )
		return oexNULL;

#if defined( OEXLIB_STACK_ENABLE_LOCKS )

	// Have to lock to modify
	oexAutoLock ll( m_lock );
	if ( !ll.IsLocked() ) 
		return oexNULL;

#endif

	// Create a new stack pointer
	p = OexAllocConstruct< CStack >();
	if ( !p )
		return oexNULL;

	// Add to our list
	if ( !m_poolStack.Add( (t_stack::t_key)dwThreadId, p ) )
	{	OexAllocDestruct( p ); return oexNULL; }

	// Setup the stack class
	p->SetThreadId( dwThreadId );

	return p;
}

oexBOOL CStackTrace::RemoveThread() 
{
#if defined( OEXLIB_STACK_KEEP_INACTIVE_TRACES )

	return oexFALSE;

#else

	if ( !m_poolStack.isValid() || !m_bEnable )
		return oexFALSE;

	// Get current thread id
	oexUINT dwThreadId = oexGetCurThreadId();
	if ( !dwThreadId )
		return oexFALSE;

	// Get stack object for this thread
	CStack *p = m_poolStack.Ptr( (t_stack::t_key)dwThreadId );
	if ( !p )
		return oexFALSE;

#if defined( OEXLIB_STACK_ENABLE_LOCKS )

	// Have to lock to modify
	oexAutoLock ll( m_lock );
	if ( !ll.IsLocked() ) 
		return oexNULL;

#endif


#if defined( OEXLIB_USING_TLSAPI )

	if ( ( (oexUINT)-1 ) != m_tls_dwIndex )
		oexTlsSetValue( m_tls_dwIndex, 0 );

#elif defined( OEXLIB_USING_TLS )

	m_tls_pStack = 0;

#endif

	// Ok to remove us, because only this thread would be accesing this slot
	m_poolStack.Remove( (t_stack::t_key)dwThreadId );

	// Delete the stack object
	OexAllocDestruct( p );

	return oexTRUE;
#endif
}
