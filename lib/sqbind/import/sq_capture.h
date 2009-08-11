/*------------------------------------------------------------------
// sq_capture.h
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

// namespace
namespace sqbind
{
    class CSqCapture
    {
	public:

		SQBIND_CLASS_CTOR_BEGIN( CSqCapture )
		SQBIND_CLASS_CTOR_END( CSqCapture )

		/// Register class for squirrel
		static void Register( sqbind::VM vm );

		/// Default constructor
		CSqCapture() {}

		// Copy semantics
		CSqCapture( const CSqCapture &r ) { }
		CSqCapture& operator=( const CSqCapture &r ) { return *this; }

		/// Close previous device
		int Destroy();

		/// Initializes the capture device
		int Init( int nDevice, int nChannel, int nWidth, int nHeight, int nFps, const stdString &sFormat );

		/// Returns encoded image
		stdString Capture( const stdString &sEncode );

		/// Returns a string describing the supported formats
		stdString GetSupportedFormats();

		/// Return the description string for the specified format
		stdString GetFormatDescription( const stdString &sFormat );

	private:

		/// Capture device
		oex::vid::CCapture			m_cap;

    };

}; // end namespace

// Declare type for use as squirrel parameters
SQBIND_DECLARE_INSTANCE( sqbind::CSqCapture, CSqCapture )
