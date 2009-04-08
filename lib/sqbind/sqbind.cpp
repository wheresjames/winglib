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

_SQBIND_REGISTER_CLASS_BEGIN( sqbind::CSqString, CSqString )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqString, str )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqString, set )
_SQBIND_REGISTER_CLASS_END()

void CSqString::Register( sqbind::VM vm )
{
	_SQBIND_EXPORT( vm, CSqString );
}

_SQBIND_REGISTER_CLASS_BEGIN( sqbind::CSqFile, CSqFile )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqFile, OpenExisting )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqFile, OpenNew )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqFile, Write )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqFile, Read )
	_SQBIND_STATIC_FUNCTION(  sqbind::CSqFile, get_contents )
	_SQBIND_STATIC_FUNCTION(  sqbind::CSqFile, put_contents )
	_SQBIND_STATIC_FUNCTION(  sqbind::CSqFile, append_contents )
	_SQBIND_STATIC_FUNCTION(  sqbind::CSqFile, mkdir )
	_SQBIND_STATIC_FUNCTION(  sqbind::CSqFile, rmdir )
	_SQBIND_STATIC_FUNCTION(  sqbind::CSqFile, exists )
	_SQBIND_STATIC_FUNCTION(  sqbind::CSqFile, get_filename )
	_SQBIND_STATIC_FUNCTION(  sqbind::CSqFile, get_path )
	_SQBIND_STATIC_FUNCTION(  sqbind::CSqFile, get_extension )
_SQBIND_REGISTER_CLASS_END()


void CSqFile::Register( sqbind::VM vm )
{
	_SQBIND_EXPORT( vm, CSqFile );
}

_SQBIND_REGISTER_CLASS_BEGIN( sqbind::CSqTime, CSqTime )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, GetLocalTime )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, GetSystemTime )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, SetTime )

	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, GetYear )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, GetMonth )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, GetDay )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, GetHour )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, Get12Hour )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, GetMinute )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, GetSecond )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, GetMilliSecond )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, GetMicroSecond )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, GetNanoSecond )

	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, SetYear )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, SetMonth )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, SetDay )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, SetHour )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, Set12Hour )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, SetMinute )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, SetSecond )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, SetMilliSecond )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, SetMicroSecond )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, SetNanoSecond )

	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, IsPM )

	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, GetUnixTime )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, SetUnixTime )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, GetDosTime )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, SetDosTime )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, GetNetTime )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, SetNetTime )

	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, FormatTime )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, ParseTime )

_SQBIND_REGISTER_CLASS_END()


void CSqTime::Register( sqbind::VM vm )
{
	_SQBIND_EXPORT( vm, CSqTime );
}

_SQBIND_REGISTER_CLASS_BEGIN( sqbind::CSqImage, CSqImage )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqImage, Load )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqImage, Save )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqImage, Encode )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqImage, Decode )
_SQBIND_REGISTER_CLASS_END()


void CSqImage::Register( sqbind::VM vm )
{
	_SQBIND_EXPORT( vm, CSqImage );
}
