/*------------------------------------------------------------------
// msg_address.h
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

#pragma pack( push, 1 )

/// Network address structure
class CMsgAddress
{
public:

    /// Default constructor
    CMsgAddress()
    {   os::CSys::Zero( this, sizeof( CMsgAddress ) ); 
    }

    /// Copy operator
    CMsgAddress( oexCONST oexGUID *x_pguidId, 
                 oexCONST oexGUID *x_pguidInstance = oexNULL, 
                 oexCONST oexGUID *x_pguidProcess = oexNULL, 
                 oexCONST oexGUID *x_pguidNetwork = oexNULL )
    {   
        if ( x_pguidId )
            guid::CopyGuid( &guidId, x_pguidId ); 
        else
            guid::ZeroGuid( &guidId ); 
        
        if ( x_pguidInstance )
            guid::CopyGuid( &guidInstance, x_pguidInstance ); 
        else
            guid::ZeroGuid( &guidInstance ); 
        
        if ( x_pguidProcess )
            guid::CopyGuid( &guidProcess, x_pguidProcess ); 
        else
            guid::ZeroGuid( &guidProcess ); 

        if ( x_pguidNetwork )
            guid::CopyGuid( &guidNetwork, x_pguidNetwork ); 
        else
            guid::ZeroGuid( &guidNetwork ); 
    }

    /// Copy operator
    CMsgAddress( oexCSTRW x_pId, 
                 oexCONST oexGUID *x_pguidInstance = oexNULL, 
                 oexCONST oexGUID *x_pguidProcess = oexNULL, 
                 oexCONST oexGUID *x_pguidNetwork = oexNULL )
    {   
		oexGUID guidTmp;
		
        if ( x_pId )
            guid::CopyGuid( &guidId, oexCreateMsgId( x_pId, &guidTmp ) ); 
        else
            guid::ZeroGuid( &guidId ); 
        
        if ( x_pguidInstance )
            guid::CopyGuid( &guidInstance, x_pguidInstance ); 
        else
            guid::ZeroGuid( &guidInstance ); 
        
        if ( x_pguidProcess )
            guid::CopyGuid( &guidProcess, x_pguidProcess ); 
        else
            guid::ZeroGuid( &guidProcess ); 

        if ( x_pguidNetwork )
            guid::CopyGuid( &guidNetwork, x_pguidNetwork ); 
        else
            guid::ZeroGuid( &guidNetwork ); 
    }

    /// Copy operator
    CMsgAddress( oexCSTR8 x_pId, 
                 oexCONST oexGUID *x_pguidInstance = oexNULL, 
                 oexCONST oexGUID *x_pguidProcess = oexNULL, 
                 oexCONST oexGUID *x_pguidNetwork = oexNULL )
    {   
		oexGUID guidTmp;

        if ( x_pId )
            guid::CopyGuid( &guidId, oexCreateMsgId( CStrW().Cnv( x_pId ).Ptr(), &guidTmp ) ); 
        else
            guid::ZeroGuid( &guidId ); 
        
        if ( x_pguidInstance )
            guid::CopyGuid( &guidInstance, x_pguidInstance ); 
        else
            guid::ZeroGuid( &guidInstance ); 
        
        if ( x_pguidProcess )
            guid::CopyGuid( &guidProcess, x_pguidProcess ); 
        else
            guid::ZeroGuid( &guidProcess ); 

        if ( x_pguidNetwork )
            guid::CopyGuid( &guidNetwork, x_pguidNetwork ); 
        else
            guid::ZeroGuid( &guidNetwork ); 
    }

    /// Copy constructor
    CMsgAddress( oexCONST CMsgAddress &x_rMa )
    {   os::CSys::MemCpy( this, &x_rMa, sizeof( CMsgAddress ) ); }

    /// Constructs address from a url
//    CMsgAddress( oexCSTR x_sStr )
//    {   Deserialize( x_sStr ); }

    /// Copy operator
    CMsgAddress& operator = ( oexCONST CMsgAddress &x_rMa )
    {   os::CSys::MemCpy( this, &x_rMa, sizeof( CMsgAddress ) ); 
        return *this;
    }

    /// Initialize the address
    void Destroy()
    {   os::CSys::Zero( this, sizeof( CMsgAddress ) ); 
    }

    /// Sets the network address
    void SetNetwork( oexCONST oexGUID *x_pGuid )
    {   if ( !x_pGuid ) guid::ZeroGuid( &guidNetwork );
        else guid::CopyGuid( &guidNetwork, x_pGuid );
    }

    /// Returns the network address
    oexGUID& GetNetwork()
    {   return guidNetwork; }

    /// Sets the process address
    void SetProcess( oexCONST oexGUID *x_pGuid )
    {   if ( !x_pGuid ) guid::ZeroGuid( &guidProcess );
        else guid::CopyGuid( &guidProcess, x_pGuid );
    }

    /// Returns the network address
    oexGUID& GetProcess()
    {   return guidProcess; }

    /// Sets the instance address
    void SetInstance( oexCONST oexGUID *x_pGuid )
    {   if ( !x_pGuid ) guid::ZeroGuid( &guidInstance );
        else guid::CopyGuid( &guidInstance, x_pGuid );
    }

    /// Returns the network address
    oexGUID& GetInstance()
    {   return guidInstance; }

    /// Sets the id
    void SetId( oexCONST oexGUID *x_pGuid )
    {   if ( !x_pGuid ) guid::ZeroGuid( &guidId );
        else guid::CopyGuid( &guidId, x_pGuid );
    }

    /// Returns the id
    oexGUID& GetId()
    {   return guidId; }

    /// Returns non-zero if id is set
    oexBOOL IsSet()
    {   return guid::CmpGuid( &IID_ZEROS, &guidInstance ); }

/*
    oex::oexCSTR pUrl = oexT( "http://user:password@server.com:1111/my/path/doc.php?a=b&c=d" );
    oex::CPropertyBag pbUrl = oex::os::CIpAddress::ParseUrl( pUrl );        

    // Verify each component
    if ( !oexVERIFY( pbUrl[ oexT( "scheme" ) ] == oexT( "http" ) )
         || !oexVERIFY( pbUrl[ oexT( "username" ) ] == oexT( "user" ) )
         || !oexVERIFY( pbUrl[ oexT( "password" ) ] == oexT( "password" ) )
         || !oexVERIFY( pbUrl[ oexT( "host" ) ] == oexT( "server.com" ) )
         || !oexVERIFY( pbUrl[ oexT( "port" ) ] == oexT( "1111" ) )
         || !oexVERIFY( pbUrl[ oexT( "path" ) ] == oexT( "/my/path/doc.php" ) )
         || !oexVERIFY( pbUrl[ oexT( "extra" ) ] == oexT( "a=b&c=d" ) ) )
        return -2;
*/

    /// Serializes the address
    CStr Serialize()
    {
        CPropertyBag pbUrl;

        pbUrl[ oexT( "scheme" ) ] = oexT( "neb" );
        pbUrl[ oexT( "host" ) ] = oexT( "msg.neb" );
        pbUrl[ oexT( "path" ) ] = 
            CStr::BuildPath( CStr().GuidToString( &guidNetwork ),
                             CStr::BuildPath( CStr().GuidToString( &guidProcess ), 
                                              CStr::BuildPath( CStr().GuidToString( &guidInstance ), 
                                                               CStr().GuidToString( &guidId ) ) ) );

        return oex::os::CIpAddress::BuildUrl( pbUrl );
    }

    oexBOOL Deserialize( CStr sStr )
    {
        // Lose old address
        Destroy();

        // Parse the string
        CPropertyBag pbUrl = os::CIpAddress::ParseUrl( sStr.Ptr() );

        // Ensure it's the correct scheme
        if ( pbUrl[ oexT( "scheme" ) ] != oexT( "neb" ) 
             || pbUrl[ oexT( "host" ) ] != oexT( "msg.neb" ) )
            return oexFALSE;

        // Split the path
        CStrList path = CParser::Split( pbUrl[ oexT( "path" ) ].ToString(), oexT( "\\/" ) );

        oexUINT uIndex = 0;
        if ( 3 < path.Size() )
            path[ uIndex++ ]->StringToGuid( &guidNetwork );

        if ( 2 < path.Size() )
            path[ uIndex++ ]->StringToGuid( &guidProcess );

        if ( 1 < path.Size() )
            path[ uIndex++ ]->StringToGuid( &guidInstance );

        if ( 0 < path.Size() )
            path[ uIndex++ ]->StringToGuid( &guidId );

        return oexTRUE;
    }

    operator CStr()
    {   return Serialize(); }


    /// Machine address
    oexGUID         guidNetwork;

    /// Instance
    oexGUID         guidProcess;

    /// Destination
    oexGUID         guidInstance;

    /// Destination
    oexGUID         guidId;
};

#pragma pack( pop )

