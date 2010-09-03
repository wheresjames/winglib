/*------------------------------------------------------------------
// sys_util.cpp
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

#include "../../../oexlib.h"

#include "std_os.h"

OEX_USING_NAMESPACE
using namespace OEX_NAMESPACE::os;

oexBOOL CSysUtil::IsRegKey( const CStr &x_sKey, const CStr &x_sPath )
{
	return oexFALSE;
}

oexBOOL CSysUtil::IsRegValue( const CStr &x_sKey, const CStr &x_sPath, const CStr &x_sName )
{
	return oexFALSE;
}

CStr CSysUtil::GetRegString( const CStr &x_sKey, const CStr &x_sPath, const CStr &x_sName )
{
	return CStr();
}

oexBOOL CSysUtil::SetRegString( const CStr &x_sKey, const CStr &x_sPath, const CStr &x_sName, const CStr &x_sValue )
{
	return oexFALSE;
}

CPropertyBag CSysUtil::GetDiskInfo(const CStr &x_sDrive)
{_STT();

	return CPropertyBag();
/*
	// Sanity check
	if ( !x_sDrive.Length() ) 
		return CPropertyBag();

	// Must be root
//    if ( geteuid() ) 
//    {	oexERROR( 0, oexT( "Only root can call GetDiskInfo()" ) ); 
//    	return CPropertyBag();
//    } // end if
/*
	static struct hd_driveid hd;
    int fd;
	
	// Open the drive
	if ( 0 > ( fd = open( oexStrToMb( x_sDrive ).Ptr(), O_RDONLY | O_NONBLOCK ) ) )	
    	return CPropertyBag();
	
	if ( ioctl( fd, HDIO_GET_IDENTITY, &hd ) )
		return CPropertyBag();
		
//        printf("Hard Disk Model: %.40s\n", hd.model);
 //       printf("  Serial Number: %.20s\n", hd.serial_no);

	
	
	// Get volume information
//	if ( !GetVolumeInformation(	x_sDrive.Ptr(), szVolume, sizeof( szVolume ),
//								&dwSn, &dwMax, &dwFlags,
//								szFileSystem, sizeof( szFileSystem ) ) )
//		return CPropertyBag();

	CPropertyBag pb;
	pb[ "drive" ] = x_sDrive;
	pb[ "volume" ] = oexMbToStrPtr( szVolume );
	pb[ "serial" ] = dwSn;
	pb[ "max_filename" ] = dwMax;
	pb[ "flags" ] = dwFlags;
	pb[ "file_system" ] = oexMbToStrPtr( szFileSystem );

	return pb;
*/
}

