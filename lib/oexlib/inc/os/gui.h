/*------------------------------------------------------------------
// gui.h
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


class gui
{
private:

    /// Constructor
	gui() {}

    /// Destructor
	virtual ~gui() {}

public:

	/// Holds information defining a line
	struct SPoint
	{
		SPoint() {}
		SPoint( oexPos p ) { x = y = p; }
		SPoint( oexPos _x, oexPos _y ) { x = _x; y = _y; }

		oexPos		x;
		oexPos		y;
	};

	/// Holds information defining a line
	struct SLine
	{
		SLine() {}
		SLine( oexPos p ) { a = b = p; }
		SLine( oexPos _a, oexPos _b ) { a = _a; b = _b; }

		SPoint		a;
		SPoint		b;

		oexFLOAT getLength()
		{	oexFLOAT dx = b.x - a.x, dy = b.y - a.y;
			return (oexFLOAT)os::math::sqrt( dx * dx + dy * dy );
		}

	};

	/// Holds information defining a rectangle
	struct SRect
	{
		SRect() {}
		SRect( oexPos s ) { left = top = 0; right = bottom = s; }
		SRect( oexPos w, oexPos h ) { left = top = 0; right = w; bottom = h; }
		SRect( oexPos l, oexPos t, oexPos r, oexPos b ) { left = l; top = t; right = r; bottom = b; }

		oexPos		left;
		oexPos		top;
		oexPos		right;
		oexPos		bottom;

		oexPos getWidth() { return right - left; }
		oexPos getHeight() { return bottom - top; }
		oexPos getArea() { return getWidth() * getHeight(); }
	};

	/// 2D range
	struct SRange2d
	{
		SRange2d() {}
		SRange2d( const SPoint &pt ) { min = max = pt; }
		SRange2d( const SPoint &ptMin, const SPoint &ptMax ) { min = ptMin; max = ptMax; }
		
		SPoint		min;
		SPoint		max;

		oexFLOAT getRange()
		{	oexFLOAT dx = max.x - min.x, dy = max.y - min.y;
			return (oexFLOAT)os::math::sqrt( dx * dx + dy * dy );
		}
	};

	
public:

	//==============================================================
	// GetCursorPosition()
	//==============================================================
	/// Returns the position of the cursor
	static oexPoint GetCursorPosition();

	//==============================================================
	// SetCursorPosition()
	//==============================================================
	/// Sets the position of the cursor
	static int SetCursorPosition( const oexPoint &p );
	

};
