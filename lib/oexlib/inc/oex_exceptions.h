/*------------------------------------------------------------------
// oex_exceptions.h
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

#ifdef OEX_USE_CPP_EXCEPTIONS

#	ifdef OEX_VERBOSE_COMPILE
#		pragma message( "OEX: Using C++ exceptions." )
#	endif

#	define OEX_TRY					try 
#	define OEX_CATCH_ALL			catch( ... )
#	define OEX_CATCH_RET( e )		catch( ... ) { return e; }
#	define OEX_THROW( type )		throw type
#	define OEX_RETHROW				throw

#else

#	define OEX_THROW_TYPE			
#	define OEX_TRY					
#	define OEX_CATCH_ALL			if ( 0 )
#	define OEX_CATCH_RET( e )
#	define OEX_THROW( type )		
#	define OEX_RETHROW

#endif
