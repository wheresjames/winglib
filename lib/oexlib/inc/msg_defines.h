/*------------------------------------------------------------------
// msg_defines.h
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

#pragma once

// This stuff is like CORBA in a bottle ;)

/// Creates a guid by hashing the parameter
#define oexCreateMsgId( s, g )                  OEX_NAMESPACE::oss::CMd5::Transform( g, OEX_NAMESPACE::obj::Ptr( s ), OEX_NAMESPACE::obj::Size( s ) )
#define oexCreateTempMsgId( s )                 oexCreateMsgId( s, &OEX_NAMESPACE::oexGUID() )
//#define oexCreateTempMsgId( s )                 OEX_NAMESPACE::oss::CMd5::Transform( OEX_NAMESPACE::oexGUID(), OEX_NAMESPACE::obj::Ptr( s ), OEX_NAMESPACE::obj::Size( s ) )

/// Registers a class pointer and function 
#define oexMsgRegisterFunction( p, c, f )       msgRegister( *oexCreateTempMsgId( oexT( #f ) ), \
                                                             OEX_NAMESPACE::CMsgTarget( (const c*)p, &c::f ) );

/// Registers a function using the 'this' pointer
#define oexMsgRegisterThisFunction( c, f )      oexMsgRegisterFunction( this, c, f )

/// Use to create a CMsg object
#define oexMsg                                  OEX_NAMESPACE::CMsg::Msg

/// Creates a CMsgAddress address object
#define oexTo                                   &OEX_NAMESPACE::CMsgAddress

/// Routes a message
#define oexNet                                  OEX_NAMESPACE::CMsgMgr::Mm()

/// Message priority mask
#define oexMsgPriorityMask                      0x0000ffff

/// Set the flag to indicate a reply is not required
#define oexMsgReply                             0x80000000
#define oexMsgNoReply                           0x40000000
#define oexMsgDirectReply                       0x20000000


