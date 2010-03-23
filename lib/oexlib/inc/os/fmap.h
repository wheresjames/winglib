/*------------------------------------------------------------------
// fmap.h
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

class CFMap
{
private:

	/// Constructor
	CFMap() {}

	/// Destructor
	virtual ~CFMap() {}

public:

	typedef oexPVOID	t_HFILEMAP;

	enum etAccess
	{
		/// Read access flag
		eAccessRead		=	0x00000001,

		/// Write access flag
		eAccessWrite	=	0x00000002,

		/// All access flag
		eAccessAll		=	( eAccessRead | eAccessWrite )
	};

	//==============================================================
	// Create()
	//==============================================================
	/// Creates a file mapping
	/**
		\param [in] x_hFile				-	Handle to open file.
		\param [out] x_pMem				-	Receives a pointer to the
											mapped memory.
		\param [in] x_llSize			-	Size of the mapped memory.
											If this value is zero, and
											a file is being mapped. The
											entire file is mapped into
											memory.
		\param [in] x_pllSize			-	Receives the number of bytes
											actually mapped into memory.
		\param [in] x_eAccess			-	Memory access flags.
		\param [out] x_pbAlreadyExists	-	Receives the pre-existing
											status.  A non-zero value will
											be written to this variable if
											the memory was already existing.
		\param [in] x_llOffset			-	Offset into memory segment

		\return Handle to file mapping

		\see
	*/
	static t_HFILEMAP Create( CFMap::t_HFILEMAP x_hFile, oexPVOID *x_pMem, oexFILESIZE_T x_llSize, oexFILESIZE_T *x_pllSize, etAccess x_eAccess, oexBOOL *x_pbAlreadyExists, oexFILESIZE_T x_llOffset = 0 );

	//==============================================================
	// Create()
	//==============================================================
	/// Creates a file mapping
	/**
		\param [in] x_pFile				-	Name of the file to map.
											Set to NULL for a memory
											mapping.
		\param [out] x_pMem				-	Receives a pointer to the
											mapped memory.
		\param [in] x_llSize			-	Size of the mapped memory.
											If this value is zero, and
											a file is being mapped. The
											entire file is mapped into
											memory.
		\param [in] x_pllSize			-	Receives the number of bytes
											actually mapped into memory.
		\param [in] x_pName				-	Shared memory name.
		\param [in] x_eAccess			-	Memory access flags.
		\param [out] x_pbAlreadyExists	-	Receives the pre-existing
											status.  A non-zero value will
											be written to this variable if
											the memory was already existing.
		\param [in] x_llOffset			-	Offset into memory segment

		\return Handle to file mapping

		\see
	*/
	static t_HFILEMAP Create( oexCSTR x_pFile, oexPVOID *x_pMem, oexFILESIZE_T x_llSize, oexFILESIZE_T *x_pllSize, oexCSTR x_pName, etAccess x_eAccess, oexBOOL *x_pbAlreadyExists, oexFILESIZE_T x_llOffset = 0 );

	//==============================================================
	// osReleaseFileMapping()
	//==============================================================
	///
	/**
		\param [in] x_hFileMap	-	Handle to file mapping to release.
		\param [in] x_pMem		-	Pointer to mapped memory.

		Pass in the values returned by osCreateFileMapping() to release
		the memory mapped file.

		\return

		\see
	*/
	static oexBOOL Release( t_HFILEMAP x_hFileMap, oexPVOID x_pMem, oexFILESIZE_T x_llSize );

	//==============================================================
	// vFailed()
	//==============================================================
	/// Returns a value representing failure.
	static oexCONST t_HFILEMAP vFailed() { return c_Failed; }

private:

	/// Holds the failure value
	static oexCONST t_HFILEMAP c_Failed;

};
