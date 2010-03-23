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

#if defined( OEXLIB_USING_TLSAPI )

/// Tls API index
static oexUINT g_tls_dwIndex = ( (oexUINT)-1 );

#elif defined( OEXLIB_USING_TLS )

/// Thread specific stack pointer
static oexTLS CStackTrace::CStack *g_tls_pStack = oexNULL;

#endif


CStackTrace::CStack* CStackTrace::InitPush() 
{
	if ( !m_bEnable )
		return oexNULL;

#if defined( OEXLIB_USING_TLSAPI )

	// Return thread local storage index
	CStack *pStack;
	if ( ( (oexUINT)-1 ) != g_tls_dwIndex )
	{	pStack = (CStack*)oexTlsGetValue( g_tls_dwIndex );
		if ( pStack != oexNULL ) 
			return pStack;
	} // end if

	// Allocate TLS 
	if ( ( (oexUINT)-1 ) == g_tls_dwIndex )
		g_tls_dwIndex = oexTlsAllocate();
	
	// Set stack value
	pStack = GetStack();
	if ( !pStack )
		return oexNULL;

	// Save stack value
	if ( ( (oexUINT)-1 ) != g_tls_dwIndex ) 
		oexTlsSetValue( g_tls_dwIndex, pStack );
		
	return pStack;

#elif defined( OEXLIB_USING_TLS )

	// Are we already initialized for this thread?
	if ( g_tls_pStack ) 
		return g_tls_pStack;

	return ( g_tls_pStack = GetStack() );

#else

	return GetStack(); 

#endif	

}

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

	if ( ( (oexUINT)-1 ) != g_tls_dwIndex )
		oexTlsSetValue( g_tls_dwIndex, 0 );

#elif defined( OEXLIB_USING_TLS )

	g_tls_pStack = 0;

#endif

	// Ok to remove us, because only this thread would be accesing this slot
	m_poolStack.Remove( (t_stack::t_key)dwThreadId );

	// Delete the stack object
	OexAllocDestruct( p );

	return oexTRUE;
#endif
}

void CStackTrace::Release() 
{	
	// No more stack tracing
	m_bEnable = oexFALSE; 

	oexTRY
	{
		// Delete object if any
		if ( oexSt().getUsedSlots() )
		{
#if defined( OEXLIB_STACK_ENABLE_LOCKS )

			// Lock for good
			oexSt().m_lock.Wait( 30000 );

#endif
			// Release stack objects
			oexUINT i = 0;
			while ( CStack *p = oexSt().Next( &i ) )
				OexAllocDestruct( p );

		} // end if

	} oexCATCH_ALL() {}
}

