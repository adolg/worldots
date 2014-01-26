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
#ifdef HT_AI_MINIMAX_HASH

#ifndef HT_AIMHASH_H
#define HT_AIMHASH_H

#include "aimhasht.h"	/* zhashtable */
#include "config.h"	/* HT_RSTR */
#include "funct.h"	/* HT_FR */
#include "gamet.h"	/* game */

extern
unsigned int
zhashkey (
/*@in@*/
/*@notnull@*/
	const struct zhashtable * const HT_RSTR,
/*@in@*/
/*@notnull@*/
	const struct board * const HT_RSTR
	)
/*@modifies nothing@*/
;

extern
unsigned int
zhashlock (
/*@in@*/
/*@notnull@*/
	const struct zhashtable * const,
/*@in@*/
/*@notnull@*/
	const struct board * const
	)
/*@modifies nothing@*/
;

/*@-protoparamname@*/
extern
void
zhashtable_put (
/*@in@*/
/*@notnull@*/
	struct zhashtable * const HT_RSTR	ht,
/*@in@*/
/*@notnull@*/
	const struct board * const HT_RSTR	b,
	const int				value,
	const enum ZVALUE_TYPE			value_type,
	unsigned short				depthleft
	)
/*@modifies * ht@*/
;
/*@=protoparamname@*/

/*@dependent@*/
/*@in@*/
/*@null@*/
extern
struct zhashnode *
zhashtable_get (
/*@in@*/
/*@notnull@*/
/*@returned@*/
	const struct zhashtable * const HT_RSTR,
	const unsigned int,
	const unsigned int,
	const unsigned short
	)
/*@modifies nothing@*/
;

/*@-protoparamname@*/
extern
enum HT_FR
zhashtable_clearunused (
/*@in@*/
/*@notnull@*/
	struct zhashtable * const	ht
	)
/*@modifies * ht@*/
;
/*@=protoparamname@*/

/*@null@*/
/*@only@*/
extern
struct zhashtable *
alloc_zhashtable_init (
/*@in@*/
/*@notnull@*/
	const struct game * const HT_RSTR
	)
/*@globals errno, internalState@*/
/*@modifies errno, internalState@*/
;

/*@-protoparamname@*/
extern
void
free_zhashtable (
/*@notnull@*/
/*@owned@*/
/*@special@*/
	struct zhashtable * const ht
	)
/*@modifies ht@*/
/*@releases ht@*/
;
/*@=protoparamname@*/

#endif

#endif

