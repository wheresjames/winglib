
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

	_OEXCOM_BEGIN_INTERFACE_MAP
		_OEXCOM_DECLARE_INTERFACE( IID_IAxTest, CAxTest )
	_OEXCOM_END_INTERFACE_MAP

	virtual int _OEXCOM_STDCALL Add( int a, int b ) { return a + b; }

};

