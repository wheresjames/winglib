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

SQBIND_REGISTER_CLASS_BEGIN( sqbind::CSqDataLog, CSqDataLog )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqDataLog, Destroy )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqDataLog, setRoot )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqDataLog, getRoot )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqDataLog, AddKey )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqDataLog, RemoveKey )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqDataLog, FindKey )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqDataLog, Log )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqDataLog, Flush )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqDataLog, getKeyList )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqDataLog, getLog )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqDataLog, getLogBin )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqDataLog, getBufferSize )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqDataLog, setLogParams )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqDataLog, getBase )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqDataLog, getStep )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqDataLog, getLimit )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqDataLog, setLimit )
	
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqDataLog, itStart )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqDataLog, itNext )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqDataLog, itPrev )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqDataLog, itRead )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqDataLog, itClose )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqDataLog, itTime )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqDataLog, itTimeMs )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqDataLog, itSize )
	
SQBIND_REGISTER_CLASS_END()

void CSqDataLog::Register( sqbind::VM vm )
{_STT();
	SQBIND_EXPORT( vm, CSqDataLog );
}

CSqMulti CSqDataLog::getKeyList( SQINT nTime )
{	CSqMulti m;
	oex::CPropertyBag pb( m_dl.GetKeyList( nTime ) );
	SQBIND_PropertyBagToMulti( pb, m );
	return m;
}

CSqMulti CSqDataLog::getLog( const stdString &sKey, SQINT nStart, SQINT nEnd, SQINT nInterval, SQINT nType, SQINT nMethod )
{	CSqMulti m;
	oex::CPropertyBag pb( m_dl.GetLog( sKey.c_str(), nStart, nEnd, nInterval, nType, nMethod ) );
	SQBIND_PropertyBagToMulti( pb, m );
	return m;
}
