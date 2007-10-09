/*------------------------------------------------------------------
// alloc.cpp
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
// CCrcHash
//
/// Calculates a crc hash
/**
	To qualify as a hashing function for oex::TAssoList<>, the class
	must contain the functions Hash() and Size().	
*/
//==================================================================
class CCrcHash
{
public:

	/// Default constructor
	CCrcHash() {}

	/// Destructor
	virtual ~CCrcHash() {}

public:

	//==============================================================
	// Hash()
	//==============================================================
	/// Calculates a CRC for the specified memory buffer
	/**
        \param [in] x_pHash     -   Buffer that receives the hash
		\param [in] x_pBuf		-	Pointer to the data to be CRC'd
		\param [in] x_uSize		-	Length of data in x_pBuf
		
		\return Non-zero if success
	
		\see 
	*/
	static oexBOOL Hash( oexPVOID x_pHash, oexCPVOID x_pBuf, oexUINT x_uSize )
	{   oexASSERT_PTR( x_pHash );
        *( (oexUINT*)x_pHash ) = CCrcHash::CRC32( 0, (oexUCHAR*)x_pBuf, x_uSize ); 
        return oexTRUE;
    }

    enum 
    {
        /// The size of the hash
        eHashSize = sizeof( oexUINT ) 
    };

private:

	//==============================================================
	// CCrcHash::CRC32()
	//==============================================================
	/// Implementation of 
	/**
		\param [in] x_crc	-	The initial CRC value
		\param [in] x_buf	-	Buffer containing data to CRC
		\param [in] x_size	-	Number of bytes in x_buf
		
		\return 32 bit CRC value of the data
	
		\see 
	*/
	static oexUINT CCrcHash::CRC32( oexUINT x_crc, oexUCHAR *x_buf, oexUINT x_size );

	/// 32 bit CRC table, this speeds up the CRC algorithm
	static oexUINT CCrcHash::m_crc_table[ 256 ];
};


//==================================================================
// TAssoListNode
//
/// Specialized list node for TAssoList<>
/**
	This class just adds memory to hold the key value for 
	TAssoList<>.
*/
//==================================================================
template < class T_OBJ, class T_KEY > class TAssoListNode : public TListNode< T_OBJ >
{
public:

    /// Default constructor
	TAssoListNode()
	{}

	/// I think I should be able to choose to automatically 
	/// inherit constructor arguments
	TAssoListNode( TAssoListNode *x_pIt, oexINT x_nOpt )
		: TListNode< T_OBJ >( x_pIt, x_nOpt )
	{}

	/// Ditto on the wish list above
	TAssoListNode( T_OBJ *x_pObj, TAssoListNode *x_pIt, oexINT x_nOpt )
		: TListNode< T_OBJ >( x_pObj, x_pIt, x_nOpt )
	{}

	/// Key value for this list node
	T_KEY				key;
};

//==================================================================
// TAssoList
//
/// Associated list
/**
	This class defines the functionality of an associated list.
	By default, keys are hashed using a CRC, this narrows the
	search time down to ( n / 2^h ).  This class does *not* assume
	there are no hash collisions.  The hash is used to reduce the 
	nodes that must be compared with the key down by 2^h where h 
	is the number of bits in the hash.

	The default size is 8 bits or 256 elements in the table.  
	Therefore there should be no collisions up to 256 elements
	using the default size.

	If you intend to store lots of data in the list, you should
	call CreateTable() or the constructor with an appropriate size.

	You can create your own hashing class and functions by supplying
	a Hash() and the enum eHashSize function with the following signatures.
	Currently, sizes over 32 bits are ignored since there would be
	no way to keep a table of such a size in memory.

  \code

    class CMyHashClass
    {
    public:

	    /// Called to get the hash of a buffer
	    oexBOOL Hash( oexPVOID pHash, oexUCHAR *x_pBuf, oexUINT x_uSize );

        enum 
        {
            /// The size of the hash
            eHashSize = sizeof( oexUINT ) 
        };

    };

  \endcode

  <b>Example uses:</b>

  \code

		TAssoList< oexINT, oexINT > alii;

		alii[ 4 ] = 11;
		alii[ 5000 ] = 22;
		alii[ 600000 ] = 33;

		if ( !oexVERIFY( 3 == alii.Size() ) )
			return -1;

		if (	!oexVERIFY( 11 == alii[4 ] ) 
				|| !oexVERIFY( 22 == alii[ 5000 ] )
				|| !oexVERIFY( 33 == alii[ 600000 ] ) )
			return -2;

  \endcode

  \code

		TAssoList< CStr, oexINT > alsi;

		alsi[ "Idx 1" ] = 11;
		alsi[ "Idx 2" ] = 22;
		alsi[ "Idx 3" ] = 33;

		if ( !oexVERIFY( 3 == alsi.Size() ) )
			return -3;

		if (	!oexVERIFY( 11 == alsi[ "Idx 1" ] ) 
				|| !oexVERIFY( 22 == alsi[ "Idx 2" ] )
				|| !oexVERIFY( 33 == alsi[ "Idx 3" ] ) )
			return -4;

  \endcode

  \code

		TAssoList< CStr, CStr > alss;

		alss[ "Idx 1" ] = "11";
		alss[ "Idx 2" ] = "22";
		alss[ "Idx 3" ] = 33;
		alss[ "Idx 4" ] = 3.141f;

		if ( !oexVERIFY( CParser::Implode( alss.Copy(), "," ) == "11,22,33,3.141000" ) )
			return -5;

		TAssoList< CStr, CStr >::iterator itSs;
		if ( !oexVERIFY( ( itSs = alss.SearchKey( "2", CStr::CmpMatch ) ).IsValid() ) 
			 || !oexVERIFY( *itSs == "22" ) )
			return -6;

		if ( !oexVERIFY( 4 == alss.Size() ) )
			return -7;

		if (	!oexVERIFY( alss[ "Idx 1" ] == "11" ) 
				|| !oexVERIFY( alss[ "Idx 2" ] == "22" )
				|| !oexVERIFY( alss[ "Idx 3" ].ToLong() == 33 )
				|| !oexVERIFY( alss[ "Idx 4" ].ToDouble() > 3 )
				|| !oexVERIFY( alss[ "Idx 4" ].ToDouble() < 4 ) )
			return -8;

  \endcode

*/
//==================================================================
template < class T_KEY, class T_OBJ = T_KEY, class T_HASH = CCrcHash, class T_NODE = TAssoListNode< T_OBJ, T_KEY > >
	class TAssoList : public TList< T_OBJ, T_NODE >
{
public:

	/// The type of iterator for this list
	typedef typename TList< T_OBJ, T_NODE >::iterator iterator;

public:

	/// Default constructor
	TAssoList() 
	{	m_uTableSize = 0;
		m_uTableBits = 0;
		m_uTableMask = 0;
	}

	//==============================================================
	// TAssoList()
	//==============================================================
	/// Constructs the object and initialize the hash table
	/**
		\param [in] x_uTableSize	-	Desired hash table size.
		
		The default size is 8 bits or 256 elements in the table.  
		Therefore there should be no collisions up to 256 elements
		using the default size.

		If you intend to store lots of data in the list, you should
		call CreateTable() or the constructor with an appropriate size.

	*/
	TAssoList( oexUINT x_uTableSize ) 
	{	m_uTableSize = 0;
		m_uTableBits = 0;
		m_uTableMask = 0;
		CreateTable( x_uTableSize );
	}

	//==============================================================
	// TAssoList()
	//==============================================================
	/// Constructs and initializes the list from a reference
	/**
		\param [in] x_rList	-	The list to initialize with.								

		Note that this class just assumes the list.  A copy is not 
		made.  After this function, rList will be empty.  

		\code

			TAssoList< oexINT, oexINT > lst1;

			lst1[ 1 ] = 11;
			lst1[ 2 ] = 22;

			TAssoList< oexINT, oexINT > lst2( lst1 );

			// lst2 now contains the list, lst1 is empty

			// If you want to make a copy, use...

			TAssoList< oexINT, oexINT > lst2( lst1.Copy() );

		\endcode

	*/
	TAssoList( TAssoList &x_rList )
	{	m_uTableSize = 0;
		m_uTableBits = 0;
		m_uTableMask = 0;
		Attach( x_rList ); 
	}

	
	/// Destructor
	virtual ~TAssoList() { Destroy(); }

	//==============================================================
	// Destroy()
	//==============================================================
	/// Destroys the list.
	/**
		This function destroys the list and readys it for reuse.

		Note that list item iterators are reference counted.  This
		means any iterators left outside the list and the objects
		in them are still valid until those references are released.

		\code

			TAssoList< oexINT, oexINT > lst;

			lst[ 1 ] = 11;
			lst[ 2 ] = 22;

			TAssoList< oexINT, oexINT >::iterator it = lst.Get( 1 );

			lst.Destroy();

			// lst is empty but it is still valid

			it.Destroy();

			// And now it's gone
		
	*/
	virtual void Destroy()
	{	m_uTableBits = 0;
		m_uTableMask = 0;
		m_uTableSize = 0;
		m_table.Delete();
		TList< T_OBJ, T_NODE >::Destroy();
	}

	//==============================================================
	// CreateTable()
	//==============================================================
	/// Creates the hash table
	/**
		\param [in] x_uBits	-	Desired hash table size.
		
		The default size is 8 bits or 256 elements in the table.  
		Therefore there should be no collisions up to 256 elements
		using the default size.

		If you intend to store lots of data in the list, you should
		call CreateTable() or the constructor with an appropriate size.

		+++ It would be real nice if this would create a new table
		without destroying the old one.  Then we could have an
		'automatic' default.

	*/
	oexBOOL CreateTable( oexUINT x_uBits = 8 )
	{
		Destroy();

		if ( 2 > x_uBits ) 
			x_uBits = 2;
		else if ( 32 < x_uBits )
			x_uBits = 32;

		// Calculate table metrics
		m_uTableBits = x_uBits;
		m_uTableMask = 0xffffffff, m_uTableMask >>= ( 32 - m_uTableBits );
		m_uTableSize = 1, m_uTableSize <<= m_uTableBits;

		// Create the table
		if ( !oexVERIFY_PTR( m_table.OexConstructArray( m_uTableSize ).Ptr() ) )
			return oexFALSE;

		return oexTRUE;
	}
	
	//==============================================================
	// GetIndex()
	//==============================================================
	/// Returns the hash table index for the specified key
	/**
		\param [in] x_key		-	Key
		
		This function creates the hash table if it doesn't exist.

		The table index is calculated by hashing the key, then
		masking the hash with the table size.

		\return The hash table index for the specified key.
	
		\see 
	*/
	oexUINT GetIndex( oexCONST T_KEY &x_key )
	{
		if ( !m_uTableSize )
			if ( !CreateTable() )
				return 0;

        // Hash buffer
        oexUCHAR ucHash[ T_HASH::eHashSize + sizeof( oexUINT ) ];

		// Calculate the hash
        if ( !oexVERIFY( m_hash.Hash( ucHash, obj::Ptr( &x_key ), obj::Size( &x_key ) ) ) )
            return 0;

        // +++ Change this, it depends on the processor being little endian

		// Calculate index
		oexUINT uIndex = m_uTableMask & *( (oexUINT*)ucHash );
		oexASSERT( uIndex < m_uTableSize );

		return uIndex;
	}

	//==============================================================
	// Get()
	//==============================================================
	/// Returns the iterator for the specified key
	/**
		\param [in] x_key	-	Key value
		
		\return iterator for the value associated with the key.
	
		\see 
	*/
	iterator Get( oexCONST T_KEY &x_key )
	{
		oexUINT i = GetIndex( x_key );

		// Search sub list for an exact match
		for ( TList< iterator >::iterator it; m_table.Ptr( i )->Next( it ); )
            if ( obj::Compare( it->Node()->key, x_key ) )
				return it->Node();

		// Add container to list
		TList< iterator >::iterator it = m_table.Ptr( i )->Append();
		
		// Create object
		*it = Append();

		// We must get a node
		oexASSERT( it->Node() );

		// Set the key
		it->Node()->key = x_key;

        return it->Node();
	}

	//==============================================================
	// Find()
	//==============================================================
	/// Returns the iterator for the key if it exists
	/**
		\param [in] x_key		-	Key value
		
		You can check for an empty iterator with

		\code
		
			TAssoList< oexINT, oexINT >::iterator it = lst.Find( 1 );

			if ( it.IsValid() )
				// Use the iterator			

		\endcode

		\return A valid iterator if the key exists, if the 
				key does not exist in the list, an empty iterator
				is returned.
	
		\see 
	*/
	iterator Find( oexCONST T_KEY &x_key )
	{
		if ( !Size() )
			return iterator();

		oexUINT i = GetIndex( x_key );

		// Search sub list for an exact match
		for ( TList< iterator >::iterator it; m_table.Ptr( i )->Next( it ); )
            if ( obj::Compare( it->Node()->key, x_key ) )     			     
				return it->Node();

		return iterator();
	}

	//==============================================================
	// SearchKey()
	//==============================================================
	/// Searches for the specified key by comparing each item
	/**
		\param [in] x_tObj	-	Item for comparison

		This function searches every list item, comparing each one
		to x_tObj until a match is found

		Can we say sloooooow!

		In general you should use Find(), Get(), or [].  There are
		special circumstances for this function.
		
		\return iterator to matching object or empty iterator.
	
		\see 
	*/
	template < class T_CMPOBJ >
		iterator SearchKey( oexCONST T_CMPOBJ x_tObj )
		{	for ( iterator it; Next( it ); )
				if ( it.Node()->key == x_tObj )
					return it;
			return iterator();
		}

	//==============================================================
	// SearchKey()
	//==============================================================
	/// Searches for the specified key by comparing each item
	/**
		\param [in] x_tObj	-	Item for comparison
		\param [in] x_fCmp	-	Function for comparison

		This function searches every list item, comparing each one
		to x_tObj using the specified function x_fCmp() until a match 
		is found.

		Obviously, this is slow, but if we must we must.

		In general you should use Find(), Get(), or [].  There are
		special circumstances for this function.

		\code

			// Compare function
			oexBOOL MyCompare( oexINT a, oexINT b )
			{	return a == b; 
			}

			// ...

			// Find item using custom compare function
			TAssoList< oexINT, oexINT > it = lst.SearchKey( 2, MyCompare );

		\endcode
		
		\return iterator to matching object or empty iterator.
	
		\see 
	*/
	template< class T_CMPOBJ, class T_CMPFUNC >
		iterator SearchKey( oexCONST T_CMPOBJ x_tObj, T_CMPFUNC x_fCmp )
		{
            for ( iterator it; Next( it ); )
    			if ( 0 <= x_fCmp( it.Node()->key, x_tObj ) )
					return it;

			return iterator();
		}

	//==============================================================
	// IsKey()
	//==============================================================
	/// Returns non-zero if the specified key is valid in the list
	/**
		\param [in] x_key	-	Key value
					
	*/
	oexBOOL IsKey( T_KEY x_key )
	{
		if ( !Size() )
			return oexFALSE;

		oexUINT i = GetIndex( x_key );

		// Search sub list for an exact match
		for ( TList< iterator >::iterator it; m_table.Ptr( i )->Next( it ); )
			if ( it->Node()->key == x_key )
				return oexTRUE;

		return oexFALSE;
	}

	//==============================================================
	// Unset()
	//==============================================================
	/// Unset the specified iterator from the list
	/**
		\param [in] x_key		-	iterator to erase
		\param [in] x_bForward	-	Non-zero to return the previous
									list item, zero to return the
									next list item.

		Use this function to remove an item from the list.

		As a convinience, the previous or next item can be returned.
	
		\return 
	
		\see 
	*/
	iterator Unset( oexCONST T_KEY x_key, oexBOOL x_bForward = oexTRUE )
    {   iterator it = Find( x_key );
        if ( !it.IsValid() )
            return it;
        return TList< T_OBJ, T_NODE >::Erase( it, x_bForward );
    }

protected:

	//==============================================================
	// Unlink()
	//==============================================================
    /// Use this function to unlink a list item when it is erased
    /**
        This unlinks items from the list when they are deleted.
        Ultimately, any item that is deleted from the list, MUST
        be done so by calling TList::Erase().
    */
    virtual void Unlink( iterator x_it ) 
    {   
		if ( !Size() || !x_it.Node() )
			return;

        // Get the index
		oexUINT i = GetIndex( x_it.Node()->key );

		// Search sub list for an exact match
		for ( TList< iterator >::iterator it; m_table.Ptr( i ) && m_table.Ptr( i )->Next( it ); )
            if ( obj::Compare( it->Node()->key, x_it.Node()->key ) )
            {   m_table.Ptr( i )->Erase( it );
                return; 
            } // end if
    }

public:

	//==============================================================
	// []()
	//==============================================================
	/// Returns a reference to the object at the specified key
	/**
		\param [in] x_key		-	Key value
		
		\return Reference to object
	
		\see Get(), Find()
	*/
	T_OBJ& operator []( T_KEY x_key )
	{	return *Get( x_key ); }	

	//==============================================================
	// operator =
	//==============================================================
	/// Assumes the list from a reference
	/**
		\param [in] x_rList	-	The list to initialize with.								

		Note that this class just assumes the list.  A copy is not 
		made.  After this function, x_rList will be empty.  

		\code

			TAssoList< oexINT, oexINT > lst1;

			lst1[ 1 ] = 11;
			lst1[ 2 ] = 22;

			TAssoList< oexINT, oexINT > lst2( lst1 );

			// lst2 now contains the list, lst1 is empty

			// If you want to make a copy, use...

			TAssoList< oexINT, oexINT > lst2( lst1.Copy() );

		\endcode

		\return Reference to this object

		\see Attach()

	*/
	TAssoList& operator = ( TAssoList &x_rList )
	{	return Attach( x_rList ); }

	//==============================================================
	// Attach()
	//==============================================================
	/// Attaches to the specified list
	/**
		\param [in] x_lst	-	List to attach to
		
		Note that this class just assumes the list.  A copy is not 
		made.  After this function, x_lst will be empty.  

		\code

			TAssoList< oexINT, oexINT > lst1;

			lst1[ 1 ] = 11;
			lst1[ 2 ] = 22;

			TAssoList< oexINT, oexINT > lst2( lst1 );

			// lst2 now contains the list, lst1 is empty

			// If you want to make a copy, use...

			TAssoList< oexINT, oexINT > lst2( lst1.Copy() );

		\endcode

		\return Reference to this object
	
		\see operator =
	*/
	TAssoList& Attach( TAssoList &x_lst )
	{
		// Lose our current list
		Destroy();

		// Punt if empty list
		if ( !x_lst.Size() )
			return *this;

		// First get the list
		TList< T_OBJ, T_NODE >::Attach( (TList< T_OBJ, T_NODE >&)x_lst );

		// Grab the indexing metrics
		m_uTableSize = x_lst.m_uTableSize;
		m_uTableMask = x_lst.m_uTableMask;
		m_uTableBits = x_lst.m_uTableBits;
		m_table.Assume( x_lst.m_table );

		// Disconnect the list
		x_lst.m_uTableSize = 0;
		x_lst.m_uTableMask = 0;
		x_lst.m_uTableBits = 0;		

		return *this;
	}

	//==============================================================
	// Detach()
	//==============================================================
	/// Detaches from the list items without releasing them
	virtual void Detach()
	{
		TList< T_OBJ, T_NODE >::Detach();

		// Disconnect the list
		m_uTableSize = 0;
		m_uTableMask = 0;
		m_uTableBits = 0;

        // Lose our table
		m_table.Delete();
	}

	//==============================================================
	// Copy()
	//==============================================================
	/// Returns a copy of the internal list
	/**
		
		\return Copy of the list
	
		\see 
	*/
	TAssoList Copy()
	{	TAssoList lst;
		for ( TList< T_OBJ, T_NODE >::iterator it; Next( it ); )
			lst[ it.Node()->key ] = *it;
		return lst;
	}

private:

	/// Hash table size
	oexUINT									m_uTableSize;

	/// Hash table mask
	oexUINT									m_uTableMask;

	/// Bits of the mask to use
	oexUINT									m_uTableBits;

	/// Hash table
	TMem< TList< iterator > >			    m_table;

	/// Hashing object
	T_HASH									m_hash;

};


