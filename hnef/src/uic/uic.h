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
#ifdef HT_UI_CLI

#ifndef HT_UIC_H
#define HT_UIC_H

#include "config.h"	/* HT_RSTR */
#include "funct.h"	/* HT_FR */
#include "gamet.h"	/* game */
#include "uit.h"	/* ui */
#include "uict.h"	/* uic */

/*@-protoparamname@*/
extern
enum HT_FR
uic_play_new (
/*@in@*/
/*@notnull@*/
	struct game * const	g,
/*@in@*/
/*@notnull@*/
	struct ui * const	interf
	)
/*@globals internalState, fileSystem, errno, stdout, stdin@*/
/*@modifies internalState, fileSystem, errno, stdout, stdin, * g@*/
/*@modifies * interf@*/
;
/*@=protoparamname@*/

/*@-protoparamname@*/
extern
enum HT_FR
uic_play_again (
/*@in@*/
/*@notnull@*/
	const struct game * const HT_RSTR	g,
/*@in@*/
/*@notnull@*/
	struct uic * const			ui_c,
/*@in@*/
/*@notnull@*/
	HT_BOOL * const HT_RSTR			again
	)
/*@globals fileSystem, stdout, stdin, errno@*/
/*@modifies fileSystem, stdout, stdin, errno, * ui_c, * again@*/
;
/*@=protoparamname@*/

#ifdef HT_AI_MINIMAX

/*@-protoparamname@*/
extern
enum HT_FR
uic_ai_stop (
/*@in@*/
/*@notnull@*/
	const struct game * const HT_RSTR	g,
/*@in@*/
/*@notnull@*/
	struct uic * const HT_RSTR		ui_c,
/*@in@*/
/*@notnull@*/
	HT_BOOL * const HT_RSTR			stop
	)
/*@modifies nothing@*/
;
/*@=protoparamname@*/

/*@-protoparamname@*/
extern
enum HT_FR
uic_ai_progress (
	struct uic * const HT_RSTR	ui_c,
	const int			prog
	)
/*@globals errno, fileSystem, stdout@*/
/*@modifies errno, fileSystem, stdout, * ui_c@*/
;
/*@=protoparamname@*/

#endif /* HT_AI_MINIMAX */

/*@null@*/
/*@only@*/
/*@partial@*/
extern
struct uic *
alloc_uic (
/*@in@*/
/*@notnull@*/
	const struct game * const
	)
/*@modifies nothing@*/
;

/*@-protoparamname@*/
extern
void
free_uic (
/*@notnull@*/
/*@owned@*/
/*@special@*/
	struct uic * const	ui_c
	)
/*@modifies ui_c@*/
/*@releases ui_c@*/
;
/*@=protoparamname@*/

/*@-protoparamname@*/
extern
enum HT_FR
uic_init (
/*@in@*/
/*@notnull@*/
	const struct game * const HT_RSTR	g,
/*@notnull@*/
/*@partial@*/
	struct uic * const HT_RSTR		ui_c
	)
/*@modifies ui_c@*/
;
/*@=protoparamname@*/

#endif

#endif

