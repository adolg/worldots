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
#include "aiminimax.h"
#endif
#include "player.h"	/* player_index_valid */
#include "playert.h"	/* player */
#include "ui.h"
#ifdef HT_UI_CLI
#include "uic.h"	/* uic_ai_progress */
#endif
#ifdef HT_UI_XLIB
#include "uix.h"	/* uix_ai_progress */
#endif
#ifdef HT_UI_MSQU
#include "uimq.h"	/* uimq_ai_progress */
#endif

/*
 * NOTE:	The following things must be defined for every interface
 *		that you create, exactly as follows:
 *
 *	struct ui#
 *	{
 *		...
 *	};
 *		A struct that contains whatever information you need.
 *		This struct must be available from "struct ui" and must
 *		have a HT_UIT_*.
 *
 *	enum HT_FR
 *	ui#_play_new (struct game * const, struct ui * const)
 *		1.	Call game_board_reset, to reset the board for a
 *			new game.
 *		2.	Play a full game, from beginning to end, and
 *			return HT_FR_SUCCESS if nothing went wrong.
 *		The reason we need a "struct ui" rather than
 *		"struct ui#" is because we need to pass it to the
 *		computer player, and the computer player doesn't know
 *		which interface we're using. The computer player will
 *		call ui_ai_progress(struct ui * const, const int), which
 *		will delegate the appropriate ui#_ai_progress function.
 *
 *	enum HT_FR
 *	ui#_play_again (const struct game * const, struct ui_c * const,
 *			HT_BOOL * const)
 *		Ask the player if he wants to play again.
 *
 *	enum HT_FR
 *	ui#_ai_stop (const struct game * const, struct ui * const,
 *			HT_BOOL * const)
 *		Set the boolean to true if the AI should stop its
 *		thinking. It should still select a move to the best of
 *		its ability, but it should do it without delay.
 *		Upon issuing the "stop" command for any reason, the
 *		boolean parameter should be set to true.
 *		Upon issuing an interface (ui_action) command, you
 *		should also set ui_action->type to the command (in
 *		addition to setting the boolean parameter to true).
 *		See how uix does it in uix_ai_stop().
 *
 *	enum HT_FR
 *	ui#_ai_progress (struct ui# * const, const int prog)
 *		Show ai thinking progress. prog is a percentage from
 *		HT_AIPROG_MIN to HT_AIPROG_MAX, that this function must
 *		be able to display.
 *		This function takes (as prog) HT_AIPROG_START when the
 *		algorithm begins, and HT_AIPROG_DONE when done. For the
 *		minimax algorithm with iterative deepening, it's ok to
 *		call the "begin" parameter once per iteration.
 *
 *	struct ui# *
 *	alloc_ui# (const struct game * const)
 *		Allocates the ui and returns non-NULL for success, NULL
 *		for failure to allocate.
 *
 *	void
 *	free_ui# (struct ui# * const)
 *		Frees whatever was allocated by alloc_ui# and
 *		ui#_init.
 *
 *	enum HT_FR
 *	ui#_init (const struct game * const, struct ui# * const)
 *		Initializes the ui based on the game struct. May
 *		allocate things that alloc_ui# couldn't.
 *
 *		It will be slightly easier to create new interfaces if
 *		you follow these rules, even if you have to define a few
 *		unnecessary functions and parameters (like uic.c does).
 */

/*
 * Checks if pl's controller would change if act_contr would be
 * applied.
 *
 * Does not check if act_contr is valid -- just if the arguments
 * contained therein are equal to the ones in pl.
 *
 * would_change is set to HT_TRUE or HT_FALSE if the function returns
 * HT_FR_SUCCESS.
 *
 * Returns HT_FR_FAIL_* on fatal failure.
 */
static
enum HT_FR
ui_player_contr_change_check (
/*@in@*/
/*@notnull@*/
	const struct player * const HT_RSTR		pl,
/*@in@*/
/*@notnull@*/
	const struct ui_action_contr * const HT_RSTR	act_contr,
/*@in@*/
/*@notnull@*/
	HT_BOOL * const HT_RSTR				would_change
	)
/*@modifies * would_change@*/
{
	const enum AITYPE pl_contr_old	= pl->contr;

	if (pl_contr_old == act_contr->type)
	{
#ifdef HT_AI_MINIMAX
		if (HT_AIT_MINIMAX == pl_contr_old)
		{
			struct aiminimax * const HT_RSTR aim =
				pl->ai.minimax;
			if (NULL == aim)
			{
				return HT_FR_FAIL_NULLPTR;
			}
			if (aim->depth_max ==
			act_contr->args.minimax.depth_max)
			{
				* would_change	= HT_FALSE;
				return		HT_FR_SUCCESS;
			}
		}
		else
#endif
		if (HT_AIT_NONE == pl_contr_old)
		{
			* would_change	= HT_FALSE;
			return		HT_FR_SUCCESS;
		}
#ifndef HT_UNSAFE
		else
		{
			/*
			 * Old AI not recognized: shouldn't happen.
			 */
			return	HT_FR_FAIL_ENUM_UNK;
		}
#endif
	}
	* would_change	= HT_TRUE;
	return		HT_FR_SUCCESS;
}

/*
 * Changes the pl->contr, if allowed. If so, returns HT_FR_SUCCESS and
 * sets valid = HT_TRUE.
 *
 * If not allowed, returns HT_FR_SUCCESS and sets valid = HT_FALSE.
 *
 * Upon fatal failure, return HT_FR_FAIL_*.
 */
static
enum HT_FR
ui_player_contr_change (
/*@in@*/
/*@notnull@*/
	const struct game * const		g,
/*@in@*/
/*@notnull@*/
	struct player * const			pl,
/*@in@*/
/*@notnull@*/
	const struct ui_action_contr * const	act_contr,
/*@in@*/
/*@notnull@*/
	HT_BOOL * const HT_RSTR			valid
	)
#ifdef HT_AI_MINIMAX_HASH
/*@globals errno, internalState@*/
#else
/*@globals internalState@*/
#endif
/*@modifies internalState, * pl, * valid@*/
#ifdef HT_AI_MINIMAX_HASH
/*@modifies errno@*/
#endif
{
#ifdef HT_AI_MINIMAX
	if (HT_AIT_MINIMAX == act_contr->type)
	{
		enum HT_FR			fr	= HT_FR_NONE;
		struct aiminimax * HT_RSTR	aim	= NULL;
#ifdef HT_AI_MINIMAX_HASH
		struct zhashtable *		old_tab	= NULL;
#endif
		if (!aiminimax_depth_max_valid(
			act_contr->args.minimax.depth_max))
		{
			* valid	= HT_FALSE;
			return	HT_FR_SUCCESS;
		}
#ifdef HT_AI_MINIMAX_HASH
		if (NULL != pl->ai.minimax)
		{
			/*
			 * Move the old table to the new AI to not have
			 * to re-allocate it. This will make alloc and
			 * free behave properly.
			 */
			old_tab		= pl->ai.minimax->tp_tab;
			pl->ai.minimax->tp_tab	= NULL;
		}
#endif
		aim	= alloc_aiminimax_init(g, act_contr->p_index,
			act_contr->args.minimax.depth_max, & fr
#ifdef HT_AI_MINIMAX_HASH
			, old_tab
#endif
			);
		if (NULL == aim)
		{
			if (HT_FR_FAIL_ILL_ARG == fr)
			{
				/*
				 * Not fatal failure, but bad arguments.
				 */
#ifdef HT_AI_MINIMAX_HASH
				if (NULL != pl->ai.minimax
				&& NULL != old_tab
				&& NULL == pl->ai.minimax->tp_tab)
				{
					pl->ai.minimax->tp_tab =
						old_tab;
					old_tab	= NULL;
				}
#endif
				* valid	= HT_FALSE;
				return	HT_FR_SUCCESS;
			}
			else
			{
#ifdef HT_AI_MINIMAX_HASH
				if (NULL != pl->ai.minimax
				&& NULL != old_tab
				&& NULL == pl->ai.minimax->tp_tab)
				{
					pl->ai.minimax->tp_tab =
						old_tab;
					old_tab	= NULL;
				}
#endif
				return	fr;
			}
		}

		if (NULL != pl->ai.minimax)
		{
			/*
			 * This is possible if the old aiminimax had a
			 * different search depth. We could just change
			 * the search depth in the existing struct, but
			 * that may break things in the future.
			 */
			free_aiminimax	(pl->ai.minimax);
		}
		pl->contr	= HT_AIT_MINIMAX;
		pl->ai.minimax	= aim;
		* valid		= HT_TRUE;
		return		HT_FR_SUCCESS;
	}
	else
#endif
	if (HT_AIT_NONE == act_contr->type)
	{
#ifdef HT_AI_MINIMAX
		if (HT_AIT_MINIMAX == pl->contr
		&& pl->ai.minimax != NULL)
		{
			free_aiminimax	(pl->ai.minimax);
			pl->ai.minimax	= NULL;
		}
#endif
		pl->contr	= HT_AIT_NONE;
		* valid		= HT_TRUE;
		return		HT_FR_SUCCESS;
	}
	else
	{
		/*
		 * This function does not recognize act_contr->type.
		 */
		return	HT_FR_FAIL_ILL_ARG;
	}
}

/*
 * Attempts to set a player in g according to the arguments in
 * act_contr.
 *
 * If this succeeds completely, then HT_FR_SUCCESS and valid == HT_TRUE
 * are returned.
 *
 * If this fails fatally (in some non-recoverable way), then
 * HT_FR_FAIL_* is returned, and valid is unchanged.
 *
 * If this fails because act_contr contains invalid arguments, or
 * because the player's controller would not change because they equal
 * the arguments in act_contr, then it returns HT_FR_SUCCESS and
 * valid == HT_FALSE. This is a recoverable error.
 */
enum HT_FR
ui_player_contr_set (
	struct game * const			g,
	const struct ui_action_contr * const	act_contr,
	HT_BOOL * const				valid
	)
{
	enum HT_FR		fr = HT_FR_NONE;
	struct player *		pl;
	if (!player_index_valid(act_contr->p_index))
	{
		* valid	= HT_FALSE;
		return	HT_FR_SUCCESS;
	}
	pl	= g->players[act_contr->p_index];

	fr	= ui_player_contr_change_check(pl, act_contr, valid);
	if (HT_FR_SUCCESS != fr)
	{
		return	fr;
	}
#ifndef HT_UNSAFE
	fr	= HT_FR_NONE;
#endif
	if (!(* valid))
	{
		/*
		 * Value would not change.
		 */
		return	HT_FR_SUCCESS;
	}

	return ui_player_contr_change(g, pl, act_contr, valid);
}

/*
 * Sets g->players[p_index] to human controlled.
 */
enum HT_FR
ui_player_contr_unset (
	struct game * const	g,
	unsigned short		p_index
	)
{
	enum HT_FR			fr	= HT_FR_NONE;
	HT_BOOL				valid	= HT_FALSE;
	struct ui_action_contr		act_contr;
#ifdef HT_AI_MINIMAX
	act_contr.args.minimax.depth_max	= HT_AIM_DEPTHMAX_MIN;
#endif
	act_contr.type				= HT_AIT_NONE;
	act_contr.p_index			= p_index;


	fr	= ui_player_contr_set (g, & act_contr, & valid);
	if (HT_FR_SUCCESS != fr)
	{
		return	fr;
	}
	if (!valid)
	{
		return	HT_FR_FAIL_ILL_ARG;
	}
	return	HT_FR_SUCCESS;
}

/*
 * Initializes the values in act to non-garbage valid values.
 *
 * The values are only guaranteed to be valid -- not to make sense or be
 * usable as they are. They still need to be set before being used.
 * Can also be used to reset values.
 */
void
ui_action_init (
	struct ui_action * const HT_RSTR act
	)
{
	act->type				= HT_UI_ACT_UNK;
	act->info.contr.type			= HT_AIT_UNK;
	act->info.contr.p_index			= HT_PLAYER_UNINIT;
#ifdef HT_AI_MINIMAX
	act->info.contr.args.minimax.depth_max = HT_AIM_DEPTHMAX_UNINIT;
#endif
}

/*
 * Get default interface. Depends on what has been compiled in.
 * Returning HT_UIT_NONE means that the program is not functional since
 * the user can't interact with it.
 */
enum HT_UIT
ui_def (void)
{
#ifdef	HT_UI_XLIB
	return	HT_UIT_XLIB;
#elif	defined(HT_UI_CLI)
	return	HT_UIT_CLI;
#elif	defined(HT_UI_MSQU)
	return	HT_UIT_MSQU;
#else
	return	HT_UIT_NONE;
#endif
}

#ifdef HT_AI_MINIMAX

enum HT_FR
ui_ai_stop (
	const struct game * const	g,
	struct ui * const		interf,
	HT_BOOL * const			stop
	)
{
#ifdef HT_UI_CLI
	if (HT_UIT_CLI == interf->type)
	{
		return	uic_ai_stop(g, interf->data.ui_c, stop);
	}
	else
#endif
#ifdef HT_UI_XLIB
	if (HT_UIT_XLIB == interf->type)
	{
		return	uix_ai_stop(g, interf->data.ui_x, stop);
	}
	else
#endif
#ifdef HT_UI_MSQU
	if (HT_UIT_MSQU == interf->type)
	{
		return	uimq_ai_stop(g, interf->data.ui_mq, stop);
	}
	else
#endif
	{
		return	HT_FR_FAIL_ENUM_UNK;
	}
}

/*
 * Delegates either uic_ai_progress or uix_ai_progress, as appropriate.
 */
enum HT_FR
ui_ai_progress (
	struct ui * const	interf,
	const int		prog
	)
{
#ifdef HT_UI_CLI
	if (HT_UIT_CLI == interf->type)
	{
		return	uic_ai_progress(interf->data.ui_c, prog);
	}
	else
#endif
#ifdef HT_UI_XLIB
	if (HT_UIT_XLIB == interf->type)
	{
		return	uix_ai_progress(interf->data.ui_x, prog);
	}
	else
#endif
#ifdef HT_UI_MSQU
	if (HT_UIT_MSQU == interf->type)
	{
		return	uimq_ai_progress(interf->data.ui_mq, prog);
	}
	else
#endif
	{
		return	HT_FR_FAIL_ENUM_UNK;
	}
}

#endif /* HT_AI_MINIMAX */

