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
#ifndef H_PLAYER_T_H
#define H_PLAYER_T_H

#include "aiminimaxt.h"	/* aiminimax */
#include "ait.h"	/* aitype */
#include "boardt.h"	/* HT_BIT_U8 */

/*@unchecked@*/
extern
const unsigned short HT_PLAYERS_MAX;

/*@unchecked@*/
extern
const unsigned short HT_PLAYER_UNINIT;

/*@exposed@*/
struct player
{

#ifdef HT_AI_MINIMAX
	/*
	 * Contains a pointer to a computer player.
	 *
	 * This pointer is NULL if contr is HT_AIT_NONE (human player)
	 * or HT_AIT_UNK (uninitialized).
	 *
	 * contr		Available union
	 * HT_AIT_MINIMAX	minimax
	 */
	union
	{

		/*
		 * Pointer to minimax computer player.
		 */
/*@null@*/
		struct aiminimax *	minimax;

	} ai;
#endif

	/*
	 * Player index, from 0 to HT_PLAYERS_MAX - 1.
	 *
	 * The index must be valid according to player_index_valid.
	 */
	unsigned short			index;

	/*
	 * Type of AI player.
	 *
	 * HT_AIT_NONE	== human.
	 * HT_AIT_UNK	== uninitialized.
	 */
	enum AITYPE			contr;

	/*
	 * Optimization.
	 *
	 * Bitmask of pieces owned by the player. Set by player_initopt.
	 */
	HT_BIT_U8			opt_owned;

	/*
	 * Optimization.
	 *
	 * The computer player benefits from having this pre-calculated.
	 */
#ifdef HT_AI_MINIMAX
	HT_BIT_U8			opt_owned_esc;
#endif

};

#endif

