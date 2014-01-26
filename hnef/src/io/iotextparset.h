/* Copyright 2013 Alexander Söderlund.
 *
 * This file is part of HNEFATAFL.
 *
 * HNEFATAFL is free software: you can distribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * HNEFATAFL is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef HT_IOTEXTPARSE_T_H
#define HT_IOTEXTPARSE_T_H

#include <stdlib.h>	/* size_t */

/*
 * A string of char.
 *
 * This is significantly easier when you need to deal with several of
 * them and keep track of their capacities, as they may grow
 * arbitrarily, and must always end with '\0'.
 */
/*@exposed@*/
struct ht_str
{

	/*
	 * The chars in the string, including '\0'.
	 *
	 * Chars following '\0' may be uninitialized, even if space for
	 * them has been allocated (according to capc).
	 */
	char *	chars;

	/*
	 * Amount of chars that we have allocated in chars. This
	 * includes the '\0' separator, and cap is therefore always at
	 * least 1 (empty string).
	 *
	 * The chars array may be ASCII or UTF-8.
	 */
	size_t	capc;

};

/*
 * A line read by a line reader / parser.
 *
 * This is means to be used by ht_line_readline.
 */
/*@exposed@*/
struct ht_line
{

/*@notnull@*/
/*@owned@*/
	struct ht_str *	words;

	/*
	 * Amount of ht_strs in words that are initialized.
	 *
	 * Words >= [wordc - 1] but < [capc - 1] are filled with either
	 * 0-length strings or old strings (but should not contain
	 * garbage), and should therefore not be used.
	 */
	size_t		wordc;

	/*
	 * Amount of words in words that have been allocated.
	 *
	 * This differs from wordc in that we can "clear" a ht_line by
	 * setting wordc to 0 and overwrite any existing strings, rather
	 * than free them and allocate them again later, reducing the
	 * amount of allocations needed and minimizing heap
	 * fragmentation.
	 */
	size_t		capc;

};

#endif

