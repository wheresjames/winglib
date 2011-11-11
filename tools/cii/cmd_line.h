// cmd_line.h

#pragma once

#include <list>
#include "str.h"
#include "property_bag.h"

template< typename T >
	class TCmdLine
{
public:

	/// Char type
	typedef T t_char;

	/// Size type
	typedef long t_size;

	/// Property bag type
	typedef TPropertyBag< t_char > t_pb;

	/// String type
	typedef typename t_pb::t_String t_String;

	/// List of strings
	typedef std::list< t_String > t_strlist;

	/// List of string pointers
	typedef std::list< t_String* > t_pstrlist;
	
public:

	/// Pass on iterator type
	typedef typename t_pb::iterator iterator;

	/// First item
	iterator begin() { return m_pb.begin(); }

	/// Item beyond the last item, yeah, that's what it is
	iterator end() { return m_pb.end(); }

public:

	// Default constructor
	TCmdLine()
	{
	}

	/// Constructs the object from the specified command line
	/**
		@param[in] argc		- Number of arguments
		@param[in] argv		- Pointer to array of argument pointers
	*/
	TCmdLine( int argc, t_char **argv )
	{
		Parse( argc, argv );
	}
	
	/// Destructor
	~TCmdLine()
	{
		clear();
	}

	/// Releases all resources
	void clear()
	{
		m_pb.clear();
	}

	/// Constructs the object from the specified command line
	/**
		@param[in] x_pStr	- Pointer to command line string
		@param[in] x_lSize	- Size of the string in str
	*/
	TCmdLine( const t_char *x_pStr, t_size x_lSize )
	{
		Parse( x_pStr, x_lSize );
	}

	/// Parses the command line arguments
	/**
		@param[in] argc		- Number of arguments
		@param[in] argv		- Pointer to array of argument pointers
		
		@return Returns the number of arguments found
	*/
	t_size Parse( int argc, t_char **argv )
	{
		// Hope the OS parsed this as we expect
		t_size i = 0; t_pstrlist last;
		for ( int n = 0; n < argc; n++ )
			if ( argv[ n ] )
				i = ParseCommandLineItem( i, argv[ n ], zstr::Length( argv[ n ] ), m_pb, &last );

		MapSwitchValues();

		return m_pb.size();
	}

	/// Parses a single command line item
	/**
		@param[in] i		- String position
		@param[in] x_sStr	- String
		@param[in] x_pb		- Reference to property bag in which to add item
	*/
	static t_size ParseCommandLineItem( t_size i, const t_char *x_p, t_size x_l, t_pb &x_pb, t_pstrlist *last )
	{
		t_size n = 0;
		if ( !x_p || 0 >= x_l )
			return i;
	
		// Is it a switch?
		if ( tcTC( t_char, '-' ) != x_p[ n ] )
		{
			// Save the naked value into the array by it's position
			t_String sVal( x_p, x_l );
			x_pb[ t_String( tcTT( t_char, "#" ) ) + tcnv::ToString< T, t_String >( i ) ] = sVal;
			
			// Update previous switch(s) with it's value
			if ( last && last->size() )
			{	stdForeach( t_pstrlist::iterator, it, (*last) )
					if ( *it )
						**it = sVal;
				last->clear();
			} // end if

			return ++i;

		} // end else

		// Dump the last switches
		if ( last )
			last->clear();
		
		// Skip the switch
		if ( ++n >= x_l )
			return i;

		// Single switch item?
		if ( tcTC( t_char, '-' ) != x_p[ n ] )
		{
			// Find string separator
			t_size sep = str::FindCharacter( &x_p[ n ], x_l - n, tcTC( t_char, ':' ) );
			t_String sVal = ( 0 < sep ) 
							? t_String( &x_p[ n + sep + 1 ], x_l - n - sep ) 
							: ( t_String( tcTT( t_char, "#" ) ) + tcnv::ToString< T, t_String >( i ) );

			// Set all switches
			while( n < x_l && tcTC( t_char, ':' ) != x_p[ n ] )
			{
				t_char key[ 2 ] = { x_p[ n++ ], 0 };
				x_pb[ key ] = sVal;

				// In case we want to update them later with a real value
				if ( 0 >= sep && last )
					last->push_back( &x_pb[ key ].str() );

			} // end while

			return i;

		} // end if

		// Skip the double switch
		if ( ++n >= x_l )
			return i;

		// Find string separator
		t_String sKey;
		t_size sep = str::FindCharacter( &x_p[ n ], x_l - n, tcTC( t_char, ':' ) );
		if ( 0 < sep )
			x_pb[ sKey = t_String( &x_p[ n ], sep ) ] = t_String( &x_p[ n + sep + 1 ], x_l - n - sep );
		else
		{
			x_pb[ sKey = t_String( &x_p[ n ], x_l - n ) ] = t_String( tcTT( t_char, "#" ) ) + tcnv::ToString< T, t_String >( i );
			if ( last )
				last->push_back( &x_pb[ sKey ].str() );
		} // end else

		return i;
	}

	/// Parses the command line arguments
	/**
		@param[in] argc		- Number of arguments
		@param[in] argv		- Pointer to array of argument pointers
		
		@return Returns the number of arguments found
	*/
	t_size Parse( const t_char *x_pStr, t_size x_lSize )
	{
		// Start over
		clear();

		// Sanity check
		if ( !x_pStr || 0 >= x_lSize )
			return 0;

		// Break the command line into chunks
		t_strlist sl = str::SplitQuoted< T, t_String, t_strlist >( x_pStr, x_lSize, 
																   tcTT( t_char, " \t" ), tcTT( t_char, "\"'" ), 
																   tcTT( t_char, "\"'" ), tcTT( t_char, "\\" ) );

		// We get anything?
		if ( !sl.size() )
			return 0;

		// Parse each item
		t_size i = 0; t_pstrlist last;
		stdForeach( t_strlist::iterator, it, sl )
			i = ParseCommandLineItem( i, it->data(), it->length(), m_pb, &last );

		// Set switch data values
		MapSwitchValues();

		return m_pb.size();
	}

	/// Will map any values with ##<offset> to the corrisponding position keys value
	void MapSwitchValues()
	{
		// @warning Don't use substr(), it has bugs ;)

		// Set switch values to corrisponding data
		stdForeach( t_pb::iterator, it, m_pb )
			if ( t_String( it->second->str().c_str(), 0, 2 ) == tcTT( t_char, "##" ) )
			{	t_String sKey = t_String( it->second->str().c_str(), 1, t_String::npos );
				if ( m_pb.IsSet( sKey ) )
					it->second = m_pb[ sKey ];
			} // end if
	}

	/// Return a reference to the property bag holding the command line params
	t_pb& pb() { return m_pb; }

private:

	/// Property bag that holds the parsed command line
	t_pb				m_pb;

};
