/*------------------------------------------------------------------
// sq_data_log.cpp
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

#include "../stdafx.h"

using namespace sqbind;

SQBIND_REGISTER_CLASS_BEGIN( sqbind::CSqTime, CSqTime )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, GetLocalTime )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, SetLocalTime )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, GetSystemTime )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, SetSystemTime )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, SetTime )

	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, GetYear )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, GetMonth )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, GetDay )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, GetDayOfWeek )	
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
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, SetDayOfWeek )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, SetHour )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, Set12Hour )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, SetMinute )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, SetSecond )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, SetMilliSecond )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, SetMicroSecond )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, SetNanoSecond )

	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, IsPM )

	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, SetTzBias )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, GetTzBias )

	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, GetTzUnixTime )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, GetUnixTime )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, SetUnixTime )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, GetDosTime )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, SetDosTime )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, GetNetTime )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, SetNetTime )

	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, FormatTime )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, ParseTime )

	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, setEscapeChar )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, getEscapeChar )

	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, getAbrMonthName )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, getMonthName )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, getAbrDayName )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTime, getDayName )

SQBIND_REGISTER_CLASS_END()


void CSqTime::Register( sqbind::VM vm )
{_STT();
	SQBIND_EXPORT( vm, CSqTime );
}

SQBIND_REGISTER_CLASS_BEGIN( sqbind::CSqTimeRange, CSqTimeRange )

	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTimeRange, getStart )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTimeRange, setStart )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTimeRange, getStartMs )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTimeRange, setStartMs )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTimeRange, setStartStr )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTimeRange, getStartStr )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTimeRange, getEnd )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTimeRange, setEnd )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTimeRange, getEndMs )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTimeRange, setEndMs )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTimeRange, setEndStr )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTimeRange, getEndStr )

	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTimeRange, CalculateTime )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTimeRange, CompareStart )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqTimeRange, CompareEnd )

SQBIND_REGISTER_CLASS_END()

void CSqTimeRange::Register( sqbind::VM vm )
{_STT();
	SQBIND_EXPORT( vm, CSqTimeRange );
}
