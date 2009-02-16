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

oexSTATIC_ASSERT( sizeof( oexPVOID ) == sizeof( void* ) );

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
	{	oexERROR( EINVAL, "Invalid function argument" );
		return oexFALSE;
	} // end if

	// Save file name
	m_sFile = x_pFile;

	if ( -1 == x_nFlags )
		x_nFlags = RTLD_LAZY | RTLD_GLOBAL;
//		x_nFlags = RTLD_NOW;

	// We may need to open the log file again
	CStr sLogFile = CLog::GlobalLog().GetPath();

	// Load the module
	_oexTRY
	{
		// Attempt to open the dynamic library
		m_hModule = dlopen( x_pFile, x_nFlags );

	} // end try
	_oexCATCH( ... )
	{	oexERROR( errno, CStr().Fmt( "Exception!!! dlopen( '%s' )\r\n: %s", oexStrToMbPtr( x_pFile ), dlerror() ) );
		Destroy();
		return oexFALSE;
	} // end catch

	// The module we loaded could possible screw up our logging
	if ( !CLog::GlobalLog().IsOpen() )
		CLog::GlobalLog().Resume( sLogFile.Ptr() );

	if ( oexNULL == m_hModule )
	{	oexERROR( errno, CStr().Fmt( "dlopen( '%s' )\r\n: %s", oexStrToMbPtr( x_pFile ), dlerror() ) );
		Destroy();
		return oexFALSE;
	} // end if

	return oexTRUE;
}

void CModule::Destroy()
{
	// Lose the pointers
	m_ptrs.Destroy();
	m_map.Destroy();

	// Free library
	if ( m_hModule )
	{	dlclose( m_hModule );
		m_hModule = oexNULL;
	} // end if
}

oexPVOID CModule::AddFunction( oexCSTR x_pFunctionName )
{
	// Sanity check
	if ( !oexCHECK_PTR( x_pFunctionName ) || !*x_pFunctionName )
	{	oexERROR( EINVAL, "Invalid function argument" );
		return oexNULL;
	} // end if

	if ( !m_hModule )
	{	oexERROR( EINVAL, "Module not loaded" );
		return oexNULL;
	} // end if

	oexPVOID pf = Addr( x_pFunctionName );
	if ( oexCHECK_PTR( pf ) )
		return pf;

	_oexTRY
	{
		// Attempt to find function entry point
		pf = dlsym( m_hModule, x_pFunctionName );
		if ( !oexCHECK_PTR( pf ) )
		{	oexERROR( errno, CStr().Fmt( "dlsym( %d, '%s' )", (int)m_hModule, oexStrToMbPtr( x_pFunctionName ) ) );
			return oexNULL;
		} // end if
	} // end try

	_oexCATCH( ... )
	{	oexERROR( errno, CStr().Fmt( "Exception!!! dlsym( '%s', %s )\r\n: %s", oexStrToMbPtr( m_sFile.Ptr() ), oexStrToMbPtr( x_pFunctionName ), dlerror() ) );
		return oexFALSE;
	} // end catch

	// Save index
	oexINT index = m_map.Size();

	// Re allocate space for pointers
	oexUINT uSize = m_ptrs.Size();
	if ( uSize <= index )
	{
		// What size to use?
		uSize = index;
		if ( 8 > uSize ) uSize = 8;
		uSize = cmn::NextPower2( uSize + 1 );

		// Resize the pointer array
		if ( !m_ptrs.OexResize( uSize ).Ptr() )
			return oexNULL;

	} // end if

	// Save address into table
	m_ptrs[ index ] = pf;

	return pf;
}

oexPVOID CModule::Addr( oexINT i )
{
	if ( i >= Size() )
	{	oexERROR( EINVAL, CStr().Fmt( oexT( "Invalid function index : %d" ), i ) );
		return oexNULL;
	} // end if

	return m_ptrs[ i ];
}

oexPVOID CModule::Addr( oexCSTR x_pFunctionName )
{
	if ( !oexCHECK_PTR( x_pFunctionName ) || !*x_pFunctionName )
	{	oexERROR( EINVAL, "Invalid function name pointer" );
		return oexNULL;
	} // end if

	if ( !m_map.IsKey( x_pFunctionName ) )
		return oexNULL;

	return m_map.Find( x_pFunctionName ).Obj();
}


