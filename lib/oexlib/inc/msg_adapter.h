/*------------------------------------------------------------------
// msg_adapter.h
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

class CMsg;
class CMsgTypeAdapter
{
public:

    CMsgTypeAdapter()
    {   m_uSize = 0;
        m_uType = 0;
        m_pMem = oexNULL;
        m_pMsg = oexNULL;
    }

    /// Constructor
    CMsgTypeAdapter( CMsg *x_pMsg, oexPVOID x_pMem, oexUINT x_uSize, oexUINT x_uType )
    {   m_pMem = x_pMem;        
        m_uSize = x_uSize;
        m_uType = x_uType;
        m_pMsg = oexNULL;
    }

    /// Copy 
    CMsgTypeAdapter& Copy( oexCONST CMsgTypeAdapter &x_rMa )
    {   m_pMem = x_rMa.m_pMem;
        m_uSize = x_rMa.m_uSize;
        m_uType = x_rMa.m_uType;
        m_pMsg = x_rMa.m_pMsg;
        return *this;
    }

    /// Sets the memory info
    void Set( CMsg *x_pMsg, oexPVOID x_pMem, oexUINT x_uSize, oexUINT x_uType )
    {   m_pMem = x_pMem;
        m_uSize = x_uSize;
        m_uType = x_uType;
    }

public:

    enum
    {
        eTypeInvalid        = 0,

        eTypeBin            = 1,

        eTypeInt8           = 2,

        eTypeUInt8          = 3,

        eTypeInt16          = 4,

        eTypeUInt16         = 5,

        eTypeInt32          = 6,

        eTypeUInt32         = 7,

        eTypeInt64          = 8,

        eTypeUInt64         = 9,

        eTypeFloat          = 10,

        eTypeDouble         = 11,

        eTypeLongDouble     = 12,

        eTypeStrW           = 13,

        eTypeStr8           = 14,

        eTypeStr16          = 15,

        eTypeStr32          = 16,

        eTypeStr64          = 17,

        eTypeGuid           = 18
    };

public:

    // Invalid param type
    template< typename T >
        static oexUINT GetType()
        {   oexASSERT( 0 );
            return eTypeInvalid; 
        }

    template<> static oexUINT GetType< oexCHAR >()
    {   return eTypeInt8; }

    template<> static oexUINT GetType< oexUCHAR >()
    {   return eTypeUInt8; }

    template<> static oexUINT GetType< oexSHORT >()
    {   return eTypeInt16; }

    template<> static oexUINT GetType< oexUSHORT >()
    {   return eTypeUInt16; }

    template<> static oexUINT GetType< oexINT >()
    {   if ( 4 == sizeof( oexINT ) )
            return eTypeInt32; 
        else if ( 8 == sizeof( oexINT ) )
            return eTypeInt64; 
        else if ( 2 == sizeof( oexINT ) )
            return eTypeInt16; 
        else if ( 1 == sizeof( oexINT ) )
            return eTypeInt8; 
    }

    template<> static oexUINT GetType< oexUINT >()
    {   if ( 4 == sizeof( oexUINT ) )
            return eTypeInt32; 
        else if ( 8 == sizeof( oexUINT ) )
            return eTypeInt64; 
        else if ( 2 == sizeof( oexUINT ) )
            return eTypeInt16; 
        else if ( 1 == sizeof( oexUINT ) )
            return eTypeInt8; 
    }

    template<> static oexUINT GetType< oexLONG >()
    {   return eTypeInt32; }

    template<> static oexUINT GetType< oexULONG >()
    {   return eTypeUInt32; }

    template<> static oexUINT GetType< oexFLOAT >()
    {   return eTypeFloat; }

    template<> static oexUINT GetType< oexDOUBLE >()
    {   return eTypeDouble; }

    template<> static oexUINT GetType< CStrW >()
    {   return eTypeStrW; }

    template<> static oexUINT GetType< CStr8 >()
    {   return eTypeStr8; }

    template<> static oexUINT GetType< oexCSTR8 >()
    {   return eTypeStr8; }

    template<> static oexUINT GetType< oexCSTRW >()
    {   return eTypeStrW; }

    template<> static oexUINT GetType< oexGUID >()
    {   return eTypeGuid; }

public:

    template< typename T >
        oexBOOL CastInteger( T *p )
        {
            if ( !m_pMem )
                return oexFALSE;

            if ( eTypeInt8 == m_uType )
            {   if ( !oexCHECK( sizeof( oexCHAR ) == m_uSize ) )
                    return oexFALSE;            
                *p = (T)*(oexCHAR*)m_pMem;
            } // end if

            else if ( eTypeUInt8 == m_uType )
            {   if ( oexCHECK( sizeof( oexUCHAR ) == m_uSize ) )
                    return oexFALSE;            
                *p = (T)*(oexUCHAR*)m_pMem;
            } // end if

            else if ( eTypeInt16 == m_uType )
            {   if ( !oexCHECK( sizeof( oexSHORT ) == m_uSize ) )
                    return oexFALSE;            
                *p = (T)*(oexSHORT*)m_pMem;
            } // end if

            else if ( eTypeUInt16 == m_uType )
            {   if ( oexCHECK( sizeof( oexUSHORT ) == m_uSize ) )
                    return oexFALSE;            
                *p = (T)*(oexUSHORT*)m_pMem;
            } // end if

            else if ( eTypeInt32 == m_uType )
            {   if ( !oexCHECK( sizeof( oexLONG ) == m_uSize ) )
                    return oexFALSE;            
                *p = (T)*(oexLONG*)m_pMem;
            } // end if

            else if ( eTypeUInt32 == m_uType )
            {   if ( oexCHECK( sizeof( oexULONG ) == m_uSize ) )
                    return oexFALSE;            
                *p = (T)*(oexULONG*)m_pMem;
            } // end if

            else if ( eTypeInt64 == m_uType )
            {   if ( !oexCHECK( sizeof( oexLONG ) == m_uSize ) )
                    return oexFALSE;            
                *p = (T)*(oexLONG*)m_pMem;
            } // end if

            else if ( eTypeUInt64 == m_uType )
            {   if ( oexCHECK( sizeof( oexULONG ) == m_uSize ) )
                    return oexFALSE;            
                *p = (T)*(oexULONG*)m_pMem;
            } // end if

            else if ( eTypeFloat == m_uType )
            {   if ( oexCHECK( sizeof( oexFLOAT ) == m_uSize ) )
                    return oexFALSE;            
                *p = (T)*(oexFLOAT*)m_pMem;
            } // end if

            else if ( eTypeDouble == m_uType )
            {   if ( oexCHECK( sizeof( oexDOUBLE ) == m_uSize ) )
                    return oexFALSE;            
                *p = (T)*(oexDOUBLE*)m_pMem;
            } // end if

            else if ( eTypeDouble == m_uType )
            {   if ( oexCHECK( sizeof( oexDOUBLE ) == m_uSize ) )
                    return oexFALSE;            
                *p = (T)*(oexDOUBLE*)m_pMem;
            } // end if

            else 
                return oexFALSE;

            return oexTRUE;
        }

    template< typename T >
        oexBOOL CastStrToInteger( T *p )
        {
            if ( !m_pMem )
                return oexFALSE;

            if ( eTypeStrW == m_uType )
                *p = (T)CStrW( (oexSTRW)m_pMem, m_uSize / sizeof( oexCSTRW ) ).ToNum();

            else if ( eTypeStr8 == m_uType )
                *p = (T)CStr8( (oexSTR8)m_pMem, m_uSize / sizeof( oexCSTR8 ) ).ToNum();

            else 
                return oexFALSE;

            return oexTRUE;
        }


    template< typename T >
        oexBOOL CastStrToDouble( T *p )
        {
            if ( !m_pMem )
                return oexFALSE;

            if ( eTypeStrW == m_uType )
                *p = (T)CStrW( (oexSTRW)m_pMem, m_uSize / sizeof( oexCSTRW ) ).ToDouble();

            else if ( eTypeStr8 == m_uType )
                *p = (T)CStr8( (oexSTR8)m_pMem, m_uSize / sizeof( oexCSTR8 ) ).ToDouble();

            else 
                return oexFALSE;

            return oexTRUE;
        }

    oexBOOL operator == ( oexINT v )
    {   return (oexINT)*this == v; }

    operator oexCONST oexINT()
    {   oexINT ret = 0;
        return CastInteger( &ret ) ? ret : ( CastStrToInteger( &ret ) ? ret : 0 );
    }

    oexINT ToInt()
    {   return (oexINT)*this; }

    oexBOOL operator == ( oexUINT v )
    {   return (oexUINT)*this == v; }

    operator oexCONST oexUINT()
    {   oexUINT ret = 0;
        return CastInteger( &ret ) ? ret : ( CastStrToInteger( &ret ) ? ret : 0 );
    }

    oexUINT ToUInt()
    {   return (oexUINT)*this; }

    oexBOOL operator == ( oexCHAR v )
    {   return (oexCHAR)*this == v; }

    operator oexCONST oexCHAR()
    {   oexCHAR ret = 0;
        return CastInteger( &ret ) ? ret : ( CastStrToInteger( &ret ) ? ret : 0 );
    }

    oexBOOL operator == ( oexUCHAR v )
    {   return (oexUCHAR)*this == v; }

    operator oexCONST oexUCHAR()
    {   oexUCHAR ret = 0;
        return CastInteger( &ret ) ? ret : ( CastStrToInteger( &ret ) ? ret : 0 );
    }

    oexBOOL operator == ( oexSHORT v )
    {   return (oexSHORT)*this == v; }

    operator oexCONST oexSHORT()
    {   oexSHORT ret = 0;
        return CastInteger( &ret ) ? ret : ( CastStrToInteger( &ret ) ? ret : 0 );
    }

    oexBOOL operator == ( oexUSHORT v )
    {   return (oexUSHORT)*this == v; }

    operator oexCONST oexUSHORT()
    {   oexUSHORT ret = 0;
        return CastInteger( &ret ) ? ret : ( CastStrToInteger( &ret ) ? ret : 0 );
    }

    oexBOOL operator == ( oexLONG v )
    {   return (oexLONG)*this == v; }

    operator oexCONST oexLONG()
    {   oexLONG ret = 0;
        return CastInteger( &ret ) ? ret : ( CastStrToInteger( &ret ) ? ret : 0 );
    }

    oexBOOL operator == ( oexULONG v )
    {   return (oexULONG)*this == v; }

    operator oexCONST oexULONG()
    {   oexULONG ret = 0;
        return CastInteger( &ret ) ? ret : ( CastStrToInteger( &ret ) ? ret : 0 );
    }

    oexBOOL operator == ( oexINT64 v )
    {   return (oexINT64)*this == v; }

    operator oexCONST oexINT64()
    {   oexINT64 ret = 0;
        return CastInteger( &ret ) ? ret : ( CastStrToInteger( &ret ) ? ret : 0 );
    }

    oexBOOL operator == ( oexUINT64 v )
    {   return (oexUINT64)*this == v; }

    operator oexCONST oexUINT64()
    {   oexUINT64 ret = 0;
        return CastInteger( &ret ) ? ret : ( CastStrToInteger( &ret ) ? ret : 0 );
    }

    oexBOOL operator == ( oexFLOAT v )
    {   return (oexFLOAT)*this == v; }

    operator oexCONST oexFLOAT()
    {   oexFLOAT ret = 0;
        return CastInteger( &ret ) ? ret : ( CastStrToDouble( &ret ) ? ret : 0 );
    }

    oexFLOAT ToFloat()
    {   return (oexFLOAT)*this; }

    oexBOOL operator == ( oexDOUBLE v )
    {   return (oexDOUBLE)*this == v; }

    operator oexCONST oexDOUBLE()
    {   oexDOUBLE ret = 0;
        return CastInteger( &ret ) ? ret : ( CastStrToDouble( &ret ) ? ret : 0 );
    }

    oexDOUBLE ToDouble()
    {   return (oexDOUBLE)*this; }

    template< typename T >
        oexBOOL CastNumberToString( T *p, oexUINT sz )
        {
            if ( !m_pMem )
                return oexFALSE;

            if ( eTypeInt8 == m_uType )
            {   if ( !oexCHECK( sizeof( oexCHAR ) == m_uSize ) )
                    return oexFALSE;
                os::CSys::StrFmt( p, sz, oexTT( T, "%li" ), (oexLONG)*(oexCHAR*)m_pMem );
            } // end if

            else if ( eTypeUInt8 == m_uType )
            {   if ( !oexCHECK( sizeof( oexUCHAR ) == m_uSize ) )
                    return oexFALSE;
                os::CSys::StrFmt( p, sz, oexTT( T, "%lu" ), (oexULONG)*(oexUCHAR*)m_pMem );
            } // end if

            else if ( eTypeInt16 == m_uType )
            {   if ( !oexCHECK( sizeof( oexSHORT ) == m_uSize ) )
                    return oexFALSE;
                os::CSys::StrFmt( p, sz, oexTT( T, "%li" ), (oexLONG)*(oexSHORT*)m_pMem );
            } // end if

            else if ( eTypeUInt16 == m_uType )
            {   if ( !oexCHECK( sizeof( oexUSHORT ) == m_uSize ) )
                    return oexFALSE;
                os::CSys::StrFmt( p, sz, oexTT( T, "%lu" ), (oexULONG)*(oexUSHORT*)m_pMem );
            } // end if

            else if ( eTypeInt32 == m_uType )
            {   if ( !oexCHECK( sizeof( oexLONG ) == m_uSize ) )
                    return oexFALSE;
                os::CSys::StrFmt( p, sz, oexTT( T, "%li" ), (oexLONG)*(oexLONG*)m_pMem );
            } // end if

            else if ( eTypeUInt32 == m_uType )
            {   if ( !oexCHECK( sizeof( oexULONG ) == m_uSize ) )
                    return oexFALSE;
                os::CSys::StrFmt( p, sz, oexTT( T, "%lu" ), (oexULONG)*(oexULONG*)m_pMem );
            } // end if

            else if ( eTypeFloat == m_uType )
            {   if ( !oexCHECK( sizeof( oexFLOAT ) == m_uSize ) )
                    return oexFALSE;
                os::CSys::StrFmt( p, sz, oexTT( T, "%f" ), (oexDOUBLE)*(oexFLOAT*)m_pMem );
            } // end if

            else if ( eTypeDouble == m_uType )
            {   if ( !oexCHECK( sizeof( oexDOUBLE ) == m_uSize ) )
                    return oexFALSE;
                os::CSys::StrFmt( p, sz, oexTT( T, "%f" ), (oexDOUBLE)*(oexDOUBLE*)m_pMem );
            } // end if

            else
                return oexFALSE;

            return oexTRUE;
        }

    oexBOOL operator == ( oexCSTR8 v )
    {   return !str::Compare( (oexCSTR8)*this, m_uSize / sizeof( oexCSTR8 ), v, zstr::Length( v ) ); }

    operator oexCSTR8()
    {
        // Types must match here
        if ( GetType< oexCSTR8 >() == m_uType )
            return (oexCSTR8)m_pMem;        

        // Pointer to buffer
        oexSTR8 pStr = m_ucNumBuf;
        *pStr = 0;

        CastNumberToString( pStr, sizeof( m_ucNumBuf ) / sizeof( pStr[ 0 ] ) );

        return pStr;
    }

    operator CStr8()
    {   return CStr8( (oexCSTR8)*this ); }

    CStr8 ToStr8()
    {   return CStr8( (oexCSTR8)*this ); }

    oexBOOL operator == ( oexCSTRW v )
    {   return !str::Compare( (oexCSTRW)*this, m_uSize / sizeof( oexCHARW ), v, zstr::Length( v ) ); }

    operator oexCSTRW()
    {
        // Types must match here
        if ( GetType< oexCSTRW >() == m_uType )
            return (oexCSTRW)m_pMem;

        // Pointer to buffer
        oexSTRW pStr = (oexSTRW)m_ucNumBuf;
        *pStr = 0;

        CastNumberToString( pStr, sizeof( m_ucNumBuf ) / sizeof( pStr[ 0 ] ) );

        return pStr;
    }

    operator CStrW()
    {   return CStrW( (oexCSTRW)*this ); }

    CStrW ToStrW()
    {   return CStrW( (oexCSTRW)*this ); }

    operator CMsg* ()
    {   return m_pMsg; }

    operator CMsgAddress()
    {
#if defined( oexUNICODE )
        return CMsgAddress( (oexCSTRW)*this );
#else
        return CMsgAddress( (oexCSTR8)*this );
#endif        
    }

private:
    
    /// Pointer to memory
    oexPVOID        m_pMem;

    /// Size of memory at m_pMem
    oexUINT         m_uSize;

    /// Type
    oexUINT         m_uType;

    /// Pointer to message object
    CMsg            *m_pMsg;

    /// Buffer, handles conversions, sharing, etc...
//    CStr8           m_sBin;

    /// Buffer for holding number conversions
    oexCHAR8        m_ucNumBuf[ 256 ];
};

