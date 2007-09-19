/*******************************************************************
// From RFC 1321 (RFC1321)

/* MD5C.C - RSA Data Security, Inc., MD5 message-digest algorithm
 */

/* Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
rights reserved.

License to copy and use this software is granted provided that it
is identified as the "RSA Data Security, Inc. MD5 Message-Digest
Algorithm" in all material mentioning or referencing this software
or this function.

License is also granted to make and use derivative works provided
that such works are identified as "derived from the RSA Data
Security, Inc. MD5 Message-Digest Algorithm" in all material
mentioning or referencing the derived work.

RSA Data Security, Inc. makes no representations concerning either
the merchantability of this software or the suitability of this
software for any particular purpose. It is provided "as is"
without express or implied warranty of any kind.

These notices must be retained in any copies of any part of this
documentation and/or software.
 
*******************************************************************/
// md5_rsa.h: interface for the CMd5Rsa class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MD5RSA_H__INCLUDED_)
#define AFX_MD5RSA_H__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/* POINTER defines a generic pointer type */
typedef unsigned char *POINTER;

/* UINT2 defines a two byte word */
typedef unsigned short int UINT2;

/* UINT4 defines a four byte word */
typedef unsigned long int UINT4;

/* MD5 context. */
typedef struct {
UINT4 state[4]; /* state (ABCD) */
UINT4 count[2]; /* number of bits, modulo 2^64 (lsb first) */
unsigned char buffer[64]; /* input buffer */
} MD5_CTX;

//==================================================================
// CMd5Rsa
//
/// RSA MD5 hash class
/**
	Calculates the RSA MD5 hash	
*/
//==================================================================
class CMd5Rsa  
{

public:

	/// Default constructor
	CMd5Rsa() {}

	/// Destructor
	~CMd5Rsa() {}

public:

	//==============================================================
	// MD5()
	//==============================================================
	/// Updates hash with input data
	/**
		\param [in,out] digest		-	Pointer to hash
		\param [in] input			-	Input data
		\param [in] inputLen		-	Number of bytes in input
	*/
	static void MD5(unsigned char digest[16], const void *input, unsigned int inputLen);

	//==============================================================
	// MD5Init()
	//==============================================================
	/// Initializes an MD5 hash data structure
	/**
		\param [out] context	-	MD5 context data to be initialized
	*/
	static void MD5Init (MD5_CTX *context);


	//==============================================================
	// MD5Update()
	//==============================================================
	/// Updates hash with input data
	/**
		\param [in,out] context		-	Pointer to MD5 hash data structure
		\param [in] input			-	Input data
		\param [in] inputLen		-	Number of bytes in input
	*/
	static void MD5Update (MD5_CTX *context, unsigned char *input, unsigned int inputLen);
	
	//==============================================================
	// MD5Final()
	//==============================================================
	/// Finalizes an MD5 hash data structure
	/**
		\param [in,out] digest		-	MD5 hash
		\param [in,out] context		-	Pointer to MD5 hash data structure
	*/
	static void MD5Final (unsigned char digest[16], MD5_CTX *context);

	//==============================================================
	// MD5Transform()
	//==============================================================
	/// Updates a hash with 64 bytes of input data
	/**
		\param [in,out] state		-	hash buffer
		\param [in,out] block		-	Input data
	*/
	static void MD5Transform (UINT4 state[4], unsigned char block[64]);

	//==============================================================
	// Encode()
	//==============================================================
	/// Encodes a hash into a character string
	/**
		\param [out] output		-	Receives hash string
		\param [in] input		-	Hash value
		\param [in] len			-	Size of buffer in output
	*/
	static void Encode (unsigned char *output, UINT4 *input, unsigned int len);

	//==============================================================
	// Decode()
	//==============================================================
	/// Decodes a hash from a character string
	/**
		\param [out] output		-	Receives hash value
		\param [in] input		-	Hash string
		\param [in] len			-	Number of bytes in input
	*/
	static void Decode (UINT4 *output, unsigned char *input, unsigned int len);

	//==============================================================
	// MD5_memcpy()
	//==============================================================
	/// Copies data from one buffer to another
	/**
		\param [out] output		-	Destination buffer
		\param [in] input		-	Source buffer
		\param [in] len			-	Number of bytes to copy
	*/
	static void MD5_memcpy (POINTER output, POINTER input, unsigned int len);

	//==============================================================
	// MD5_memset()
	//==============================================================
	/// Initializes a memory buffer
	/**
		\param [out] output		-	Destination buffer
		\param [in] value		-	Initialization value
		\param [in] len			-	Number of bytes to initialize
	*/
	static void MD5_memset (POINTER output, int value, unsigned int len);

private:

	/// hash padding data
	static unsigned char	PADDING[ 64 ];

};

#endif // !defined(AFX_MD5RSA_H__INCLUDED_)
