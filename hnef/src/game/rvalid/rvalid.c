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
#include <string.h>		/* strlen */

#include "board.h"		/* game_over */
#include "lang.h"		/* HT_L_* */
#include "num.h"		/* single_bit */
#include "playert.h"		/* HT_PLAYER_UNINIT */
#include "rvalid.h"
#include "type_piece.h"		/* type_piece_get */
#include "type_square.h"	/* type_square_get */

/*
 * Returns false if:
 *
 * *	ruleset->path is a 0-length string.
 */
static
HT_BOOL
ruleset_valid_gen (
/*@in@*/
/*@notnull@*/
	const struct game * const HT_RSTR	g
	)
/*@modifies nothing@*/
{
	if (strlen(g->rules->path) < (size_t)1)
	{
		return	HT_FALSE;
	}

	return	HT_TRUE;
}

/*
 * Checks every bit in the bit array, and returns false if any bit:
 *
 * *	Is not 0 or a single bit.
 */
static
HT_BOOL
ruleset_valid_bitarr (
/*@in@*/
/*@notnull@*/
	const HT_BIT_U8 * const HT_RSTR	arr,
	const unsigned short		arrlen
	)
/*@modifies nothing@*/
{
	unsigned short	i;
	for (i = (unsigned short)0; i < arrlen; i++)
	{
		const HT_BIT_U8 bit = arr[i];
		if (HT_BIT_U8_EMPTY != bit
		&& !single_bit((unsigned int)bit))
		{
			return	HT_FALSE;
		}
	}
	return	HT_TRUE;
}

/*
 * Returns false if:
 *
 * *	Any bit in r->pieces is not among r->type_pieces.
 *
 * *	Any type_piece does not appear in r->pieces.
 *
 * Assumes that all bits in r->pieces are single bits or
 * HT_BIT_U8_EMPTY.
 */
static
HT_BOOL
ruleset_valid_bits_def_piece (
/*@in@*/
/*@notnull@*/
	struct ruleset * const	r
	)
/*@modifies nothing@*/
{
	unsigned short	i;
	for (i = (unsigned short)0; i < r->opt_blen; i++)
	{
		const HT_BIT_U8 pbit = r->pieces[i];
		if (HT_BIT_U8_EMPTY != pbit
		&& HT_BIT_U8_EMPTY == type_piece_get(r, pbit)->bit)
		{
			return	HT_FALSE;
		}
	}

	for (i = (unsigned short)0; i < r->type_piecec; i++)
	{
		unsigned short j;
		const struct type_piece * const HT_RSTR tp =
			& r->type_pieces[i];
		for (j = (unsigned short)0; j < r->opt_blen; j++)
		{
			if (r->pieces[j] == tp->bit)
			{
				goto	CONTINUE_LOOP;
			}
		}
		return	HT_FALSE;
		CONTINUE_LOOP:;
	}
	return	HT_TRUE;
}

/*
 * Returns false if:
 *
 * *	Any bit in r->squares is not among r->type_squares.
 *
 * *	Any type_square does not appear in r->squares.
 *
 * Assumes that all bits in r->squares are single bits or
 * HT_BIT_U8_EMPTY.
 */
static
HT_BOOL
ruleset_valid_bits_def_square (
/*@in@*/
/*@notnull@*/
	struct ruleset * const	r
	)
/*@modifies nothing@*/
{
	unsigned short	i;
	for (i = (unsigned short)0; i < r->opt_blen; i++)
	{
		const HT_BIT_U8 sqbit = r->squares[i];
		if (HT_BIT_U8_EMPTY != sqbit
		&& HT_BIT_U8_EMPTY == type_square_get(r, sqbit)->bit)
		{
			return	HT_FALSE;
		}
	}

	for (i = (unsigned short)0; i < r->type_squarec; i++)
	{
		unsigned short j;
		const struct type_square * const HT_RSTR ts =
			& r->type_squares[i];
		for (j = (unsigned short)0; j < r->opt_blen; j++)
		{
			if (r->squares[j] == ts->bit)
			{
				goto	CONTINUE_LOOP;
			}
		}
		return	HT_FALSE;
		CONTINUE_LOOP:;
	}
	return	HT_TRUE;
}

/*
 * Checks if all of the bits in bitmask are among r->type_pieces. If so,
 * returns true; else (if some is not present) returns false.
 */
static
HT_BOOL
bits_among_tpieces (
/*@in@*/
/*@notnull@*/
	const struct ruleset * const HT_RSTR	r,
	const HT_BIT_U8				bitmask
	)
/*@modifies nothing@*/
{
	unsigned short bit;
	for (bit = (unsigned short)1; bit <= (unsigned short)128;
		bit = (unsigned short)(bit * 2))
	{
		unsigned short i;
		if (((unsigned int)bit & (unsigned int)bitmask)
			!= (unsigned int)bit)
		{
			continue;
		}
		for (i = (unsigned short)0; i < r->type_piecec; i++)
		{
			if (r->type_pieces[i].bit == (HT_BIT_U8)bit)
			{
				goto	CONTINUE_LOOP;
			}
		}
		return	HT_FALSE;
		CONTINUE_LOOP:;
	}
	return	HT_TRUE;
}

/*
 * Checks if all of the bits in bitmask are among r->type_squares. If
 * so, returns true; else (if some is not present) returns false.
 */
static
HT_BOOL
bits_among_tsquares (
/*@in@*/
/*@notnull@*/
	const struct ruleset * const HT_RSTR	r,
	const HT_BIT_U8				bitmask
	)
/*@modifies nothing@*/
{
	unsigned short bit;
	for (bit = (unsigned short)1; bit <= (unsigned short)128;
		bit = (unsigned short)(bit * 2))
	{
		unsigned short i;
		if (((unsigned int)bit & (unsigned int)bitmask)
			!= (unsigned int)bit)
		{
			continue;
		}
		for (i = (unsigned short)0; i < r->type_squarec; i++)
		{
			if (r->type_squares[i].bit == (HT_BIT_U8)bit)
			{
				goto	CONTINUE_LOOP;
			}
		}
		return	HT_FALSE;
		CONTINUE_LOOP:;
	}
	return	HT_TRUE;
}

/*
 * Returns false if:
 *
 * *	Any piece in tp->captures or tp->anvils is owned by tp->owner.
 */
static
HT_BOOL
ruleset_valid_tpiece_captures (
/*@in@*/
/*@notnull@*/
	const struct game * const	g,
/*@in@*/
/*@notnull@*/
	const struct type_piece * const tp
	)
/*@modifies nothing@*/
{
	unsigned short bit;
	for (bit = (unsigned short)1; bit <= (unsigned short)128;
		bit = (unsigned short)(bit * 2))
	{
		if (((unsigned int)bit & (unsigned int)tp->captures)
			!= (unsigned int)bit)
		{
			continue;
		}
		if (type_piece_get(g->rules, (HT_BIT_U8)bit)->owner
			== tp->owner)
		{
			return	HT_FALSE;
		}
	}
	for (bit = (unsigned short)1; bit <= (unsigned short)128;
		bit = (unsigned short)(bit * 2))
	{
		if (((unsigned int)bit & (unsigned int)tp->anvils)
			!= (unsigned int)bit)
		{
			continue;
		}
		if (type_piece_get(g->rules, (HT_BIT_U8)bit)->owner
			== tp->owner)
		{
			return	HT_FALSE;
		}
	}
	return	HT_TRUE;
}

/*
 * Returns false if:
 *
 * *	bit is not in arr.
 */
static
HT_BOOL
ruleset_valid_bit_in_arr (
/*@in@*/
/*@notnull@*/
	const HT_BIT_U8 * const HT_RSTR	arr,
	const unsigned short		arrlen,
	const HT_BIT_U8			bit
	)
/*@modifies nothing@*/
{
	unsigned short i;
	for (i = (unsigned short)0; i < arrlen; i++)
	{
		if (bit == arr[i])
		{
			return	HT_TRUE;
		}
	}
	return	HT_FALSE;
}

/*
 * Returns false if the player has no pieces on the board.
 */
static
HT_BOOL
ruleset_valid_player_pieces (
/*@in@*/
/*@notnull@*/
	const struct game * const	g,
	unsigned short			p_index
	)
/*@modifies nothing@*/
{
	unsigned short pos;
	for (pos = (unsigned short)0; pos < g->rules->opt_blen; pos++)
	{
		const HT_BIT_U8 pbit = g->rules->pieces[pos];
		if (HT_BIT_U8_EMPTY != pbit
		&& type_piece_get(g->rules, pbit)->owner == p_index)
		{
			return	HT_TRUE;
		}
	}
	return	HT_FALSE;
}

/*
 * Returns false if:
 *
 * *	Some piece owned by p_index has dbl_trap, but no piece owned by
 *	p_index has dbl_trap_capt.
 *
 * *	Some piece owned by p_index has dbl_trap_compl, but no piece
 *	owned by p_index is without dbl_trap_compl.
 */
static
HT_BOOL
ruleset_valid_player_dtrap (
/*@in@*/
/*@notnull@*/
	const struct game * const	g,
	unsigned short			p_index
	)
/*@modifies nothing@*/
{
	unsigned short i;
	for (i = (unsigned short)0; i < g->rules->type_piecec; i++)
	{
		const struct type_piece * const HT_RSTR tp =
			& g->rules->type_pieces[i];
		if (tp->owner == p_index && tp->dbl_trap)
		{
			unsigned short j = (unsigned short)0;
			for (; j < g->rules->type_piecec; j++)
			{
				const struct type_piece * const HT_RSTR
					tp2 =
					& g->rules->type_pieces[j];
				if (tp2->owner == p_index
				&& tp2->dbl_trap_capt)
				{
					goto BREAKLOOP;
				}
			}
			/*
			 * A piece has dbl_trap, but no piece has
			 * dbl_trap_capt.
			 */
			return	HT_FALSE;
		}
	}
	BREAKLOOP:;

	for (i = (unsigned short)0; i < g->rules->type_piecec; i++)
	{
		const struct type_piece * const HT_RSTR tp =
			& g->rules->type_pieces[i];
		if (tp->owner == p_index && tp->dbl_trap_compl)
		{
			unsigned short j = (unsigned short)0;
			for (; j < g->rules->type_piecec; j++)
			{
				const struct type_piece * const HT_RSTR
					tp2 =
					& g->rules->type_pieces[j];
				if (tp2->owner == p_index
				&& !tp2->dbl_trap_compl)
				{
					goto BREAKLOOP2;
				}
			}
			/*
			 * A piece has dbl_trap_compl, but no piece is
			 * without dbl_trap_compl.
			 */
			return	HT_FALSE;
		}
	}
	BREAKLOOP2:;

	return	HT_TRUE;
}

static
HT_BOOL
ruleset_valid_player (
/*@in@*/
/*@notnull@*/
	const struct game * const	g,
	unsigned short			p_index
	)
/*@modifies nothing@*/
{
	if (	!ruleset_valid_player_pieces	(g, p_index)
	||	!ruleset_valid_player_dtrap	(g, p_index))
	{
		return	HT_FALSE;
	}

	return	HT_TRUE;
}

static
HT_BOOL
ruleset_valid_players (
/*@in@*/
/*@notnull@*/
	const struct game * const	g
	)
/*@modifies nothing@*/
{
	unsigned short i;
	for (i = (unsigned short)0; i < g->playerc; i++)
	{
		if (!ruleset_valid_player(g, i))
		{
			return	HT_FALSE;
		}
	}
	return	HT_TRUE;
}

/*
 * Returns false if:
 *
 * *	Any piece bit in g->rules->pieces can not occupy the
 *	corresponding bit in g->rules->squares, id est if a piece starts
 *	on a square that he can not occupy.
 *
 *	This returns false if any piece starts on an off-board (0)
 *	square.
 *
 * *	Any piece bit in g->rules->pieces stands on a square that he can
 *	escape to.
 */
static
HT_BOOL
ruleset_valid_tpieces_arr (
/*@in@*/
/*@notnull@*/
	const struct game * const	g
	)
/*@modifies nothing@*/
{
	unsigned short i;
	for (i = (unsigned short)0; i < g->rules->opt_blen; i++)
	{
		const HT_BIT_U8		pbit	= g->rules->pieces[i];
		const HT_BIT_U8		sqbit	= g->rules->squares[i];
		const struct type_piece * HT_RSTR tp	= NULL;
		if (HT_BIT_U8_EMPTY == pbit)
		{
			continue;
		}
		tp	= type_piece_get(g->rules, pbit);

		if (!type_piece_can_occupy(tp, sqbit))
		{
			return	HT_FALSE;
		}

		if ((((unsigned int)pbit
			& (unsigned int)g->rules->opt_tp_escape)
			== (unsigned int)pbit)
		&& (((unsigned int)sqbit
			& (unsigned int)g->rules->opt_ts_escape)
			== (unsigned int)sqbit))
		{
			return	HT_FALSE;
		}
	}
	return	HT_TRUE;
}

/*
 * Validates a single type_piece.
 *
 * Returns false if:
 *
 * *	Owner index is out of game->playerc bounds (owned does not
 *	exist).
 *
 * *	Any bit in tp->captures is not a defined type_piece.
 *
 * *	Any bit in tp->noreturn is not a defined type_square.
 *
 * *	Any bit in tp->occupies is not a defined type_square.
 *
 * *	Any bit in tp->noreturn or tp->occupies does not exist in
 *	g->rules->squares.
 *
 * *	Any bit in tp->noreturn is not occupiable by tp.
 */
static
HT_BOOL
ruleset_valid_tpiece (
/*@in@*/
/*@notnull@*/
	const struct game * const	g,
/*@in@*/
/*@notnull@*/
	const struct type_piece * const tp
	)
/*@modifies nothing@*/
{
	unsigned short bit;
	if (tp->owner >= g->playerc || tp->owner == HT_PLAYER_UNINIT)
	{
		return	HT_FALSE;
	}

	if (	!bits_among_tpieces	(g->rules, tp->captures)
	||	!bits_among_tpieces	(g->rules, tp->anvils)
	||	!bits_among_tsquares	(g->rules, tp->noreturn)
	||	!bits_among_tsquares	(g->rules, tp->occupies))
	{
		return	HT_FALSE;
	}

	if (	tp->capt_sides < HT_TPIECE_CAPT_SIDES_MIN
	||	tp->capt_sides > HT_TPIECE_CAPT_SIDES_MAX)
	{
		return	HT_FALSE;
	}

	if (!ruleset_valid_tpiece_captures(g, tp))
	{
		return	HT_FALSE;
	}

	if (tp->custodial && tp->capt_sides < (unsigned short)2)
	{
		return	HT_FALSE;
	}

	if (tp->capt_edge && tp->capt_sides < (unsigned short)2)
	{
		return	HT_FALSE;
	}

	if (!ruleset_valid_bit_in_arr(g->rules->pieces,
		g->rules->opt_blen, tp->bit))
	{
		return	HT_FALSE;
	}

	if ((tp->dbl_trap_capt || tp->dbl_trap_compl
		|| tp->dbl_trap_edge || tp->dbl_trap_encl)
	&& !tp->dbl_trap)
	{
		return	HT_FALSE;
	}

	/*
	 * Check that all noreturn and occupies squares exist and that
	 * all squares in noreturn are occupiable by tp.
	 */
	for (bit = (unsigned short)1; bit <= (unsigned short)128;
		bit = (unsigned short)(bit * 2))
	{
		if ((((unsigned int)tp->noreturn
			| (unsigned int)tp->occupies)
			& (unsigned int)bit) == (unsigned int)bit)
		{
			/*
			 * bit is in noreturn or occupies.
			 */
			if (!ruleset_valid_bit_in_arr(g->rules->squares,
				g->rules->opt_blen, (HT_BIT_U8)bit))
			{
				return	HT_FALSE;
			}
		}

		if (((unsigned int)tp->noreturn
			& (unsigned int)bit) == (unsigned int)bit)
		{
			/*
			 * bit is in noreturn.
			 */
			if (!type_piece_can_occupy(tp, (HT_BIT_U8)bit))
			{
				return	HT_FALSE;
			}
		}
	}

	return	HT_TRUE;
}

/*
 * Validates all type_pieces.
 */
static
HT_BOOL
ruleset_valid_tpieces (
/*@in@*/
/*@notnull@*/
	const struct game * const	g
	)
/*@modifies nothing@*/
{
	unsigned short i;
	for (i = (unsigned short)0; i < g->rules->type_piecec; i++)
	{
		if (!ruleset_valid_tpiece(g,
			& g->rules->type_pieces[i]))
		{
			return	HT_FALSE;
		}
	}
	return	HT_TRUE;
}

/*
 * Validates a single type_square.
 *
 * Returns false if:
 *
 * *	Any bit in ts->captures is not a defined type_piece.
 */
static
HT_BOOL
ruleset_valid_tsquare (
/*@in@*/
/*@notnull@*/
	const struct game * const	g,
/*@in@*/
/*@notnull@*/
	const struct type_square * const ts
	)
/*@modifies nothing@*/
{
	if (!bits_among_tpieces(g->rules, ts->captures))
	{
		return	HT_FALSE;
	}

	if (ts->capt_sides != HT_TSQUARE_CAPT_SIDES_NONE
	&& 	(	ts->capt_sides < HT_TPIECE_CAPT_SIDES_MIN
		||	ts->capt_sides > HT_TPIECE_CAPT_SIDES_MAX))
	{
		return	HT_FALSE;
	}

	if (!ruleset_valid_bit_in_arr(g->rules->squares,
		g->rules->opt_blen, ts->bit))
	{
		return	HT_FALSE;
	}

	return	HT_TRUE;
}

/*
 * Validates all type_squares.
 */
static
HT_BOOL
ruleset_valid_tsquares (
/*@in@*/
/*@notnull@*/
	const struct game * const	g
	)
/*@modifies nothing@*/
{
	unsigned short i;
	for (i = (unsigned short)0; i < g->rules->type_squarec; i++)
	{
		if (!ruleset_valid_tsquare(g,
			& g->rules->type_squares[i]))
		{
			return	HT_FALSE;
		}
	}
	return	HT_TRUE;
}

/*
 * Checks that if a type_piece has escape, then a type_square must have
 * it too, and vice versa.
 *
 * However, it doesn't check that a piece with escape can occupy some
 * square with escape.
 */
static
HT_BOOL
ruleset_valid_escape (
/*@in@*/
/*@notnull@*/
	const struct ruleset * const	r
	)
/*@modifies nothing@*/
{
	return (	HT_BIT_U8_EMPTY == r->opt_tp_escape)
		== (	HT_BIT_U8_EMPTY == r->opt_ts_escape);
}

/*
 * Checks if the game is over for either player. If so, returns false.
 *
 * g->b is temporarily modified, but it's restored before returning.
 *
 * NOTE:	This function assumes that there are only two players in
 *		the game.
 */
static
HT_BOOL
ruleset_valid_initpos (
/*@in@*/
/*@notnull@*/
	struct game * const	g
	)
/*@modifies * g@*/
{
	/*
	 * winner is ignored.
	 */
	unsigned short winner		= HT_PLAYER_UNINIT;
	unsigned short turn_init	= g->b->turn;

	if (game_over(g, & winner))
	{
		return	HT_FALSE;
	}

	g->b->turn = (unsigned short)
		(0 == g->b->turn ? (unsigned short)1 : 0);
	if (game_over(g, & winner))
	{
		g->b->turn	= turn_init;
		return		HT_FALSE;
	}

	g->b->turn	= turn_init;
	return		HT_TRUE;
}

/*
 * Returns NULL if g is invalid, or a string explaining why g is invalid
 * if it's not valid.
 *
 * Call this after reading a game object (from a ruleset file) using
 * rreader.c.
 *
 * Refer to the individual functions in this file to see exactly what's
 * checked.
 *
 * Although this function modifies g, it restores g to its original
 * state before returning.
 */
const char *
ruleset_valid (
	struct game * const	g
	)
{
	if (!ruleset_valid_gen(g))
	{
		return	HT_L_RVALID_GEN;
	}
	else if (!ruleset_valid_bitarr(g->rules->squares,
		g->rules->opt_blen)
	||	!ruleset_valid_bitarr(g->rules->pieces,
		g->rules->opt_blen))
	{
		return	HT_L_RVALID_TYPE_INVALID;
	}
	else if (!ruleset_valid_bits_def_piece(g->rules)
	||	!ruleset_valid_bits_def_square(g->rules))
	{
		return	HT_L_RVALID_TYPE_UNDEF;
	}
	else if (!ruleset_valid_tpieces_arr(g))
	{
		return	HT_L_RVALID_PIECEARR;
	}
	else if (!ruleset_valid_tpieces(g)
	||	!ruleset_valid_tsquares(g)
	||	!ruleset_valid_escape(g->rules))
	{
		return	HT_L_RVALID_TYPE_INVALID;
	}
	else if (!ruleset_valid_players(g))
	{
		return	HT_L_RVALID_PLAYERS;
	}
	else if (!ruleset_valid_initpos(g))
	{
		return	HT_L_RVALID_INITPOS;
	}
	else
	{
		return	NULL;
	}
}

