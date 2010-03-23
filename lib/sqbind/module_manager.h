/*------------------------------------------------------------------
// module_manager.h
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

#include "module_instance.h"

class CModuleManager
{
public:

    /// The module list type
	typedef oexStdMap( stdString, CModuleInstance* )		t_ModuleList;

public:

    /// Default constructor
	CModuleManager();

    /// Destructor
	virtual ~CModuleManager();

    /// Unloads all modules
    void Destroy();

public:

    /// Returns non-zero if the module is loaded
	oex::oexBOOL Exists( oex::oexCSTR x_pFile );

    /// Loads the specified module if needed and returns an instance object
	CModuleInstance* Load( oex::oexCSTR x_pFile );

    /// Unloads the specified module
	/**
		\param [in] x_pFile		-	File to unload

		!!! Be careful here, you probably can't unload a module
		    unless your *CERTAIN* that squirrel is done with it.
			For the most part, you won't be able to use this function.
	*/
	oex::oexBOOL Unload( oex::oexCSTR x_pFile );

private:

    /// List of module handles
    t_ModuleList					m_lstModules;

    /// Thread access lock
	oexLock							m_lock;

};
