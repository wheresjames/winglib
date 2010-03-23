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

/// This scheme just helps to ensure that a service function is defined correctly
#define oexDECLARE_SRV_Start( f ) OEX_NAMESPACE::oexRESULT f( OEX_NAMESPACE::SRawAllocator x_sAllocator, OEX_NAMESPACE::oexCSTR x_pPath, OEX_NAMESPACE::oexCSTR x_pCommandLine, OEX_NAMESPACE::oexINT x_nCommandLine, OEX_NAMESPACE::oexCPVOID x_pData )
#define oexDECLARE_SRV_Stop( f ) OEX_NAMESPACE::oexRESULT f()
#define oexDECLARE_SRV_Idle( f ) OEX_NAMESPACE::oexRESULT f()
#define oexDECLARE_SRV_GetModuleInfo( f ) OEX_NAMESPACE::oexRESULT f( OEX_NAMESPACE::os::service::SSrvInfo *pDi )
#define oexDECLARE_SRV_FUNCTION( f ) oexDECLARE_##f( f )


/**
	The functions below define a simple module interface.

*/
namespace service
{
	//==============================================================
	// SRV_Start()
	//==============================================================
	/// Called when module is loaded.  Do all initialization here.
	/**
	 	\param [in] x_pPath			- Path to the module
		\param [in] x_pCommandLine	- Parameters passed on command line
		\param [in] x_nCommandLine  - Bytes of data in x_pCommandLine
		\param [in] x_pData			- User defined data pointer

		This function does not need to be provided.

		\return Zero if module load should be aborted.

		\see
	*/
	typedef oexDECLARE_SRV_Start( (*PFN_SRV_Start) );

	//==============================================================
	// SRV_Stop()
	//==============================================================
	/// Called when module is unloaded.  Do final shutdown here.
	/**
		This function does not need to be provided.

		\return Ignored

		\see
	*/
	typedef oexDECLARE_SRV_Stop( (*PFN_SRV_Stop) );

	//==============================================================
	// SRV_Idle()
	//==============================================================
	/// Called while loader is idling
	/**
		This function does not need to be provided.

		Sleep is called after each call to SRV_Idle().  It
		is not required this function return until program
		termination is desired.

		\return Zero if module should terminate

		\see
	*/
	typedef oexDECLARE_SRV_Idle( (*PFN_SRV_Idle) );

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

		/// Pointer to module's stack trace object
//		CStackTrace			*pSt;
	};

	//==============================================================
	// SRV_GetModuleInfo()
	//==============================================================
	/// Returns information about the module
	/**
		\param [in] pDi		-	Pointer to SDtpInfo structure

		This function must be provided.

		\return Non-zero if success

		\see
	*/
	typedef oexDECLARE_SRV_GetModuleInfo( (*PFN_SRV_GetModuleInfo) );

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
private:

	/// Default constructor
	CService() {}

	/// Destructor
    ~CService() {}

public:

	//==============================================================
	// Run()
	//==============================================================
	/// Starts the module in a seperate process.
	/**
	 	\param [in] x_sModule		- Path to module to fork
		\param [in] x_sCommandLine	- Command line to be processed by
									  the module.
	 	\param [in] x_pData			- User data to pass on to module
	 	\param [in] x_pguidType		- GUID specifying the required type
	 							  	  of the module.  An error will be
	 							  	  returned if the module type does
	 							  	  not match.  Pass NULL if the module
	 							  	  type is to be ignored.
	 	\param [in] x_nIdleDelay	- Delay between calls to the modules
	 								  Idle() function.
	 	\param [in] x_nFlags		- Module load flags.  These flags
	 								  are passed to dlopen().

		Module can optionally provide the functions.

		SRV_GetModuleInfo() 	- Must be provided.  The module should fill
								  in the appropriate fields of the SSrvInfo
								  structure.

		SRV_Start()				- This function is called to initialize the
								  module.  It is not required that this
								  function be provided.  If this function
								  returns zero, the process will exit and
								  SRV_Idle() and SRV_Stop() will not be called.

	    SRV_Idle()				- This function is called repetedly.  This
	    						  function does need not be provided.  Sleep()
	    						  is called after each call to SRV_Idle() with
	    						  the wait value of x_nIdleDelay so long as a
	    						  non-zero value is returned.  If this SRV_Idle()
	    						  returns zero, the process will exit.  It is
	    						  not required that this function return until
	    						  process termination is desired.

	    SRV_Stop()				- This function is called after SRV_Idle()
	    						  returns zero.  Use this function to clean
	    						  up.  This function need not be provided.
	    						  The return value is ignored.

		\see RunModule()

		\return Less than zero if failure, zero if child, greater
				than zero on return from parent.
	*/
	static oexINT Run( CStr x_sModule, CStr x_sCommandLine, oexCPVOID x_pData, oexGUID *x_pguidType, oexINT x_nIdleDelay = 10, oexINT x_nFlags = -1 );

	/// Runs the module in the current process
	/**
		\see Run()
	*/
	static oexINT RunModule( CStr x_sModule, CStr x_sCommandLine, oexCPVOID x_pData, oexGUID *x_pguidType, oexINT x_nIdleDelay = 10, oexINT x_nFlags = -1 );
};

