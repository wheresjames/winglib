/*------------------------------------------------------------------
// oexlib_typedefs.h
// Copyright (c) 2006 
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
					
#define oexCONST					const

typedef long						oexRESULT;
		
typedef int							oexINT;
typedef unsigned int				oexUINT;
typedef long						oexLONG;
typedef unsigned long				oexULONG;
typedef short						oexSHORT;
typedef unsigned short				oexUSHORT;
typedef __int32						oexINT32;
typedef unsigned __int32			oexUINT32;
typedef __int64						oexINT64;
typedef unsigned __int64			oexUINT64;
typedef float						oexFLOAT;
typedef double						oexDOUBLE;
typedef bool						oexBOOL;
typedef char						oexCHAR;
typedef unsigned char				oexUCHAR;
								
#ifdef UNICODE
	typedef short					oexTCHAR;
	typedef unsigned short			oexTUCHAR;
#else
	typedef char					oexTCHAR;
	typedef unsigned char			oexTUCHAR;
#endif

typedef oexTCHAR*					oexSTR;
typedef oexCONST oexTCHAR*			oexCSTR;

typedef void*						oexPVOID;
typedef void oexCONST *				oexCPVOID;

#define oexNULL						0

// Our string type
//typedef oexstd::basic_string< oexTCHAR > oexString;

typedef st::stString oexString;

typedef struct _oexGUID {
    unsigned long  Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char  Data4[ 8 ];
} oexGUID;