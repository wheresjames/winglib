// cii.cpp

#include "cmd_line.h"
#include "find_files.h"

// File processing functions
#include "process_binary.h"

typedef TCmdLine< char > t_cmdline8;
typedef std::basic_string< char > t_string8;
typedef std::list< t_string8 > t_strlist8;

int process_file( t_string8 x_sIn, t_string8 x_sOut, t_cmdline8 &cl )
{
	if ( !cl.pb().IsSet( "q" ) )
		printf( "%s -> %s\n", x_sIn.c_str(), x_sOut.c_str() );

	long lBytes = process_binary< char, t_string8 >( x_sIn, x_sOut,
													 "\nconst char *data = \n{\n\t",
													 "\n};\n\nconst long bytes = ",
													 ";\n"
													);

	return 0;
}

int process_directory( t_string8 x_sIn, t_string8 x_sOut, t_cmdline8 &cl )
{
	if ( !x_sIn.length() || !x_sOut.length() )
		return 0;

	// Process directory contents
	ff::SFindData fd;
	ff::HFIND hFind = ff::FindFirst( x_sIn.c_str(), "*", &fd );
	if ( ff::c_invalid_hfind == hFind )
		return 0;

	do 
	{ 
		// Ignore relative
		if ( '.' != *fd.szName )
		{
			// Recurse if directory
			if ( ff::eFileAttribDirectory & fd.uFileAttributes )
				process_directory( ff::FilePath< char, t_string8 >( x_sIn, fd.szName ), 
								   ff::FilePath< char, t_string8 >( x_sOut, fd.szName ),
								   cl );

			// Go process the file
			else
				process_file( ff::FilePath< char, t_string8 >( x_sIn, fd.szName ), 
							  ff::FilePath< char, t_string8 >( x_sOut, fd.szName ) + t_string8( ".cpp" ),
							  cl );

		} // end if

	} while ( ff::FindNext( hFind, &fd ) );

	ff::FindClose( hFind );

	return 1;
}

int main( int argc, char* argv[] )
{
	// Parse the command line
    t_cmdline8 cl( argc, argv );

	// Dumping the command line options?
	if ( cl.pb().IsSet( "d" ) || cl.pb().IsSet( "debug" ) )
	{	for ( TCmdLine< char >::iterator it = cl.begin(); cl.end() != it; it++ )
			printf( "[%s] = '%s'\n", it->first.c_str(), it->second->c_str() );
		return 0;
	} // end if

	// Combine help commands
	if ( cl.pb().IsSet( "help" ) )
		cl.pb()[ "h" ] = 1;

	// Ensure all needed parameters are present
	if ( cl.pb().IsSet( "h" )
		 || !cl.pb().IsSet( "i" ) || !cl.pb().IsSet( "o" ) )
	{	printf( "Options\n"
				" -i            '<comma separated input directories>'\n"
				" -o            '<output directory>'\n"
				" -q / --quiet  Suppress all output\n"
				" -d / --debug  'Show processed command line array'\n"
				" -h / --help   Display this information\n"
				);
		return cl.pb().IsSet( "h" ) ? 0 : -1;
	} // end if

	if ( cl.pb().IsSet( "quiet" ) )
		cl.pb()[ "q" ] = 1;

	// Separate the different directories
	t_strlist8 dl = str::SplitQuoted< char, t_string8, t_strlist8 >
									  ( (char*)cl.pb()[ "i" ].data(), cl.pb()[ "i" ].length(), 
									    ";, \t", "\"'", "\"'", "\\", true );

	// Process each directory
	stdForeach( t_strlist8::iterator, it, dl )
		process_directory( *it, cl.pb()[ "o" ].str(), cl );

	return 0;
}
