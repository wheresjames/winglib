/*------------------------------------------------------------------
// md5.cpp
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

#include "oexlib.h"

// Include md5 functions
#include "../oss/md5/md5rsa.cpp"

OEX_USING_NAMESPACE
using namespace OEX_NAMESPACE::oss;

oexSTATIC_ASSERT( sizeof( oexGUID ) == sizeof( oexUCHAR[ 16 ] ) );
oexSTATIC_ASSERT( sizeof( CMd5::SContext ) == sizeof( MD5_CTX ) );

void CMd5::Init()
{
    CMd5Rsa::MD5Init( (MD5_CTX*)&m_md5Context );
}

void CMd5::Update( oexCPVOID x_pBuf, oexUINT x_uSize )
{
	CMd5Rsa::MD5Update( (MD5_CTX*)&m_md5Context, (oexUCHAR*)x_pBuf, x_uSize );
}

void CMd5::Final( oexGUID *x_pGuid )
{
	CMd5Rsa::MD5Final( m_md5.ucBuf, (MD5_CTX*)&m_md5Context );
    if ( x_pGuid ) guid::CopyGuid( x_pGuid, &m_md5.guid );
}

/// Calculates an md5 in one step
oexGUID* CMd5::Transform( oexGUID *x_pGuid, oexCPVOID x_pBuf, oexUINT x_uSize )
{
    SContext m_md5Context;
    CMd5Rsa::MD5Init( (MD5_CTX*)&m_md5Context );
	CMd5Rsa::MD5Update( (MD5_CTX*)&m_md5Context, (oexUCHAR*)x_pBuf, x_uSize );
	CMd5Rsa::MD5Final( x_pGuid->ucBuf, (MD5_CTX*)&m_md5Context );

    return x_pGuid;
}



