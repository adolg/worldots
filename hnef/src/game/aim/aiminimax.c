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

#include <stdlib.h>	/* malloc, free */

#ifdef HT_AI_MINIMAX_HASH
#include "aimhash.h"	/* alloc_zhashtable, free_zhashtable, ... */
#endif
#include "aiminimax.h"
#include "board.h"	/* alloc_board, free_board */
#include "listm.h"	/* alloc_listm, free_listm */
#include "listmh.h"	/* alloc_listmh, free_listmh */
#include "player.h"	/* player_index_valid */

/*
 * Default listm capacity for moves when allocating.
 *
 * This shouldn't grow too many times: 80 -> 120 -> 180.
 *
 * It may be excessive on 7x7 boards, and far from enough on 19x19, but
 * it should be enough with only a few growths on 9x9 to 13x13.
 */
/*@unchecked@*/
static
const size_t HT_AIM_OPT_LIST_CAP_DEF	= (size_t)80;

/*
 * Value of an escape square in aiminimax->board_escval.
 *
 * NOTE:	You can tune this value.
 */
/*@unchecked@*/
static
const int HT_AIM_ESCDIST_BASE	= 20;

/*
 * Modification in value for every step from an escape square in
 * aiminimax->board_escval. This should be negative to make the computer
 * player prefer moving toward the escape square; and it must be
 * negative to prevent infinite recursion when assigning values.
 *
 * NOTE:	You can tune this value.
 */
/*@unchecked@*/
static
const int HT_AIM_ESCDIST_MOD	= -2;

HT_BOOL
aiminimax_depth_max_valid (
	const unsigned short	depth_max
	)
{
	return depth_max >= HT_AIM_DEPTHMAX_MIN
	&& depth_max <= HT_AIM_DEPTHMAX_MAX ? HT_TRUE : HT_FALSE;
}

/*@null@*/
/*@only@*/
/*@partial@*/
static
struct listm * *
alloc_opt_buf_moves (
	const unsigned short opt_buf_len
	)
/*@modifies nothing@*/
{
	unsigned short	i;
	struct listm * * opt_buf_moves = malloc(sizeof(* opt_buf_moves)
		* opt_buf_len);
	if (NULL == opt_buf_moves)
	{
		return		NULL;
	}
	for (i = (unsigned short)0; i < opt_buf_len; i++)
	{
		opt_buf_moves[i] = alloc_listm(HT_AIM_OPT_LIST_CAP_DEF);
		if (NULL == opt_buf_moves[i])
		{
			unsigned short j;
			for (j = (unsigned short)0; j < i; j++)
			{
				free_listm(opt_buf_moves[j]);
			}
			free	(opt_buf_moves);
			return	NULL;
		}
	}
	return	opt_buf_moves;
}

/*@null@*/
/*@only@*/
/*@partial@*/
static
struct board * *
alloc_opt_buf_board (
	const unsigned short	opt_buf_len,
	const unsigned short	blen
	)
/*@modifies nothing@*/
{
	unsigned short	i;
	struct board * * opt_buf_board = malloc(sizeof(* opt_buf_board)
		* opt_buf_len);
	if (NULL == opt_buf_board)
	{
		return	NULL;
	}
	for (i = (unsigned short)0; i < opt_buf_len; i++)
	{
		opt_buf_board[i] = alloc_board(blen);
		if (NULL == opt_buf_board[i])
		{
			unsigned short j;
			for (j = (unsigned short)0; j < i; j++)
			{
				free_board(opt_buf_board[j]);
			}
			free	(opt_buf_board);
			return	NULL;
		}
	}
	return	opt_buf_board;
}

/*
 * This function assigns value to pos_x / pos_y in b_escval, and then
 * recurses with value - HT_AIM_ESCDIST_MOD on every adjacent square,
 * stopping when it encounters a higher value or when value is reduced
 * to 0.
 *
 * NOTE:	This function uses recursion and assumes that the call
 *		stack won't be depleted. Since the maximum board size is
 *		19, this shouldn't ever be close to happening regardless
 *		of ruleset.
 */
static
void
escval_walk (
/*@in@*/
/*@notnull@*/
	const struct ruleset * const HT_RSTR	r,
/*@in@*/
/*@notnull@*/
	int *					b_escval,
	const unsigned short			pos_x,
	const unsigned short			pos_y,
	const int				value
	)
/*@modifies * b_escval@*/
{
	unsigned short	pos;
	if (pos_x >= r->bwidth || pos_y >= r->bheight || value <= 0)
	{
		return;
	}

	pos	= (unsigned short)(pos_y * r->bheight + pos_x);

	if (value > b_escval[pos])
	{
		b_escval[pos]	= value;
	}
	else /* if (value <= b_escval[pos]) */
	{
		return;
	}

	escval_walk(r, b_escval, (unsigned short)(pos_x + 1),
				(unsigned short)pos_y,
		value + HT_AIM_ESCDIST_MOD);
	escval_walk(r, b_escval, (unsigned short)(pos_x - 1),
				(unsigned short)pos_y,
		value + HT_AIM_ESCDIST_MOD);
	escval_walk(r, b_escval, (unsigned short)pos_x,
				(unsigned short)(pos_y + 1),
		value + HT_AIM_ESCDIST_MOD);
	escval_walk(r, b_escval, (unsigned short)pos_x,
				(unsigned short)(pos_y - 1),
		value + HT_AIM_ESCDIST_MOD);
}

static
enum HT_FR
aiminimax_board_escval_init (
/*@in@*/
/*@notnull@*/
	const struct game * const HT_RSTR	g,
/*@in@*/
/*@notnull@*/
	int * const HT_RSTR			board_escval
	)
/*@modifies * board_escval@*/
{
	unsigned short	pos;

	if (HT_AIM_ESCDIST_MOD >= 0)
	{
		/*
		 * This will cause infinite recursion.
		 */
		return	HT_FR_FAIL_ILL_STATE;
	}

	for (pos = (unsigned short)0; pos < g->rules->opt_blen; pos++)
	{
		const HT_BIT_U8 sbit	= g->rules->squares[pos];
		if (((unsigned int)sbit
			& (unsigned int)g->rules->opt_ts_escape)
			== (unsigned int)sbit)
		{
			const unsigned short pos_x = (unsigned short)
				(pos % g->rules->bwidth);
			const unsigned short pos_y = (unsigned short)
				(pos / g->rules->bwidth);
			escval_walk(g->rules, board_escval,
				pos_x, pos_y, HT_AIM_ESCDIST_BASE);
		}
	}
	return	HT_FR_SUCCESS;
}

/*
 * Allocates all variables and initializes everything.
 *
 * Returning (as fr) HT_FR_FAIL_ILL_ARG means that p_index or depth_max
 * are invalid, but otherwise there are no fatal errors.
 *
 * Returning any other HT_FR_FAIL_* is program-fatal.
 *
 * HT_FR_SUCCESS means all parameters were valid and everything worked.
 *
 * old_tab can be given to transfer an old hash table, so that it
 * doesn't have to be allocated again. If it's given (non-NULL), then it
 * will be preserved (not released!) if the AI fails to allocate.
 */
struct aiminimax *
alloc_aiminimax_init (
	const struct game * const	g,
	const unsigned short		p_index,
	const unsigned short		depth_max,
	enum HT_FR * const HT_RSTR	fr
#ifdef HT_AI_MINIMAX_HASH
	,
	struct zhashtable * const	old_tab
#endif
	)
{
	struct aiminimax *	aim = NULL;
#ifndef HT_UNSAFE
	if (NULL == g)
	{
		* fr	= HT_FR_FAIL_NULLPTR;
		return	NULL;
	}
#endif

	/*
	 * HT_FR_FAIL_ILL_ARG are non-fatal.
	 */
	if (!player_index_valid(p_index))
	{
		* fr	= HT_FR_FAIL_ILL_ARG;
		return	NULL;
	}
	if (!aiminimax_depth_max_valid(depth_max))
	{
		* fr	= HT_FR_FAIL_ILL_ARG;
		return	NULL;
	}

	aim	= malloc(sizeof(* aim));
	if (NULL == aim)
	{
		* fr	= HT_FR_FAIL_ALLOC;
		return	NULL;
	}

	/*
	 * Set to NULL here for free_aiminimax.
	 */
	aim->board_escval	= NULL;
	aim->opt_moves		= NULL;
	aim->opt_buf_moves	= NULL;
	aim->opt_buf_board	= NULL;
	aim->opt_movehist	= NULL;

#ifdef HT_AI_MINIMAX_HASH
	aim->tp_tab		= NULL;
#endif
	aim->p_index		= p_index;
	aim->depth_max		= depth_max;
	aim->opt_buf_len	= (unsigned short)(aim->depth_max + 1);

	aim->opt_moves		= alloc_listm(HT_AIM_OPT_LIST_CAP_DEF);
	if (NULL == aim->opt_moves)
	{
		free_aiminimax	(aim);
		* fr		= HT_FR_FAIL_ALLOC;
		return		NULL;
	}

	aim->opt_movehist	= alloc_listmh(HT_LISTMH_CAP_DEF);
	if (NULL == aim->opt_movehist)
	{
		free_aiminimax	(aim);
		* fr		= HT_FR_FAIL_ALLOC;
		return		NULL;
	}

	aim->opt_buf_moves	= alloc_opt_buf_moves(aim->opt_buf_len);
	if (NULL == aim->opt_buf_moves)
	{
		free_aiminimax	(aim);
		* fr		= HT_FR_FAIL_ALLOC;
		return		NULL;
	}

	aim->opt_buf_board	= alloc_opt_buf_board(aim->opt_buf_len,
		g->rules->opt_blen);
	if (NULL == aim->opt_buf_board)
	{
		free_aiminimax	(aim);
		* fr		= HT_FR_FAIL_ALLOC;
		return		NULL;
	}

	aim->board_escval	= calloc((size_t)g->rules->opt_blen,
					sizeof(* aim->board_escval));
	if (NULL == aim->board_escval)
	{
		free_aiminimax	(aim);
		* fr		= HT_FR_FAIL_ALLOC;
		return		NULL;
	}
	* fr	= aiminimax_board_escval_init(g, aim->board_escval);
	if (HT_FR_SUCCESS != * fr)
	{
		free_aiminimax	(aim);
		return		NULL;
	}

#ifdef HT_AI_MINIMAX_HASH
	if (NULL == old_tab)
	{
/*
 * splint complains about a possible memory leak here. The problem is
 * this:
 * 1.	When we create a completely new AI, we give it a NULL parameter
 *	as old_tab, indicating that it should be allocated.
 * 2.	When we create an AI to replace one that already exists (to
 *	change search depth), then it's unnecessary to allocate the
 *	whole table again, since it could be >100 MB. Then we just
 *	transfer the pointers.
 * 3.	Therefore aim->tp_tab has to be dependent, because ui.c will
 *	handle transferring it. If it's not NULL in free_aiminimax, then
 *	it will be freed -- else ui.c will free it.
 * So there is no problem. I have tested it.
 */
/*@i1@*/\
		aim->tp_tab		= alloc_zhashtable_init(g);
		if (NULL == aim->tp_tab)
		{
			free_aiminimax	(aim);
			* fr		= HT_FR_FAIL_ALLOC;
			return		NULL;
		}
	}
	else
	{
		/*
		 * Old table given as parameter. Simply set the pointer.
		 */
		aim->tp_tab	= old_tab;
	}
#endif

	* fr	= HT_FR_SUCCESS;
	return aim;
}

void
free_aiminimax (
	struct aiminimax * const aim
	)
{
	if (NULL != aim->board_escval)
	{
		free	(aim->board_escval);
	}

#ifdef HT_AI_MINIMAX_HASH
	if (NULL != aim->tp_tab)
	{
		/*
		 * Note that we may set tp_tab to NULL when we create a
		 * new minimax player, in order to not have to
		 * re-allocate the hashtable (which is very large).
		 * Therefore we actually depend on this variable
		 * possibly being NULL, but only in this function.
		 */
		free_zhashtable	(aim->tp_tab);
	}
#endif

	if (NULL != aim->opt_buf_board)
	{
		unsigned short i;
		for (i = (unsigned short)0;
		i < (unsigned short)(aim->depth_max + 1); i++)
		{
			free_board	(aim->opt_buf_board[i]);
		}
		free	(aim->opt_buf_board);
	}

	if (NULL != aim->opt_buf_moves)
	{
		unsigned short i;
		for (i = (unsigned short)0;
		i < (unsigned short)(aim->depth_max + 1); i++)
		{
			free_listm	(aim->opt_buf_moves[i]);
		}
		free	(aim->opt_buf_moves);
	}

	if (NULL != aim->opt_moves)
	{
		free_listm	(aim->opt_moves);
	}

	if (NULL != aim->opt_movehist)
	{
		free_listmh	(aim->opt_movehist);
	}

	free(aim);
}

#endif

