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

/*
Crimson flames tied through my ears
Rollin' high and mighty traps
Pounced with fire on flaming roads
Using ideas as my maps
"We'll meet on edges, soon," said I
Proud 'neath heated brow.
Ah, but I was so much older then,
I'm younger than that now.

Half-wracked prejudice leaped forth
"Rip down all hate," I screamed
Lies that life is black and white
Spoke from my skull. I dreamed
Romantic facts of musketeers
Foundationed deep, somehow.
Ah, but I was so much older then,
I'm younger than that now.

Girls' faces formed the forward path
From phony jealousy
To memorizing politics
Of ancient history
Flung down by corpse evangelists
Unthought of, though, somehow.
Ah, but I was so much older then,
I'm younger than that now.

A self-ordained professor's tongue
Too serious to fool
Spouted out that liberty
Is just equality in school
"Equality," I spoke the word
As if a wedding vow.
Ah, but I was so much older then,
I'm younger than that now.

In a soldier's stance, I aimed my hand
At the mongrel dogs who teach
Fearing not that I'd become my enemy
In the instant that I preach
My pathway led by confusion boats
Mutiny from stern to bow.
Ah, but I was so much older then,
I'm younger than that now.

Yes, my guard stood hard when abstract threats
Too noble to neglect
Deceived me into thinking
I had something to protect
Good and bad, I define these terms
Quite clear, no doubt, somehow.
Ah, but I was so much older then,
I'm younger than that now.

- Bob Dylan
*/

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
private:
	CEvent()
    {}

	virtual ~CEvent()
    {}

public:

	//==============================================================
	// Create()
	//==============================================================
	/// Creates a event object
	/**
		\param [in] x_rEvent	-	Handle to event object
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
	static oexRESULT Create( CResource &x_rEvent, oexCSTR x_pName, oexBOOL x_bManualReset );

	//==============================================================
	// Destroy()
	//==============================================================
	/// Free a event object created by osCreateEvent()
	/**
		\param [in] x_rEvent	-	Handle to event object

		\see osCreateEvent()
	*/
	static void Destroy( CResource &x_rEvent );

	//==============================================================
	// SetEvent()
	//==============================================================
	/// Sets the event object to the signaled state
	/**
		\param [in] x_rEvent	-	Handle to event object

		\return Non-zero if success

		\see osResetEvent()
	*/
	static oexBOOL SetEvent( CResource &x_rEvent );

	//==============================================================
	// ResetEvent()
	//==============================================================
	/// Sets the event object to the un-signaled state
	/**
		\param [in] x_rEvent	-	Handle to event object

		\return Non-zero if success

		\see osSetEvent()
	*/
	static oexBOOL ResetEvent( CResource &x_rEvent );


private:

};

