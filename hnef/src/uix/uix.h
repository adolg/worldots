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
#ifdef HT_UI_XLIB

#ifndef HT_UIX_H
#define HT_UIX_H

#include "config.h"	/* HT_RSTR */
#include "funct.h"	/* HT_FR */
#include "uit.h"	/* ui */
#include "uixt.h"	/* uix */

/*@-protoparamname@*/
extern
enum HT_FR
uix_play_new (
/*@in@*/
/*@notnull@*/
	struct game * const	g,
/*@in@*/
/*@notnull@*/
	struct ui * const	interf
	)
/*@globals internalState, fileSystem, errno, stdout@*/
/*@modifies internalState, fileSystem, errno, stdout, * g, * interf@*/
;
/*@=protoparamname@*/

/*@-protoparamname@*/
extern
enum HT_FR
uix_play_again (
/*@in@*/
/*@notnull@*/
	const struct game * const	g,
/*@in@*/
/*@notnull@*/
	struct uix * const		ui_x,
/*@in@*/
/*@notnull@*/
	HT_BOOL * const HT_RSTR		again
	)
/*@globals fileSystem, errno, stdout@*/
/*@modifies fileSystem, errno, stdout, * ui_x, * again@*/
;
/*@=protoparamname@*/

#ifdef HT_AI_MINIMAX

/*@-protoparamname@*/
extern
enum HT_FR
uix_ai_stop (
/*@in@*/
/*@notnull@*/
	const struct game * const	g,
/*@in@*/
/*@notnull@*/
	struct uix * const		ui_x,
/*@in@*/
/*@notnull@*/
	HT_BOOL * const HT_RSTR		stop
	)
/*@globals errno, fileSystem, stdout@*/
/*@modifies errno, fileSystem, stdout, * ui_x, * stop@*/
;
/*@=protoparamname@*/

/*@-protoparamname@*/
extern
enum HT_FR
uix_ai_progress (
	struct uix * const	ui_x,
	const int		prog
	)
/*@modifies * ui_x@*/
;
/*@=protoparamname@*/

#endif /* HT_AI_MINIMAX */

/*@null@*/
/*@only@*/
/*@partial@*/
extern
struct uix *
alloc_uix (
/*@in@*/
/*@notnull@*/
	const struct game * const
	)
/*@modifies nothing@*/
;

/*@-protoparamname@*/
extern
void
free_uix (
/*@notnull@*/
/*@owned@*/
/*@special@*/
	struct uix * const	ui_x
	)
/*@modifies ui_x->img_pieces, ui_x->img_squares, ui_x->gc,
	ui_x->display, ui_x@*/
/*@releases ui_x->img_pieces, ui_x->img_squares, ui_x->gc,
	ui_x->display, ui_x@*/
;
/*@=protoparamname@*/

/*@-protoparamname@*/
extern
enum HT_FR
uix_init (
/*@in@*/
/*@notnull@*/
	const struct game * const	g,
/*@notnull@*/
/*@partial@*/
	struct uix * const		ui_x
	)
/*@modifies ui_x@*/
;
/*@=protoparamname@*/

#endif

#endif

