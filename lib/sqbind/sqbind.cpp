/*------------------------------------------------------------------
// sqbind.cpp
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

#include "stdafx.h"

using namespace sqbind;

namespace sqbind
{
	void SqBindAll( sqbind::VM x_vm )
	{_STT();
		CSqFactory::Register( x_vm );
		CSqSize::Register( x_vm );
		CSqPos::Register( x_vm );
		CSq3dVector::Register( x_vm );
		CSq3dVectord::Register( x_vm );
		CSqColor::Register( x_vm );
		CSqBinary::Register( x_vm );
//		CSqShareOne2One::Register( x_vm );
//		CSqShareOne2Many::Register( x_vm );
		CSqBinaryShare::Register( x_vm );
		CSqFifoShare::Register( x_vm );
		CSqVideoShare::Register( x_vm );
		CSqDataLog::Register( x_vm );
		CSqString::Register( x_vm );
		CSqMulti::Register( x_vm );
		CSqFile::Register( x_vm );
		CSqTimeRange::Register( x_vm );
		CSqTime::Register( x_vm );
		CSqImage::Register( x_vm );
		CSqCapture::Register( x_vm );
		CSqSocket::Register( x_vm );
		CSqSerialPort::Register( x_vm );
		CSqSockAddress::Register( x_vm );
		CSqHttpServer::Register( x_vm );
		CSqGui::Register( x_vm );

#if defined( OEX_ENABLE_SQLITE )
		CSqSQLite::Register( x_vm );
#endif

	};

};

SQBIND_REGISTER_CLASS_BEGIN( sqbind::CSqFactory, CSqFactory )
SQBIND_REGISTER_CLASS_END()

void CSqFactory::Register( sqbind::VM vm )
{_STT();
	SQBIND_EXPORT( vm, CSqFactory );
}

SQBIND_REGISTER_CLASS_BEGIN( sqbind::CSqString, CSqString )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqString, str )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqString, set )
SQBIND_REGISTER_CLASS_END()

void CSqString::Register( sqbind::VM vm )
{_STT();
	SQBIND_EXPORT( vm, CSqString );
}

SQBIND_REGISTER_CLASS_BEGIN( sqbind::CSqFile, CSqFile )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFile, CreateAlways )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFile, OpenAlways )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFile, OpenExisting )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFile, OpenNew )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFile, Close )
	
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFile, SetPtrPosBegin )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFile, SetPtrPosEnd )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFile, Size )

	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFile, WriteBin )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFile, ReadBin )
	SQBIND_STATIC_FUNCTION(  sqbind::CSqFile, get_contents_bin )
	SQBIND_STATIC_FUNCTION(  sqbind::CSqFile, put_contents_bin )
	SQBIND_STATIC_FUNCTION(  sqbind::CSqFile, append_contents_bin )

	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFile, Write )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqFile, Read )
	SQBIND_STATIC_FUNCTION(  sqbind::CSqFile, get_contents )
	SQBIND_STATIC_FUNCTION(  sqbind::CSqFile, put_contents )
	SQBIND_STATIC_FUNCTION(  sqbind::CSqFile, append_contents )

	SQBIND_STATIC_FUNCTION(  sqbind::CSqFile, mkdir )
	SQBIND_STATIC_FUNCTION(  sqbind::CSqFile, rmdir )
	SQBIND_STATIC_FUNCTION(  sqbind::CSqFile, exists )
	SQBIND_STATIC_FUNCTION(  sqbind::CSqFile, unlink )
	SQBIND_STATIC_FUNCTION(  sqbind::CSqFile, delete_file )
	SQBIND_STATIC_FUNCTION(  sqbind::CSqFile, delete_path )
	SQBIND_STATIC_FUNCTION(  sqbind::CSqFile, rename )
	SQBIND_STATIC_FUNCTION(  sqbind::CSqFile, copy )
	SQBIND_STATIC_FUNCTION(  sqbind::CSqFile, get_filename )
	SQBIND_STATIC_FUNCTION(  sqbind::CSqFile, get_path )
	SQBIND_STATIC_FUNCTION(  sqbind::CSqFile, get_extension )
	SQBIND_STATIC_FUNCTION(  sqbind::CSqFile, get_dirlist )
	SQBIND_STATIC_FUNCTION(  sqbind::CSqFile, get_dirlist_r )	
SQBIND_REGISTER_CLASS_END()

void CSqFile::Register( sqbind::VM vm )
{_STT();
	SQBIND_EXPORT( vm, CSqFile );
}

SQBIND_REGISTER_CLASS_BEGIN( sqbind::CSqSerialPort, CSqSerialPort )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSerialPort, Open )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSerialPort, OpenNamed )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSerialPort, Destroy )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSerialPort, getName )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSerialPort, getIndex )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSerialPort, isOpen )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSerialPort, isPort )

	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSerialPort, WriteBin )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSerialPort, ReadBin )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSerialPort, AppendBin )	

	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSerialPort, getWriteBytes )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSerialPort, Write )

	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSerialPort, getReadBytes )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSerialPort, Read )

	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSerialPort, setBaudRate )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSerialPort, getBaudRate )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSerialPort, setStopBits )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSerialPort, getStopBits )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSerialPort, setParity )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSerialPort, getParity )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSerialPort, setRtsCtrl )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSerialPort, getRtsCtrl )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSerialPort, setDtrCtrl )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSerialPort, getDtrCtrl )

SQBIND_REGISTER_CLASS_END()

void CSqSerialPort::Register( sqbind::VM vm )
{_STT();
	SQBIND_EXPORT( vm, CSqSerialPort );
}

SQBIND_REGISTER_CLASS_BEGIN( sqbind::CSqColor, CSqColor )

	SQBIND_MEMBER_FUNCTION(  sqbind::CSqColor, r )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqColor, g )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqColor, b )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqColor, a )

	SQBIND_MEMBER_FUNCTION(  sqbind::CSqColor, setR )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqColor, setG )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqColor, setB )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqColor, setA )

	SQBIND_MEMBER_FUNCTION(  sqbind::CSqColor, set )

SQBIND_REGISTER_CLASS_END()


void CSqColor::Register( sqbind::VM vm )
{_STT();
	SQBIND_EXPORT( vm, CSqColor );
}


SQBIND_REGISTER_CLASS_BEGIN( sqbind::CSqImage, CSqImage )

	SQBIND_MEMBER_FUNCTION(  sqbind::CSqImage, Create )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqImage, Destroy )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqImage, Load )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqImage, Save )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqImage, Encode )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqImage, Decode )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqImage, getWidth )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqImage, getHeight )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqImage, getSize )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqImage, getScanWidth )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqImage, CopyImage )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqImage, Resample )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqImage, isValid )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqImage, setPixels )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqImage, getPixels )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqImage, refPixels )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqImage, EncodeJpg )	

	SQBIND_MEMBER_FUNCTION(  sqbind::CSqImage, Dilate )	
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqImage, Erode )	
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqImage, Light )	
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqImage, Colorize )	
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqImage, Threshold )	
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqImage, ShiftRGB )	
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqImage, Gamma )	
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqImage, Median )	
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqImage, Noise )	
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqImage, Crop )	
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqImage, Dither )	
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqImage, Rotate )	
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqImage, Negative )	
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqImage, GrayScale )	
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqImage, Flip )	
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqImage, Mirror )	

SQBIND_REGISTER_CLASS_END()

void CSqImage::Register( sqbind::VM vm )
{_STT();
	SQBIND_EXPORT( vm, CSqImage );
}

SQBIND_REGISTER_CLASS_BEGIN( sqbind::CSqSize, CSqSize )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSize, set )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSize, setX )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSize, getX )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSize, setY )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSize, getY )
SQBIND_REGISTER_CLASS_END()

void CSqSize::Register( sqbind::VM vm )
{_STT();
	SQBIND_EXPORT( vm, CSqSize );
}

SQBIND_REGISTER_CLASS_BEGIN( sqbind::CSqPos, CSqPos )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqPos, set )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqPos, setX )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqPos, getX )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqPos, setY )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqPos, getY )
SQBIND_REGISTER_CLASS_END()

void CSqPos::Register( sqbind::VM vm )
{_STT();
	SQBIND_EXPORT( vm, CSqPos );
}

SQBIND_REGISTER_CLASS_BEGIN( sqbind::CSq3dVector, CSq3dVector )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSq3dVector, set )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSq3dVector, setX )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSq3dVector, getX )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSq3dVector, setY )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSq3dVector, getY )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSq3dVector, setZ )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSq3dVector, getZ )
SQBIND_REGISTER_CLASS_END()

void CSq3dVector::Register( sqbind::VM vm )
{_STT();
	SQBIND_EXPORT( vm, CSq3dVector );
}

SQBIND_REGISTER_CLASS_BEGIN( sqbind::CSq3dVectord, CSq3dVectord )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSq3dVectord, set )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSq3dVectord, setX )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSq3dVectord, getX )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSq3dVectord, setY )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSq3dVectord, getY )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSq3dVectord, setZ )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSq3dVectord, getZ )
SQBIND_REGISTER_CLASS_END()

void CSq3dVectord::Register( sqbind::VM vm )
{_STT();
	SQBIND_EXPORT( vm, CSq3dVectord );
}


