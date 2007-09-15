/*------------------------------------------------------------------
// list.h
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

template< typename T_OBJ >
    class TListNode
{
public:

	enum
	{
		/// Invokes the insert action
		eInsert = 1,

		/// Invokes the append action
		eAppend = 2
	};

public:

    /// Default constructor
    TListNode()
	{	m_pPrev = oexNULL; 
		m_pNext = oexNULL; 
    }

	//==============================================================
	// TListNode()
	//==============================================================
	/// Constructs a list node object
	/**
		\param [in] x_pIt	-	Pointer to list node object
		\param [in] x_nOpt	-	How to insert

		Inserts this node into the list by inserting or appending to
		the object in x_pIt.
		
		\return 
	
		\see 
	*/
	TListNode( TListNode *x_pIt, oexINT x_nOpt )
	{
		m_pPrev = oexNULL; m_pNext = oexNULL; 
		if ( x_pIt ) 
        {
            if ( eAppend == x_nOpt ) 
                Append( x_pIt ); 
            else 
                Insert( x_pIt ); 
        } // end if
	}

	//==============================================================
	// TListNode()
	//==============================================================
	/// Constructs a list node object
	/**
		\param [in] x_pObj	-	Pointer to object to contain in this
								node.
		\param [in] x_pIt	-	Pointer to list node object
		\param [in] x_nOpt	-	How to insert

		Inserts this node into the list by inserting or appending to
		the object in x_pIt.
		
		\return 
	
		\see 
	*/
	TListNode( T_OBJ *x_pObj, TListNode *x_pIt, oexINT x_nOpt )
	{
        if ( x_pObj )
            m_obj = *x_pObj;

		m_pPrev = oexNULL; 
        m_pNext = oexNULL;

		if ( x_pIt ) 
        { 
            if ( eAppend == x_nOpt ) 
                Append( x_pIt ); 
            else 
                Insert( x_pIt ); 
        } // end if
	}

    /// Destructor
    virtual ~TListNode()
    {   Destroy();
    }

	//==============================================================
	// Destroy()
	//==============================================================
	/// Removes from the list and destroys the encapsulated object
	void Destroy()
	{	Remove(); 
	}

public:

	//==============================================================
	// operator ->()
	//==============================================================
	/// Pointer difference operator
	/**
		\return Linked reference to the encapsulated object
	*/
	T_OBJ* operator ->() 
    {
        return &m_obj; 
    }

	//==============================================================
	// operator *()
	//==============================================================
	/// Cast operator
	/**
		\return Linked reference pointer to the encapsulated object
	*/
	T_OBJ& operator *() 
    {
        return m_obj; 
    }


	//==============================================================
	// operator &()
	//==============================================================
	/// Cast operator
	/**
		\return Linked reference reference to the encapsulated object
	*/
	operator T_OBJ&() 
    {
        return m_obj; 
    }

	//==============================================================
	// Obj()
	//==============================================================
	/// Returns object pointer
	/**
		\return Linked reference reference to the encapsulated object
	*/
	T_OBJ& Obj() 
    {
        return m_obj; 
    }

    /// Assignment operator
    TListNode& operator = ( oexCONST T_OBJ &x_rObj )
    {
        m_obj = x_rObj;

        return *this;
    }

public:

	//==============================================================
	// Next()
	//==============================================================
	/// Next node in the list
	/**
		\return Pointer to the next node in the list
	*/
	TListNode* Next() { return m_pNext; }

	//==============================================================
	// Prev()
	//==============================================================
	/// Previous node in the list
	/**
		\return Pointer to the previous node in the list
	*/
	TListNode* Prev() { return m_pPrev; }

public:

	//==============================================================
	// Insert()
	//==============================================================
	/// Inserts the item at the beginning of the list
	/**
		\param [in] x_pIt	-	Pointer to node object
		
		\return Linked reference to inserted object
	
		\see Append()
	*/
	TListNode& Insert( TListNode *x_pIt )
	{
		if ( !x_pIt ) 
            return *this;

		// Insert us
		m_pNext = x_pIt;
		m_pPrev = x_pIt->m_pPrev;
		x_pIt->m_pPrev = this;

		if ( m_pPrev )
            m_pPrev->m_pNext = this;

		return *this;			
	}

	//==============================================================
	// Append()
	//==============================================================
	/// Appends the item to the end of a list
	/**
		\param [in] x_pIt	-	Pointer to node object
		
		\return Linked reference to inserted object
	
		\see Insert()
	*/
	TListNode& Append( TListNode *x_pIt )
	{
		if ( !x_pIt ) 
            return *this;

		// Insert us
		m_pPrev = x_pIt;
		m_pNext = x_pIt->m_pNext;
		x_pIt->m_pNext = this;

		if ( m_pNext ) 
            m_pNext->m_pPrev = this;

		return *this;			
	}
	
	//==============================================================
	// Remove()
	//==============================================================
	/// Removes the item from the list without deleting it
	/**			
		\return Linked reference to the encapsulated object
	
		\warning This does not delete the item!

		\see 
	*/
	TListNode& Remove()
	{	
        if ( m_pPrev ) 
            m_pPrev->m_pNext = m_pNext;

		if ( m_pNext ) 
            m_pNext->m_pPrev = m_pPrev;

		m_pPrev = m_pNext = oexNULL;

		return *this;
	}

	//==============================================================
	// MoveUp()
	//==============================================================
	/// Moves the item up in the list
	/**
		\return Linked reference to the encapsulated object
	*/
	TListNode& MoveUp()
	{	
        if ( !m_pPrev ) 
            return *this;

		TListNode *pPrev = m_pPrev;

		Remove(); 
        Insert( pPrev );

		return *this;
	}

	//==============================================================
	// MoveDown()
	//==============================================================
	/// Moves the item down in the list
	/**
		\return Linked reference to the encapsulated object
	*/
	TListNode& MoveDown()
	{	
        if ( !m_pNext ) 
            return *this;

		TListNode *pNext = m_pNext;
		
        Remove(); 
        Append( pNext );

		return *this;
	}

	//==============================================================
	// SetAt()
	//==============================================================
	/// Sets the position within the list
	/**
		\param [in] x_pPrev		-	Pointer to previous node object
		\param [in] x_pNext		-	Pointer to next node object
		
		Sets this nodes explicit position in the list

		\return Pointer to this object
	
		\see 
	*/
	TListNode& SetAt( TListNode *x_pPrev, TListNode *x_pNext )
	{	
        m_pPrev = x_pPrev; 
        m_pNext = x_pNext; 
        return *this; 
    }

private:
    
	/// Pointer to the next list node
	TListNode					*m_pNext;

	/// Pointer to the previous list node
	TListNode					*m_pPrev;

	/// The actual encapsulated object
	T_OBJ       			    m_obj;

};


/// List iterator
template< class T_OBJ, class T_NODE = TListNode< T_OBJ > > 
    class TListIterator
{
public:

    TListIterator() 
    {
    }

    TListIterator( TListNode< T_OBJ > *x_pLn )
    {
        m_memListNode.Share( x_pLn );
    }

    TListIterator( TListIterator &x_rLi )
    {
        m_memListNode.Share( x_rLi.m_memListNode );
    }

    virtual ~TListIterator()
    {
        Destroy();
    }

    void Destroy()
    {
        m_memListNode.Delete();
    }

    oexBOOL IsValid()
    {   return m_memListNode.Ptr() ? oexTRUE : oexFALSE;
    }

    TListIterator& operator = ( TListNode< T_OBJ > *x_pLn )
    {
        m_memListNode.Share( x_pLn );
        return *this;
    }

    TListIterator& operator = ( TListIterator &x_rLi )
    {
        m_memListNode.Share( x_rLi.m_memListNode );
        return *this;
    }

    T_OBJ* Ptr()
    {
        if ( !m_memListNode.Ptr() )
            return oexNULL;

        return &m_memListNode.Ptr()->Obj();
    }

	//==============================================================
	// operator ->()
	//==============================================================
	/// Pointer difference operator
	/**
		\return Linked reference to the encapsulated object
	*/
	T_OBJ*  operator ->() 
    { return Ptr(); }

	//==============================================================
	// operator *()
	//==============================================================
	/// Cast operator
	/**
		\return Linked reference pointer to the encapsulated object
	*/
	T_OBJ& operator *() 
    {
        return *Ptr(); 
    }


	//==============================================================
	// Obj()
	//==============================================================
	/// Cast operator
	/**
		\return Returns reference to the encapsulated object
	*/
	T_OBJ& Obj() 
    {
        return *Ptr(); 
    }


    TListIterator& operator = ( oexCONST T_OBJ &x_rObj )
    {
        if ( m_memListNode.Ptr() )
            *m_memListNode.Ptr() = x_rObj;
        return *this;
    }

    oexBOOL operator == ( oexCONST T_OBJ &x_rObj )
    {
        if ( !m_memListNode.Ptr() )
            return oexFALSE;
        
        return *m_memListNode.Ptr() == x_rObj;        
    }

public:

	//==============================================================
	// End()
	//==============================================================
	/// Returns non-zero if at the end of the list
	oexBOOL End() 
    {
        return ( !m_memListNode.Ptr() || !m_memListNode.Ptr()->Next() );
    }

	//==============================================================
	// Begin()
	//==============================================================
	/// Returns non-zero if at the beginning of the list
	oexBOOL Begin() 
    {
        return ( !m_memListNode.Ptr() || !m_memListNode.Ptr()->Prev() );
    }

	//==============================================================
	// Next()
	//==============================================================
	/// Advances to the next position in the list
	oexBOOL Next() 
	{
        if ( !m_memListNode.Ptr() || !m_memListNode.Ptr()->Next() )
            return oexFALSE;

        m_memListNode.Share( m_memListNode.Ptr()->Next() );

        return oexTRUE;

	}

    TListIterator GetNext()
    {
        if ( !m_memListNode.Ptr() )
            return TListIterator();

        return TListIterator( m_memListNode.Ptr()->Next() );
    }

	//==============================================================
	// Prev()
	//==============================================================
	/// Advances to the previous position in the list
	oexBOOL Prev() 
	{
        if ( !m_memListNode.Ptr() || !m_memListNode.Ptr()->Prev() )
            return oexFALSE;

        m_memListNode.Share( m_memListNode.Ptr()->Prev() );

        return oexTRUE;

	}

    TListIterator GetPrev()
    {
        if ( !m_memListNode.Ptr() )
            return TListIterator();

        return TListIterator( m_memListNode.Ptr()->Prev() );
    }

	//==============================================================
	// operator ++
	//==============================================================
	/// Advances to the next position in the list
	TListIterator& operator ++( int )
	{	Next();
		return *this;
	}

	//==============================================================
	// operator --
	//==============================================================
	/// Advances to the previous position in the list
	TListIterator& operator --( int )
	{	Prev();
		return *this;
	}

private:

    /// List node
    TMem< T_NODE >      m_memListNode;

};


/// Linked list
/**
    

*/
template< typename T_OBJ, typename T_NODE = TListNode< T_OBJ > >
    class TList
{
public:

	/// TList iterator type
	typedef TListIterator< T_OBJ, T_NODE > iterator;

public:

    TList()
	{	m_uSize = 0;
		m_pHead = m_pTail = oexNULL; 
    }

    virtual ~TList()
    {   Destroy();
    }

	//==============================================================
	// Destroy()
	//==============================================================
	/// Destroys the list and readys this object for reuse
	void Destroy()
	{
		m_uSize = 0;
		m_pTail = oexNULL;

        // For the entire list
		while ( m_pHead )
		{
            // Save pointer
            T_NODE *pNode = m_pHead;
            
            // Point to the next node
            m_pHead = pNode->Next();

            // Just in case someone else is still hanging onto the memory
            pNode->SetAt( oexNULL, oexNULL );                       
            
            // Delete this node
            OexAllocDelete( pNode );

		} // end while

	}

	//==============================================================
	// operator <<
	//==============================================================
	/// Appends the object to the back of the list
	/**
		\param [in] x_rObj		-	Reference to object that is used
									to initialize the new list item.

		\return iterator containing object
	*/
	TList& operator << ( oexCONST T_OBJ &x_rObj )
	{	
        iterator itNew = Append();

		if ( itNew.Ptr() ) 
            itNew = x_rObj;

		return *this;
	}

	//==============================================================
	// Append()
	//==============================================================
	/// Appends the object to the back of the list
	/**
		\param [in] x_pObj		-	Pointer to object to insert.
									If x_pObj is NULL, a new object
									is created and inserted.

		\return iterator containing object
	*/
	iterator
        Append( T_OBJ *x_pObj = oexNULL )
	{
        T_NODE *pNode = oexNULL;

        pNode = OexAllocConstruct< T_NODE >( x_pObj, m_pTail, T_NODE::eAppend ); 
        if ( !pNode )
            return iterator();

		m_uSize++; 

		m_pTail = pNode; 

		if ( !m_pHead ) 
			m_pHead = pNode; 

		return m_pTail; 
	}

	//==============================================================
	// Append()
	//==============================================================
	/// Appends the object to the back of the list
	/**
		\param [in] x_rObj		-	Reference to object that is used
									to initialize the new list item.

		\return iterator containing object
	*/
	iterator
        Append( oexCONST T_OBJ &x_rObj )
	{	
        iterator itNew = Append();

		if ( itNew.Ptr() ) 
            itNew = x_rObj;

		return itNew;
	}

	//==============================================================
	// First()
	//==============================================================
	/// Returns the iterator for the first list item
	iterator First() 
    { return iterator( m_pHead ); }

	//==============================================================
	// Last()
	//==============================================================
	/// Returns the iterator for the last list item
	iterator Last() 
    { return iterator( m_pTail ); }

	//==============================================================
	// Next()
	//==============================================================
	/// Sets the iterator to the next item in the list
	/**
		\param [in] x_it 

		If it is an empty iterator, it is set to the first item
		in the list.

		If there are no more items in the list, this function
		returns zero.
	
	  \code

		// Walk the list forward
		for ( oex::TList< int >::iterator it; lst.Next( it ); )
			
			Use( it );
	
	  \endcode

		\return Non-zero if it points to the next item in the list
	
		\see Prev()
	*/
	oexCONST oexBOOL Next( iterator& x_it )
	{	
        if ( x_it.IsValid() )
			return x_it.Next();

		else if ( !m_pHead )
			return oexFALSE;

		x_it = m_pHead;

		return oexTRUE;
	}

	//==============================================================
	// Prev()
	//==============================================================
	/// Sets the iterator to the previous item in the list
	/**
		\param [in] x_it 

		If it is an empty iterator, it is set to the last item
		in the list.

		If there are no more items in the list, this function
		returns zero.
	
	  \code

		// Walk the list backward
		for ( oex::TList< int >::iterator it; lst.Prev( it ); )
			
			Use( it );
	
	  \endcode

		\return Non-zero if it points to the previous list item
	
		\see Next()
	*/
	oexCONST oexBOOL Prev( iterator& x_it )
	{	
        if ( x_it.IsValid() )
			return x_it.Prev();

		else if ( !m_pTail )
			return oexFALSE;

		x_it = m_pTail;

		return oexTRUE;
	}

	//==============================================================
	// Erase()
	//==============================================================
	/// Erases the specified iterator from the list
	/**
		\param [in] x_it		-	iterator to erase
		\param [in] x_bForward	-	Non-zero to return the previous
									list item, zero to return the
									next list item.

		Use this function to remove an item from the list.

		As a convinience, the previous or next item can be returned.
		By assigning this return value to the iterator, you can
		continue an iteration loop

	\code

		// Forward
		for ( oex::TList< int >::iterator it; lst.Next( it ); )
		{
			if ( WantErase( it ) )

				it = lst.Erase( it );

		} // end if

		// Backward
		for ( oex::TList< int >::iterator it; lst.Prev( it ); )
		{
			if ( WantErase( it ) )

				it = lst.Erase( it, oexFALSE );

		} // end if

	\endcode
		
		\return 
	
		\see 
	*/
	iterator Erase( iterator x_it, oexBOOL x_bForward = oexTRUE )
	{
		// Ensure valid iterator
		if ( !x_it.IsValid() ) 
            return iterator();

        T_NODE *pThis = x_it.Node();
		T_NODE *pNext = x_it.GetNext().Node();
		T_NODE *pPrev = x_it.GetPrev().Node();

		// Update head pointer if needed
		if ( x_it.Node() == m_pHead )
			m_pHead = pNext;

		// Update tail pointer if needed
		if ( x_it.Node() == m_pTail )
			m_pTail = pPrev;

        // Unlink the item
        Unlink( x_it );

        // One less item
        if ( m_uSize ) 
            m_uSize--;

        // Delete the node
        OEX_TRY { delete pThis; }
        OEX_CATCH_ALL { oexASSERT( 0 ); }

		// Are we going backward?
		if ( !x_bForward ) 
			return pNext;

		return pPrev;
	}

protected:

	//==============================================================
	// Unlink()
	//==============================================================
    /// Use this function to unlink a list item when it is erased
    /**
        This is used by TAssoList for instance, to remove the 
        item from the index when it is erased.
    */
    virtual void Unlink( iterator x_it ) {}

public:

	//==============================================================
	// SearchValue()
	//==============================================================
	/// Searches for the specified value by comparing each item
	/**
		\param [in] x_tObj	-	Item for comparison

		This function searches every list item, comparing each one
		to x_tObj until a match is found

		Can we say sloooooow!

		You should use TAssoList<> if you require random access.  
		There are special circumstances for this function.
		
		\return iterator to matching object or empty iterator.
	
		\see 
	*/
	template < class T_CMPOBJ >
		iterator SearchValue( oexCONST T_CMPOBJ x_tObj )
		{	for ( iterator it; Next( it ); )
				if ( *it == x_tObj )
					return it;
			return iterator();
		}

	//==============================================================
	// SearchValue()
	//==============================================================
	/// Searches for the specified key by comparing each item
	/**
		\param [in] x_tObj	-	Item for comparison
		\param [in] x_fCmp	-	Function for comparison

		This function searches every list item, comparing each one
		to x_tObj using the specified function x_fCmp() until a match 
		is found.

		Obviously, this is slow.

		You should use TAssoList<> if you require random access.  
		There are special circumstances for this function.

		\code

			// Compare function
			oexBOOL MyCompare( oexINT a, oexINT b )
			{	return a == b; 
			}

			// ...

			// Find item using custom compare function
			TList< oexINT > it = lst.SearchKey( 2, MyCompare );

		\endcode
		
		\return iterator to matching object or empty iterator.
	
		\see 
	*/
	template< class T_CMPOBJ, class T_CMPFUNC >
		iterator SearchValue( oexCONST T_CMPOBJ tObj, T_CMPFUNC fCmp )
		{	for ( iterator it; Next( it ); )
				if ( 0 <= fCmp( *it, tObj ) )
					return it;
			return iterator();
		}


public:

	//==============================================================
	// Size()
	//==============================================================
	/// Returns the number of items in the list
	oexUINT Size() { return m_uSize; }

private:
    
	/// Holds the size of the list
	oexUINT					m_uSize;

	/// Pointer to the first node in the list
	T_NODE					*m_pHead;

	/// Pointer to the last node in the list
	T_NODE					*m_pTail;

};



