/*------------------------------------------------------------------
// circ_buf.h
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
// CCircBuf
//
/// Implements a circular buffer
/**
	This class encapsulates the functionality of a cicular buffer.  It
	is specifically designed for buffering serial and network communications.
	It has hooks for supporting packetizing, compression, and encryption.
	Efficiency is maintained by allowing inplace operations on the data
	through the use of views.


	Basic test:
  \code

	oex::CCircBuf buf;

	LPCTSTR cstrTest = "Hello World";
	TCHAR szTemp[ 256 ] = "";
	UINT uBufferedData = 0;
	for ( UINT i = 0; i < 1000; i++ )
	{
		// Write several strings to the buffer
		for ( UINT x = 0; x < 8; x++ )
			buf.Write( cstrTest ), uBufferedData += strlen( cstrTest );

		// Read a string
		UINT uRead = 0;
		buf.Read( szTemp, strlen( cstrTest ), &uRead );
		szTemp[ uRead ] = 0;
		uBufferedData -= strlen( cstrTest );

		// Ensure correct data read
		if ( strcmp( szTemp, cstrTest ) )
			MessageBox( NULL, "Test Failed", "Error", MB_OK );
	};

	// Ensure correct number of bytes remain in the buffer
	if ( uBufferedData != buf.GetMaxRead() )
		MessageBox( NULL, "Test Failed", "Error", MB_OK );

  \endcode

*/
//==================================================================
class CCircBuf
{
public:

    enum
    {
        /// Default maximum buffer size of 32 megabytes
        eDefaultMaxBufferSize = ( 32 * 1048576 )
    };

#pragma pack( push, 1 )

    /// Contains information about the buffer
    struct SBufferInfo
    {
	    /// The position of the next character slot available for reading
	    oexUINT					uReadPtr;

	    /// The position of the next character slot available for writing
	    oexUINT					uWritePtr;
    };

#pragma pack( pop )

protected:

	//==============================================================
	// NameEvents()
	//==============================================================
    /// Sets the shared memory name for sync events
    /**
        \param [in] x_pName -   Shared memory base name

    */
    oexBOOL NameEvents( oexCSTR x_pBaseName );

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
    virtual CCircBuf& SetName( oexCSTR x_pName, oexUINT x_uLen = 0 )
    {   NameEvents( x_pName );
        m_memBuffer.SetName( x_pName, x_uLen );
        return *this;
    }

	//==============================================================
	// GetName()
	//==============================================================
    /// Returns the shared name
    oexCSTR GetName()
    {
        return m_memBuffer.GetName();
    }

    //==============================================================
	// Init()
	//==============================================================
	/// Initializes the circular buffer
	/**
		\return Non-zero if success.

		\see
	*/
	oexBOOL Init();

	//==============================================================
	// WaitData()
	//==============================================================
	/// Waits for data to be written to the circular buffer
	/**
		\param [in] x_uTimeout	-	Maximum number of milliseconds to wait for data.

		\return Non-zero if data was received before timeout expired.

		\see
	*/
	oexBOOL WaitData( oexUINT x_uTimeout = os::CResource::cInfinite() );

	//==============================================================
	// WaitEmpty()
	//==============================================================
	/// Waits for the buffer to become empty
	/**
		\param [in] x_uTimeout	-	Maximum number of milliseconds to wait for data.

		\return Non-zero if data was received before timeout expired.

		\see
	*/
	oexBOOL WaitEmpty( oexUINT x_uTimeout = os::CResource::cInfinite() );

	//==============================================================
	// GetWriteView()
	//==============================================================
	/// Returns a view of the buffer with respect to the write pointer
	/**
		\param [in] x_uView		-	Index of the view to return.
		\param [in] x_uOffset	-	Offset from the write pointer of data segment.
		\param [in] x_uSize		-	Total size of data segment
		\param [out] x_ppucBuf	-	Pointer receives pointer to start of view
		\param [out] x_puSize	-	Receives the size of this view.

		To view all of data segment, call with dwView set to 0, increment
		on each call until GetWriteView() returns zero.

		\return Non-zero if success.

		\see GetReadView
	*/
	oexBOOL GetWriteView( oexUINT x_uView, oexUINT x_uOffset, oexUINT x_uSize, oexUCHAR **x_ppucBuf, oexUINT *x_puSize )
	{
        if ( !m_pBi )
            return oexFALSE;
        return GetView( x_uView, NormalizePtr( m_pBi->uWritePtr + x_uOffset, m_uSize ), x_uSize, m_pBuf, m_uSize, x_ppucBuf, x_puSize );
    }

	//==============================================================
	// GetWriteView()
	//==============================================================
	/// Returns a view of the buffer with respect to the read pointer
	/**
		\param [in] x_uView		-	Index of the view to return.
		\param [in] x_uOffset	-	Offset from the read pointer of data segment.
		\param [in] x_uSize		-	Total size of data segment
		\param [out] x_ppucBuf	-	Pointer receives pointer to start of view
		\param [out] x_puSize	-	Receives the size of this view.

		To view all of data segment, call with dwView set to 0, increment
		on each call until GetReadView() returns zero.

		\return Non-zero if success.

		\see GetWriteView
	*/
	oexBOOL GetReadView( oexUINT x_uView, oexUINT x_uOffset, oexUINT x_uSize, oexUCHAR **x_ppucBuf, oexUINT *x_puSize )
	{
        if ( !m_pBi )
            return oexFALSE;

        return GetView( x_uView, NormalizePtr( m_pBi->uReadPtr + x_uOffset, m_uSize ), x_uSize, m_pBuf, m_uSize, x_ppucBuf, x_puSize );
    }

	//==============================================================
	// NormalizePtr()
	//==============================================================
	/// Wraps the specified pointer
	/**
		\param [in] x_uPtr	-	Pointer position
		\param [in] x_uMax	-	Maximum buffer position

		The pointer may point to the byte after the buffer, this wraps it

		\return Returns dwPtr wrapped to dwMax

		\see
	*/
	oexUINT NormalizePtr( oexUINT x_uPtr, oexUINT x_uMax );

	//==============================================================
	// AdvanceReadPtr()
	//==============================================================
	/// Advances the read pointer
	/**
		\param [in] x_lStep	-	Number of bytes to advance read pointer.

		\return Non-zero if success, otherwise zero.

		\see
	*/
	oexBOOL AdvanceReadPtr( oexLONG x_lStep )
	{   oexAutoLock ll( &m_lock );
		if ( !ll.IsLocked() )
            return oexFALSE;
        if ( !m_pBi )
            return oexFALSE;
		if ( !GetMaxRead() )
            return oexFALSE;
		m_pBi->uReadPtr = NormalizePtr( AdvancePtr( m_pBi->uReadPtr, x_lStep, m_uSize ), m_uSize );
		Defrag();
		return oexTRUE;
	}

	//==============================================================
	// AdvanceWritePtr()
	//==============================================================
	/// Advances the write pointer
	/**
		\param [in] x_lStep	-	Number of bytes to advance write pointer

		\return Non-zero if success

		\see
	*/
	oexBOOL AdvanceWritePtr( oexLONG x_lStep )
	{	oexAutoLock ll( &m_lock );
		if ( !ll.IsLocked() )
            return oexFALSE;
        if ( !m_pBi )
            return oexFALSE;
		if ( !GetMaxWrite() )
            return oexFALSE;
		m_pBi->uWritePtr = AdvancePtr( m_pBi->uWritePtr, x_lStep, m_uSize );
		return oexTRUE;
	}

	//==============================================================
	// AdvancePtr()
	//==============================================================
	/// Advances a pointer
	/**
		\param [in] x_uPtr	-	Current pointer value
		\param [in] x_lStep	-	Amount to add to pointer
		\param [in] x_uMax	-	Maximum size of buffer

		\return New pointer value

		\see
	*/
	static oexUINT AdvancePtr( oexUINT x_uPtr, oexLONG x_lStep, oexUINT x_uMax );

	//==============================================================
	// GetView()
	//==============================================================
	/// Returns the desired view of a specified data segment
	/**
		\param [in] x_uView		-	Index of the view to return.
		\param [in] x_uPtr		-	Offset of the start of the segment.
		\param [in] x_uSize		-	Total size of the segment.
		\param [in] x_pucRing	-	Pointer to the start of the buffer.
		\param [in] x_uMax		-	Size of the buffer
		\param [out] x_pucBuf	-	Pointer to start of view
		\param [out] x_puSize	-	Size of this view segment.

		\return Non-zero if success

		\see
	*/
	static oexBOOL GetView( oexUINT x_uView, oexUINT x_uPtr, oexUINT x_uSize, oexUCHAR *x_pucRing, oexUINT x_uMax, oexUCHAR **x_pucBuf, oexUINT *x_puSize );

	//==============================================================
	// Defrag()
	//==============================================================
	/// Defragments the circular buffer
	/**
		Resets read and write pointers to zero if buffer is empty.
		This attempts to keep the buffer from wrapping
	*/
	void Defrag();

	//==============================================================
	// EnsureWriteSpace()
	//==============================================================
	/// Ensures space is available for write by growing buffer if needed.
	/**
		\param [in] x_uSize		-	Number of bytes required.
		\param [in] x_uReadPtr	-	Position of read pointer.
		\param [in] x_uWritePtr	-	Position of write pointer.
		\param [in] x_uMax		-	Maximum size of buffer.

		Checks to ensure the specified amount of space is available for writing.
		If it is not, the function creates it without losing data that is in the buffer.

		\return Space available

		\see
	*/
	oexUINT EnsureWriteSpace( oexUINT x_uSize, oexUINT x_uReadPtr, oexUINT x_uWritePtr, oexUINT x_uMax );

	//==============================================================
	// Resize()
	//==============================================================
	/// Resizes the buffer without losing the data within
	/**
		\param [in] x_uNewSize	-	New size for circular buffer

		\return	New size of buffer

		\see
	*/
	oexUINT Resize( oexUINT x_uNewSize );

	//==============================================================
	// Read()
	//==============================================================
	/// Read a string from the buffer
	/**
		\param [in] x_pStr	-	Buffer to hold string
		\param [in] x_uMax	-	Size of buffer in pStr.

		\return Non-zero if success

		\see
	*/
	virtual oexBOOL Read( oexSTR8 x_pStr, oexUINT x_uMax );

	//==============================================================
	// Read()
	//==============================================================
	/// Reads a string from the buffer
	/**
		\param [in] x_uMax	-   Number of bytes to retrieve, zero for
                                all available.

		\return CStr object containing the data

		\see
	*/
    virtual CStr8 Read( oexUINT x_uMax = 0 )
    {   CStr8 str; Read( str, x_uMax ); return str; }

	//==============================================================
	// Read()
	//==============================================================
	/// Reads a string from the buffer
	/**
        \param [out] str    -   Receives the string
		\param [in] x_uMax	-   Number of bytes to retrieve, zero for
                                all available.

        This version is more efficient than the one that returns the
        string if you will be calling multiple times.  Calling this
        function multiple times with the same CStr object will reuse
        the previously allocated memory.

		\return CStr object containing the data

		\see
	*/
    virtual oexBOOL Read( CStr8 &x_sStr, oexUINT x_uMax = 0 );

	//==============================================================
	// Write()
	//==============================================================
	/// Write a string to the buffer
	/**
		\param [in] x_pStr	-	String value to write to buffer

		\return Non-zero if success.

		\see
	*/
	virtual oexBOOL Write( oexCSTR8 x_pStr )
	{	return Write( x_pStr, zstr::Length( x_pStr ) ); }

	//==============================================================
	// Write()
	//==============================================================
	/// Write a string to the buffer
	/**
		\param [in] x_sStr	-	String to write

		\return Non-zero if success.

		\see
	*/
	virtual oexBOOL Write( CStr8 x_sStr )
    {   return Write( x_sStr.Ptr(), x_sStr.Length() ); }


	//==============================================================
	// GetMaxWrite()
	//==============================================================
	/// Returns the maximum amount of data that can be written to the buffer
	/**
		\param [in] x_uReadPtr	-	Position of read pointer.
		\param [in] x_uWritePtr	-	Position of write pointer.
		\param [in] x_uMax		-	Total size of circular buffer

		\return Number of bytes that can be written

		\see
	*/
	static oexUINT GetMaxWrite( oexUINT x_uReadPtr, oexUINT x_uWritePtr, oexUINT x_uMax );

	//==============================================================
	// GetMaxWrite()
	//==============================================================
	/// Returns the maximum amount of data that can be written to the buffer
	/**
		\return Number of bytes that can be written
	*/
	oexUINT GetMaxWrite()
    {   if ( !m_pBi )
            return oexFALSE;
        return GetMaxWrite( m_pBi->uReadPtr, m_pBi->uWritePtr, m_uSize );
    }

	//==============================================================
	// GetMaxRead()
	//==============================================================
	/// Returns the maximum amount of data that can be read from the buffer
	/**
		\param [in] x_uReadPtr	-	Position of read pointer.
		\param [in] x_uWritePtr	-	Position of write pointer.
		\param [in] x_uMax		-	Total size of circular buffer

		\return Number of bytes that can be read

		\see
	*/
	static oexUINT GetMaxRead( oexUINT x_uReadPtr, oexUINT x_uWritePtr, oexUINT x_uMax );

	//==============================================================
	// GetMaxRead()
	//==============================================================
	/// Returns the maximum amount of data that can be read from the buffer
	/**
		\return Number of bytes that can be read
	*/
	oexUINT GetMaxRead()
    {   if ( !m_pBi )
            return oexFALSE;
        return GetMaxRead( m_pBi->uReadPtr, m_pBi->uWritePtr, m_uSize );
    }

	//==============================================================
	// Destroy()
	//==============================================================
	/// Releases all resources associated with the circular buffer.
	virtual void Destroy();

	//==============================================================
	// Write()
	//==============================================================
	/// Writes specified data to the circular buffer
	/**
		\param [in] x_pvBuf		-	Pointer to buffer containing data to be written.
		\param [in] x_uSize		-	Size of the buffer in pBuf.
		\param [in] x_puPtr		-	Write pointer position
		\param [in] x_uEncode	-	Optional encoding

		\return Non-zero on success.

		\see
	*/
	oexBOOL Write( oexCPVOID x_pvBuf, oexUINT x_uSize, oexUINT *x_puPtr, oexUINT x_uEncode = 0 );

	//==============================================================
	// Write()
	//==============================================================
	/// Writes specified data to the circular buffer
	/**
		\param [in] x_pvBuf		-	Pointer to buffer containing data to be written.
		\param [in] x_uSize		-	Size of the buffer in pBuf.
		\param [in] x_uEncode	-	Optional encoding

		\return Non-zero on success

		\see
	*/
	virtual oexBOOL Write( oexCPVOID x_pvBuf, oexUINT x_uSize, oexUINT x_uEncode = 0 )
	{
		// Sanity check
		if ( !oexCHECK_PTR( x_pvBuf ) || !x_uSize )
			return oexFALSE;

	    // Lock the buffer
	    oexAutoLock ll( &m_lock );
	    if ( !ll.IsLocked() )
            return oexFALSE;

        if ( m_bAutoGrow )
        {
            if ( !m_pBi )
                Allocate( x_uSize );
            else
                EnsureWriteSpace( x_uSize, m_pBi->uReadPtr, m_pBi->uWritePtr, m_uSize );

        } // end if

        if ( !m_pBi )
            return oexFALSE;

		if ( !Write( x_pvBuf, x_uSize, &m_pBi->uWritePtr, x_uEncode ) )
            return oexFALSE;

		return OnWrite();
	}

	//==============================================================
	// InitPoke()
	//==============================================================
	/// Used for revokable writing to buffer
	/**
		Used for putting data into the buffer without committing it.
		Usefull if you may fail while writing multi-part data.

		\see CancelPoke(), Poke(), EndPoke(), GetPokeSize()
	*/
	oexBOOL InitPoke()
    {
        if ( !m_pBi )
            return oexFALSE;

        m_uPokePtr = m_pBi->uWritePtr;

        return oexTRUE;
    }

	//==============================================================
	// CancelPoke()
	//==============================================================
	/// Cancels a poke operation
	/**
		\see InitPoke(), Poke(), EndPoke(), GetPokeSize()
	*/
	oexBOOL CancelPoke()
    {
        if ( !m_pBi )
            return oexFALSE;

        m_uPokePtr = m_pBi->uWritePtr;

        return oexTRUE;
    }

	//==============================================================
	// Poke()
	//==============================================================
	/// Pokes data into the buffer
	/**
		\param [in] x_pvBuf		-	Data to poke into write buffer
		\param [in] x_uSize		-	Size of pBuf
		\param [in] x_uEncode	-	Optional data encoding

		\return Non-zero if success

		\see InitPoke(), CancelPoke(), EndPoke(), GetPokeSize()
	*/
	oexBOOL Poke( oexCPVOID x_pvBuf, oexUINT x_uSize, oexUINT x_uEncode = 0 )
	{
        if ( m_bAutoGrow )
        {
            if ( !m_pBi )
                Allocate( x_uSize );
            else
                EnsureWriteSpace( x_uSize, m_pBi->uReadPtr, m_uPokePtr, m_uSize );

        } // end if

        if ( !m_pBi )
            return oexFALSE;

		return Write( x_pvBuf, x_uSize, &m_uPokePtr, x_uEncode );
	}

	//==============================================================
	// EndPoke()
	//==============================================================
	/// Committs the poked data to the buffer
	/**
		\return Non-zero if success

		\see InitPoke(), CancelPoke(), Poke(), GetPokeSize()
	*/
	oexBOOL EndPoke()
	{
        if ( !m_pBi )
            return oexFALSE;

        // Anything to commit?
        if ( m_pBi->uWritePtr != m_uPokePtr )
		{	m_pBi->uWritePtr = m_uPokePtr;
			return OnWrite();
		} // end if

		return oexFALSE;
	}

	//==============================================================
	// GetPokeSize()
	//==============================================================
	/// Returns the size of currently poked data
	/**
		\return Number of bytes currently poked

		\see InitPoke(), CancelPoke(), Poke(), EndPoke()
	*/
	oexUINT GetPokeSize()
	{
        if ( !m_pBi )
            return 0;
        return GetMaxRead( m_pBi->uReadPtr, m_uPokePtr, m_uSize );
    }

	//==============================================================
	// OnWrite()
	//==============================================================
	/// Called when data is written to the circular buffer
	/**
		\return Non-zero if success

		Over-ride this function to provide write data handling.

		\see
	*/
	virtual oexBOOL OnWrite()
    {   DataReady(); return oexTRUE; }

	//==============================================================
	// Read()
	//==============================================================
	/// Call to read and remove data from the circular buffer
	/**
		\param [out] x_pvBuf	-	Buffer to receive read data.
		\param [in] x_uMax		-	Size of buffer in pBuf
		\param [out] x_puRead	-	Receives number of bytes read
		\param [out] x_puPtr	-	Receives read pointer
		\param [in] x_uEncode	-	Optional decoding

		\return Non-zero if success, otherwise zero.

		\see
	*/
	oexBOOL Read( oexPVOID x_pvBuf, oexUINT x_uMax, oexUINT *x_puRead, oexUINT *x_puPtr, oexUINT x_uEncode = 0 );

	//==============================================================
	// Read()
	//==============================================================
	/// Call to read and remove data from the circular buffer
	/**
		\param [out] x_pvBuf	-	Buffer to receive read data.
		\param [in] x_uMax		-	Size of buffer in pBuf
		\param [out] x_puRead	-	Receives number of bytes read
		\param [in] x_uEncode	-	Optional decoding

		\return Non-zero if success, otherwise zero.

		\see
	*/
	virtual oexBOOL Read( oexPVOID x_pvBuf, oexUINT x_uMax, oexUINT *x_puRead, oexUINT x_uEncode = 0 );

	//==============================================================
	// Peek()
	//==============================================================
	/// Call to read data from the buffer without removing it
	/**
		\param [in] x_pvBuf		-	Buffer that receives peeked data
		\param [in] x_uMax		-	Size of the buffer in pBuf
		\param [in] x_puRead	-	Number of bytes read into pBuf
		\param [in] x_lOffset	-	Offset from read pointer to start
		\param [in] x_uEncode	-	Optional data decoding

		\return Non-zero if success.

		\see
	*/
	oexBOOL Peek( oexPVOID x_pvBuf, oexUINT x_uMax, oexUINT *x_puRead, oexLONG x_lOffset = 0, oexUINT x_uEncode = 0 );

    virtual oexBOOL Peek( CStr8 &x_sStr, oexUINT x_uMax = 0 );

    virtual CStr8 Peek( oexUINT x_uMax = 0 )
    {   CStr8 str; Peek( str, x_uMax ); return str; }

	//==============================================================
	// Allocate()
	//==============================================================
	/// Pre allocates the specified amount of data for the circular buffer
	/**
		\param [in] x_uSize		-	Number of bytes to allocate.

		\return Non-zero if success

		\see
	*/
	virtual oexBOOL Allocate( oexUINT x_uSize );

	//==============================================================
	// CCircBuf()
	//==============================================================
	/// Constructor
	/**
		\param [in] x_bSync		-	Non-zero to enable thread synchronization
		\param [in] x_uSize		-	Number of bytes to allocate.
		\param [in] x_bAutoGrow	-	Non-zero if buffer should auto-grow on writes.
	*/
	CCircBuf( oexBOOL x_bSync = oexTRUE, oexUINT x_uSize = 0, oexBOOL x_bAutoGrow = oexTRUE );

	/// Destructor
	virtual ~CCircBuf();

	//==============================================================
	// SetReadPtr()
	//==============================================================
	/// Sets the position of the read pointer
	/**
		\param [in] x_uPtr	-	New value for read pointer.
	*/
	oexBOOL SetReadPtr( oexUINT x_uPtr )
    {
        if ( !m_pBi )
            return oexFALSE;
        m_pBi->uReadPtr = x_uPtr;
        return oexTRUE;
    }

	//==============================================================
	// GetReadPtr()
	//==============================================================
	/// Returns the position of the read pointer
	/**
		\return read pointer position
	*/
	oexUINT GetReadPtr()
    {   if ( !m_pBi )
            return 0;
        return m_pBi->uReadPtr;
    }

	//==============================================================
	// SetWritePtr()
	//==============================================================
	/// Sets the position of the write pointer
	/**
		\param [in] x_uPtr	-	New value for write pointer.
	*/
	oexBOOL SetWritePtr( oexUINT x_uPtr )
    {   if ( !m_pBi )
            return oexFALSE;
        m_pBi->uWritePtr = x_uPtr;
        return oexTRUE;
    }

	//==============================================================
	// GetWritePtr()
	//==============================================================
	/// Returns the position of the write pointer
	/**
		\return write pointer position
	*/
	oexUINT GetWritePtr()
    {   if ( !m_pBi )
            return 0;
        return m_pBi->uWritePtr;
    }

	/// Returns a pointer for the buffers thread locking class
	operator oexLock&()
    {   return m_lock; }

	//==============================================================
	// SetMaxSize()
	//==============================================================
	/// Sets the maximum size the buffer can grow
	/**
		\param [in] dwMaxSize	-	Sets the maximum size for the circular buffer

		\warning This should really be set to something reasonable!

		\see SetAutoGrow(), SetAutoGrow(), SetMaxSize()
	*/
	void SetMaxSize( oexUINT x_uMaxSize )
    {   m_uMaxSize = x_uMaxSize; }

	//==============================================================
	// GetMaxSize()
	//==============================================================
	/// Returns the maximum size the buffer can grow
	/**
		\return Returns the maximum size for the circular buffer.

		\see GetAutoGrow(), SetAutoGrow(), SetMaxSize()
	*/
	oexUINT GetMaxSize()
    {   return m_uMaxSize; }

	//==============================================================
	// SetAutoGrow()
	//==============================================================
	/// Enables / disables the auto resizing feature of CCircBuf
	/**
		\param [in] b	-	Non-zero to enable resizing, zero to disable.

		Sets auto grow enable / disable. If auto grow is enabled, the buffer will
		automatically resized to accomidate any write less than m_dwMaxSize.

		\see GetAutoGrow(), SetMaxSize(), GetMaxSize()
	*/
	oexBOOL SetAutoGrow( oexBOOL b )
    {
        // Can't resize shared memory
        if ( m_memBuffer.IsMapped() )
            m_bAutoGrow = oexFALSE;

        else
            m_bAutoGrow = b;

        return m_bAutoGrow;
    }

	//==============================================================
	// GetAutoGrow()
	//==============================================================
	/// Returns the auto grow status
	/**
		\return Non-zero if auto-grow is enabled.

		\see SetAutoGrow(), SetMaxSize(), GetMaxSize()
	*/
	oexBOOL GetAutoGrow()
    {   return m_bAutoGrow; }

	//==============================================================
	// IsEmpty()
	//==============================================================
	/// Provides a quick way to check for read data availability
	/**
		\return Non-zero if data can be read from the buffer.
	*/
	oexBOOL IsEmpty()
    {   return m_bEmpty; }

	//==============================================================
	// DataReady()
	//==============================================================
	/// Call this function to signal that data is ready to be read.
	void DataReady()
	{	m_bEmpty = oexFALSE;
        m_evDataReady.Signal();
        m_evEmpty.Reset();
	}

	//==============================================================
	// GetDataReadyHandle()
	//==============================================================
	/// Returns the data ready event handle
    oexEvent& GetDataReadyHandle()
    {   return m_evDataReady; }

	//==============================================================
	// GetEmptyHandle()
	//==============================================================
	/// Returns the buffer empty event handle
    oexEvent& GetEmptyHandle()
    {   return m_evEmpty; }

	//==============================================================
	// Empty()
	//==============================================================
	/// Empties the circular buffer
	/**
		\return Non-zero if success.
	*/
	oexBOOL Empty()
	{	oexAutoLock ll( &m_lock );
		if ( !ll.IsLocked() )
            return oexFALSE;

        if ( !m_pBi )
            return oexTRUE;

		m_bEmpty = oexTRUE;
		m_pBi->uWritePtr = m_pBi->uReadPtr = 0;

        m_evDataReady.Reset();
        m_evEmpty.Signal();

		return oexTRUE;
	}

	//==============================================================
	// OnInspectRead()
	//==============================================================
	/// Over-ride to implement read block inspection
	/**
		\param [in] x_uBlock	-	Index of block
		\param [in] x_pucBuf	-	Pointer to data
		\param [in] x_uSize		-	Size of buffer in pBuf

		\return Non-zero if success

		\see
	*/
	virtual oexBOOL OnInspectRead( oexUINT x_uBlock, oexUCHAR *x_pucBuf, oexUINT x_uSize )
    {   return oexTRUE; }

	//==============================================================
	// OnInspectWrite()
	//==============================================================
	/// Over-ride to implement write block inspection
	/**
		\param [in] x_uBlock	-	Index of block
		\param [in] x_pucBuf	-	Pointer to data
		\param [in] x_uSize		-	Size of buffer in pBuf

		\return Non-zero if success

		\see
	*/
	virtual oexBOOL OnInspectWrite( oexUINT x_uBlock, oexUCHAR *x_pucBuf, oexUINT x_uSize )
    {   return oexTRUE; }

	/// Over-ride this function to enable generic block encoding
	//==============================================================
	// OnEncode()
	//==============================================================
	///
	/**
		\param [in] x_uType		-	Type of encoding to perform
		\param [in] x_uBlock	-	Index of block
		\param [in] x_pucBuf	-	Pointer to data
		\param [in] x_uSize		-	Size of buffer in pBuf

		\return Non-zero if success

		\see
	*/
	virtual oexBOOL OnEncode( oexUINT x_uType, oexUINT x_uBlock, oexUCHAR *x_pucBuf, oexUINT x_uSize )
    {   return oexTRUE; }

	/// Over-ride this function to enable generic block decoding
	//==============================================================
	// OnDecode()
	//==============================================================
	///
	/**
		\param [in] x_uType		-	Type of encoding to perform
		\param [in] x_uBlock	-	Index of block
		\param [in] x_pucBuf	-	Pointer to data
		\param [in] x_uSize		-	Size of buffer in pBuf

		\return Non-zero if success

		\see
	*/
	virtual oexBOOL OnDecode( oexUINT x_uType, oexUINT x_uBlock, oexUCHAR *x_pucBuf, oexUINT x_uSize )
    {   return oexTRUE; }

	//==============================================================
	// GetBufferSize()
	//==============================================================
	/// Returns the size of the buffer
	/**
		\return	Total size of the circular buffer
	*/
	oexUINT GetBufferSize()
    {   return m_uSize; }

private:

	/// Thread lock
	oexLock					m_lock;

	/// Data ready event
	oexEvent				m_evDataReady;

	/// Data ready event
    oexEvent				m_evEmpty;

	/// Non-zero if buffer is empty
	oexBOOL					m_bEmpty;

	/// Set to non-zero to allow the buffer to grow during write operations
	oexBOOL					m_bAutoGrow;

	/// Sets the maximum size for the circular buffer
	oexUINT					m_uMaxSize;

    /// Current size of circular buffer
    oexUINT                 m_uSize;

    /// Pointer to the start of the buffer
    oexUCHAR                *m_pBuf;

    /// Buffer information
    SBufferInfo             *m_pBi;

	/// Position of data that is being poked into the buffer
	oexUINT					m_uPokePtr;

	/// Encapsulates the actual circular buffer memory
	TMem< oexUCHAR >		m_memBuffer;

};

