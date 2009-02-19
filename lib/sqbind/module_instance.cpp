/*------------------------------------------------------------------
// module_instance.cpp
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

#include "stdafx.h"

using namespace sqbind;

CModuleInstance::CModuleInstance()
{
	m_fGetId = oexNULL;
	m_fExportSymbols = oexNULL;
}

CModuleInstance::~CModuleInstance()
{	Destroy(); }

void CModuleInstance::Destroy()
{
	// Lose the function pointers
	m_fGetId = oexNULL;
	m_fExportSymbols = oexNULL;

	// Lose the module
	m_cModule.Destroy();
}

oex::oexBOOL CModuleInstance::Load( oex::oexCSTR x_pFile )
{
	// Lose the old module
	Destroy();

	// Ensure file name
	if ( !oexCHECK_PTR( x_pFile ) || !*x_pFile )
		return oex::oexFALSE;

	// Load the module
	if ( !m_cModule.Load( x_pFile ) )
		return oex::oexFALSE;

	// Attempt to load function pointers
	if ( !LoadFunctions() )
	{	Destroy();
		return oex::oexFALSE;
	} // end if

	return oex::oexTRUE;
}

oex::oexBOOL CModuleInstance::LoadFunctions()
{
	// Ensure module
	if ( !IsLoaded() )
		return oex::oexFALSE;

	// Get id function address
	m_fGetId =
		(oex::os::service::PFN_SRV_GetModuleInfo)m_cModule.AddFunction( oexT( "SRV_GetModuleInfo" ) );
	if ( !m_fGetId )
	{	oexERROR( 0, oex::CStr().Fmt( oexT( "In module '%s', SRV_GetModuleInfo() not found" ),
			     				      oexStrToMbPtr( m_cModule.GetPath().Ptr() ) ) );
		return oex::oexFALSE;
	} // end if

	oex::os::service::SSrvInfo si;
	oexZeroMemory( &si, sizeof( si ) );
	if ( oex::oexINT ret = m_fGetId( &si ) )
	{	oexERROR( ret, oex::CStr().Fmt( oexT( "In module '%s', SRV_GetModuleInfo() failed by returning non-zero" ),
			     				        oexStrToMbPtr( m_cModule.GetPath().Ptr() ) ) );
		return oex::oexFALSE;
	} // end if

	// Verify squirrel module
	if ( !oex::guid::CmpGuid( &sqbind::SQBIND_MODULE_IID, &si.guidType ) )
	{	oexERROR( 0, oex::CStr().Fmt( oexT( "In module '%s', Wrong id for a squirrel module, %s != %s" ),
									  oexStrToMbPtr( m_cModule.GetPath().Ptr() ),
									  oexStrToMbPtr( oexGuidToString( &sqbind::SQBIND_MODULE_IID ).Ptr() ),
									  oexStrToMbPtr( oexGuidToString( &si.guidType ).Ptr() ) ) );
		return oex::oexFALSE;
	} // end if

	// Get export function
	m_fExportSymbols =
		(sqbind::PFN_SQBIND_Export_Symbols)m_cModule.AddFunction( oexT( "SQBIND_Export_Symbols" ) );
	if ( !m_fExportSymbols )
	{	oexERROR( 0, oex::CStr().Fmt( oexT( "In module '%s', SQBIND_Export_Symbols() not found" ),
			     			          oexStrToMbPtr( m_cModule.GetPath().Ptr() ) ) );
		return oex::oexFALSE;
	} // end if

	return TRUE;
}

oex::oexBOOL CModuleInstance::IsLoaded() 
{ return m_cModule.IsLoaded(); }

oex::oexBOOL CModuleInstance::Export( SquirrelVM *pVm )
{
	// Ensure we have an export function
	if ( !m_fExportSymbols )
		return oex::oexFALSE;

	// Attempt to export the functionality
	sqbind::SSqAllocator sa = { &malloc, &realloc, &free };
	oex::oexINT nRet = m_fExportSymbols( pVm, &sa );

	if ( nRet == -1 )
	{	oexERROR( 0, oex::CStr().Fmt( oexT( "In module '%s', SQBIND_Export() failed because heaps do not match, use shared linking" ),
			     				        oexStrToMbPtr( m_cModule.GetPath().Ptr() ) ) );
		return oex::oexFALSE;
	} // end if

	else if ( nRet )
	{	oexERROR( nRet, oex::CStr().Fmt( oexT( "In module '%s', SQBIND_Export() failed" ),
			     				        oexStrToMbPtr( m_cModule.GetPath().Ptr() ) ) );
		return oex::oexFALSE;
	} // end if

	return oex::oexTRUE;
}
