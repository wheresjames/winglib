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
		
		CSqPos( const oexPoint &pt ) { m_x = pt.x; m_y = pt.y; }

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

	class CSqColor
	{
	public:

		_SQBIND_CLASS_CTOR_BEGIN( CSqColor )
			_SQBIND_CLASS_CTOR( CSqColor, 3 ) ( sa.GetInt( 2 ), sa.GetInt( 3 ), sa.GetInt( 4 ), 255 )
			_SQBIND_CLASS_CTOR( CSqColor, 4 ) ( sa.GetInt( 2 ), sa.GetInt( 3 ), sa.GetInt( 4 ), sa.GetInt( 5 ) )
		_SQBIND_CLASS_CTOR_END( CSqColor )

		static void Register( sqbind::VM vm );

		/// Default constructor
		CSqColor() { m_c[ 0 ] = m_c[ 1 ] = m_c[ 2 ] = m_c[ 3 ] = 0; }
		CSqColor( int r, int g, int b, int a ) { m_c[ 0 ] = r; m_c[ 1 ] = g; m_c[ 2 ] = b; m_c[ 3 ] = a; }
		~CSqColor() {}

		// Copy semantics
		CSqColor( const CSqColor &r ) { m_c[ 0 ] = r.m_c[ 1 ]; m_c[ 1 ] = r.m_c[ 2 ]; m_c[ 2 ] = r.m_c[ 2 ]; m_c[ 3 ] = r.m_c[ 3 ]; }
		CSqColor& operator =( const CSqColor &r ) { m_c[ 0 ] = r.m_c[ 1 ]; m_c[ 1 ] = r.m_c[ 2 ]; m_c[ 2 ] = r.m_c[ 2 ]; m_c[ 3 ] = r.m_c[ 3 ]; return *this; }

		void set( int r, int g, int b, int a ) { m_c[ 0 ] = r; m_c[ 1 ] = g; m_c[ 2 ] = b; m_c[ 3 ] = a; }

		int r() { return  m_c[ 0 ]; }
		void setR( int v ) { m_c[ 0 ] = v; }
		int g() { return  m_c[ 1 ]; }
		void setG( int v ) { m_c[ 1 ] = v; }
		int b() { return  m_c[ 2 ]; }
		void setB( int v ) { m_c[ 2 ] = v; }
		int a() { return  m_c[ 3 ]; }
		void setA( int v ) { m_c[ 3 ] = v; }

		int* Ptr() { return m_c; }

	private:

		/// Color components
		int m_c[ 4 ];
	};

	class CSq3dVector
	{

	public:

		static void Register( sqbind::VM vm );

		SQBIND_CLASS_CTOR_BEGIN( CSq3dVector )
			_SQBIND_CLASS_CTOR( CSq3dVector, 1 ) ( sa.GetInt( 2 ) )
			_SQBIND_CLASS_CTOR( CSq3dVector, 2 ) ( sa.GetInt( 2 ), sa.GetInt( 3 ), sa.GetInt( 4 ) )
		SQBIND_CLASS_CTOR_END( CSq3dVector )

		CSq3dVector() { m_x = 0; m_y = 0; m_z = 0; }

		CSq3dVector( int n ) { m_x = m_y = m_z = n; }

		CSq3dVector( int x, int y, int z ) { m_x = x; m_y = y; m_z = z; }

		// Copy semantics
		CSq3dVector( const CSq3dVector &r ) { m_x = r.m_x; m_y = r.m_y; m_z = r.m_z; }
		CSq3dVector& operator=( const CSq3dVector &r ) { m_x = r.m_x; m_y = r.m_y; m_z = r.m_z; return *this; }

		void set( int x, int y ) { m_x = x; m_y = y; }

		int getX() { return m_x; }

		void setX( int x ) { m_x = x; }

		int getY() { return m_y; }

		void setY( int y ) { m_y = y; }

		int getZ() { return m_z; }

		void setZ( int z ) { m_z = z; }
		
	private:

		int		m_x;

		int		m_y;

		int		m_z;
	};

	class CSq3dVectord
	{

	public:

		static void Register( sqbind::VM vm );

		SQBIND_CLASS_CTOR_BEGIN( CSq3dVectord )
			_SQBIND_CLASS_CTOR( CSq3dVectord, 1 ) ( sa.GetFloat( 2 ) )
			_SQBIND_CLASS_CTOR( CSq3dVectord, 2 ) ( sa.GetFloat( 2 ), sa.GetFloat( 3 ), sa.GetFloat( 4 ) )
		SQBIND_CLASS_CTOR_END( CSq3dVectord )

		CSq3dVectord() { m_x = 0; m_y = 0; m_z = 0; }

		CSq3dVectord( double n ) { m_x = m_y = m_z = n; }

		CSq3dVectord( double x, double y, double z ) { m_x = x; m_y = y; m_z = z; }

		// Copy semantics
		CSq3dVectord( const CSq3dVectord &r ) { m_x = r.m_x; m_y = r.m_y; m_z = r.m_z; }
		CSq3dVectord& operator=( const CSq3dVectord &r ) { m_x = r.m_x; m_y = r.m_y; m_z = r.m_z; return *this; }

		void set( double x, double y ) { m_x = x; m_y = y; }

		double getX() { return m_x; }

		void setX( double x ) { m_x = x; }

		double getY() { return m_y; }

		void setY( double y ) { m_y = y; }

		double getZ() { return m_z; }

		void setZ( double z ) { m_z = z; }
		
	private:

		double		m_x;

		double		m_y;

		double		m_z;
	};


}; // end namespace

// Declare type for use as squirrel parameters
SQBIND_DECLARE_INSTANCE( sqbind::CSqSize, CSqSize )
SQBIND_DECLARE_INSTANCE( sqbind::CSqPos, CSqPos )
SQBIND_DECLARE_INSTANCE( sqbind::CSqColor, CSqColor )
SQBIND_DECLARE_INSTANCE( sqbind::CSq3dVector, CSq3dVector )
SQBIND_DECLARE_INSTANCE( sqbind::CSq3dVectord, CSq3dVectord )
