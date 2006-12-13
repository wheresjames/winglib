/*------------------------------------------------------------------
// asso_list.h
// Copyright (c) 2006 
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

template < class OBJ > class CLinkedPtr
{
public:

	CLinkedPtr()
	{	m_pPrev = stNULL; m_pNext = stNULL; m_pObj = CreateObj(); }

	CLinkedPtr( OBJ *pObj )
	{	m_pPrev = stNULL; m_pNext = stNULL; 
		if ( !pObj ) m_pObj = CreateObj(); else m_pObj = pObj;
	}

	CLinkedPtr( const OBJ &rObj )
	{	m_pPrev = stNULL; m_pNext = stNULL; 
		m_pObj = CreateObj(); *m_pObj = rObj;
	}

	CLinkedPtr( CLinkedPtr &rLp )
	{	Insert( rLp );	}

	CLinkedPtr( const CLinkedPtr &rLp )
	{	Insert( rLp );	}

	CLinkedPtr( CLinkedPtr *pLp )
	{	if ( pLp ) Insert( *pLp ); }

	virtual ~CLinkedPtr() { Destroy(); }

	CLinkedPtr* operator = ( CLinkedPtr &rLp ) 
	{	Insert( rLp ); return this; }

	CLinkedPtr* operator = ( const CLinkedPtr &rLp ) 
	{	Insert( rLp ); return this; }

	CLinkedPtr* operator = ( CLinkedPtr *rLp ) 
	{	if ( pLp ) Insert( *pLp ); return this; }

	/// Obtains pointer and inserts us in the pointer chain
	OBJ* Insert( const CLinkedPtr &rLp )
	{	
		// Save object pointer
		m_pObj = rLp.m_pObj;

		// Insert us in the chain
		m_pPrev = (CLinkedPtr*)&rLp;
		m_pNext = rLp.m_pNext;
		rLp.m_pNext = this;
		if ( m_pNext ) m_pNext->m_pPrev = this;

		return m_pObj;
	}

	/// Destroys pointer and removes us from the pointer chain
	void Destroy()
	{	
		// Remove us from the list
		if ( m_pPrev ) m_pPrev->m_pNext = m_pNext;
		if ( m_pNext ) m_pNext->m_pPrev = m_pPrev;

		// Delete object if no more references
		if ( !m_pPrev && !m_pNext && m_pObj ) 
			delete m_pObj;

		// Lose pointers
		m_pObj = stNULL;
		m_pPrev = m_pNext = stNULL;
	}

	/// Provides a count of object references
	/*
		To make this faster I would have had to use more memory.  
		I really couldn't think of a good argument in favor of.

		\warning Not particularly fast!
	*/
	stUINT Count()
	{
		stUINT uCount = 1;

		// How many behind us?
		CLinkedPtr *p = m_pPrev;
		while ( p ) { uCount++; p = p->m_pPrev; }

		// How many ahead?
		p = m_pNext;
		while ( p ) { uCount++; p = p->m_pNext; }

		return uCount;
	}

	/// Creates object
	OBJ* CreateObj() 
	{	
		try { m_pObj = new OBJ; 
		} catch( ... ) { return stNULL; }

		return m_pObj;
	}

	OBJ& operator = ( const OBJ &rObj )
	{	if ( m_pObj ) *m_pObj = rObj; 
		return *m_pObj;
	}

	/// Returns object pointer
	operator OBJ*() { return m_pObj; }

	/// Returns object pointer
	operator OBJ&() { stASSERT( m_pObj ); return *m_pObj; }

	/// Difference operator
	OBJ* operator ->() { return m_pObj; }
	
private:

	/// Next element in the list
	mutable CLinkedPtr		*m_pNext;

	/// Previous element in the list
	mutable CLinkedPtr		*m_pPrev;

	/// Object pointer
	OBJ						*m_pObj;
};

template < class OBJ > class TListNode
{
	public:

		enum
		{
			eInsert = 1,

			eAppend = 2
		};

	public:

	TListNode()
	{	m_pPrev = NULL; m_pNext = stNULL; }

	TListNode(TListNode *pIt, stINT nOpt )
	{	m_pPrev = stNULL; m_pNext = stNULL; 
		if ( eAppend == nOpt ) Append( pIt ); else Insert( pIt );
	}

	TListNode( OBJ *pObj, TListNode *pIt, stINT nOpt ) : m_obj( pObj )
	{	m_pPrev = stNULL; m_pNext = stNULL;
		if ( pIt ) { if ( eAppend == nOpt ) Append( pIt ); else Insert( pIt ); }
	}

	TListNode* SetAt( TListNode *pPrev, TListNode *pNext )
	{	m_pPrev = pPrev; m_pNext = pNext; return this; }

	virtual ~TListNode() { Destroy(); }

	void Destroy()
	{	Remove(); m_obj.Destroy(); }

	CLinkedPtr< OBJ > Insert( TListNode *pIt )
	{
		if ( !pIt ) return *this;

		// Insert us
		m_pNext = pIt;
		m_pPrev = pIt->m_pPrev;
		pIt->m_pPrev = this;
		if ( m_pPrev ) m_pPrev->m_pNext = this;

		return *this;			
	}

	CLinkedPtr< OBJ > Append( TListNode *pIt )
	{
		if ( !pIt ) return *this;

		// Insert us
		m_pPrev = pIt;
		m_pNext = pIt->m_pNext;
		pIt->m_pNext = this;
		if ( m_pNext ) m_pNext->m_pPrev = this;

		return *this;			
	}
	
	CLinkedPtr< OBJ > Remove()
	{	if ( m_pPrev ) m_pPrev->m_pNext = m_pNext;
		if ( m_pNext ) m_pNext->m_pPrev = m_pPrev;
		m_pPrev = m_pNext = stNULL;
		return *this;
	}

	CLinkedPtr< OBJ > MoveUp()
	{	if ( !m_pPrev ) return *this;
		TListNode *pPrev = m_pPrev;
		Remove(); Insert( pPrev );
	}

	CLinkedPtr< OBJ > MoveDown()
	{	if ( !m_pNext ) return *this;
		TListNode *pNext = m_pNext;
		Remove(); Append( pNext );
	}

	/// Difference operator
	CLinkedPtr< OBJ > operator ->() { return m_pObj; }

	/// Cast operator
	operator CLinkedPtr< OBJ >*() { return m_obj; }

	/// Cast operator
	operator CLinkedPtr< OBJ >&() { return m_obj; }

	CLinkedPtr< OBJ >& Obj() { return m_obj; }

	/// Next node in the list
	TListNode* Next() { return m_pNext; }

	/// Previous node in the list
	TListNode* Prev() { return m_pPrev; }

private:

	TListNode			*m_pNext;

	TListNode			*m_pPrev;

	CLinkedPtr< OBJ >	m_obj;
};



//==================================================================
// TList
//
/// 
/**

  \code

	st::TList< int > lst;

	st::TList< int >::iterator it = 
		lst.Append( 4 );

	lst.Append( 8 );
	lst.Append( 16 );

	int a = it++;
	int b = it++;
	int c = it--;
	int d = it--;

  \endcode
	
*/
//==================================================================
template < class OBJ > class TList
{
public:

	class iterator
	{
	public:

		iterator() { m_pNode = stNULL; }

		iterator( TListNode< OBJ > &rNode )
		{	m_pNode = &rNode;
			m_obj = rNode.Obj();
		}

		iterator( TListNode< OBJ > *pNode )
		{	m_pNode = pNode;
			if ( pNode ) m_obj = pNode->Obj();
		}

		iterator( const iterator &it )
		{	m_pNode = it.m_pNode;
			m_obj = it.m_obj;
		}

		iterator( iterator &it )
		{	m_pNode = it.m_pNode;
			m_obj = it.m_obj;
		}

		iterator& operator = ( const iterator &it )
		{	m_pNode = it.m_pNode;
			m_obj = it.m_obj;
			return *this;
		}

		iterator& operator = ( iterator &it )
		{	m_pNode = it.m_pNode;
			m_obj = it.m_obj;
			return *this;
		}

		OBJ& operator = ( const OBJ &rObj )
		{	m_obj = rObj;
			return m_obj;
		}

		/// Returns non-zero if at the end of the list
		stBOOL End() { return ( !m_pNode  || !m_pNode->Next() ); }

		/// Returns non-zero if at the beginning of the list
		stBOOL Begin() { return ( m_pNode && !m_pNode->Prev() ); }

		/// Iterate forward
		OBJ& operator ++( int )
		{	if ( m_pNode && m_pNode->Next() )
			{	m_pNode = m_pNode->Next(); m_obj = m_pNode->Obj(); }
			return m_obj;
		}

		/// Iterate in reverse
		OBJ& operator --( int )
		{	if ( m_pNode && m_pNode->Prev() )
			{	m_pNode = m_pNode->Prev(); m_obj = m_pNode->Obj(); }
			return m_obj;
		}

		/// Returns a pointer to the object
		OBJ* Ptr() { return m_obj; }

		/// Returns object pointer
		operator OBJ*() { return m_obj; }

		/// Returns object pointer
		operator OBJ&() { return m_obj; }

		/// Difference operator
		OBJ* operator ->() { return m_obj; }

	private:

		TListNode< OBJ >			*m_pNode;

		CLinkedPtr< OBJ >			m_obj;

	};


public:

	TList()
	{	m_uSize = 0;
		m_pHead = m_pTail = stNULL; 
	}

	virtual ~TList() { Destroy(); }

	void Destroy()
	{
		m_uSize = 0;
		m_pTail = stNULL;
		while ( m_pHead )
		{	TListNode< OBJ > *pNode = m_pHead;
			m_pHead = pNode->Next();
			delete pNode;
		} // end while

	}

	iterator Insert( const OBJ *pObj = stNULL )
	{	TListNode< OBJ > *pNode = stNULL;
		try { pNode = new TListNode< OBJ >( (OBJ*)pObj, m_pHead, TListNode< OBJ >::eInsert ); }
		catch( ... ) { return iterator(); }
		if ( pNode ) { m_uSize++; m_pHead = pNode; if ( !m_pTail ) m_pTail = pNode; return *m_pHead; }
		return iterator();
	}

	iterator Insert( const OBJ &rObj )
	{	iterator itNew = Insert();
		if ( itNew.Ptr() ) itNew = rObj;
		return itNew;
	}

	iterator Append( const OBJ *pObj = stNULL )
	{	TListNode< OBJ > *pNode = stNULL;
		try { pNode = new TListNode< OBJ >( (OBJ*)pObj, m_pTail, TListNode< OBJ >::eAppend ); }
		catch( ... ) { return iterator(); }
		if ( pNode ) { m_uSize++; m_pTail = pNode; if ( !m_pHead ) m_pHead = pNode; return *m_pTail; }
		return iterator();
	}

	iterator Append( const OBJ &rObj )
	{	iterator itNew = Append();
		if ( itNew.Ptr() ) itNew = rObj;
		return itNew;
	}

	const iterator First() { return iterator( m_pHead ); }

	const iterator Last() { return iterator( m_pTail ); }

	stUINT Size() { return m_uSize; }

private:

	stUINT					m_uSize;

	TListNode< OBJ >		*m_pHead;

	TListNode< OBJ >		*m_pTail;

};


template < class KEY, class OBJ > class TAssoList
{
public:	

	class iterator
	{	
	public:

		int operator == ( iterator &it ) { return this->p == it.p; }

		//==============================================================
		// operator T*()
		//==============================================================
		/// Returns a pointer to the list element object
		operator OBJ*() { return p; }

		//==============================================================
		// operator ->()
		//==============================================================
		/// Differences to ourself
		OBJ* operator ->() { return p; }


	private:

		/// Pointer to the list element object
		OBJ		*p;

	};

public:

	TAssoList() {}

	virtual ~TAssoList() {}

	// +++
	OBJ& operator [] ( KEY key ) { return *new OBJ; }

	iterator find( KEY &key ) { return m_it; }

	iterator end() { return m_it; }

public:

	void			*m_pList;

	// +++ Remove
	iterator		m_it;

};
