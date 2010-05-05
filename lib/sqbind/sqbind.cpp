/*------------------------------------------------------------------
// sqbind.cpp
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

#include "stdafx.h"

using namespace sqbind;

namespace sqbind
{
	void SqBindAll( sqbind::VM x_vm )
	{_STT();
		CSqBinary::Register( x_vm );
		CSqString::Register( x_vm );
		CSqVector::Register( x_vm );
		CSqList::Register( x_vm );
		CSqMap::Register( x_vm );
		CSqMulti::Register( x_vm );
		CSqFile::Register( x_vm );
		CSqTime::Register( x_vm );
		CSqImage::Register( x_vm );
		CSqCapture::Register( x_vm );
		CSqSocket::Register( x_vm );
		CSqSockAddress::Register( x_vm );
		CSqHttpServer::Register( x_vm );

#if defined( OEX_ENABLE_SQLITE )
		CSqSQLite::Register( x_vm );
#endif

	};

};

SQBIND_REGISTER_CLASS_BEGIN( sqbind::CSqString, CSqString )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqString, str )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqString, set )
SQBIND_REGISTER_CLASS_END()

void CSqString::Register( sqbind::VM vm )
{_STT();
	SQBIND_EXPORT( vm, CSqString );
}

SQBIND_REGISTER_CLASS_BEGIN( sqbind::CSqFile, CSqFile )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFile, CreateAlways )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFile, OpenAlways )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFile, OpenExisting )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFile, OpenNew )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFile, Close )

	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFile, SetPtrPosBegin )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFile, SetPtrPosEnd )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFile, Size )

	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFile, WriteBin )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFile, ReadBin )
	SQBIND_STATIC_FUNCTION(  sqbind::CSqFile, get_contents_bin )
	SQBIND_STATIC_FUNCTION(  sqbind::CSqFile, put_contents_bin )
	SQBIND_STATIC_FUNCTION(  sqbind::CSqFile, append_contents_bin )

	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFile, Write )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFile, Read )
	SQBIND_STATIC_FUNCTION(  sqbind::CSqFile, get_contents )
	SQBIND_STATIC_FUNCTION(  sqbind::CSqFile, put_contents )
	SQBIND_STATIC_FUNCTION(  sqbind::CSqFile, append_contents )

	SQBIND_STATIC_FUNCTION(  sqbind::CSqFile, mkdir )
	SQBIND_STATIC_FUNCTION(  sqbind::CSqFile, rmdir )
	SQBIND_STATIC_FUNCTION(  sqbind::CSqFile, exists )
	SQBIND_STATIC_FUNCTION(  sqbind::CSqFile, delete_file )
	SQBIND_STATIC_FUNCTION(  sqbind::CSqFile, delete_path )
	SQBIND_STATIC_FUNCTION(  sqbind::CSqFile, rename )
	SQBIND_STATIC_FUNCTION(  sqbind::CSqFile, get_filename )
	SQBIND_STATIC_FUNCTION(  sqbind::CSqFile, get_path )
	SQBIND_STATIC_FUNCTION(  sqbind::CSqFile, get_extension )
	SQBIND_STATIC_FUNCTION(  sqbind::CSqFile, get_dirlist )
SQBIND_REGISTER_CLASS_END()

void CSqFile::Register( sqbind::VM vm )
{_STT();
	SQBIND_EXPORT( vm, CSqFile );
}

SQBIND_REGISTER_CLASS_BEGIN( sqbind::CSqTime, CSqTime )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, GetLocalTime )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, SetLocalTime )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, GetSystemTime )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, SetSystemTime )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, SetTime )

	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, GetYear )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, GetMonth )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, GetDay )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, GetHour )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, Get12Hour )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, GetMinute )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, GetSecond )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, GetMilliSecond )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, GetMicroSecond )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, GetNanoSecond )

	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, SetYear )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, SetMonth )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, SetDay )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, SetHour )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, Set12Hour )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, SetMinute )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, SetSecond )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, SetMilliSecond )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, SetMicroSecond )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, SetNanoSecond )

	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, IsPM )

	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, GetUnixTime )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, SetUnixTime )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, GetDosTime )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, SetDosTime )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, GetNetTime )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, SetNetTime )

	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, FormatTime )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, ParseTime )

SQBIND_REGISTER_CLASS_END()


void CSqTime::Register( sqbind::VM vm )
{_STT();
	SQBIND_EXPORT( vm, CSqTime );
}

SQBIND_REGISTER_CLASS_BEGIN( sqbind::CSqImage, CSqImage )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqImage, Create )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqImage, Load )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqImage, Save )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqImage, Encode )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqImage, Decode )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqImage, getWidth )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqImage, getHeight )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqImage, CopyImage )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqImage, Resample )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqImage, isValid )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqImage, setPixels )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqImage, getPixels )
SQBIND_REGISTER_CLASS_END()

void CSqImage::Register( sqbind::VM vm )
{_STT();
	SQBIND_EXPORT( vm, CSqImage );
}

#define REGISTER_TYPE_ACCESS( t )	\
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, set##t )			\
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, get##t )			\
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, setAbs##t )			\
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, getAbs##t )			\
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, LE_set##t )			\
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, LE_get##t )			\
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, LE_setAbs##t )		\
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, LE_getAbs##t )		\
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, BE_set##t )			\
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, BE_get##t )			\
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, BE_setAbs##t )		\
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, BE_getAbs##t )		\
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, find##t )

SQBIND_REGISTER_CLASS_BEGIN( sqbind::CSqBinary, CSqBinary )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, Allocate )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, Free )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, Resize )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, Zero )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, Copy )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, Append )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, Share )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, Size )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, AsciiHexStr )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, Fingerprint )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, get )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, set )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, SetName )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, GetName )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, setUsed )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, getUsed )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, setString )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, getString )

	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, FindBin )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, Sub )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqBinary, failed )

	REGISTER_TYPE_ACCESS( CHAR )
	REGISTER_TYPE_ACCESS( UCHAR )
	REGISTER_TYPE_ACCESS( SHORT )
	REGISTER_TYPE_ACCESS( USHORT )
	REGISTER_TYPE_ACCESS( INT )
	REGISTER_TYPE_ACCESS( UINT )
	REGISTER_TYPE_ACCESS( LONG )
	REGISTER_TYPE_ACCESS( ULONG )
	REGISTER_TYPE_ACCESS( FLOAT )
	REGISTER_TYPE_ACCESS( DOUBLE )

SQBIND_REGISTER_CLASS_END()

void CSqBinary::Register( sqbind::VM vm )
{_STT();
	SQBIND_EXPORT( vm, CSqBinary );
}
