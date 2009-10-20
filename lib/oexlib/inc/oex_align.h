/*------------------------------------------------------------------
// oex_align.h
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

#if defined( oexDEBUG )

#	if defined( OEX_GCC )
#		define oexDeclAligned( n, t, v )	t v __attribute__ ( ( aligned( n ) ) )
#	elif defined( OEX_MSC )
#		define oexDeclAligned( n, t, v )	__declspec( align( n ) ) t v
#	else
#		define oexDeclAligned( n, t, v )	t v
#	endif

#else

#	if defined( OEX_GCC )
#		define oexDeclAligned( n, t, v )	t v __attribute__ ( ( aligned( n ) ) )
#	elif defined( OEX_MSC )
#		define oexDeclAligned( n, t, v )	__declspec( align( n ) ) t v
#	else
#		define oexDeclAligned( n, t, v )	t v
#	endif

#endif

#define oexAligned( t, v )					oexDeclAligned( oexSIZEOFINT, t, v )
#define oexAligned2( t, v )					oexDeclAligned( 2, t, v )
#define oexAligned4( t, v )					oexDeclAligned( 4, t, v )
#define oexAligned8( t, v )					oexDeclAligned( 8, t, v )
#define oexAligned16( t, v )				oexDeclAligned( 16, t, v )
#define oexAligned32( t, v )				oexDeclAligned( 32, t, v )
#define oexAligned64( t, v )				oexDeclAligned( 64, t, v )

#define oexIsAligned( v )					OEX_NAMESPACE::cmn::IsAligned( ( v ) )
#define oexIsAligned2( v )					OEX_NAMESPACE::cmn::IsAligned2( (oexLONG)( v ) )
#define oexIsAligned4( v )					OEX_NAMESPACE::cmn::IsAligned4( (oexLONG)( v ) )
#define oexIsAligned8( v )					OEX_NAMESPACE::cmn::IsAligned8( (oexLONG)( v ) )
#define oexIsAligned16( v )					OEX_NAMESPACE::cmn::IsAligned16( (oexLONG)( v ) )
#define oexIsAligned32( v )					OEX_NAMESPACE::cmn::IsAligned32( (oexLONG)( v ) )
#define oexIsAligned64( v )					OEX_NAMESPACE::cmn::IsAligned64( (oexLONG)( v ) )

#if defined( __OEX_GCC )

#	define oexNaked
#	define oexAlignStack( n )					\
		__asm__ volatile (						\
			"mov	%ebx, %esp			\n\t"	\
			"and	%esp, ~( n - 1 )	\n\t"	\
			"sub	%esp, 12			\n\t"	\
			"push	%ebx				\n\t"	\
		);

#	define oexRestoreStack()					\
		__asm__ volatile (						\
			"pop	%ebx				\n\t"	\
			"mov	%esp, %ebx			\n\t"	\
		);
#	define oexAlignStackNaked( n )	oexAlignStack( n )
#	define oexRestoreStackNaked()	oexRestoreStack()

#	define oexAsmRet()

#elif defined( OEX_MSC )

#	define oexNaked							__declspec( naked )

#	define oexAlignStack( n )					\
		__asm { mov		ebx, esp			}	\
		__asm { and		esp, ~( n - 1 )		}	\
		__asm { sub		esp, 12				}	\
		__asm { push	ebx					}

#	define oexRestoreStack()					\
		__asm { pop		ebx					}	\
		__asm { mov		esp, ebx			}

#	define oexAlignStackNaked( n )				\
		__asm { push	ebp					}	\
		__asm { mov		ebp, esp			}	\
		__asm { sub		esp, __LOCAL_SIZE	}	\
		__asm { and		esp, ~( n - 1 )		}	\

#	define oexRestoreStackNaked()				\
		__asm { mov		esp, ebp			}	\
		__asm { pop		ebp					}

#	define oexAsmRet() __asm { ret }

#else

#	define oexNaked							// __attribute__ ((naked))
#	define oexAlignStack( n )
#	define oexRestoreStack()
#	define oexAlignStackNaked( n )
#	define oexRestoreStackNaked()
#	define oexAsmRet()

#endif

#	define oexAlignCall( n, f ) oexAlignStack( n ); f; oexRestoreStack();

// Disable warning about modifying ebp
#if defined( OEX_MSC )
#	pragma warning( push )
#	pragma warning( disable : 4731 )
#endif

#define oex_align_tmpl_P0		typename T_F
#define oex_align_decl_P0		T_F f
#define oex_align_call_P0

#define oex_align_tmpl_P1		typename T_F, typename T_P1
#define oex_align_decl_P1		T_F f, T_P1 p1
#define oex_align_call_P1		p1

#define oex_align_tmpl_P2		typename T_F, typename T_P1, typename T_P2
#define oex_align_decl_P2		T_F f, T_P1 p1, T_P2 p2
#define oex_align_call_P2		p1, p2

#define oex_align_tmpl_P3		typename T_F, typename T_P1, typename T_P2, typename T_P3
#define oex_align_decl_P3		T_F f, T_P1 p1, T_P2 p2, T_P3 p3
#define oex_align_call_P3		p1, p2, p3

#define oex_align_tmpl_P4		typename T_F, typename T_P1, typename T_P2, typename T_P3, typename T_P4
#define oex_align_decl_P4		T_F f, T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4
#define oex_align_call_P4		p1, p2, p3, p4

#define oex_align_tmpl_P5		typename T_F, typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5
#define oex_align_decl_P5		T_F f, T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4, T_P5 p5
#define oex_align_call_P5		p1, p2, p3, p4, p5

#define oex_align_tmpl_P6		typename T_F, typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5, typename T_P6
#define oex_align_decl_P6		T_F f, T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4, T_P5 p5, T_P6 p6
#define oex_align_call_P6		p1, p2, p3, p4, p5, p6

#define oex_align_tmpl_P7		typename T_F, typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5, typename T_P6, typename T_P7
#define oex_align_decl_P7		T_F f, T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4, T_P5 p5, T_P6 p6, T_P7 p7
#define oex_align_call_P7		p1, p2, p3, p4, p5, p6, p7

#define oex_align_tmpl_P8		typename T_F, typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5, typename T_P6, typename T_P7, typename T_P8
#define oex_align_decl_P8		T_F f, T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4, T_P5 p5, T_P6 p6, T_P7 p7, T_P8 p8
#define oex_align_call_P8		p1, p2, p3, p4, p5, p6, p7, p8

#define oex_align_tmpl_P9		typename T_F, typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5, typename T_P6, typename T_P7, typename T_P8, typename T_P9
#define oex_align_decl_P9		T_F f, T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4, T_P5 p5, T_P6 p6, T_P7 p7, T_P8 p8, T_P9 p9
#define oex_align_call_P9		p1, p2, p3, p4, p5, p6, p7, p8, p9

#define oex_align_tmpl_P10		typename T_F, typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5, typename T_P6, typename T_P7, typename T_P8, typename T_P9, typename T_P10
#define oex_align_decl_P10		T_F f, T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4, T_P5 p5, T_P6 p6, T_P7 p7, T_P8 p8, T_P9 p9, T_P10 p10
#define oex_align_call_P10		p1, p2, p3, p4, p5, p6, p7, p8, p9, p10

#define oex_align_fn( n, p )	template < typename T_RET, oex_align_tmpl_P##p > oexNaked void AlignCall##n( T_RET &r, oex_align_decl_P##p ) \
								{ oexAlignStackNaked( n ); r = f( oex_align_call_P##p ); oexRestoreStackNaked(); oexAsmRet(); } \
								template < oex_align_tmpl_P##p > oexNaked void AlignCall##n( oexNoRetType, oex_align_decl_P##p ) \
								{ oexAlignStackNaked( n ); f( oex_align_call_P##p ); oexRestoreStackNaked(); oexAsmRet(); }

#define oex_align_decl( n )		oex_align_fn( n, 0 ) oex_align_fn( n, 1 ) oex_align_fn( n, 2 ) oex_align_fn( n, 3 ) \
								oex_align_fn( n, 4 ) oex_align_fn( n, 5 ) oex_align_fn( n, 6 ) oex_align_fn( n, 7 ) \
								oex_align_fn( n, 8 ) oex_align_fn( n, 9 ) oex_align_fn( n, 10 )


// Declare functions
oex_align_decl( 2 )
oex_align_decl( 4 )
oex_align_decl( 8 )
oex_align_decl( 16 )
oex_align_decl( 32 )
oex_align_decl( 64 )

#if defined( OEX_MSC )
#	pragma warning( pop )
#endif
