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
    CModuleInstance();

    /// Destructor
    virtual ~CModuleInstance();

public:

    /// Unloads the modules and frees all resources
    void Destroy();

    /// Loads the specified module
    /**
        \param [in] x_pFile   -   File name of module to load
    */
	oex::oexBOOL Load( oex::oexCSTR x_pFile );

    /// Loads exported function pointers
    oex::oexBOOL LoadFunctions();

    /// Returns non-zero if non NULL module handle
    oex::oexBOOL IsLoaded();

    /// Exports module functionality to the VM
    oex::oexBOOL Export( SquirrelVM *pVm );

private:

    /// Module class
	oex::os::CModule							m_cModule;

    /// GetId() function pointer
	oex::os::service::PFN_SRV_GetModuleInfo		m_fGetId;

    /// Export() function pointer
    sqbind::PFN_SQBIND_Export_Symbols			m_fExportSymbols;

};
