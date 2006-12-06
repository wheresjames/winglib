/*------------------------------------------------------------------
// st_typedefs.h
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
					
#define stCONST						const

typedef long						stRESULT;
		
typedef int							stINT;
typedef unsigned int				stUINT;
typedef long						stLONG;
typedef unsigned long				stULONG;
typedef short						stSHORT;
typedef unsigned short				stUSHORT;
typedef __int32						stINT32;
typedef unsigned __int32			stUINT32;
typedef __int64						stINT64;
typedef unsigned __int64			stUINT64;
typedef float						stFLOAT;
typedef double						stDOUBLE;
typedef bool						stBOOL;
typedef char						stCHAR;
typedef unsigned char				stUCHAR;
								
#ifdef UNICODE
	typedef short					stTCHAR;
	typedef unsigned short			stTUCHAR;
#else
	typedef char					stTCHAR;
	typedef unsigned char			stTUCHAR;
#endif

typedef stTCHAR*					stSTR;
typedef stCONST stTCHAR*			stCSTR;

typedef void*						stPVOID;
typedef void stCONST *				stCPVOID;

#define stNULL						0

//#define stMAP						stlib_std::map

// Unicode
#define stT

const stBOOL stFALSE = false;
const stBOOL stTRUE = true;

// Our string type
//typedef stlib_std::basic_string< stTCHAR > stString;							  

typedef struct _stGUID {
    unsigned long  Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char  Data4[ 8 ];
} stGUID;