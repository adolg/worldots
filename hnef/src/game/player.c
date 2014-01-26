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
#include <stdlib.h>	/* malloc, free */

#ifdef HT_AI_MINIMAX_HASH
#include "aimhash.h"	/* zhashtable_clearunused */
#endif
#ifdef HT_AI_MINIMAX
#include "aiminimax.h"	/* free_aiminimax */
#endif
#include "player.h"

/*
 * This clears unused nodes in the minimax players' hash tables, setting
 * all used nodes to unused. Since this is also called every time when
 * aiminimax_command is called, it will remove all unused nodes that
 * were flagged in this function, and we don't need a new function in
 * aimhash.c for clearing moves regardless if they are used or not (it
 * would still entail the functions being called twice, thus no gain).
 */
enum HT_FR
player_reset (
	struct player * const	p
	)
{
#ifdef HT_AI_MINIMAX_HASH
	if (HT_AIT_MINIMAX == p->contr)
	{
		enum HT_FR	fr	= HT_FR_NONE;
#ifndef HT_UNSAFE
		if (NULL == p->ai.minimax)
		{
			return	HT_FR_FAIL_NULLPTR;
		}
#endif

		fr = zhashtable_clearunused(p->ai.minimax->tp_tab);
		if (HT_FR_SUCCESS != fr)
		{
			return	fr;
		}
	}
#endif
	return	HT_FR_SUCCESS;
}

/*
 * Checks if a player index is valid.
 */
HT_BOOL
player_index_valid (
	const unsigned short index
	)
/*@modifies nothing@*/
{
	return index < HT_PLAYERS_MAX ? HT_TRUE : HT_FALSE;
}

/*
 * Initializes optimization variables in p.
 */
void
player_initopt (
	const struct game * const HT_RSTR	g,
	struct player * const HT_RSTR		p
	)
{
	unsigned short	i;
	for (i = (unsigned short)0; i < g->rules->type_piecec; i++)
	{
		const struct type_piece * const HT_RSTR tp =
			& g->rules->type_pieces[i];
		if (tp->owner == p->index)
		{
			p->opt_owned = (HT_BIT_U8)
				((unsigned int)p->opt_owned
				| (unsigned int)tp->bit);
#ifdef HT_AI_MINIMAX
			if (tp->escape)
			{
				p->opt_owned_esc = (HT_BIT_U8)
					((unsigned int)p->opt_owned_esc
					| (unsigned int)tp->bit);
			}
#endif
		}
	}
}

/*
 * Returns HT_TRUE if values were initialized, or HT_FALSE if they were
 * invalid.
 *
 * In addition to p->index, p->contr is set to HT_AIT_NONE (human
 * player) and the union pointer is set to NULL.
 *
 * After the ruleset has been read and validated, player_initopt must
 * also be called.
 */
HT_BOOL
player_init (
	struct player * const HT_RSTR	p,
	const unsigned short		index
	)
{
#ifndef HT_UNSAFE
	if (NULL == p)
	{
		return HT_FALSE;
	}
#endif
	if (!player_index_valid(index))
	{
		return HT_FALSE;
	}
	p->index	= index;
	p->contr	= HT_AIT_NONE;
#ifdef HT_AI_MINIMAX
	p->ai.minimax	= NULL;
#endif
	p->opt_owned		= HT_BIT_U8_EMPTY;
#ifdef HT_AI_MINIMAX
	p->opt_owned_esc	= HT_BIT_U8_EMPTY;
#endif
	return HT_TRUE;
}

/*
 * The ai union pointer is set to NULL.
 */
struct player *
alloc_player (void)
{
	struct player * const HT_RSTR p	= malloc(sizeof(* p));
	if (NULL == p)
	{
		return	NULL;
	}

#ifdef HT_AI_MINIMAX
	p->ai.minimax	= NULL;
#endif

	return	p;
}

void
free_player (
	struct player * const	p
	)
{
#ifdef HT_AI_MINIMAX
	if (HT_AIT_MINIMAX == p->contr && p->ai.minimax != NULL)
	{
		free_aiminimax	(p->ai.minimax);
	}
#endif
	free	(p);
}

