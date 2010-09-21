/*------------------------------------------------------------------
// std_os.h
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

//#define _LARGE_FILES

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <conio.h>

#if !defined( OEX_NODIRENT )
#	include <dirent.h>
#endif

//#include <stdio.h>
#if !defined( OEX_NOWCHAR )
#	include <wchar.h>
#endif

#include <string.h>
//#include <inttypes.h>

#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>

#include <sys/time.h>
#include <sys/times.h>

#include <time.h>
void tzset (void);
extern char *tzname[ 2 ];
extern long timezone;
extern int daylight;

#include <locale.h>

#include <sys/socket.h>
#if !defined( OEX_NOEPOLL )
#	include <sys/epoll.h>
#endif
#include <arpa/inet.h>
#include <netdb.h>

#if !defined( OEX_NOEXECINFO )
#	include <execinfo.h>
#endif

#include <sys/ioctl.h>

#if !defined( OEX_IPHONE )
#	include <linux/types.h>
#endif

#ifndef OEX_NOVIDEO
#	include <linux/videodev.h>
#	ifndef OEX_VFL1
#		include <linux/videodev2.h>
#	endif
#endif

#include <sys/sem.h>

#include <dlfcn.h>

#include <signal.h>

