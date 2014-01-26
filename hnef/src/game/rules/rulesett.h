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
#ifndef HT_RULESET_T_H
#define HT_RULESET_T_H

#include "boardt.h"		/* HT_BIT_U8 */
#include "type_piecet.h"	/* type_piece */
#include "type_squaret.h"	/* type_square */
#include "types.h"		/* HT_TYPE_MAX */

/*
 * The length of the pieces and squares arrays is, in both cases,
 * bwidth * bheight. This is also the length of the pieces array in
 * board.
 */
/*@exposed@*/
struct ruleset
{

	/*
	 * Path to working directory.
	 */
/*@notnull@*/
/*@owned@*/
	char *		path,
	* id,
	* name,
	* hash;

	/*
	 * true if repetition is forbidden.
	 */
	HT_BOOL			forbid_repeat;

	/*
	 * Width (x-axis length) of the board.
	 */
	unsigned short		bwidth;

	/*
	 * Height (y-axis length) of the board.
	 */
	unsigned short		bheight;

	/*
	 * Pieces array.
	 *
	 * Length is bwidth * bheight.
	 */
/*@notnull@*/
/*@owned@*/
	HT_BIT_U8 *		pieces;

	/*
	 * Squares array.
	 * Length is bwidth * bheight.
	 */
/*@notnull@*/
/*@owned@*/
	HT_BIT_U8 *		squares;

	/*
	 * Defined piece types.
	 *
	 * These piece types that are always present in this array
	 * regardless if they have been defined by a ruleset file:
	 *
	 * INDEX	PIECE BIT
	 * 0		1
	 * 1		2
	 * 2		4
	 * 3		8
	 * 4		16
	 * 5		32
	 * 6		64
	 * 7		128
	 * 8		Always HT_BIT_U8_EMPTY (invalid)
	 *
	 * Note that type_pieces[8] is a type_piece that is returned as
	 * an invalid value. You should not use it (for anything else
	 * than checking if it's bit is HT_BIT_U8_EMPTY), nor change its
	 * statistics.
	 *
	 * If a type_piece in this array has bit == HT_BIT_U8_EMPTY,
	 * then that type_piece's bit (which you gave to type_piece_get)
	 * has not been defined by the ruleset, or it's invalid.
	 *
	 * The type_piece->bit is only set to non-0 for pieces that are
	 * defined in the ruleset file. All other type_pieces have
	 * bit == HT_BIT_U8_EMPTY, to show that they are not used.
	 *
	 * The pieces have to be defined from 1 to 128 in order.
	 * Defining piece 1 and then 4 is an error. Therefore
	 * type_piecec will always be the amount of defined pieces and
	 * you can always iterate over type_pieces from 0 to
	 * type_piecec.
	 */
/*@notnull@*/
	struct type_piece	type_pieces[(unsigned short)
						(HT_TYPE_MAX + 1)];

	/*
	 * Amount of defined type_squares.
	 *
	 * This does not equal the array length.
	 */
	unsigned short		type_piecec;

	/*
	 * Defined square types.
	 *
	 * Works exactly like type_piece but for type_square structs.
	 */
/*@notnull@*/
	struct type_square	type_squares[(unsigned short)
						(HT_TYPE_MAX + 1)];

	/*
	 * Amount of defined type_squares.
	 *
	 * This does not equal the array length.
	 */
	unsigned short		type_squarec;

	/*
	 * Optimization.
	 *
	 * Bitmask of type_pieces with double_trap == HT_TRUE.
	 */
	HT_BIT_U8		opt_tp_dbl_trap;

	/*
	 * Optimization.
	 *
	 * Bitmask of type_pieces with double_trap_capt == HT_TRUE.
	 */
	HT_BIT_U8		opt_tp_dbl_trap_capt;

	/*
	 * Optimization.
	 *
	 * Bitmask of type_pieces with double_trap_compl == HT_TRUE.
	 */
	HT_BIT_U8		opt_tp_dbl_trap_compl;

	/*
	 * Optimization.
	 *
	 * Bitmask of type_square for which type_square.capt_sides
	 * overrides type_piece.capt_sides. If a square type's bit is
	 * among these, then type_square.capt_sides is a valid amount of
	 * sides that can override type_piece.capt_sides.
	 */
	HT_BIT_U8		opt_ts_capt_sides;

	/*
	 * bwidth * bheight.
	 */
	unsigned short		opt_blen;

	/*
	 * Bitmask of type_pieces with escape.
	 */
	HT_BIT_U8		opt_tp_escape;

	/*
	 * Bitmask of type_squares with escape.
	 */
	HT_BIT_U8		opt_ts_escape;

};

#endif

