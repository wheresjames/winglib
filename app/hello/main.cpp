
#if !defined( OEX_WINCE )

#include "stdio.h"

#	if defined( ANDROID )
#		include "stdlib.h"
#	endif

int main(int argc, char* argv[])
{
	printf( "Hello World!\n" );

#	if defined( ANDROID )
	exit( 0 );
#	endif
	
	return 0;
}

#else

	#include <windows.h>
	#include <stdio.h>
	#include <stdlib.h>

	#define	TEST_DIR "/storage card/test"

	int APIENTRY WinMain(HINSTANCE a,HINSTANCE b,LPWSTR c,int d)
	{
		MessageBox( NULL, L"Hello World!", L"Hello World!", MB_OK );
	}

#endif

