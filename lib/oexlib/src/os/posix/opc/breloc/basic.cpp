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

#include "binreloc.h"

/*** FUNCTION BEGIN */
/** @internal
 * Find the canonical filename of the executable. Returns the filename
 * (which must be freed) or NULL on error. If the parameter 'error' is
 * not NULL, the error code will be stored there, if an error occured.
 */
static char *
_br_find_exe (BrInitError *error)
{
#ifndef ENABLE_BINRELOC
	if (error)
		*error = BR_INIT_ERROR_DISABLED;
	return NULL;
#else
	char *path, *path2, *line, *result;
	size_t buf_size;
	ssize_t size;
	struct stat stat_buf;
	FILE *f;

	/* Read from /proc/self/exe (symlink) */
	if (sizeof (path) > SSIZE_MAX)
		buf_size = SSIZE_MAX - 1;
	else
		buf_size = PATH_MAX - 1;
	path = (char *) malloc (buf_size);
	if (path == NULL) {
		/* Cannot allocate memory. */
		if (error)
			*error = BR_INIT_ERROR_NOMEM;
		return NULL;
	}
	path2 = (char *) malloc (buf_size);
	if (path2 == NULL) {
		/* Cannot allocate memory. */
		if (error)
			*error = BR_INIT_ERROR_NOMEM;
		free (path);
		return NULL;
	}

	strncpy (path2, "/proc/self/exe", buf_size - 1);

	while (1) {
		int i;

		size = readlink (path2, path, buf_size - 1);
		if (size == -1) {
			/* Error. */
			free (path2);
			break;
		}

		/* readlink() success. */
		path[size] = '\0';

		/* Check whether the symlink's target is also a symlink.
		 * We want to get the final target. */
		i = stat (path, &stat_buf);
		if (i == -1) {
			/* Error. */
			free (path2);
			break;
		}

		/* stat() success. */
		if (!S_ISLNK (stat_buf.st_mode)) {
			/* path is not a symlink. Done. */
			free (path2);
			return path;
		}

		/* path is a symlink. Continue loop and resolve this. */
		strncpy (path, path2, buf_size - 1);
	}


	/* readlink() or stat() failed; this can happen when the program is
	 * running in Valgrind 2.2. Read from /proc/self/maps as fallback. */

	buf_size = PATH_MAX + 128;
	line = (char *) realloc (path, buf_size);
	if (line == NULL) {
		/* Cannot allocate memory. */
		free (path);
		if (error)
			*error = BR_INIT_ERROR_NOMEM;
		return NULL;
	}

	f = fopen ("/proc/self/maps", "r");
	if (f == NULL) {
		free (line);
		if (error)
			*error = BR_INIT_ERROR_OPEN_MAPS;
		return NULL;
	}

	/* The first entry should be the executable name. */
	result = fgets (line, (int) buf_size, f);
	if (result == NULL) {
		fclose (f);
		free (line);
		if (error)
			*error = BR_INIT_ERROR_READ_MAPS;
		return NULL;
	}

	/* Get rid of newline character. */
	buf_size = strlen (line);
	if (buf_size <= 0) {
		/* Huh? An empty string? */
		fclose (f);
		free (line);
		if (error)
			*error = BR_INIT_ERROR_INVALID_MAPS;
		return NULL;
	}
	if (line[buf_size - 1] == 10)
		line[buf_size - 1] = 0;

	/* Extract the filename; it is always an absolute path. */
	path = strchr (line, '/');

	/* Sanity check. */
	if (strstr (line, " r-xp ") == NULL || path == NULL) {
		fclose (f);
		free (line);
		if (error)
			*error = BR_INIT_ERROR_INVALID_MAPS;
		return NULL;
	}

	path = strdup (path);
	free (line);
	fclose (f);
	return path;
#endif /* ENABLE_BINRELOC */
}


/** @internal
 * Find the canonical filename of the executable which owns symbol.
 * Returns a filename which must be freed, or NULL on error.
 */
/*
static char *
_br_find_exe_for_symbol (const void *symbol, BrInitError *error)
{
#ifndef ENABLE_BINRELOC
	if (error)
		*error = BR_INIT_ERROR_DISABLED;
	return (char *) NULL;
#else
	#define SIZE PATH_MAX + 100
	FILE *f;
	size_t address_string_len;
	char *address_string, line[SIZE], *found;

	if (symbol == NULL)
		return (char *) NULL;

	f = fopen ("/proc/self/maps", "r");
	if (f == NULL)
		return (char *) NULL;

	address_string_len = 4;
	address_string = (char *) malloc (address_string_len);
	found = (char *) NULL;

	while (!feof (f)) {
		char *start_addr, *end_addr, *end_addr_end, *file;
		void *start_addr_p, *end_addr_p;
		size_t len;

		if (fgets (line, SIZE, f) == NULL)
			break;

		/ * Sanity check. * /
		if (strstr (line, " r-xp ") == NULL || strchr (line, '/') == NULL)
			continue;

		/ * Parse line. * /
		start_addr = line;
		end_addr = strchr (line, '-');
		file = strchr (line, '/');

		/ * More sanity check. * /
		if (!(file > end_addr && end_addr != NULL && end_addr[0] == '-'))
			continue;

		end_addr[0] = '\0';
		end_addr++;
		end_addr_end = strchr (end_addr, ' ');
		if (end_addr_end == NULL)
			continue;

		end_addr_end[0] = '\0';
		len = strlen (file);
		if (len == 0)
			continue;
		if (file[len - 1] == '\n')
			file[len - 1] = '\0';

		/ * Get rid of "(deleted)" from the filename. * /
		len = strlen (file);
		if (len > 10 && strcmp (file + len - 10, " (deleted)") == 0)
			file[len - 10] = '\0';

		/ * I don't know whether this can happen but better safe than sorry. * /
		len = strlen (start_addr);
		if (len != strlen (end_addr))
			continue;


		/ * Transform the addresses into a string in the form of 0xdeadbeef,
		 * then transform that into a pointer. * /
		if (address_string_len < len + 3) {
			address_string_len = len + 3;
			address_string = (char *) realloc (address_string, address_string_len);
		}

		memcpy (address_string, "0x", 2);
		memcpy (address_string + 2, start_addr, len);
		address_string[2 + len] = '\0';
		sscanf (address_string, "%p", &start_addr_p);

		memcpy (address_string, "0x", 2);
		memcpy (address_string + 2, end_addr, len);
		address_string[2 + len] = '\0';
		sscanf (address_string, "%p", &end_addr_p);


		if (symbol >= start_addr_p && symbol < end_addr_p) {
			found = file;
			break;
		}
	}

	free (address_string);
	fclose (f);

	if (found == NULL)
		return (char *) NULL;
	else
		return strdup (found);
#endif / * ENABLE_BINRELOC * /
}
*/
/*** FUNCTION END */
