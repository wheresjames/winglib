// stdafx.cpp

#include "stdafx.h"

extern "C" void assert(int expression)
{
}


// Export Functions
SQBIND_REGISTER_CLASS_BEGIN( CFfDecoder, CFfDecoder )

	SQBIND_MEMBER_FUNCTION( CFfDecoder, Destroy )
	SQBIND_MEMBER_FUNCTION( CFfDecoder, Create )

	// Codec types
	SQBIND_GLOBALCONST( CODEC_ID_H264 )
	SQBIND_GLOBALCONST( CODEC_ID_MPEG4 )

SQBIND_REGISTER_CLASS_END()
DECLARE_INSTANCE_TYPE( CFfDecoder );

SQBIND_REGISTER_CLASS_BEGIN( CFfEncoder, CFfEncoder )

	SQBIND_MEMBER_FUNCTION( CFfEncoder, Destroy )
	SQBIND_MEMBER_FUNCTION( CFfEncoder, Create )

	// Codec types
	SQBIND_GLOBALCONST( CODEC_ID_MPEG4 )

SQBIND_REGISTER_CLASS_END()
DECLARE_INSTANCE_TYPE( CFfEncoder );

// Export classes
static void SQBIND_Export_ffmpeg( sqbind::VM x_vm )
{
	if ( !oexCHECK_PTR( x_vm ) )
		return;

    SQBIND_EXPORT( x_vm, CFfDecoder );
    SQBIND_EXPORT( x_vm, CFfEncoder );
}

#if defined( SQBIND_STATIC )
	#include "ff_decoder.cpp"
	#include "ff_encoder.cpp"
#else

	static void SQBIND_Export( sqbind::VM x_vm )
	{	SQBIND_Export_ffmpeg( x_vm ); }

	// Include squirrel module exported symbols
	#include <sqmod_extern.hpp>

#endif
