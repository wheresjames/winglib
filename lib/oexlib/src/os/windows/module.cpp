/*------------------------------------------------------------------
// module.cpp
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

#include "../../../oexlib.h"
#include "std_os.h"

OEX_USING_NAMESPACE
using namespace OEX_NAMESPACE::os;

oexSTATIC_ASSERT( sizeof( oexPVOID ) == sizeof( HMODULE ) );

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CModule::CModule()
{
	m_hModule = oexNULL;
}

CModule::CModule( oexCSTR x_pFile )
{
	CModule();
	Load( x_pFile );
}

CModule::~CModule()
{
	Destroy();
}

oexBOOL CModule::Load( oexCSTR x_pFile, oexINT x_nFlags )
{
	// Do we already have this module loaded?
	if ( oexNULL != m_hModule && x_pFile && m_sFile == x_pFile )
		return oexTRUE;

	// Lose module
	Destroy();

	// Punt if bad filename
	if ( !oexCHECK_PTR( x_pFile ) || !*x_pFile )
		return oexFALSE;

	// Save file name
	m_sFile = x_pFile;

	// Load the module
	m_hModule = ::LoadLibrary( x_pFile );
	if ( m_hModule == oexNULL )
		return oexFALSE;

	return oexTRUE;
}

void CModule::Destroy()
{
	// Lose the pointers
	m_ptrs.Destroy();

	// Free library
	if ( m_hModule )
	{	::FreeLibrary( (HMODULE)m_hModule );
		m_hModule = oexNULL;
	} // end if

}

oexPVOID CModule::AddFunction( oexCSTR x_pFunctionName )
{
	// Sanity check
	if ( x_pFunctionName )
		return oexNULL;

	oexPVOID pf = Addr( x_pFunctionName );
	if ( oexCHECK_PTR( pf ) )
		return pf;

	pf = ::GetProcAddress( (HMODULE)m_hModule, x_pFunctionName );
	if ( !oexCHECK_PTR( pf ) )
		return oexFALSE;

	// Save index
	oexINT index = Size();

	// Re allocate space for pointers
	if ( m_ptrs.Size() <= index )
		m_ptrs.Resize( m_ptrs.Size() * 2 );

	// Save address into table
	m_ptrs[ index ] = pf;

	return pf;
}

oexPVOID CModule::Addr( oexINT i )
{
	if ( i >= Size() )
		return oexNULL;
	return m_ptrs[ i ];
}

oexPVOID CModule::Addr( oexCSTR x_pFunctionName )
{
	if ( !oexCHECK_PTR( x_pFunctionName ) || !*x_pFunctionName )
		return oexNULL;

	if ( !m_map.IsKey( x_pFunctionName ) )
		return oexNULL;

	return m_map.Find( x_pFunctionName ).Obj();
}

