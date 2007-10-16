/*------------------------------------------------------------------
// msg_target.h
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

/// Invokes an arbitrary function
/**
    I decided to auto-gen the code rather than use macros to make
    things a little easier to debug.
*/
class CMsgTarget
{
public:

    /// Constructor
    CMsgTarget()
    {   m_uParams = 0;
        m_pClass = m_pThunk = 0;        
        *(oexINT64*)m_rawFunction = 0;
        *(oexINT64*)&m_rawFunction[ 8 ] = 0;
    }

    /// Copy constructor
    CMsgTarget( CMsgTarget &x_rD )
    {   m_uParams = x_rD.m_uParams;
        m_pClass = x_rD.m_pClass;
        m_pThunk = x_rD.m_pThunk;
        *(oexINT64*)m_rawFunction = *(oexINT64*)x_rD.m_rawFunction;
        *(oexINT64*)&m_rawFunction[ 8 ] = *(oexINT64*)&x_rD.m_rawFunction[ 8 ];
    }

    /// Copy constructor
    CMsgTarget& operator = ( const CMsgTarget &x_rD )
    {   m_uParams = x_rD.m_uParams;
        m_pClass = x_rD.m_pClass;
        m_pThunk = x_rD.m_pThunk;
        *(oexINT64*)m_rawFunction = *(oexINT64*)x_rD.m_rawFunction;
        *(oexINT64*)&m_rawFunction[ 8 ] = *(oexINT64*)&x_rD.m_rawFunction[ 8 ];
        return *this;
    }

public:

    /// Calls function
    CMsg operator ()( CMsg &x_params )
    {
        if ( !IsValid() )
            return CMsg();

        typedef CMsg (*t_Thunk)( oexCPVOID, oexPVOID, CMsg& );
        return ((t_Thunk)m_pThunk)( m_pClass, &m_rawFunction, x_params ); 
    }

    /// Returns non zero if there is a target set
    oexBOOL IsValid()
    {   return oexCHECK_PTR( m_pClass ) ? oexTRUE : oexFALSE; }

    /// Returns the number of parameters
    oexUINT GetNumParams()
    {   return m_uParams; }

private:

    /// Class pointer
    oexCPVOID               m_pClass;

    /// Function pointer buffer
    oexCHAR8                m_rawFunction[ 16 ];

    /// Thunk
    oexPVOID                m_pThunk;

    /// Number of params
    oexUINT                 m_uParams; 


//------------------------------------------------------------------
//-------- Begin auto generated code (agen_msg_target.php) ---------
//------------------------------------------------------------------

//---- 0 Parameter(s) ---------------------------------------------------

public:                 

    /// 0 parameter constructor
    template < typename T_RET, typename T_CLASS >
    CMsgTarget( const T_CLASS *x_pC, T_RET(T_CLASS::*x_pF)(  ) )
    {
        _Register< T_RET >( x_pC, x_pF ); 
    }

    /// Registers function with 0 parameters
    template < typename T_RET, typename T_CLASS >
        void Register( const T_CLASS *x_pC, T_RET(T_CLASS::*x_pF)(  ) )
    {   
        _Register< T_RET >( x_pC, x_pF ); 
    }

private:

    /// Register 0 parameters
    template < typename T_RET, typename T_CLASS, typename T_FUNCTION >
        void _Register( oexCONST T_CLASS *x_pC, oexCONST T_FUNCTION x_pF )
    {   
        m_uParams = 0;
        m_pClass = x_pC;
        *(T_FUNCTION*)(oexPVOID)m_rawFunction = x_pF;
        typedef CMsg (*t_Thunk)( oexPVOID, oexPVOID );
        m_pThunk = (t_Thunk)&SThunk_0< T_RET >::Thunk< T_RET, T_CLASS, T_FUNCTION >;
    }

    /// Makes the function call to a 0 parameter function
    template< typename T_RET > struct SThunk_0
    {   template < typename T_RET, typename T_CLASS, typename T_FUNCTION >
            static CMsg Thunk( oexCPVOID x_pC, oexPVOID x_pF, CMsg& x_params )
        {
            if (    !oexCHECK( x_params.IsValid() ) )
                return CMsg();

            return oexMsg( x_params.GetPriority() | oexMsgReply, x_params.Mi().Src(),
                           ( ( (T_CLASS*)x_pC )->*( *( (T_FUNCTION*)x_pF ) ) )
                                (  ) ); 
        }
    };

    /// Makes the function call to a 0 parameter function with void return type
    template <> struct SThunk_0< void >
    {   template < typename T_RET, typename T_CLASS, typename T_FUNCTION >
            static CMsg Thunk( oexCPVOID x_pC, oexPVOID x_pF, CMsg& x_params )
        {
            if (    !oexCHECK( x_params.IsValid() ) )
                return CMsg();

            ( ( (T_CLASS*)x_pC )->*( *( (T_FUNCTION*)x_pF ) ) )
                (  ); 

            return oexMsg( x_params.GetPriority() | oexMsgReply, 
                           x_params.Mi().Src() ); 
        }
    };
    

//---- 1 Parameter(s) ---------------------------------------------------

public:                 

    /// 1 parameter constructor
    template < typename T_RET, typename T_P1, typename T_CLASS >
    CMsgTarget( const T_CLASS *x_pC, T_RET(T_CLASS::*x_pF)( T_P1 ) )
    {
        _Register< T_RET, T_P1 >( x_pC, x_pF ); 
    }

    /// Registers function with 1 parameters
    template < typename T_RET, typename T_P1, typename T_CLASS >
        void Register( const T_CLASS *x_pC, T_RET(T_CLASS::*x_pF)( T_P1 ) )
    {   
        _Register< T_RET, T_P1 >( x_pC, x_pF ); 
    }

private:

    /// Register 1 parameters
    template < typename T_RET, typename T_P1, typename T_CLASS, typename T_FUNCTION >
        void _Register( oexCONST T_CLASS *x_pC, oexCONST T_FUNCTION x_pF )
    {   
        m_uParams = 1;
        m_pClass = x_pC;
        *(T_FUNCTION*)(oexPVOID)m_rawFunction = x_pF;
        typedef CMsg (*t_Thunk)( oexPVOID, oexPVOID, T_P1 );
        m_pThunk = (t_Thunk)&SThunk_1< T_RET >::Thunk< T_RET, T_P1, T_CLASS, T_FUNCTION >;
    }

    /// Makes the function call to a 1 parameter function
    template< typename T_RET > struct SThunk_1
    {   template < typename T_RET, typename T_P1, typename T_CLASS, typename T_FUNCTION >
            static CMsg Thunk( oexCPVOID x_pC, oexPVOID x_pF, CMsg& x_params )
        {
            if (    !oexCHECK( x_params.IsValid() )
                    || !oexCHECK( x_params.VerifyParam< T_P1 >( 0 ) ) )
                return CMsg();

            return oexMsg( x_params.GetPriority() | oexMsgReply, x_params.Mi().Src(),
                           ( ( (T_CLASS*)x_pC )->*( *( (T_FUNCTION*)x_pF ) ) )
                                ( x_params[ 0 ] ) ); 
        }
    };

    /// Makes the function call to a 1 parameter function with void return type
    template <> struct SThunk_1< void >
    {   template < typename T_RET, typename T_P1, typename T_CLASS, typename T_FUNCTION >
            static CMsg Thunk( oexCPVOID x_pC, oexPVOID x_pF, CMsg& x_params )
        {
            if (    !oexCHECK( x_params.IsValid() )
                    || !oexCHECK( x_params.VerifyParam< T_P1 >( 0 ) ) )
                return CMsg();

            ( ( (T_CLASS*)x_pC )->*( *( (T_FUNCTION*)x_pF ) ) )
                ( x_params[ 0 ] ); 

            return oexMsg( x_params.GetPriority() | oexMsgReply, 
                           x_params.Mi().Src() ); 
        }
    };
    

//---- 2 Parameter(s) ---------------------------------------------------

public:                 

    /// 2 parameter constructor
    template < typename T_RET, typename T_P1, typename T_P2, typename T_CLASS >
    CMsgTarget( const T_CLASS *x_pC, T_RET(T_CLASS::*x_pF)( T_P1, T_P2 ) )
    {
        _Register< T_RET, T_P1, T_P2 >( x_pC, x_pF ); 
    }

    /// Registers function with 2 parameters
    template < typename T_RET, typename T_P1, typename T_P2, typename T_CLASS >
        void Register( const T_CLASS *x_pC, T_RET(T_CLASS::*x_pF)( T_P1, T_P2 ) )
    {   
        _Register< T_RET, T_P1, T_P2 >( x_pC, x_pF ); 
    }

private:

    /// Register 2 parameters
    template < typename T_RET, typename T_P1, typename T_P2, typename T_CLASS, typename T_FUNCTION >
        void _Register( oexCONST T_CLASS *x_pC, oexCONST T_FUNCTION x_pF )
    {   
        m_uParams = 2;
        m_pClass = x_pC;
        *(T_FUNCTION*)(oexPVOID)m_rawFunction = x_pF;
        typedef CMsg (*t_Thunk)( oexPVOID, oexPVOID, T_P1, T_P2 );
        m_pThunk = (t_Thunk)&SThunk_2< T_RET >::Thunk< T_RET, T_P1, T_P2, T_CLASS, T_FUNCTION >;
    }

    /// Makes the function call to a 2 parameter function
    template< typename T_RET > struct SThunk_2
    {   template < typename T_RET, typename T_P1, typename T_P2, typename T_CLASS, typename T_FUNCTION >
            static CMsg Thunk( oexCPVOID x_pC, oexPVOID x_pF, CMsg& x_params )
        {
            if (    !oexCHECK( x_params.IsValid() )
                    || !oexCHECK( x_params.VerifyParam< T_P1 >( 0 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P2 >( 1 ) ) )
                return CMsg();

            return oexMsg( x_params.GetPriority() | oexMsgReply, x_params.Mi().Src(),
                           ( ( (T_CLASS*)x_pC )->*( *( (T_FUNCTION*)x_pF ) ) )
                                ( x_params[ 0 ], x_params[ 1 ] ) ); 
        }
    };

    /// Makes the function call to a 2 parameter function with void return type
    template <> struct SThunk_2< void >
    {   template < typename T_RET, typename T_P1, typename T_P2, typename T_CLASS, typename T_FUNCTION >
            static CMsg Thunk( oexCPVOID x_pC, oexPVOID x_pF, CMsg& x_params )
        {
            if (    !oexCHECK( x_params.IsValid() )
                    || !oexCHECK( x_params.VerifyParam< T_P1 >( 0 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P2 >( 1 ) ) )
                return CMsg();

            ( ( (T_CLASS*)x_pC )->*( *( (T_FUNCTION*)x_pF ) ) )
                ( x_params[ 0 ], x_params[ 1 ] ); 

            return oexMsg( x_params.GetPriority() | oexMsgReply, 
                           x_params.Mi().Src() ); 
        }
    };
    

//---- 3 Parameter(s) ---------------------------------------------------

public:                 

    /// 3 parameter constructor
    template < typename T_RET, typename T_P1, typename T_P2, typename T_P3, typename T_CLASS >
    CMsgTarget( const T_CLASS *x_pC, T_RET(T_CLASS::*x_pF)( T_P1, T_P2, T_P3 ) )
    {
        _Register< T_RET, T_P1, T_P2, T_P3 >( x_pC, x_pF ); 
    }

    /// Registers function with 3 parameters
    template < typename T_RET, typename T_P1, typename T_P2, typename T_P3, typename T_CLASS >
        void Register( const T_CLASS *x_pC, T_RET(T_CLASS::*x_pF)( T_P1, T_P2, T_P3 ) )
    {   
        _Register< T_RET, T_P1, T_P2, T_P3 >( x_pC, x_pF ); 
    }

private:

    /// Register 3 parameters
    template < typename T_RET, typename T_P1, typename T_P2, typename T_P3, typename T_CLASS, typename T_FUNCTION >
        void _Register( oexCONST T_CLASS *x_pC, oexCONST T_FUNCTION x_pF )
    {   
        m_uParams = 3;
        m_pClass = x_pC;
        *(T_FUNCTION*)(oexPVOID)m_rawFunction = x_pF;
        typedef CMsg (*t_Thunk)( oexPVOID, oexPVOID, T_P1, T_P2, T_P3 );
        m_pThunk = (t_Thunk)&SThunk_3< T_RET >::Thunk< T_RET, T_P1, T_P2, T_P3, T_CLASS, T_FUNCTION >;
    }

    /// Makes the function call to a 3 parameter function
    template< typename T_RET > struct SThunk_3
    {   template < typename T_RET, typename T_P1, typename T_P2, typename T_P3, typename T_CLASS, typename T_FUNCTION >
            static CMsg Thunk( oexCPVOID x_pC, oexPVOID x_pF, CMsg& x_params )
        {
            if (    !oexCHECK( x_params.IsValid() )
                    || !oexCHECK( x_params.VerifyParam< T_P1 >( 0 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P2 >( 1 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P3 >( 2 ) ) )
                return CMsg();

            return oexMsg( x_params.GetPriority() | oexMsgReply, x_params.Mi().Src(),
                           ( ( (T_CLASS*)x_pC )->*( *( (T_FUNCTION*)x_pF ) ) )
                                ( x_params[ 0 ], x_params[ 1 ], x_params[ 2 ] ) ); 
        }
    };

    /// Makes the function call to a 3 parameter function with void return type
    template <> struct SThunk_3< void >
    {   template < typename T_RET, typename T_P1, typename T_P2, typename T_P3, typename T_CLASS, typename T_FUNCTION >
            static CMsg Thunk( oexCPVOID x_pC, oexPVOID x_pF, CMsg& x_params )
        {
            if (    !oexCHECK( x_params.IsValid() )
                    || !oexCHECK( x_params.VerifyParam< T_P1 >( 0 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P2 >( 1 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P3 >( 2 ) ) )
                return CMsg();

            ( ( (T_CLASS*)x_pC )->*( *( (T_FUNCTION*)x_pF ) ) )
                ( x_params[ 0 ], x_params[ 1 ], x_params[ 2 ] ); 

            return oexMsg( x_params.GetPriority() | oexMsgReply, 
                           x_params.Mi().Src() ); 
        }
    };
    

//---- 4 Parameter(s) ---------------------------------------------------

public:                 

    /// 4 parameter constructor
    template < typename T_RET, typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_CLASS >
    CMsgTarget( const T_CLASS *x_pC, T_RET(T_CLASS::*x_pF)( T_P1, T_P2, T_P3, T_P4 ) )
    {
        _Register< T_RET, T_P1, T_P2, T_P3, T_P4 >( x_pC, x_pF ); 
    }

    /// Registers function with 4 parameters
    template < typename T_RET, typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_CLASS >
        void Register( const T_CLASS *x_pC, T_RET(T_CLASS::*x_pF)( T_P1, T_P2, T_P3, T_P4 ) )
    {   
        _Register< T_RET, T_P1, T_P2, T_P3, T_P4 >( x_pC, x_pF ); 
    }

private:

    /// Register 4 parameters
    template < typename T_RET, typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_CLASS, typename T_FUNCTION >
        void _Register( oexCONST T_CLASS *x_pC, oexCONST T_FUNCTION x_pF )
    {   
        m_uParams = 4;
        m_pClass = x_pC;
        *(T_FUNCTION*)(oexPVOID)m_rawFunction = x_pF;
        typedef CMsg (*t_Thunk)( oexPVOID, oexPVOID, T_P1, T_P2, T_P3, T_P4 );
        m_pThunk = (t_Thunk)&SThunk_4< T_RET >::Thunk< T_RET, T_P1, T_P2, T_P3, T_P4, T_CLASS, T_FUNCTION >;
    }

    /// Makes the function call to a 4 parameter function
    template< typename T_RET > struct SThunk_4
    {   template < typename T_RET, typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_CLASS, typename T_FUNCTION >
            static CMsg Thunk( oexCPVOID x_pC, oexPVOID x_pF, CMsg& x_params )
        {
            if (    !oexCHECK( x_params.IsValid() )
                    || !oexCHECK( x_params.VerifyParam< T_P1 >( 0 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P2 >( 1 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P3 >( 2 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P4 >( 3 ) ) )
                return CMsg();

            return oexMsg( x_params.GetPriority() | oexMsgReply, x_params.Mi().Src(),
                           ( ( (T_CLASS*)x_pC )->*( *( (T_FUNCTION*)x_pF ) ) )
                                ( x_params[ 0 ], x_params[ 1 ], x_params[ 2 ], x_params[ 3 ] ) ); 
        }
    };

    /// Makes the function call to a 4 parameter function with void return type
    template <> struct SThunk_4< void >
    {   template < typename T_RET, typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_CLASS, typename T_FUNCTION >
            static CMsg Thunk( oexCPVOID x_pC, oexPVOID x_pF, CMsg& x_params )
        {
            if (    !oexCHECK( x_params.IsValid() )
                    || !oexCHECK( x_params.VerifyParam< T_P1 >( 0 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P2 >( 1 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P3 >( 2 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P4 >( 3 ) ) )
                return CMsg();

            ( ( (T_CLASS*)x_pC )->*( *( (T_FUNCTION*)x_pF ) ) )
                ( x_params[ 0 ], x_params[ 1 ], x_params[ 2 ], x_params[ 3 ] ); 

            return oexMsg( x_params.GetPriority() | oexMsgReply, 
                           x_params.Mi().Src() ); 
        }
    };
    

//---- 5 Parameter(s) ---------------------------------------------------

public:                 

    /// 5 parameter constructor
    template < typename T_RET, typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5, typename T_CLASS >
    CMsgTarget( const T_CLASS *x_pC, T_RET(T_CLASS::*x_pF)( T_P1, T_P2, T_P3, T_P4, T_P5 ) )
    {
        _Register< T_RET, T_P1, T_P2, T_P3, T_P4, T_P5 >( x_pC, x_pF ); 
    }

    /// Registers function with 5 parameters
    template < typename T_RET, typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5, typename T_CLASS >
        void Register( const T_CLASS *x_pC, T_RET(T_CLASS::*x_pF)( T_P1, T_P2, T_P3, T_P4, T_P5 ) )
    {   
        _Register< T_RET, T_P1, T_P2, T_P3, T_P4, T_P5 >( x_pC, x_pF ); 
    }

private:

    /// Register 5 parameters
    template < typename T_RET, typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5, typename T_CLASS, typename T_FUNCTION >
        void _Register( oexCONST T_CLASS *x_pC, oexCONST T_FUNCTION x_pF )
    {   
        m_uParams = 5;
        m_pClass = x_pC;
        *(T_FUNCTION*)(oexPVOID)m_rawFunction = x_pF;
        typedef CMsg (*t_Thunk)( oexPVOID, oexPVOID, T_P1, T_P2, T_P3, T_P4, T_P5 );
        m_pThunk = (t_Thunk)&SThunk_5< T_RET >::Thunk< T_RET, T_P1, T_P2, T_P3, T_P4, T_P5, T_CLASS, T_FUNCTION >;
    }

    /// Makes the function call to a 5 parameter function
    template< typename T_RET > struct SThunk_5
    {   template < typename T_RET, typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5, typename T_CLASS, typename T_FUNCTION >
            static CMsg Thunk( oexCPVOID x_pC, oexPVOID x_pF, CMsg& x_params )
        {
            if (    !oexCHECK( x_params.IsValid() )
                    || !oexCHECK( x_params.VerifyParam< T_P1 >( 0 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P2 >( 1 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P3 >( 2 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P4 >( 3 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P5 >( 4 ) ) )
                return CMsg();

            return oexMsg( x_params.GetPriority() | oexMsgReply, x_params.Mi().Src(),
                           ( ( (T_CLASS*)x_pC )->*( *( (T_FUNCTION*)x_pF ) ) )
                                ( x_params[ 0 ], x_params[ 1 ], x_params[ 2 ], x_params[ 3 ], x_params[ 4 ] ) ); 
        }
    };

    /// Makes the function call to a 5 parameter function with void return type
    template <> struct SThunk_5< void >
    {   template < typename T_RET, typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5, typename T_CLASS, typename T_FUNCTION >
            static CMsg Thunk( oexCPVOID x_pC, oexPVOID x_pF, CMsg& x_params )
        {
            if (    !oexCHECK( x_params.IsValid() )
                    || !oexCHECK( x_params.VerifyParam< T_P1 >( 0 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P2 >( 1 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P3 >( 2 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P4 >( 3 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P5 >( 4 ) ) )
                return CMsg();

            ( ( (T_CLASS*)x_pC )->*( *( (T_FUNCTION*)x_pF ) ) )
                ( x_params[ 0 ], x_params[ 1 ], x_params[ 2 ], x_params[ 3 ], x_params[ 4 ] ); 

            return oexMsg( x_params.GetPriority() | oexMsgReply, 
                           x_params.Mi().Src() ); 
        }
    };
    

//---- 6 Parameter(s) ---------------------------------------------------

public:                 

    /// 6 parameter constructor
    template < typename T_RET, typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5, typename T_P6, typename T_CLASS >
    CMsgTarget( const T_CLASS *x_pC, T_RET(T_CLASS::*x_pF)( T_P1, T_P2, T_P3, T_P4, T_P5, T_P6 ) )
    {
        _Register< T_RET, T_P1, T_P2, T_P3, T_P4, T_P5, T_P6 >( x_pC, x_pF ); 
    }

    /// Registers function with 6 parameters
    template < typename T_RET, typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5, typename T_P6, typename T_CLASS >
        void Register( const T_CLASS *x_pC, T_RET(T_CLASS::*x_pF)( T_P1, T_P2, T_P3, T_P4, T_P5, T_P6 ) )
    {   
        _Register< T_RET, T_P1, T_P2, T_P3, T_P4, T_P5, T_P6 >( x_pC, x_pF ); 
    }

private:

    /// Register 6 parameters
    template < typename T_RET, typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5, typename T_P6, typename T_CLASS, typename T_FUNCTION >
        void _Register( oexCONST T_CLASS *x_pC, oexCONST T_FUNCTION x_pF )
    {   
        m_uParams = 6;
        m_pClass = x_pC;
        *(T_FUNCTION*)(oexPVOID)m_rawFunction = x_pF;
        typedef CMsg (*t_Thunk)( oexPVOID, oexPVOID, T_P1, T_P2, T_P3, T_P4, T_P5, T_P6 );
        m_pThunk = (t_Thunk)&SThunk_6< T_RET >::Thunk< T_RET, T_P1, T_P2, T_P3, T_P4, T_P5, T_P6, T_CLASS, T_FUNCTION >;
    }

    /// Makes the function call to a 6 parameter function
    template< typename T_RET > struct SThunk_6
    {   template < typename T_RET, typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5, typename T_P6, typename T_CLASS, typename T_FUNCTION >
            static CMsg Thunk( oexCPVOID x_pC, oexPVOID x_pF, CMsg& x_params )
        {
            if (    !oexCHECK( x_params.IsValid() )
                    || !oexCHECK( x_params.VerifyParam< T_P1 >( 0 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P2 >( 1 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P3 >( 2 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P4 >( 3 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P5 >( 4 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P6 >( 5 ) ) )
                return CMsg();

            return oexMsg( x_params.GetPriority() | oexMsgReply, x_params.Mi().Src(),
                           ( ( (T_CLASS*)x_pC )->*( *( (T_FUNCTION*)x_pF ) ) )
                                ( x_params[ 0 ], x_params[ 1 ], x_params[ 2 ], x_params[ 3 ], x_params[ 4 ], x_params[ 5 ] ) ); 
        }
    };

    /// Makes the function call to a 6 parameter function with void return type
    template <> struct SThunk_6< void >
    {   template < typename T_RET, typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5, typename T_P6, typename T_CLASS, typename T_FUNCTION >
            static CMsg Thunk( oexCPVOID x_pC, oexPVOID x_pF, CMsg& x_params )
        {
            if (    !oexCHECK( x_params.IsValid() )
                    || !oexCHECK( x_params.VerifyParam< T_P1 >( 0 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P2 >( 1 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P3 >( 2 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P4 >( 3 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P5 >( 4 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P6 >( 5 ) ) )
                return CMsg();

            ( ( (T_CLASS*)x_pC )->*( *( (T_FUNCTION*)x_pF ) ) )
                ( x_params[ 0 ], x_params[ 1 ], x_params[ 2 ], x_params[ 3 ], x_params[ 4 ], x_params[ 5 ] ); 

            return oexMsg( x_params.GetPriority() | oexMsgReply, 
                           x_params.Mi().Src() ); 
        }
    };
    

//---- 7 Parameter(s) ---------------------------------------------------

public:                 

    /// 7 parameter constructor
    template < typename T_RET, typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5, typename T_P6, typename T_P7, typename T_CLASS >
    CMsgTarget( const T_CLASS *x_pC, T_RET(T_CLASS::*x_pF)( T_P1, T_P2, T_P3, T_P4, T_P5, T_P6, T_P7 ) )
    {
        _Register< T_RET, T_P1, T_P2, T_P3, T_P4, T_P5, T_P6, T_P7 >( x_pC, x_pF ); 
    }

    /// Registers function with 7 parameters
    template < typename T_RET, typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5, typename T_P6, typename T_P7, typename T_CLASS >
        void Register( const T_CLASS *x_pC, T_RET(T_CLASS::*x_pF)( T_P1, T_P2, T_P3, T_P4, T_P5, T_P6, T_P7 ) )
    {   
        _Register< T_RET, T_P1, T_P2, T_P3, T_P4, T_P5, T_P6, T_P7 >( x_pC, x_pF ); 
    }

private:

    /// Register 7 parameters
    template < typename T_RET, typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5, typename T_P6, typename T_P7, typename T_CLASS, typename T_FUNCTION >
        void _Register( oexCONST T_CLASS *x_pC, oexCONST T_FUNCTION x_pF )
    {   
        m_uParams = 7;
        m_pClass = x_pC;
        *(T_FUNCTION*)(oexPVOID)m_rawFunction = x_pF;
        typedef CMsg (*t_Thunk)( oexPVOID, oexPVOID, T_P1, T_P2, T_P3, T_P4, T_P5, T_P6, T_P7 );
        m_pThunk = (t_Thunk)&SThunk_7< T_RET >::Thunk< T_RET, T_P1, T_P2, T_P3, T_P4, T_P5, T_P6, T_P7, T_CLASS, T_FUNCTION >;
    }

    /// Makes the function call to a 7 parameter function
    template< typename T_RET > struct SThunk_7
    {   template < typename T_RET, typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5, typename T_P6, typename T_P7, typename T_CLASS, typename T_FUNCTION >
            static CMsg Thunk( oexCPVOID x_pC, oexPVOID x_pF, CMsg& x_params )
        {
            if (    !oexCHECK( x_params.IsValid() )
                    || !oexCHECK( x_params.VerifyParam< T_P1 >( 0 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P2 >( 1 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P3 >( 2 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P4 >( 3 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P5 >( 4 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P6 >( 5 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P7 >( 6 ) ) )
                return CMsg();

            return oexMsg( x_params.GetPriority() | oexMsgReply, x_params.Mi().Src(),
                           ( ( (T_CLASS*)x_pC )->*( *( (T_FUNCTION*)x_pF ) ) )
                                ( x_params[ 0 ], x_params[ 1 ], x_params[ 2 ], x_params[ 3 ], x_params[ 4 ], x_params[ 5 ], x_params[ 6 ] ) ); 
        }
    };

    /// Makes the function call to a 7 parameter function with void return type
    template <> struct SThunk_7< void >
    {   template < typename T_RET, typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5, typename T_P6, typename T_P7, typename T_CLASS, typename T_FUNCTION >
            static CMsg Thunk( oexCPVOID x_pC, oexPVOID x_pF, CMsg& x_params )
        {
            if (    !oexCHECK( x_params.IsValid() )
                    || !oexCHECK( x_params.VerifyParam< T_P1 >( 0 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P2 >( 1 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P3 >( 2 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P4 >( 3 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P5 >( 4 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P6 >( 5 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P7 >( 6 ) ) )
                return CMsg();

            ( ( (T_CLASS*)x_pC )->*( *( (T_FUNCTION*)x_pF ) ) )
                ( x_params[ 0 ], x_params[ 1 ], x_params[ 2 ], x_params[ 3 ], x_params[ 4 ], x_params[ 5 ], x_params[ 6 ] ); 

            return oexMsg( x_params.GetPriority() | oexMsgReply, 
                           x_params.Mi().Src() ); 
        }
    };
    

//---- 8 Parameter(s) ---------------------------------------------------

public:                 

    /// 8 parameter constructor
    template < typename T_RET, typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5, typename T_P6, typename T_P7, typename T_P8, typename T_CLASS >
    CMsgTarget( const T_CLASS *x_pC, T_RET(T_CLASS::*x_pF)( T_P1, T_P2, T_P3, T_P4, T_P5, T_P6, T_P7, T_P8 ) )
    {
        _Register< T_RET, T_P1, T_P2, T_P3, T_P4, T_P5, T_P6, T_P7, T_P8 >( x_pC, x_pF ); 
    }

    /// Registers function with 8 parameters
    template < typename T_RET, typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5, typename T_P6, typename T_P7, typename T_P8, typename T_CLASS >
        void Register( const T_CLASS *x_pC, T_RET(T_CLASS::*x_pF)( T_P1, T_P2, T_P3, T_P4, T_P5, T_P6, T_P7, T_P8 ) )
    {   
        _Register< T_RET, T_P1, T_P2, T_P3, T_P4, T_P5, T_P6, T_P7, T_P8 >( x_pC, x_pF ); 
    }

private:

    /// Register 8 parameters
    template < typename T_RET, typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5, typename T_P6, typename T_P7, typename T_P8, typename T_CLASS, typename T_FUNCTION >
        void _Register( oexCONST T_CLASS *x_pC, oexCONST T_FUNCTION x_pF )
    {   
        m_uParams = 8;
        m_pClass = x_pC;
        *(T_FUNCTION*)(oexPVOID)m_rawFunction = x_pF;
        typedef CMsg (*t_Thunk)( oexPVOID, oexPVOID, T_P1, T_P2, T_P3, T_P4, T_P5, T_P6, T_P7, T_P8 );
        m_pThunk = (t_Thunk)&SThunk_8< T_RET >::Thunk< T_RET, T_P1, T_P2, T_P3, T_P4, T_P5, T_P6, T_P7, T_P8, T_CLASS, T_FUNCTION >;
    }

    /// Makes the function call to a 8 parameter function
    template< typename T_RET > struct SThunk_8
    {   template < typename T_RET, typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5, typename T_P6, typename T_P7, typename T_P8, typename T_CLASS, typename T_FUNCTION >
            static CMsg Thunk( oexCPVOID x_pC, oexPVOID x_pF, CMsg& x_params )
        {
            if (    !oexCHECK( x_params.IsValid() )
                    || !oexCHECK( x_params.VerifyParam< T_P1 >( 0 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P2 >( 1 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P3 >( 2 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P4 >( 3 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P5 >( 4 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P6 >( 5 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P7 >( 6 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P8 >( 7 ) ) )
                return CMsg();

            return oexMsg( x_params.GetPriority() | oexMsgReply, x_params.Mi().Src(),
                           ( ( (T_CLASS*)x_pC )->*( *( (T_FUNCTION*)x_pF ) ) )
                                ( x_params[ 0 ], x_params[ 1 ], x_params[ 2 ], x_params[ 3 ], x_params[ 4 ], x_params[ 5 ], x_params[ 6 ], x_params[ 7 ] ) ); 
        }
    };

    /// Makes the function call to a 8 parameter function with void return type
    template <> struct SThunk_8< void >
    {   template < typename T_RET, typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5, typename T_P6, typename T_P7, typename T_P8, typename T_CLASS, typename T_FUNCTION >
            static CMsg Thunk( oexCPVOID x_pC, oexPVOID x_pF, CMsg& x_params )
        {
            if (    !oexCHECK( x_params.IsValid() )
                    || !oexCHECK( x_params.VerifyParam< T_P1 >( 0 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P2 >( 1 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P3 >( 2 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P4 >( 3 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P5 >( 4 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P6 >( 5 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P7 >( 6 ) )
                    || !oexCHECK( x_params.VerifyParam< T_P8 >( 7 ) ) )
                return CMsg();

            ( ( (T_CLASS*)x_pC )->*( *( (T_FUNCTION*)x_pF ) ) )
                ( x_params[ 0 ], x_params[ 1 ], x_params[ 2 ], x_params[ 3 ], x_params[ 4 ], x_params[ 5 ], x_params[ 6 ], x_params[ 7 ] ); 

            return oexMsg( x_params.GetPriority() | oexMsgReply, 
                           x_params.Mi().Src() ); 
        }
    };
        
//------------------------------------------------------------------
//-------- End auto generated code (agen_msg_target.php) -----------
//------------------------------------------------------------------

};

