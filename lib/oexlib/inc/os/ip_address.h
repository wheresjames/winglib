/*------------------------------------------------------------------
// ip_address.h
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
// CIpAddress
//
/// Windows socket address wrapper
/**
	\see CAutoSocket, CSocketProtocol, CIpSocket
*/
//==================================================================
class CIpAddress
{
public:

    enum
    {
        // IPV4 address type
        eAddrTypeIpv4           = 0,

        // IPV6 address type
        eAddrTypeIpv6           = 1

    };

public:

    /// Default constructor
    CIpAddress() 
    {   m_uType = 0; m_uCrc = 0; m_uPort = 0; m_llIpv6 = 0; 
    }

    /// Resets address information
    void Destroy() 
    {   m_uType = 0; m_uCrc = 0; m_uPort = 0; m_llIpv6 = 0; 
    }

    //==============================================================
    // GetId()
    //==============================================================
    /// Generates a guid describing the connection.
    /**
        \param [in] x_pGuid -   Receives the guid

        This function uses the address and port to generate a unique
        id for the connection.

    */
    oexCONST oexGUID* GetId( oexGUID *x_pGuid );

    //==============================================================
    // GetId()
    //==============================================================
    /// Generates a guid describing the connection.
    CStr GetId()
    {   oexGUID guid;
        return CStr().GuidToString( GetId( &guid ) ); 
    }

    /// Sets the address from an ID
    CIpAddress& SetId( oexCONST oexGUID *x_pGuid );

    /// Sets the address from an ID
    CIpAddress& SetId( CStr &sId )
    {   oexGUID guid;
        return SetId( sId.StringToGuid( &guid ) ); 
    }

    /// Builds a url from the given information
    static CStr BuildUrl( CPropertyBag &x_pbUi );

    /// Parses a url into components
    static CPropertyBag ParseUrl( oexCSTR pUrl, oexUINT uMaxBufferSize = 0 );

    /// Returns the dot address
    CStr GetDotAddress();

    /// Fills in the address using dot address
    oexBOOL SetDotAddress( oexCSTR x_pDotAddress, oexINT32 uPort, oexINT32 uType = eAddrTypeIpv4 );

    /// Sets the raw address values
    oexBOOL SetRawAddress( oexINT64 x_llIp, oexINT32 x_uPort, oexINT32 x_uType = eAddrTypeIpv4 );

    /// Validates the address check sum
    oexBOOL CIpAddress::ValidateAddress();

    //==============================================================
    // LookupUrl()
    //==============================================================
    /// Gets url server address information from DNS server
    /**
	    \param [in] x_pUrl	-	Url to lookup
        \param [in] x_uPort -   Optional port to set

	    \return Returns non-zero if success.
    */
    oexBOOL LookupUrl( oexCSTR x_pUrl, oexINT32 uPort = 0, oexINT32 uType = eAddrTypeIpv4 );

    //==============================================================
    // LookupHost()
    //==============================================================
    /// Gets host address information from DNS server
    /**
	    \param [in] x_pServer	-	Host address to lookup
        \param [in] x_uPort     -   Optional port to set

	    \return Returns non-zero if success.
    */
    oexBOOL LookupHost( oexCSTR x_pServer, oexINT32 uPort, oexINT32 uType = eAddrTypeIpv4 );

    /// Returns the binary ip address value
    oexINT64 GetIpv6() 
    {   return m_llIpv6; }

    /// Returns the lo part of the ip address
    oexINT32 GetIpv4() 
    {   return m_uIpv4; }

    /// Returns the port number
    oexINT32 GetPort() 
    {   return m_uPort; }

    /// Returns the port number
    oexINT32 GetType() 
    {   return m_uType; }

    /// Returns the address check sum
    oexINT32 GetCrc() 
    {   return m_uCrc; }

    /// Returns the GUID
    oexGUID& GetGuid() 
    {   return m_guid; }

    /// Guid structure
    operator oexGUID*() 
    {   return &m_guid; }

    /// Copy another address
    CIpAddress& Copy( CIpAddress &rIa )
    {   guid::CopyGuid( &m_guid, &rIa.m_guid ); 
        ValidateAddress(); return *this; 
    }

    /// Copy operator
    CIpAddress& operator = ( CIpAddress &rIa )
    {   return Copy( rIa ); }

    /// Compare addresses
    oexBOOL Cmp( CIpAddress &rIa )
    {   return guid::CmpGuid( &m_guid, &rIa.m_guid ); }

    /// EQ operator
    oexBOOL operator == ( CIpAddress &rIa )
    {   return Cmp( rIa ); }

    /// NEQ operator
    oexBOOL operator != ( CIpAddress &rIa )
    {   return !Cmp( rIa ); }

private:

    union
    {
        struct
        {
            /// Connection Port
	        oexUINT32			    m_uPort;

	        /// CRC
	        oexUINT16			    m_uCrc;

	        /// Address type
	        oexUINT16			    m_uType;

            union
            {
    	        /// IPV6 Address
	            oexINT64			    m_llIpv6;

                struct
                {
                    /// Filler
                    oexINT32            m_uIpv4Extra;

                    /// IPV4 address
                    oexINT32            m_uIpv4;

                };
            };
        };

        /// Unique id
        oexGUID                 m_guid;
    };

};

