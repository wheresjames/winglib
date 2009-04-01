
#if !defined( OEX_WINCE )

#include "stdio.h"

int main(int argc, char* argv[])
{
	printf( "Hello World!\n" );

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

