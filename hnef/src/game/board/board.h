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
#ifndef HT_BOARD_H
#define HT_BOARD_H

#ifdef HT_AI_MINIMAX
#include "aimhasht.h"	/* zhashtable */
#endif
#include "boardt.h"	/* board */
#include "config.h"	/* HT_RSTR */
#include "funct.h"	/* HT_FR */
#include "gamet.h"	/* game */
#include "listmt.h"	/* listm */
#include "player.h"	/* HT_PLAYERS_MAX */

#ifdef HT_AI_MINIMAX
/*@-protoparamname@*/
extern
void
board_copy (
/*@in@*/
/*@notnull@*/
	const struct board * const HT_RSTR	src,
/*@in@*/
/*@notnull@*/
	struct board * const HT_RSTR		dest,
	const unsigned short			blen
	)
/*@modifies * dest@*/
;
/*@=protoparamname@*/
#endif /* HT_AI_MINIMAX */

/*@-protoparamname@*/
extern
enum HT_FR
board_move_unsafe (
/*@in@*/
/*@notnull@*/
	const struct game * const HT_RSTR	g,
/*@in@*/
/*@notnull@*/
	struct board * const			b,
/*@in@*/
/*@notnull@*/
	struct listmh * const			movehist,
	const unsigned short			pos,
	const unsigned short			dest
#ifdef HT_AI_MINIMAX_HASH
	,
/*@in@*/
/*@null@*/
	const struct zhashtable * const HT_RSTR	hashtable,
/*@in@*/
/*@null@*/
	unsigned int * const			hashkey,
/*@in@*/
/*@null@*/
	unsigned int * const			hashlock
#endif
	)
/*@modifies * b, * movehist@*/
#ifdef HT_AI_MINIMAX
/*@modifies * hashkey, * hashlock@*/
#endif
;
/*@=protoparamname@*/

/*@-protoparamname@*/
extern
enum HT_FR
game_move (
/*@in@*/
/*@notnull@*/
	struct game * const HT_RSTR	g,
	const unsigned short		pos,
	const unsigned short		dest,
/*@out@*/
/*@notnull@*/
	HT_BOOL * const HT_RSTR		legal
	)
/*@modifies * g, * legal@*/
;
/*@=protoparamname@*/

#ifdef HT_AI_MINIMAX

/*@-protoparamname@*/
extern
enum HT_FR
board_moves_get (
/*@in@*/
/*@notnull@*/
	const struct game * const HT_RSTR	g,
/*@in@*/
/*@notnull@*/
	const struct board * const HT_RSTR	b,
/*@in@*/
/*@notnull@*/
	const struct listmh * const HT_RSTR	movehist,
/*@in@*/
/*@notnull@*/
	struct listm * const			list
	)
/*@modifies * list@*/
;
/*@=protoparamname@*/

#endif /* HT_AI_MINIMAX */

extern
int
board_movec_get (
/*@in@*/
/*@notnull@*/
	const struct game * const HT_RSTR,
/*@in@*/
/*@notnull@*/
	const struct board * const HT_RSTR,
/*@in@*/
/*@notnull@*/
	const struct listmh * const HT_RSTR,
	const HT_BIT_U8
	)
/*@modifies nothing@*/
;

/*@-protoparamname@*/
HT_BOOL
board_game_over (
/*@in@*/
/*@notnull@*/
	const struct game * const HT_RSTR	g,
/*@in@*/
/*@notnull@*/
	const struct board * const HT_RSTR	b,
/*@in@*/
/*@notnull@*/
	const struct listmh * const HT_RSTR	movehist,
/*@in@*/
/*@notnull@*/
	unsigned short * const			winner
	)
/*@modifies * winner@*/
;
/*@=protoparamname@*/

/*@-protoparamname@*/
HT_BOOL
game_over (
/*@in@*/
/*@notnull@*/
	const struct game * const HT_RSTR	g,
/*@in@*/
/*@notnull@*/
	unsigned short * const			winner
	)
/*@modifies * winner@*/
;
/*@=protoparamname@*/

/*@-protoparamname@*/
extern
void
board_init (
	struct board * const	b,
	const unsigned short	blen
	)
/*@modifies b->turn, b->pieces@*/
;
/*@=protoparamname@*/

/*@null@*/
/*@only@*/
/*@partial@*/
extern
struct board *
alloc_board (
	const unsigned short
	)
/*@modifies nothing@*/
;

/*@-protoparamname@*/
extern
void
free_board (
/*@notnull@*/
/*@owned@*/
/*@special@*/
	struct board * const b
	)
/*@modifies b->pieces, b@*/
/*@releases b->pieces, b@*/
;
/*@=protoparamname@*/

#endif

