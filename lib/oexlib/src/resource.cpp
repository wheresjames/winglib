/*------------------------------------------------------------------
// resource.cpp
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

const CResource::t_HANDLE CResource::c_Invalid = (oexPVOID)0xffffffff;

const oexUINT CResource::c_Infinite = 0xffffffff;

oexINT CResource::Init( oexINT x_nType )
{
	// Lose any existing resource
	Destroy();

	oexINT nErr = -1;

	// Execute proper Init function if valid handle
	if ( c_Invalid != m_hHandle )
		switch( m_nType )
	{
		case eRtInvalid :
			break;

		case eRtFile :
			break;

		case eRtSocket :
			break;

		case eRtThread :
			break;

		case eRtMutex :
			break;

		default :
			oexERROR( -1, oexT( "Attempt to release unknown resource type" ) );
			break;

	} // end switch

	return nErr;
}


void CResource::Destroy( oexINT *x_pnErr )
{
	// Error code
	oexINT nErr = -1;

	// Execute proper release function if valid handle
	if ( c_Invalid != m_hHandle )
		switch( m_nType )
	{
		case eRtInvalid :
			break;

		case eRtFile :
			os::CBaseFile::Close( m_hHandle, &nErr );
			break;

		case eRtSocket :
			break;

		case eRtThread :
			break;

		case eRtMutex :
			break;

		default :
			oexERROR( -1, oexT( "Attempt to release unknown resource type" ) );
			break;

	} // end switch

	// Clear handle info
	m_hHandle = c_Invalid;
	m_nType = eRtInvalid;

	// Does the caller want the error code?
	if ( x_pnErr && oexCHECK_PTR( x_pnErr ) )
		*x_pnErr = nErr;
}

oexINT CResource::Wait( oexUINT x_uTimeout, oexINT x_nType )
{
	// Error code
	oexUINT uErr = -1;

	// Execute proper release function if valid handle
	if ( c_Invalid != m_hHandle )
		switch( m_nType )
	{
		case eRtInvalid :
			break;

		case eRtFile :
			break;

		case eRtSocket :
			break;

		case eRtThread :
			break;

		case eRtMutex :
			break;

		default :
			oexERROR( -1, oexT( "Attempt to wait on unknown resource type" ) );
			break;

	} // end switch

	return uErr;
}
