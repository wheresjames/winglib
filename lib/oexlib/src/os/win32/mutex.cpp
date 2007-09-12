/*------------------------------------------------------------------
// mutex.cpp
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

// Ensure size
oexSTATIC_ASSERT( sizeof( CMutex::t_MUTEX ) == sizeof( HANDLE ) );

const CMutex::t_MUTEX	CMutex::c_Invalid = NULL;
									 
CMutex::t_MUTEX CMutex::osCreateMutex( oexCSTR x_pName )
{	return (void*)CreateMutex( NULL, FALSE, x_pName );
}

void CMutex::osDestroyMutex( t_MUTEX x_pMutex )
{	if ( x_pMutex ) CloseHandle( (HANDLE)x_pMutex );
}

void CMutex::osReleaseMutex( t_MUTEX x_pMutex )
{	if ( x_pMutex ) ReleaseMutex( (HANDLE)x_pMutex );
}


