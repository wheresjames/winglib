/*------------------------------------------------------------------
// event.h
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
// CEvent
//
/// OS independent event
/**
	Functions for creating os independent event objects
	
*/
//==================================================================
class CEvent
{
public:

	typedef oexPVOID		t_EVENT;

private:
	CEvent() {}
	virtual ~CEvent() {}

public:

	//==============================================================
	// osCreateEvent()
	//==============================================================
	/// Creates a event object
	/**
		\param [in] x_pName		    -	Optional name for the event object,
									    this value can be NULL.

        \param [in] x_bManualReset  -   Non-zero for manual reset. If
                                        set to zero, the event will
                                        be automatically reset after
                                        a single thread is released from
                                        a wait function.
		
		\return Handle to a event object
	
		\see osDestroyEvent()
	*/
	static t_EVENT osCreateEvent( oexCSTR x_pName, oexBOOL x_bManualReset );

	//==============================================================
	// osDestroyEvent()
	//==============================================================
	/// Free a event object created by osCreateEvent()
	/**
		\param [in] x_pEvent	-	Handle to event object

		\see osCreateEvent()		
	*/
	static void osDestroyEvent( t_EVENT x_pEvent );

	//==============================================================
	// osSetEvent()
	//==============================================================
	/// Sets the event object to the signaled state
	/**
		\param [in] x_pEvent	-	Handle to event object
		
		\return Non-zero if success

		\see osResetEvent()
	*/
	static oexBOOL osSetEvent( t_EVENT x_pEvent );
	
	//==============================================================
	// osResetEvent()
	//==============================================================
	/// Sets the event object to the un-signaled state
	/**
		\param [in] x_pEvent	-	Handle to event object
		
		\return Non-zero if success
	
		\see osSetEvent()
	*/
	static oexBOOL osResetEvent( t_EVENT x_pEvent );

	enum etWait
	{	
		/// Value indicating wait succeeded
		waitSuccess = 0,

		/// Value indicating wait timed out
		waitTimeout = -1,

		/// Value indicating wait expired
		waitFailed = -2
	};

	//==============================================================
	// osWaitForSingleObject()
	//==============================================================
	/// Waits for a event object to become signaled
	/**
		\param [in] x_pEvent	-	Handle to event object
		\param [in] x_uTimeout	-	Maximum amount of time, in
									milli-seconds, to wait.
		
		\return Zero if success, otherwise waitTimeout or waitFailed
	
		\see 
	*/
	static oexINT osWaitForSingleObject( t_EVENT x_pEvent, oexUINT x_uTimeout );

	//==============================================================
	// osWaitForMultipleObjects()
	//==============================================================
	/// Waits for multiple event objects
	/**
		\param [in] x_uObjects	-	Number of event objects
		\param [in] x_pEvent	-	Array of event objects
		\param [in] x_bWaitAll	-	Non-zero if you wish to wait for
									all objects to become signaled. 
									Otherwise this function returns
									as soon as any objects are in
									the signaled state.
		\param [in] x_uTimeout	-	Maximum amount of time, in 
									milli-seconds, to wait.
		
		\return Zero based index of object that is signaled if success, 
				otherwise waitTimeout or waitFailed
	
		\see 
	*/
	static oexINT osWaitForMultipleObjects( oexUINT x_uObjects, t_EVENT *x_pEvent, oexBOOL x_bWaitAll, oexUINT x_uTimeout );

	//==============================================================
	// vInfinite()
	//==============================================================
	/// Value representing infinite timeout.
	/**
		Use with wait functions
	*/
	static oexCONST oexUINT vInfinite() { return c_Infinite; }

	//==============================================================
	// vInvalid()
	//==============================================================
	/// Value representing an invalid event handle value
	static oexCONST t_EVENT vInvalid() { return c_Invalid; }

private:

	/// Invalid event handle value
	static oexCONST t_EVENT		c_Invalid;

	/// Infinite timeout value
	static oexCONST oexUINT		c_Infinite;

};

