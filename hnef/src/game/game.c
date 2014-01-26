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
#include <string.h>	/* memcpy */

#include "game.h"
#include "board.h"	/* alloc_board, free_board */
#include "listmh.h"	/* alloc_listmh, free_listmh */
#include "player.h"	/* alloc_player, free_player, ... */
#include "ruleset.h"	/* alloc_ruleset, free_ruleset */

/*
 * Resets the board struct (g->b) based on the ruleset.
 *
 * This is equivalent to "starting a new game". The function can't fail.
 */
enum HT_FR
game_board_reset (
	struct game * const g
	)
{
	const struct ruleset * const r = g->rules;

	memcpy	(g->b->pieces, r->pieces, (size_t)g->rules->opt_blen);
	g->b->turn	= (unsigned short)0;

	{
		unsigned short	i;
		for (i = (unsigned short)0; i < g->playerc; i++)
		{
			const enum HT_FR fr =
				player_reset(g->players[i]);
			if (HT_FR_SUCCESS != fr)
			{
				return	fr;
			}
		}
	}

	/*
	 * Remove all history:
	 */
	g->movehist->elemc	= 0;

	return	HT_FR_SUCCESS;
}

/*
 * Initializes optimization for the game.
 *
 * This can be called on an invalid ruleset.
 */
void
game_initopt (
	struct game * const g
	)
{
	unsigned short i;
	for (i = (unsigned short)0; i < g->playerc; i++)
	{
		player_initopt	(g, g->players[i]);
	}

	ruleset_initopt	(g->rules);
}

/*
 * Initializes all variables in g to their default values.
 *
 * Once the ruleset has been read and validated, game_initopt also has
 * to be called.
 */
void
game_init (
	struct game * const g
	)
{
	board_init	(g->b, g->rules->opt_blen);
	ruleset_init	(g->rules);
}

/*
 * Returns NULL if failing to allocate.
 *
 * If successful, then game->playerc is set to HT_PLAYERS_MAX. The
 * players are allocated and initialized according to player_init.
 *
 * You must still initialize the rest of the game after calling this
 * function (the players are only initialized because their player
 * indices correspond to the array in struct game, and then we may as
 * well fully initialize them).
 *
 * These variables are fully allocated, but not initialized:
 *
 * *	game->rules
 *
 * *	game->b
 */
struct game *
alloc_game (
	const unsigned short	bwidth,
	const unsigned short	bheight
	)
{
	unsigned short		i;
	const unsigned short blen = (unsigned short)(bwidth * bheight);
	struct game * const	g	= malloc(sizeof(* g));
	if (NULL == g)
	{
		return	NULL;
	}

	g->rules	= alloc_ruleset(bwidth, bheight);
	if (NULL == g->rules)
	{
		free	(g);
		return	NULL;
	}

	/*
	 * This is set later and uses ruleset information.
	 */
	g->b	= alloc_board(blen);
	if (NULL == g->b)
	{
		free_ruleset	(g->rules);
		free		(g);
		return		NULL;
	}

	g->movehist	= alloc_listmh(HT_LISTMH_CAP_DEF);
	if (NULL == g->movehist)
	{
		free_board	(g->b);
		free_ruleset	(g->rules);
		free		(g);
		return		NULL;
	}

	g->playerc = HT_PLAYERS_MAX;
	g->players = malloc(sizeof(* g->players) * (size_t)g->playerc);
	if (NULL == g->players)
	{
		free_listmh	(g->movehist);
		free_board	(g->b);
		free_ruleset	(g->rules);
		free		(g);
		return		NULL;
	}
	for (i = (unsigned short)0; i < g->playerc; i++)
	{
		g->players[i]	= alloc_player();
		if (NULL == g->players[i]
		|| !player_init(g->players[i], i))
		{
			unsigned short j;
			for (j = (unsigned short)0; j <= i
/* Check added for GCC's loop optimizations: */
				&& j <= HT_PLAYERS_MAX
				; j++)
			{
				if (NULL != g->players[j])
				{
					free_player(g->players[j]);
				}
			}
			free		(g->players);
			free_listmh	(g->movehist);
			free_board	(g->b);
			free_ruleset	(g->rules);
			free		(g);
			return		NULL;
		}
	}

	return	g;
}

void
free_game (
	struct game * const g
	)
{
	if (NULL != g->b)
	{
		free_board	(g->b);
	}

	if (NULL != g->rules)
	{
		free_ruleset	(g->rules);
	}

	if (NULL != g->movehist)
	{
		free_listmh	(g->movehist);
	}

	if (NULL != g->players)
	{
		unsigned short i;
		for (i = (unsigned short)0; i < g->playerc; i++)
		{
			if (NULL != g->players[i])
			{
				free_player	(g->players[i]);
			}
		}
		free		(g->players);
/* splint thinks g->rules isn't released "if (NULL != g->players)". */
/*@i2@*/\
	}

	free	(g);
}

