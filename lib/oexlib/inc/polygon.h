/*------------------------------------------------------------------
// virtual_fs.h
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
// CPolygon
//
/// Encapsulates a polygon
/**
	This structure encapsulates a polylgon and provides a few 
	nifty functions for manipulating them.  The polygon is stored
	simply as a list of points.	
*/
//==================================================================
class CPolygon  
{
public:

	/// Position type
	typedef oexLONG		t_pos;

	/// Size type
	typedef oexULONG	t_size;

	/// Floating point type
	typedef oexFLOAT	t_float;

	/// Floating point type
	typedef oexDOUBLE	t_double;

	/// Holds information defining a line
	struct SPoint
	{
		t_pos		x;
		t_pos		y;
	};

	/// Holds information defining a line
	struct SLine
	{
		SPoint		a;
		SPoint		b;

		t_float getLength()
		{	t_float dx = b.x - a.x, dy = b.y - a.y;
			return (t_float)os::math::sqrt( dx * dx + dy * dy );
		}

	};

	/// Holds information defining a rectangle
	struct SRect
	{
		t_pos		left;
		t_pos		top;
		t_pos		right;
		t_pos		bottom;

		t_pos getWidth() { return right - left; }
		t_pos getHeight() { return bottom - top; }
	};

	/// 2D range
	struct SRange
	{
		SPoint		min;
		SPoint		max;

		t_float getRange()
		{	t_float dx = max.x - min.x, dy = max.y - min.y;
			return (t_float)os::math::sqrt( dx * dx + dy * dy );
		}
	};

	/// Point type
	typedef SPoint		t_point;

	/// Line type
	typedef SLine		t_line;

	/// Rect type
	typedef SRect		t_rect;

public:


	//==============================================================
	// Distance()
	//==============================================================
	/// Calculates the minimum distance between polygon points
	/**
		\param [in] x_pWp - CPolygon object
		
		This function calculates the minimum distance between to
		points in the polygons.

		\warning	This function currently makes only a rough 
					estimation of distance.

		\return The distance between the closest points.
	
		\see 
	*/
	t_pos Distance( CPolygon *x_pWp );

	//==============================================================
	// GetIBeam()
	//==============================================================
	/// Returns points representing the vertical height of the polygon.
	/**
		\param [out] x_ptTop		-	The top point of the line
		\param [out] x_ptBottom		-	Bottom point of the line
		\param [in]  x_bInvY		-	Non-zero to flip the line upside
										down.
		
		This function basically creates a line that cuts the polygon
		in half vertcally.  The line is as long as the polygon is high,
		and is located roughly at the horizontal center.

		I know this is a strange function, but I needed it OK?

		\return Non-zero if success
	
		\see 
	*/
	oexBOOL GetIBeam( t_point *x_ptTop, t_point *x_ptBottom, oexBOOL x_bInvY = oexFALSE );

	//==============================================================
	// Scale()
	//==============================================================
	/// Scales the polygon points according to the ratio of specified rectangles
	/**
		\param [in] x_pSrc		-	Source rect 
		\param [in] x_pDst		-	Destination rect

		Calculates the vertical and horizontal difference and scales the
		polygon accordingly.  Great for scaling a polygon for overlaying
		on one image to the same image of a different size.
		
		\return Non-zero if success
	
		\see 
	*/
	oexBOOL Scale( t_rect *x_pSrc, t_rect *x_pDst );

	//==============================================================
	// Scale()
	//==============================================================
	/// Scales the point array according to the ratio of specified rectangles
	/**
		\param [in] x_psrcPts	-	Source point array
		\param [in] x_pdstPts	-	Destination point array
		\param [in] x_uPts		-	Number of points in x_psrcPts and x_pdstPts
		\param [in] x_pSrc		-	Source rect 
		\param [in] x_pDst		-	Destination rect

		Calculates the vertical and horizontal difference and scales the
		polygon accordingly.  Great for scaling a polygon for overlaying
		on one image to the same image of a different size.
		
		\return Non-zero if success
	
		\see 
	*/
	oexBOOL ScalePoints( t_point *x_psrcPts, t_point *x_pdstPts, t_size x_uPts, t_rect *x_pSrc, t_rect *x_pDst );


	//==============================================================
	// GetBoundingRect()
	//==============================================================
	/// Returns the bounding rectangle
	/**
		\param [in] x_pRect		-	Receives bounding rect

		\return Non-zero if success
	
		\see 
	*/
	oexBOOL GetBoundingRect( t_rect *x_pRect );

	//==============================================================
	// Intersect()
	//==============================================================
	/// Collision tests with another polygon
	/**
		\param [in] x_pPts	-	Points in polygon for collision test
		\param [in] x_uPts	-	Number of points in pPts
  		
		Unfortunately, this is not a very optimized function.  But
		it works well.

		\return Non-zero if polygons intersect
	
		\see 
	*/
	oexBOOL Intersect( t_point *x_pPts, t_size x_uPts );

	//==============================================================
	// Intersect()
	//==============================================================
	/// Collision tests with another polygon
	/**
		\param [in] x_pWp		-	CPolygon object to collision test
		
		\return Non-zero if polygons intersect
	
		\see 
	*/
	oexBOOL Intersect( CPolygon *x_pWp )
	{	return Intersect( x_pWp->getPoints(), x_pWp->getNumPoints() ); }

	//==============================================================
	// rScanSort()
	//==============================================================
	/// Performs a recursive quick sort on the polygons
	/**
		\param [in] x_pPts		-	Points to sort
		\param [in] x_left		-	Left index
		\param [in] x_right		-	Right index

		Call ScanSort().  Don't call this function directly
	*/
	void rScanSort( t_point *x_pPts, t_size x_left, t_size x_right );

	//==============================================================
	// ScanSort()
	//==============================================================
	/// Performs a recursive quick sort on the polygons
	/**
		Reasonably fast sorting algorithm.
		
		\return Non-zero if success
	
		\see 
	*/
	oexBOOL ScanSort();

	//==============================================================
	// Inflate()
	//==============================================================
	/// Inflates the polygon by the specified amount
	/**
		\param [in] x_x	-	Horizontal inflate value
		\param [in] x_y	-	Vertical inflate value
		
		\return Non-zero if success
	
		\see 
	*/
	oexBOOL Inflate( t_pos x_x, t_pos x_y );

	//==============================================================
	// SimpleArea()
	//==============================================================
	/// Calculates a rough estimate of the polygon area.
	/**
		
		\return value representing the rough polygon area.
	
		\see 
	*/
	t_float SimpleArea();

	//==============================================================
	// ConvexHull()
	//==============================================================
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
	oexBOOL ConvexHull();

	//==============================================================
	// ApxConvexHull()
	//==============================================================
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
	oexBOOL ApxConvexHull();

	//==============================================================
	// FromMap()
	//==============================================================
	/// Creates a polygon from a map
	/**
		\param [in] x_map	-	pointer to map values
		\param [in] x_x		-	Horizontal offset for the extraction
		\param [in] x_y		-	Vertical offset for the extraction
		\param [in] x_w		-	Width of the image area for the extraction
		\param [in] x_h		-	Height of the image area for the extraction
		\param [in] x_th	-	Value threshold
		\param [in] x_buf	-	Pointer to buffer that holds map walk data.
								You can speed up multiple calls to this
								function by allocating this ahead of time,
								then passing it into this function.
		
		This function can create a polygon by 'filling' around a
		bitmap image.  Great for selcting parts of an image.

		\return Non-zero if success
	
		\see 
	*/
	oexBOOL FromMap( t_size *x_map, t_pos x_x, t_pos x_y, t_pos x_w, t_pos x_h, t_size x_th, oexUCHAR *x_buf = oexNULL );

	/// Default constructor
	CPolygon() 
	{ 
		m_uPts = 0; 
	}

	//==============================================================
	// CPolygon()
	//==============================================================
	/// Constructs a polygon object from a list of points
	/**
		\param [in] x_pPts	-	Pointer to POINT structure array
		\param [in] x_uPts	-	Number of points in pPts
	*/
	CPolygon( t_point *x_pPts, t_size x_uPts )
	{	m_uPts = 0; setPoints( x_pPts, x_uPts ); }

	//==============================================================
	// CPolygon()
	//==============================================================
	/// Constructs a polygon object by copying another CPolygon object
	/**
		\param [in] x_wp	-	Pointer to CPolygon object
	*/
	CPolygon( const CPolygon &x_wp )
	{	m_uPts = 0; 
		setPoints( ((CPolygon&)x_wp).getPoints(), x_wp.getNumPoints() ); 
	}

	/// Destructor
	virtual ~CPolygon();

	//==============================================================
	// Destroy()
	//==============================================================
	/// Releases the polygon resources
	void Destroy() { m_uPts = 0; m_pts.Destroy(); }

	//==============================================================
	// Allocate()
	//==============================================================
	/// Allocates the specified number of points in the polygon
	/**
		\param [in] sz	-	Number of points to allocate
		
		\return Non-zero if success
	
		\see 
	*/
	oexBOOL Allocate( t_size x_sz ) 
	{	if ( !m_pts.New( x_sz ).Ptr() ) 
			return oexFALSE; 
		m_pts.Zero(); 
		return oexTRUE; 
	}

	//==============================================================
	// getPoints()
	//==============================================================
	/// Returns a pointer to the polygon POINT structure array
	t_point* getPoints() { return m_pts.Ptr(); }

	//==============================================================
	// getNumPoints()
	//==============================================================
	/// Returns the total number of points in the polygon
	t_size getNumPoints() const { return m_uPts; }

	//==============================================================
	// getNumEdges()
	//==============================================================
	/// Returns the number of edges the polygon has
	t_size getNumEdges() 
	{	t_size pts = getNumPoints(); 
		if ( pts ) 
			return pts - 1; 
		return 0; 
	}

	//==============================================================
	// getEdge()
	//==============================================================
	/// Returns a line struture pointer for the specified edge
	/**
		\param [in] i	-	Edge index
		
		This function returns an offset into the point array of the
		specified edge.  It does not allocate any new memory

		\return Pointer to the specified edge
	
		\see 
	*/
	t_line* getEdge( t_size i ) 
	{	if ( !m_uPts || i >= ( m_uPts - 1 ) )
			return oexNULL; 
		return (t_line*)m_pts.Ptr( i ); 
	}

	//==============================================================
	// PtInPolygon()
	//==============================================================
	/// Tests to see if the point lies within the polygon
	/**
		\param [in] x_ppt		-	Point to test

		This function works well and is reasonably fast.
		
		\return Non-zero if point is within the polygon
	
		\see 
	*/
	oexBOOL PtInPolygon( t_point *x_ppt ) 
	{	return PtInPolygon( getPoints(), getNumPoints(), x_ppt ); }

	//==============================================================
	// PtInPolygon()
	//==============================================================
	/// Tests to see if the point lies within the polygon
	/**
		\param [in] x_pptList		-	Polygon point list
		\param [in] x_uListSize		-	Number of points in pptList
		\param [in] x_ppt			-	Point to test
		
		This function works well and is reasonably fast.
		
		\return Non-zero if point is within the polygon
	
		\see 
	*/
	static oexBOOL PtInPolygon( t_point *x_pptList, t_size x_uListSize, t_point *x_ppt );

	//==============================================================
	// CombinePoint()
	//==============================================================
	/// Adds a point into the current polygon by calculating the closest edge
	/**
		\param [in] x_ppt	-	Point to add
		
		Magic function

		\warning I haven't implemented this function yet!

		The idea is just to calculate the closest edge to the polygon and
		conjoin the point into the polygon at that edge.

		\return Non-zero if success
	
		\see 
	*/
	oexBOOL CombinePoint( t_point *x_ppt ) 
	{	return CombinePoint( getPoints(), getNumPoints(), x_ppt ); }

	//==============================================================
	// CombinePoint()
	//==============================================================
	/// Adds a point into the current polygon by calculating the closest edge
	/**
		\param [in] x_pptList			-	Pointer to POINT structure array
		\param [in] x_dwListSize		-	Number of points in pptList
		\param [in] x_ppt				-	Point to add
		
		Magic function

		\warning I haven't implemented this function yet!

		The idea is just to calculate the closest edge to the polygon and
		conjoin the point into the polygon at that edge.

		\return Non-zero if success
	
		\see 
	*/
	static oexBOOL CombinePoint( t_point *x_pptList, t_size x_uListSize, t_point *x_ppt );

	//==============================================================
	// operator += ()
	//==============================================================
	/// Appends point to the end of the polygon
	/**
		\param [in] ppt		-	Point to add
		
		\return Reference to this object
	
		\see 
	*/
	CPolygon& operator += ( t_point *x_ppt ) { AddPoint( x_ppt ); return *this; }

	//==============================================================
	// AddPoint()
	//==============================================================
	/// Appends point to the end of the polygon
	/**
		\param [in] x_ppt		-	Point to add
		
		\return Non-zero if added
	
		\see 
	*/
	oexBOOL AddPoint( t_point *x_ppt ) 
	{	if ( !x_ppt ) return oexFALSE; 
		return AddPoint( x_ppt->x, x_ppt->y ); 
	}

	//==============================================================
	// AddPoints()
	//==============================================================
	/// Appends points to the end of the polygon point list
	/**
		\param [in] x_ppt		-	List of points
		\param [in] x_uPts	-	Number of points in ppt
		
		\return Non-zero if success
	
		\see 
	*/
	oexBOOL AddPoints( t_point *x_ppt, t_size x_uPts )
	{	oexBOOL bRet = oexTRUE; 
		for ( t_size i = 0; i < x_uPts; i++ ) 
			if ( !AddPoint( &x_ppt[ i ] ) ) 
				bRet = oexFALSE;
		return bRet;
	}

	//==============================================================
	// AddPoint()
	//==============================================================
	/// Appends point to the end of the polygon
	/**
		\param [in] x_x	-	Horizontal offset of point
		\param [in] x_y	-	Vertical offset of point
		
		\return Non-zero if success
	
		\see 
	*/
	oexBOOL AddPoint( t_pos x_x, t_pos x_y )
	{	if ( !m_pts.OexResize( m_uPts + 1 ).Ptr() ) 
			return oexFALSE;
		m_pts[ m_uPts ].x = x_x; 
		m_pts[ m_uPts ].y = x_y;
		m_uPts++;
		return oexTRUE;
	}

	//==============================================================
	// AddPolygon()
	//==============================================================
	/// Add The points from the specified polygon
	/**
		\param [in] pWp		-	Polygon containing points to add
		
		\return Non-zero if success
	
		\see 
	*/
	oexBOOL AddPolygon( CPolygon *pWp )
	{	return AddPoints( pWp->getPoints(), pWp->getNumPoints() ); }

	//==============================================================
	// RemovePoint()
	//==============================================================
	/// Removes the specified point from the polygon
	/**
		\param [in] x_uPt	-	Index of point to remove
		
		\return Non-zero if success
	
		\see 
	*/
	oexBOOL RemovePoint( t_size x_uPt )
	{	if ( !m_uPts || x_uPt >= m_uPts ) 
			return oexFALSE;
		for ( t_size i = x_uPt + 1; i < m_uPts; i++ )
			m_pts[ i - 1 ] = m_pts[ i ];
		m_uPts--;
		return oexTRUE;
	}

	//==============================================================
	// RemovePoint()
	//==============================================================
	/// Removes the specified point from the polygon
	/**
		\param [in] x_x	-	Horizontal offset of point
		\param [in] x_y	-	Vertical offset of point

		Searches the polygon for a point matching the specified x, y
		position and removes the first occurence.
	  
		\return Non-zero if success
	
		\see 
	*/
	oexBOOL RemovePoint( t_pos x_x, t_pos x_y )
	{	for ( t_size i = 0; i < m_uPts; i++ )
			if ( m_pts[ i ].x == x_x && m_pts[ i ].y == x_y )
				return RemovePoint( i );
		return oexFALSE;
	}

	//==============================================================
	// operator DWORD()
	//==============================================================
	/// Returns the number of points in the polygon
	operator t_size() { return m_uPts; }

	//==============================================================
	// operator LPPOINT()
	//==============================================================
	/// Returns a pointer to the polygon POINT structure array
	operator t_point*() { return m_pts.Ptr(); }


	//==============================================================
	// operator []()
	//==============================================================
	/// Returns a reference to the specified point
	t_point& operator []( t_size i ) {	return m_pts[ i ]; }

	//==============================================================
	// isLeft()
	//==============================================================
	/// Returns greater than zero if the specified triangle is left-handed
	/**
		\param [in] P0	-	First point in triangle
		\param [in] P1	-	Second point in triangle
		\param [in] P2	-	Third point in triangle				 		

		\return greater than zero if left handed triangle is specified
	
		\see 
	*/
	inline t_pos isLeft( t_point P0, t_point P1, t_point P2 )
	{	return ( P1.x - P0.x ) * ( P2.y - P0.y ) - ( P2.x - P0.x ) * ( P1.y - P0.y ); }

	//==============================================================
	// ComparePoint()
	//==============================================================
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
	inline t_pos ComparePoint( t_point p1, t_point p2 )
	{	if ( p1.x > p2.x ) return 1; 
		else if ( p1.x == p2.x ) 
		{ if ( p1.y > p2.y ) return 1; else if ( p1.y == p2.y ) return 0; }
		return -1;
	}		

	//==============================================================
	// CalculateCenter()
	//==============================================================
	/// Calculates the center point of the polygon
	/**
		\return Center point of polygon
	
		\see 
	*/
	t_point CalculateCenter();


	//==============================================================
	// operator =()
	//==============================================================
	/// Copies another CPolygon object
	CPolygon& operator = ( CPolygon &x_wp )
	{	setPoints( &x_wp ); return *this;
	}

	//==============================================================
	// setPoints()
	//==============================================================
	/// Sets the value of the polygon points
	/**
		\param [in] x_pPts	-	Pointer to POINT array
		\param [in] x_uPts	-	Number of points in pPts
		
		\return Non-zero if success
	
		\see 
	*/
	oexBOOL setPoints( t_point *x_pPts, t_size x_uPts )
	{	if ( !m_pts.New( x_uPts ).Ptr() ) return oexFALSE;
		m_pts.MemCpy( x_pPts, x_uPts ); m_uPts = x_uPts;
		return oexTRUE;
	}

	//==============================================================
	// setPoints()
	//==============================================================
	/// Sets the value of the polygon points
	/**
		\param [in] pWp		-	Polygon containing point values
		
		\return Non-zero if success
	
		\see 
	*/
	oexBOOL setPoints( CPolygon *pWp )
	{	return setPoints( pWp->getPoints(), pWp->getNumPoints() ); }

		
	//==============================================================
	// Offset()
	//==============================================================
	/// Offsets the polygon by the specified distance
	/**
		\param [in] ppt		-	Point specifying offset distance
	*/
	void Offset( t_point *ppt ) { Offset( ppt->x, ppt->y ); }

	//==============================================================
	// Offset()
	//==============================================================
	/// Offsets the polygon by the specified distance
	/**
		\param [in] x	-	Horizontal offset distance
		\param [in] y	-	Vertical offset distance
	*/
	void Offset( t_pos x, t_pos y )
	{	for ( t_size i = 0; i < m_uPts; i++ )
			m_pts[ i ].x += x, m_pts[ i ].y += y;
	}

	//==============================================================
	// ConvexArea()
	//==============================================================
	/// Calculates the area of a convex polygon
	/**

		The polygon in the object must be convex or this function
		returns an undefined value!
		
		\return Area
	
		\see 
	*/
	t_float ConvexArea();

	//==============================================================
	// TriangleArea()
	//==============================================================
	/// Calculates the area of a triangle given the lengths of it's sides
	/**
		\param [in] a	-	First side length
		\param [in] b	-	Second side length
		\param [in] c	-	Third side length
		
		\return Area of the triangle
	
		\see 
	*/
	static t_float TriangleArea( t_float a, t_float b, t_float c )
	{	t_float s = ( a + b + c ) / (t_float)2;
		return os::math::sqrt( s * ( s - a ) * ( s - b ) * ( s - c ) );
	}

	//==============================================================
	// TriangleArea()
	//==============================================================
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
	static t_float TriangleArea( t_float x1, t_float y1, t_float x2, t_float y2, t_float x3, t_float y3 )
	{
		t_float ax = x1 - x2;
		t_float ay = y1 - y2;
		t_float al = os::math::sqrt( ( ax * ax ) + ( ay * ay ) );

		t_float bx = x2 - x3;
		t_float by = y2 - y3;
		t_float bl = os::math::sqrt( ( bx * bx ) + ( by * by ) );

		t_float cx = x3 - x1;
		t_float cy = y3 - y1;
		t_float cl = os::math::sqrt( ( cx * cx ) + ( cy * cy ) );

		return TriangleArea( al, bl, cl );
	}


private:

	/// Contains the polygon point array
	TMem< SPoint >				m_pts;

	/// Number of points in the polygon
	t_size						m_uPts;

};
