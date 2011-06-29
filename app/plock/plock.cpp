
#if defined( _WIN32 ) || defined( WIN32 )
#	define USE_WINTHREADS
#	include <windows.h>
#else
#	include <unistd.h>
#	include <pthread.h>
#endif

#include "stdio.h"
#	if defined( ANDROID )
#		include "stdlib.h"
#	endif

#define _MSG( m ) printf( "%s(%d) : " m "\n", __FILE__, __LINE__ )
#define _TR() _MSG( "MARKER" )

typedef void* t_id;

void tsleep( unsigned int u )
{
#if defined( USE_WINTHREADS )
	::Sleep( u );
#else
	::usleep( u * 1000 );
#endif
}

template < typename T_ID = unsigned long, typename T_COUNT = unsigned long > 
	class TPseudoLock
{
private:

	volatile T_ID 			hGate;
	volatile T_ID			hAce;
	volatile T_ID			hPending;
	volatile T_ID			hOwner;
	volatile T_COUNT		nCount;
	
public:

	TPseudoLock()
	{
		hGate = 0;
		hAce = 0;
		hPending = 0;
		hOwner = 0;
		nCount = 0;
	}

	int Lock( const T_ID id )
	{
		// Fairness enforcing
		if ( id == hAce )
		{	hAce = 0;
			tsleep( 10 );
		} // end if
	
		// Already the owned?
		if ( hOwner )
		{
			// Someone else owns it?
			if ( id != hOwner )
				return 0;

			// Ref
			nCount++;

			return 1;
			
		} // end if

		// Is the gate open?
		if ( hGate )
			return 0;

		// Register our id
		hAce = id;	

		// Is the gate still open?
		if ( hGate )
			return 0;

		// Close the gate
		hGate = id;

		// Punt if we're not the potential winner?
		if ( id != hAce )
		{
			// Open the gate if we're the gate keeper
			if ( id == hGate )
				hGate = 0;

			return 0;
		
		} // end if

		// Are we the gate keeper?
		if ( id != hGate )
			return 0;

		// Then the lock is ours
		hOwner = id;
		nCount = 1;

		return 1;
	}

	int Unlock( const T_ID id )
	{
		// Are we the owner
		if ( id != hOwner )
			return 0;

		// Decrement ref count
		if ( nCount )
			nCount--;

		// Track refs
		if ( 0 < nCount )
			return 1;

		// Disown
		hOwner = 0;
		
		// Open the gate
		hGate = 0;
		
		return 1;
	}

	int isLocked()
	{
		return hOwner ? 1 : 0;
	}

	T_ID getOwner()
	{
		return hOwner;
	}

	int isOwner( const T_ID id )
	{
		return ( id == hOwner ) ? 1 : 0;
	}

};

typedef TPseudoLock< t_id > CPseudoLock;

t_id getThreadId()
{
#if defined( USE_WINTHREADS )
	return (t_id)GetCurrentThreadId();
#else
	return (t_id)::pthread_self();
#endif
}

struct SThreadInfo
{
#if defined( USE_WINTHREADS )
	volatile HANDLE						hThread;
#else
	volatile pthread_t					hThread;
#endif

	CPseudoLock			 				*pLock;

	volatile bool						bRun;

	volatile unsigned long * volatile	pWaiting;

	volatile unsigned long * volatile	pAttempts;

	volatile unsigned long * volatile	pSuccess;
	
	volatile unsigned long * volatile	pCollisions;
};

#if defined( USE_WINTHREADS )
static DWORD WINAPI ThreadProc( LPVOID pData )
#else
static void* ThreadProc( void* pData )
#endif
{
	SThreadInfo *pTi = (SThreadInfo*)pData;
	if ( !pTi )
		return 0;

	t_id id = getThreadId();

	while ( pTi->bRun )
	{
		(*pTi->pAttempts)++;

		if ( pTi->pLock->Lock( id ) )
		{
			printf( "LOCKED : %lu\n", (unsigned long)id );

			// Double lock?
			if ( (*pTi->pWaiting) )
			{	(*pTi->pCollisions)++;
				printf( "!!!! COLLISION : %lu -> %lu \n", (unsigned long)id, (unsigned long)(*pTi->pWaiting) );
			} // end if

			*pTi->pWaiting = (volatile unsigned long)id;

			// Count one and hold the lock a bit
			(*pTi->pSuccess)++;

			tsleep( 10 );

			(*pTi->pWaiting) = 0;

			printf( "UNLOCKED : %lu\n", (unsigned long)id );

			pTi->pLock->Unlock( id );

		} // end if
		
		// tsleep( 0 );

	} // end while

	return 0;
}

int create_thread( SThreadInfo *pTi )
{
#if defined( USE_WINTHREADS )

	// Create a thread
	DWORD dwThreadId = 0;
	pTi->hThread = CreateThread( (LPSECURITY_ATTRIBUTES)NULL, 0, 
								 ThreadProc, (LPVOID)pTi, 0, (LPDWORD)&dwThreadId );
	if ( !pTi->hThread )
		return 0;
		
#else

	// Make thread joinable
	pthread_attr_t attr; pthread_attr_init( &attr );
	pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_JOINABLE );

	// Create the thread
	int nRet = pthread_create( (pthread_t*)&pTi->hThread, &attr, ThreadProc, pTi );

	// Lose attributes structure
	pthread_attr_destroy( &attr );
	
	if ( nRet )
		return 0;

#endif

	return 1;
}

int join_thread( SThreadInfo *pTi )
{
	pTi->bRun = false;

#if defined( USE_WINTHREADS )

	if ( WAIT_OBJECT_0 != WaitForSingleObject( pTi->hThread, 3000 ) )
		return 0;

#else

	if ( pthread_join( pTi->hThread, 0 ) )
		return 0;

#endif

	return 1;
}

int main(int argc, char* argv[])
{
	_MSG( "Lock Test" );
	
	t_id id = getThreadId();
	
	CPseudoLock pl;

	if ( !pl.Lock( id ) )
	{	_MSG( "Lock() failed" );
		return -1;
	} // end if
	
	if ( !pl.isOwner( id ) )
	{	_MSG( "isOwner() failed" );
		return -1;
	} // end if
	
	if ( pl.getOwner() != id )
	{	_MSG( "getOwner() failed" );
		return -1;
	} // end if

	pl.Unlock( id );
	
	if ( pl.isLocked() )
	{	_MSG( "isLocked() failed" );
		return -1;
	} // end if

	// Now for the real tests
	SThreadInfo ti[ 4 ];
	volatile unsigned long ulWaiting = 0, ulSuccess = 0, ulAttempts = 0, ulCollisions = 0;
	long nThreads = sizeof( ti ) / sizeof( ti[ 0 ] );
	
	for( long i = 0; i < nThreads; i++ )
	{
		ti[ i ].bRun = true;
		ti[ i ].pLock = &pl;
		ti[ i ].pWaiting = &ulWaiting;
		ti[ i ].pSuccess = &ulSuccess;
		ti[ i ].pAttempts = &ulAttempts;
		ti[ i ].pCollisions = &ulCollisions;
		
		if ( !create_thread( &(ti[ i ]) ) )
		{	printf( "create_thread() failed : %lu \n", (unsigned long)i );
			return -1;
		} // end if

	} // end for

	printf( "Created %lu threads...\n", (unsigned long)nThreads );
	
	// Let it run a while
	tsleep( 10 * 1000 );
	
	// Close threads
	for( long i = 0; i < nThreads; i++ )
		if ( !join_thread( &(ti[ i ]) ) )
			printf( "join_thread() failed : %lu \n", (unsigned long)i );
	
	printf( "\n\n Total Attempts : %lu \n Total Locks : %lu \n Total Collisions : %lu \n\n", 
			ulAttempts, ulSuccess, ulCollisions );
	
	_MSG( "Tests Succeeded" );

	return 0;
}
