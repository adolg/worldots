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
#ifndef HT_FUNC_H
#define HT_FUNC_H

#include "boolt.h"	/* HT_BOOL */
#include "funct.h"	/* HT_FR */

/*@observer@*/
extern
const char *
fail_invoc_what (
	const enum HT_FAIL_INVOC
	)
/*@modifies nothing@*/
;

/*@observer@*/
extern
const char *
fail_rreader_what (
	const enum HT_FAIL_RREADER
	)
/*@modifies nothing@*/
;

extern
HT_BOOL
fr_fail_invoc (
	const enum HT_FR
	)
/*@modifies nothing@*/
;

extern
HT_BOOL
fr_fail_rreader (
	const enum HT_FR
	)
/*@modifies nothing@*/
;

/*@observer@*/
extern
const char *
fr_fail_what (
	const enum HT_FR
	)
/*@modifies nothing@*/
;

#endif

