#ifdef OEXLIB_USE_CPP_EXCEPTIONS

#	ifdef OEXLIB_VERBOSE_COMPILE
#		pragma message( "winglib: Using C++ exceptions." )
#	endif

#	define OEXLIB_TRY					try 
#	define OEXLIB_CATCH_ALL			catch( ... )
#	define OEXLIB_CATCH_RET( e )		catch( ... ) { return e; }
#	define OEXLIB_CATCH_AND_DITCH		catch( ... ) {}
#	define OEXLIB_RETHROW				throw

#else

#	define OEXLIB_THROW_TYPE			
#	define OEXLIB_TRY					
#	define OEXLIB_THROW( type )		
#	define OEXLIB_CATCH_ALL			if ( 0 )
#	define OEXLIB_CATCH_RET( e )
#	define OEXLIB_CATCH_AND_DITCH
#	define OEXLIB_RETHROW				

#endif
