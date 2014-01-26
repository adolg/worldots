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
#ifndef HT_TYPE_PIECE_H
#define HT_TYPE_PIECE_H

#include "config.h"		/* HT_RSTR */
#include "funct.h"		/* HT_FR */
#include "rulesett.h"		/* ruleset */
#include "type_piecet.h"	/* type_piece */

extern
HT_BOOL
type_piece_can_hammer (
/*@in@*/
/*@notnull@*/
	const struct type_piece * const HT_RSTR,
	const HT_BIT_U8
	)
/*@modifies nothing@*/
;

extern
HT_BOOL
type_piece_can_anvil (
/*@in@*/
/*@notnull@*/
	const struct type_piece * const HT_RSTR,
	const HT_BIT_U8
	)
/*@modifies nothing@*/
;

extern
HT_BOOL
type_piece_can_occupy (
/*@in@*/
/*@notnull@*/
	const struct type_piece * const HT_RSTR,
	const HT_BIT_U8
	)
/*@modifies nothing@*/
;

extern
HT_BOOL
type_piece_can_traverse (
/*@in@*/
/*@notnull@*/
	const struct type_piece * const HT_RSTR,
	const HT_BIT_U8
	)
/*@modifies nothing@*/
;

extern
HT_BOOL
type_piece_can_return (
/*@in@*/
/*@notnull@*/
	const struct type_piece * const HT_RSTR,
	const HT_BIT_U8,
	const HT_BIT_U8
	)
/*@modifies nothing@*/
;

/*@-protoparamname@*/
extern
void
type_piece_init (
/*@in@*/
/*@notnull@*/
	struct type_piece * const HT_RSTR	tp
	)
/*@modifies * tp@*/
;
/*@=protoparamname@*/

/*@in@*/
/*@notnull@*/
extern
struct type_piece *
type_piece_get (
/*@in@*/
/*@notnull@*/
/*@returned@*/
	struct ruleset * const HT_RSTR,
	const HT_BIT_U8
	)
/*@modifies nothing@*/
;

/*@-protoparamname@*/
extern
enum HT_FR
type_piece_set (
/*@in@*/
/*@notnull@*/
	struct ruleset * const HT_RSTR	r,
	const HT_BIT_U8			bit
	)
/*@modifies * r@*/
;
/*@=protoparamname@*/

#endif

