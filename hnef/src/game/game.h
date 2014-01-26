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
#ifndef HT_GAME_H
#define HT_GAME_H

#include "funct.h"	/* HT_FR */
#include "gamet.h"	/* game */

/*@-protoparamname@*/
extern
enum HT_FR
game_board_reset (
/*@in@*/
/*@notnull@*/
	struct game * const g
	)
/*@modifies * g@*/
;
/*@=protoparamname@*/

/*@-protoparamname@*/
extern
void
game_initopt (
/*@in@*/
/*@notnull@*/
	struct game * const g
	)
/*@modifies * g@*/
;
/*@=protoparamname@*/

/*@-protoparamname@*/
extern
void
game_init (
/*@notnull@*/
/*@partial@*/
	struct game * const g
	)
/*@modifies * g@*/
;
/*@=protoparamname@*/

/*@null@*/
/*@only@*/
/*@partial@*/
extern
struct game *
alloc_game (
	const unsigned short,
	const unsigned short
	)
/*@modifies nothing@*/
;

/*@-protoparamname@*/
extern
void
free_game (
/*@notnull@*/
/*@owned@*/
/*@special@*/
	struct game * const g
	)
/*@modifies * g->players, g->players, g->rules, g->b, g@*/
/*@releases * g->players, g->players, g->rules, g->b, g@*/
;
/*@=protoparamname@*/

#endif

