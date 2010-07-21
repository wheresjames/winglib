/*------------------------------------------------------------------
// sq_generic.h
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

// namespace
namespace sqbind
{
	class CSqSize
	{
	public:

		static void Register( sqbind::VM vm );

		SQBIND_CLASS_CTOR_BEGIN( CSqSize )
			_SQBIND_CLASS_CTOR( CSqSize, 1 ) ( sa.GetInt( 2 ) )
			_SQBIND_CLASS_CTOR( CSqSize, 2 ) ( sa.GetInt( 2 ), sa.GetInt( 3 ) )
		SQBIND_CLASS_CTOR_END( CSqSize )

		CSqSize() { m_x = 0; m_y = 0; }

		CSqSize( int n ) { m_x = m_y = n; }

		CSqSize( int x, int y ) { m_x = x; m_y = y; }

		// Copy semantics
		CSqSize( const CSqSize &r ) { m_x = r.m_x; m_y = r.m_y; }
		CSqSize& operator=( const CSqSize &r ) { m_x = r.m_x; m_y = r.m_y; return *this; }

		void set( int x, int y ) { m_x = x; m_y = y; }

		int getX() { return m_x; }

		void setX( int x ) { m_x = x; }

		int getY() { return m_y; }

		void setY( int y ) { m_y = y; }

	private:

		int		m_x;

		int		m_y;

	};

	class CSqPos
	{
	public:

		static void Register( sqbind::VM vm );

		SQBIND_CLASS_CTOR_BEGIN( CSqPos )
			_SQBIND_CLASS_CTOR( CSqPos, 1 ) ( sa.GetInt( 2 ) )
			_SQBIND_CLASS_CTOR( CSqPos, 2 ) ( sa.GetInt( 2 ), sa.GetInt( 3 ) )
		SQBIND_CLASS_CTOR_END( CSqPos )

		CSqPos() { m_x = 0; m_y = 0; }

		CSqPos( int n ) { m_x = m_y = n; }

		CSqPos( int x, int y ) { m_x = x; m_y = y; }

		// Copy semantics
		CSqPos( const CSqPos &r ) { m_x = r.m_x; m_y = r.m_y; }
		CSqPos& operator=( const CSqPos &r ) { m_x = r.m_x; m_y = r.m_y; return *this; }

		void set( int x, int y ) { m_x = x; m_y = y; }

		int getX() { return m_x; }

		void setX( int x ) { m_x = x; }

		int getY() { return m_y; }

		void setY( int y ) { m_y = y; }

	private:

		int		m_x;

		int		m_y;

	};

}; // end namespace

// Declare type for use as squirrel parameters
SQBIND_DECLARE_INSTANCE( sqbind::CSqSize, CSqSize )
SQBIND_DECLARE_INSTANCE( sqbind::CSqPos, CSqPos )
