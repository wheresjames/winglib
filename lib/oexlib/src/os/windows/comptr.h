/*------------------------------------------------------------------
// comptr.h
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

#ifndef __ATLBASE_H__

/// Simple wrapper around a COM pointer
template < typename T >
	class CComPtr
{
public:

	/// Default constructor
	CComPtr()
	{
		p = NULL;
	}

	/// Copy constructor
	CComPtr( const CComPtr< T > &r )
	{
		p = r.p;
		if ( p )
			p->AddRef();
	}

	/// Copy constructor
	CComPtr( T *t )
	{
		p = t;
		if ( p )
			p->AddRef();
	}

	/// Destructor
	~CComPtr()
	{	Release();
	}

	/// Releases the com pointer
	void Release()
	{
		if ( p )
			p->Release();
		p = NULL;
	}

	/// Attach to an existing pointer
	void Attach( T* t )
	{
		Release();
		p = t;
	}

	/// Detach from encapsulated pointer
	T* Detach()
	{
		T* t = p;
		p = NULL;
		return t;
	}

	/// Create object instance
	HRESULT CoCreateInstance( REFCLSID rclsid, LPUNKNOWN pUnkOuter = NULL, DWORD dwClsContext = CLSCTX_ALL, REFIID riid = __uuidof( T ), LPVOID *ppv = NULL )
	{	Release();
		HRESULT hr = ::CoCreateInstance( rclsid, pUnkOuter, dwClsContext, riid, (void**)&p);
		if ( ppv )
			*ppv = p;
		return hr;
	}

	// operators
	operator bool() { return NULL != p; }
	T** operator &() { return &p; }
	T* operator ->() { return p; }
	operator T&() { return *p; }
	operator T*() { return p; }
	CComPtr< T >& operator = ( const CComPtr< T > &r ) { p = r.p; if ( p ) p->AddRef(); return *this; }
	CComPtr< T >& operator = ( T *t ) { p = t; if ( p ) p->AddRef(); return *this; }

public:

	// Must be the same as the real CComPtr
	T *p;

};

#endif