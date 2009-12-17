
// sq_vmime.cpp

#include "stdafx.h"

CSqVMime::CSqVMime()
{
	vmime::ref < vmime::net::session > m_session
		= vmime::create < vmime::net::session >();
}

CSqVMime::~CSqVMime()
{
	Destroy();
}

void CSqVMime::Destroy()
{
}

