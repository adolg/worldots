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
#ifndef HT_LISTM_H
#define HT_LISTM_H

#include "config.h"	/* HT_RSTR */
#include "funct.h"	/* HT_FR */
#include "listmt.h"	/* listm */

/*@null@*/
/*@only@*/
/*@partial@*/
extern
struct listm *
alloc_listm (
	const size_t
	)
/*@modifies nothing@*/
;

/*@-protoparamname@*/
extern
void
free_listm (
/*@notnull@*/
/*@owned@*/
/*@special@*/
	struct listm * const l
	)
/*@modifies l->elems, l@*/
/*@releases l->elems, l@*/
;
/*@=protoparamname@*/

/*@-protoparamname@*/
extern
enum HT_FR
listm_add (
/*@notnull@*/
/*@partial@*/
	struct listm * const l,
	const unsigned short,
	const unsigned short
	)
/*@modifies * l@*/
;
/*@=protoparamname@*/

/*@-protoparamname@*/
extern
void
listm_clear (
/*@notnull@*/
	struct listm * const HT_RSTR l
	)
/*@modifies * l@*/
;
/*@=protoparamname@*/

/*@-protoparamname@*/
extern
enum HT_FR
listm_swaptofr (
	struct listm * const HT_RSTR	l,
	const size_t			index
	)
/*@modifies * l@*/
;
/*@=protoparamname@*/

/*@-protoparamname@*/
extern
enum HT_FR
listm_remove (
	struct listm * const HT_RSTR	l,
	const size_t			index
	)
/*@modifies * l@*/
;
/*@=protoparamname@*/

#endif

