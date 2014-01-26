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
#ifndef HT_NUM_H
#define HT_NUM_H

#include "boolt.h"	/* HT_BOOL */
#include "config.h"	/* HT_RSTR */

extern
unsigned short
max (
	const unsigned short,
	const unsigned short
	)
/*@modifies nothing@*/
;

extern
unsigned short
min (
	const unsigned short,
	const unsigned short
	)
/*@modifies nothing@*/
;

extern
HT_BOOL
single_bit (
	const unsigned int
	)
/*@modifies nothing@*/
;

/*@-protoparamname@*/
extern
HT_BOOL
texttoushort (
/*@in@*/
/*@notnull@*/
	const char * const HT_RSTR	str_num,
/*@out@*/
/*@notnull@*/
	unsigned short * const HT_RSTR	num
	)
/*@globals errno@*/
/*@modifies errno, * num@*/
;
/*@=protoparamname@*/

#endif

