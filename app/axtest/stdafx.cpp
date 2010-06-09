
#include "stdafx.h"

// {50B3ED5F-DF15-4d26-BA57-94EA8F53D145}
extern const _OEXCOM_IID IID_IAxTest = 
	oexINITGUID( 0x50b3ed5f, 0xdf15, 0x4d26, 0xba, 0x57, 0x94, 0xea, 0x8f, 0x53, 0xd1, 0x45 );

/*
_OEXCOM_BEGIN_FACTORY
	_OEXCOM_CREATE_OBJECT( IID_IAxTest, CAxTest )
_OEXCOM_END_FACTORY
*/

_OEXCOM_STDAPI DllGetClassObject( const _OEXCOM_IID &clsid, const _OEXCOM_IID &iid, void** ppv )
{
	if ( 0 );
	
	else if ( _OEXCOM_CMPGUID( &IID_IAxTest, &clsid ) )
	{	
		_OEXCOM_CClassFactory< CAxTest > *p = 
			_OEXCOM_CClassFactory< CAxTest >::_OEXCOM_CreateInstance< _OEXCOM_CClassFactory< CAxTest > >();

		if ( !p ) 
			return _OEXCOM_E_OUTOFMEMORY;

		_OEXCOM_HRESULT hr = p->QueryInterface( iid, ppv );

		p->Release();

		return hr;
	}

	return _OEXCOM_E_NOTIMPL; 
}

_OEXCOM_STDAPI DllRegisterServer()
{
	oex::oexLONG lRes = oex::os::CServiceImpl::RegisterInterface( oexT( "IAxTest" ), IID_IAxTest, 4 );
	if ( _OEXCOM_S_OK != lRes )
		return lRes;

	lRes = oex::os::CServiceImpl::RegisterServer( oexT( "IAxTest.Server" ), IID_IAxTest, 
												  oexT( "Both" ), 
												  oexGetModuleFileName( oexGetInstanceHandle() ).Ptr(),
												  oex::oexTRUE, oexNULL );
	return lRes;
}

_OEXCOM_STDAPI DllUnregisterServer()
{
	oex::oexLONG lRes = oex::os::CServiceImpl::UnregisterInterface( IID_IAxTest );
	if ( _OEXCOM_S_OK != lRes )
		return lRes;

	lRes = oex::os::CServiceImpl::UnregisterServer( oexT( "IAxTest.Server" ), IID_IAxTest );
	return lRes;
}
