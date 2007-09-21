/*------------------------------------------------------------------
// fifo_sync.h
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
// CFifoSync
//
/// Implements a first in first out synchronized buffer
/**
	Use this to add variable length blocks into a synchronized
	first-in-first-out buffer.
*/
//==================================================================
class CFifoSync : public CCircBuf
{
public:

    enum
    {
        /// The default number of max buffers
        eDefaultMaxBuffers = 1024
    };

    struct SBufferInfo
    {
	    /// Fifo head pointer
	    oexUINT				        uHeadPtr;

	    /// Fifo tail pointer
	    oexUINT				        uTailPtr;
    };

public:

	//==============================================================
	// SetName()
	//==============================================================
    /// Sets the shared memory name
    /**
        \param [in] x_pName -   Shared memory name
        \param [in] x_uLen  -   Length of the string in x_pName.
                                Can be zero if the string is
                                NULL terminated.
        
    */
    virtual oexCSTR SetName( oexCSTR x_pName, oexUINT x_uLen = 0 )
    {   
        if ( x_pName && !x_uLen ) 
            x_uLen = zstr::Length( x_pName );
        
        // Set shared name for the block sizes
        if ( !x_pName || !*x_pName )
            m_auSize.SetName( oexNULL );
        else
            m_auSize.SetName( CStr( x_pName, x_uLen ).Append( oexT( "_block" ) ).Ptr() );

        return CCircBuf::SetName( x_pName, x_uLen );
    }

	//==============================================================
	// Allocate()
	//==============================================================
	/// Pre allocates the specified amount of data for the circular buffer
	/**
		\param [in] x_uSize		-	Number of bytes to allocate.
		
		\return Non-zero if success
	
		\see 
	*/
	virtual oexBOOL Allocate( oexUINT x_uSize )
    {
        if ( !CCircBuf::Allocate( x_uSize ) )
            return oexFALSE;

        return AllocateBuffers();
    }

	//==============================================================
	// Allocate()
	//==============================================================
	/// Pre allocates the specified amount of data for the circular buffer
	/**
		\param [in] x_uSize		    -	Number of bytes to allocate.
        \param [in] x_uMaxBuffers   -   Maximum number of buffers
		
		\return Non-zero if success
	
		\see 
	*/
	virtual oexBOOL Allocate( oexUINT x_uSize, oexUINT x_uMaxBuffers )
    {
        if ( !CCircBuf::Allocate( x_uSize ) )
            return oexFALSE;

        m_uMaxBuffers = cmn::NextPower2( x_uMaxBuffers );

        return AllocateBuffers();
    }

	//==============================================================
	// Destroy2()
	//==============================================================
    /// Destroys the fifo sync
    void CFifoSync::Destroy2();

	//==============================================================
	// SkipBlock()
	//==============================================================
	/// Skips the next data block in the buffer
	/**
		\return Non-zero if success
	*/
	oexBOOL SkipBlock();

	//==============================================================
	// EndFifoWrite()
	//==============================================================
	/// Advances head fifo head pointer, i.e. commits write
	/**		
		\return Non-zero if success
	*/
	oexBOOL EndFifoWrite();

	//==============================================================
	// InitFifoWrite()
	//==============================================================
	/// Ensures space for a fifo write
	/**		
		\return Non-zero if success
	*/
	oexBOOL InitFifoWrite();

	//==============================================================
	// AddFifo()
	//==============================================================
	/// Adds a data block to the fifo
	/**
		\param [in] x_pBuf		-	Pointer to the data to add
		\param [in] x_uSize		-	Size of the data in x_pBuf
		\param [in] x_uEncode	-	Optional encoding
		
		\return Non-zero if success
	
		\see 
	*/
	oexBOOL AddFifo( oexCPVOID x_pBuf, oexUINT x_uSize, oexUINT x_uEncode = 0 );

	//==============================================================
	// AllocateBuffers()
	//==============================================================
	/// Allocates buffers for fifo
	/**							  		
		\return Non-zero if success
	*/
	oexBOOL AllocateBuffers();

	/// Constructor
	CFifoSync();

	/// Destructor
	virtual ~CFifoSync();

  	//==============================================================
	// Destroy()
	//==============================================================
	/// Releases all resources associated with the circular buffer.
	virtual void Destroy();

	//==============================================================
	// Write()
	//==============================================================
	/// Writes a block to the fifo
	/**
		\param [in] x_pBuf		-	Pointer to the data block
		\param [in] x_uSize		-	Size of the data in x_pBuf
		\param [in] x_uEncode	-	Optional encoding
		
		\return Non-zero if success
	
		\see 
	*/
	virtual oexBOOL Write( oexCPVOID x_pBuf, oexUINT x_uSize, oexUINT x_uEncode = 0 );

	//==============================================================
	// Write()
	//==============================================================
	/// Write a string to the buffer
	/**
		\param [in] x_pStr	-	String value to write to buffer
		
		\return Non-zero if success.
	
		\see 
	*/
	virtual oexBOOL Write( oexCSTR x_pStr )
    {   return Write( x_pStr, zstr::Length( x_pStr ) ); }

	//==============================================================
	// Write()
	//==============================================================
	/// Write a string to the buffer
	/**
		\param [in] x_sStr	-	String to write
		
		\return Non-zero if success.
	
		\see 
	*/
	virtual oexBOOL Write( CStr x_sStr )
    {   return Write( x_sStr.Ptr(), x_sStr.Length() ); }

	//==============================================================
	// Read()
	//==============================================================
	/// Reads a block of data from the fifo
	/**
		\param [out] x_pBuf		-	Pointer to buffer that receives the data
		\param [in] x_uSize		-	Size of buffer in x_pBuf
		\param [in] x_puRead	-	Number of bytes read into x_pBuf
		\param [in] x_uEncode	-	Optional decoding
		
		\return Non-zero if success
	
		\see 
	*/
	virtual oexBOOL Read( oexPVOID x_pBuf, oexUINT x_uSize, oexUINT *x_puRead, oexUINT x_uEncode = 0 );

    virtual oexBOOL Read( CStr &x_sStr, oexUINT x_uMax = 0 );

    virtual CStr Read( oexUINT x_uMax = 0 )
    {   CStr str; Read( str, x_uMax ); return str; }


	//==============================================================
	// Peek()
	//==============================================================
	/// Peeks at the data in the fifo buffer
	/**
		\param [out] x_pBuf		-	Pointer to buffer that receives the data
		\param [in] x_uSize		-	Size of buffer in x_pBuf
		\param [in] x_puRead	-	Number of bytes read into x_pBuf
		\param [in] x_lOffset	-	Offset from first byte to start read
		\param [in] x_uEncode	-	Optional decoding
		
		\return 
	
		\see 
	*/
	virtual oexBOOL Peek( oexPVOID x_pBuf, oexUINT x_uSize, oexUINT *x_puRead, oexLONG x_lOffset = 0, oexUINT x_uEncode = 0 );

    virtual oexBOOL Peek( CStr &x_sStr, oexUINT x_uMax = 0 );

    virtual CStr Peek( oexUINT x_uMax = 0 )
    {   CStr str; Peek( str, x_uMax ); return str; }

	//==============================================================
	// GetMaxBuffers()
	//==============================================================
	/// Returns the number of total buffer slots in the fifo
	oexUINT GetMaxBuffers() { return m_uMaxBuffers; }

	//==============================================================
	// SetMaxBuffers()
	//==============================================================
	/// Sets the total number of buffers slots in the fifo
	/**
		\param [in] x_uMaxBuffers	-	Number of buffers
	*/
	void SetMaxBuffers( oexUINT x_uMaxBuffers ) 
    {   m_uMaxBuffers = cmn::NextPower2( x_uMaxBuffers ); }

	//==============================================================
	// GetUsedBuffers()
	//==============================================================
	/// Returns the number of buffer slots in the fifo that are in use
	oexUINT GetUsedBuffers()
	{   
        CTlLocalLock ll( *this );
	    if ( !ll.IsLocked() ) 
            return oexFALSE;

        if ( !m_pBi )
            return oexFALSE;

        return GetMaxRead( m_pBi->uTailPtr, m_pBi->uHeadPtr, m_uMaxBuffers ); 
    }

private:

	/// Maximum fifo buffer size
	oexUINT				        m_uMaxBuffers;

    /// Buffer information
    SBufferInfo                 *m_pBi;

    /// Pointer to size buffer
    oexUINT                     *m_pBuf;

	/// Fifo block size values
	TMem< oexUINT >             m_auSize;
};
