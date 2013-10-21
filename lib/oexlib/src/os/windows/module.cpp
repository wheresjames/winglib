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

#include "oexlib.h"
#include "std_os.h"

OEX_USING_NAMESPACE
using namespace OEX_NAMESPACE::os;

oexSTATIC_ASSERT( sizeof( oexPVOID ) == sizeof( HMODULE ) );

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CModule::CModule()
{_STT();
	m_hModule = oexNULL;
}

CModule::CModule( oexCSTR x_pFile )
{_STT();
	CModule();
	Load( x_pFile );
}

CModule::~CModule()
{_STT();
	Destroy();
}

oexBOOL CModule::Load( oexCSTR x_pFile, oexINT x_nFlags )
{_STT();
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


#if defined( OEX_WINCE )

	// On Windows CE 6.1, we have to ensure the library is loaded in slot zero
	// by setting the following key...
	// HKEY_LOCAL_MACHINE\System\Loader\LoadModuleLow\<xxx.dll> = dword:1

	// Open the key
	HKEY hKey = oexNULL;
	if ( RegOpenKeyEx( HKEY_LOCAL_MACHINE, oexT( "System\\Loader\\LoadModuleLow" ),
					   0, KEY_ALL_ACCESS, &hKey ) == ERROR_SUCCESS )
	{
		// Add key if it does not already exist
		oex::CStr sKey = m_sFile.GetFileName();
		if ( ERROR_SUCCESS != RegQueryValueEx( hKey, sKey.Ptr(), 0, 0, 0, 0 ) )
		{
			DWORD dwVal = 1;
			if ( ERROR_SUCCESS != RegSetValueEx( hKey, sKey.Ptr(), 0, REG_DWORD, (LPBYTE)&dwVal, sizeof( dwVal ) ) )
				oexWARNING( GetLastError(), oexMks( oexT( "Error applying Windows CE 6.1+ reg hack for dll " ), x_pFile ) );

		} // end if

		// Close the key
		RegCloseKey( hKey );

	} // end if

	// If key doesn't exist, assume it's not Windows CE 6.1+
//	else
//		oexWARNING( GetLastError(), oexMks( oexT( "Could not apply Windows CE 6.1+ reg hack for dll " ), x_pFile ) );

#endif

	// LoadLibrary is sometimes stubborn about forward slashes
	CStr sPath = CStr( x_pFile ).Replace( oexT( '/' ), oexT( '\\' ) );

	// Load the module
	m_hModule = ::LoadLibrary( sPath.Ptr() );
	if ( m_hModule == oexNULL )
	{	oexERROR( GetLastError(), oexMks( oexT( "Failed to load module " ), sPath.Ptr() ) );
		return oexFALSE;
	} // end if

	return oexTRUE;
}

void CModule::Destroy()
{_STT();
	// Lose the pointers
	m_ptrs.Destroy();

	// Free library
	if ( m_hModule )
	{	::FreeLibrary( (HMODULE)m_hModule );
		m_hModule = oexNULL;
	} // end if

}

oexPVOID CModule::AddFunction( oexCSTR x_pFunctionName )
{_STT();
	// Sanity check
	if ( !oexCHECK_PTR( x_pFunctionName ) || !*x_pFunctionName )
	{	oexERROR( ERROR_INVALID_PARAMETER, oexT( "Invalid function argument" ) );
		return oexNULL;
	} // end if

	if ( !m_hModule )
	{	oexERROR( ERROR_INVALID_PARAMETER, oexT( "Module not loaded" ) );
		return oexNULL;
	} // end if

	oexPVOID pf = Addr( x_pFunctionName );
	if ( oexCHECK_PTR( pf ) )
		return pf;

	pf = (oexPVOID)::GetProcAddress( (HMODULE)m_hModule, oexStrToMbPtr( x_pFunctionName ) );
	if ( !oexCHECK_PTR( pf ) )
	{	oexWARNING( GetLastError(), oexMks( oexT( "GetProcAddress() failed to load function : " ), x_pFunctionName ) );
		return oexFALSE;
	} // end if

	// Save index
	oexINT index = m_map.Size();

	// Re allocate space for pointers
	oexUINT uSize = m_ptrs.Size();
	if ( uSize <= (oexUINT)index )
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
{_STT();
	if ( i >= Size() )
	{	oexERROR( ERROR_INVALID_PARAMETER, CStr().Fmt( oexT( "Invalid function index : %d" ), i ) );
		return oexNULL;
	} // end if

	return m_ptrs[ i ];
}

oexPVOID CModule::Addr( oexCSTR x_pFunctionName )
{_STT();
	if ( !oexCHECK_PTR( x_pFunctionName ) || !*x_pFunctionName )
	{	oexERROR( ERROR_INVALID_PARAMETER, oexT( "Invalid function name pointer" ) );
		return oexNULL;
	} // end if

	if ( !m_map.IsKey( x_pFunctionName ) )
		return oexNULL;

	return m_map.Find( x_pFunctionName ).Obj();
}

