/*------------------------------------------------------------------
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
	/// Squirrel map adaptor class
	class CSqPolygon
	{
	public:
			
		/// Point structure
		struct CSqPoint
		{	long		x;
			long		y;
		};
		
		struct CSqRange
		{	CSqPoint	min;
			CSqPoint	max;		
		};
	
	
	public:

		SQBIND_CLASS_CTOR_BEGIN( CSqPolygon )
		SQBIND_CLASS_CTOR_END( CSqPolygon )

		/// Destructor
		virtual ~CSqPolygon();

		/// Default constructor
		CSqPolygon();
		
		/// Register interface to VM
		static void Register( sqbind::VM vm );

		/// Copy
		CSqPolygon( const CSqPolygon &r ) { AddPoints( &r ); }
		CSqPolygon& operator = ( const CSqPolygon &r ) { CopyConst( &r ); return *this; }		
		
	/** \addtogroup COcvObjects
		@{
	*/
	
		/// Releases polygon resources
		void Destroy();

		/// Add a point to the polygon
		int AddPoint( long x, long y );
		
		/// Returns the number of points in the polygon
		long getNumPoints() { return m_lPts; }
		long getNumPointsConst() const { return m_lPts; }

		/// Returns the number of edges the polygon has
		long getNumEdges() { return ( 1 < m_lPts ) ? ( m_lPts - 1 ) : 0; }
		long getNumEdgesConst() const { return ( 1 < m_lPts ) ? ( m_lPts - 1 ) : 0; }
		
		/// Sorts points
		int ScanSort();
		
		/// Calculates a convex hull of the polygon
		/**
			After calling this function, the polygon will contain a 
			convex hull of the original polygon.  This calculates an
			exact convex hull.

			\warning	Requires sorting, call ScanSort() before this function
						unless you know the points are sorted.

			Copyright notice for this function:

			Based on "Andrew's Monotone Chain algorithm"
			Copyright 2001, softSurfer (www.softsurfer.com)
			This code may be freely used and modified for any purpose
			providing that this copyright notice is included with it.
			SoftSurfer makes no warranty for this code, and cannot be held
			liable for any real or imagined damage resulting from its use.
			Users of this code must verify correctness for their application.
			
			\return Non-zero if success
		
			\see 
		*/
		int ConvexHull();

		/// Calculates an approximate convex hull 
		/** 
			After this call, this class will contain the approximate
			convex hull of the original points.  

			Similar to the BFP Approximate Hull Algorithm
			( Bentley-Faust-Preparata, 1982 )
			but slightly faster

			\warning	This does not calculate an <b>exact</b> convex hull.
						For a precise convex hull, call ConvexHull()

			\return Non-zero if success
		
			\see 
		*/
		int ApxConvexHull();
		
		/// Calculates simple polygon area
		float SimpleArea();
		
		/// Calculates convex polygon area
		float ConvexArea();
		
		/// Returns non-zero if the polygon intersects
		int Intersect( CSqPolygon *pPoly )
		{	return IntersectPts( pPoly->getPoints(), pPoly->getNumPoints() ); }
		
		/// Calculates the minimum distance between polygon points
		/**
			\param [in] pWp - CWinPolygon object
			
			This function calculates the minimum distance between to
			points in the polygons.

			\warning	This function currently makes only a rough 
						estimation of distance.

			\return The distance between the closest points.
		
			\see 
		*/
		long Distance( CSqPolygon *pPoly );
	
		/// Copies the specifid polygon
		long Copy( CSqPolygon *pPoly )
		{	Destroy(); AddPoints( pPoly ); return getNumPoints(); }	
		
		/// Returns point list
		long getPointList( CSqMulti *pPoints );
		
	/** @} */
	
		long CopyConst( const CSqPolygon *pPoly )
		{	Destroy(); AddPoints( pPoly ); return getNumPoints(); }	
		
		/// Returns a pointer to point array
		CSqPoint* getPoints() { return m_pts.Ptr(); }
		
		/// Returns constant pointer to array
		const CSqPoint* getPointsConst() const { return m_pts.c_Ptr(); }
				
		/**
			\param [in] P0	-	First point in triangle
			\param [in] P1	-	Second point in triangle
			\param [in] P2	-	Third point in triangle				 		

			\return greater than zero if left handed triangle is specified
		
			\see 
		*/
		inline long isLeft( const CSqPoint &P0, const CSqPoint &P1, const CSqPoint &P2 )
		{	return ( P1.x - P0.x ) * ( P2.y - P0.y ) - ( P2.x - P0.x ) * ( P1.y - P0.y ); }
		
		/// Calculates the area of a triangle given the lengths of it's sides
		/**
			\param [in] a	-	First side length
			\param [in] b	-	Second side length
			\param [in] c	-	Third side length
			
			\return Area of the triangle
		
			\see 
		*/
		static float TriangleArea( float a, float b, float c );

		/// Calculates the area of the triangle defined by three points.
		/**
			\param [in] x1	-	Horizontal offset first point
			\param [in] y1	-	Vertical offset first point
			\param [in] x2	-	Horizontal offset second point
			\param [in] y2	-	Vertical offset second point
			\param [in] x3	-	Horizontal offset third point
			\param [in] y3	-	Vertical offset third point
			
			\return Area of the specified triangle
		
			\see 
		*/
		static float TriangleArea( float x1, float y1, float x2, float y2, float x3, float y3 );		
		
		/// Appends points to the end of the polygon point list
		/**
			\param [in] ppt		-	List of points
			\param [in] dwPts	-	Number of points in ppt
			
			\return Non-zero if success
		
			\see 
		*/
		int AddPoints( const CSqPoint *ppt, long lPts )
		{	int bRet = 1; 
			for ( long i = 0; i < lPts; i++ ) 
				if ( !AddPoint( ppt[ i ].x, ppt[ i ].y ) ) 
					bRet = 0;
			return bRet;
		}

		/// Appends points to the end of the polygon point list
		/**
			\param [in] ppt		-	List of points
			\param [in] dwPts	-	Number of points in ppt
			
			\return Non-zero if success
		
			\see 
		*/
		int AddPoints( const CSqPolygon &poly )
		{	return AddPoints( poly.getPointsConst(), poly.getNumPointsConst() ); 
		}

		/// Appends points to the end of the polygon point list
		/**
			\param [in] ppt		-	List of points
			\param [in] dwPts	-	Number of points in ppt
			
			\return Non-zero if success
		
			\see 
		*/
		int AddPoints( const CSqPolygon *pPoly )
		{	if ( !pPoly ) return 0;
			return AddPoints( pPoly->getPointsConst(), pPoly->getNumPointsConst() ); 
		}

		/// For sorting points
		/**
			\param [in] p1	-	First point
			\param [in] p2	-	Second point

			-1	if p1 is before p2
			0 	if points are the same
			1	if p2 is before p1
			
			\return 
		
			\see 
		*/
		inline long ComparePoint( const CSqPoint &p1, const CSqPoint &p2 )
		{	
			if ( p1.x > p2.x ) 
				return 1; 
			
			else if ( p1.x == p2.x ) 
			{ 	
				if ( p1.y > p2.y ) 
					return 1; 
					
				else if ( p1.y == p2.y ) 
					return 0;
					
			} // end else if
			
			return -1;
		}		
		
		void rScanSort( CSqPoint *pPts, long left, long right );
		
		/// Returns non-zero if point is in polygon
		int PtInPolygon( const CSqPoint *pptList, long lListSize, const CSqPoint *ppt );
	
		/// Returns non-zero if the specified polygon intersects
		int IntersectPts( const CSqPoint *pPts, long lPts );

		/// Returns specified points x coord
		int getPointX( int i ) { return ( i >= m_lPts ) ? 0 : m_pts[ i ].x; }
		
		/// Returns specified points y coord
		int getPointY( int i ) { return ( i >= m_lPts ) ? 0 : m_pts[ i ].y; }
		
		/// Returns point information
		int getPointInfo( CSqMulti *pPoints );
		
	public:
	
		/// Number of points in the polygon
		long						m_lPts;
		
		/// Array of points
		oex::TMem< CSqPoint >		m_pts;
		
	};

}; // end namespace

// Declare type for use as squirrel parameters
SQBIND_DECLARE_INSTANCE( sqbind::CSqPolygon, CSqPolygon )

