/*------------------------------------------------------------------
// stlib.h
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
/*! \mainpage stacktrace - Base classes

  \section sec_intro Introduction

	Provides global exception handling, logging, and profiling.


  \subsection sub_linking Linking to your project

		-	It is recommended you add something to the following
			to your projects main header or StdAfx.h file.  If your
			application path does not match the above, you will need
			to modify the paths in the code section below

		\code
		
	#include "..\..\stlib\stlib.h"
	#ifdef _DEBUG
	#pragma comment( lib, "..\\lib\\stlib\\Debug\\stlib.lib" )
	#else
	#pragma comment( lib, "..\\lib\\stlib\\Release\\stlib.lib" )
	#endif

		\endcode

		-	You could also add the <b> dsl </b> folder to
			your include path.  Manually link the libs, and
			add to your headers something to the effect of...

		\code
	
	#include "stlib.h"	

		\endcode

  \subsection sub_compile Compiling this library.

	To compile this library on Windows you will need the following

		- Microsoft Developer Studio Visual C++ 6 Service Pack 6

		- Microsoft Platform SDK
			-#	Install and update include paths

	You may also want to install

		- Doxygen
			-#	http://sourceforge.net/projects/doxygen/

		- Graphviz
			-#	http://www.graphviz.org/


  \subsection sub_tools Useful Tools

		-	Tortoise CVS
			-#	http://www.tortoisecvs.org/

		-	Winmerge
			-#	http://winmerge.sourceforge.net/

		-	Tortoise SVN
			-#	http://tortoisesvn.tigris.org/

*/

#pragma once

#include "config.h"

#include "inc/st_namespace.h"

STLIB_START_NAMESPACE

#include "inc/st_typedefs.h"
#include "inc/st_debugging.h"
#include "inc/st_exceptions.h"

// OS
#include "src/os/mutex.h" 
#include "src/os/hq_timer.h"
#include "src/os/tls.h"
#include "src/os/string.h"

#include "inc/asso_list.h"
#include "inc/thread_safe.h"
#include "inc/memory.h"

#include "inc/stack_trace.h"
//#include "inc/stack_report.h"

STLIB_END_NAMESPACE
