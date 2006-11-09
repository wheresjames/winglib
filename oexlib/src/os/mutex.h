/*------------------------------------------------------------------
// mutex.h
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

#pragma once

class CMutex
{
private:
	CMutex() {}

public:

	static oexMUTEX osCreateMutex( oexCSTR x_pName );

	static void osDestroyMutex( oexMUTEX x_pMutex );

	static void osReleaseMutex( oexMUTEX x_pMutex );

	static oexBOOL osSetEvent( oexMUTEX x_pMutex );
	
	static oexBOOL osResetEvent( oexMUTEX x_pMutex );

	enum etWait
	{	waitSuccess = 0,
		waitTimeout = -1,
		waitFailed = -2
	};

	static oexINT osWaitForSingleObject( oexMUTEX x_pMutex, oexUINT x_uTimeout );

	static oexINT osWaitForMultipleObjects( oexUINT x_uObjects, oexMUTEX *x_pMutex, oexBOOL x_bWaitAll, oexUINT x_uTimeout );

	static oexULONG osInterlockedIncrement( oexLONG *x_puVal );

	static oexULONG osInterlockedDecrement( oexLONG *x_puVal );

	static const oexUINT vInfinite() { return c_Infinite; }

	static const oexMUTEX vInvalid() { return c_Invalid; }

private:

	/// Invalid mutex handle value
	static const oexMUTEX			c_Invalid;

	static const oexUINT			c_Infinite;

};

