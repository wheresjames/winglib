/*------------------------------------------------------------------
// mmap.h
// Copyright (c) 2006 
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
// http://www.opengroup.org/onlinepubs/000095399/functions/mmap.html

#pragma once

class CMMap
{
private:

	CMMap() {}

public:

	enum etProt
	{
		eProtNone		=	0x00,
		eProtRead		=	0x01,
		eProtWrite		=	0x02,
		eProtExec		=	0x04,
		eProtUser		=	0x08,
		eProtZfod		=	( eProtRead | eProtWrite | eProtExec | eProtUser ),
		eProtAll		=	( eProtRead | eProtWrite | eProtExec | eProtUser ),
		eProtText		=	( eProtExec | eProtRead ),
		eProtData		=	( eProtRead | eProtWrite | eProtExec ),
		eProtExcl		=	0x20
	};

	enum etMap
	{
		eMapShared		=	0x01,
		eMapPrivate		=	0x02,
		eMapFixed		=	0x10,
		eMapNoReserve	=	0x40,
		eMapType		=	0x0F,
	};

	//==============================================================
	// mmap()
	//==============================================================
	/// Creates a memory mapping
	/**
		\param [in] x_addr		-	Desired address of the memory mapping.
		\param [in] x_len		-	Number of bytes to map
		\param [in] x_prot		-	Desired protection for mamory mapping.
									-	PROT_READ	=	Read access
									-	PROT_WRITE	=	Write access
		\param [in] x_flags		-	Mapping attributes
									-	MAP_SHARED	=	Shared access
									-	MAP_PRIVATE	=	Private access
														changes not 
														reflected in the
														mapped file.
		\param [in] x_fildes	-	File descriptor of the file to be mapped.
									Use -1 for memory mapping.
		\param [in] x_off		-	Starting offet in file of memory mapping.
		
		\return Pointer to mapped file memory or MAP_FAILED if error.

		\see munmap()
	*/
	static oexHMEMMAP osMMap( oexCPVOID x_addr, oexINT64 x_len, oexINT x_prot, oexINT x_flags, oexINT x_fildes, oexINT x_off );

	//==============================================================
	// munmap()
	//==============================================================
	/// Releases a memory mapping
	/**
		\param [in] x_addr	-	Address of memory mapping to free.
		\param [in] x_len	-	Ignored
		
		\return Zero if success, otherwise MAP_FAILED

		\see mmap()
	*/
	static oexHMEMMAP osMUnMap( oexHMEMMAP x_addr, size_t x_len = 0 );

	/// Valued returned on failure
	const oexHMEMMAP vFailed() { return c_MapFailed; }

private:

	/// Failure value
	static const oexHMEMMAP c_MapFailed;


};
