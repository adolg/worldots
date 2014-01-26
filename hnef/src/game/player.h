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
#ifndef HT_PLAYER_H
#define HT_PLAYER_H

#include "boolt.h"	/* HT_BOOL */
#include "config.h"	/* HT_RSTR */
#include "gamet.h"	/* game */
#include "playert.h"	/* player */

/*@-protoparamname@*/
extern
enum HT_FR
player_reset (
/*@in@*/
/*@notnull@*/
	struct player * const	p
	)
/*@modifies * p@*/
;
/*@=protoparamname@*/

/*@-protoparamname@*/
extern
void
player_initopt (
/*@in@*/
/*@notnull@*/
	const struct game * const HT_RSTR	g,
/*@in@*/
/*@notnull@*/
	struct player * const HT_RSTR		p
	)
/*@modifies * p@*/
;
/*@=protoparamname@*/

/*@-protoparamname@*/
extern
HT_BOOL
player_init (
	struct player * const HT_RSTR	p,
	const unsigned short		index
	)
/*@modifies * p@*/
;
/*@=protoparamname@*/

extern
HT_BOOL
player_index_valid (
	const unsigned short
	)
/*@modifies nothing@*/
;

/*@null@*/
/*@only@*/
/*@partial@*/
extern
struct player *
alloc_player (void)
/*@modifies nothing@*/
;

/*@-protoparamname@*/
extern
void
free_player (
/*@notnull@*/
/*@owned@*/
/*@special@*/
	struct player * const	p
	)
/*@modifies p@*/
/*@releases p@*/
;
/*@=protoparamname@*/

#endif

