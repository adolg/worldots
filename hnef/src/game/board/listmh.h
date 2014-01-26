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
#ifndef HT_LISTMH_H
#define HT_LISTMH_H

#include "config.h"	/* HT_RSTR */
#include "funct.h"	/* HT_FR */
#include "listmht.h"	/* listmh */

/*@null@*/
/*@only@*/
/*@partial@*/
extern
struct listmh *
alloc_listmh (
	const size_t
	)
/*@modifies nothing@*/
;

/*@-protoparamname@*/
extern
void
free_listmh (
/*@notnull@*/
/*@owned@*/
/*@special@*/
	struct listmh * const l
	)
/*@modifies l->elems, l@*/
/*@releases l->elems, l@*/
;
/*@=protoparamname@*/

/*@-protoparamname@*/
extern
enum HT_FR
listmh_add (
/*@notnull@*/
/*@partial@*/
	struct listmh * const l,
	const unsigned short,
	const unsigned short,
	const HT_BOOL
	)
/*@modifies * l@*/
;
/*@=protoparamname@*/

/*@-protoparamname@*/
extern
enum HT_FR
listmh_copy (
/*@in@*/
/*@notnull@*/
	const	struct listmh * const	src,
/*@in@*/
/*@notnull@*/
		struct listmh * const	dest
	)
/*@modifies * dest@*/
;
/*@=protoparamname@*/

#endif

