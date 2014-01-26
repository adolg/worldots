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
#ifndef HT_IOTEXTPARSE_H
#define HT_IOTEXTPARSE_H

#include <stdio.h>		/* FILE */

#include "boolt.h"		/* HT_BOOL */
#include "funct.h"		/* HT_FR */
#include "iotextparset.h"	/* ht_line */

/*@null@*/
/*@only@*/
/*@partial@*/
extern
struct ht_line *
alloc_ht_line (
	const size_t,
	const size_t
	)
/*@modifies nothing@*/
;

/*@-protoparamname@*/
extern
enum HT_FR
ht_line_readline (
	FILE * const		f,
	struct ht_line * const	l
	)
/*@globals fileSystem, errno@*/
/*@modifies fileSystem, errno, * f, * l@*/
;
/*@=protoparamname@*/

/*@-protoparamname@*/
extern
void
free_ht_line (
/*@notnull@*/
/*@owned@*/
/*@special@*/
	struct ht_line * const l
	)
/*@modifies l->words, l@*/
/*@releases l->words, l@*/
;
/*@=protoparamname@*/

extern
HT_BOOL
ht_line_empty (
/*@notnull@*/
	const struct ht_line * const
	)
/*@modifies nothing@*/
;

#endif

