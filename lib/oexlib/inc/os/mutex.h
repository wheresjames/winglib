/*------------------------------------------------------------------
// mutex.h
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

//==================================================================
// CMutex
//
/// OS independent mutex
/**
	Functions for creating os independent mutex objects
	
*/
//==================================================================
class CMutex
{
public:

	typedef oexPVOID		t_MUTEX;

private:
	CMutex() {}
	virtual ~CMutex() {}

public:

	//==============================================================
	// osCreateMutex()
	//==============================================================
	/// Creates a mutex object
	/**
		\param [in] x_pName		-	Optional name for the mutex object,
									this value can be NULL.
		
		\return Handle to a mutex object
	
		\see osDestroyMutex()
	*/
	static t_MUTEX osCreateMutex( oexCSTR x_pName );

	//==============================================================
	// osDestroyMutex()
	//==============================================================
	/// Free a mutex object created by osCreateMutex()
	/**
		\param [in] x_pMutex	-	Handle to mutex object

		\see osCreateMutex()		
	*/
	static void osDestroyMutex( t_MUTEX x_pMutex );

	//==============================================================
	// osReleaseMutex()
	//==============================================================
	/// Releases ownership of the mutex object
	/**
		\param [in] x_pMutex	-	Handle to mutex object
	*/
	static void osReleaseMutex( t_MUTEX x_pMutex );

	//==============================================================
	// vInvalid()
	//==============================================================
	/// Value representing an invalid mutex handle value
	static oexCONST t_MUTEX vInvalid() 
    {   return c_Invalid; }

private:

	/// Invalid mutex handle value
	static oexCONST t_MUTEX		c_Invalid;

};

