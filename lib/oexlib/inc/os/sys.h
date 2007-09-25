/*------------------------------------------------------------------
// sys.h
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

class CSys
{
private:

    /// Constructor
	CSys() {}

    /// Destructor
	virtual ~CSys() {}

public:	

    /// Initializes os system stuff
    static oexBOOL Init();

    /// Cleans up os system stuff
    static oexBOOL Uninit();

    /// Call to terminate the application
	static void Exit( oexINT nRet );


    // Multi-byte
public:

	static oexCSTR8 StrFmt( oexSTR8 pDst, oexUINT uMax, oexCSTR8 pFmt, ... );

	static oexCSTR8 vStrFmt( oexSTR8 pDst, oexUINT uMax, oexCSTR8 pFmt, oexCPVOID pArgs );

	static oexCSTR8 StrFmt( oexRESULT *pRes, oexSTR8 pDst, oexUINT uMax, oexCSTR8 pFmt, ... );

	static oexCSTR8 vStrFmt( oexRESULT *pRes, oexSTR8 pDst, oexUINT uMax, oexCSTR8 pFmt, oexCPVOID pArgs );

    /// Converts to int64
    static oexINT64 StrToInt64( oexCSTR8 x_pStr, oexUINT x_uRadix );

    /// Converts to uint64
    static oexUINT64 StrToUInt64( oexCSTR8 x_pStr, oexUINT x_uRadix );

	/// Converts to long
	static oexLONG StrToLong( oexCSTR8 pStr, oexUINT uRadix = 10 );

	/// Converts to unsigned long
	static oexULONG StrToULong( oexCSTR8 pStr, oexUINT uRadix = 10 );

	/// Converts to double
	static oexDOUBLE StrToDouble( oexCSTR8 pStr );

    // Unicode
public:

	static oexCSTRW StrFmt( oexSTRW pDst, oexUINT uMax, oexCSTRW pFmt, ... );

	static oexCSTRW vStrFmt( oexSTRW pDst, oexUINT uMax, oexCSTRW pFmt, oexCPVOID pArgs );

	static oexCSTRW StrFmt( oexRESULT *pRes, oexSTRW pDst, oexUINT uMax, oexCSTRW pFmt, ... );

	static oexCSTRW vStrFmt( oexRESULT *pRes, oexSTRW pDst, oexUINT uMax, oexCSTRW pFmt, oexCPVOID pArgs );

    /// Converts to int64
    static oexINT64 StrToInt64( oexCSTRW x_pStr, oexUINT x_uRadix );

    /// Converts to uint64
    static oexUINT64 StrToUInt64( oexCSTRW x_pStr, oexUINT x_uRadix );

	/// Converts to long
	static oexLONG StrToLong( oexCSTRW pStr, oexUINT uRadix = 10 );

	/// Converts to unsigned long
	static oexULONG StrToULong( oexCSTRW pStr, oexUINT uRadix = 10 );

	/// Converts to double
	static oexDOUBLE StrToDouble( oexCSTRW pStr );

public:

	static oexPVOID MemCpy( oexPVOID pDst, oexCPVOID pSrc, oexUINT uSize );

	static oexINT MemCmp( oexCPVOID pDst, oexCPVOID pSrc, oexUINT uSize );

	static oexPVOID MemSet( oexPVOID pDst, oexINT nCh, oexUINT uSize );

    static oexPVOID Zero( oexPVOID pDst, oexUINT uSize )
    {   return MemSet( pDst, 0, uSize ); }  

    /// Creates a guid
    static oexGUID * CreateGuid( oexGUID *pGuid );

//------------------------------------------------------------------
// Sleep functions
//------------------------------------------------------------------
public:

    /// Sleep in milliseconds
    static void Sleep( oexUINT uMilliseconds, oexUINT uSeconds = 0 );

    /// Sleep in microseconds
    static oexBOOL MicroSleep( oexUINT uMicroseconds, oexUINT uSeconds = 0 );

	/// Strings constants
	static const oexUINT		c_StrErr_OK;
	static const oexUINT		c_StrErr_INVALID_PARAMETER;
	static const oexUINT		c_StrErr_INSUFFICIENT_BUFFER;

//------------------------------------------------------------------
// Wait functions
//------------------------------------------------------------------
public:

	enum etWait
	{	
		/// Value indicating wait succeeded
		waitSuccess = 0,

		/// Value indicating wait timed out
		waitTimeout = -1,

		/// Value indicating wait expired
		waitFailed = -2
	};

    /// Waitable handle type
    typedef oexCPVOID t_WAITABLE;

	//==============================================================
	// WaitForSingleObject()
	//==============================================================
	/// Waits for a waitable object to become signaled
	/**
		\param [in] x_pHandle	-	Handle to waitable object
		\param [in] x_uTimeout	-	Maximum amount of time, in
									milli-seconds, to wait.
		
		\return Zero if success, otherwise waitTimeout or waitFailed
	
		\see 
	*/
	static oexINT WaitForSingleObject( t_WAITABLE x_pHandle, oexUINT x_uTimeout );

	//==============================================================
	// WaitForMultipleObjects()
	//==============================================================
	/// Waits for multiple waitable objects
	/**
		\param [in] x_uObjects	-	Number of waitable objects
		\param [in] x_pHandle	-	Array of waitable objects
		\param [in] x_bWaitAll	-	Non-zero if you wish to wait for
									all objects to become signaled. 
									Otherwise this function returns
									as soon as any objects are in
									the signaled state.
		\param [in] x_uTimeout	-	Maximum amount of time, in 
									milli-seconds, to wait.
		
		\return Zero based index of object that is signaled if success, 
				otherwise waitTimeout or waitFailed
	
		\see 
	*/
	static oexINT WaitForMultipleObjects( oexUINT x_uObjects, t_WAITABLE *x_pHandle, oexBOOL x_bWaitAll, oexUINT x_uTimeout );

	//==============================================================
	// InterlockedIncrement()
	//==============================================================
	/// Thread safe value increment.
	/**
		\param [in] x_puVal		-	Pointer to value to increment.
		
		\return Value *before* incrementing.
	
		\see 
	*/
	static oexULONG InterlockedIncrement( oexLONG *x_puVal );

	//==============================================================
	// InterlockedDecrement()
	//==============================================================
	/// Thread safe value increment.
	/**
		\param [in] x_puVal		-	Pointer to value to decrement
		
		\return Value *before* decrementing.
	
		\see 
	*/
	static oexULONG InterlockedDecrement( oexLONG *x_puVal );

	//==============================================================
	// vInfinite()
	//==============================================================
	/// Value representing infinite timeout.
	/**
		Use with wait functions
	*/
	static oexCONST oexUINT vInfinite() { return c_Infinite; }

private:

	/// Infinite timeout value
	static oexCONST oexUINT		c_Infinite;

public:

    /// Contains information on the current time
    struct STime
    {
        oexUINT         uYear;
        oexUINT         uMonth;
        oexUINT         uDayOfWeek;
        oexUINT         uDay;
        oexUINT         uHour;
        oexUINT         uMinute;
        oexUINT         uSecond;
        oexUINT         uMillisecond;
        oexINT          nTzBias;
    };

    /// Local time
    static oexBOOL GetLocalTime( STime &x_st );

    /// System time
    static oexBOOL GetSystemTime( STime &x_st );

    /// Converts an STime structure to file time
    static oexINT64 SystemTimeToFileTime( STime &x_st );

    /// Converts a file time to an STime structure
    static void FileTimeToSystemTime( STime &x_st, oexINT64 x_ft );

    /// Returns the local timezone bias
    static oexINT GetLocalTzBias();

public:


};

/*
	static float TriangleArea( float a, float b, float c )
	{	float s = ( a + b + c ) / (float)2;
		return sqrt( s * ( s - a ) * ( s - b ) * ( s - c ) );
	}

	static float TriangleArea( float x1, float y1, float x2, float y2, float x3, float y3 )
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


*/