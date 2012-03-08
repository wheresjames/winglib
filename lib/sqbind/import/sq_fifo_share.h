/*------------------------------------------------------------------
// sq_fifo_share.h
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

namespace sqbind
{
    class CSqFifoShare
    {
    
    public:
    
    	/// Buffer pointer information
    	struct SPtrInfo
    	{
    		/// Offset to start of data
    		oex::oexUINT32		uPtr;
    		
    		/// Header size
    		oex::oexUINT32		uHeader;
    		
    		/// Data size
    		oex::oexUINT32		uData;
    		
    		/// User value
    		oex::oexUINT32		uUser;
    		
    		/// Timestamp
    		oex::oexUINT64		uTs;
    	};
    
	public:

		/// Export constructors
		SQBIND_CLASS_CTOR_BEGIN( CSqFifoShare )
			_SQBIND_CLASS_CTOR2( CSqFifoShare, OT_STRING, OT_INTEGER ) 
				( stdString( sa.GetString( 2 ), sq_getsize( x_v, 2 ) ), sa.GetInt( 3 ), 1 )
			_SQBIND_CLASS_CTOR3( CSqFifoShare, OT_STRING, OT_INTEGER, OT_INTEGER ) 
				( stdString( sa.GetString( 2 ), sq_getsize( x_v, 2 ) ), sa.GetInt( 3 ), sa.GetInt( 4 ) )
		SQBIND_CLASS_CTOR_END( CSqFifoShare )

		/// Default constructor
		CSqFifoShare();

		/// Init constructor
		CSqFifoShare( const sqbind::stdString &sPrefix, SQInteger nId, SQInteger nGlobal );

		/// Copy constructor
		CSqFifoShare( const CSqFifoShare &r );

		/// Assignment operator
		CSqFifoShare& operator = ( const CSqFifoShare &r ) { return *this; }

		/// Registers the class
		static void Register( sqbind::VM vm );

		/** \addtogroup CSqFifoShare
			@{
		*/

		/// Releases the share and all associated resources
		void Destroy();

		/// Creates a writable share
		/**
			@param [in] sName		-	Share name
			@param [in] sBufName	-	Share name for the image buffer.  Leave blank if same as sName.
			@param [in] nBufSize	-	Size of buffer
			@param [in] nBlocks		-	Number of blocks
			@param [in] sHeader		-	User information

			Opens a new writable share.  This function fails if the share already exists.

			@return Non-zero if success

		*/
		int Create( const sqbind::stdString &sName, const sqbind::stdString &sBufName, int nBufSize, int nBlocks, const sqbind::stdString &sHeader );

		/// Opens an existing share
		/**
			@param [in] sName	-	Share name

			Opens an existing share.  The share must exist.

			@return Non-zero if success
		*/
		int Open( const sqbind::stdString &sName );

		/// Returns non zero if there is data to be read
		int isRead();

		/// Reads the current block header
		sqbind::stdString ReadHeader();

		/// Reads next data block
		sqbind::CSqBinary ReadData();

		/// Reads next user data
		int ReadUser();

		/// Reads next timestamp
		SQInteger ReadTs();

		/// Returns the minimum timestamp in the buffer
		SQInteger ReadTsMin()
		{	SQInteger t = 0; CalculateTsRange( &t, 0 ); return t; }

		/// Returns the minimum timestamp in the buffer
		SQInteger ReadTsMax()
		{	SQInteger t = 0; CalculateTsRange( 0, &t ); return t; }

		/// Calculates the timestamp range
		SQInteger CalculateTsRange( SQInteger *pMin, SQInteger *pMax );

		/// Increments the read pointer
		int incReadPtr();

		/// Decrements the read pointer
		int decReadPtr();

		/// Writes data into the share buffer
		/**
			@param [in] pData	-	Pointer to CSqBinary object containing the data to write
			@param [in] sHeader	-	Header data
			@param [in] nUser	-	User integer
			@param [in] ts		-	Timestamp
		*/
		int Write( sqbind::CSqBinary *pData, const sqbind::stdString &sHeader, int nUser, SQInteger ts );

		/// Resets buffer pointers
		int Reset();

		/// Cancels the buffer, i.e. signals everyone to disconnect
		int Cancel();

		/// Return the share name
		sqbind::stdString getName() { return m_sName; }
		
		/// Sets the prefix string
		void setPrefix( const sqbind::stdString &s ) { m_sPrefix = s; }
		
		/// Returns the current prefix string
		sqbind::stdString getPrefix() { return m_sPrefix; }
		
		/// Sets the control block id
		void setCbId( oex::oexUINT id ) { m_uCbId = id; }
		
		/// Gets the control block id
		oex::oexUINT getCbId() { return m_uCbId; }
				
		/// Set to non-zero to enable global access to share
		void setGlobal( int b ) { m_bGlobal = b; }
		
		/// Returns non-zero if global access to share is enabled
		int getGlobal() { return m_bGlobal; }

		/// Sets the amount of padding to add to the end of the buffer
		/**
			Many codecs read over the end of the buffer, and so padding is required.
		*/
		void setPadding( int n ) { m_nPadding = n; }

		/// Returns the amount of padding added to the end of the buffer
		/**
			Many codecs read over the end of the buffer, and so padding is required.
		*/
		int getPadding() { return m_nPadding; }

		/// Returns the global header string
		sqbind::stdString getHeader();		

		/// Returns the control block size of the buffer from the control block
		int getWritePtr()
		{	if ( !m_cb.getUsed() ) 
				return 0;
			return m_cb.getINT( 1 );
		}

		/// Returns the control block size of the buffer from the control block
		int getCbSize()
		{	if ( !m_cb.getUsed() ) 
				return 0;
			return m_cb.getINT( 2 );
		}

		/// Returns the timestamp from the control block
		int getTs()
		{	if ( !m_cb.getUsed() ) 
				return 0;
			return m_cb.getINT( 3 );
		}

		/// Returns the process id from the control block
		int getProcId()
		{	if ( !m_cb.getUsed() ) 
				return 0;
			return m_cb.getINT( 4 );
		}

		/// Returns the total size of the buffer from the control block
		int getBufSize()
		{	if ( !m_cb.getUsed() ) 
				return 0;
			return m_cb.getINT( 5 );
		}
		
		/// Returns the number of blocks from the control block
		int getBlocks()
		{	if ( !m_cb.getUsed() ) 
				return 0;
			return m_cb.getINT( 6 );
		}

		/// Returns the header size from the control block
		int getHeaderSize()
		{	if ( !m_cb.getUsed() ) 
				return 0;
			return m_cb.getINT( 7 );
		}

		/// Returns the number of writes from the control block
		int getWrites()
		{	if ( !m_cb.getUsed() ) 
				return 0;
			return m_cb.getINT( 8 );
		}

		/// Sets the number of writes
		void setWrites( int n )
		{	if ( !m_cb.getUsed() ) 
				return;
			return m_cb.setINT( 8, n );
		}
		
		/// Returns the number of reads from the control block
		int getReads()
		{	if ( !m_cb.getUsed() ) 
				return 0;
			return m_cb.getINT( 9 );
		}

		/// Sets the number of reads
		void setReads( int n )
		{	if ( !m_cb.getUsed() ) 
				return;
			return m_cb.setINT( 9, n );
		}

		/// Set to non-zero to tell the writer to reset
		void setReset( int b )
		{	if ( !m_cb.getUsed() ) 
				return;
			m_cb.setINT( 10, b );
		}

		/// Returns the current reset value
		int getReset()
		{	if ( !m_cb.getUsed() ) 
				return 0;
			return m_cb.getINT( 10 );
		}

		/// Returns the header size from the control block
		int getBufferGuid( sqbind::CSqBinary *p )
		{
			if ( !m_cb.getUsed() ) 
				return 0;
			
			if ( 16 != sizeof( oex::oexGUID ) )
				return 0;
			
			// Ensure there is space for the guid
			if ( p->Size() < sizeof( oex::oexGUID ) && !p->Allocate( sizeof( oex::oexGUID ) ) )
				return 0;

			// Copy guid
			p->setUINT( 0, m_cb.getUINT( 12 ) );
			p->setUINT( 1, m_cb.getUINT( 13 ) );
			p->setUINT( 2, m_cb.getUINT( 14 ) );
			p->setUINT( 3, m_cb.getUINT( 15 ) );
			p->setUsed( sizeof( oex::oexGUID ) );
			
			return sizeof( oex::oexGUID );
		}
		
		/// Copy's the guid from the control block
		sqbind::stdString getBufferGuidStr()
		{
			if ( !m_cb.getUsed() ) 
				return oexT( "" );
			
			if ( 16 != sizeof( oex::oexGUID ) )
				return oexT( "" );

			// Copy guid
			oex::oexGUID guid;
			guid.u1 = m_cb.getUINT( 12 );
			guid.u2 = m_cb.getUINT( 13 );
			guid.u3 = m_cb.getUINT( 14 );
			guid.u4 = m_cb.getUINT( 15 );

			return sqbind::oex2std( oex::CStr( guid ) );
		}

		/// Returns a string describing the last error
		sqbind::stdString getLastErrorStr() { return m_sLastErr; }

		/// Sets the last error string
		void setLastErrorStr( const sqbind::stdString &sErr ) { m_sLastErr = sErr; }

		/// Returns non-zero if a share is open
		int isOpen() { return ( m_cb.getUsed() && m_buf.getUsed() ); }

		/// Returns non-zero if a valid share is open
		int isValid();

		/** @} */

public:

		/// Writes raw data into the buffer
		/**
			@param [in] pData	-	Pointer to buffer
			@param [in] nSize	-	Number of bytes to be written
			@param [in] sHeader	-	Header data
			@param [in] nUser	-	User integer
			@param [in] ts		-	Timestamp
		*/
		int WritePtr( const void *pData, int nSize, const sqbind::stdString &sHeader, int nUser, SQInteger ts );
	
		/// Returns the current read pointer structure, or NULL if none
		SPtrInfo* ReadPtr();
		
protected:

		/// Copy's the guid from the control block
		oex::oexGUID* CopyGuid( oex::oexGUID *p )
		{
			if ( !m_cb.getUsed() || !p ) 
				return 0;
			
			if ( 16 != sizeof( oex::oexGUID ) )
				return 0;

			// Copy guid
			p->u1 = m_cb.getUINT( 14 );
			p->u2 = m_cb.getUINT( 15 );
			p->u3 = m_cb.getUINT( 16 );
			p->u4 = m_cb.getUINT( 17 );

			return p;
		}

	private:

		/// Non-zero for global share
		int						m_bGlobal;

		/// Non-zero if the share is open for writing
		int						m_bWrite;

		/// Non-zero to allow frame skipping
		int						m_bAllowFrameSkipping;

		/// Padding added to the end of the allocated buffer
		int						m_nPadding;

		/// Time the stream was opened
		oex::oexUINT			m_uTs;

		/// Control block id
		oex::oexUINT			m_uCbId;

		/// The share name
		sqbind::stdString		m_sName;

		/// Share prefix
		sqbind::stdString		m_sPrefix;

		/// The shared control block
		CSqBinary				m_cb;

		/// The shared buffer
		CSqBinary				m_buf;

		/// String describing the last error
		sqbind::stdString		m_sLastErr;

		/// Current read index
		int						m_iRead;
		
		/// Current write index
		int						m_iWrite;
	};

}; // end namespace

// Declare type for use as squirrel parameters
SQBIND_DECLARE_INSTANCE( sqbind::CSqFifoShare, CSqFifoShare )
