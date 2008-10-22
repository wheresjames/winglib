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
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
//#include <stdio.h>
#include <wchar.h>
#include <string.h>
//#include <inttypes.h>

#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#include <sys/time.h>
#include <sys/times.h>

#include <time.h>
void tzset (void);
extern char *tzname[ 2 ];
extern long timezone;
extern int daylight;

#include <locale.h>
#include <sys/mman.h>

#ifndef OEX_ARM
#	include <uuid/uuid.h>
#endif

#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <execinfo.h>

#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/videodev.h>

#ifndef OEX_SNAPGEAR
#	include <linux/videodev2.h>
#endif

#include <sys/sem.h>

/*
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/videodev.h>
#include <linux/videodev2.h>
#include <sys/mman.h>

#include <errno.h>
*/
