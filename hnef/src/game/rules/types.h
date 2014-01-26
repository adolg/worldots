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
#ifndef HT_TYPES_H
#define HT_TYPES_H

#include "boardt.h"	/* HT_BIT_U8 */

/*
 * Using define because it must be available at compile time.
 *
 * Maximum amount of pieces and squares that can be defined.
 *
 * This is the maximum value of ruleset->type_piecec,
 * ruleset->type_piececap, ruleset->type_squarec and
 * ruleset->type_squarecap.
 *
 * Note that the length fo the type_pieces and type_squares arrays is
 * 1 longer than this. The last index (HT_TYPE_INDEX_INVALID) contains
 * the invalid piece/square type.
 */
#define HT_TYPE_MAX (unsigned short)8

/*
 * Define since used in switch.
 *
 * Index of the invalid piece in ruleset->type_pieces and
 * ruleset->type_squares.
 */
#define HT_TYPE_INDEX_INVALID (unsigned short)8

/*@unchecked@*/
extern
const unsigned short HT_TPIECE_CAPT_SIDES_MAX;

/*@unchecked@*/
extern
const unsigned short HT_TPIECE_CAPT_SIDES_MIN;

/*@unchecked@*/
extern
const unsigned short HT_TPIECE_CAPT_SIDES_DEF;

extern
unsigned short
type_index_get (
	const HT_BIT_U8
	)
/*@modifies nothing@*/
;

#endif

