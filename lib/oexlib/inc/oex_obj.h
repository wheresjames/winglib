/*------------------------------------------------------------------
// obj_info.h
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

/// Special object functions
namespace obj
{
    // iii Unfortunately, Doxygen craps out on these next 
    //     function templates.

	//==============================================================
	// Size()
	//==============================================================
	/// Returns the size of the object
	/**
		\param [in] x_obj		-	The object whose size will be 
								    returned.
	*/
	template < typename T >
		static oexUINT Size( oexCONST T *x_obj ) 
        {   return sizeof( T ); }
		
    // +++ These next two are dangerous since they could be invoked by actual 
	//     pointers to chars as opposed to strings.  Please fix!
    template<> oexITS oexUINT Size< oexCHAR8 >( oexCONST oexCHAR8 *x_obj )
    {   return zstr::Length( x_obj ); }

    template<> oexITS oexUINT Size< oexCHARW >( oexCONST oexCHARW *x_obj )
    {   return zstr::Length( x_obj ) * sizeof( oexCHARW ); }

	template < typename T >
		static oexUINT Size( oexCONST T **x_obj ) 
        {   return sizeof( T ); }

    template<> oexITS oexUINT Size< oexCHAR8 >( oexCONST oexCHAR8 **x_obj )
    {   return zstr::Length( *x_obj ); }

    template<> oexITS oexUINT Size< oexCHARW >( oexCONST oexCHARW **x_obj )
    {   return zstr::Length( *x_obj ) * sizeof( oexCHARW ); }

	//==============================================================
	// Terminator()
	//==============================================================
	/// Returns the size of the object terminator
	/**
		\param [in] x_obj		-	Returns the length of a strings
                                    NULL terminator.  Returns zero
                                    for other objects.
	*/
	template < typename T >
		static oexUINT Terminator( oexCONST T *x_obj ) 
        {   return 0; }

    template<> oexITS oexUINT Terminator< oexCSTR8 >( oexCONST oexCSTR8 *x_obj )
    {   return 1; }

    template<> oexITS oexUINT Terminator< oexCSTRW >( oexCONST oexCSTRW *x_obj )
    {   return 2; }

	//==============================================================
	// Ptr()
	//==============================================================
	/// Returns a pointer to the memory
	/**
		\param [in] x_ptr		-	The object whose size will be 
									returned.
	*/
	template < typename T >
		static oexPVOID Ptr( oexCONST T *x_ptr ) 
        {   return (oexPVOID)x_ptr; }

	template < typename T >
		static oexPVOID Ptr( oexCONST T **x_ptr ) 
        {   return (oexPVOID)x_ptr; }

    template<> oexITS oexPVOID Ptr< oexCHAR8 >( oexCONST oexCHAR8 **x_ptr ) 
    {   return (oexPVOID)*x_ptr; }

    template<> oexITS oexPVOID Ptr< oexCHARW >( oexCONST oexCHARW **x_ptr ) 
    {   return (oexPVOID)*x_ptr; }

	//==============================================================
	// Compare()
	//==============================================================
    /// Compares objects
    template < typename T >
        static oexBOOL Compare( oexCONST T &k1,oexCONST T &k2 )
        {   return !os::CSys::MemCmp( &k1, &k2, sizeof( T ) );
        }

    template<> oexITS oexBOOL Compare< oexCSTR8 >( oexCONST oexCSTR8 &k1, oexCONST oexCSTR8 &k2 )
    {   return !str::Compare( k1, zstr::Length( k1 ), k2, zstr::Length( k2 ) ); }

    template<> oexITS oexBOOL Compare< oexCSTRW >( oexCONST oexCSTRW &k1, oexCONST oexCSTRW &k2 )
    {   return !str::Compare( k1, zstr::Length( k1 ), k2, zstr::Length( k2 ) ); }


};
