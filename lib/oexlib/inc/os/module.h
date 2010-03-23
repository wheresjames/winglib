/*------------------------------------------------------------------
// module.h
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


//==================================================================
// CModule
//
/// Encapsulates the Windows LoadLibrary() API functions
/**
	Encapsulates the Windows LoadLibrary() API functions.  Provides
	a simple way to load Windows DLLs and execute functions.
*/
//==================================================================
class CModule
{
public:

	/// Function map type
	typedef TAssoList< CStr, oexPVOID > t_FunctionMap;


public:

	//==============================================================
	// Addr()
	//==============================================================
	/// Returns the function address for specified index
	/**
		\param [in] x_i	-	Function index

		\return Pointer to function or NULL if none

		\see
	*/
	oexPVOID Addr( oexINT x_i );

	//==============================================================
	// Addr()
	//==============================================================
	/// Returns a pointer to the specified function name
	/**
		\param [in] x_pFunctionName	-	Function name

		\return Pointer to function or zero if not found

		\see
	*/
	oexPVOID Addr( oexCSTR x_pFunctionName );

	//==============================================================
	// AddFunction()
	//==============================================================
	/// Adds the specified function to the function list
	/**
		\param [in] x_pFunctionName	-	Function name

		\return Pointer to function address or NULL if not found

		\see
	*/
	oexPVOID AddFunction( oexCSTR x_pFunctionName );

	//==============================================================
	// Load()
	//==============================================================
	/// Loads the specified module
	/**
		\param [in] x_pFile	-	Module filename

		\return Non-zero if success

		\see
	*/
	oexBOOL Load( oexCSTR x_pFile, oexINT x_nFlags = -1 );

	//==============================================================
	// GetPath()
	//==============================================================
	/// Returns the file path of the currently loaded module.
	CStr GetPath() { return m_sFile; }

	/// Default constructor
	CModule();

	//==============================================================
	// CModule()
	//==============================================================
	/// Constructs object and loads the specified module
	/**
		\param [in] x_pFile	-	Module filename
	*/
	CModule( oexCSTR x_pFile );

	/// Destructor
	virtual ~CModule();

	//==============================================================
	// Destory()
	//==============================================================
	/// Unloads the module and releases associated resources
	void Destroy();


	//==============================================================
	// operator []
	//==============================================================
	/// Returns the function address for specified index
	/**
		\param [in] i	-	Function index

		\return Pointer to function or NULL if none

		\see
	*/
	oexPVOID operator [] ( oexINT i )
	{	oexPVOID ptr = Addr( i );
		if ( !ptr )
		{	oexTHROW( oexT( "0:CModule: Invalid function pointer" ) );
			return oexNULL;
		} // end if
		return ptr;
	}

	//==============================================================
	// GetModuleBaseAddress()
	//==============================================================
	/// Returns the loaded modules base address
	oexPVOID GetModuleBaseAddress() { return m_hModule; }

	//==============================================================
	// IsLoaded()
	//==============================================================
	/// Returns non-zero if there is a valid module handle
	oexBOOL IsLoaded() { return ( m_hModule != oexNULL ); }

	//==============================================================
	// HashTable()
	//==============================================================
	/// Returns a hash of the function address table for verification
	oexINT HashTable()
	{	if ( !m_ptrs.Size() )
			return 0;
        return CCrcHash::Hash( 0, (oexUCHAR*)m_ptrs.Ptr(), m_ptrs.SizeInBytes() );
	}

	//==============================================================
	// Size()
	//==============================================================
	/// Returns the number of mapped function pointers
	oexINT Size()
	{	return m_map.Size(); }

private:

	/// Current module handle
	oexPVOID						m_hModule;

	/// Module function pointer array
	TMem< oexPVOID >				m_ptrs;

	/// Function name to pointer map
	t_FunctionMap				 	m_map;

	/// Currently loaded file name
	CStr							m_sFile;
};

