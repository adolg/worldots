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
#ifndef HT_INVOC_H
#define HT_INVOC_H

#include "funct.h"	/* HT_FR, HT_FAIL_INVOC */
#include "invoct.h"	/* invoc */

/*@-protoparamname@*/
extern
enum HT_FR
invoc_read (
	const size_t,
/*@in@*/
/*@notnull@*/
/*@observer@*/
	const char * *,
/*@notnull@*/
/*@partial@*/
	struct invoc * const		inv,
/*@in@*/
/*@notnull@*/
	enum HT_FAIL_INVOC * const	invfail
	)
/*@modifies * inv, * invfail@*/
;
/*@=protoparamname@*/

#endif

