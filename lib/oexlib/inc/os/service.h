/*------------------------------------------------------------------
// service.h
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

namespace service
{
	//==============================================================
	// SRV_Start()
	//==============================================================
	/// Called when module is loaded.  Do all initialization here.
	/**
		This function does not have to be provided

		\return Zero if module load should be aborted.

		\see
	*/
	typedef oexBOOL (*PFN_SRV_Start)();

	//==============================================================
	// SRV_Stop()
	//==============================================================
	/// Called when module is unloaded.  Do final shutdown here.
	/**
		This function does not have to be provided

		\return Ignored

		\see
	*/
	typedef oexBOOL (*PFN_SRV_Stop)();


	//==============================================================
	// SRV_Idle()
	//==============================================================
	/// Called while loader is idling
	/**
		This function does not have to be provided

		\return Zero if module should terminate

		\see
	*/
	typedef oexBOOL (*PFN_SRV_Idle)();

	//==============================================================
	// SSrvInfo
	//==============================================================
	/// Module information structure
	/**
	*/
	struct SSrvInfo
	{
		/// Human readable module name
		oexCHAR8			szName[ 256 ];

		/// Human readable module description
		oexCHAR8			szDesc[ 256 ];

		/// Module type
		oexGUID				guidType;

		/// Unique module id
		oexGUID				guidId;

		/// Identifies this current instance
		oexGUID				guidInstance;

		/// Module high version
		oexLONG				lVer;
	};

	//==============================================================
	// SRV_GetModuleInfo()
	//==============================================================
	/// Returns information about the module
	/**
		\param [in] pDi		-	Pointer to SDtpInfo structure

		This function must be provided

		\return Non-zero if success

		\see
	*/
	typedef oexBOOL (*PFN_SRV_GetModuleInfo)( SSrvInfo *pDi );

};

//==============================================================
// CService
//==============================================================
/// Handles system services
/**
	This class provides a platform independent way to start and run
	a shared object as a service.  It must export at least
	the function SRV_GetModuleInfo() and return appropriate
	information.
*/
class CService
{
public:


private:

	/// Default constructor
	CService() {}

	/// Destructor
    ~CService() {}

public:

	/// Service module
	/**
		Starts the module in a forked process.

		\return Less than zero if failure, zero if child, greater
				than zero on return from parent.
	*/
	static oexINT Run( CStr x_sModule );

	/// Fork current process
	/**

		\return Less than zero if failure, zero if child, greater
				than zero on return from parent.
	*/
	static oexINT Fork( CStr x_sWorkingDirectory );


};
