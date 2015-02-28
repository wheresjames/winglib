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

#include "../stdafx.h"
#include "math.h"

using namespace sqbind;

#define SQ_DIF_VAL( a, b )		( a > b ? a - b : b - a )

_SQBIND_REGISTER_CLASS_BEGIN( sqbind::CSqPolygon, CSqPolygon )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqPolygon, Destroy )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqPolygon, Copy )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqPolygon, AddPoint )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqPolygon, getNumPoints )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqPolygon, getNumEdges )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqPolygon, ScanSort )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqPolygon, ConvexHull )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqPolygon, ApxConvexHull )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqPolygon, SimpleArea )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqPolygon, ConvexArea )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqPolygon, Intersect )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqPolygon, Distance )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqPolygon, getPointList )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqPolygon, getPointX )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqPolygon, getPointY )
	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqPolygon, getPointInfo )
//	_SQBIND_MEMBER_FUNCTION(  sqbind::CSqPolygon,  )
_SQBIND_REGISTER_CLASS_END()

void CSqPolygon::Register( sqbind::VM vm )
{_STT();
	_SQBIND_EXPORT( vm, CSqPolygon );
}

CSqPolygon::CSqPolygon()
{
	m_lPts = 0;
}

CSqPolygon::~CSqPolygon()
{
	Destroy();
}

void CSqPolygon::Destroy()
{_STT();

	m_lPts = 0;

}

int CSqPolygon::AddPoint( long x, long y )
{	
	// Resize array
	if ( m_pts.Size() < ( m_lPts + 1 ) )
		m_pts.Resize( ( m_lPts < 8 ) ? 8 : ( m_lPts * 2 ) );

	// Set point
	m_pts[ m_lPts ].x = x; 
	m_pts[ m_lPts ].y = y;

	return ++m_lPts;
}

int CSqPolygon::ScanSort()
{_STT();

	// How many edges?
	long lPoints = getNumPoints();
	if ( lPoints <= 2 ) 
		return 0;

	// Get edge pointer
	CSqPoint *pPts = getPoints();
	if ( pPts == NULL ) 
		return 0;

	// Sort
	rScanSort( pPts, 0, lPoints - 1 );

	return TRUE;
}

void CSqPolygon::rScanSort( CSqPoint *pPts, long left, long right )
{_STT();

	CSqPoint pivot = pPts[ left ];
	long l = left, r = right;

	while ( l < r )
	{
		// Find smaller value
		while ( ComparePoint( pPts[ r ], pivot ) >= 0 && l < r ) 
			r--;

		// Need to copy?
		if ( l != r ) 
			pPts[ l++ ] = pPts[ r ];

		// Find larger value
		while ( ComparePoint( pPts[ l ], pivot ) <= 0 && l < r ) 
			l++;

		// Need to copy?
		if ( l != r ) 
			pPts[ r-- ] = pPts[ l ];
		
	} // end while

	// Copy pivot point
	pPts[ l ] = pivot;

	// Recurse to sort sub lists
	if ( left < l ) 
		rScanSort( pPts, left, l - 1 );
		
	if ( right > l ) 
		rScanSort( pPts, l + 1, right );
}

int CSqPolygon::ConvexHull()
{_STT();

	// How many edges?
	long lPoints = getNumPoints();
	if ( lPoints <= 3 ) 
		return 1;

	// Get edge pointer
	CSqPoint *pPts = getPoints();
	if ( !pPts ) 
		return 0;

	// Allocate new array of points
	oex::TMem< CSqPoint > npts;
	if ( !npts.OexNew( lPoints + 1 ).Ptr() ) 
		return 0;

	CSqPoint *pNPts = npts.Ptr();

	long bot = 0, top = -1;

	// Find the bottom
	long i, minmin = 0, minmax, xmin = pPts[ 0 ].x;
	for ( i = 1; i < lPoints && pPts[ i ].x == xmin; i++ );
	minmax = i - 1;

	// Verify valid points
	if ( i == lPoints ) 
		return 0;

	// Find the top
	long maxmin, maxmax = lPoints - 1, xmax = pPts[ lPoints - 1 ].x;
	for ( i = lPoints - 2; i >= 0 && pPts[ i ].x == xmax; i-- );
	maxmin = i + 1;

	// Verify valid points
	if ( minmax >= maxmin ) 
		return 0;

	pNPts[ ++top ] = pPts[ minmin ];

	i = minmax;
	while( ++i <= maxmin )
	{
		// lower line joines minmin to maxmin
		if ( isLeft( pPts[ minmin ], pPts[ maxmin ], pPts[ i ] ) >= 0 && i < maxmin )
			continue;

		// if at least two points on the stack
		while ( top > 0 )
		{
			// test if i is left of the line at top of stack
			if ( isLeft( pNPts[ top - 1 ], pNPts[ top ], pPts[ i ] ) > 0 ) 
				break;
			else 
				top--;

		} // end while

		// push i onto stack
		pNPts[ ++top ] = pPts[ i ];

	} // end while


	if ( maxmax != maxmin ) 
		pNPts[ ++top ] = pPts[ maxmax ];

	bot = top; i = maxmin;

	while( --i >= minmax )
	{
		if ( isLeft( pPts[ maxmax ], pPts[ minmax ], pPts[ i ] ) >= 0 && i > minmax )
			continue;

		while( top > bot )
		{
			if ( isLeft( pNPts[ top - 1 ], pNPts[ top ], pPts[ i ] ) > 0 )
				break;
			else 
				top--;

		} // end while

		pNPts[ ++top ] = pPts[ i ];

	} // end while

	if ( minmax != minmin ) 
		pNPts[ ++top ] = pPts[ minmin ];

	// new size is ( top + 1 )
	m_lPts = top + 1;

	// Switch to new point list
	m_pts.Assume( npts );
	m_pts.Resize( m_lPts );

	return 1;
}

long CSqPolygon::getPointList( CSqMulti *pPoints )
{_STT();

	long lPoints = getNumPoints();
	if ( 0 >= lPoints )
		return 0;

	CSqPoint *pPts = getPoints();
	if ( !pPts ) 
		return 0;

	for ( long i = 0; i < lPoints; i++ )
	{	stdString sNum = sqbind::oex2std( oexMks( i ) );
		(*pPoints)[ sNum ][ oexT( "x" ) ] = sqbind::oex2std( oexMks( pPts[ i ].x ) );
		(*pPoints)[ sNum ][ oexT( "y" ) ] = sqbind::oex2std( oexMks( pPts[ i ].y ) );
	} // end for

	return lPoints;
}


float CSqPolygon::SimpleArea()
{_STT();
	// How many edges?
	long lPoints = getNumPoints();
	if ( lPoints <= 2 ) 
		return 0;

	// Get edge pointer
	CSqPoint *pPts = getPoints();
	if ( !pPts ) 
		return 0;

	// Calculate area
    float area = 0;
    for ( long i = 1, j = 2, k = 0; j < lPoints; i++, j++, k++ )
        area += pPts[ i ].x * ( pPts[ j ].y - pPts[ k ].y );
	if ( area < 0 ) 
		area = -area;

    return area / 2.f;
}


float CSqPolygon::ConvexArea()
{_STT();

	// How many edges?
	long lPoints = getNumPoints();
	if ( lPoints <= 2 ) 
		return 0;

	// Get edge pointer
	CSqPoint *pPts = getPoints();
	if ( !pPts ) 
		return 0;

	float area = 0;
	for ( long a = 0, b = 1, c = 2; c < lPoints; b++, c++ )

		// Add the area of this triangle
		area += TriangleArea(	(float)pPts[ a ].x, (float)pPts[ a ].y,
								(float)pPts[ b ].x, (float)pPts[ b ].y,
								(float)pPts[ c ].x, (float)pPts[ c ].y );

    return area;
}


#define HULL_SECTIONS			32
int CSqPolygon::ApxConvexHull()
{_STT();
	// How many points?
	long lPoints = getNumPoints();
	if ( lPoints <= 3 ) 
		return 1;

	// Get edge pointer
	CSqPoint *pPts = getPoints();
	if ( !pPts ) 
		return 0;

	// Allocate new array of points
	oex::TMem< CSqPoint > npts;
	if ( !npts.OexNew( lPoints ).Ptr() ) 
		return 0;
		
	CSqPoint *pNPts = npts.Ptr();

	// Find xrange
	long i, xmin = pPts[ 0 ].x, xmax = pPts[ 0 ].x;
	for ( i = 1; i < lPoints; i++ )
		if ( pPts[ i ].x < xmin ) 
			xmin = pPts[ i ].x;
		else if ( pPts[ i ].x > xmax ) 
			xmax = pPts[ i ].x;

	// Ensure valid points
	if ( xmin == oexMAXLONG || xmax == oexMAXLONG ) 
		return 0;

	// Calculate range
	long xr = xmax - xmin;
	if ( xr <= 0 ) 
		return 0;

	// Ensure no data overflow
	if( ( (double)xmax * (double)HULL_SECTIONS ) > (double)oexMAXLONG )
		return 0;

	// Allocate bins and mark invalid
	CSqRange bin[ HULL_SECTIONS ];
	for ( i = 0; i < HULL_SECTIONS; i++ )
		bin[ i ].min.x = bin[ i ].max.x = oexMAXLONG;

	// Min max points
	CSqPoint min = pPts[ 0 ], max = pPts[ 0 ];

	// Get min/max points
	for ( i = 0; i < lPoints; i++ )	
	{
		if ( pPts[ i ].x < min.x ) 
			min = pPts[ i ];
		else if ( pPts[ i ].x == min.x && pPts[ i ].y < min.y )
			min = pPts[ i ];
		
		if ( pPts[ i ].x > max.x ) 
			max = pPts[ i ];
		else if ( pPts[ i ].x == max.x && pPts[ i ].y > max.y ) 
			max = pPts[ i ];
	} // end if

	// Get ranges
	for ( i = 0; i < lPoints; i++ )	
	{	
		long xi = ( ( pPts[ i ].x - xmin ) * ( HULL_SECTIONS - 1 ) / xr );

//		ASSERT( xi < HULL_SECTIONS );
		
		// Top
		// sometimes works better with, sometimes without ???
		if ( isLeft( min, max, pPts[ i ] ) > 0 )
		{	if ( bin[ xi ].max.x == oexMAXLONG || pPts[ i ].y > bin[ xi ].max.y )
				bin[ xi ].max = pPts[ i ];
		} // end if

		// Bottom
		else if ( bin[ xi ].min.x == oexMAXLONG || pPts[ i ].y < bin[ xi ].min.y ) 
				bin[ xi ].min = pPts[ i ];

	} // end for
	
	// First the top half
	long lNPoints = 0;
	for ( i = 0; lNPoints < lPoints && i < HULL_SECTIONS; i++ )
	{	
		// If valid max
		if ( bin[ i ].max.x != oexMAXLONG )
		{
			// Backup if inside this point
			while ( ( lNPoints > 1 ) && isLeft( pNPts[ lNPoints - 2 ], pNPts[ lNPoints - 1 ], bin[ i ].max ) < 0 )
				lNPoints--;

			// Save this point
			pNPts[ lNPoints++ ] = bin[ i ].max;
		
		} // end if

	} // end for
		
	// Now for the bottom half
	long lRight = lNPoints + 1;
	for ( i = HULL_SECTIONS - 1; lNPoints < lPoints && i >= 0; i-- )
	{	
		// If valid min
		if ( bin[ i ].min.x != oexMAXLONG )
		{
			// Backup if inside this point
			while ( lNPoints > lRight && isLeft( pNPts[ lNPoints - 2 ], pNPts[ lNPoints - 1 ], bin[ i ].min ) < 0 )
				lNPoints--;

			// Save this point
			pNPts[ lNPoints++ ] = bin[ i ].min;

		} // end if

	} // end for

	// Just punt if no change
	if ( m_lPts == lNPoints ) 
		return 1;

	// New size
	m_lPts = lNPoints;

	// Switch to new point list
	m_pts.Assume( npts );
	m_pts.Resize( m_lPts );

	return 1;
}

float CSqPolygon::TriangleArea( float a, float b, float c )
{	float s = ( a + b + c ) / (float)2;
	return sqrt( s * ( s - a ) * ( s - b ) * ( s - c ) );
}

float CSqPolygon::TriangleArea( float x1, float y1, float x2, float y2, float x3, float y3 )
{
	float ax = x1 - x2;
	float ay = y1 - y2;
	float al = sqrt( ( ax * ax ) + ( ay * ay ) );

	float bx = x2 - x3;
	float by = y2 - y3;
	float bl = sqrt( ( bx * bx ) + ( by * by ) );

	float cx = x3 - x1;
	float cy = y3 - y1;
	float cl = sqrt( ( cx * cx ) + ( cy * cy ) );

	return TriangleArea( al, bl, cl );
}

int CSqPolygon::PtInPolygon( const CSqPoint *pptList, long lListSize, const CSqPoint *ppt )
{_STT();

	long hits = 0, i = 0;
	long ySave = 0;

	// Skip halfline intersection
	while ( i < lListSize && pptList[ i ].y == ppt->y ) 
		i++;
		
	if ( i >= lListSize ) 
		return 0;

	// For each line
	for ( long n = 0; n < lListSize; n++ )
	{
		// Next point in line
		long j = i + 1; if ( j >= lListSize ) j = 0;

		// Distance
		long dx = pptList[ j ].x - pptList[ i ].x;
		long dy = pptList[ j ].y - pptList[ i ].y;

		// Ignore horizontal edges
		if ( dy != 0 )
		{
			// Check for halfline intersection
			int rx = ppt->x - pptList[ i ].x;
			int ry = ppt->y - pptList[ i ].y;

			// Deal with edges starting or ending on the halfline
			if ( pptList[ j ].y == ppt->y && pptList[ j ].x >= ppt->x )
				ySave = pptList[ i ].y;

			// Look for intersection
			if ( pptList[ i ].y == ppt->y && pptList[ i ].x >= ppt->x ) 
				if ( ( ySave > ppt->y ) != ( pptList[ j ].y > ppt->y ) ) 
					hits--;

			// Tally intersections with halfline
			float s = (float)ry / (float)dy;
			if ( s >= 0.0f && s <= 1.0f && ( s * dx ) >= rx ) 
				hits++;

		} // end if

		// Skip to next line
		i = j;

	} // end for

	// Inside if odd intersections
	return hits & 1;
}


// +++ This is slow (2On^2), please replace
// +++ At least change this to a line segment 
//     intersection test so it actually works
//     for all cases!
int CSqPolygon::IntersectPts( const CSqPoint *pPts, long lPts )
{_STT();

	long lMyPts = getNumPoints();
	CSqPoint *pMyPts = getPoints();

	// Sanity check
	if ( lMyPts == 0 || lPts == 0 ) 
		return 0;

	if ( !pMyPts || !pPts ) 
		return 0;

	// Check passed points against ours
	long i;
	for ( i = 0; i < lPts; i++ )
		if ( PtInPolygon( pMyPts, lMyPts, &pPts[ i ] ) )
			return 1;

	// Check our points against passed points
	for ( i = 0; i < lMyPts; i++ )
		if ( PtInPolygon( pPts, lPts, &pMyPts[ i ] ) )
			return 1;

	return 0;
}

long CSqPolygon::Distance( CSqPolygon *pPoly )
{
	CSqPoint *pPts = getPoints(); 	
	long lPts = getNumPoints();
	if ( 0 >= lPts || !pPts ) 
		return 0; 

	CSqPoint *pPts2 = pPoly->getPoints(); 	
	long lPts2 = pPoly->getNumPoints();
	if ( 0 >= lPts2 || !pPts2 ) 
		return 0; 
			 
	long dx = oexMAXLONG;
	long dy = oexMAXLONG;
	for ( long i = 0; i < lPts; i++ )
	{
		for ( long k = 0; k < lPts2; k++ )
		{
			// Get distance between points
			long tdx = SQ_DIF_VAL( pPts[ i ].x, pPts2[ k ].x );
			long tdy = SQ_DIF_VAL( pPts[ i ].y, pPts2[ k ].y );

			// Save minimum
			if ( dx > tdx && dy > tdy )
				dx = tdx, dy = tdy;

		} // end for

	} // end for

	// Did we get a valid distance?
	if ( dx == oexMAXLONG && dy == oexMAXLONG ) 
		return oexMAXLONG;

	// Calculate actual distance
	return (long)sqrt( (double)( ( dx * dx ) + ( dy * dy ) ) );
}

int CSqPolygon::getPointInfo( CSqMulti *pPoints )
{_STT();

	CSqPoint *pPts = getPoints(); 	
	long lPts = getNumPoints();
	if ( !pPts || 0 >= lPts )
		return 0;

	// Copy points
	for ( long i = 0; i < lPts; i++ )
	{	CSqMulti &r = (*pPoints)[ oex2std( oexMks( i ) ) ];
		r[ oexT( "x" ) ] = oex2std( oexMks( pPts[ i ].x ) );
		r[ oexT( "y" ) ] = oex2std( oexMks( pPts[ i ].y ) );		
	} // end for
	
	return lPts;
}

