// sq_tinyxml.h

#pragma once

/// Decodes / Encodes XML data
/**
*/
class CSqXml
{
public:

	// Just a few security measures
	enum
	{
		/// Maximum depth to parse
		eMaxParseDepth = 1024,

		/// Maximum number of elements to parse
		eMaxParseElements = 16 * 1024

	};

public:

	SQBIND_CLASS_CTOR_BEGIN( CSqXml )
	SQBIND_CLASS_CTOR_END( CSqXml )

	/// Default constructor
	CSqXml();

	/// Destructor
	~CSqXml();

	/// Registers the class
	static void Register( sqbind::VM vm );
	
	/** \addtogroup CSqXml
		@{

	Ok, the xml data is going to be mapped to a associative array,
	the issue here is that associative arrays don't allow two
	items at the same level to have the same name.  So, there will
	be two options which I will call keyed and indexed.

	input:

	\code
		<tag1>
			<value1>1</value1>
			<value2>2</value2>
		</tag1>
		<tag1>
			<value1>3</value1>
			<value2>4</value2>
		</tag1>
		<tag2>
			<value1 attrib1=1 attrib2=2>5</value1>
			<value2>6</value2>
		</tag2>
	\endcode

	output:
		
		Option #1 - bIndexed = 0 - Keyed

	\code
			// Decode() returns 4
			{
				tag1 = { value1 = 1, value2 = 2 } 
				tag2 = { value1 = [5]{ attrib1 = 1, attrib2 = 2 }, value2 = 6 } 
			}
	\endcode

		Option #2 - bIndexed = 1 - Indexed

	\code
			// Decode returns 6
			{ 
				0 { $ = tag1, 0 = [1]{ $ = value1 }, 1 = [2]{ $ = value2 },
				1 { $ = tag1, 0 = [3]{ $ = value1 }, 1 = [4]{ $ = value2 },
				2 { $ = tag2, 0 = [5]{ $ = value1, attrib1 = 1, attrib2 = 2 }, 1 = [6]{ $ = value2 }
			}
	\endcode

	bIndexed == 0

		Duplicate tags will be ignored, and the attributes and sub tags will be mapped 
		into the same array. If you re-encode the keyed version, you will get something 
		like...

	\code
			<tag1 value1="1" value2="2" />
			<tag2 value2="6" >
			  <value1 attrib1=1 attrib2=2>5</value1>
		    </tag2>
	\endcode

		This is because the information that value1 and value2 were sub tags and not 
		attributes is lost, and attributes are preferred since they take up less space.  
		If you don't care about the original format, this may be a good thing since this 
		encoding is more efficient.

	bIndexed == 1
	
		All the tags will be there, and in order.  You will have to use indexes to 
		navigate.  The special key '$' is reserved to hold the tag name and cannot be
		used as an attribute name.  Keep this limitation in mind. The indexed version 
		will re-encode like the original.
	*/

	/// Releases resources
	void Destroy();

	/// Encodes XML data
	static sqbind::stdString Encode( sqbind::CSqMulti *pData, 
									 const sqbind::stdString &sLineBreak, 
									 const sqbind::stdString &sTab, int bIndexed );

	/// Decodes XML data
	static int Decode( const sqbind::stdString &sData, sqbind::CSqMulti *pOut, int bIndexed, int nCaseI );

	/** @} */

private:

};

SQBIND_DECLARE_INSTANCE( CSqXml, CSqXml );

