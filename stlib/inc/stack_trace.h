/*------------------------------------------------------------------
// stack_trace.h
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

#if !defined(AFX_STACKTRACE_H__444879CF_2267_458B_9685_5237B375EC2C__INCLUDED_)
#define AFX_STACKTRACE_H__444879CF_2267_458B_9685_5237B375EC2C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define ENABLE_STACK_TRACING
#define ENABLE_STACK_HISTORY
#define ENABLE_STACK_PROFILE

// DLL / EXE ( Always works )
#define CStackTrace_USING_TLSAPI

// EXE only
//#define CStackTrace_USING_TLS

// Normal stack tracing
#if defined( ENABLE_STACK_TRACING ) && ( defined( __INTEL_COMPILER ) || ( _MSC_VER > 1200 ) )
#	define _STT() STLIB_NAMESPACE::CLocalStackTrace _l_lst( __FUNCTION__ );
#else
#	define _STT()
#endif

// More extensive stack tracing
#if defined( ENABLE_STACK_TRACING_EX ) && ( defined( __INTEL_COMPILER ) || ( _MSC_VER > 1200 ) )
#	define _STTEX() STLIB_NAMESPACE::CLocalStackTrace _l_lst( __FUNCTION__ );
#else
#	define _STTEX()
#endif

/// Call before memory dump else you will 'think' you have memory leaks.
#define _STT_RELEASE() CStackTrace::Release()

//#define ENABLE_STACK_HISTORY
//#define ENABLE_STACK_PROFILE

//==================================================================
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

	enum { eMaxStack = 1024 };

	/// Encapsulates stack tracing functionality for a single thread
	class CStack
	{
	public:

#ifdef ENABLE_STACK_PROFILE

		/// Stack profile item
		struct SProfileItem
		{
			SProfileItem() { llTotalTime = 0; }

			/// Timer is used to resolve the amount of time spent in a function
			CHqTimer		hqTimer;

			/// Accumulates the total amount of time spent on a function
			stINT64			llTotalTime;
		};

		/// Profile list type
		typedef TAssoList< stString, CAutoMem< SProfileItem > > t_ProfileList;

		/// Profile list iterator type
		typedef t_ProfileList::iterator t_itProfileList;

#endif

	public:

		/// Constructor
		CStack() 
		{
			m_uStackPtr = 0; 
			m_memStack.Allocate( eMaxStack ); 
			m_memStack.Zero();
			m_pStack = m_memStack.GetData();

#ifdef ENABLE_STACK_HISTORY

			m_uMaxHistory = 0;
			m_uStackHistoryPtr = 0;

			m_memStackHistory.Allocate( eMaxStack ); 
			m_memStackHistory.Zero();
			m_pStackHistory = m_memStackHistory.GetData();

#endif

#ifdef ENABLE_STACK_PROFILE

			m_memCallTime.Allocate( eMaxStack ); 
			m_memCallTime.Zero();
			m_pCallTime = m_memCallTime.GetData();

#endif
		}

		//==============================================================
		// Push()
		//==============================================================
		/// Push pointer onto the stack
		/**
			\param [in] pFunction	-	Name of function to push on the stack
			
			\return Pointer to profile item if profiling is enabled, otherwise void
		
			\see Pop()
		*/
#ifdef ENABLE_STACK_PROFILE
		SProfileItem* Push( stCSTR pFunction )
#else
		void Push( stCSTR pFunction )
#endif
		{
			// Normal stack
			if ( eMaxStack > m_uStackPtr )
				m_pStack[ m_uStackPtr ] = pFunction, m_uStackPtr++;

#ifdef ENABLE_STACK_PROFILE

			// This takes a little time...
			stINT64 llTime = CHqTimer::GetTimer();

			// Save call time
			m_pCallTime[ m_uStackHistoryPtr ] = llTime;

#endif

#ifdef ENABLE_STACK_HISTORY

			// Save stack history
			m_pStackHistory[ m_uStackHistoryPtr ] = pFunction;

			// Increment stack pointer
			m_uStackHistoryPtr++;

			// Track total items in history
			if ( m_uMaxHistory < m_uStackHistoryPtr ) m_uMaxHistory = m_uStackHistoryPtr;

			// Update the history pointer
			if ( m_uStackHistoryPtr >= eMaxStack ) m_uStackHistoryPtr = 0;

#endif

#ifdef ENABLE_STACK_PROFILE

			CTlLocalLock ll( *CStackTrace::St() );
			if ( !ll.IsLocked() ) return stNULL;

			// Get existing profile object
			SProfileItem *pPi = m_lstProfile[ pFunction ];
			
			// Start the timer
			if ( pPi ) pPi->hqTimer.Start( llTime );

			return pPi;
#endif
		}

		//==============================================================
		// Pop()
		//==============================================================
		/// Pop a pointer off the stack
		void Pop() 
		{
			if ( m_uStackPtr ) m_uStackPtr--; 
		}

		/// Returns the current stack pointer
		stUINT GetStackPtr() { return m_uStackPtr; }

		/// Retuns the actual stack
		stCSTR* GetStack() { return m_memStack; }

#ifdef ENABLE_STACK_HISTORY

		/// Returns the number of items in the history
		stUINT GetMaxHistory() { return m_uMaxHistory; }

		/// Returns the history pointer
		stUINT GetHistoryPtr() { return m_uStackHistoryPtr; }

		/// Returns the history stack
		stCSTR* GetHistory() { return m_memStackHistory; }

#endif

#ifdef ENABLE_STACK_PROFILE


		/// Returns a pointer to the profile list
		t_ProfileList* GetProfile() { return &m_lstProfile; }

#endif

	private:

		/// pointer to next empty slot on the stack
		stUINT						m_uStackPtr;

		/// Stack memory
		TMemory< stCSTR >			m_memStack;

		/// Stack memory pointer
		stCSTR						*m_pStack;

#ifdef ENABLE_STACK_HISTORY

		/// Holds the number of items in the history
		stUINT						m_uMaxHistory;

		/// Pointer to next empty slot in stack history
		stUINT						m_uStackHistoryPtr;

		/// Stack history
		TMemory< stCSTR >			m_memStackHistory;

		/// Stack history pointer
		stCSTR						*m_pStackHistory;

#endif

#ifdef ENABLE_STACK_PROFILE

		/// Stack history call times
		TMemory< stINT64 >			m_memCallTime;

		/// Call time pointer
		stINT64						*m_pCallTime;

		/// Stack profile
		t_ProfileList				m_lstProfile;		

#endif

	};

	/// List type
	typedef TAssoList< stINT32, CAutoMem< CStack > > t_StackList;

    /// Iterator type
    typedef t_StackList::iterator t_itStackList;

public:

	//==============================================================
	// GetStack()
	//==============================================================
	/// Returns stack object for current thread
	/**
		\return Pointer to the stack object for the calling thread
	
		\see 
	*/
	CStack* GetStack()
	{	stINT32 dwThreadId = 0; /* +++ ::GetCurrentThreadId(); */
		t_itStackList it = m_lstStack.find( dwThreadId );
		if ( m_lstStack.end() == it ) return m_lstStack[ dwThreadId ];
		if ( m_lstStack.end() == it ) return stNULL;
		return *it;
	}

	//==============================================================
	// InitPush()
	//==============================================================
	/// Initializes stack for calling thread if needed and returns pointer
	/**
		\return Pointer to the stack object for the calling thread
	
		\see 
	*/
	CStack* InitPush() 
	{

#if defined( CStackTrace_USING_TLSAPI )

		// Return thread local storage index
		CStack *pStack;
		if ( CTls::vInvalid() != m_tls_dwIndex )
		{	pStack = (CStack*)CTls::osGetValue( m_tls_dwIndex );
			if ( pStack != stNULL ) return pStack;
		} // end if

		// Initialize for this thread
		CTlLocalLock ll( m_lock );
		if ( !ll.IsLocked() ) return stNULL;

		// Allocate TLS 
		if ( CTls::vInvalid() == m_tls_dwIndex )
			m_tls_dwIndex = CTls::osAllocate();
		
		// Set stack value
		pStack = GetStack();
		if ( CTls::vInvalid() != m_tls_dwIndex ) 
			CTls::osSetValue( m_tls_dwIndex, pStack );
			
		return pStack;

#elif defined( CStackTrace_USING_TLS )

		// Are we already initialized for this thread?
		if ( m_tls_pStack ) return m_tls_pStack;

		// Initialize for this thread
		CTlLocalLock ll( m_lock );
		if ( !ll.IsLocked() ) return stNULL;
		return ( m_tls_pStack = GetStack() );

#else

		// Lock first
		CTlLocalLock ll( m_lock );
		if ( !ll.IsLocked() ) return stNULL;
		return GetStack(); 

#endif	

	}

	//==============================================================
	// GetList()
	//==============================================================
	/// Returns a pointer to the stack list
	t_StackList* GetList() { return &m_lstStack; }

	/// Constructor
	CStackTrace();

	/// Destructor
	virtual ~CStackTrace();

private:	

	/// Current stack
	t_StackList					m_lstStack;

	/// Lock
	CTlLock						m_lock;

public:

	//==============================================================
	// CTlLock*()
	//==============================================================
	/// Return the lock object
	operator CTlLock*() { return &m_lock; }

	//==============================================================
	// Save()
	//==============================================================
	/// Save stack to file
	/**
		\param [in] pFile	-	Disk filename
		
		\return Non-zero if success
	
		\see 
	*/
	stBOOL Save( stCSTR pFile );

	/// Instance of stack trace
	static CStackTrace			*m_pst;

	//==============================================================
	// St()
	//==============================================================
	/// Returns instance of stack trace object
	static CStackTrace* St() 
	{ 	if ( !m_pst ) m_pst = new CStackTrace(); return m_pst; }

	//==============================================================
	// Release()
	//==============================================================
	/// Releases stack trace object
	static void Release() 
	{	
		// No more stack tracing
		m_bShutdown = stTRUE; 

		// Delete object if any
		if ( m_pst )
		{
			// Lock for good
			m_pst->m_lock.Lock( 30000 );

			// Delete
			delete m_pst; 

		} // end if

		// All gone
		m_pst = stNULL; 
	}

	/// Release all
	static stBOOL			m_bShutdown;

#if defined( CStackTrace_USING_TLSAPI )

	static stINT32				  m_tls_dwIndex;

#elif defined( CStackTrace_USING_TLS )

	/// Thread specific stack pointer
	static _declspec( thread ) CStack *m_tls_pStack;

#endif

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
	CLocalStackTrace( stCSTR pFunction ) 
	{
		if ( !CStackTrace::m_bShutdown )
		{
			m_pStack = CStackTrace::St()->InitPush(); 

#ifdef ENABLE_STACK_PROFILE
			if ( m_pStack ) m_pPi = m_pStack->Push( pFunction );
			else m_pPi = stNULL;
#else
			if ( m_pStack ) m_pStack->Push( pFunction );
#endif

		} // end if
		
#ifdef ENABLE_STACK_PROFILE
		else m_pStack = stNULL, m_pPi = stNULL; 
#else
		else m_pStack = stNULL; 
#endif

	}

	/// Destructor
	~CLocalStackTrace() 
	{ 
		if ( CStackTrace::m_bShutdown ) return;

		if ( m_pStack ) m_pStack->Pop();

#ifdef ENABLE_STACK_PROFILE

		if ( m_pPi ) m_pPi->llTotalTime += m_pPi->hqTimer.Ellapsed();

#endif

	}

private:

	/// Pointer to stack item that will be released
	CStackTrace::CStack					*m_pStack;

#ifdef ENABLE_STACK_PROFILE

	/// This is just to make sure the timer frequency is initialized
	CHqTimer							m_hqDummy;

	/// Pointer to profile item that is to be updated on release
	CStackTrace::CStack::SProfileItem	*m_pPi;

#endif
};


#endif // !defined(AFX_STACKTRACE_H__444879CF_2267_458B_9685_5237B375EC2C__INCLUDED_)
