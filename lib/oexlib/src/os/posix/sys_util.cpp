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

#include "oexlib.h"

#include "std_os.h"

#if !defined( OEX_NOSTATFS )
#	include <sys/statfs.h>
#elif !defined( OEX_NOSTATVFS )
#	include <sys/statvfs.h>
#endif

#if !defined( OEX_NOMNTENT )
#	include <mntent.h>
#endif


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

oexBOOL CSysUtil::DeleteRegKey( const CStr &x_sKey, const CStr &x_sPath, oexBOOL x_bSubKeys )
{
	return oexFALSE;
}

oexBOOL CSysUtil::DeleteRegSubKeys( const CStr &x_sKey, const CStr &x_sPath )
{
	return oexFALSE;
}

CPropertyBag CSysUtil::GetRegKeys( const CStr &x_sKey, const CStr &x_sPath, oexBOOL x_bValues, oexBOOL x_bSubKeys )
{
	return CPropertyBag();
}

oexCSTR GetFsTypeStr( unsigned long type )
{
	switch( type )
	{
		case 0x0000ADF5	: return oexT( "ADFS" );
		case 0x0000ADFF	: return oexT( "AFFS" );
		case 0x42465331	: return oexT( "BEFS" );
		case 0x1badface : return oexT( "BFS" );
		case 0xFF534D42	: return oexT( "CIFS" );
		case 0x73757245	: return oexT( "CODA" );
		case 0x012FF7B7 : return oexT( "COH" );
		case 0x00001373 : return oexT( "DEVFS" );
		case 0x00414A53 : return oexT( "EFS" );
		case 0x0000137D : return oexT( "EXT" );
		case 0x0000EF51 : return oexT( "EXT2 old" );
		case 0x0000EF52 : return oexT( "EXT2" );
		case 0x0000EF53 : return oexT( "EXT3" );
		case 0x00004244 : return oexT( "HFS" );
		case 0xF995E849 : return oexT( "HPFS" );
		case 0x958458f6 : return oexT( "HUGETLBFS" );
		case 0x00009660 : return oexT( "ISOFS" );
		case 0x000072b6 : return oexT( "JFFS2" );
		case 0x3153464a : return oexT( "JFS" );
		case 0x0000137F : return oexT( "MINIX" );
		case 0x0000138F : return oexT( "MINIX-30" );
		case 0x00002468 : return oexT( "MINIX2" );
		case 0x00002478 : return oexT( "MINIX2-30" );
		case 0x00004d44 : return oexT( "MSDOS" );
		case 0x0000564c : return oexT( "NCP" );
		case 0x00006969 : return oexT( "NFS" );
		case 0x5346544e : return oexT( "NTFS" );
		case 0x00009fa1 : return oexT( "OPENPROM" );
		case 0x00009fa0 : return oexT( "PROC" );
		case 0x0000002f : return oexT( "QNX4" );
		case 0x52654973 : return oexT( "REISERFS" );
		case 0x00007275 : return oexT( "ROMFS" );
		case 0x0000517B : return oexT( "SMB" );
		case 0x012FF7B6 : return oexT( "SYSV2" );
		case 0x012FF7B5 : return oexT( "SYSV4" );
		case 0x01021994 : return oexT( "TMPFS" );
		case 0x15013346 : return oexT( "UDF" );
		case 0x00011954 : return oexT( "UFS" );
		case 0x00009fa2 : return oexT( "USBDEV" );
		case 0xa501FCF5 : return oexT( "VXFS" );
		case 0x012FF7B4 : return oexT( "XENIX" );
		case 0x58465342 : return oexT( "XFS" );
		case 0x012FD16D : return oexT( "XIAFS" );

	} // end switch

	return oexT( "unknown" );
}

oexBOOL CSysUtil::IsMounted( const CStr &x_sDrive )
{
	return oexTRUE;
}


CPropertyBag CSysUtil::GetDiskInfo(const CStr &x_sDrive)
{_STT();

	// Sanity check
	if ( !x_sDrive.Length() )
		return CPropertyBag();

	CPropertyBag pb;
	pb[ oexT( "drive" ) ] = x_sDrive;
//	pb[ oexT( "drive_type" ) ] = GetDriveTypeStr( x_sDrive.c_str() );

#if !defined( OEX_NOSTATFS )

	struct statfs di;
    if ( 0 > statfs( oexStrToMb( x_sDrive ).Ptr(), &di ) )
	{	pb[ oexT( "drive_mounted" ) ] = 0;
    	return CPropertyBag();
	} // end if

	pb[ oexT( "file_system_type" ) ] = (unsigned int)di.f_type;
	pb[ oexT( "file_system_str" ) ] = GetFsTypeStr( di.f_type );
	pb[ oexT( "file_system_id32" ) ] = (unsigned int)di.f_fsid.__val[ 0 ];
	pb[ oexT( "file_system_id64" ) ] = *(oexUINT64*)&di.f_fsid;

#elif !defined( OEX_NOSTATVFS )

	struct statvfs di;
    if ( 0 > statvfs( x_sDrive.c_str(), &di ) )
    	return CPropertyBag();

	pb[ oexT( "flags" ) ] = di.f_flag;
	pb[ oexT( "max_filename" ) ] = di.f_namemax;
//	pb[ oexT( "file_system_type" ) ] = di.f_type;
//	pb[ oexT( "file_system_str" ) ] = GetFsTypeStr( di.f_type );
	pb[ oexT( "file_system_id32" ) ] = (unsigned int)di.f_fsid;
//	pb[ oexT( "file_system_id64" ) ] = (oexUINT64)di.f_fsid64;

#endif

    // Space info
	pb[ oexT( "bytes" ) ] = (oexUINT64)di.f_blocks * (oexUINT64)di.f_bsize;
	pb[ oexT( "bytes_free" ) ] = (oexUINT64)di.f_bfree * (oexUINT64)di.f_bsize;
	pb[ oexT( "bytes_used" ) ] = (oexUINT64)( di.f_blocks - di.f_bfree ) * (oexUINT64)di.f_bsize;
	pb[ oexT( "bytes_available" ) ] = (oexUINT64)di.f_bavail * (oexUINT64)di.f_bsize;
	pb[ oexT( "bytes_unavailable" ) ] = (oexUINT64)( di.f_blocks - di.f_bavail ) * (oexUINT64)di.f_bsize;

	pb[ oexT( "inodes" ) ] = di.f_files;
	pb[ oexT( "block_size" ) ] = di.f_bsize;
	pb[ oexT( "fragment_size" ) ] = di.f_frsize;

	pb[ oexT( "drive_mounted" ) ] = di.f_bsize ? 1 : 0;

	// If there is storage, calculate percentages
	if( di.f_bsize )
	{
		// Available percentages
		pb[ oexT( "percent_available" ) ] 
			= CStr().Fmt( "%.2f", (double)di.f_bavail / (double)di.f_bsize * (double)100 );
		if ( di.f_bsize > di.f_bavail )
			pb[ oexT( "percent_unavailable" ) ] 
				= CStr().Fmt( "%.2f", (double)( di.f_bsize - di.f_bavail )
										/ (double)di.f_bsize * (double)100 );

		// Used percentages
		pb[ oexT( "percent_free" ) ] 
			= CStr().Fmt( "%.2f", (double)di.f_bfree / (double)di.f_bsize * (double)100 );
		if ( di.f_bsize > di.f_bfree )
			pb[ oexT( "percent_used" ) ] 
				= CStr().Fmt( "%.2f", (double)( di.f_bsize - di.f_bfree )
										/ (double)di.f_bsize * (double)100 );
	} // en dif

	return pb;
}

CPropertyBag CSysUtil::GetDisksInfo( oexBOOL bInfo )
{_STT();

	long lTotal = 0;
	CPropertyBag pb;

#if !defined( OEX_NOMNTENT )

	struct mntent *m;
	FILE *f = setmntent( "/proc/mounts", "r" );
	while ( ( m = getmntent( f ) ) )
		if ( m )
		{
			CStr sPath = m->mnt_dir
						  ? oexMbToStr( m->mnt_dir )
						  : CStr( lTotal );

			if ( bInfo )
			{
				CPropertyBag &r = pb[ sPath ];
				r = GetDiskInfo( sPath );
				r[ oexT( "volume" ) ] = oexMbToStr( m->mnt_fsname ? m->mnt_fsname : "" );
				r[ oexT( "drive_type_os" ) ] = oexMbToStr( m->mnt_type ? m->mnt_type : "" );
				r[ oexT( "drive_path" ) ] = oexMbToStr( m->mnt_dir ? m->mnt_dir : "" );
				r[ oexT( "drive_fs" ) ] = oexMbToStr( m->mnt_fsname ? m->mnt_fsname : "" );
				r[ oexT( "drive_opts" ) ] = oexMbToStr( m->mnt_opts ? m->mnt_opts : "" );
				r[ oexT( "drive_freq" ) ] = m->mnt_freq;
				r[ oexT( "drive_passno" ) ] = m->mnt_passno;
				r[ oexT( "drive_type" ) ] = GetDriveTypeStr( oexMbToStr( m->mnt_type ? m->mnt_type : "" ) );

				// +++ Hmmmm
				if ( pb[ oexT( "bytes" ) ].ToLong()
					 && r[ oexT( "drive_type" ) ].ToString() == "unknown"
					 && r[ oexT( "file_system_str" ) ].ToString() != "TMPFS" )
					pb[ oexT( "drive_type" ) ] = oexT( "fixed" );

			} // end if

			else
				pb[ sPath ] = GetDriveTypeStr( oexMbToStr( m->mnt_type ? m->mnt_type : "" ) );

			lTotal++;

		} // end while

	endmntent( f );

#endif

	return pb;
}

CStr CSysUtil::GetDriveTypeStr(const CStr &x_sDrive)
{_STT();

	if ( !x_sDrive.Length() )	 	return oexT( "noroot" );

	CStr s = x_sDrive;
	if ( s == oexT( "fd" ) ) 		return oexT( "removable" );

	if ( s == oexT( "hd" ) ) 		return oexT( "fixed" );
	if ( s == oexT( "ext" ) ) 		return oexT( "fixed" );
	if ( s == oexT( "ext2" ) ) 		return oexT( "fixed" );
	if ( s == oexT( "ext3" ) ) 		return oexT( "fixed" );
	if ( s == oexT( "fuseblk" ) ) 	return oexT( "fixed" );
	if ( s == oexT( "ecryptfs" ) ) 	return oexT( "fixed" );

	if ( s == oexT( "cdrom" ) ) 	return oexT( "cdrom" );

	if ( s == oexT( "ram" ) ) 		return oexT( "ramdisk" );
	if ( s == oexT( "tmpfs" ) ) 	return oexT( "ramdisk" );
	if ( s == oexT( "tempfs" ) ) 	return oexT( "ramdisk" );
	if ( s == oexT( "devtmpfs" ) ) 	return oexT( "ramdisk" );

	if ( s == oexT( "devpts" ) ) 	return oexT( "remote" );
	if ( s == oexT( "subst" ) ) 	return oexT( "remote" );
	if ( s == oexT( "join" ) ) 		return oexT( "remote" );
	if ( s == oexT( "net" ) ) 		return oexT( "remote" );

	return oexT( "unknown" );
}

