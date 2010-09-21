
_self.load_module( "tinyxml", "" );

function _init()
{
	local xml = CSqXml();

	local data	= "<tag1>\r\n"
				+ "<!-- comment -->\r\n"
			    + " <value1>1</value1>\r\n"
				+ " <value2>2</value2>\r\n"
				+ "</tag1>\r\n"
				+ "<tag1>\r\n"
			    + " <value1>1</value1>\r\n"
				+ " <value2>2</value2>\r\n"
				+ "</tag1>\r\n"
				+ "<tag2>\r\n"
			    + " <value1 attrib1=1 attrib2=2>1</value1>\r\n"
				+ " <value2 attrib1=3 attrib2=4>2</value2>\r\n"
				+ "</tag2>\r\n"
			;

	_self.echo( "\r\n========= Sample XML ==========\r\n" );

	_self.echo( data );

	_self.echo( "\r\n========= Decode XML ==========\r\n" );

	// Keyed
	local keyed = CSqMulti();
	local found = xml.Decode( data, keyed, 0 )	
	_self.echo( "\r\n === Keyed Elements found : " + found  );
	if ( found ) _self.echo( "\r\n" + keyed.print_r( 1 ) );

	// Indexed
	local indexed = CSqMulti();
	found = xml.Decode( data, indexed, 1 )
	_self.echo( "\r\n === Indexed Elements found : " + found  );
	if ( found ) _self.echo( "\r\n" + indexed.print_r( 1 ) );


	_self.echo( "\r\n========= Encode XML ==========\r\n" );

	_self.echo( "\r\n === Keyed\r\n\r\n" + xml.Encode( keyed, "\r\n", "  ", 0 ) );

	_self.echo( "\r\n === Indexed\r\n\r\n" + xml.Encode( indexed, "\r\n", "  ", 1 ) );

	_self.echo( "\r\n === Data check \r\n" );

	_self.echo( "keyed[ \"tag1\" ][ \"value1\" ].str() = " + keyed[ "tag1" ][ "value1" ].str() );
	_self.echo( "keyed[ \"tag2\" ][ \"value2\" ].str() = " + keyed[ "tag2" ][ "value2" ].str() );
	_self.echo( "keyed[ \"tag2\" ][ \"value2\" ][ \"attrib1\" ].str() = " + keyed[ "tag2" ][ "value2" ][ "attrib1" ].str() );

	_self.echo( "\r\n === End" );

	_self.echo( "\n...press any key...\n" );
	_self.get_key();

}


