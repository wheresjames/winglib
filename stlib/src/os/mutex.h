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
public:

	typedef void*	t_MUTEX;

private:
	CMutex() {}

public:

	static t_MUTEX osCreateMutex( stCSTR x_pName );

	static void osDestroyMutex( t_MUTEX x_pMutex );

	static void osReleaseMutex( t_MUTEX x_pMutex );

	static stBOOL osSetEvent( t_MUTEX x_pMutex );
	
	static stBOOL osResetEvent( t_MUTEX x_pMutex );

	enum etWait
	{	waitSuccess = 0,
		waitTimeout = -1,
		waitFailed = -2
	};

	static stINT osWaitForSingleObject( t_MUTEX x_pMutex, stUINT x_uTimeout );

	static stINT osWaitForMultipleObjects( stUINT x_uObjects, t_MUTEX *x_pMutex, stBOOL x_bWaitAll, stUINT x_uTimeout );

	static stULONG osInterlockedIncrement( stLONG *x_puVal );

	static stULONG osInterlockedDecrement( stLONG *x_puVal );

	static const stUINT vInfinite() { return c_Infinite; }

	static const t_MUTEX vInvalid() { return c_Invalid; }

private:

	/// Invalid mutex handle value
	static const t_MUTEX		c_Invalid;

	static const stUINT			c_Infinite;

};

