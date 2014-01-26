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
#ifdef HT_AI_MINIMAX

#ifndef HT_AIMINIMAX_T_H
#define HT_AIMINIMAX_T_H

#ifdef HT_AI_MINIMAX_HASH
#include "aimhasht.h"	/* zhashtable */
#endif

/*
 * Maximum value for aiminimax->depth_max.
 *
 * NOTE:	This must be a (compile-time) constant (because
 *		HT_AIM_VALUE_WIN in aimalgo.c needs it), hence define
 *		rather than const.
 */
#define HT_AIM_DEPTHMAX_MAX (unsigned short)1000

/*@unchecked@*/
extern
const unsigned short HT_AIM_DEPTHMAX_MIN;

/*@unchecked@*/
extern
const unsigned short HT_AIM_DEPTHMAX_UNINIT;

/*@exposed@*/
struct aiminimax
{

	/*
	 * Owning player's index.
	 */
	unsigned short		p_index;

	/*
	 * Maximum search depth.
	 */
	unsigned short		depth_max;

#ifdef HT_AI_MINIMAX_HASH
	/*
	 * Transposition table.
	 */
/*@dependent@*/
/*@in@*/
/*@notnull@*/
	struct zhashtable *	tp_tab;
#endif

	/*
	 * Array that has the same length as ruleset->opt_blen, which
	 * contains values for squares depending on their proximity to
	 * escape squares.
	 */
/*@in@*/
/*@notnull@*/
/*@owned@*/
	int * 			board_escval;

	/*
	 * Optimization.
	 *
	 * This is a list of moves that is derived from the original
	 * board passed to aiminimax_command, so we don't have to
	 * re-allocate it all the time.
	 */
/*@in@*/
/*@owned@*/
/*@notnull@*/
	struct listm *		opt_moves;

	/*
	 * Optimization.
	 *
	 * Move history.
	 */
/*@in@*/
/*@owned@*/
/*@notnull@*/
	struct listmh *		opt_movehist;

	/*
	 * Optimization.
	 *
	 * Pointer to a list of listm structs. The amount of listm
	 * structs is depth_max + 1.
	 *
	 * In the aiminimax_min() and aiminimax_max() functions, these
	 * lists are used (specifically: opt_buf_moves[depth_cur]) so
	 * they don't have to be re-allocated all the time.
	 * opt_buf_moves[0] is used in the initial call to
	 * aiminimax_min() in aiminimax_command().
	 */
/*@in@*/
/*@owned@*/
/*@notnull@*/
	struct listm * *	opt_buf_moves;

	/*
	 * Optimization.
	 *
	 * Pointer to a list of board structs. The amount of board
	 * structs is depth_max + 1.
	 *
	 * In the aiminimax_min() and aiminimax_max() functions, these
	 * boards are used (specifically: opt_buf_board[depth_cur]) so
	 * they don't have to be re-allocated all the time.
	 * opt_buf_board[0] is used in the initial call to
	 * aiminimax_min() in aiminimax_command().
	 */
/*@in@*/
/*@owned@*/
/*@notnull@*/
	struct board * *	opt_buf_board;

	/*
	 * Length of opt_buf_moves and opt_buf_board.
	 */
	unsigned short		opt_buf_len;

};

#endif

#endif

