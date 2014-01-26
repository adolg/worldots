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
#ifndef HT_RULESET_H
#define HT_RULESET_H

#include "boolt.h"	/* HT_BOOL */
#include "config.h"	/* HT_RSTR */
#include "rulesett.h"	/* ruleset */

extern
HT_BOOL
valid_size (
	const unsigned short
	)
/*@modifies nothing@*/
;

/*@-protoparamname@*/
extern
void
ruleset_initopt (
/*@in@*/
/*@notnull@*/
	struct ruleset * const HT_RSTR r
	)
/*@modifies * r@*/
;
/*@=protoparamname@*/

/*@-protoparamname@*/
extern
void
ruleset_init (
/*@in@*/
/*@notnull@*/
	struct ruleset * const HT_RSTR r
	)
/*@modifies * r@*/
;
/*@=protoparamname@*/

/*@null@*/
/*@only@*/
/*@partial@*/
extern
struct ruleset *
alloc_ruleset (
	const unsigned short,
	const unsigned short
	)
/*@modifies nothing@*/
;

/*@-protoparamname@*/
extern
void
free_ruleset (
/*@notnull@*/
/*@owned@*/
/*@special@*/
	struct ruleset * const r
	)
/*@modifies r->pieces, r->squares, r@*/
/*@releases r->pieces, r->squares, r@*/
;
/*@=protoparamname@*/

#endif

