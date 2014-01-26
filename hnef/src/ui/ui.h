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
#ifndef HT_UI_H
#define HT_UI_H

#include "config.h"	/* HT_RSTR */
#include "funct.h"	/* HT_FR */
#include "gamet.h"	/* game */
#include "uit.h"	/* HT_UIT */
#include "uict.h"	/* uic */

/*@-protoparamname@*/
extern
enum HT_FR
ui_player_contr_set (
/*@in@*/
/*@notnull@*/
	struct game * const			g,
/*@in@*/
/*@notnull@*/
	const struct ui_action_contr * const	act_contr,
/*@in@*/
/*@notnull@*/
	HT_BOOL * const				valid
	)
#ifdef HT_AI_MINIMAX_HASH
/*@globals errno, internalState@*/
#else
/*@globals internalState@*/
#endif
/*@modifies internalState, * g, * valid@*/
#ifdef HT_AI_MINIMAX_HASH
/*@modifies errno@*/
#endif

;
/*@=protoparamname@*/

/*@-protoparamname@*/
extern
enum HT_FR
ui_player_contr_unset (
	struct game * const	g,
	unsigned short		p_index
	)
#ifdef HT_AI_MINIMAX_HASH
/*@globals errno, internalState@*/
#else
/*@globals internalState@*/
#endif
/*@modifies internalState, * g@*/
#ifdef HT_AI_MINIMAX_HASH
/*@modifies errno@*/
#endif
;
/*@=protoparamname@*/

/*@-protoparamname@*/
extern
void
ui_action_init (
/*@notnull@*/
/*@partial@*/
	struct ui_action * const HT_RSTR act
	)
/*@modifies * act@*/
;
/*@=protoparamname@*/

extern
enum HT_UIT
ui_def (void)
/*@modifies nothing@*/
;

#ifdef HT_AI_MINIMAX

/*@-protoparamname@*/
extern
enum HT_FR
ui_ai_stop (
/*@in@*/
/*@notnull@*/
	const struct game * const	g,
/*@in@*/
/*@notnull@*/
	struct ui * const		interf,
/*@in@*/
/*@notnull@*/
	HT_BOOL * const			stop
	)
/*@globals errno, fileSystem, stdout@*/
/*@modifies errno, fileSystem, stdout, * interf, * stop@*/
;
/*@=protoparamname@*/

/*@-protoparamname@*/
extern
enum HT_FR
ui_ai_progress (
/*@in@*/
/*@notnull@*/
	struct ui * const	interf,
	const int		prog
	)
/*@globals errno, fileSystem, stdout@*/
/*@modifies errno, fileSystem, stdout, * interf@*/
;
/*@=protoparamname@*/

#endif /* HT_AI_MINIMAX */

#endif

