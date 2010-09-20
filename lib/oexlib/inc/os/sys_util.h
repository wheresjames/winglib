/*------------------------------------------------------------------
// sys_util.h
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

class CSysUtil
{
private:

    /// Constructor
	CSysUtil() {}

    /// Destructor
	virtual ~CSysUtil() {}
	
public:

	//==============================================================
	// GetRegString()
	//==============================================================
	/// Reads a string from the registry
	/**
		\param [in] x_sKey		-	Key name
		\param [in] x_sPath		-	Registry path
		\param [in] x_sName		-	Value name

		\return Non-zero if success

		\see
	*/
	static CStr GetRegString( const CStr &x_sKey, const CStr &x_sPath, const CStr &x_sName );

	//==============================================================
	// SetRegString()
	//==============================================================
	/// Reads a string from the registry
	/**
		\param [in] x_sKey		-	Key name
		\param [in] x_sPath		-	Registry path
		\param [in] x_sName		-	Value name
		\param [in] x_sValue	-	Value

		\return Non-zero if success

		\see
	*/
	static oexBOOL SetRegString( const CStr &x_sKey, const CStr &x_sPath, const CStr &x_sName, const CStr &x_sValue );

	//==============================================================
	// IsRegKey()
	//==============================================================
	/// Checks for reg keys existence
	/**
		\param [in] x_sKey		-	Key name
		\param [in] x_sPath		-	Registry path

		\return Non-zero if success

		\see
	*/
	static oexBOOL IsRegKey( const CStr &x_sKey, const CStr &x_sPath );

	//==============================================================
	// IsRegValue()
	//==============================================================
	/// Checks for reg key values existence
	/**
		\param [in] x_sKey		-	Key name
		\param [in] x_sPath		-	Registry path
		\param [in] x_sName		-	Value name

		\return Non-zero if success

		\see
	*/
	static oexBOOL IsRegValue( const CStr &x_sKey, const CStr &x_sPath, const CStr &x_sName );

	//==============================================================
	// DeleteRegKey()
	//==============================================================
	/// Deletes a registry key
	/**
		\param [in] x_sKey		-	Key name
		\param [in] x_sPath		-	Registry path
		\param [in] x_bSubKeys	-	Non-zero to delete sub keys as well.
									If this value is zero, the key will
									only be deleted if there are no sub
									keys.

		\return Non-zero if success

		\see
	*/
	static oexBOOL DeleteRegKey( const CStr &x_sKey, const CStr &x_sPath, oexBOOL x_bSubKeys );

	//==============================================================
	// DeleteRegSubKeys()
	//==============================================================
	/// Deletes registry sub keys
	/**
		\param [in] x_sKey		-	Key name
		\param [in] x_sPath		-	Registry path

		\return Non-zero if success

		\see
	*/
	static oexBOOL DeleteRegSubKeys( const CStr &x_sKey, const CStr &x_sPath );


	//==============================================================
	// GetDiskInfo()
	//==============================================================
	/// Returns information about the specified drive
	/**
		\param [in] x_sDrive	-	System drive path
	*/
	static CPropertyBag GetDiskInfo( const CStr &x_sDrive );

};

