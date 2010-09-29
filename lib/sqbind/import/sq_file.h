/*------------------------------------------------------------------
// sq_file.h
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

		SQBIND_CLASS_CTOR_BEGIN( CSqFile )
		SQBIND_CLASS_CTOR_END( CSqFile )

		int CreateAlways( const stdString &sFile )
		{	return m_f.CreateAlways( sFile.c_str() ).IsOpen(); }

		int OpenExisting( const stdString &sFile )
		{	return m_f.OpenExisting( sFile.c_str() ).IsOpen(); }

		int OpenAlways( const stdString &sFile )
		{	return m_f.OpenAlways( sFile.c_str() ).IsOpen(); }

		int OpenNew( const stdString &sFile )
		{	return m_f.CreateAlways( sFile.c_str() ).IsOpen(); }

		void Close() { m_f.Destroy(); }

		int SetPtrPosBegin( int n ) { return m_f.SetPtrPosBegin( n ); }

		int SetPtrPosEnd( int n ) { return m_f.SetPtrPosEnd( n ); }

		int Size() { return m_f.Size(); }

		int Write( const stdString &sData )
		{	return m_f.Write( oexStrToBinPtr( sData.c_str() ), sData.length() ); }

		int WriteBin( CSqBinary *sData )
		{	if ( !sData ) return 0;
			return m_f.Write( sData->Ptr(), sData->getUsed() );
		}

		stdString Read()
		{	return oexBinToStr( m_f.Read() ).Ptr(); }

		CSqBinary ReadBin( int nBytes )
		{
			// Don't read more than is available
			oex::oexINT64 nSize = m_f.Size();
			if ( !nBytes || !nSize || nSize < nBytes )
				nBytes = (int)nSize;

			if ( !nBytes )
				return CSqBinary();

			// Allocate buffer
			CSqBinary buf;
			if ( !buf.Allocate( nBytes ) )
				return buf;

			// Attempt read
			oex::oexINT64 nRead = 0;
			if ( !m_f.Read( buf._Ptr(), nBytes, &nRead ) )
				return CSqBinary();

			// How'd it go?
			buf.setUsed( (int)nRead );

			return buf;
		}

		static stdString get_contents( const stdString &sFile )
		{	oex::CStr data = oexBinToStr( oex::CFile().OpenExisting( sFile.c_str() ).Read() );
			return stdString( data.Ptr(), data.Length() );
		}

		static CSqBinary get_contents_bin( const stdString &sFile, int nBytes )
		{	CSqFile f; if ( !f.OpenExisting( sFile ) ) return CSqBinary(); return f.ReadBin( nBytes ); }

		static int put_contents( const stdString &sFile, const stdString &sData )
		{	return oex::CFile().CreateAlways( sFile.c_str() ).Write( sData.c_str(), sData.length() ); }

		static int put_contents_bin( const stdString &sFile, CSqBinary *sData )
		{	if ( !sData ) return 0;
			CSqFile f; if ( !f.CreateAlways( sFile.c_str() ) ) return 0;
			return f.WriteBin( sData );
		}

		static int append_contents( const stdString &sFile, const stdString &sData )
		{	oex::CFile f;
			if ( !f.OpenAlways( sFile.c_str() ).IsOpen() )
				return 0;
			f.SetPtrPosEnd( 0 );
			return f.Write( sData.c_str(), sData.length() );
		}

		static int append_contents_bin( const stdString &sFile, CSqBinary *sData )
		{	if ( !sData ) return 0;
			CSqFile f;
			if ( !f.OpenAlways( sFile.c_str() ) )
				return 0;
			f.SetPtrPosEnd( 0 );
			return f.WriteBin( sData );
		}

		static int mkdir( const stdString &sDir )
		{	return oexCreatePath( sDir.c_str() ); }

		static int rmdir( const stdString &sDir )
		{	return oexDeletePath( sDir.c_str() ); }

		static int delete_file( const stdString &sPath )
		{	return oexDelete( sPath.c_str() ); }

		static int delete_path( const stdString &sPath )
		{	return oexDeletePath( sPath.c_str() ); }

		static int rename( const stdString &sOld, const stdString &sNew )
		{	return oexRename( sOld.c_str(), sNew.c_str() ); }

		static int exists( const stdString &sFile )
		{	return oexExists( sFile.c_str() ); }

		static stdString get_filename( const stdString &sFile )
		{	return oexGetFileName( sFile.c_str() ).Ptr(); }

		static stdString get_path( const stdString &sFile )
		{	return oexGetPath( sFile.c_str() ).Ptr(); }

		static stdString get_extension( const stdString &sFile )
		{	return oexGetFileExtension( sFile.c_str() ).Ptr(); }

		static CSqMulti get_dirlist( const stdString &sDir, const stdString &sMask, int bFiles, int bDirs )
		{
			oex::CFindFiles ff;
			CSqMulti mRet;
			if ( ff.FindFirst( sDir.c_str(), sMask.c_str() ) )
				do
				{
					// Directory
					if ( ff.IsDirectory() )
					{	if ( bDirs )
							mRet[ ff.GetFileName().Ptr() ] = oexT( "d" );
					} // end if

					// File
					else if ( bFiles )
						mRet[ ff.GetFileName().Ptr() ] = oexT( "f" );

				} while ( ff.FindNext() );

			return mRet;
		}

		static void Register( sqbind::VM vm );

	private:

		oex::CFile	m_f;
    };

}; // end namespace

// Declare type for use as squirrel parameters
SQBIND_DECLARE_INSTANCE( sqbind::CSqFile, CSqFile )
