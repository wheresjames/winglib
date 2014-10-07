/*------------------------------------------------------------------
// sq_avi.h
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
    class CSqAviFile
    {
	public:

		SQBIND_CLASS_CTOR_BEGIN( CSqAviFile )
		SQBIND_CLASS_CTOR_END( CSqAviFile )

		/// Register class for squirrel
		static void Register( sqbind::VM vm );

		/// Default constructor
		CSqAviFile() {}

		// Copy semantics
		CSqAviFile( const CSqAviFile &r ) { }
		CSqAviFile& operator=( const CSqAviFile &r ) { return *this; }

		/// Close previous device
		int Destroy();
		
		/// Creates a new avi file
		int Create( const sqbind::stdString &sFile );

		/// Add a video stream
		int AddVideoStream( int nCodecId, int nWidth, int nHeight, float fFps );

		/// Adds a frame of video to the file
		int AddVideoFrame( int nType, int nStream, CSqBinary *pFrame );
		
		
		/// Returns the capture image width
//		int getWidth() { return m_cap.GetWidth(); }

		/// Returns the capture image height
//		int getHeight() { return m_cap.GetHeight(); }

		/// Returns the capture fps
//		float getFps() { return m_cap.GetFps(); }

		/// Returns non-zero if a capture device is open
//		int isOpen() { return m_cap.IsOpen(); }

	public:
	
		/// Convert codec string to id
		static int fourCC( const sqbind::stdString &sCodec );

	private:

		/// Capture device
		oex::vid::CAviFile			m_avi;

    };

}; // end namespace

// Declare type for use as squirrel parameters
SQBIND_DECLARE_INSTANCE( sqbind::CSqAviFile, CSqAviFile )
