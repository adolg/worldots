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
#ifndef HT_IOGEN_H
#define HT_IOGEN_H

#include <stdio.h>
#include "boolt.h"		/* HT_BOOL */
#include "funct.h"		/* HT_FR */
#include "invoct.h"		/* HT_INV_PRINT */
#include "iotextparset.h"	/* ht_line */

extern
enum HT_FR
print_inv_help (
	const HT_BOOL
	)
/*@globals fileSystem, stderr, stdout, errno@*/
/*@modifies fileSystem, stderr, errno@*/
;

extern
enum HT_FR
print_inv (
	const enum HT_INV_PRINT
	)
/*@globals fileSystem, stderr, stdout, errno, internalState@*/
/*@modifies fileSystem, stderr, stdout, errno, internalState@*/
;

extern
enum HT_FR
print_err_fr (
	FILE * ,
	const enum HT_FR
	)
/*@globals fileSystem, stderr, errno@*/
/*@modifies fileSystem, stderr@*/
;

extern
enum HT_FR
print_err_fr_invoc (
	const enum HT_FR,
	const enum HT_FAIL_INVOC
	)
/*@globals fileSystem, stderr, errno@*/
/*@modifies fileSystem, stderr@*/
;

extern
enum HT_FR
print_err_fr_rreader (
	FILE * ,
	const enum HT_FR,
	const enum HT_FAIL_RREADER,
/*@in@*/
/*@notnull@*/
	const struct ht_line * const
	)
/*@globals fileSystem, stderr, errno@*/
/*@modifies fileSystem, stderr, errno@*/
;

#endif

