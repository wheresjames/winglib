/*------------------------------------------------------------------
// sq_vector.h
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
    /// Squirrel vector adaptor class
    class CSqVector
    {
    public:

        /// List object type
		typedef std::tstring            t_Obj;

        /// List type
        typedef std::vector< t_Obj >    t_List;

    private:

        /// List
        t_List                          m_lst;

    public:
        
		/// Default constructor
		CSqVector() {}

        /// Returns a reference to the underlying vector
        t_List& list() 
        {   return m_lst; }

        /// Registers the vector class for use with Squirrel
        static void Register( SquirrelVM &vm )
        {
            SqPlus::SQClassDef< CSqVector >( vm, oexT( "CSqVector" ) )
                    . func( &CSqVector::push_back,     oexT( "push_back" ) )
                    . func( &CSqVector::_get,          oexT( "_get" ) )
                    . func( &CSqVector::_nexti,        oexT( "_nexti" ) )
                ;
        }

        /// Adds an element to the vector
        void push_back( const t_Obj &s )
        {
            m_lst.push_back( s );
        }

    private:

        /// Internal squirrel function, returns value of specified item
        SquirrelObject _get( HSQUIRRELVM v )
        {
            StackHandler sa( v );

            // Ensure valid index
            int nIndex = sa.GetInt( 2 );
            if ( 0 > nIndex || m_lst.size() <= (unsigned int)nIndex )
                return SquirrelObject( v );

            // Stuff string
            SquirrelObject so( v );
	        sq_pushstring( v, m_lst[ nIndex ].c_str(), (int)m_lst[ nIndex ].length() );
	        so.AttachToStackObject( -1 );
	        sq_pop( v, 1 );

            return so;
        }

        /// Internal squirrel function used to iterate list items
        SquirrelObject _nexti( HSQUIRRELVM v )
        {
            StackHandler sa( v );

            SQObjectType type = (SQObjectType)sa.GetType( 2 );

            switch( type )
            {
                case OT_NULL:
                {
                    if ( !m_lst.size() )
                        return SquirrelObject( v );

                    SquirrelObject so( v );

                    so = 0;

                    return so;

                } break;

                case OT_INTEGER:
                {
                    // Verify index
                    unsigned int idx = (unsigned int)sa.GetInt( 2 );
                    if ( ( idx + 1 ) >= m_lst.size() )
                        return SquirrelObject( v );

                    SquirrelObject so( v );

                    so = idx + 1;

                    return so;

                } break;

            } // end switch

            // ???
            sa.ThrowError( _T( "Invalid index type" ) );
            return SquirrelObject( v );
        }

    };

}; // end namespace

// Declare type for use as squirrel parameters
DECLARE_INSTANCE_TYPE_NAME( sqbind::CSqVector, CSqVector )
