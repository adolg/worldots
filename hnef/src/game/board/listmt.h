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
#ifndef HT_LISTM_T_H
#define HT_LISTM_T_H

#include "movet.h"	/* move */

/*@concrete@*/
struct listm
{

	/*
	 * An array of moves that contains elemc move structs.
	 *
	 * You can access elems[0] to elems[elemc - 1]. Accessing other
	 * elements may be out of array bounds, point to garbage, point
	 * to an old removed value, or other behavior.
	 */
/*@notnull@*/
/*@owned@*/
	struct move * elems;

	/*
	 * Length of elems.
	 *
	 * Don't change.
	 */
	size_t elemc;

	/*
	 * The amount of move structs allocated. This may exceed elemc.
	 *
	 * Don't change or use.
	 */
	size_t capc;

};

#endif

