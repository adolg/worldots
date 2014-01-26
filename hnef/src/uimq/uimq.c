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
#ifdef HT_UI_MSQU

#include <string.h>		/* strlen */
#include <pthread.h>

#include "ait.h"		/* aitype HT_AIT_* */
#ifdef HT_AI_MINIMAX
#include "aimalgo.h"		/* aiminimax_command */
#include "aiminimax.h"		/* HT_AIM_DEPTHMAX_* */
#endif
#include "board.h"		/* board_move_safe */
#include "game.h"		/* game_board_reset */
#include "lang.h"		/* HT_L_* */
#include "num.h"		/* texttoushort */
#include "iotextparse.h"	/* ht_line, ht_line_* */
#include "player.h"		/* player_index_valid */
#include "playert.h"		/* player */
#include "types.h"		/* type_index_get */
#include "uimq.h"
#include "ui.h"			/* ui_action_init */
#include "uimqt.h"		/* HT_UIMQ_ACTT, uic */
#include "uit.h"		/* ui_action */
#include "queue.h"
#include "console.h"
// TODO do not include directly!
#include "ppapi_main.h"

/*
 * NOTE:	There is no portable way to read from stdin without
 *		blocking, so it's not possible to be able to interrupt
 *		the computer player's thinking in the CLI. This is
 *		possible with the XLib interface.
 */

static
enum HT_FR
print_cmd_prompt (void)
/*@globals fileSystem, stdout@*/
/*@modifies fileSystem, stdout@*/
{
	return fputs(HT_L_UIC_CMD_PROMPT , stdout) < 0 ?
		HT_FR_FAIL_IO_PRINT : HT_FR_SUCCESS;
}

static
enum HT_FR
print_help (void)
/*@globals fileSystem, stdout, errno@*/
/*@modifies fileSystem, stdout, errno@*/
{
	int r		= printf("%c x1 y1 x2 y2\t", HT_L_UIC_ARG_MOVE);
	r = r < 0 ? r : puts(HT_L_UIC_HELP_ARG_MOVE);

	r = r < 0 ? r : printf("%c x y z\t\t", HT_L_UIC_ARG_CONTR);
	r = r < 0 ? r : puts(HT_L_UIC_HELP_ARG_CONTR);

	r = r < 0 ? r : printf("\t\t%c\t", HT_L_UIC_ARG_CONTR_NONE);
	r = r < 0 ? r : puts(HT_L_UIC_HELP_ARG_CONTR_NONE);

#ifdef HT_AI_MINIMAX
	r = r < 0 ? r : printf("\t\t%c\t", HT_L_UIC_ARG_CONTR_MINIMAX);
	r = r < 0 ? r : printf("%s%d <= z <= %d\n",
		HT_L_UIC_HELP_ARG_CONTR_MINIMAX,
		(int)HT_AIM_DEPTHMAX_MIN, (int)HT_AIM_DEPTHMAX_MAX);
#endif

	r = r < 0 ? r : printf("%c\t\t", HT_L_UIC_ARG_CONTR);
	r = r < 0 ? r : puts(HT_L_UIC_HELP_ARG_PRINT_PLAYERS);

	r = r < 0 ? r : printf("%c\t\t", HT_L_UIC_ARG_PRINT_BOARD);
	r = r < 0 ? r : puts(HT_L_UIC_HELP_ARG_PRINT_BOARD);

	r = r < 0 ? r : printf("%c\t\t", HT_L_UIC_ARG_QUIT);
	r = r < 0 ? r : puts(HT_L_UIC_HELP_ARG_QUIT);

	return r < 0 ? HT_FR_FAIL_IO_PRINT : HT_FR_SUCCESS;
}

/*
 * Prints pieces with symbols A to H for piece bits 1 to 128; lower case
 * for player 1 (yes, it's discrimination because player 1 is typically
 * Muscovites).
 */
static
enum HT_FR
print_board (
/*@in@*/
/*@notnull@*/
	const struct game * const HT_RSTR g
	)
/*@globals fileSystem, stdout, errno@*/
/*@modifies fileSystem, stdout, errno@*/
{
	unsigned short	i, j = (unsigned short)0;
	char buffer [g->rules->opt_blen + g->rules->bheight + 1];
	for (i = (unsigned short)0; i < g->rules->opt_blen; i++)
	{
/*
		if (i % g->rules->bwidth == 0)
		{
			buffer[j++] = '\n';
		}
*/
		const HT_BIT_U8 pbit	= g->b->pieces[i];
		sprintf(&(buffer[j++]), "%1d", (int)pbit);
	}
	PostMessage("pieces", buffer);
	return	HT_FR_SUCCESS;
}

/*
 * Prints player indices, name and turn indicator.
 */
static
enum HT_FR
print_players (
/*@in@*/
/*@notnull@*/
	const struct game * const HT_RSTR g
	)
/*@globals fileSystem, stdout, errno@*/
/*@modifies fileSystem, stdout, errno@*/
{
	unsigned short i;
	for (i = 0; i < g->playerc; i++)
	{
		const struct player * const HT_RSTR pl = g->players[i];

		if (HT_AIT_NONE == pl->contr)
		{
			if (printf("%d\t%s", (int)i,
				HT_L_UIC_ACT_PLAYERS_HUMAN) < 0)
			{
				return	HT_FR_FAIL_IO_PRINT;
			}
		}
#ifdef HT_AI_MINIMAX
/* splint incomplete logic */ /*@i2@*/\
		else if (HT_AIT_MINIMAX == pl->contr)
		{
#ifndef HT_UNSAFE
			if (NULL == pl->ai.minimax)
			{
				return	HT_FR_FAIL_NULLPTR;
			}
#endif
			if (printf("%d\t%s-%d", (int)i,
				HT_L_UIC_ACT_PLAYERS_AIM,
				(int)pl->ai.minimax->depth_max) < 0)
			{
				return	HT_FR_FAIL_IO_PRINT;
			}
		}
#endif

		if (i == g->b->turn && fputs("\t*", stdout) < 0)
		{
			return	HT_FR_FAIL_IO_PRINT;
		}
		if (EOF == fputc('\n', stdout))
		{
			return	HT_FR_FAIL_IO_PRINT;
		}
	}
	return	HT_FR_SUCCESS;
}

/*
 * Translates a HT_UI_ACTT that has not been set yet (the first command
 * argument).
 */
static
enum HT_FR
cmd_human_interpw_first (
	const char				arg_ch,
/*@in@*/
/*@notnull@*/
	struct ui_action * const HT_RSTR	act,
/*@in@*/
/*@notnull@*/
	enum HT_UIMQ_ACTT * const HT_RSTR	act_sp,
/*@in@*/
/*@notnull@*/
	HT_BOOL * const HT_RSTR			done
	)
/*@modifies * act, * act_sp, * done@*/
{
	if (HT_L_UIC_ARG_PRINT_BOARD == arg_ch)
	{
		act->type	= HT_UI_ACT_SPECIAL;
		* act_sp	= HT_UIMQ_ACT_PRINT_BOARD;
		* done		= HT_TRUE;
		return		HT_FR_SUCCESS;
	}
	else if (HT_L_UIC_ARG_CONTR == arg_ch)
	{
		act->type	= HT_UI_ACT_CONTR;
		return		HT_FR_SUCCESS;
	}
	else if (HT_L_UIC_ARG_MOVE == arg_ch)
	{
		struct move * const HT_RSTR m	= & act->info.mov;
		m->pos	= m->dest		= HT_BOARDPOS_NONE;
		act->type			= HT_UI_ACT_MOVE;
		return				HT_FR_SUCCESS;
	}
	else if (HT_L_UIC_ARG_QUIT == arg_ch)
	{
		act->type	= HT_UI_ACT_QUIT;
		* done		= HT_TRUE;
		return		HT_FR_SUCCESS;
	}
	else
	{
		/*
		 * Unrecognized char argument.
		 */
		act->type	= HT_UI_ACT_UNK;
		* done		= HT_TRUE;
		return		HT_FR_SUCCESS;
	}
}

/*
 * Translates HT_UI_ACT_CONTR.
 */
static
enum HT_FR
cmd_human_interpw_contr (
/*@in@*/
/*@notnull@*/
	const char * const HT_RSTR	arg_w,
/*@in@*/
/*@notnull@*/
	struct ui_action * const	act,
/*@in@*/
/*@notnull@*/
	HT_BOOL * const HT_RSTR		act_contr_index,
/*@in@*/
/*@notnull@*/
	HT_BOOL * const HT_RSTR		act_contr_type,
/*@in@*/
/*@notnull@*/
	HT_BOOL * const HT_RSTR		done
	)
/*@globals errno@*/
/*@modifies errno, * act, * act_contr_index, * act_contr_type, * done@*/
{
	char arg_ch;
#ifndef HT_UNSAFE
	if (strlen(arg_w) < (size_t)1)
	{
		/*
		 * Empty word: can't happen.
		 */
		return		HT_FR_FAIL_ILL_ARG;
	}
#endif
	arg_ch	= arg_w[0];

	if (* act_contr_index)
	{
		if (* act_contr_type)
		{
			/*
			 * "p x y <z>" (AI args).
			 */
#ifdef HT_AI_MINIMAX
			if (texttoushort(arg_w,
			& act->info.contr.args.minimax.depth_max))
			{
				* act_contr_index	= HT_TRUE;
				* done			= HT_TRUE;
			}
			else
#endif
			{
				/*
				 * <z> is NAN, or !HT_AI_MINIMAX.
				 */
				act->type	= HT_UI_ACT_UNK;
				* done		= HT_TRUE;
			}
		}
		else
		{
			/*
			 * Reading "p x <y>" (AI type).
			 */
			if (HT_L_UIC_ARG_CONTR_NONE == arg_ch)
			{
				act->info.contr.type = HT_AIT_NONE;
				* act_contr_type	= HT_TRUE;
				* done			= HT_TRUE;
			}
#ifdef HT_AI_MINIMAX
			else if (HT_L_UIC_ARG_CONTR_MINIMAX == arg_ch)
			{
				act->info.contr.type = HT_AIT_MINIMAX;
				* act_contr_type	= HT_TRUE;
			}
#endif
			else
			{
				/*
				 * <y> is unrecognized controller.
				 */
				act->type	= HT_UI_ACT_UNK;
				* done		= HT_TRUE;
			}
		}
	}
	else
	{
		/*
		 * Reading "p <x>" (player index).
		 */
		if (texttoushort(arg_w, & act->info.contr.p_index)
		&& player_index_valid(act->info.contr.p_index))
		{
			* act_contr_index	= HT_TRUE;
		}
		else
		{
			/*
			 * <x> is not a number, or invalid index.
			 */
			act->type	= HT_UI_ACT_UNK;
			* done		= HT_TRUE;
		}
	}
	return		HT_FR_SUCCESS;
}

/*
 * Translates HT_UI_ACT_MOVE.
 */
static
enum HT_FR
cmd_human_interpw_move (
/*@in@*/
/*@notnull@*/
	const struct game * const HT_RSTR	g,
/*@in@*/
/*@notnull@*/
	const char * const HT_RSTR		arg_w,
/*@in@*/
/*@notnull@*/
	struct ui_action * const HT_RSTR	act,
/*@in@*/
/*@notnull@*/
	unsigned short * const HT_RSTR		move_x_tmp,
/*@in@*/
/*@notnull@*/
	HT_BOOL * const HT_RSTR			done
	)
/*@globals errno@*/
/*@modifies errno, * act, * move_x_tmp, * done@*/
{
	const unsigned short	bw	= g->rules->bwidth;
	const unsigned short	bh	= g->rules->bheight;
	unsigned short		arg_num;
#ifndef HT_UNSAFE
	if (strlen(arg_w) < (size_t)1)
	{
		/*
		 * Empty word: can't happen.
		 */
		return		HT_FR_FAIL_ILL_ARG;
	}
#endif

	if (!texttoushort(arg_w, & arg_num))
	{
		act->type	= HT_UI_ACT_UNK;
		* done		= HT_TRUE;
		return		HT_FR_SUCCESS;
	}

	if (HT_BOARDPOS_NONE == * move_x_tmp)
	{
		/*
		 * arg_num is x.
		 */
		if (arg_num >= bw)
		{
			act->type	= HT_UI_ACT_UNK;
			* done		= HT_TRUE;
			return		HT_FR_SUCCESS;
		}
		* move_x_tmp	= arg_num;
		return		HT_FR_SUCCESS;
	}
	else
	{
		/*
		 * move_x_tmp is x, arg_num is y.
		 */
		unsigned short bpos = (unsigned short)
			(arg_num * bw + * move_x_tmp);
		if (* move_x_tmp >= bw || arg_num >= bh
		|| bpos >= bw * bh)
		{
			act->type	= HT_UI_ACT_UNK;
			* done		= HT_TRUE;
			return		HT_FR_SUCCESS;
		}

		if (HT_BOARDPOS_NONE == act->info.mov.pos)
		{
			act->info.mov.pos	= bpos;
			* move_x_tmp		= HT_BOARDPOS_NONE;
		}
		else
		{
#ifndef HT_UNSAFE
			if (HT_BOARDPOS_NONE != act->info.mov.dest)
			{
				return	HT_FR_FAIL_ILL_STATE;
			}
#endif
			act->info.mov.dest	= bpos;
			* done			= HT_TRUE;
		}
		return		HT_FR_SUCCESS;
	}
}

/*
 * Translates a single word, in a command argument, into whatever
 * information can be extracted out of it.
 *
 * *	act: the ui_action to take.
 *
 *	HT_UI_ACT_CONTR uses:
 *
 *	*	act_contr_index: is player index in act set?
 *
 *	*	act_contr_type: is player controller type in act set?
 *
 *	HT_UI_ACT_MOVE uses:
 *
 *	*	move_x_tmp: keep track of the x position in a move in
 *		act.
 *
 *	HT_UI_ACT_SPECIAL uses:
 *
 *	*	act_sp, which may be set to:
 *
 *		*	HT_UIC_ACT_PRINT_BOARD: set by this function.
 *
 *		*	HT_UIC_ACT_PRINT_PLAYER: set later, if the
 *			HT_UI_ACT_CONTR command only contains a single
 *			argument.
 *
 * HT_UI_ACT_CONTR is HT_UI_ACT_UNK if the action is obviously invalid
 * already at this point (but it may be invalid later too, for example
 * if a move is disallowed).
 */
static
enum HT_FR
cmd_human_interpw (
/*@in@*/
/*@notnull@*/
	const struct game * const	g,
/*@in@*/
/*@notnull@*/
	const char * const		arg_w,
/*@in@*/
/*@notnull@*/
	struct ui_action * const	act,
/*@in@*/
/*@notnull@*/
	HT_BOOL * const			act_contr_index,
/*@in@*/
/*@notnull@*/
	HT_BOOL * const			act_contr_type,
/*@in@*/
/*@notnull@*/
	unsigned short * const		move_x_tmp,
/*@in@*/
/*@notnull@*/
	enum HT_UIMQ_ACTT * const	act_sp,
/*@in@*/
/*@notnull@*/
	HT_BOOL * const			done
	)
/*@globals errno@*/
/*@modifies errno, * act, * act_contr_index, * act_contr_type@*/
/*@modifies * move_x_tmp, * act_sp, * done@*/
{
	char arg_ch;
	if (strlen(arg_w) < (size_t)1)
	{
		/*
		 * Empty word: may happen.
		 */
		act->type	= HT_UI_ACT_UNK;
		return		HT_FR_SUCCESS;
	}
	arg_ch	= arg_w[0];

	if (HT_UI_ACT_UNK == act->type)
	{
		/*
		 * First argument in a line.
		 */
		return cmd_human_interpw_first(arg_ch, act, act_sp,
			done);
	}
	else if (HT_UI_ACT_CONTR == act->type)
	{
		return cmd_human_interpw_contr(arg_w, act,
			act_contr_index, act_contr_type, done);
	}
	else if (HT_UI_ACT_MOVE == act->type)
	{
		return cmd_human_interpw_move(g, arg_w, act,
			move_x_tmp, done);
	}
	else
	{
		return	HT_FR_FAIL_ILL_STATE;
	}
}

/*
 * Called when there are no more command arguments in the line that was
 * input by the user.
 *
 * Checks if the command is complete and sets its final state.
 *
 * If the command is incomplete in any way, then act->type is set to
 * HT_UI_ACT_UNK.
 *
 * If the "controller" ("p") command was given without any further
 * arguments, then it counts as the "print players" special command
 * (HT_UIC_ACT_PRINT_PLAYERS).
 */
static
void
cmd_human_finalize (
/*@in@*/
/*@notnull@*/
	struct ui_action * const HT_RSTR	act,
	const HT_BOOL				act_contr_index,
/*@in@*/
/*@notnull@*/
	enum HT_UIMQ_ACTT * const HT_RSTR	act_sp,
/*@in@*/
/*@notnull@*/
	HT_BOOL * const HT_RSTR			done
	)
/*@modifies * act, * act_sp, * done@*/
{
	if (HT_UI_ACT_CONTR == act->type && !act_contr_index)
	{
		/*
		 * "p" command without arguments: print players.
		 */
		act->type	= HT_UI_ACT_SPECIAL;
		* act_sp	= HT_UIMQ_ACT_PRINT_PLAYERS;
		* done		= HT_TRUE;
	}

	if (!(* done))
	{
		/*
		 * Command is incomplete: invalidate it.
		 */
		act->type	= HT_UI_ACT_UNK;
		* done		= HT_TRUE;
	}
}

/*
 * Converts information in line to information in act (and possibly
 * act_sp, for CLI-specific commands) by looking at every word
 * (cmd_human_interpw).
 */
static
enum HT_FR
cmd_human_interp (
/*@in@*/
/*@notnull@*/
	const struct game * const	g,
/*@partial@*/
/*@notnull@*/
	const struct ht_line * const	line,
/*@in@*/
/*@notnull@*/
	struct ui_action * const	act,
/*@in@*/
/*@notnull@*/
	enum HT_UIMQ_ACTT * const	act_sp
	)
/*@globals errno@*/
/*@modifies errno, * act, * act_sp@*/
{
	enum HT_FR	fr		= HT_FR_NONE;
	unsigned short	move_x_tmp	= HT_BOARDPOS_NONE;
	HT_BOOL		act_contr_index, act_contr_type, done;
	size_t		i;

	if (ht_line_empty(line))
	{
		act->type	= HT_UI_ACT_UNK;
		return		HT_FR_SUCCESS;
	}

	act_contr_index = act_contr_type = done		= HT_FALSE;

	for (i = 0; i < line->wordc && !done; i++)
	{
		const char * arg_w	= line->words[i].chars;
		fr = cmd_human_interpw(g, arg_w, act, & act_contr_index,
			& act_contr_type, & move_x_tmp, act_sp, & done);
		if (HT_FR_SUCCESS != fr)
		{
			return fr;
		}
#ifndef HT_UNSAFE
		fr	= HT_FR_NONE;
#endif

		if (i + 1 >= line->wordc)
		{
			cmd_human_finalize(act, act_contr_index, act_sp,
				& done);
		}
	}
	return HT_FR_SUCCESS;
}

/*
 * Asks the human to enter one command and converts it to command
 * information in act.
 */
static
enum HT_FR
cmd_human (
/*@in@*/
/*@notnull@*/
	const struct game * const	g,
/*@partial@*/
/*@notnull@*/
	struct ht_line * const		line,
/*@in@*/
/*@notnull@*/
	struct ui_action * const	act,

	struct ui * const interf,
/*@in@*/
/*@notnull@*/
	enum HT_UIMQ_ACTT * const	act_sp
	)
/*@globals fileSystem, errno, stdout, stdin@*/
/*@modifies fileSystem, errno, stdout, stdin, * line, * act, * act_sp@*/
{
	enum HT_FR fr	= HT_FR_NONE;

	fr	= print_cmd_prompt();
	if (HT_FR_SUCCESS != fr)
	{
		return	fr;
	}
#ifndef HT_UNSAFE
	fr	= HT_FR_NONE;
#endif

	FILE* fp = fopen("/hnef.tmp" , "w+");
	DequeueMessageIntoTmpFile(fp);
	fr	= ht_line_readline(fp, line);
	if (HT_FR_SUCCESS != fr)
	{
		return	fr;
	}
#ifndef HT_UNSAFE
	fr	= HT_FR_NONE;
#endif

	if (0 != ferror(fp))
	{
		return		HT_FR_FAIL_IO_FILE_R;
	}
	else if (ht_line_empty(line))
	{
		/*
		 * Print help.
		 */
		act->type	= HT_UI_ACT_UNK;
		fclose(fp);
		return		HT_FR_SUCCESS;
	}
	else
	{
		fclose(fp);
		return		cmd_human_interp(g, line, act, act_sp);
	}
}

static
enum HT_FR
action_perform (
/*@in@*/
/*@notnull@*/
	struct game * const		g,
/*@in@*/
/*@notnull@*/
	const struct ui_action * const	act,
/*@in@*/
/*@notnull@*/
	const enum HT_UIMQ_ACTT * const	act_sp
	)
/*@globals internalState, fileSystem, errno, stdout@*/
/*@modifies internalState, fileSystem, errno, stdout, * g@*/
{
	enum HT_FR fr	= HT_FR_NONE;

	if (HT_UI_ACT_MOVE == act->type)
	{
		const struct move * const HT_RSTR m = & act->info.mov;
		HT_BOOL				legal	= HT_FALSE;

		fr = game_move(g, m->pos, m->dest, & legal);
		if (HT_FR_SUCCESS != fr)
		{
			return	fr;
		}
#ifndef HT_UNSAFE
		fr	= HT_FR_NONE;
#endif
		if (!legal)
		{
			const struct player * const playercur =
				g->players[g->b->turn];
			if (HT_AIT_NONE == playercur->contr)
			{
				if (puts(HT_L_UIC_ACT_MOVE_ILL_HUMAN)
					< 0)
				{
					return	HT_FR_FAIL_IO_PRINT;
				}
			}
#ifdef HT_AI_MINIMAX
			else
			{
				fr = ui_player_contr_unset(g,
					g->b->turn);
				if (HT_FR_SUCCESS != fr)
				{
					return	fr;
				}
#ifndef HT_UNSAFE
				fr	= HT_FR_NONE;
#endif

				if (puts(HT_L_UIC_ACT_MOVE_ILL_AI_UNS)
					< 0)
				{
					return	HT_FR_FAIL_IO_PRINT;
				}
			}
#endif /* HT_AI_MINIMAX */
		}

		fr	= print_board(g);
		if (HT_FR_SUCCESS != fr)
		{
			return	fr;
		}
#ifndef HT_UNSAFE
		fr	= HT_FR_NONE;
#endif
	}
/* splint incomplete logic */ /*@i1@*/\
	else if (HT_UI_ACT_CONTR == act->type)
	{
		HT_BOOL valid	= HT_FALSE;
		fr = ui_player_contr_set(g, & act->info.contr, & valid);
		if (HT_FR_SUCCESS != fr)
		{
			return fr;
		}
#ifndef HT_UNSAFE
		fr	= HT_FR_NONE;
#endif
		if (!valid)
		{
			if (puts(HT_L_UIC_ACT_CONTR_ILL) < 0)
			{
				return	HT_FR_FAIL_IO_PRINT;
			}
		}
	}
	else if (HT_UI_ACT_SPECIAL == act->type)
	{
		if (HT_UIMQ_ACT_PRINT_BOARD == * act_sp)
		{
			return	print_board(g);
		}
		else if (HT_UIMQ_ACT_PRINT_PLAYERS == * act_sp)
		{
			return	print_players(g);
		}
		else
		{
			return	HT_FR_FAIL_ENUM_UNK;
		}
	}
/* splint incomplete logic */ /*@i2@*/\
	else if (HT_UI_ACT_UNK == act->type)
	{
		/*
		 * Invalid input.
		 */
		if (HT_AIT_NONE == g->players[g->b->turn]->contr)
		{
			return	print_help();
		}
#ifdef HT_AI_MINIMAX
		else
		{
			fr = ui_player_contr_unset(g, g->b->turn);
			if (HT_FR_SUCCESS != fr)
			{
				return	fr;
			}
#ifndef HT_UNSAFE
			fr	= HT_FR_NONE;
#endif

			if (puts(HT_L_UIC_ACT_MOVE_UNK_AI) < 0)
			{
				return	HT_FR_FAIL_IO_PRINT;
			}
		}
#endif /* HT_AI_MINIMAX */
	}
	/*
	 * else: HT_UI_ACT_NONE or HT_UI_ACT_QUIT -- do no more.
	 */

	return HT_FR_SUCCESS;
}

static
enum HT_FR
player_action (
/*@in@*/
/*@notnull@*/
	struct game * const		g,
/*@in@*/
/*@notnull@*/
	struct ui * const		interf,
/*@partial@*/
/*@notnull@*/
	struct ht_line * const		line,
/*@partial@*/
/*@notnull@*/
	struct ui_action * const	act
	)
/*@globals internalState, fileSystem, errno, stdout, stdin@*/
/*@modifies internalState, fileSystem, errno, stdout, stdin, * g@*/
/*@modifies * act, * interf, * line@*/
{
	enum HT_FR		fr	= HT_FR_NONE;
	enum HT_UIMQ_ACTT	act_sp	= HT_UIMQ_ACT_UNK;
	struct player *		playercur;
#ifndef HT_UNSAFE
	if (NULL == g || NULL == g->b || NULL == interf || NULL == act)
	{
		return	HT_FR_FAIL_NULLPTR;
	}
#endif
	playercur	= g->players[g->b->turn];
	ui_action_init	(act);
	/*
	 * act->type = HT_UI_ACT_UNK.
	 */

	if (HT_AIT_NONE == playercur->contr)
	{
		fr	= cmd_human(g, line, act, interf, & act_sp);
		if (HT_FR_SUCCESS != fr)
		{
			return	fr;
		}
#ifndef HT_UNSAFE
		fr	= HT_FR_NONE;
#endif
	}
#ifdef HT_AI_MINIMAX
	else if (HT_AIT_MINIMAX == playercur->contr)
	{
		if (NULL == playercur->ai.minimax)
		{
			return	HT_FR_FAIL_NULLPTR;
		}

		fr = aiminimax_command(g, interf,
			playercur->ai.minimax, act);
		if (HT_FR_SUCCESS != fr)
		{
			return	fr;
		}
#ifndef HT_UNSAFE
		fr	= HT_FR_NONE;
#endif
	}
#endif /* HT_AI_MINIMAX */
	else
	{
		/*
		 * Unrecognized controller.
		 */
		return	HT_FR_FAIL_ENUM_UNK;
	}

	return action_perform(g, act, & act_sp);
}

/*
 * Plays a full game, from beginning to end.
 *
 * Resets the board for a new game automatically.
 */
enum HT_FR
uimq_play_new (
	struct game * const	g,
	struct ui * const	interf
	)
{
//
	startStdinThread();
	InitializeMessageQueue();
/*
	EnqueueMessage("p 0 m 3");
	EnqueueMessage("m 5 1 1 1");
*/
	struct ht_line *	line	= NULL;
	struct ui_action *	act	= NULL;
	enum HT_FR		fr	= HT_FR_NONE;
	unsigned short		winner	= HT_PLAYER_UNINIT;

#ifndef HT_UNSAFE
	if (NULL == g || NULL == interf)
	{
		return	HT_FR_FAIL_NULLPTR;
	}
#endif

	line	= interf->data.ui_mq->line;

	act	= malloc(sizeof(* act));
	if (NULL == act)
	{
		return		HT_FR_FAIL_ALLOC;
	}

	fr	= game_board_reset(g);
	if (HT_FR_SUCCESS != fr)
	{
		free		(act);
		return	fr;
	}
#ifndef HT_UNSAFE
	fr	= HT_FR_NONE;
#endif

	//interf->data.ui_mq->memfl = fopen("/hnef.tmp", "w+");
	fr	= print_board(g);
	if (HT_FR_SUCCESS != fr)
	{
		free	(act);
		return	fr;
	}
#ifndef HT_UNSAFE
	fr	= HT_FR_NONE;
#endif

	do
	{
		fr	= player_action(g, interf, line, act);
		if (HT_FR_SUCCESS != fr)
		{
			free		(act);
			return		fr;
		}
#ifndef HT_UNSAFE
		fr	= HT_FR_NONE;
#endif

		if (HT_UI_ACT_QUIT == act->type)
		{
			break;
		}
	} while (!game_over(g, & winner));
	/*
	 * NOTE:	winner is ignored.
	 */
	//fclose(interf->data.ui_mq->memfl);
	free		(act);
	return		HT_FR_SUCCESS;
}

enum HT_FR
uimq_play_again (
	const struct game * const HT_RSTR	g,
	struct uimq * const			ui_mq,
	HT_BOOL * const HT_RSTR			again
	)
{
	return	HT_FR_SUCCESS;
	if (0)
	{
	enum HT_FR	fr	= HT_FR_NONE;
	if (NULL != g) { }	/* -Wunused */

	do
	{
		if (printf("%s [%c/%c]\n", HT_L_UIC_PLAYAGAIN,
			HT_L_UIC_ARG_Q_YES, HT_L_UIC_ARG_Q_NO) < 0)
		{
			return	HT_FR_FAIL_IO_PRINT;
		}

		fr	= print_cmd_prompt();
		if (HT_FR_SUCCESS != fr)
		{
			return	fr;
		}
#ifndef HT_UNSAFE
		fr	= HT_FR_NONE;
#endif

		fr	= ht_line_readline(stdin, ui_mq->line);
		if (HT_FR_SUCCESS != fr)
		{
			return	fr;
		}
#ifndef HT_UNSAFE
		fr	= HT_FR_NONE;
#endif

		if (!ht_line_empty(ui_mq->line))
		{
			char ch	= ui_mq->line->words[0].chars[0];
			if (	HT_L_UIC_ARG_Q_YES	== ch)
			{
				* again	= HT_TRUE;
				return	HT_FR_SUCCESS;
			}
/* splint incomplete logic */ /*@i1@*/\

			else if (HT_L_UIC_ARG_Q_NO	== ch
			||	HT_L_UIC_ARG_QUIT	== ch)
			{
				* again	= HT_FALSE;
				return	HT_FR_SUCCESS;
			}
		}
	} while (0 == ferror(stdin) && 0 == feof(stdin));

	return	HT_FR_FAIL_IO_FILE_R;
	}
}

#ifdef HT_AI_MINIMAX

/*
 * NOTE:	There is no portable way to receive input from stdin
 *		without blocking, so therefore this function does
 *		nothing. It's hear for clarity -- any compiler worthy of
 *		its name should optimize it away.
 */
enum HT_FR
uimq_ai_stop (
	const struct game * const HT_RSTR	g,
	struct uimq * const HT_RSTR		ui_mq,
	HT_BOOL * const HT_RSTR			stop
	)
{
	if (NULL != g && NULL != ui_mq && NULL != stop) {} /* -Wunused */
	return	HT_FR_SUCCESS;
}

enum HT_FR
uimq_ai_progress (
	struct uimq * const HT_RSTR	ui_mq,
	const int			prog
	)
{
	unsigned short	symtoprt	= 0;
#ifndef HT_UNSAFE
	if (NULL == ui_mq)
	{
		return	HT_FR_FAIL_NULLPTR;
	}
#endif

	if (HT_AIPROG_START == prog)
	{
		ui_mq->aiprog_symprtd	= 0;
		if (EOF == fputc('\t', stdout))
		{
			return	HT_FR_FAIL_IO_PRINT;
		}
		return			HT_FR_SUCCESS;
	}
	else if (HT_AIPROG_DONE == prog)
	{
		symtoprt = (unsigned short)(9 * 3 + 1);
	}
	else if (prog >= HT_AIPROG_MIN && prog <= HT_AIPROG_MAX)
	{
		symtoprt = (unsigned short)((prog * 9 * 3 + 1) / 100);
	}
	else
	{
		return	HT_FR_FAIL_ILL_ARG;
	}

	while (ui_mq->aiprog_symprtd < symtoprt)
	{
		unsigned short	tmp	= ui_mq->aiprog_symprtd % 3;
		char		ch;
		if (tmp < (unsigned short)1)
		{
			ch	= '-';
		}
		else if (tmp < (unsigned short)2)
		{
			ch	= '(';
		}
		else
		{
			ch	= '}';
		}

		if (EOF == fputc(ch, stdout))
		{
			return	HT_FR_FAIL_IO_PRINT;
		}
		ui_mq->aiprog_symprtd++;
	}

	if (EOF == fflush(stdout))
	{
		return	HT_FR_FAIL_IO_PRINT;
	}

	if (HT_AIPROG_DONE == prog)
	{
		if (EOF == fputc('\n', stdout))
		{
			return	HT_FR_FAIL_IO_PRINT;
		}
	}

	return	HT_FR_SUCCESS;
}

#endif /* HT_AI_MINIMAX */

struct uimq *
alloc_uimq (
	const struct game * const g
	)
{
	struct uimq * const	ui_mq = malloc(sizeof(* ui_mq));
	if (NULL == ui_mq)
	{
		return	NULL;
	}

	ui_mq->line	= alloc_ht_line((size_t)4, (size_t)11);
	if (NULL == ui_mq->line)
	{
		free	(ui_mq);
		return	NULL;
	}
	ui_mq->line_out = malloc(255);
	ui_mq->line_in = malloc(64);
	if (NULL != g) {}	/* -Wunused */
	return	ui_mq;
}

void
free_uimq (
	struct uimq * const	ui_mq
	)
{
	if (NULL != ui_mq->line)
	{
		free_ht_line	(ui_mq->line);
	}
	free(ui_mq->line_out);
	free(ui_mq->line_in);
	free	(ui_mq);
}

enum HT_FR
uimq_init (
	const struct game * const HT_RSTR	g,
	struct uimq * const HT_RSTR		ui_mq
	)
{
	if (NULL == g) {}	/* -Wunused */
	ui_mq->aiprog_symprtd	= (unsigned short)0;
	return	HT_FR_SUCCESS;
}

#endif

