/*------------------------------------------------------------------
// oexlib.h
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
/*! \mainpage oex - Base classes

  \section sec_intro Introduction

    - <em>"In movies they call it suspension of disbelief.
          In programming we call it the user interface." </em>
        - http://www.scripting.com/davenet/1997/05/07/Programmers.html

    - <em>"I call it a Kaiser blade ... mmm, hmmmm" </em>
        - Karl Childers

    <b>Goals</b>

    -   This library should have no other library dependencies.

    -   OS dependencies will be isolated as to foster the future
        move to other platforms such as Linux and embedded architectures.

    -   Built in flexible and secure memory management, including
        text parsing and other string manipulation.

    -   Basic debugging and logging information.

    -   Post-mortem crash reporting and logging.

    -   Provide optional Squirrel-Script hooks.

  \subsection sec_tips Tips and Guidlines
    
    -   Create tests for new classes.  See <b>test_functions.h</b>.
        Before fixing a bug, create a test that exemplifies it, then
        fix the bug.

    -   +++ is used as the TODO marker.  Search the code base for +++
        to get a list of known issues and task list.

    -   !!! is used to mark hacks that may cause trouble or flags other important
        information.  Sometimes a perfect solution doesn't reaveal itself.  This 
        marks things that may need further work.

    -   iii Flags some important bit of information about how the code works.

    -   Maintain a 'clean' namespace, do not require the introduction 
        of OS or library specific declarations in the namespace.  Don't 
        add things like <b>#include &lt;Windows.h&gt;</b> or <b>#include &lt;string&gt;</b> to 
        header files.  Isolate dependencies into the <b>os</b> folder.

    -   Do *not* use tabs.  Use spaces only.  This library is meant to be cross 
        platform and all editors do not support tabs the same way.

    -   All functionality should be well documented using Doxygen.

    -   Hungarian notation would be nice.  Currently using

        <b>Variables</b>

        -   m_      =   member variable
        -   g_      =   global variable
        -   c_      =   constant
        -   s_      =   static
        -   t_      =   typedef
        -   x_      =   function parameter
        -   T_      =   template argument

        -   p       =   pointer
        -   r       =   reference
        -   sz      =   raw string buffer
        -   s       =   string class
        -   us      =   unsafe string, raw user input
        -   b       =   boolean
        -   l       =   long
        -   ul      =   unsigned long
        -   n       =   int
        -   un      =   unsigned int
        -   w       =   short
        -   uw      =   unsigned short
        -   d       =   double
        -   f       =   float
        -   c       =   class
        -   ch      =   char
        -   uc      =   unsigned char
        -   t       =   template argument type
        -   _       =   specialized function or variable for internal use
        -   CAPS    =   macro (#define)
        -   a       =   array or vector
        -   fn      =   function
        -   it      =   iterator
        -   h       =   handle
        -   x       =   Cartesian x value 
        -   y       =   Cartesian y value 
        -   pt      =   Point
        -   rt      =   Rectangle

        <b>Functions</b>
        -   v       =   variable arguments
        -   vp      =   pointer to variable arguments
        -   get     =   returns a member variable
        -   set     =   sets a member variable
        -   _       =   very specialized function or variable for internal use


  \subsection sec_depends Dependencies

	This library is designed to have no external dependencies.
    But here are the exceptions...

  \subsection sub_linking Linking to your project

		-	It is recommended you add something to the following
			to your projects main header or <b> StdAfx.h </b> file.  

		\code
		
	#include "../../lib/oexlib/oexlib.h"

		\endcode

		-	And to you're <b> StdAfx.cpp </b> or equivalent, add

		\code

	#ifdef _DEBUG
	#pragma comment( lib, "../../lib/oexlib/Debug/oexlib.lib" )
	#else
	#pragma comment( lib, "../../lib/oexlib/Release/oexlib.lib" )
	#endif

		\endcode

		-	You could also add the <b> oexlib </b> folder to
			your include path.  Add the libs to your project
			settings, and add to your headers something to 
			the effect of...

		\code
	
	#include "oexlib.h"	

		\endcode

  \subsection sub_using Using this library.

	Before using any functions in this library, be sure and call
	oex::COex::Init().  Before your program shuts down,
	call oex::COex::Uninit().  You may also want to run the 
    self test functions after initialization, at least in the debug
	mode.

	\code

	// Initialize oexlib
	oex::COex::Init();

	// Run self tests
	oex::CTestManager::Run();


	// ... Use oexlib ... 


	// Cleanup oexlib
	oex::COex::Uninit();

	\endcode
				

  \subsection sub_compile Compiling this library.

	To compile this library on Windows you may need the following

		- Microsoft Developer Studio Visual C++

		- Microsoft Platform SDK
			-	Install and update include paths

	To generate documentation, you may also want to install

		- Doxygen
			-	http://sourceforge.net/projects/doxygen/

		- Graphviz
			-	http://www.graphviz.org/


  \subsection sub_tools Useful Tools

		-	Tortoise CVS
			-	http://www.tortoisecvs.org/

		-	Tortoise SVN
			-	http://tortoisesvn.tigris.org/

		-	Winmerge
			-	http://winmerge.sourceforge.net/


  \subsection sub_references References

		- Member Function Pointers and the Fastest Possible C++ Delegates (Don Clugston)
			-	http://www.codeproject.com/cpp/FastDelegate.asp

		- The Impossibly Fast C++ Delegates (Sergey Ryazanov)
			-	http://www.codeproject.com/cpp/ImpossiblyFastCppDelegate.asp

		- RMI for C++ (Jarl Lindrud)
			-	http://www.codeproject.com/threads/RMI_For_Cpp.asp

		- CORBA / ACE / TAO
			-	http://www.cs.wustl.edu/~schmidt/corba.html

		- STLport - You may wish to use this library instead of the 
					one included with your compiler.
			-	http://www.stlport.org/

		- Boost - Lot's of tricks here
			-	http://www.boost.org/


	\subsection sub_design_philosophy  Design Philosophy

	Guiding principles that provide direction when facing real world problems.

	- <b> Simpler is better </b> - 
		Usually simpler implementations are better. There should not be features 
		added to the code base that are not needed to complete the application. 
		Some problems are inherently complex. For these situations, complexity 
		should be isolated as much as possible. Attempt to reduce complex problems 
		to simple interfaces.

	- <b> Make extensive use of existing libraries </b> - 
		Don't re-invent the wheel. Lot's of good free libraries exist. These 
		should be utilized as often as possible. Many times it is better to accept 
		slightly less functionality for an already proven and developed framework 
		and idea.   Be sure you know where the value of your product lies. It will 
		help in making these decisions.

	- <b> Prevent unmaintainable code </b> - 
		Hacked together code is far more trouble than it's worth. Debugging times 
		far exceed the time it takes to develop good design.  Throwing away source 
		code can be good.  Especially when the new functionality is significantly 
		different than existing functionality.  It can be faster, more productive,
        more reliable, ( and often more enjoyable ) to just start over on a design.

	- <b> New releases often have less source code then their predecessors </b> - 
		Trimming away features that are not used will simplify the code base and make
		software easier to maintain and extend. Features that are later determined to 
		be useless should be dropped and not supported endlessly.  After all, it's
        in source control should you need it again, right, right???
		
	- <b> Open architecture results in higher levels of functionality and reliability </b> - 
		The basic framework of an application does not usually hold any significant 
		commercial value, however does pose significant possibility for bugs and 
		limitations. Open frameworks and architectures provide great amounts of 
		productivity by leveraging the work of others.  Look at Boost for example.  
        Consider carefully the option of using existing open frameworks.

	- <b> Modular architecture supports high source code reuse </b> - 
		Code that is broken down into modules and libraries is easier to maintain and 
		extend.  Libraries are easier to debug than large single code bases.

	- <b> When in doubt, throw away and rewrite </b> - 
		Experience says, usually throwing out questionable code is faster and better 
		than attempting to 'code around' it's limitations. Failing to heed this will 
		surely result in many problems and long painful debugging.

	- <b> Design based on functionality </b> - 
		Design should be based on required functionality. Spending the time up front 
		to eliminate features that are not needed will pay off in the long run in 
		shorter development and debugging cycles.

	- <b> Focus on customer goals </b> - 
		Decisions should always be made with your customers goals in mind.  To often,
		time and money is spent on features customers don't need or won't use, and 
        sometimes, don't want.  These efforts are a waste of time and money, and 
        eventually a liability when bugs crop up in and around these useless features.


	\subsection sub_design_implementation  Implementation

	These items require specific actions or describe implementation details.

	- <b> Have a complete feature list in hand before starting </b> - 
		Work out all the requirements, at the macro level before starting development. 
		This includes installation, documentation, and deployment. Then try hard to 
		resist changes.  It's much better to roll changes into the next version which 
		can come quickly if there is diligent restraint to making changes to the feature 
		list.

	- <b> Zero bug policy </b> - 
		Fix existing bugs before adding new features. Bugs get harder and more expensive 
		to fix as time goes on. In extreme cases, unfixed bugs can doom an entire project,
		forcing a complete re-design before the opportunity to capitalize on any of the 
        current effort.  
		
	- <b> Debugging plan </b> - 
		This is a very important element that is often forgotten. Before implementing a 
		product, plan out how it will be deployed and used. Devise a strategy for collecting 
		customer feedback, memory dumps, stack traces, error logs, etc... This should be as 
		automated as possible. Adding this as an after thought can be very time consuming 
		and can extend project time-lines significantly.  <em>PS: The tighter your deadlines,
		the more you'll be needing this ;)</em>

	- <b> Installation and documentation </b> - 
		Even if the developers are not responsible for this, a plan should be in place and
		implications known before starting development. If tech-writers are available, it 
		is possible for developers to create Doxygen-like commenting that is sufficient for 
		techwriters to transcribe into final documentation. This can greatly streamline an 
		area that developers notoriously find tedious.

	- <b> Automatic testing in libraries </b> - 
		If libraries are to be maintained and reused. Automatic testing will make it much 
		easier to extend without breaking current functionality. When a bug is found in a 
		library, a test should first be written that identifies the bug. Then fix the bug.

	- <b> Code based documentation system </b> - 
		Doxygen-like documentation is probably the easiest documentation method for
		programmers since the documentation is in the actual source code.   Take note
		here programmers.  Much of the time spent writting technical descriptions for 
		tech writers can be done here much less painfully.  The professionalism alone 
		makes this worth while.

	- <b> Code versioning system </b> - 
		The days of logging changes in file headers and source comments are over. As is
		saving backups into zip files. Code versioning systems such as Subversion, CVS and
		Source Safe not only backup code, but provide huge debugging help by allowing you to
		see when and where changes to code were made, and by who.

	- <b> Automatic builds </b> - 
		For large projects it is vital that builds be made by a single batch file or utility. 
		It is worth the time it takes to set this up and maintain it. Complicating the build 
		cycle will result in 'stupid' mistakes and eat up enormous amounts of time trying to 
		get the build 'right' from memory. Check lists are also inadequate, mistakes will be 
		made and it is too time consuming. Shot for nothing less than complete automation. In 
		fact, having a dedicated machine that executes completely autonomous nightly builds 
		from the code versioning system is becoming the norm. This helps greatly when trying 
		to determine exactly when a bug is introduced.

	- <b> Automatic bug tracking </b> - 
		A list of current issues cannot be kept in your head. Some method of bug tracking needs 
		to be in place. Ideally, an automated way of gathering this information from users would 
		be used.


	\subsection sub_design_references References

    - No Silver Bullet
        - http://www-inst.eecs.berkeley.edu/~maratb/readings/NoSilverBullet.html

	- Measuring Bugs
		- http://www.ganssle.com/articles/Measuringbugs.htm

	- Lines of code
		- http://www.developer.com/java/other/article.php/988641

	- The stand alone programmer
		- http://www.codeproject.com/gen/work/standaloneprogrammer2.asp

	- Microsoft practices
		- http://www.stsc.hill.af.mil/crosstalk/1995/10/Shippin.asp

	- Joel test
		- http://www.joelonsoftware.com/articles/fog0000000043.html

    - Work vs. Progress
        - http://www.scottberkun.com/essays/essay45.htm

    - Maintainable Programmers
        - http://lesscode.org/2005/12/30/maintainable-programmers/

*/

#pragma once

//------------------------------------------------------------------
// External Dependencies
//------------------------------------------------------------------
// Please don't pollute the namespace ;)

//------------------------------------------------------------------
// Configuration
//------------------------------------------------------------------
#include "inc/oex_config.h"
#include "inc/oex_namespace.h"
#include "inc/oex_global.h"

//------------------------------------------------------------------
// Begin oex namespace
//------------------------------------------------------------------
OEX_START_NAMESPACE

//------------------------------------------------------------------
// Typedefs, defines, Macros, etc...
//------------------------------------------------------------------
#include "inc/oex_defines.h"
#include "inc/oex_typedefs.h"
#include "inc/oex_const.h"
#include "inc/oex_exceptions.h"
#include "inc/oex_common.h"

//------------------------------------------------------------------
// Platform independent debugging functions
//------------------------------------------------------------------
namespace os
{
#   include "inc/os/debug.h"
};

#include "inc/oex_debugging.h"

//------------------------------------------------------------------
// OS Specific
//------------------------------------------------------------------
namespace os
{
#   include "inc/os/sys.h"
#   include "inc/os/fmap.h"
#   include "inc/os/mem.h"
#   include "inc/os/tls.h"
#   include "inc/os/mutex.h"
#   include "inc/os/event.h"
#   include "inc/os/hq_timer.h"
};

//------------------------------------------------------------------
// Supporting classes
//------------------------------------------------------------------
#include "inc/oex_str.h"
#include "inc/alloc.h"
#include "inc/file_mapping.h"
#include "inc/memory.h"
#include "inc/thread_safe.h"
#include "inc/str.h"
#include "inc/list.h"
#include "inc/asso_list.h"
#include "inc/property_bag.h"
#include "inc/sys_time.h"
#include "inc/md5.h"

/*
*/
//#include "src/stack_trace.h"

//------------------------------------------------------------------
// Dependent OS Specific
//------------------------------------------------------------------
namespace os
{

#   include "inc/os/base_file.h"
#   include "inc/os/ip_address.h"
#   include "inc/os/ip_socket.h"
//#   include "inc/os/thread.h"    

};

//------------------------------------------------------------------
// Automatic testing
//------------------------------------------------------------------
//#include "inc/lib_test.h"

//------------------------------------------------------------------
// Implementation
//------------------------------------------------------------------
#include "inc/parser.h"
#include "inc/file.h"
#include "inc/zip.h"
#include "inc/unzip.h"
#include "inc/find_files.h"
#include "inc/circ_buf.h"
#include "inc/fifo_sync.h"
#include "inc/data_packet.h"


/*
#include "inc/dispatch.h"
#include "inc/file.h"
#include "inc/zip.h"
#include "inc/unzip.h"
#include "inc/parser.h"
#include "inc/find_files.h"
#include "inc/circ_buf.h"
#include "inc/fifo_sync.h"
#include "inc/data_packet.h"
//#include "inc/thread_pool.h"
//#include "inc/auto_socket.h"
//#include "inc/auto_server.h"
//#include "inc/wsp_stream.h"
//#include "inc/http_session.h"
//#include "inc/ftp_session.h"
//#include "inc/virtual_fs.h"
//#include "inc/vfs_ftp_session.h"
*/

//------------------------------------------------------------------
// Init / Uninit
//------------------------------------------------------------------

//==================================================================
// COexlib
//
/// This class is just for containing the startup / shutdown code
/**

	Before using any functions in this library, be sure and call
	oex::COex::Init().  Before your program shuts down,
	call oex::COex::Uninit().  You may also want to run the self
	test functions after initialization, at least in the debug
	version.

	\code

	// Initialize oexlib
	oex::COexlib::Init();

	// Run self tests
	oex::CTestManager::Run();


	// ... Use oexlib ... 


	// Cleanup oexlib
	oex::COexlib::Uninit();

	\endcode
	
*/
//==================================================================
class COex
{
public:

	class CVerifyStartup
	{
	public:

		CVerifyStartup(){}

		virtual ~CVerifyStartup()
		{
			if ( !COex::GetStartupCode() )
			{	oexTRACE( "! oexlib - oex::COex::Init() was not called!\n" );
				oexBREAK( "oex::COex::Init() was not called!" );
			} // end if
			else if ( 0 > COex::GetStartupCode() )
			{	oexTRACE( "! oexlib - oex::COex::Init() reported error code %li\n", COex::GetStartupCode() );
				oexBREAK( "oex::COex::Init() reported an error" );
			} // end else if

			if ( !COex::GetShutdownCode() )
			{	oexTRACE( "! oexlib - oex::COex::Uninit() was not called!\n" );
				oexBREAK( "oex::COex::Uninit() was not called!" );
			} // end if
			else if ( 0 > COex::GetShutdownCode() )
			{	oexTRACE( "! oexlib - oex::COex::Uninit() reported error code %li\n", COex::GetShutdownCode() );
				oexBREAK( "oex::COex::Uninit() reported an error" );
			} // end else if
		}
	};

private:
	COex() {}
	virtual ~COex() {}

public:

	//==============================================================
	// Init()
	//==============================================================
	/// Call this function before using the oex library
	/**

		Call before using any oexlib functionality.
		
		\return Positive non-zero value on success.  If this function
				fails, the return value is negative.
	
		\see Uninit()
	*/
	static oexINT Init();

	//==============================================================
	// Uninit()
	//==============================================================
	/// Call this function to clean up the oex library
	/**
		Call this function before your program shuts down.
		
		\return Positive non-zero value on success.  If this function
				fails, the return value is negative.
	
		\see Init()
	*/
	static oexINT Uninit();

	/// Returns the startup code
	static oexINT GetStartupCode()
	{	return m_nStartupCode; }

	/// Returns the startup code
	static oexINT GetShutdownCode()
	{	return m_nShutdownCode; }

private:

	/// oexlib startup result
	static oexINT				m_nStartupCode;

	/// oexlib shutdown result
	static oexINT				m_nShutdownCode;

	/// This is just to warn of improper initialization or shutdown
	static CVerifyStartup		m_cVerifyStartup;
};

OEX_END_NAMESPACE
