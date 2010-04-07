/*------------------------------------------------------------------
// stack_trace.h
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

// iii Things have been carefully designed so that thread locking is not needed.
//     However, should someone come along and make changes that break it, the
//     the following macro could come in handy.
//#define OEXLIB_STACK_ENABLE_LOCKS

// CStackTrace
//
/// Contains stack traces for multiple threads
/**
	This class handles recording stack traceing.  Thread safe.	
*/
//==================================================================
class CStackTrace  
{
public:


	/// Maxium stack size
	enum { eMaxStack = 1024 };

	/// Max threads that can be tracked simultaniously 
	/// 10 = 1,024, 11 = 2,048, 12 = 4,096
	enum { eMaxThreadsBits = 12 };
	
	/// String type
	typedef oexSTTSTR t_str;
	
	/// Encapsulates stack tracing functionality for a single thread
	class CStack
	{
	public:

		/// Constructor
		CStack() 
		{
			m_uCreated = oexGetUnixTime();
			m_uThreadId = 0;
			m_uStackPtr = 0;
			m_memStack.OexNew( eMaxStack ); 
			m_memStack.Zero();
			m_pStack = m_memStack.Ptr();

			m_lCheckPoint = 0;

#ifdef OEXLIB_STACK_HISTORY

			m_uMaxHistory = 0;
			m_uStackHistoryPtr = 0;

			m_memStackHistory.OexNew( eMaxStack ); 
			m_memStackHistory.Zero();
			m_pStackHistory = m_memStackHistory.Ptr();

#endif
		}

		//==============================================================
		// Push()
		//==============================================================
		/// Push pointer onto the stack
		/**
			\param [in] x_pFunction	-	Name of function to push on the stack

			\see Pop()
		*/
		void Push( t_str x_pFunction )
		{
			// Normal stack
			if ( eMaxStack > m_uStackPtr )
				m_pStack[ m_uStackPtr ] = x_pFunction, m_uStackPtr++;

#ifdef OEXLIB_STACK_HISTORY

			// Save stack history
			m_pStackHistory[ m_uStackHistoryPtr ] = x_pFunction;

			// Increment stack pointer
			m_uStackHistoryPtr++;

			// Track total items in history
			if ( m_uMaxHistory < m_uStackHistoryPtr ) 
				m_uMaxHistory = m_uStackHistoryPtr;

			// Update the history pointer
			if ( m_uStackHistoryPtr >= eMaxStack ) 
				m_uStackHistoryPtr = 0;

#endif
		}

		//==============================================================
		// Pop()
		//==============================================================
		/// Pop a pointer off the stack
		void Pop() 
		{
			if ( m_uStackPtr ) 
				m_uStackPtr--; 
		}

		/// Returns the current stack pointer
		oexUINT GetStackPtr() 
		{	return m_uStackPtr; }

		/// Retuns the actual stack
		t_str* GetStack() 
		{	return m_memStack.Ptr(); }

#ifdef OEXLIB_STACK_HISTORY

		/// Returns the number of items in the history
		oexUINT GetMaxHistory() 
		{	return m_uMaxHistory; }

		/// Returns the history pointer
		oexUINT GetHistoryPtr() 
		{	return m_uStackHistoryPtr; }

		/// Returns the history stack
		t_str* GetHistory() 
		{	return m_memStackHistory.Ptr(); }

#endif

		/// Returns the thread id for this class
		oexUINT GetThreadId() { return m_uThreadId; }

		/// Sets the thread id
		void SetThreadId( oexUINT id ) { m_uThreadId = id; }

		/// Returns the time stamp when the thread was created
		oexUINT GetCreatedTime() { return m_uCreated; }

		/// Sets the thread name
		oexCONST CStr SetName( oexCONST CStr x_sName ) { m_sName = x_sName; return x_sName; }

		/// Gets the thread name
		CStr GetName() { return m_sName; } 

		/// Sets the thread tag
		oexCONST CStr SetTag( oexCONST CStr x_sTag ) { m_sTag = x_sTag; return x_sTag; }

		/// Gets the thread tag
		CStr GetTag() { return m_sTag; } 

		/// Sets the checkpoint value
		oexLONG SetCheckpoint( oexLONG cp ) { m_lCheckPoint = cp; return cp; }

		/// Returns the checkpoint value
		oexLONG GetCheckpoint() { return m_lCheckPoint; }

	private:

		/// Time created
		oexUINT						m_uCreated;

		/// The thread id
		oexUINT						m_uThreadId;

		/// pointer to next empty slot on the stack
		oexUINT						m_uStackPtr;

		/// Stack memory
		TMem< t_str >				m_memStack;

		/// Stack memory pointer
		t_str						*m_pStack;

		/// Stack name
		CStr						m_sName;

		/// Tag
		CStr						m_sTag;

		/// Checkpoint
		oexLONG						m_lCheckPoint;

#ifdef OEXLIB_STACK_HISTORY

		/// Holds the number of items in the history
		oexUINT						m_uMaxHistory;

		/// Pointer to next empty slot in stack history
		oexUINT						m_uStackHistoryPtr;

		/// Stack history
		TMem< t_str >				m_memStackHistory;

		/// Stack history pointer
		t_str						*m_pStackHistory;

#endif

	};

	/// Stack pool type
	typedef TMappedPool< CStack >	t_stack;

public:

	//==============================================================
	// GetStack()
	//==============================================================
	/// Returns stack object for current thread
	/**
		\return Pointer to the stack object for the calling thread
	
		\see 
	*/
	CStack* GetStack();

	//==============================================================
	// InitPush()
	//==============================================================
	/// Initializes stack for calling thread if needed and returns pointer
	/**
		\return Pointer to the stack object for the calling thread
	
		\see 
	*/
	CStack* InitPush();

	//==============================================================
	// RemoveThread()
	//==============================================================
	/// Removes the stack trace for the calling thread
	oexBOOL RemoveThread();

	/// Constructor
	CStackTrace()
	{
		m_bEnable = oexFALSE;
	}

	/// Destructor
	virtual ~CStackTrace()
	{
	}

	/// Allows iterating through stack pointers
	CStack* Next( oexUINT *i )
	{	return m_poolStack.Next( i );
	}

	/// Number of free stack tracing slots
	oexUINT getFreeSlots()
	{	return m_poolStack.getFreeSlots();
	}

	/// Number of stack tracing slots being used
	oexUINT getUsedSlots()
	{	return m_poolStack.getUsedSlots();
	}

	/// Total number of stack tracing slots
	oexUINT getTotalSlots()
	{	return m_poolStack.getTotalSlots();
	}

private:	

	/// Current stack
	t_stack							m_poolStack;

#if defined( OEXLIB_STACK_ENABLE_LOCKS )

	/// Lock
	oexLock							m_lock;

#endif

public:

#if defined( OEXLIB_STACK_ENABLE_LOCKS )

	//==============================================================
	// CTlLock*()
	//==============================================================
	/// Return the lock object
	operator oexLock*() { return &m_lock; }

#endif

	//==============================================================
	// Save()
	//==============================================================
	/// Save stack to file
	/**
		\param [in] x_pFile	-	Disk filename
		
		\return Non-zero if success
	
		\see 
	*/
	oexBOOL Save( oexCSTR x_pFile );

	//==============================================================
	// Release()
	//==============================================================
	/// Releases stack trace object
	void EnableStackTracing( oexBOOL bEnable )
	{	m_bEnable = bEnable; }

	//==============================================================
	// isEnabled()
	//==============================================================
	/// Returns non zero if stack tracing is enabled
	oexBOOL isEnabled() { return m_bEnable; }

	//==============================================================
	// Release()
	//==============================================================
	/// Releases stack trace object
	void Release();

private:

	/// Release all
	oexBOOL				m_bEnable;

};

//==================================================================
// CLocalStackTrace
//
/// Use this class to wrap local stack pushes
/**
	Wraps the CStack Push() and ensures Pop() is called when the 
	function exits.
*/
//==================================================================
class CLocalStackTrace
{
public:

	/// Default Constructor
	CLocalStackTrace( CStackTrace::t_str x_pFunction ) 
	{
		// Get stack object
		m_pStack = oexSt().InitPush(); 

		// Push function if valid
		if ( m_pStack ) 
			m_pStack->Push( x_pFunction );
	}

	/// Destructor
	~CLocalStackTrace() 
	{ 
		if ( !oexSt().isEnabled() ) 
			return;

		if ( m_pStack ) 
			m_pStack->Pop();

	}

private:

	/// Pointer to stack item that will be released
	CStackTrace::CStack					*m_pStack;

};



