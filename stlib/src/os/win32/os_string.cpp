/*------------------------------------------------------------------
// os_string.h
// Copyright (c) 2006 
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

#include "../../../stlib.h"
#include "stdwin.h"

STLIB_USING_NAMESPACE

stUINT COsString::strlen( stCSTR pStr )
{	return strlen( pStr );
}

stINT COsString::memcmp( stCPVOID pBuf1, stCPVOID pBuf2, stLONG lCount )
{	return memcmp( pBuf1, pBuf2, lCount ); 
}

stPVOID COsString::memcpy( stCPVOID pBuf1, stCPVOID pBuf2, stLONG lCount )
{	return memcpy( pBuf1, pBuf2, lCount ); 
}

stPVOID memset( stCPVOID pDest, stINT ch, stLONG lCount )
{	return memset( pDest, ch, lCount ); 
}

stUINT strtoul( stCSTR pStr, stSTR *pEndPtr, stINT nBase )
{	return strtoul( pStr, pEndPtr, nBase );
}