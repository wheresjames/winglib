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

	class COleObject : public _OEXCOM_COleObject
	{
		virtual _OEXCOM_ULONG _OEXCOM_STDCALL QueryInterface( oexCONST _OEXCOM_IID& iid,void** ppv )
		{	if ( !ppv ) return _OEXCOM_E_INVALIDARG;
			return _OEXCOM_COleObject::QueryInterface( iid, ppv );
		}

	}; COleObject *m_pCOleObject;

public:

	CAxTest()
	{
		m_pCOleObject = 0;
	}

	_OEXCOM_BEGIN_INTERFACE_MAP
		_OEXCOM_DECLARE_INTERFACE( IID_IAxTest, CAxTest )
		_OEXCOM_DECLARE_INTERFACE_OBJECT( _OEXCOM_IID_IOleObject, COleObject, m_pCOleObject )
	_OEXCOM_END_INTERFACE_MAP

	virtual int _OEXCOM_STDCALL Add( int a, int b );

private:

};

