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
		if ( x_pIt ) { if ( eAppend == x_nOpt ) Append( x_pIt ); else Insert( x_pIt ); }
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
        if ( x_pObj ) m_obj.Attach( x_pObj );
		m_pPrev = oexNULL; m_pNext = oexNULL;
		if ( x_pIt ) { if ( eAppend == x_nOpt ) Append( x_pIt ); else Insert( x_pIt ); }
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
        m_obj.Delete(); 
	}

public:

	//==============================================================
	// operator ->()
	//==============================================================
	/// Pointer difference operator
	/**
		\return Linked reference to the encapsulated object
	*/
	TMem< T_OBJ > operator ->() { return m_obj; }

	//==============================================================
	// operator *()
	//==============================================================
	/// Cast operator
	/**
		\return Linked reference pointer to the encapsulated object
	*/
	operator TMem< T_OBJ >*() { return m_obj; }

	//==============================================================
	// operator &()
	//==============================================================
	/// Cast operator
	/**
		\return Linked reference reference to the encapsulated object
	*/
	operator TMem< T_OBJ >&() { return m_obj; }

	//==============================================================
	// Obj()
	//==============================================================
	/// Returns object pointer
	/**
		\return Linked reference reference to the encapsulated object
	*/
	TMem< T_OBJ >& Obj() { return m_obj; }

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
	TMem< T_OBJ > Insert( TListNode *x_pIt )
	{
		if ( !x_pIt ) return m_obj;

		// Insert us
		m_pNext = x_pIt;
		m_pPrev = x_pIt->m_pPrev;
		x_pIt->m_pPrev = this;
		if ( m_pPrev ) m_pPrev->m_pNext = this;

		return m_obj;			
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
	TMem< T_OBJ > Append( TListNode *x_pIt )
	{
		if ( !x_pIt ) return m_obj;

		// Insert us
		m_pPrev = x_pIt;
		m_pNext = x_pIt->m_pNext;
		x_pIt->m_pNext = this;
		if ( m_pNext ) m_pNext->m_pPrev = this;

		return m_obj;			
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
	TMem< T_OBJ > Remove()
	{	if ( m_pPrev ) m_pPrev->m_pNext = m_pNext;
		if ( m_pNext ) m_pNext->m_pPrev = m_pPrev;
		m_pPrev = m_pNext = oexNULL;
		return m_obj;
	}

	//==============================================================
	// MoveUp()
	//==============================================================
	/// Moves the item up in the list
	/**
		\return Linked reference to the encapsulated object
	*/
	TMem< T_OBJ > MoveUp()
	{	if ( !m_pPrev ) return m_obj;
		TListNode *pPrev = m_pPrev;
		Remove(); Insert( pPrev );
		return m_obj;
	}

	//==============================================================
	// MoveDown()
	//==============================================================
	/// Moves the item down in the list
	/**
		\return Linked reference to the encapsulated object
	*/
	TMem< T_OBJ > MoveDown()
	{	if ( !m_pNext ) return m_obj;
		TListNode *pNext = m_pNext;
		Remove(); Append( pNext );
		return m_obj;
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
	TListNode* SetAt( TListNode *x_pPrev, TListNode *x_pNext )
	{	m_pPrev = x_pPrev; m_pNext = x_pNext; return this; }

private:
    
	/// Pointer to the next list node
	TListNode					*m_pNext;

	/// Pointer to the previous list node
	TListNode					*m_pPrev;

	/// The actual encapsulated object
	TMem< T_OBJ >			    m_obj;

};


/// Linked list
/**
*/
template< typename T_OBJ, typename T_NODE = TListNode< T_OBJ > >
    class TList
{
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
            
            // Delete this node
            OexAllocDelete( pNode );

		} // end while

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
//	iterator
    T_NODE*
        Append( T_OBJ *x_pObj = oexNULL )
	{
        T_NODE *pNode = oexNULL;

        pNode = OexAllocConstruct< T_NODE >( x_pObj, m_pTail, T_NODE::eAppend ); 
//        if ( !pNode )
//            return iterator();

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
//	iterator
    T_NODE*
        Append( oexCONST T_OBJ &x_rObj )
	{	
        return Append();

//        iterator itNew = Append();

//		if ( itNew.Ptr() ) 
//            itNew = x_rObj;

//		return itNew;
	}


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



