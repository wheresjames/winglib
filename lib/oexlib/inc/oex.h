/*------------------------------------------------------------------
// oex.h
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
		CVerifyStartup();
		virtual ~CVerifyStartup();
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
	static oexINT GetStartupCode();

	/// Returns the shutdown code
	static oexINT GetShutdownCode();

	/// Returns data for the specified session id
	static CPropertyBag GetSessionData( CStr sId );

	/// Sets the data for the specified session id
	static oexBOOL SetSessionData( CStr sId, CPropertyBag &x_pbSession );

	/// Releases data for the specified session id
	static oexBOOL ReleaseSessionData( CStr sId );

#if defined( oexDEBUG )

	/// Returns a reference to the memory leak detector
	static CMemLeak& GetMemLeak();

#endif

	/// Returns a pointer to the binary buffer share
	static CBinShare& GetBinShare();

	/// Returns a pointer to the binary buffer share
	static COexResourceHelper& GetResourceHelper();

private:

	/// oexlib startup result
	static oexINT				m_nStartupCode;

	/// oexlib shutdown result
	static oexINT				m_nShutdownCode;

	/// This is just to warn of improper initialization or shutdown
	static CVerifyStartup		m_cVerifyStartup;

	/// Resource helper for this module
	static COexResourceHelper	m_cResourceHelper;

	/// Stack trace class
	static CStackTrace			m_cStackTrace;

};
