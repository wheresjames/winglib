
#include "stdafx.h"

extern const _OEXCOM_IID _OEXCOM_IID_IUnknown = 
	oexINITGUID( 0x00000000, 0x0000, 0x0000, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46 );

extern const _OEXCOM_IID _OEXCOM_IID_IClassFactory = 
	oexINITGUID( 0x00000001, 0x0000, 0x0000, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46 );

extern const _OEXCOM_IID _OEXCOM_IID_IDispatch = 
	oexINITGUID( 0x00020400, 0x0000, 0x0000, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46 );

extern oex::oexLONG _g_oex_lRefCount = 0;

_OEXCOM_BOOL _OEXCOM_STDCALL DllMain( _OEXCOM_HANDLE InsModule, _OEXCOM_UINT dwReason, void* lpReserved )
{
	return _OEXCOM_TRUE;
}

_OEXCOM_STDAPI DllCanUnloadNow()
{
	return ( 0 < _g_oex_lRefCount ) ? _OEXCOM_S_FALSE : _OEXCOM_S_OK;
}
