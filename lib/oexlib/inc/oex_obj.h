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

	// Data types
    enum
    {
        eTypeInvalid        = 0,

        eTypeBin            = 1,

        eTypeInt            = 2,

        eTypeUInt           = 3,

        eTypeInt8           = 4,

        eTypeUInt8          = 5,

        eTypeInt16          = 6,

        eTypeUInt16         = 7,

        eTypeInt32          = 8,

        eTypeUInt32         = 9,

        eTypeInt64          = 10,

        eTypeUInt64         = 11,

        eTypeChar           = 12,

        eTypeUChar          = 13,

        eTypeCharW          = 14,

        eTypeUCharW         = 15,

        eTypeChar8          = 16,

        eTypeUChar8         = 17,

        eTypeChar16         = 18,

        eTypeUChar16        = 19,

        eTypeChar32         = 20,

        eTypeUChar32        = 21,

        eTypeChar64         = 22,

        eTypeUChar64        = 23,

        eTypeFloat          = 24,

        eTypeDouble         = 25,

        eTypeLongDouble     = 26,

        eTypeStrW           = 27,

        eTypeStr8           = 28,

        eTypeStr16          = 29,

        eTypeStr32          = 30,

        eTypeStr64          = 31,

        eTypeGuid           = 32
    };

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


    // Invalid param type
    template< typename T >
        static oexUINT GetType()
        {   oexASSERT( 0 );
            return eTypeInvalid;
        }

    template<> oexITS oexUINT GetType< oexCHAR >()
    {	switch( sizeof( oexCHAR ) )
		{	case 1 : return eTypeChar8;
			case 2 : return eTypeChar16;
			case 4 : return eTypeChar32;
			case 8 : return eTypeChar64;
		} // end switch
		return eTypeInvalid;
    }

    template<> oexITS oexUINT GetType< oexUCHAR >()
    {	switch( sizeof( oexUCHAR ) )
		{	case 1 : return eTypeUChar8;
			case 2 : return eTypeUChar16;
			case 4 : return eTypeUChar32;
			case 8 : return eTypeUChar64;
		} // end switch
		return eTypeInvalid;
    }

    template<> oexITS oexUINT GetType< oexCHARW >()
    {	switch( sizeof( oexCHARW ) )
		{	case 1 : return eTypeChar8;
			case 2 : return eTypeChar16;
			case 4 : return eTypeChar32;
			case 8 : return eTypeChar64;
		} // end switch
		return eTypeInvalid;
    }

    template<> oexITS oexUINT GetType< oexSHORT >()
    {	switch( sizeof( oexSHORT ) )
		{	case 1 : return eTypeInt8;
			case 2 : return eTypeInt16;
			case 4 : return eTypeInt32;
			case 8 : return eTypeInt64;
		} // end switch
		return eTypeInvalid;
    }

    template<> oexITS oexUINT GetType< oexUSHORT >()
    {	switch( sizeof( oexUSHORT ) )
		{	case 1 : return eTypeUInt8;
			case 2 : return eTypeUInt16;
			case 4 : return eTypeUInt32;
			case 8 : return eTypeUInt64;
		} // end switch
		return eTypeInvalid;
    }

    template<> oexITS oexUINT GetType< oexINT >()
    {	switch( sizeof( oexINT ) )
		{	case 1 : return eTypeInt8;
			case 2 : return eTypeInt16;
			case 4 : return eTypeInt32;
			case 8 : return eTypeInt64;
		} // end switch
		return eTypeInvalid;
    }

    template<> oexITS oexUINT GetType< oexUINT >()
    {	switch( sizeof( oexUINT ) )
		{	case 1 : return eTypeUInt8;
			case 2 : return eTypeUInt16;
			case 4 : return eTypeUInt32;
			case 8 : return eTypeUInt64;
		} // end switch
		return eTypeInvalid;
    }

    template<> oexITS oexUINT GetType< oexLONG >()
    {	switch( sizeof( oexLONG ) )
		{	case 1 : return eTypeInt8;
			case 2 : return eTypeInt16;
			case 4 : return eTypeInt32;
			case 8 : return eTypeInt64;
		} // end switch
		return eTypeInvalid;
    }

    template<> oexITS oexUINT GetType< oexULONG >()
    {	switch( sizeof( oexULONG ) )
		{	case 1 : return eTypeUInt8;
			case 2 : return eTypeUInt16;
			case 4 : return eTypeUInt32;
			case 8 : return eTypeUInt64;
		} // end switch
		return eTypeInvalid;
    }

    template<> oexITS oexUINT GetType< oexFLOAT >()
    {   return eTypeFloat; }

    template<> oexITS oexUINT GetType< oexDOUBLE >()
    {   return eTypeDouble; }

    template<> oexITS oexUINT GetType< oexCONST oexCHAR * >()
    {	switch( sizeof( oexCHAR ) )
		{	case 1 : return eTypeStr8;
			case 2 : return eTypeStr16;
			case 4 : return eTypeStr32;
			case 8 : return eTypeStr64;
		} // end switch
		return eTypeInvalid;
    }

    template<> oexITS oexUINT GetType< oexCONST oexCHARW * >()
    {	switch( sizeof( oexCHAR ) )
		{	case 1 : return eTypeStr8;
			case 2 : return eTypeStr16;
			case 4 : return eTypeStr32;
			case 8 : return eTypeStr64;
		} // end switch
		return eTypeInvalid;
    }

    template<> oexITS oexUINT GetType< oexGUID >()
    {   return eTypeGuid; }

};
