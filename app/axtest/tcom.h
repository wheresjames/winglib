
#pragma once

//------------------------------------------------------------------
// COM Error codes
//------------------------------------------------------------------

#define _OEXCOM_S_OK				0x00000000L
#define _OEXCOM_S_FALSE				0x00000001L
#define _OEXCOM_E_NOTIMPL			0x80004001L
#define _OEXCOM_E_NOINTERFACE		0x80004002L
#define _OEXCOM_E_POINTER			0x80004003L
#define _OEXCOM_E_ABORT				0x80004004L
#define _OEXCOM_E_FAIL				0x80004005L
#define _OEXCOM_E_NOAGGREGATION		0x80040110L
#define _OEXCOM_E_HANDLE			0x80070006L
#define _OEXCOM_E_OUTOFMEMORY		0x8007000EL
#define _OEXCOM_E_INVALIDARG		0x80070057L
#define _OEXCOM_E_UNEXPECTED		0x8000FFFFL


//------------------------------------------------------------------
// COM types
//------------------------------------------------------------------

#define _OEXCOM_STDCALL				__stdcall
#define _OEXCOM_STDAPI				extern "C" _OEXCOM_HRESULT _OEXCOM_STDCALL

#define _OEXCOM_HRESULT				oex::oexULONG
#define _OEXCOM_IID					oex::oexGUID
#define _OEXCOM_ULONG				oex::oexULONG
#define _OEXCOM_LONG				oex::oexLONG
#define _OEXCOM_UINT				oex::oexUINT
#define _OEXCOM_INT					oex::oexINT
#define _OEXCOM_BOOL				oex::oexINT
#define _OEXCOM_TRUE				1
#define _OEXCOM_FALSE				0

#define _OEXCOM_MAKE_HANDLE( n )	struct n##__ { int n; }; typedef struct n##__ *n
_OEXCOM_MAKE_HANDLE( _OEXCOM_HANDLE );

extern const _OEXCOM_IID _OEXCOM_IID_IUnknown;
extern const _OEXCOM_IID _OEXCOM_IID_IClassFactory;
extern const _OEXCOM_IID _OEXCOM_IID_IDispatch;
extern oex::oexLONG _g_oex_lRefCount;

//------------------------------------------------------------------
// COM functions
//------------------------------------------------------------------

#define _OEXCOM_CMPGUID			oex::guid::CmpGuid
#define _OEXCOM_NEW				OexAllocConstruct
#define _OEXCOM_DELETE			OexAllocDelete

//------------------------------------------------------------------
// OEX Macros for COM
//------------------------------------------------------------------
#define _OEXCOM_IUnknown_Interface() \
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL QueryInterface( oexCONST _OEXCOM_IID& iid,void** ppv ) = 0; \
	virtual _OEXCOM_ULONG _OEXCOM_STDCALL AddRef() = 0; \
	virtual _OEXCOM_ULONG _OEXCOM_STDCALL Release() = 0; \

#define _OEXCOM_IUnknown_Implement() \
	virtual _OEXCOM_ULONG _OEXCOM_STDCALL QueryInterface( oexCONST _OEXCOM_IID& iid,void** ppv ) \
	{	if ( !ppv ) return _OEXCOM_E_INVALIDARG; \
		*ppv = 0; \
		if ( _OEXCOM_CMPGUID( &_OEXCOM_IID_IUnknown, &iid ) ) \
		{	AddRef(); *ppv = (void*)this; return _OEXCOM_S_OK; } \
		return OnQueryInterface( iid, ppv ); \
	} \
	virtual _OEXCOM_ULONG _OEXCOM_STDCALL AddRef() \
	{	oexInterlockedIncrement( &_g_oex_lRefCount ); \
		return (_OEXCOM_ULONG)oexInterlockedIncrement( &_m_ref._m_ref_count ); } \
	virtual _OEXCOM_ULONG _OEXCOM_STDCALL Release() \
	{	oexInterlockedDecrement( &_g_oex_lRefCount ); \
		_OEXCOM_ULONG c = oexInterlockedDecrement( &_m_ref._m_ref_count ); \
		if ( !c ) _OEXCOM_DeleteInstance( this ); \
		return c; \
	} \
	template< typename T > static T* _OEXCOM_CreateInstance() \
		{ T *p = _OEXCOM_NEW< T >(); if ( p ) p->AddRef(); return p; } \
	template< typename T > static void _OEXCOM_DeleteInstance( T *p ) { if ( p ) _OEXCOM_DELETE( p ); } \
	_OEXCOM_IReference _m_ref;

#define _OEXCOM_BEGIN_INTERFACE_MAP \
	_OEXCOM_IUnknown_Implement(); \
	virtual _OEXCOM_ULONG OnQueryInterface( oexCONST _OEXCOM_IID& iid,void** ppv ) \
	{	if ( !ppv ) return _OEXCOM_E_INVALIDARG;
#define _OEXCOM_END_INTERFACE_MAP return _OEXCOM_E_NOINTERFACE; }
#define _OEXCOM_DECLARE_INTERFACE( n, c ) \
	if ( _OEXCOM_CMPGUID( &n, &iid ) ) \
	{	AddRef(); \
		*ppv = (void*)(c*)this; \
		return _OEXCOM_S_OK; \
	}

//------------------------------------------------------------------
// COM Interfaces
//------------------------------------------------------------------
class _OEXCOM_IReference
{
public:
	_OEXCOM_IReference() : _m_ref_count( 0 ) { }
	_OEXCOM_LONG _m_ref_count;
};


class _OEXCOM_IUnknown
{
public:
	_OEXCOM_IUnknown_Interface();
};

class _OEXCOM_IClassFactory : public _OEXCOM_IUnknown
{
public:

	_OEXCOM_IUnknown_Interface();

	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL CreateInstance( _OEXCOM_IUnknown* pUnkOuter, const _OEXCOM_IID& iid, void** ppv ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL LockServer( _OEXCOM_BOOL bLock ) = 0;

};

template< typename T >
	class _OEXCOM_CClassFactory : public _OEXCOM_IClassFactory
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

	template< typename TT > static TT* _OEXCOM_CreateInstance()
		{
			TT *p = _OEXCOM_NEW< TT >();
			if ( p )
				p->AddRef();
			return p;
		}

	template< typename TT > static void _OEXCOM_DeleteInstance( TT *p )
	{ if ( p ) _OEXCOM_DELETE( p ); }

	_OEXCOM_IReference _m_ref;


	virtual _OEXCOM_ULONG OnQueryInterface( oexCONST _OEXCOM_IID& iid,void** ppv )
	{
		if ( !ppv )
			return _OEXCOM_E_INVALIDARG;

		if ( _OEXCOM_CMPGUID( &_OEXCOM_IID_IClassFactory, &iid ) )
		{	AddRef();
			*ppv = (void*)(_OEXCOM_CClassFactory*)this;
			return _OEXCOM_S_OK;
		}
		return _OEXCOM_E_NOINTERFACE;
	}


//	_OEXCOM_BEGIN_INTERFACE_MAP
//		_OEXCOM_DECLARE_INTERFACE( _OEXCOM_IID_IClassFactory, _OEXCOM_CClassFactory )
//	_OEXCOM_END_INTERFACE_MAP

	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL CreateInstance( _OEXCOM_IUnknown* pUnkOuter, const _OEXCOM_IID& iid, void** ppv )
	{
		if ( !ppv )
			return _OEXCOM_E_INVALIDARG;

		*ppv = oexNULL;

		if ( pUnkOuter )
			return _OEXCOM_E_NOAGGREGATION;

		T *p = _OEXCOM_CreateInstance< T >();
		if ( !p )
			return _OEXCOM_E_OUTOFMEMORY;

		_OEXCOM_HRESULT hr = p->QueryInterface( iid, ppv );
		if ( _OEXCOM_S_OK != hr || !*ppv )
			p->Release();

		return hr;
	}

	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL LockServer( _OEXCOM_BOOL bLock )
	{
		if ( bLock )
			oexInterlockedIncrement( &_g_oex_lRefCount );
		else
			oexInterlockedDecrement( &_g_oex_lRefCount );

		return _OEXCOM_S_OK;
	}

};

//------------------------------------------------------------------
// COM Exported functions
//------------------------------------------------------------------
#define _OEXCOM_BEGIN_FACTORY \
_OEXCOM_STDAPI DllGetClassObject( const _OEXCOM_IID &clsid, const _OEXCOM_IID &iid, void** ppv ) \
{	if ( 0 );

#define _OEXCOM_CREATE_OBJECT( n, c ) \
	else if ( _OEXCOM_CMPGUID( &n, &clsid ) ) \
	{	_OEXCOM_CClassFactory< c > *p = _OEXCOM_CClassFactory< c >::_OEXCOM_CreateInstance< _OEXCOM_CClassFactory< c > >(); \
		if ( !p ) return _OEXCOM_E_OUTOFMEMORY; \
		_OEXCOM_HRESULT hr = p->QueryInterface( iid, ppv ); \
		p->Release(); \
		return hr; \
	}

#define _OEXCOM_END_FACTORY return _OEXCOM_E_NOTIMPL; }
