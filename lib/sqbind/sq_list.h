/*------------------------------------------------------------------
// sq_list.h
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

// namespace
namespace sqbind
{
    // +++ Not sure this class actually works

    /// Squirrel list adaptor class
    class CSqList
    {
    public:

        /// List object type
		typedef stdString	          	t_Obj;

        /// List type
        typedef oexStdList( t_Obj )		t_List;

    private:

        /// List
        t_List                          m_lst;

    public:

        /// Returns a reference to the underlying vector
        t_List& vector();

        /// Registers the vector class for use with Squirrel
        static void Register( SquirrelVM &vm );

        /// Adds an element to the vector
        void push_back( const t_Obj &s );

    private:

        /// Internal squirrel function, returns value of specified item
        SquirrelObject _get( HSQUIRRELVM v );

        /// Internal squirrel function used to iterate list items
        SquirrelObject _nexti( HSQUIRRELVM v );

    };

}; // end namespace

// Declare type for use as squirrel parameters
DECLARE_INSTANCE_TYPE_NAME( sqbind::CSqList, CSqList )
