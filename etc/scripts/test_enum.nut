

_self.echo( " --- ENUM TEST ---" );

_self.echo( "INT_42 = " + _self.INT_42 );
_self.echo( "INT_NEG_42 = " + _self.INT_42 );
_self.echo( "INT64_42 = " + _self.INT64_42 );
_self.echo( "INT64_NEG_42 = " + _self.INT64_42 );

if ( 42 == _self.INT_42 ) _self.echo( " 42 == INT_42" );
if ( 42 != _self.INT_42 ) _self.echo( " 42 != INT_42" );

if ( -42 == _self.INT_NEG_42 ) _self.echo( "-42 == INT_NEG_42" );
if ( -42 != _self.INT_NEG_42 ) _self.echo( "-42 != INT_NEG_42" );

if ( 42 == _self.INT64_42 ) _self.echo( " 42 == INT64_42" );
if ( 42 != _self.INT64_42 ) _self.echo( " 42 != INT64_42" );

if ( -42 == _self.INT64_NEG_42 ) _self.echo( "-42 == INT64_NEG_42" );
if ( -42 != _self.INT64_NEG_42 ) _self.echo( "-42 != INT64_NEG_42" );

