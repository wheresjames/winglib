/*------------------------------------------------------------------
// module_instance.h
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

#pragma once

class CModuleInstance
{
public:

    /// Initializes the module instance
    CModuleInstance()
	{
		m_fGetId = NULL;
		m_fExport = NULL;
	}

    /// Destructor
    virtual ~CModuleInstance()
	{	Destroy(); }
    
public:

    /// Unloads the modules and frees all resources
    void Destroy()
	{
		// Lose the function pointers
		m_fGetId = NULL;
		m_fExport = NULL;

		// Lose the module
		m_cModule.Destroy();
	}

    /// Loads the specified module
    /**
        \param [in] x_pFile   -   File name of module to load
    */
	oex::oexBOOL Load( oex::oexCSTR x_pFile )
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
 
    /// Loads exported function pointers
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
		m_fExport = 
			(sqbind::PFN_SQBIND_Export)m_cModule.AddFunction( oexT( "SQBIND_Export" ) );
		if ( !m_fExport )
		{	oexERROR( 0, oex::CStr().Fmt( oexT( "In module '%s', SQBIND_Export() not found" ),
				     			          oexStrToMbPtr( m_cModule.GetPath().Ptr() ) ) );
			return oex::oexFALSE;
		} // end if

		return TRUE;
	}

    /// Returns non-zero if non NULL module handle
    oex::oexBOOL IsLoaded() { return m_cModule.IsLoaded(); }

    /// Exports module functionality to the VM
    oex::oexBOOL Export( SquirrelVM *pVm )
	{
		// Ensure we have an export function
		if ( !m_fExport )
			return oex::oexFALSE;

		// Attempt to export the functionality
		oex::oexINT nRet = m_fExport( pVm, &malloc );

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

private:

    /// Module class
	oex::os::CModule							m_cModule;

    /// GetId() function pointer    
	oex::os::service::PFN_SRV_GetModuleInfo		m_fGetId;

    /// Export() function pointer
    sqbind::PFN_SQBIND_Export					m_fExport;

};
