/*------------------------------------------------------------------
// service.cpp
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


oexINT CService::Run( CStr x_sModule, CStr x_sCommandLine, oexCPVOID x_pData, oexGUID *x_pguidType, oexINT x_nIdleDelay, oexINT x_nFlags )
{_STT();
	return RunModule( x_sModule, x_sCommandLine, x_pData, x_pguidType, x_nIdleDelay, x_nFlags );
}

oexINT CService::RunModule( CStr x_sModule, CStr x_sCommandLine, oexCPVOID x_pData, oexGUID *x_pguidType, oexINT x_nIdleDelay, oexINT x_nFlags )
{_STT();
	// Load the module
	CModule mod;
	if ( !mod.Load( x_sModule.Ptr() ) )
	{	oexERROR( 0, oexMks( oexT( "Failed to load module " ), x_sModule ) );
		return -1;
	} // end if

	service::PFN_SRV_GetModuleInfo pGetModuleInfo =
		(service::PFN_SRV_GetModuleInfo)mod.AddFunction( oexT( "SRV_GetModuleInfo" ) );
	if ( !oexCHECK_PTR( pGetModuleInfo ) )
	{	oexERROR( 0, CStr().Fmt( oexT( "Module '%s' does not contain symbol SRV_GetModuleInfo()" ),
					     		 x_sModule.Ptr() ) );
		return -2;
	} // end if

	// Get module information
	service::SSrvInfo si;
	oexZeroMemory( &si, sizeof( si ) );
	if ( oexINT ret = pGetModuleInfo( &si ) )
	{	oexERROR( ret, CStr().Fmt( oexT( "In module '%s', SRV_GetModuleInfo() failed by returning non-zero" ),
				     			 x_sModule.Ptr() ) );
		return -3;
	} // end if

	// Add module stack trace object
//	CStackTrace::CLocalModuleInfo _mi( mod.GetModuleBaseAddress(), si.pSt, x_sModule.GetFileName().Ptr() );

	// Verify correct module type
	if ( oexCHECK_PTR( x_pguidType ) && !guid::CmpGuid( x_pguidType, &si.guidType ) )
	{	oexERROR( 0, CStr().Fmt( oexT( "In module '%s', incorrect module type, %s != %s" ),
								 x_sModule.Ptr(),
								 CStr().GuidToString( x_pguidType ).Ptr(),
								 CStr().GuidToString( &si.guidType ).Ptr() ) );
		return -4;
	} // end if

	// Log information about the module
	oexNOTICE( 0, CStr().Fmt( oexT( "Module Loaded:  '%s'\r\n"
									"Command Line:   '%s'\r\n"
								    "Name:           %s\r\n"
								    "Version:        %d.%d\r\n"
								    "Description:    %s\r\n"
								    "Type:           %s\r\n"
								    "ID:             %s\r\n"
								    "Instance:       %s\r\n" ),
							   x_sModule.Ptr(),
							   x_sCommandLine.Ptr(),
							   oexMbToStrPtr( si.szName ),
							   oexVERSION_MAJOR( si.lVer ), oexVERSION_MINOR( si.lVer ),
							   oexMbToStrPtr( si.szDesc ),
							   CStr().GuidToString( &si.guidType ).Ptr(),
							   CStr().GuidToString( &si.guidId ).Ptr(),
							   CStr().GuidToString( &si.guidInstance ).Ptr() ) );

	// Load start function
	service::PFN_SRV_Start pStart =
		(service::PFN_SRV_Start)mod.AddFunction( oexT( "SRV_Start" ) );
	if ( !oexCHECK_PTR( pStart ) )
	{	oexWARNING( 0, CStr().Fmt( oexT( "Symbol SRV_Start() not found in module '%s'" ),
					   			   oexStrToMbPtr( x_sModule.Ptr() ) ) );
		return 0;
	} // end if

	// Call start function if provided
	else if ( oexINT ret = pStart( CMem::GetRawAllocator(), x_sModule.Ptr(), x_sCommandLine.Ptr(), x_sCommandLine.Length(), x_pData ) )
	{	oexNOTICE( ret, CStr().Fmt( oexT( "Exiting because SRV_Start() returned non-zero in module %s" ),
					   			  oexStrToMbPtr( x_sModule.Ptr() ) ) );
		return -5;
	} // end if

	oexNOTICE( 0, CStr().Fmt( oexT( "Module '%s' started successfully" ),
					       	  x_sModule.Ptr() ) );

	// Load start function
	service::PFN_SRV_Idle pIdle =
		(service::PFN_SRV_Idle)mod.AddFunction( oexT( "SRV_Idle" ) );
	if ( !oexCHECK_PTR( pIdle ) )
		oexWARNING( 0, CStr().Fmt( oexT( "Symbol SRV_Idle() not found in module '%s'" ),
					   			   x_sModule.Ptr() ) );

	// Run idle loop if function provided
	else
		while ( !pIdle() )
			os::CSys::Sleep( x_nIdleDelay );

	// Forever
//	else for( ; ; )
//		os::CSys::Sleep( 60000 );

	// Check for stop function
	service::PFN_SRV_Stop pStop =
		(service::PFN_SRV_Stop)mod.AddFunction( oexT( "SRV_Stop" ) );
	if ( !oexCHECK_PTR( pStop ) )
	{	oexWARNING( 0, CStr().Fmt( oexT( "Symbol SRV_Stop() not found in module '%s'" ),
					   			   oexStrToMbPtr( x_sModule.Ptr() ) ) );
		return 0;
	} // end if

	// Call stop function
	return pStop();
}


