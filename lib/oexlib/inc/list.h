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
	/// Removes from the list
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
/*	operator T_OBJ&() 
    {
        return m_obj; 
    }
*/
	//==============================================================
	// Obj()
	//==============================================================
	/// Returns object reference
	/**
		\return Linked reference reference to the encapsulated object
	*/
	T_OBJ& Obj() 
    {
        return m_obj; 
    }

	//==============================================================
	// Ptr()
	//==============================================================
	/// Returns object reference
	/**
		\return Linked reference reference to the encapsulated object
	*/
	T_OBJ* Ptr() 
    {
        return &m_obj; 
    }

    /// Assignment operator
    TListNode& operator = ( oexCONST T_OBJ &x_rObj )
    {
        m_obj = (T_OBJ&)x_rObj;

        return *this;
    }

    /// Comparison operator
    oexBOOL operator == ( oexCONST T_OBJ &x_rObj )
    {
        return m_obj == (T_OBJ&)x_rObj;
    }

    /// Comparison operator
    oexBOOL operator != ( oexCONST T_OBJ &x_rObj )
    {
        return m_obj != (T_OBJ&)x_rObj;
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
	TListNode* Prev() 
    {   return m_pPrev; }

public:

	//==============================================================
	// Insert()
	//==============================================================
	/// Inserts the item at the beginning of the list
	/**
		\param [in] x_pIt	-	Pointer to node object
		
		\return Reference to this item
	
		\see Append()
	*/
	TListNode& Insert( TListNode *x_pIt )
	{
		if ( !x_pIt ) 
            return *this;

        // Remove this item from the list
        Remove();

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
		
		\return Reference to this item
	
		\see Insert()
	*/
	TListNode& Append( TListNode *x_pIt )
	{
		if ( !x_pIt ) 
            return *this;

        // Remove us from the list
        Remove();

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
		\return Reference to this item
	*/
	TListNode& MoveUp()
	{	
        if ( m_pPrev ) 
            Insert( m_pPrev );

		return *this;
	}

	//==============================================================
	// MoveDown()
	//==============================================================
	/// Moves the item down in the list
	/**
		\return Reference to this item
	*/
	TListNode& MoveDown()
	{	
        if ( m_pNext ) 
            Append( m_pNext );

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

		\return Reference to this item
	
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

    TListIterator( oexCONST T_NODE *x_pLn )
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
/*
    TListIterator& operator = ( TListNode< T_OBJ > *x_pLn )
    {
        m_memListNode.Share( x_pLn );
        return *this;
    }
*/
    TListIterator& operator = ( oexCONST T_NODE *x_pLn )
    {
        m_memListNode.Share( x_pLn );
        return *this;
    }

    TListIterator& operator = ( oexCONST TListIterator &x_rLi )
    {
        m_memListNode.Share( x_rLi.m_memListNode );
        return *this;
    }

    T_OBJ* Ptr()
    {
        if ( !m_memListNode.Ptr() )
            return oexNULL;

        return m_memListNode.Ptr()->Ptr();
    }

	//==============================================================
	// operator ->()
	//==============================================================
	/// Pointer difference operator
	/**
		\return Linked reference to the encapsulated object
	*/
	T_OBJ* operator ->() 
    {
        return Ptr(); 
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
/*
    TListIterator& operator = ( oexCONST T_OBJ &x_rObj )
    {
        if ( m_memListNode.Ptr() )
            m_memListNode.Ptr()->Obj() = x_rObj;

        return *this;
    }

    oexBOOL operator == ( oexCONST T_OBJ &x_rObj )
    {
        if ( !m_memListNode.Ptr() )
            return oexFALSE;
        
        return m_memListNode.Ptr()->Obj() == x_rObj;        
    }
*/
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

        m_memListNode.Share( (T_NODE*)m_memListNode.Ptr()->Next() );

        return oexTRUE;
	}

    TListIterator GetNext()
    {
        if ( !m_memListNode.Ptr() )
            return TListIterator();

        return TListIterator( (T_NODE*)m_memListNode.Ptr()->Next() );
    }

	//==============================================================
	// Prev()
	//==============================================================
	/// Advances to the previous position in the list
	oexBOOL Prev() 
	{
        if ( !m_memListNode.Ptr() || !m_memListNode.Ptr()->Prev() )
            return oexFALSE;

        m_memListNode.Share( (T_NODE*)m_memListNode.Ptr()->Prev() );

        return oexTRUE;

	}

    TListIterator GetPrev()
    {
        if ( !m_memListNode.Ptr() )
            return TListIterator();

        return TListIterator( (T_NODE*)m_memListNode.Ptr()->Prev() );
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

	//==============================================================
	// Node()
	//==============================================================
	/// Returns a pointer to the list node
    T_NODE* Node()
    {   
        if ( !m_memListNode.Ptr() )
            return oexNULL;
        return m_memListNode.Ptr();
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

	TList( TList &x_rList )
	{	m_uSize = 0;
		m_pHead = m_pTail = oexNULL; 
		Attach( x_rList ); 
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
            m_pHead = (T_NODE*)pNode->Next();
//			m_pHead = dynamic_cast<T_NODE*>( pNode->Next() );

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
            itNew.Obj() = x_rObj;

		return *this;
	}

	//==============================================================
	// Append()
	//==============================================================
	/// Appends a list onto this one
	/**
		\param [in] x_lst		-	List to append

		\return Reference to this list
	*/
	TList& Append( TList &x_lst )
    {
        // Ensure valid list
        if ( !x_lst.Size() || !x_lst.m_pHead || !x_lst.m_pTail )
            return *this;

        // Update our tail pointer if any
        if ( m_pTail )
            m_pTail->SetAt( m_pTail->Prev(), x_lst.m_pHead );

        // Update our head pointer
        if ( !m_pHead )
            m_pHead = x_lst.m_pHead;

        // Update our tail pointer
        m_pTail = x_lst.m_pTail;

        // Relieve the list of it's burden
        x_lst.Detach();

        return *this;
    }

	//==============================================================
	// Insert()
	//==============================================================
	/// Insert a list into this one
	/**
		\param [in] x_lst		-	List to insert

		\return Reference to this list
	*/
	TList& Insert( TList &x_lst )
    {
        // Ensure valid list
        if ( !x_lst.Size() || !x_lst.m_pHead || !x_lst.m_pTail )
            return *this;

        // Update our tail pointer if any
        if ( m_pHead )
            m_pHead->SetAt( x_lst.m_pTail, m_pTail->Next() );

        // Update our head pointer
        if ( !m_pTail )
            m_pTail = x_lst.m_pTail;

        // Update our tail pointer
        m_pHead = x_lst.m_pHead;

        // Relieve the list of it's burden
        x_lst.Detach();

        return *this;
    }

	//==============================================================
	// Append()
	//==============================================================
	/// Appends the object to the back of the list
	/**
		\param [in] x_it		-	Node to append.  Must not be a
                                    member of another list.
                                    

		\return iterator containing object
	*/
	iterator Append( iterator x_it )
    {   if ( x_it.Node() )
            Append( *x_it.Node() );
        return x_it;
    }

	//==============================================================
	// Append()
	//==============================================================
	/// Appends the object to the back of the list
	/**
		\param [in] x_rNode		-	Node to append.  Must not be a
                                    member of another list.

		\return iterator containing object
	*/
	iterator Append( T_NODE &x_rNode )
    {
        x_rNode.Append( m_pTail );

		m_uSize++; 

		m_pTail = &x_rNode; 

		if ( !m_pHead ) 
			m_pHead = &x_rNode; 

		return m_pTail; 

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
	iterator Append( T_OBJ *x_pObj = oexNULL )
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
	iterator Append( oexCONST T_OBJ &x_rObj )
	{	
        iterator itNew = Append();

		if ( itNew.Ptr() ) 
            itNew.Obj() = x_rObj;

		return itNew;
	}

	//==============================================================
	// Append()
	//==============================================================
	/// Append an object array onto the list
	/**
		\param [in] x_pList		-	Pointer to object array
		\param [in] x_uCount		-	Number of items in the array
		
		\return Reference to this list
	
		\see 
	*/
	template < class T >
		TList& Append( T *x_pList, oexUINT x_uCount )
	{	for( oexUINT i = 0; i < x_uCount; i++ )
			Append( x_pList[ i ] );			
		return *this;
	}


	//==============================================================
	// Insert()
	//==============================================================
	/// Inserts the object to the front of the list
	/**
		\param [in] x_it		-	Node to append.  Must not be a
                                    member of another list.

		\return iterator containing object
	*/
	iterator Insert( iterator x_it )
    {   if ( x_it.Node() )
            Insert( *x_it.Node() );
        return x_it;
    }

	//==============================================================
	// Insert()
	//==============================================================
	/// Inserts the object to the front of the list
	/**
		\param [in] x_rNode		-	Node to insert.  Must not be a
                                    member of another list.

		\return iterator containing object
	*/
	iterator Insert( T_NODE &x_rNode )
    {
        x_rNode.Insert( m_pHead );

		m_uSize++; 

		m_pHead = &x_rNode; 

		if ( !m_pTail ) 
			m_pTail = &x_rNode; 

		return m_pHead; 

    }

	//==============================================================
	// Insert()
	//==============================================================
	/// Inserts the object to the back of the list
	/**
		\param [in] x_pObj		-	Pointer to object to insert.
									If x_pObj is NULL, a new object
									is created and inserted.

		\return iterator containing object
	*/
	iterator Insert( T_OBJ *x_pObj = oexNULL )
	{
        T_NODE *pNode = oexNULL;

        pNode = OexAllocConstruct< T_NODE >( x_pObj, m_pHead, T_NODE::eInsert ); 
        if ( !pNode )
            return iterator();

		m_uSize++; 

		m_pHead = pNode; 

		if ( !m_pTail ) 
			m_pTail = pNode; 

		return m_pHead; 
	}

	//==============================================================
	// Insert()
	//==============================================================
	/// Inserts the object to the back of the list
	/**
		\param [in] x_rObj		-	Reference to object that is used
									to initialize the new list item.

		\return iterator containing object
	*/
	iterator Insert( oexCONST T_OBJ &x_rObj )
	{	
        iterator itNew = Insert();

		if ( itNew.Ptr() ) 
            itNew.Obj() = x_rObj;

		return itNew;
	}

	//==============================================================
	// Insert()
	//==============================================================
	/// Inserts an object array into the list
	/**
		\param [in] x_pList		-	Pointer to object array
		\param [in] x_uCount		-	Number of items in the array
		
		\return Reference to this list
	
		\see 
	*/
	template < class T >
		TList& Insert( T *x_pList, oexUINT x_uCount )
	{	for( oexUINT i = 0; i < x_uCount; i++ )
			Insert( x_pList[ i ] );			
		return *this;
	}


	//==============================================================
	// First()
	//==============================================================
	/// Returns the iterator for the first list item
	iterator First() oexCONST
    {   return iterator( m_pHead ); }

	//==============================================================
	// Last()
	//==============================================================
	/// Returns the iterator for the last list item
	iterator Last() oexCONST
    {   return iterator( m_pTail ); }

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
	oexCONST oexBOOL Next( iterator& x_it ) oexCONST
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
	oexCONST oexBOOL Prev( iterator& x_it ) oexCONST
	{	
        if ( x_it.IsValid() )
			return x_it.Prev();

		else if ( !m_pTail )
			return oexFALSE;

		x_it = m_pTail;

		return oexTRUE;
	}

	//==============================================================
	// MoveUp()
	//==============================================================
	/// Moves a node closer to the start of the list
	/**
		\param [in] x_itMove	-	Node to move.

		\return iterator containing object
	*/
	iterator MoveUp( iterator x_itMove )
    {   
        // Sanity checks
        if ( !x_itMove.IsValid() )
            return x_itMove;

        // Update head pointer if needed
        if ( x_itMove.Node()->Prev() == m_pHead )
            m_pHead = x_itMove.Node();
        
        // Update tail pointer if needed
        if ( x_itMove.Node() == m_pTail )
            m_pTail = x_itMove.Node()->Prev();

        // Put us up
        x_itMove.Node()->MoveUp();

        return x_itMove;
    }


	//==============================================================
	// MoveDown()
	//==============================================================
	/// Moves a node closer to the end of the list
	/**
		\param [in] x_itMove	-	Node to move.

		\return iterator containing object
	*/
	iterator MoveDown( iterator x_itMove )
    {   
        // Sanity checks
        if ( !x_itMove.IsValid() )
            return x_itMove;

        // Update head pointer if needed
        if ( x_itMove.Node() == m_pHead )
            m_pHead = x_itMove.Node()->Next();

        // Update tail pointer if needed
        if ( x_itMove.Node()->Next() == m_pTail )
            m_pTail = x_itMove.Node();
        
        // Put us up
        x_itMove.Node()->MoveDown();

        return x_itMove;
    }


	//==============================================================
	// MoveBefore()
	//==============================================================
	/// Inserts the object to the front of the list
	/**
		\param [in] x_itMove	-	Node to move.
        \param [in] x_itRef     -   Reference node.

		\return iterator containing object
	*/
	iterator MoveBefore( iterator x_itMove, iterator x_itRef )
    {   
        // Sanity checks
        if ( !x_itRef.IsValid() || !x_itMove.IsValid() )
            return x_itMove;

        // Put us in front of this node
        x_itMove.Node()->Insert( x_itRef.Node() );

        // Update head pointer if needed
        if ( x_itRef.Node() == m_pHead )
            m_pHead = x_itMove.Node();
        
        // Update tail pointer if needed
        if ( x_itMove.Node() == m_pTail )
            m_pTail = x_itRef.Node();

        return x_itMove;
    }


	//==============================================================
	// MoveAfter()
	//==============================================================
	/// Inserts the object to the front of the list
	/**
		\param [in] x_itMove	-	Node to move.
        \param [in] x_itRef     -   Reference node.

		\return iterator containing object
	*/
	iterator MoveAfter( iterator x_itMove, iterator x_itRef )
    {   
        // Sanity checks
        if ( !x_itRef.IsValid() || !x_itMove.IsValid() )
            return x_itMove;

        // Put us in front of this node
        x_itMove.Node()->Append( x_itRef.Node() );

        // Update head pointer if needed
        if ( x_itMove.Node() == m_pHead )
            m_pHead = x_itRef.Node();
        
        // Update tail pointer if needed
        if ( x_itRef.Node() == m_pTail )
            m_pTail = x_itMove.Node();

        return x_itMove;
    }



	//==============================================================
	// Remove()
	//==============================================================
    /// Removes a node from the list without deleting it
	iterator Remove( iterator x_it )
	{
		// Ensure valid iterator
		if ( !x_it.IsValid() ) 
            return iterator();

        T_NODE *pThis = x_it.Node();
		T_NODE *pNext = x_it.GetNext().Node();
		T_NODE *pPrev = x_it.GetPrev().Node();

		// Update head pointer if needed
		if ( pThis == m_pHead )
			m_pHead = pNext;

		// Update tail pointer if needed
		if ( pThis == m_pTail )
			m_pTail = pPrev;

        // Remove from the list
        pThis->Remove();

        // Unlink the item
        Unlink( x_it );

        return x_it;
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
		if ( pThis == m_pHead )
			m_pHead = pNext;

		// Update tail pointer if needed
		if ( pThis == m_pTail )
			m_pTail = pPrev;

        // Unlink the item
        Unlink( x_it );

        // One less item
        if ( m_uSize ) 
            m_uSize--;

        // Delete the node
        OEX_TRY { OexAllocDelete( pThis ); }
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
    virtual void Unlink( iterator x_it ) 
    {}

public:

	//==============================================================
	// operator []
	//==============================================================
	/// Returns the list item by index
    /**
        \warning n time
    */
    iterator operator[]( oexUINT uIndex )
	{	for ( iterator it; Next( it ); )
            if ( !uIndex-- )
                return it;
		return iterator();
    }

	//==============================================================
	// GetByIndex()
	//==============================================================
	/// Returns the list item by index
    /**
        \warning n time
    */
    iterator GetByIndex( oexUINT uIndex )
	{	for ( iterator it; Next( it ); )
            if ( !uIndex-- )
                return it;
		return iterator();
    }

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
	// operator =
	//==============================================================
	/// Assignes a list object from a list reference
	/**
		\param [in] x_rList 
		
		Note that this class just assumes the list.  A copy is not 
		made.  After this function, x_rList will be empty.  

		See Attach() for more details

		\return Reference to this object
	
		\see 
	*/
	TList& operator = ( oexCONST TList &x_rList )
	{	return Attach( x_rList ); }

	//==============================================================
	// Attach()
	//==============================================================
	/// Assignes a list object from a list reference
	/**
		\param [in] x_rList 
		
		Note that this class just assumes the list.  A copy is not 
		made.  After this function, x_rList will be empty.  

		\code

			TList< oexINT > lst1;

			lst1 << 11;
			lst1 << = 22;

			TList< oexINT > lst2( lst1 );

			// lst2 now contains the list, lst1 is empty

			// If you want to make a copy, use...

			TList< oexINT > lst2( lst1.Copy() );

		\endcode

		\return Reference to this object
	
		\see 
	*/
	TList& Attach ( oexCONST TList &x_rList )
	{
		Destroy();

		// Copy the list variables
		m_uSize = x_rList.m_uSize;
		m_pHead = x_rList.m_pHead;
		m_pTail = x_rList.m_pTail;

		// We own the list now
		( (TList&)x_rList ).m_uSize = 0;
		( (TList&)x_rList ).m_pHead = oexNULL;
		( (TList&)x_rList ).m_pTail = oexNULL;

		return *this;
	}

	//==============================================================
	// Detach()
	//==============================================================
	/// Detaches from the list without releasing list items
	virtual void Detach()
	{
		m_uSize = 0;
		m_pHead = oexNULL;
		m_pTail = oexNULL;
	}

	//==============================================================
	// Copy()
	//==============================================================
	/// Returns a copy of the list
	/**

		\return TList object containing a copy of the list
	
		\see 
	*/
	TList Copy()
	{	TList lst;
		for ( TList< T_OBJ >::iterator it; Next( it ); )
			lst.Append( *it );
		return lst;
	}


public:

	//==============================================================
	// Size()
	//==============================================================
	/// Returns the number of items in the list
	oexUINT Size() 
    {   return m_uSize; }

private:
    
	/// Holds the size of the list
	oexUINT					m_uSize;

	/// Pointer to the first node in the list
	T_NODE					*m_pHead;

	/// Pointer to the last node in the list
	T_NODE					*m_pTail;

};



