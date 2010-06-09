
#pragma once

#include "stdafx.h"

extern const _OEXCOM_IID IID_IAxTest;

class IAxTest
{
public:

	_OEXCOM_IUnknown_Interface();

	virtual int _OEXCOM_STDCALL Add( int a, int b ) = 0;

};

class CAxTest : IAxTest
{
public:

	virtual _OEXCOM_ULONG _OEXCOM_STDCALL QueryInterface( oexCONST _OEXCOM_IID& iid,void** ppv )
	{	if ( !ppv ) return _OEXCOM_E_INVALIDARG;
		*ppv = 0;
		if ( _OEXCOM_CMPGUID( &_OEXCOM_IID_IUnknown, &iid ) )
		{	AddRef(); *ppv = (void*)(_OEXCOM_IUnknown*)this; return _OEXCOM_S_OK; }
		return OnQueryInterface( iid, ppv );
	}

	virtual _OEXCOM_ULONG _OEXCOM_STDCALL AddRef()
	{	oexInterlockedIncrement( &_g_oex_lRefCount );
		return (_OEXCOM_ULONG)oexInterlockedIncrement( &_m_ref._m_ref_count ); 
	}

	virtual _OEXCOM_ULONG _OEXCOM_STDCALL Release()
	{	oexInterlockedDecrement( &_g_oex_lRefCount );
		_OEXCOM_ULONG c = oexInterlockedDecrement( &_m_ref._m_ref_count );
		if ( !c ) 
			_OEXCOM_DeleteInstance( this );
		return c;
	}

	template< typename T > static T* _OEXCOM_CreateInstance()
		{ 
			T *p = _OEXCOM_NEW< T >(); 
			if ( p ) 
				p->AddRef(); 
			return p; 
		}

	template< typename T > static void _OEXCOM_DeleteInstance( T *p ) 
	{ if ( p ) _OEXCOM_DELETE( p ); }

	_OEXCOM_IReference _m_ref;


	virtual _OEXCOM_ULONG OnQueryInterface( oexCONST _OEXCOM_IID& iid,void** ppv ) 
	{
		if ( !ppv ) 
			return _OEXCOM_E_INVALIDARG;

		if ( _OEXCOM_CMPGUID( &IID_IAxTest, &iid ) )
		{	AddRef();
			*ppv = (void*)(CAxTest*)this;
			return _OEXCOM_S_OK;
		}
		return _OEXCOM_E_NOINTERFACE; 
	}

//	_OEXCOM_BEGIN_INTERFACE_MAP
//		_OEXCOM_DECLARE_INTERFACE( IID_IAxTest, CAxTest )
//	_OEXCOM_END_INTERFACE_MAP

	virtual int _OEXCOM_STDCALL Add( int a, int b ) { return a + b; }

};

