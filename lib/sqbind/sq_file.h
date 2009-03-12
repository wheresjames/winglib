/*------------------------------------------------------------------
// sq_vector.h
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
    class CSqFile
    {
	public:

		int OpenExisting( const stdString &sFile )
		{	return m_f.OpenExisting( sFile.c_str() ).IsOpen(); }

		int OpenNew( const stdString &sFile )
		{	return m_f.CreateAlways( sFile.c_str() ).IsOpen(); }

		void Close() { m_f.Destroy(); }

		int Write( const stdString &sData )
		{	return m_f.Write( oexStrToBin( sData.c_str() ), sData.length() ); }

		stdString Read()
		{	return oexBinToStr( m_f.Read() ).Ptr(); }

		static stdString get_contents( const stdString &sFile )
		{	return oexBinToStr( oex::CFile().OpenExisting( sFile.c_str() ).Read() ).Ptr(); }

		static int put_contents( const stdString &sFile, const stdString &sData )
		{	return oex::CFile().CreateAlways( sFile.c_str() ).Write( sData.c_str(), sData.length() ); }

		static void Register( SquirrelVM &vm )
		{
			SqPlus::SQClassDef< CSqFile >( vm, oexT( "CSqFile" ) )
					. func( &CSqFile::OpenExisting,			oexT( "OpenExisting" ) )
					. func( &CSqFile::OpenNew,				oexT( "OpenNew" ) )
					. func( &CSqFile::Write,				oexT( "Write" ) )
					. func( &CSqFile::Read,					oexT( "Read" ) )
					. staticFunc( &CSqFile::get_contents,	oexT( "get_contents" ) )
					. staticFunc( &CSqFile::put_contents,	oexT( "put_contents" ) )
				;
		}

	private:

		oex::CFile	m_f;
    };

}; // end namespace

// Declare type for use as squirrel parameters
DECLARE_INSTANCE_TYPE_NAME( sqbind::CSqFile, CSqFile )
