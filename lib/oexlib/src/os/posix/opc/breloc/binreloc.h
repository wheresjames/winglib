/*
	Robert Umbehant <rumbehant@wheresjames.com> lifted this code from
	http://autopackage.org/docs/binreloc/ , slightly mutilated it to
	compile with a c++ compiler, and thereafter, burdened the public
	domain with his changes.

	Thanks Hongli!!!
*/

/*** LICENSE BEGIN */
/*
 * BinReloc - a library for creating relocatable executables
 * Written by: Hongli Lai <h.lai@chello.nl>
 * http://autopackage.org/
 *
 * This source code is public domain. You can relicense this code
 * under whatever license you want.
 *
 * See http://autopackage.org/docs/binreloc/ for
 * more information and how to use this.
 */
/*** LICENSE END */

#ifndef _binreloc_2008_02_08
#define _binreloc_2008_02_08

#define ENABLE_BINRELOC

/*** INCLUDE BEGIN */
#ifdef ENABLE_BINRELOC
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <unistd.h>
#endif /* ENABLE_BINRELOC */
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
/*** INCLUDE END */

/*** ERROR BEGIN */
/** These error codes can be returned by br_init(), br_init_lib(), gbr_init() or gbr_init_lib(). */
typedef enum {
	/** Cannot allocate memory. */
	BR_INIT_ERROR_NOMEM,
	/** Unable to open /proc/self/maps; see errno for details. */
	BR_INIT_ERROR_OPEN_MAPS,
	/** Unable to read from /proc/self/maps; see errno for details. */
	BR_INIT_ERROR_READ_MAPS,
	/** The file format of /proc/self/maps is invalid; kernel bug? */
	BR_INIT_ERROR_INVALID_MAPS,
	/** BinReloc is disabled (the ENABLE_BINRELOC macro is not defined). */
	BR_INIT_ERROR_DISABLED
} BrInitError;
/*** ERROR END */


/* From basic.cpp */
static char * _br_find_exe (BrInitError *error);
//static char * _br_find_exe_for_symbol (const void *symbol, BrInitError *error);


/* From normal.cpp */
int br_init (BrInitError *error);
int br_init_lib (BrInitError *error);
void br_free_lib ();
char * br_find_exe (const char *default_exe);
char * br_find_prefix (const char *default_prefix);
char * br_find_bin_dir (const char *default_bin_dir);
char * br_find_sbin_dir (const char *default_sbin_dir);
char * br_find_data_dir (const char *default_data_dir);
char * br_find_locale_dir (const char *default_locale_dir);
char * br_find_lib_dir (const char *default_lib_dir);
char * br_find_libexec_dir (const char *default_libexec_dir);
char * br_find_etc_dir (const char *default_etc_dir);
char * br_strcat (const char *str1, const char *str2);
char * br_build_path (const char *dir, const char *file);
//static char * br_strndup (const char *str, size_t size);
char * br_dirname (const char *path);

#endif /* _binreloc_2008_02_08 */



