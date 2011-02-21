/*------------------------------------------------------------------
// resources.h
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

#if !defined( OEX_RESOURCES )
#	define OEX_NO_RESOURCES 1
#endif

struct _SOexResourceInfo
{
	const char *   name;
	const char *   data;
	unsigned long  size;
};

class COexResourceHelper
{
public:

	/// Initializes resource engine
	static oexBOOL InitResources();

public:

	/// Constructor
	COexResourceHelper();

	/// Constructor
	COexResourceHelper( const _SOexResourceInfo *pRes );

	/// Constructs class from resource name
	COexResourceHelper( CStr sName );

	/// Returns non-zero if there is a resource list
	oexBOOL IsResources();

	/// Sets the pointer to the resource array
	void SetResourcePtr( const _SOexResourceInfo *pRes );

	/// Returns resource data
	CStr8 GetResource( CStr sName );

	/// Returns resource data
	oexBOOL GetResource( CStr sName, CStr8 *pRes );

	/// Gets resource data for specified index
	CStr8 GetResource( oexLONG i );

	/// Returns resource pointer for specified name
	oexLONG GetResource( CStr sName, const void ** p, oexLONG *l );

	/// Returns resource pointer for specified index
	oexLONG GetResource( oexLONG i, const void ** p, oexLONG *l );

	/// Returns specified resource size
	oexLONG GetResourceSize( oexLONG i );

	/// Returns specified resource pointer
	oexCPVOID GetResourcePtr( oexLONG i );

	/// Returns the resource name for the specified index
	CStr GetResourceName( oexLONG i );

	/// Gets total resource count
	oexLONG GetResourceCount();

	/// Returns a list of resources matching a pattern
	CStrList FindResource( CStr sName, oexBOOL bIgnoreCase = oexFALSE );

private:

	/// Pointer to resources
	const _SOexResourceInfo		*m_oexlib_resources_ptr; 
};

