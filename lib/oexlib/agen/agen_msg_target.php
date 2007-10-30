<?php
/*------------------------------------------------------------------
// agen_msg_target.php
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


// C++ template

function _cs( $i, $code )
{   
    $ret = '';

    // Prefix
    if ( $i && isset( $code[ 0 ] ) ) 
        $ret = str_replace( "#", $i, $code[ 0 ] );
    
    // Body
    if ( isset( $code[ 1 ] ) )
        for ( $n = 0; $n < $i; $n++ )
        {
            $ret .= str_replace( "@", $n, str_replace( "#", ( $n + 1 ), $code[ 1 ] ) );

            if ( ( $n + 1 ) != $i && isset( $code[ 2 ] ) ) 
                $ret .= str_replace( "@", ( $n - 1 ), str_replace( "#", $n, $code[ 2 ] ) );

        } // end for

    // Suffix
    if ( $i && isset( $code[ 3 ] ) ) 
        $ret .= str_replace( "#", $i, $code[ 3 ] );

    return $ret;
}

function CodeTemplate( $i )
{
    return str_replace( "#", $i, '

//---- # Parameter(s) ---------------------------------------------------

public:                 

    /// # parameter constructor
    template < typename T_RET, ' . _cs( $i, array( '', 'typename T_P#, ' ) ) . 'typename T_CLASS >
    CMsgTarget( const T_CLASS *x_pC, T_RET(T_CLASS::*x_pF)( ' . _cs( $i, array( '', 'T_P#', ', ' ) ) . ' ) )
    {
        _Register< T_RET' . _cs( $i, array( ', ', 'T_P#', ', ' ) ) . ' >( x_pC, x_pF ); 
    }

    /// Registers function with # parameters
    template < typename T_RET, ' . _cs( $i, array( '', 'typename T_P#, ' ) ) . 'typename T_CLASS >
        void Register( const T_CLASS *x_pC, T_RET(T_CLASS::*x_pF)( ' . _cs( $i, array( '', 'T_P#', ', ' ) ) . ' ) )
    {   
        _Register< T_RET' . _cs( $i, array( ', ', 'T_P#', ', ' ) ) . ' >( x_pC, x_pF ); 
    }

private:

    /// Register # parameters
    template < typename T_RET, ' . _cs( $i, array( '', 'typename T_P#, ' ) ) . 'typename T_CLASS, typename T_FUNCTION >
        void _Register( oexCONST T_CLASS *x_pC, oexCONST T_FUNCTION x_pF )
    {   
        m_uParams = #;
        m_pClass = x_pC;
        *(T_FUNCTION*)(oexPVOID)m_rawFunction = x_pF;
        typedef CMsg (*t_Thunk)( oexPVOID, oexPVOID' . _cs( $i, array( ', ', 'T_P#', ', ' ) ) . ' );
        m_pThunk = (t_Thunk)&SThunk_#< T_RET >::Thunk< T_RET, ' . _cs( $i, array( '', 'T_P#, ' ) ) . 'T_CLASS, T_FUNCTION >;
    }

    /// Makes the function call to a # parameter function
    template< typename T_RET > struct SThunk_#
    {   template < typename T_RET, ' . _cs( $i, array( '', 'typename T_P#, ' ) ) . 'typename T_CLASS, typename T_FUNCTION >
            static CMsg Thunk( oexCPVOID x_pC, oexPVOID x_pF, CMsg& x_params )
        {
            if (    !oexCHECK( x_params.IsValid() )'
                . _cs( $i, array( '', "\r\n                    || !oexCHECK( x_params.VerifyParam< T_P# >( @ ) )" ) ) .
                ' )
                return CMsg();

            return oexMsg( x_params.GetPriority() | oexMsgReply, x_params.Mi().Src(),
                           ( ( (T_CLASS*)x_pC )->*( *( (T_FUNCTION*)x_pF ) ) )
                                ( ' . _cs( $i, array( '', 'x_params[ @ ]', ', ' ) ) . ' ) ); 
        }
    };

    /// Makes the function call to a # parameter function with void return type
    template <> struct SThunk_#< void >
    {   template < typename T_RET, ' . _cs( $i, array( '', 'typename T_P#, ' ) ) . 'typename T_CLASS, typename T_FUNCTION >
            static CMsg Thunk( oexCPVOID x_pC, oexPVOID x_pF, CMsg& x_params )
        {
            if (    !oexCHECK( x_params.IsValid() )'
                . _cs( $i, array( '', "\r\n                    || !oexCHECK( x_params.VerifyParam< T_P# >( @ ) )" ) ) .
                ' )
                return CMsg();

            ( ( (T_CLASS*)x_pC )->*( *( (T_FUNCTION*)x_pF ) ) )
                ( ' . _cs( $i, array( '', 'x_params[ @ ]', ', ' ) ) . ' ); 

            return oexMsg( x_params.GetPriority() | oexMsgReply, 
                           x_params.Mi().Src() ); 
        }
    };
    ' );
}

    $code = '';

    // Generate code for up to eight params
    for( $i = 0; $i <= 8; $i++ )
        $code .= CodeTemplate( $i );

    echo $code;
?>