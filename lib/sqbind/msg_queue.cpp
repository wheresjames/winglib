#include "stdafx.h"
//#include "MainThread.h"

#define CMsgQueue_ReleaseHandle( h )   ( ( h ) ? ( CloseHandle( h ), h = NULL ) : ( h ) )

CMsgQueue::CMsgQueue(void)
{
    // Create events
    m_hMsgWaiting = CreateEvent( NULL, TRUE, FALSE, NULL );

    m_dwCurrentThreadId = 0;
}

CMsgQueue::~CMsgQueue() 
{
    // Acquire lock
    CLocalLock ll( &m_cLock );
    if ( !ll.IsLocked() )
        return;

    Destroy(); 

    // Kill the events
    CMsgQueue_ReleaseHandle( m_hMsgWaiting );
}


void CMsgQueue::Destroy()
{
    // Acquire lock
    CLocalLock ll( &m_cLock );
    if ( !ll.IsLocked() )
        return;

    // Dump the queue
    m_lstMsgQueue.clear();

    // Reset event
    ResetEvent( m_hMsgWaiting );
}

BOOL CMsgQueue::Msg( std::tstring sMsg, t_Params *pmapParams, t_Params *pmapReply, UINT uTimeout )
{
    HANDLE hReply = NULL;

    // If it's our own thread calling
    if ( m_dwCurrentThreadId && GetCurrentThreadId() == m_dwCurrentThreadId )
    {
        // Just process the message directly
        if ( pmapParams )
            return ProcessMsg( sMsg, *pmapParams, pmapReply );

        else
        {   t_Params params;
            return ProcessMsg( sMsg, params, pmapReply );
        } // end else

    } // end if                

    { // Stuff message into buffer

        // Acquire lock
        CLocalLock ll( &m_cLock );
        if ( !ll.IsLocked() )
            return FALSE;

        // Reply event handle needed?
        if ( pmapReply )
        {
            hReply = CreateEvent( NULL, TRUE, FALSE, NULL );
//            ASSERT( hReply );

            if ( hReply )
                ResetEvent( hReply );
            else
                pmapReply = NULL;

        } // end if

        // Add a message
        m_lstMsgQueue.push_back( SMsg( sMsg, pmapParams, hReply, pmapReply ) );

        // Signal that a message is waiting
        SetEvent( m_hMsgWaiting );

    } // end message stuffing

    // Wait for relpy if needed
    if ( pmapReply )
    {   
        // Wait for reply
        BOOL bSuccess = ::WaitForSingleObject( hReply, uTimeout ) == WAIT_OBJECT_0;

        // Lose reply event
        CMsgQueue_ReleaseHandle( hReply );

        // Punt if we got the reply
        if ( bSuccess )
            return TRUE;
        
        // Acquire lock
        CLocalLock ll( &m_cLock );
        if ( !ll.IsLocked() )
        {
            // You're screwed here because pmapReply is dangling
            // But don't worry, this won't ever happen ;)
//            ASSERT( 0 );

            // Only safe thing to do would be kill the thread...
            KillThread();

            return FALSE;

        } // end if

        // Clear the list so pmapReply is not dangling
        m_lstMsgQueue.clear();

    } // end if

    return TRUE;
}

BOOL CMsgQueue::ProcessMsgs()
{
    // Acquire lock
    CLocalLock ll( &m_cLock );
    if ( !ll.IsLocked() )
        return TRUE;

    // Process messages
    t_MsgQueue::iterator it;
    while ( m_lstMsgQueue.end() != ( it = m_lstMsgQueue.begin() ) )
    {
        // Process the message
        ProcessMsg( it->sMsg, it->mapParams, it->pmapReply );

        // We must stop processing if someone is waiting for a reply
        if ( it->pmapReply )
        {
            // Signal that reply is ready
            ::SetEvent( it->hReply );
            m_lstMsgQueue.erase( it );

            // Reset signal if queue is empty
            if ( !m_lstMsgQueue.size() )
                ::ResetEvent( m_hMsgWaiting );

            return TRUE;

        } // end if

        // Ditch the message
        m_lstMsgQueue.erase( it );

    } // end while

    // Reset signal
    ::ResetEvent( m_hMsgWaiting );

    return TRUE;
}

BOOL CMsgQueue::ProcessMsg( std::tstring &sMsg, t_Params &mapParams, t_Params *pmapReply )
{
    return TRUE;
}

