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

#ifndef HT_AIMINIMAX_H
#define HT_AIMINIMAX_H

#include "aiminimaxt.h"	/* aiminimax */
#include "boolt.h"	/* HT_BOOL */
#include "config.h"	/* HT_RSTR */
#include "funct.h"	/* HT_FR */
#include "gamet.h"	/* game */

extern
HT_BOOL
aiminimax_depth_max_valid (
	const unsigned short
	)
/*@modifies nothing@*/
;

/*@-protoparamname@*/
/*@null@*/
/*@only@*/
/*@partial@*/
extern
struct aiminimax *
alloc_aiminimax_init (
/*@in@*/
/*@notnull@*/
	const struct game * const	g,
	const unsigned short		p_index,
	const unsigned short		depth_max,
/*@in@*/
/*@notnull@*/
	enum HT_FR * const HT_RSTR	fr
#ifdef HT_AI_MINIMAX_HASH
	,
/*@dependent@*/
/*@in@*/
/*@null@*/
	struct zhashtable * const	old_tab
#endif
	)
#ifdef HT_AI_MINIMAX_HASH
/*@globals errno, internalState@*/
#else
/*@globals internalState@*/
#endif
/*@modifies internalState, * fr@*/
#ifdef HT_AI_MINIMAX_HASH
/*@modifies errno, * old_tab@*/
#endif
;
/*@=protoparamname@*/

/*@-protoparamname@*/
extern
void
free_aiminimax (
/*@notnull@*/
/*@owned@*/
/*@special@*/
	struct aiminimax * const aim
	)
/*@modifies aim@*/
/*@releases aim@*/
;
/*@=protoparamname@*/

#endif

#endif

