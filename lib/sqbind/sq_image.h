/*------------------------------------------------------------------
// sq_image.h
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
    class CSqImage
    {
	public:

		CSqImage() {}

		// Copy semantics
		CSqImage( const CSqImage &r ) { m_img = r.m_img; }
		CSqImage& operator=( const CSqImage &r ) { m_img = r.m_img; return *this; }

		int Load( const stdString &sFile, const stdString &sType )
		{	return m_img.Load( sFile.c_str(), sType.c_str() ); }

		int Save( const stdString &sFile, const stdString &sType )
		{	return m_img.Save( sFile.c_str(), sType.c_str() ); }

		stdString Encode( const stdString &sType )
		{
			oex::oexPBYTE pBuf = oexNULL;
			oex::oexINT nSize = 0;
			if ( !m_img.Encode( &pBuf, &nSize, sType.c_str() ) 
			     || !oexCHECK_PTR( pBuf ) || !nSize )
				return oexT( "" );
			
			return stdString().assign( (oex::oexCSTR)pBuf, nSize );
		}
		
		int Decode( const stdString &sType, const stdString &sData )
		{	return m_img.Decode( (oex::oexBYTE*)sData.c_str(), sData.length(), sType.c_str() ); }		

		static void Register( SquirrelVM &vm )
		{
			SqPlus::SQClassDef< CSqImage >( vm, oexT( "CSqImage" ) )
					. func( &CSqImage::Load,				oexT( "Load" ) )
					. func( &CSqImage::Save,				oexT( "Save" ) )									
					. func( &CSqImage::Encode,				oexT( "Encode" ) )
					. func( &CSqImage::Decode,				oexT( "Decode" ) )
				;
		}

	private:
	
		/// Image object
		oex::CImage		m_img;

    };

}; // end namespace

// Declare type for use as squirrel parameters
DECLARE_INSTANCE_TYPE_NAME( sqbind::CSqImage, CSqImage )
