/*------------------------------------------------------------------
// polygon.cpp
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

#include "../oexlib.h"

OEX_USING_NAMESPACE

CPolygon::~CPolygon()
{_STT();
}


oexBOOL CPolygon::CombinePoint( t_point *x_pptList, t_size x_uListSize, t_point *x_ppt )
{_STT();
	t_line *pLn = (t_line*)x_pptList;
	if ( !pLn ) 
		return oexFALSE;

	return oexFALSE;
}

oexBOOL CPolygon::PtInPolygon( t_point *x_pptList, t_size x_uListSize, t_point *x_ppt )
{_STT();
	t_size hits = 0, i = 0;
	t_pos ySave = 0;

	if ( !x_pptList )
		return oexFALSE;

	// Skip halfline intersection
	while ( i < x_uListSize && x_pptList[ i ].y == x_ppt->y ) i++;
	if ( i >= x_uListSize ) 
		return oexFALSE;

	// For each line
	for ( t_size n = 0; n < x_uListSize; n++ )
	{
		// Next point in line
		t_size j = i + 1; if ( j >= x_uListSize ) j = 0;

		// Distance
		t_pos dx = x_pptList[ j ].x - x_pptList[ i ].x;
		t_pos dy = x_pptList[ j ].y - x_pptList[ i ].y;

		// Ignore horizontal edges
		if ( dy != 0 )
		{
			// Check for halfline intersection
			t_pos rx = x_ppt->x - x_pptList[ i ].x;
			t_pos ry = x_ppt->y - x_pptList[ i ].y;

			// Deal with edges starting or ending on the halfline
			if ( x_pptList[ j ].y == x_ppt->y && x_pptList[ j ].x >= x_ppt->x )
				ySave = x_pptList[ i ].y;

			// Look for intersection
			if ( x_pptList[ i ].y == x_ppt->y && x_pptList[ i ].x >= x_ppt->x ) 
				if ( ( ySave > x_ppt->y ) != ( x_pptList[ j ].y > x_ppt->y ) ) hits--;

			// Tally intersections with halfline
			t_float s = (t_float)ry / (t_float)dy;
			if (s >= 0.0f && s <= 1.0f && ( s * dx ) >= rx ) hits++;

		} // end if

		// Skip to next line
		i = j;

	} // end for

	// Inside if odd intersections
	return hits & 1;
}

oexBOOL CPolygon::GetBoundingRect( t_rect *x_pRect )
{
	if ( !x_pRect )
		return oexFALSE;

	// How many edges?
	t_size uPoints = getNumPoints();
	if ( 0 >= uPoints ) 
		return oexFALSE;

	// Get edge pointer
	t_point *pPts = getPoints();
	if ( pPts == oexNULL ) 
		return oexFALSE;

	// Initialize
	x_pRect->left = x_pRect->right = pPts->x;
	x_pRect->top = x_pRect->bottom = pPts->y;

	// Get minimal bounding rect
	for ( t_size i = 1; i < uPoints; i++ )
	{
		if ( pPts[ i ].x < x_pRect->left )
			x_pRect->left = pPts[ i ].x;

		if ( pPts[ i ].x > x_pRect->right )
			x_pRect->right = pPts[ i ].x;

		if ( pPts[ i ].y < x_pRect->top )
			x_pRect->top = pPts[ i ].y;

		if ( pPts[ i ].y > x_pRect->bottom )
			x_pRect->bottom = pPts[ i ].y;

	} // end for

	return oexTRUE;
}


#define BP( x, y, w ) ( ( y * w ) + x )
#define BPSW( x, y, w, sw ) ( ( ( y * w ) + x ) * 3 ) + ( y * sw )
oexBOOL CPolygon::FromMap(t_size *x_map, t_pos x_x, t_pos x_y, t_pos x_w, t_pos x_h, t_size x_th, oexUCHAR *x_buf)
{_STT();
	// Get the size
	long sz = x_w * x_h; 
	if ( sz <= 0 ) 
		return oexFALSE;

	// sanity checks
	if ( !x_map ) 
		return oexFALSE;
	if ( 0 > x_x || x_x >= x_w ) 
		return oexFALSE;
	if ( 0 > x_y || x_y >= x_h ) 
		return oexFALSE;
	if ( !x_th ) 
		return oexFALSE;

	// Ensure first pixel passes the muster
	long i = BP( x_x, x_y, x_w );
	if ( x_map[ i ] < x_th ) 
		return oexFALSE;

	// Allocate trail if needed
	TMem< oexUCHAR > apTrail;
	if ( !x_buf )
	{	if ( !apTrail.New( sz ).Ptr() ) 
			return oexFALSE;
		x_buf = apTrail.Ptr();
	} // end if

	// Empty the trail
	oexZeroMemory( x_buf, sz );

	// Do the fill
	while ( ( x_buf[ i ] & 0x0f ) <= 3 )
	{
		if ( ( x_buf[ i ] & 0x0f ) == 0 )
		{
			// Change the map value
			x_map[ i ] = 0;

			AddPoint( i % x_w, i / x_w );

			// Point to next direction
			x_buf[ i ] &= 0xf0, x_buf[ i ] |= 1;
	
			// Can we go up?
			if ( x_y < ( x_h - 1 ) )
			{	t_pos n = BP( x_x, ( x_y + 1 ), x_w );
				if ( x_map[ n ] >= x_th ) 
				{	x_y++; i = BP( x_x, x_y, x_w ); x_buf[ i ] = 0x10; }
			} // end if

		} // end if

		if ( ( x_buf[ i ] & 0x0f ) == 1 )
		{
			// Point to next direction
			x_buf[ i ] &= 0xf0, x_buf[ i ] |= 2;

			// Can we go right?
			if ( x_x < ( x_w - 1 ) )
			{	t_pos n = BP( ( x_x + 1 ), x_y, x_w );
				if ( x_map[ n ] >= x_th ) 
				{	x_x++; i = BP( x_x, x_y, x_w ); x_buf[ i ] = 0x20; }
			} // end if

		} // end if

		if ( ( x_buf[ i ] & 0x0f ) == 2 )
		{
			// Point to next direction
			x_buf[ i ] &= 0xf0, x_buf[ i ] |= 3;
		
			// Can we go down?
			if ( x_y > 0 )
			{	t_pos n = BP( x_x, ( x_y - 1 ), x_w );
				if ( x_map[ n ] >= x_th ) 
				{	x_y--; i = BP( x_x, x_y, x_w ); x_buf[ i ] = 0x30; }
			} // end if

		} // end if

		if ( ( x_buf[ i ] & 0x0f ) == 3 )
		{
			// Point to next
			x_buf[ i ] &= 0xf0, x_buf[ i ] |= 4;

			// Can we go left
			if ( x_x > 0 )
			{	t_pos n = BP( ( x_x - 1 ), x_y, x_w );
				if ( x_map[ n ] >= x_th ) 
				{	x_x--; i = BP( x_x, x_y, x_w ); x_buf[ i ] = 0x40; }
			} // end if

		} // end if

		// Time to backup?
		while ( ( x_buf[ i ] & 0xf0 ) > 0 && ( x_buf[ i ] & 0x0f ) > 3 )
		{
			// Go back
			if ( ( x_buf[ i ] & 0xf0 ) == 0x10 ) x_y--;
			else if ( ( x_buf[ i ] & 0xf0 ) == 0x20 ) x_x--;
			else if ( ( x_buf[ i ] & 0xf0 ) == 0x30 ) x_y++;
			else if ( ( x_buf[ i ] & 0xf0 ) == 0x40 ) x_x++;

			i = BP( x_x, x_y, x_w );

		} // end if

	} // end if

	return oexTRUE;
}


// *** Requires sorting ***
//
// Based on "Andrew's Monotone Chain algorithm"
//
// Copyright 2001, softSurfer (www.softsurfer.com)
// This code may be freely used and modified for any purpose
// providing that this copyright notice is included with it.
// SoftSurfer makes no warranty for this code, and cannot be held
// liable for any real or imagined damage resulting from its use.
// Users of this code must verify correctness for their application.
//
oexBOOL CPolygon::ConvexHull()
{_STT();
	// How many edges?
	t_size uPoints = getNumPoints();
	if ( uPoints <= 3 ) 
		return oexTRUE;

	// Get edge pointer
	t_point *pPts = getPoints();
	if ( !pPts ) 
		return oexFALSE;

	// Allocate new array of points
	TMem< t_point > npts;
	if ( !npts.OexNew( uPoints + 1 ).Ptr() ) 
		return oexFALSE;

	t_point *pNPts = npts.Ptr();

	t_pos bot = 0, top = -1;

	// Find the bottom
	t_pos i, minmin = 0, minmax, xmin = pPts[ 0 ].x;
	for ( i = 1; i < (t_pos)uPoints && pPts[ i ].x == xmin; i++ );
	minmax = i - 1;

	// Verify valid points
	if ( i == (t_pos)uPoints ) 
		return oexFALSE;

	// Find the top
	t_pos maxmin, maxmax = uPoints - 1, xmax = pPts[ uPoints - 1 ].x;
	for ( i = uPoints - 2; i >= 0 && pPts[ i ].x == xmax; i-- );
	maxmin = i + 1;

	// Verify valid points
	if ( minmax >= maxmin ) 
		return oexFALSE;

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
			if ( isLeft( pNPts[ top - 1 ], pNPts[ top ], pPts[ i ] ) > 0 ) break;
			else top--;

		} // end while

		// push i onto stack
		pNPts[ ++top ] = pPts[ i ];

	} // end while


	if ( maxmax != maxmin ) pNPts[ ++top ] = pPts[ maxmax ];
	bot = top; i = maxmin;

	while( --i >= minmax )
	{
		if ( isLeft( pPts[ maxmax ], pPts[ minmax ], pPts[ i ] ) >= 0 && i > minmax )
			continue;

		while( top > bot )
		{
			if ( isLeft( pNPts[ top - 1 ], pNPts[ top ], pPts[ i ] ) > 0 )
				break;
			else top--;

		} // end while

		pNPts[ ++top ] = pPts[ i ];

	} // end while

	if ( minmax != minmin ) pNPts[ ++top ] = pPts[ minmin ];


	// new size is ( top + 1 )
	m_uPts = top + 1;

	// Switch to new point list
	m_pts.Assume( npts );

	return oexTRUE;
}

CPolygon::t_float CPolygon::SimpleArea()
{_STT();
	// How many edges?
	t_size uPoints = getNumPoints();
	if ( uPoints <= 2 ) 
		return 0;

	// Get edge pointer
	t_point *pPts = getPoints();
	if ( !pPts ) 
		return 0;

	// Calculate area
    t_float area = 0;
    for ( t_pos i = 1, j = 2, k = 0; j < (t_pos)uPoints; i++, j++, k++ )
        area += pPts[ i ].x * ( pPts[ j ].y - pPts[ k ].y );
	if ( area < 0 ) area = -area;

    return area / 2.f;
}


CPolygon::t_float CPolygon::ConvexArea()
{_STT();

	// How many edges?
	t_size uPoints = getNumPoints();
	if ( uPoints <= 2 ) 
		return 0;

	// Get edge pointer
	t_point *pPts = getPoints();
	if ( !pPts ) 
		return 0;

	t_float area = 0;
	for ( t_pos a = 0, b = 1, c = 2; c < (t_pos)uPoints; b++, c++ )

		// Add the area of this triangle
		area += TriangleArea(	(t_float)pPts[ a ].x, (t_float)pPts[ a ].y,
								(t_float)pPts[ b ].x, (t_float)pPts[ b ].y,
								(t_float)pPts[ c ].x, (t_float)pPts[ c ].y );
    return area;
}


// 
// Similar to the BFP Approximate Hull Algorithm
// ( Bentley-Faust-Preparata, 1982 )
// but slightly faster
//
#define HULL_SECTIONS	32
oexBOOL CPolygon::ApxConvexHull()
{_STT();
	// How many points?
	t_size uPoints = getNumPoints();
	if ( uPoints <= 3 ) 
		return oexTRUE;

	// Get edge pointer
	t_point *pPts = getPoints();
	if ( !pPts ) 
		return oexFALSE;

	// Allocate new array of points
	TMem< t_point > npts;
	if ( !npts.OexNew( uPoints ).Ptr() ) 
		return oexFALSE;
	t_point *pNPts = npts.Ptr();

	// Find xrange
	t_pos i, xmin = pPts[ 0 ].x, xmax = pPts[ 0 ].x;
	for ( i = 1; i < (t_pos)uPoints; i++ )
		if ( pPts[ i ].x < xmin ) xmin = pPts[ i ].x;
		else if ( pPts[ i ].x > xmax ) xmax = pPts[ i ].x;

	// Ensure valid points
	if ( xmin == oexMAXLONG || xmax == oexMAXLONG ) 
		return oexFALSE;

	// Calculate range
	t_pos xr = xmax - xmin;
	if ( xr <= 0 ) 
		return oexFALSE;

	// Ensure no data overflow
	oexASSERT( ( (t_double)xmax * (t_double)HULL_SECTIONS ) < (t_double)oexMAXLONG );

	// Allocate bins and mark invalid
	SRange	bin[ HULL_SECTIONS ];
	for ( i = 0; i < HULL_SECTIONS; i++ )
		bin[ i ].min.x = bin[ i ].max.x = oexMAXLONG;

	// Min max points
	t_point min = pPts[ 0 ], max = pPts[ 0 ];

	// Get min/max points
	for ( i = 0; i < (t_pos)uPoints; i++ )	
	{
		if ( pPts[ i ].x < min.x ) min = pPts[ i ];
		else if ( pPts[ i ].x == min.x && pPts[ i ].y < min.y )
			min = pPts[ i ];
		
		if ( pPts[ i ].x > max.x ) max = pPts[ i ];
		else if ( pPts[ i ].x == max.x && pPts[ i ].y > max.y ) 
			max = pPts[ i ];
	} // end if

	// Get ranges
	for ( i = 0; i < (t_pos)uPoints; i++ )	
	{	
		t_pos xi = ( ( pPts[ i ].x - xmin ) * ( HULL_SECTIONS - 1 ) / xr );

		oexASSERT( xi < HULL_SECTIONS );
		
		// Top
		// sometimes works better with, sometimes without ???
//		if ( isLeft( min, max, pPts[ i ] ) > 0 )
		{	if ( bin[ xi ].max.x == oexMAXLONG || pPts[ i ].y > bin[ xi ].max.y )
				bin[ xi ].max = pPts[ i ];
		} // end if

		// Bottom
//		else
			if ( bin[ xi ].min.x == oexMAXLONG || pPts[ i ].y < bin[ xi ].min.y ) 
				bin[ xi ].min = pPts[ i ];

	} // end for
	
	// First the top half
	t_pos lNPoints = 0;
	for ( i = 0; lNPoints < (t_pos)uPoints && i < HULL_SECTIONS; i++ )
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
	t_pos lRight = lNPoints + 1;
	for ( i = HULL_SECTIONS - 1; lNPoints < (t_pos)uPoints && i >= 0; i-- )
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
	if ( m_uPts == (t_size)lNPoints ) 
		return oexTRUE;

	// New size
	m_uPts = lNPoints;

	// Switch to new point list
	m_pts.Assume( npts );

	return oexTRUE;
}


oexBOOL CPolygon::Inflate( t_pos x_x, t_pos x_y )
{_STT();
	// How many edges?
	t_size uPoints = getNumPoints();
	if ( uPoints <= 2 ) return 0;

	// Get edge pointer
	t_point *pPts = getPoints();
	if ( !pPts ) 
		return 0;

	t_point ptCenter = CalculateCenter();

    for ( t_size i = 0; i < uPoints; i++ )
	{
		if ( pPts[ i ].x < ptCenter.x ) pPts[ i ].x -= x_x;
		else pPts[ i ].x += x_x;
		
		if ( pPts[ i ].y < ptCenter.y ) pPts[ i ].y -= x_y;
		else pPts[ i ].y += x_y;

	} // end for

	return oexTRUE;
}


oexBOOL CPolygon::ScanSort()
{_STT();
	// How many edges?
	t_size uPoints = getNumPoints();
	if ( uPoints <= 2 ) 
		return oexFALSE;

	// Get edge pointer
	t_point *pPts = getPoints();
	if ( !pPts ) 
		return oexFALSE;

	// Sort
	rScanSort( pPts, 0, uPoints - 1 );

	return oexTRUE;
}

void CPolygon::rScanSort(t_point *x_pPts, t_size x_left, t_size x_right)
{_STT();
	
	if ( !x_pPts )
		return;

	t_point register pivot = x_pPts[ x_left ];
	t_size register l = x_left;
	t_size register r = x_right;

	while ( l < r )
	{
		// Find smaller value
		while ( ComparePoint( x_pPts[ r ], pivot ) >= 0 && l < r ) r--;

		// Need to copy?
		if ( l != r ) 
			x_pPts[ l++ ] = x_pPts[ r ];

		// Find larger value
		while ( ComparePoint( x_pPts[ l ], pivot ) <= 0 && l < r ) l++;

		// Need to copy?
		if ( l != r ) 
			x_pPts[ r-- ] = x_pPts[ l ];
		
	} // end while

	// Copy pivot point
	x_pPts[ l ] = pivot;

	// Recurse to sort sub lists
	if ( x_left < l ) 
		rScanSort( x_pPts, x_left, l - 1 );
	if ( x_right > l ) 
		rScanSort( x_pPts, l + 1, x_right );
}

CPolygon::SPoint CPolygon::CalculateCenter()
{_STT();
	t_point pt;
	t_point *pPts = m_pts.Ptr(); 	
	if ( !pPts || !m_uPts ) 
	{	pt.x = oexMAXLONG; pt.y = oexMAXLONG;
		return pt; 
	} // end if

	t_pos minx, maxx, miny, maxy;
	minx = maxx = pPts[ 0 ].x;
	miny = maxy = pPts[ 0 ].y;

	for ( t_size i = 1; i < m_uPts; i++ )
	{	if ( pPts[ i ].x < minx ) minx = pPts[ i ].x;
		else if ( pPts[ i ].x > maxx ) maxx = pPts[ i ].x;
		if ( pPts[ i ].y < miny ) miny = pPts[ i ].y;
		else if ( pPts[ i ].y > maxy ) maxy = pPts[ i ].y;
	} // end for

	pt.x = minx + ( ( maxx - minx ) >> 1 ); 
	pt.y = miny + ( ( maxy - miny ) >> 1 ); 
	
	return pt;
}

// +++ This is slow (2On^2), please replace
// +++ At least change this to a line segment 
//     intersection test so it actually works
//     for all cases!
oexBOOL CPolygon::Intersect(t_point *x_pPts, t_size x_uPts)
{_STT();
	t_size uMyPts = getNumPoints();
	t_point *pMyPts = getPoints();

	// Sanity check
	if ( !uMyPts || !x_uPts ) 
		return oexFALSE;
	if ( !pMyPts || !x_pPts ) 
		return oexFALSE;

	// Check passed points against ours
	t_size i;
	for ( i = 0; i < x_uPts; i++ )
		if ( PtInPolygon( pMyPts, uMyPts, &x_pPts[ i ] ) )
			return oexTRUE;

	// Check our points against passed points
	for ( i = 0; i < uMyPts; i++ )
		if ( PtInPolygon( x_pPts, x_uPts, &pMyPts[ i ] ) )
			return oexTRUE;

	return oexFALSE;
}


oexBOOL CPolygon::Scale(t_rect *x_pSrc, t_rect *x_pDst)
{_STT();
	// Get point information
	t_size uPts = getNumPoints();
	t_point *pPts = getPoints();
	if ( !uPts || !pPts )
		return oexFALSE;

	TMem< t_point > pts;
	if ( !pts.OexNew( uPts ).Ptr() ) 
		return oexFALSE;
	
	t_pos ox = x_pDst->left - x_pSrc->left;
	t_pos oy = x_pDst->top - x_pSrc->top;

	t_pos wsrc = x_pSrc->getWidth(), wdst = x_pDst->getWidth();
	t_pos hsrc = x_pSrc->getHeight(), hdst = x_pDst->getHeight();

	// Scale the points
	for ( t_size i = 0; i < uPts; i++ )
	{	pPts[ i ].x = ox + ( ( pPts[ i ].x * wdst ) / wsrc );
		pPts[ i ].y = oy + ( ( pPts[ i ].y * hdst ) / hsrc );
	} // end for

	// Switch buffers
	m_pts.Assume( pts );

	return oexTRUE;
}

oexBOOL CPolygon::GetIBeam(t_point *x_ptTop, t_point *x_ptBottom, oexBOOL x_bInvY)
{_STT();

	if ( !m_uPts )
		return oexFALSE;

	t_point *pPts = m_pts.Ptr(); 	
	if ( !pPts ) 
		return oexFALSE; 

	x_ptBottom->x = x_ptTop->x = pPts[ 0 ].x;
	x_ptBottom->y = x_ptTop->y = pPts[ 0 ].y;

	if ( x_bInvY )
	{
		for ( t_size i = 1; i < m_uPts; i++ )
		{
			// Check for top position
			if ( pPts[ i ].y < x_ptTop->y )
			{	x_ptTop->x = pPts[ i ].x;
				x_ptTop->y = pPts[ i ].y;
				x_ptBottom->x = pPts[ i ].x;				
			} // end if

			// Check for bottom
			if ( pPts[ i ].y > x_ptBottom->y )
				x_ptBottom->y = pPts[ i ].y;

		} // end for

	} // end if

	return oexTRUE;
}

CPolygon::t_pos CPolygon::Distance(CPolygon *pWp)
{
	t_point *pPts = getPoints(); 	
	t_size uPts = getNumPoints();
	if ( !uPts || !pPts ) 
		return oexMAXLONG; 

	t_point *pPts2 = pWp->getPoints(); 	
	t_size uPts2 = pWp->getNumPoints();
	if ( !uPts2 || !pPts2 ) 
		return oexMAXLONG; 
			 
	t_pos dx = oexMAXLONG;
	t_pos dy = oexMAXLONG;
	for ( t_size i = 0; i < uPts; i++ )
	{
		for ( t_size k = 0; k < uPts2; k++ )
		{
			// Get distance between points
			t_pos tdx = cmn::Dif( pPts[ i ].x, pPts2[ k ].x );
			t_pos tdy = cmn::Dif( pPts[ i ].y, pPts2[ k ].y );

			// Save minimum
			if ( dx > tdx && dy > tdy )
				dx = tdx, dy = tdy;

		} // end for

	} // end for

	// Did we get a valid distance?
	if ( dx == oexMAXLONG && dy == oexMAXLONG ) 
		return oexMAXLONG;

	// Calculate actual distance
	return (t_pos)os::math::sqrt( (t_double)( ( dx * dx ) + ( dy * dy ) ) );
}

oexBOOL CPolygon::ScalePoints( t_point *x_psrcPts, t_point *x_pdstPts, t_size x_uPts, t_rect *x_pSrc, t_rect *x_pDst )
{_STT();
	t_pos ox = x_pDst->left - x_pSrc->left;
	t_pos oy = x_pDst->top - x_pSrc->top;

	t_pos wsrc = x_pSrc->getWidth(), wdst = x_pDst->getWidth();
	t_pos hsrc = x_pSrc->getHeight(), hdst = x_pDst->getHeight();

	for ( t_size i = 0; i < x_uPts; i++ )
	{	x_pdstPts[ i ].x = ox + ( ( x_psrcPts[ i ].x * wdst ) / wsrc );
		x_pdstPts[ i ].y = oy + ( ( x_psrcPts[ i ].y * hdst ) / hsrc );
	} // end for

	return oexTRUE;
}
