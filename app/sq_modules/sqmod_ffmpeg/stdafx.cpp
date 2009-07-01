// stdafx.cpp

#include "stdafx.h"

// Export Functions
SQBIND_REGISTER_CLASS_BEGIN( CFfDecoder, CFfDecoder )
	
	SQBIND_MEMBER_FUNCTION( CFfDecoder, Destroy )
	SQBIND_MEMBER_FUNCTION( CFfDecoder, Create )

	// Codec types	
	SQBIND_GLOBALCONST( CODEC_ID_H264 )
	
SQBIND_REGISTER_CLASS_END()
DECLARE_INSTANCE_TYPE( CFfDecoder );

// Export classes
static void SQBIND_Export_ffmpeg( sqbind::VM x_vm )
{
	if ( !oexCHECK_PTR( x_vm ) )
		return;

    SQBIND_EXPORT( x_vm, CFfDecoder );
}

#if defined( SQBIND_STATIC )
	#include "ff_decoder.cpp"
#else

	static void SQBIND_Export( sqbind::VM x_vm )
	{	SQBIND_Export_ffmpeg( x_vm ); }

	// Include squirrel module exported symbols
	#include <sqmod_extern.hpp>

#endif
