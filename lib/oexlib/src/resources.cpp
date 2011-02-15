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

#include "oexlib.h"

OEX_USING_NAMESPACE

COexResourceHelper::COexResourceHelper()
{_STT();
	m_oexlib_resources_ptr = 0;
}

COexResourceHelper::COexResourceHelper( const _SOexResourceInfo *pRes )
{_STT();
	m_oexlib_resources_ptr = pRes;
}

oexBOOL COexResourceHelper::IsResources()
{_STT();
	return oexNULL != m_oexlib_resources_ptr;
}

void COexResourceHelper::SetResourcePtr( const _SOexResourceInfo *pRes )
{_STT();
	m_oexlib_resources_ptr = pRes;
}


CStr8 COexResourceHelper::GetResource( CStr sName )
{_STT();
	long l = 0;
	const void *p = oexNULL;

	if ( GetResource( sName, &p, &l ) )
		return CStr8();

	return oex::zip::CUncompress::Uncompress( (oexCHAR8*)p, l );
}

oexBOOL COexResourceHelper::GetResource( CStr sName, CStr8 *pRes )
{_STT();
	if ( !pRes )
		return oexFALSE;

	long l = 0;
	const void *p = oexNULL;

	if ( GetResource( sName, &p, &l ) )
		return oexFALSE;

	*pRes = oex::zip::CUncompress::Uncompress( (oexCHAR8*)p, l );

	return oexTRUE;
}

CStr8 COexResourceHelper::GetResource( oexLONG i )
{_STT();
	long l = 0;
	const void *p = oexNULL;

	if ( GetResource( i, &p, &l ) )
		return CStr8();

	return oex::zip::CUncompress::Uncompress( (oexCHAR8*)p, l );
}

oexLONG COexResourceHelper::GetResource( CStr sName, const void ** p, oexLONG *l )
{_STT();
	if ( !m_oexlib_resources_ptr )
		return -1;

	// Search for the item
	CStr8 sName8 = oexStrToMb( sName );
	for ( int i = 0; m_oexlib_resources_ptr[ i ].name; i++ )
		if ( sName8 == m_oexlib_resources_ptr[ i ].name )
		{	*p = m_oexlib_resources_ptr[ i ].data;
			*l = m_oexlib_resources_ptr[ i ].size;
			return 0;
		} // end if

	return -1;
}

oexLONG COexResourceHelper::GetResource( oexLONG i, const void ** p, oexLONG *l )
{_STT();
	if ( !m_oexlib_resources_ptr )
		return -1;

	if ( 0 > i || GetResourceCount() <= i )
		return -1;

	// Save resource info
	*p = m_oexlib_resources_ptr[ i ].data;
	*l = m_oexlib_resources_ptr[ i ].size;

	return 0;
}

oexLONG COexResourceHelper::GetResourceSize( oexLONG i )
{_STT();
	if ( !m_oexlib_resources_ptr )
		return -1;

	if ( 0 > i || GetResourceCount() <= i )
		return -1;

	return m_oexlib_resources_ptr[ i ].size;
}

oexCPVOID COexResourceHelper::GetResourcePtr( oexLONG i )
{_STT();
	if ( !m_oexlib_resources_ptr )
		return 0;

	if ( 0 > i || GetResourceCount() <= i )
		return 0;

	return m_oexlib_resources_ptr[ i ].data;
}

CStr COexResourceHelper::GetResourceName( oexLONG i )
{_STT();
	if ( !m_oexlib_resources_ptr )
		return CStr();

	if ( 0 > i || GetResourceCount() <= i )
		return CStr();

	return oexMbToStrPtr( m_oexlib_resources_ptr[ i ].name );
}

oexLONG COexResourceHelper::GetResourceCount()
{_STT();
	if ( !m_oexlib_resources_ptr )
		return 0;

	oexLONG i = 0;
	while ( m_oexlib_resources_ptr[ i ].name )
		i++;

	return i;
}

