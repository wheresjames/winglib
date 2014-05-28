
	local a = CSqMulti(), t = CSqMulti();

	a[ "group" ][ "a" ] <- "b";
	a[ "group" ][ "c" ] <- "1234";
	a[ "group" ][ "d" ] <- "012345678901234567890123456789012345678901234567890123456789";

	a[ "group" ][ "e" ][ "x" ][ "y" ] <- "z";
	a[ "group" ][ "e" ][ "x1" ][ "y1" ] <- "z1";
	a[ "group" ][ "e" ][ "x2" ][ "y2" ] <- "z2";

	a[ "group" ][ "f" ][ "x" ][ "y" ] <- "z";
	a[ "group" ][ "f" ][ "x1" ][ "y1" ] <- "z1";
	a[ "group" ][ "f" ][ "x2" ][ "y2" ] <- "z2";

	a[ "group" ][ "g" ][ "x" ][ "y" ] <- "z";
	a[ "group" ][ "g" ][ "x1" ][ "y1" ] <- "z1";
	a[ "group" ][ "g" ][ "x2" ][ "y2" ] <- "z2";

	a[ "group" ][ "h" ][ "x" ][ "y" ] <- "z";
	a[ "group" ][ "h" ][ "x1" ][ "y1" ] <- "z1";
	a[ "group" ][ "h" ][ "x2" ][ "y2" ] <- "z2";

	a[ "second" ][ "a" ][ "b" ][ "c" ][ "1" ] <- "x";
	a[ "second" ][ "a" ][ "b" ][ "c" ][ "2" ] <- "y";
	a[ "second" ][ "a" ][ "b" ][ "c" ][ "3" ] <- "z";

	t[ "group" ][ "c" ] <- "dword";
	t[ "group" ][ "d" ] <- "hex";
	t[ "group" ][ "e" ] <- "reg";
	t[ "group" ][ "f" ] <- "reghex";
	t[ "group" ][ "g" ] <- "json";
	t[ "group" ][ "h" ] <- "url";

	t[ "second" ][ "a" ][ "*" ] <- "url";
	t[ "second" ][ "a" ][ "b" ][ "c" ] <- "json";

	_self.echo( " --- ARRAY ---\n" + a.getJSON() );
	_self.echo( " --- TYPES ---\n" + t.getJSON() );

	_self.echo( " --- REG ---\n" + _self.RegEncodeTyped( a, t ) );
