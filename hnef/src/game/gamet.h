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
#ifndef HT_GAME_T_H
#define HT_GAME_T_H

#include "listmht.h"	/* listmh */
#include "rulesett.h"	/* ruleset */

/*
 * Information about a game.
 *
 * *	Ruleset.
 *
 *	*	Initial piece layout array	(rules->pieces).
 *
 *	*	Squares array			(rules->squares).
 *
 *	*	Piece types.
 *
 *	*	Square types.
 *
 * *	Players.
 *
 * *	*	AI (maybe)			(players[i]->ai->...).
 *
 * *	Board (the ongoing game).
 *
 *	*	Player turn			(b->turn).
 *
 *	*	Pieces array			(b->pieces).
 */
struct game
{

/*@notnull@*/
/*@owned@*/
	struct ruleset *	rules;

	/*
	 * The ongoing game.
	 *
	 * This is never NULL.
	 */
/*@notnull@*/
/*@owned@*/
	struct board *		b;

/*@notnull@*/
/*@owned@*/
	/*
	 * Array of playerc players.
	 *
	 * This is a pointer to an array, rather than an array, because
	 * of alloc_player. We can't just malloc a bunch of players in
	 * alloc_game, since that would not fully allocate the player
	 * struct.
	 */
	struct player * *	players;

	/*
	 * This is always equal to HT_PLAYERS_MAX.
	 */
	unsigned short		playerc;

	/*
	 * A move history list.
	 *
	 * NOTE:	This list (and the corresponding list in the
	 *		aiminimax struct: aiminimax->opt_movehist) can
	 *		grow indefinitely. This can consume infinite
	 *		memory, but will of course only consume 8 bytes
	 *		per move (or however large a moveh is).
	 */
	struct listmh *		movehist;

};

#endif

