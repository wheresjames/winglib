
#include "stdafx.h"

SQBIND_REGISTER_CLASS_BEGIN( CSqHash, CSqHash )

	SQBIND_MEMBER_FUNCTION( CSqHash, Destroy )
	SQBIND_MEMBER_FUNCTION( CSqHash, sha1 )

SQBIND_REGISTER_CLASS_END()

void CSqHash::Register( sqbind::VM vm )
{_STT();
	SQBIND_EXPORT( vm, CSqHash );
}

CSqHash::CSqHash()
{_STT();

}

CSqHash::~CSqHash()
{_STT();
}

void CSqHash::Destroy()
{_STT();
}

sqbind::CSqBinary CSqHash::sha1( sqbind::CSqBinary *bin )
{_STT();

	if ( !bin )
		return sqbind::CSqBinary();

	SHA256_CTX context;
	unsigned char md[ SHA256_DIGEST_LENGTH ] = { 0 };
	
    if( !SHA256_Init( &context ) )
		return sqbind::CSqBinary();

    if( !SHA256_Update( &context, (unsigned char*)bin->Ptr(), bin->getUsed() ) )
		return sqbind::CSqBinary();

    if( !SHA256_Final( md, &context ) )
		return sqbind::CSqBinary();
	
	return sqbind::CSqBinary( md, sizeof( md ) );
}


