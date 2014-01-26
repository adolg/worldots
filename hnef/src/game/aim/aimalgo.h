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
#ifdef HT_AI_MINIMAX

#ifndef HT_AIMALGO_H
#define HT_AIMALGO_H

#include "aiminimaxt.h"	/* aiminimax */
#include "boolt.h"	/* HT_BOOL */
#include "config.h"	/* HT_RSTR */
#include "funct.h"	/* HT_FR */
#include "gamet.h"	/* game */
#include "uit.h"	/* ui_action, ui */

/*@-protoparamname@*/
extern
enum HT_FR
aiminimax_command (
/*@in@*/
/*@notnull@*/
	const struct game * const HT_RSTR	g,
/*@in@*/
/*@notnull@*/
	struct ui * const			interf,
/*@in@*/
/*@notnull@*/
	struct aiminimax * const		aim,
/*@in@*/
/*@notnull@*/
	struct ui_action * const HT_RSTR	act
	)
/*@globals errno, fileSystem, stdout@*/
/*@modifies errno, fileSystem, stdout, *interf, * aim, * act@*/
;
/*@=protoparamname@*/

#endif

#endif

