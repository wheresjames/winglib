/*------------------------------------------------------------------
// oex_memory.h
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

typedef void* (*PFN_malloc)( oexSIZE_T size );
typedef void* (*PFN_realloc)( oexPVOID ptr, oexSIZE_T size );
typedef void (*PFN_free)( oexPVOID free );

class CMemLeak;
class CBinShare;
class CLog;
class COexResourceHelper;
class CStackTrace;
struct SRawAllocator
{
	/// Allocates memory
	PFN_malloc			fMalloc;

	/// Resizes a memory block
	PFN_realloc			fRealloc;

	/// Frees a memory block
	PFN_free			fFree;

	/// Binary shares
	CBinShare			*pBinShare;

	/// Memory leak tracker
	CMemLeak			*pMemLeak;

	/// Logging class
	CLog				*pLog;

	/// Resource helper
	COexResourceHelper	*pResourceHelper;

	/// Stack trace object
	CStackTrace			*pStackTrace;
};

class CMem
{
private:
	CMem()
    {}
	virtual ~CMem()
    {}

public:

    static oexBOOL DumpLeaks();

    static oexBOOL MemReport();

    static oexPVOID New( oexUINT x_uSize, oexUINT x_uLine, oexCSTR x_pFile  );

    static oexPVOID Resize( oexPVOID x_pMem, oexUINT x_uSize, oexUINT x_uLine, oexCSTR x_pFile  );

    static void Delete( oexPVOID x_pMem );

	/// Returns a copy of the raw allocator function pointers
	static SRawAllocator GetDefaultAllocator()
	{	return m_def; }

	/// Returns a copy of the raw allocator function pointers
	static SRawAllocator GetRawAllocator()
	{	return m_ra; }

	/// Sets the raw allocator function pointers
	static void SetRawAllocator( SRawAllocator x_ra )
	{	m_ra = x_ra; }

	/// Sets the default raw allocator function pointers
	static void SetDefaultRawAllocator()
	{	m_ra = m_def; }

	/// Returns a pointer to the stack trace object
	static CStackTrace& GetStackTrace();

private:

	/// Raw allocator used for all memory allocations
	static SRawAllocator	m_ra;

	/// The default raw allocator
	static SRawAllocator	m_def;
};
