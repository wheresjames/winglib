/*------------------------------------------------------------------
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
	/// Squirrel map adaptor class
	class CSqEzdib
	{	
	public:

		SQBIND_CLASS_CTOR_BEGIN( CSqEzdib )
		SQBIND_CLASS_CTOR_END( CSqEzdib )

		/// Destructor
		virtual ~CSqEzdib();

		/// Default constructor
		CSqEzdib();
		
		/// Register interface to VM
		static void Register( sqbind::VM vm );

		/// Copy
		CSqEzdib( const CSqEzdib &r ) { }
		CSqEzdib& operator = ( const CSqEzdib &r ) { return *this; }		
		
		
		
	/** \addtogroup COcvObjects
		@{
	*/
	
		/// Releases polygon resources
		void Destroy();

		/// Initializes a buffer
		int Init( CSqBinary *pBuf, int lWidth, int lHeight, int lBpp );
		
		/// Saves a bitmap image to disk
		int Save( const stdString &sFile );

		/// Fills the image with the specified color
		int Fill( int col );

		/// Flood fill an outline
		int FloodFill( int x, int y, int bcol, int col );
	
		/// Draw a circle
		int Circle( int x, int y, int rad, int col );
	
		/// Draws an arc
		int Arc( int x, int y, int rad, double start, double end, int col );
		
		/// Draws a rectangle
		int Rect( int x1, int y1, int x2, int y2, int col );
		
		/// Fills the specified rectangle
		int FillRect( int x1, int y1, int x2, int y2, int col );
		
		/// Draws a line
		int Line( int x1, int y1, int x2, int y2, int col );
		
		/// Sets the pixels color value
		int SetPixel( int x, int y, int col );
		
		/// Returns the specified pixels color value
		int GetPixel( int x, int y );
		
		
	/** @} */
	
		
	private:
	
		/// Handle to ezdib image
		oss::HEZDIMAGE			m_ezimg;
	
		/// Buffer object
		CSqBinary				m_buf;
		
	};

}; // end namespace

// Declare type for use as squirrel parameters
SQBIND_DECLARE_INSTANCE( sqbind::CSqEzdib, CSqEzdib )
