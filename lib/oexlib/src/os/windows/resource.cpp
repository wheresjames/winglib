/*------------------------------------------------------------------
// resource.cpp
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

#include <objbase.h>

OEX_USING_NAMESPACE
using namespace OEX_NAMESPACE::os;

const CResource::t_HANDLE CResource::c_Invalid = (oexPVOID)0xffffffff;

const oexUINT CResource::c_Infinite = 0xffffffff;

namespace OEX_NAMESPACE
{
	namespace os
	{
		/// Contains OS specific information about a resource
		struct SResourceInfo
		{
			// Resource handle
			HANDLE								hHandle;

			// Owner
			oexUINT								uOwner;

			// Thread info
			oexUINT								uThreadId;
			oexPVOID							pData;
			CResource::PFN_ThreadProc			fnCallback;

			// Lock info
			oexINT								nCount;
		};
	};
};


//==============================================================
// CThread::CThreadProcImpl
//==============================================================
/// This is just a function stub that calls CThread::ThreadProc()
/**
	\param [in] x_pData		-	CThread class pointer

    I went ahead and left the signature as DWORD(*)(LPVOID);
    This means I had to do this stub since DWORD and LPVOID
    aren't in the header name space.

	\return Thread return value
*/
class CResource::CThreadProcImpl
{
public:
    static DWORD WINAPI ThreadProc( LPVOID x_pData )
    {
		// Do the thread stuff
		return (DWORD)CResource::ThreadProc( x_pData );
	}
};


static SResourceInfo* CreateRi()
{
	SResourceInfo *pRi = OexAllocConstruct< SResourceInfo >();
	if ( !pRi )
		return oexNULL;

	// Add a reference count for deletion
	if ( 2 != CAlloc::AddRef( pRi ) )
		return oexNULL;

	pRi->uOwner = 0;
	pRi->hHandle = INVALID_HANDLE_VALUE;
	pRi->uThreadId = 0;
	pRi->pData = 0;
	pRi->fnCallback = 0;
	pRi->nCount = 0;

	return pRi;
}

static oexINT FreeRi( SResourceInfo* x_pRi, oexINT x_eType, oexUINT x_uTimeout, oexBOOL x_bForce )
{
	// Any resources to release?
	if ( CResource::cInvalid() == x_pRi || CResource::eRtInvalid == x_eType )
		return -1;

	// Get pointer to resource information
	if ( !oexCHECK_PTR( x_pRi ) )
		return -1;

	// Check for other references
	oexINT nRef;
	if ( 1 != ( nRef = OexAllocDelete( x_pRi ) ) )
		return nRef;

	// Error code
	oexINT nErr = 0;

	// Execute proper release function if valid handle
	switch( x_eType )
	{
		case CResource::eRtInvalid :
			break;

		case CResource::eRtFile :
			os::CBaseFile::Close( x_pRi->hHandle, &nErr );
			break;

		case CResource::eRtSocket :
			break;

		case CResource::eRtThread :

			// Wait to see if the thread will shutdown on it's own
			if ( WAIT_OBJECT_0 != WaitForSingleObject( x_pRi->hHandle, x_uTimeout ) )
			{
				if ( x_bForce )
				{	oexERROR( GetLastError(), oexT( "!!! Thread failed to exit gracefully, Calling TerminateThread() !!!" ) );
					TerminateThread( x_pRi->hHandle, oexFALSE );
				} // end if
				else
					oexERROR( GetLastError(), oexT( "!!! Thread failed to exit gracefully, it is being abandonded !!!" ) );

			} // end if

		case CResource::eRtMutex :
		case CResource::eRtEvent :
		case CResource::eRtLock :
			CloseHandle( x_pRi->hHandle );
			break;

		default :
			oexERROR( -1, oexT( "Attempt to release unknown resource type" ) );
			break;

	} // end switch

	// Drop the memory for real
	OexAllocDelete( x_pRi );

	return 0;
}

oexINT CResource::AddRef() const
{
	if ( CResource::cInvalid() == m_hHandle || !oexCHECK_PTR( m_hHandle ) )
		return -1;
	return CAlloc::AddRef( m_hHandle ) - 1;
}


oexRESULT CResource::Destroy( oexUINT x_uTimeout, oexBOOL x_bForce )
{
	// Ensure reasonable values
	if ( CResource::cInvalid() == m_hHandle || CResource::eRtInvalid == m_eType )
	{	m_hHandle = c_Invalid;
		m_eType = eRtInvalid;
		return -1;
	} // end if

	// Get destroy info
	SResourceInfo* pRi = (SResourceInfo*)m_hHandle;
	E_RESOURCE_TYPE eType = m_eType;

	// Good practice and all...
	m_hHandle = c_Invalid;
	m_eType = eRtInvalid;

	// Free the resource
	if ( 0 > FreeRi( pRi, eType, x_uTimeout, x_bForce ) )
	{	oexERROR( 0, oexT( "Invalid resource info object pointer" ) );
		return -1;
	} // end if

	return 0;
}

oexUINT CResource::GetOwner()
{
	SResourceInfo *pRi = (SResourceInfo*)m_hHandle;
	if ( !oexCHECK_PTR( pRi ) )
		return 0;

	return pRi->uOwner;
}

oexRESULT CResource::NewEvent( oexCSTR x_sName, oexBOOL x_bManualReset, oexBOOL x_bInitialState )
{
	// Out with the old
	Destroy();

	SResourceInfo *pRi = CreateRi();
	if ( !pRi )
		return oexERROR( ERROR_OUTOFMEMORY, oexT( "Out of memory" ) );

	// Initialize member variables
	m_eType = eRtEvent;
	m_hHandle = (CResource::t_HANDLE)pRi;

	// Create the event
	pRi->hHandle = CreateEvent( NULL, x_bManualReset, x_bInitialState, x_sName );
	if ( c_Invalid == pRi->hHandle )
		return -1;

	return 0;
}

oexRESULT CResource::NewMutex( oexCSTR x_sName, oexBOOL x_bInitialOwner )
{
	// Out with the old
	Destroy();

	SResourceInfo *pRi = CreateRi();
	if ( !pRi )
		return oexERROR( ERROR_OUTOFMEMORY, oexT( "Out of memory" ) );

	// Save information
	m_eType = eRtMutex;
	m_hHandle = (CResource::t_HANDLE)pRi;

	// Create the mutex
	pRi->hHandle = CreateMutex( NULL, x_bInitialOwner, x_sName );
	if ( c_Invalid == pRi->hHandle )
		return -1;

	return 0;
}

oexRESULT CResource::NewThread( PFN_ThreadProc x_fnCallback, oexPVOID x_pData )
{_STT();
	// Out with the old
	Destroy();

	SResourceInfo *pRi = CreateRi();
	if ( !pRi )
		return oexERROR( ERROR_OUTOFMEMORY, oexT( "Out of memory" ) );

	m_eType = eRtThread;
	m_hHandle = (CResource::t_HANDLE)pRi;

	// Save user data
	pRi->pData = x_pData;
	pRi->fnCallback = x_fnCallback;
	pRi->uOwner = oexGetCurThreadId();

	// Create a thread
	pRi->hHandle = CreateThread(	(LPSECURITY_ATTRIBUTES)NULL,
									0,
									CThreadProcImpl::ThreadProc,
									(LPVOID)pRi,
									0,
									(LPDWORD)&pRi->uThreadId );

	// Seems to be confusion about what this function returns on error
	if ( NULL == pRi->hHandle || INVALID_HANDLE_VALUE == pRi->hHandle )
	{	Destroy(); return -1; }

	return 0;
}

oexPVOID CResource::ThreadProc( oexPVOID x_pData )
{
	// Get pointer to resource information
	SResourceInfo *pRi = (SResourceInfo*)x_pData;
	if ( !oexCHECK_PTR( pRi ) )
	{	oexERROR( 0, oexT( "Invalid resource info object pointer" ) );
		return (oexPVOID)-1;
	} // end if

	// Initialize COM
#if defined( OEX_WINCE )
	CoInitializeEx( NULL, COINIT_MULTITHREADED );
#else
	CoInitialize( NULL );
#endif

	// Call user thread
	oexPVOID pRet = pRi->fnCallback( pRi->pData );

	// Remove the stack trace for this thread
	oexSt().RemoveThread();

	// Uninitialize COM
	CoUninitialize();

	return pRet;
}

oexRESULT CResource::NewLock( oexCSTR x_sName, oexBOOL x_bInitialOwner )
{
	// Out with the old
	Destroy();

	SResourceInfo *pRi = CreateRi();
	if ( !pRi )
		return oexERROR( ERROR_OUTOFMEMORY, oexT( "Out of memory" ) );

	// Initialize member variables
	m_eType = eRtLock;
	m_hHandle = (CResource::t_HANDLE)pRi;

	// Initialize structure
	pRi->nCount = 0;
	pRi->uOwner = 0;

	// Create the event
	pRi->hHandle = CreateMutex( NULL, x_bInitialOwner, x_sName );
	if ( c_Invalid == pRi->hHandle )
		return -1;

	return 0;
}

oexRESULT CResource::Wait( oexUINT x_uTimeout )
{
	// Verify information
	if ( cInvalid() == m_hHandle || eRtInvalid == m_eType )
		return -1;

	// Get pointer to resource information
	SResourceInfo *pRi = (SResourceInfo*)m_hHandle;
	if ( !oexCHECK_PTR( pRi ) )
	{	oexERROR( 0, oexT( "Invalid resource info object pointer" ) );
		return -1;
	} // end if

	// Execute proper release function if valid handle
	switch( m_eType )
	{
		case eRtInvalid :
			break;

		case eRtFile :
			break;

		case eRtSocket :
			break;

		case eRtThread :
		case eRtEvent :
			return ( WAIT_OBJECT_0 == WaitForSingleObject( pRi->hHandle, x_uTimeout ) ) ? 0 : -1;

		case eRtMutex :
		case eRtLock :
			if ( WAIT_OBJECT_0 == WaitForSingleObject( pRi->hHandle, x_uTimeout ) )
			{	pRi->uOwner = oexGetCurThreadId();
				return waitSuccess;
			} // end if

			return waitTimeout;
			break;

		default :
			oexERROR( 0, oexT( "Attempt to wait on unknown resource type" ) );
			break;

	} // end switch

	return 0;
}

oexRESULT CResource::Signal( oexUINT x_uTimeout )
{
	// Verify information
	if ( cInvalid() == m_hHandle || eRtInvalid == m_eType )
		return -1;

	// Get pointer to resource information
	SResourceInfo *pRi = (SResourceInfo*)m_hHandle;
	if ( !oexCHECK_PTR( pRi ) )
	{	oexERROR( 0, oexT( "Invalid resource info object pointer" ) );
		return -1;
	} // end if

	// Execute proper release function if valid handle
	if ( cInvalid() != m_hHandle )
		switch( m_eType )
	{
		case eRtInvalid :
			break;

		case eRtFile :
			break;

		case eRtSocket :
			break;

		case eRtThread :
			break;

		case eRtMutex :
		case eRtLock :
			return Wait( x_uTimeout );
			break;

		case eRtEvent :
			return SetEvent( pRi->hHandle ) ? 0 : -1;
			break;

		default :
			oexERROR( -1, oexT( "Attempt to wait on unknown resource type" ) );
			break;

	} // end switch

	return -1;
}

oexRESULT CResource::Reset( oexUINT x_uTimeout )
{
	// Verify information
	if ( cInvalid() == m_hHandle || eRtInvalid == m_eType )
		return -1;

	// Get pointer to resource information
	SResourceInfo *pRi = (SResourceInfo*)m_hHandle;
	if ( !oexCHECK_PTR( pRi ) )
	{	oexERROR( 0, oexT( "Invalid resource info object pointer" ) );
		return -1;
	} // end if

	// Execute proper release function if valid handle
	if ( cInvalid() != m_hHandle )
		switch( m_eType )
	{
		case eRtInvalid :
			break;

		case eRtFile :
			break;

		case eRtSocket :
			break;

		case eRtThread :
			break;

		case eRtMutex :
		case eRtLock :
			return ReleaseMutex( pRi->hHandle ) ? waitSuccess : waitFailed;
			break;

		case eRtEvent :
			return ResetEvent( pRi->hHandle ) ? waitSuccess : waitFailed;
			break;

		default :
			oexERROR( -1, oexT( "Attempt to wait on unknown resource type" ) );
			break;

	} // end switch

	return -1;
}

oexINT CResource::WaitMultiple( oexINT x_nCount, CResource **x_pResources, oexUINT x_uTimeout, oexBOOL x_nMin )
{
	oexINT nNumHandles = 0;
	HANDLE hHandles[ MAXIMUM_WAIT_OBJECTS ];
	oexINT nRealIndex[ MAXIMUM_WAIT_OBJECTS ];

	if ( x_nCount > MAXIMUM_WAIT_OBJECTS )
		oexWARNING( 0, oexT( "Number of handles specified is beyond system capabilities!!!" ) );

	// Loop through the handles
	for( oexINT i = 0; i < x_nCount && nNumHandles < MAXIMUM_WAIT_OBJECTS; i++ )
	{
		if ( !x_pResources[ i ]->IsValid() )
			oexWARNING( 0, oexT( "Invalid handle specified to WaitMultiple()" ) );
		else
		{
			SResourceInfo *pRi = (SResourceInfo*)x_pResources[ i ]->GetHandle();

			// +++ Should this be INVALID_HANDLE_VALUE?
			if ( !oexCHECK_PTR( pRi ) )
				oexERROR( 0, oexT( "Invalid data pointer" ) );

			else
				hHandles[ nNumHandles ] = pRi->hHandle, nRealIndex[ nNumHandles++ ] = i;

		} // end else

	} // end for

	// Wait for signal or timeout
	oexINT nRet = WaitForMultipleObjects( nNumHandles, hHandles, 0 != x_nMin, x_uTimeout );

	// Error?
	if ( 0 > nRet )
		return nRet;

	// Index out of range?
	else if ( nRet >= nNumHandles )
		return -1;

	// Return the index of the signaled object
	return nRealIndex[ nRet ];
}
