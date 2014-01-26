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
#ifndef HT_TYPE_SQUARE_T
#define HT_TYPE_SQUARE_T

#include "config.h"		/* HT_RSTR */
#include "funct.h"		/* HT_FR */
#include "rulesett.h"		/* ruleset */
#include "type_squaret.h"	/* type_square */

/*
 * Value for capt_sides not overridden by type_square.
 */
/*@unchecked@*/
extern
unsigned short HT_TSQUARE_CAPT_SIDES_NONE;

extern
HT_BOOL
type_square_can_capture (
/*@in@*/
/*@notnull@*/
	const struct type_square * const HT_RSTR,
	const HT_BIT_U8
	)
/*@modifies nothing@*/
;

/*@-protoparamname@*/
extern
void
type_square_init (
/*@in@*/
/*@notnull@*/
	struct type_square * const HT_RSTR	ts
	)
/*@modifies * ts@*/
;
/*@=protoparamname@*/

/*@in@*/
/*@notnull@*/
extern
struct type_square *
type_square_get (
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
type_square_set (
/*@in@*/
/*@notnull@*/
	struct ruleset * const HT_RSTR	r,
	const HT_BIT_U8			bit
	)
/*@modifies * r@*/
;
/*@=protoparamname@*/

#endif

