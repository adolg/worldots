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

#include <limits.h>	/* INT_* */

#include "aimalgo.h"
#ifdef HT_AI_MINIMAX_HASH
#include "aimhash.h"	/* zhashtable_clearunused, ... */
#endif
#include "aiminimaxt.h"	/* HT_AIM_DEPTHMAX_MAX */
#include "board.h"	/* board_* */
#include "listm.h"	/* listm_clear */
#include "listmh.h"	/* listmh_copy */
#include "ui.h"		/* ui_ai_progress */

/*
 * Maximum value of a move.
 */
/*@unchecked@*/
static
const int HT_AIM_VALUE_INFMAX = INT_MAX;

/*
 * Minimum value of a move.
 */
/*@unchecked@*/
static
const int HT_AIM_VALUE_INFMIN = INT_MIN;

/*
 * Victory threshhold.
 *
 * This value exists because the value of a move decreases by -1 if it's
 * deeper down the search tree (because we want the computer to prefer
 * moves that are instantly winning over winning moves that are several
 * moves ahead). But we still want the computer player to recognize that
 * the value is a winning move so he can do it immediately without
 * checking the rest of the move list. Therefore the computer only needs
 * to check for this value when determining (in search()) if a move is a
 * win.
 */
/*@unchecked@*/
static
const int HT_AIM_VALUE_WIN = INT_MAX - (HT_AIM_DEPTHMAX_MAX + 1);

/*
 * Value of a piece on the board.
 *
 * NOTE:	You can tune this value.
 */
/*@unchecked@*/
static
const int HT_AIM_VALUE_PIECE		= 1000;

/*
 * Value of a possible move by the king (piece with type_piece.escapes).
 *
 * NOTE:	You can tune this value.
 */
/*@unchecked@*/
static
const int HT_AIM_VALUE_MOVE_KING	= 200;

/*
 * When a piece with escape is near an escape square, the value in
 * aiminimax->board_escval for that position is multiplied by this
 * value.
 */
/*@unchecked@*/
static
const int HT_AIM_ESCVAL_KING	= 10;

/*
 * When a piece is near an escape square, the value in
 * aiminimax->board_escval for that position is multiplied by this
 * value.
 */
/*@unchecked@*/
static
const int HT_AIM_ESCVAL_NORM	= 1;

#ifdef HT_AI_MINIMAX_UNSAFE

/*
 * Optimization.
 *
 * Used to not have to propagate a HT_FR value from min or max.
 *
 * If HT_AI_MINIMAX_UNSAFE is enabled, it's still possible that min() or
 * max() calls board_moves_get() which has to allocate memory for more
 * moves in the move list (listm_add() which calls listm_grow()). Thus
 * it's conceivable that we get a HT_FR_FAIL_ALLOC that must be caught.
 * It's not reasonable to ignore out of memory conditions under any
 * circumstances, so this global allows us to check it in search() after
 * each recursion has finished.
 *
 * This is only ever set by board_moves_get() in min() or max(). It's
 * never set anywhere else. It's only checked in search().
 */
/*@unchecked@*/
static
enum HT_FR opt_fr	= HT_FR_SUCCESS;

#endif /* HT_AI_MINIMAX_UNSAFE */

/*
 * Evaluates the board position for aim->p_index.
 *
 * For every player:
 *
 * *	A piece on the board is worth HT_AIM_VALUE_PIECE.
 *
 * *	A possible move by the king is worth HT_AIM_VALUE_MOVE_KING.
 */
static
int
aiminimax_evaluate (
/*@in@*/
/*@notnull@*/
	const struct game * const HT_RSTR	g,
/*@in@*/
/*@notnull@*/
	const struct board * const HT_RSTR	b,
/*@in@*/
/*@notnull@*/
	const struct aiminimax * const HT_RSTR	aim
	)
/*@modifies nothing@*/
{
	int		value	= 0;
	unsigned short	i;
	HT_BIT_U8	pbit;
	HT_BIT_U8	p_owned = g->players[aim->p_index]->opt_owned;

	/*
	 * Material value.
	 *
	 * A piece on the board is worth HT_AIM_VALUE_PIECE.
	 *
	 * In addition, it's worth whatever the value of board_escval is
	 * for that position. For "king" pieces (with escape), it's
	 * multiplied by HT_AIM_ESCVAL_KING, and for ordinary pieces by
	 * HT_AIM_ESCVAL_NORM. This makes the computer player a little
	 * more eager to defend escape squares as well as move the king
	 * to them.
	 */
	for (i = (unsigned short)0; i < g->rules->opt_blen; i++)
	{
		pbit	= b->pieces[i];
		if (HT_BIT_U8_EMPTY != pbit)
		{
			const HT_BOOL owned = ((unsigned int)pbit
				& (unsigned int) p_owned)
				== (unsigned int)pbit;
			const HT_BOOL king = ((unsigned int)pbit
				& (unsigned int)g->rules->opt_tp_escape)
				== (unsigned int)pbit;
			value += ((HT_AIM_VALUE_PIECE +
				aim->board_escval[i] * (king ?
					HT_AIM_ESCVAL_KING :
					HT_AIM_ESCVAL_NORM)))
				* (owned ? 1 : -1);
		}
	}

	/*
	 * Tactical value.
	 *
	 * It takes a very long time to call board_movec_get, so only do
	 * it for the player with the king piece. Don't count move value
	 * for all pieces.
	 */
	for (i = (unsigned short)0; i < g->playerc; i++)
	{
		const struct player * const HT_RSTR p = g->players[i];
		if (HT_BIT_U8_EMPTY != p->opt_owned_esc)
		{
			value += ((board_movec_get(g, b,
				aim->opt_movehist, p->opt_owned_esc)
				* HT_AIM_VALUE_MOVE_KING)
				* ((aim->p_index == i) ? 1 : -1));
		}
	}

	return	value;
}

/*@-protoparamname@*/
static
int
aiminimax_min (
/*@in@*/
/*@notnull@*/
	const struct game * const HT_RSTR	g,
/*@in@*/
/*@notnull@*/
	const struct board * const HT_RSTR	b_old,
/*@in@*/
/*@notnull@*/
	struct aiminimax * const		aim,
	const int				alpha,
	int					beta,
	const unsigned short			depth,
	const unsigned short			depthmax
#ifdef HT_AI_MINIMAX_HASH
	,
	const unsigned int			hashkey,
	const unsigned int			hashlock
#endif
#ifndef HT_AI_MINIMAX_UNSAFE
	,
/*@in@*/
/*@notnull@*/
	enum HT_FR * const			fr
#endif
	)
/*@modifies * aim@*/
#ifndef HT_AI_MINIMAX_UNSAFE
/*@modifies * fr@*/
#endif
;
/*@=protoparamname@*/

/*
 * Like aiminimax_min, but maximizes the lowest possible value (alpha
 * cutoff) for the computer player.
 *
 * The current best value is worth at least alpha.
 */
static
int
aiminimax_max (
/*@in@*/
/*@notnull@*/
	const struct game * const HT_RSTR	g,
/*@in@*/
/*@notnull@*/
	const struct board * const HT_RSTR	b_old,
/*@in@*/
/*@notnull@*/
	struct aiminimax * const		aim,
	int					alpha,
	const int				beta,
	const unsigned short			depth,
	const unsigned short			depthmax
#ifdef HT_AI_MINIMAX_HASH
	,
	const unsigned int			hashkey,
	const unsigned int			hashlock
#endif
#ifndef HT_AI_MINIMAX_UNSAFE
	,
/*@in@*/
/*@notnull@*/
	enum HT_FR * const			fr
#endif
	)
/*@modifies * aim@*/
#ifndef HT_AI_MINIMAX_UNSAFE
/*@modifies * fr@*/
#endif
{
	unsigned short			winner	= HT_PLAYER_UNINIT;

#ifdef HT_AI_MINIMAX_HASH
	struct zhashnode * const HT_RSTR node	= zhashtable_get
		(aim->tp_tab, hashkey, hashlock, b_old->turn);
	if (NULL != node
	&& node->depthleft >= (unsigned short)(depthmax - depth))
	{
		/*
		 * NOTE:	Assumes that value can't be VACANT.
		 */
		if (HT_AIM_ZHT_BETA != node->value_type)
		{
			node->used	= HT_TRUE;
			if (HT_AIM_VALUE_INFMAX == node->value)
			{
				return		node->value - depth;
			}
			else
			{
				return		node->value;
			}
		}
	}
#endif

	if (board_game_over(g, b_old, aim->opt_movehist, & winner))
	{
		if (aim->p_index == winner)
		{
#ifdef HT_AI_MINIMAX_HASH
			zhashtable_put(aim->tp_tab, b_old,
				HT_AIM_VALUE_INFMAX, HT_AIM_ZHT_EXACT,
				(unsigned short)(depthmax - depth));
#endif
			return	HT_AIM_VALUE_INFMAX - depth;
		}
		else
		{
#ifdef HT_AI_MINIMAX_HASH
			zhashtable_put(aim->tp_tab, b_old,
				HT_AIM_VALUE_INFMIN, HT_AIM_ZHT_EXACT,
				(unsigned short)(depthmax - depth));
#endif
			return	HT_AIM_VALUE_INFMIN;
		}
	}
	else if (depth >= depthmax)
	{
		const int value = aiminimax_evaluate(g, b_old, aim);
#ifdef HT_AI_MINIMAX_HASH
		zhashtable_put(aim->tp_tab, b_old, value,
			HT_AIM_ZHT_EXACT,
			(unsigned short)(depthmax - depth));
#endif
		return	value;
	}
	else
	{
		int			tmp	= 0;
		size_t			i;
		struct listm * const moves = aim->opt_buf_moves[depth];
		struct move * HT_RSTR	mov;
#ifdef HT_AI_MINIMAX_HASH
		unsigned int		keytmp, locktmp;
#endif
		listm_clear(moves);

#ifndef HT_AI_MINIMAX_UNSAFE
		* fr = board_moves_get(g, b_old, aim->opt_movehist,
			moves);
		if (HT_FR_SUCCESS != * fr)
		{
			return	HT_AIM_VALUE_INFMIN;
		}
#else
		if (HT_FR_SUCCESS != board_moves_get(g, b_old,
			aim->opt_movehist, moves))
		{
			opt_fr	= HT_FR_FAIL_ALLOC;
			return	HT_AIM_VALUE_INFMIN;
		}
#endif

#ifndef HT_AI_MINIMAX_UNSAFE
		if (moves->elemc < (size_t)1)
		{
			/*
			 * board_game_over should have triggered.
			 */
			* fr	= HT_FR_FAIL_ILL_STATE;
			return	HT_AIM_VALUE_INFMIN;
		}
#endif
		for (i = (size_t)0; i < moves->elemc; i++)
		{
			struct board * const b_new =
				aim->opt_buf_board[depth];
			board_copy(b_old, b_new, g->rules->opt_blen);
			mov	= & moves->elems[i];
#ifdef HT_AI_MINIMAX_HASH
			keytmp	= hashkey;
			locktmp	= hashlock;
#endif

#ifndef HT_AI_MINIMAX_UNSAFE
			* fr =
				board_move_unsafe(g, b_new,
				aim->opt_movehist, mov->pos, mov->dest
#ifdef HT_AI_MINIMAX_HASH
				, aim->tp_tab, & keytmp, & locktmp
#endif
				);
			if (HT_FR_SUCCESS != * fr)
			{
				return	HT_AIM_VALUE_INFMIN;
			}
#else /* ifdef HT_AI_MINIMAX_UNSAFE */
			if (HT_FR_SUCCESS != board_move_unsafe(g, b_new,
				aim->opt_movehist, mov->pos, mov->dest
#ifdef HT_AI_MINIMAX_HASH
				, aim->tp_tab, & keytmp, & locktmp
#endif
				))
			{
				opt_fr	= HT_FR_FAIL_ALLOC;
				return	HT_AIM_VALUE_INFMIN;
			}
#endif /* HT_AI_MINIMAX_UNSAFE */

			tmp = aiminimax_min(g, b_new, aim, alpha, beta,
				(unsigned short)(depth + 1), depthmax
#ifdef HT_AI_MINIMAX_HASH
				, keytmp, locktmp
#endif
#ifndef HT_AI_MINIMAX_UNSAFE
				, fr
#endif
				);
#ifndef HT_AI_MINIMAX_UNSAFE
			if (HT_FR_SUCCESS != * fr)
			{
				return	HT_AIM_VALUE_INFMIN;
			}
#endif

			aim->opt_movehist->elemc--;

			if (tmp > alpha)
			{
				alpha	= tmp;
			}
			if (alpha >= beta)
			{
#ifdef HT_AI_MINIMAX_HASH
				zhashtable_put(aim->tp_tab, b_new,
					alpha, HT_AIM_ZHT_ALPHA,
					(unsigned short)
						(depthmax - depth));
#endif
				return	alpha;
			}
		}
#ifdef HT_AI_MINIMAX_HASH
		zhashtable_put(aim->tp_tab, b_old, alpha,
			HT_AIM_ZHT_ALPHA,
			(unsigned short)(depthmax - depth));
#endif
		return	alpha;
	}
}

/*
 * Minimizes the highest possible value (beta cutoff) for the opponent.
 *
 * The current best value is worth at most beta.
 *
 * Upon successful return, returns an evaluated value.
 *
 * Upon failed return, returns 0.
 *
 * This function never sets HT_FR fr parameter to anything else but a
 * failure code in the event of a failure. It assumes that the fr
 * parameter is HT_FR_SUCCESS when passed to this function, and will not
 * change it upon successful execution.
 *
 * If HT_AI_MINIMAX_UNSAFE is enabled, then this function could possibly
 * receive a HT_FR_FAIL_ALLOC from listm_add(). If so, it aborts. This
 * only happens if HT_AI_MINIMAX_UNSAFE is enabled -- it doesn't abort
 * otherwise.
 *
 * Stops searching when depth >= depth_max.
 */
static
int
aiminimax_min (
	const struct game * const HT_RSTR	g,
	const struct board * const HT_RSTR	b_old,
	struct aiminimax * const		aim,
	const int				alpha,
	int					beta,
	const unsigned short			depth,
	const unsigned short			depthmax
#ifdef HT_AI_MINIMAX_HASH
	, const unsigned int			hashkey,
	const unsigned int			hashlock
#endif
#ifndef HT_AI_MINIMAX_UNSAFE
	,
	enum HT_FR * const			fr
#endif
	)
{
	/*
	 * This function is called literally millions of times.
	 * Therefore we never do NULL-checks here, not even without
	 * HT_UNSAFE.
	 */
	unsigned short			winner	= HT_PLAYER_UNINIT;

#ifdef HT_AI_MINIMAX_HASH
	struct zhashnode * const HT_RSTR node	= zhashtable_get
		(aim->tp_tab, hashkey, hashlock, b_old->turn);
	if (NULL != node
	&& node->depthleft >= (unsigned short)(depthmax - depth))
	{
		/*
		 * Position already evaluated.
		 */
		if (HT_AIM_ZHT_ALPHA != node->value_type)
		{
			node->used	= HT_TRUE;
			if (HT_AIM_VALUE_INFMAX == node->value)
			{
				return		node->value - depth;
			}
			else
			{
				return		node->value;
			}
		}
	}
#endif

	if (board_game_over(g, b_old, aim->opt_movehist, & winner))
	{
		if (aim->p_index == winner)
		{
#ifdef HT_AI_MINIMAX_HASH
			zhashtable_put(aim->tp_tab, b_old,
				HT_AIM_VALUE_INFMAX, HT_AIM_ZHT_EXACT,
				(unsigned short)(depthmax - depth));
#endif
			return	HT_AIM_VALUE_INFMAX - depth;
		}
		else
		{
#ifdef HT_AI_MINIMAX_HASH
			zhashtable_put(aim->tp_tab, b_old,
				HT_AIM_VALUE_INFMIN, HT_AIM_ZHT_EXACT,
				(unsigned short)(depthmax - depth));
#endif
			return	HT_AIM_VALUE_INFMIN;
		}
	}
	else if (depth >= depthmax)
	{
		const int value = aiminimax_evaluate(g, b_old, aim);
#ifdef HT_AI_MINIMAX_HASH
		zhashtable_put(aim->tp_tab, b_old, value,
			HT_AIM_ZHT_EXACT,
			(unsigned short)(depthmax - depth));
#endif
		return	value;
	}
	else
	{
		int			tmp	= 0;
		size_t			i;
		struct listm * const moves = aim->opt_buf_moves[depth];
		struct move * HT_RSTR	mov;
#ifdef HT_AI_MINIMAX_HASH
		unsigned int		keytmp, locktmp;
#endif
		listm_clear(moves);

#ifndef HT_AI_MINIMAX_UNSAFE
		* fr = board_moves_get(g, b_old, aim->opt_movehist,
			moves);
		if (HT_FR_SUCCESS != * fr)
		{
			return	HT_AIM_VALUE_INFMIN;
		}
#else
		if (HT_FR_SUCCESS != board_moves_get(g, b_old,
			aim->opt_movehist, moves))
		{
			opt_fr	= HT_FR_FAIL_ALLOC;
			return	HT_AIM_VALUE_INFMIN;
		}
#endif

#ifndef HT_AI_MINIMAX_UNSAFE
		if (moves->elemc < (size_t)1)
		{
			/*
			 * board_game_over should have triggered.
			 */
			* fr	= HT_FR_FAIL_ILL_STATE;
			return	HT_AIM_VALUE_INFMIN;
		}
#endif
		for (i = (size_t)0; i < moves->elemc; i++)
		{
			struct board * const b_new =
				aim->opt_buf_board[depth];
			board_copy(b_old, b_new, g->rules->opt_blen);
			mov	= & moves->elems[i];
#ifdef HT_AI_MINIMAX_HASH
			keytmp	= hashkey;
			locktmp	= hashlock;
#endif

#ifndef HT_AI_MINIMAX_UNSAFE
			* fr =
				board_move_unsafe(g, b_new,
				aim->opt_movehist, mov->pos, mov->dest
#ifdef HT_AI_MINIMAX_HASH
				, aim->tp_tab, & keytmp, & locktmp
#endif
				);
			if (HT_FR_SUCCESS != * fr)
			{
				return	HT_AIM_VALUE_INFMIN;
			}
#else /* ifdef HT_AI_MINIMAX_UNSAFE */
			if (HT_FR_SUCCESS != board_move_unsafe(g, b_new,
				aim->opt_movehist, mov->pos, mov->dest
#ifdef HT_AI_MINIMAX_HASH
				, aim->tp_tab, & keytmp, & locktmp
#endif
				))
			{
				opt_fr	= HT_FR_FAIL_ALLOC;
				return	HT_AIM_VALUE_INFMIN;
			}
#endif /* HT_AI_MINIMAX_UNSAFE */

			tmp = aiminimax_max(g, b_new, aim, alpha, beta,
				(unsigned short)(depth + 1), depthmax
#ifdef HT_AI_MINIMAX_HASH
				, keytmp, locktmp
#endif
#ifndef HT_AI_MINIMAX_UNSAFE
				, fr
#endif
				);
#ifndef HT_AI_MINIMAX_UNSAFE
			if (HT_FR_SUCCESS != * fr)
			{
				return	HT_AIM_VALUE_INFMIN;
			}
#endif

			aim->opt_movehist->elemc--;

			if (tmp < beta)
			{
				beta	= tmp;
			}
			if (alpha >= beta)
			{
#ifdef HT_AI_MINIMAX_HASH
				zhashtable_put(aim->tp_tab, b_new,
					beta, HT_AIM_ZHT_BETA,
					(unsigned short)
						(depthmax - depth));
#endif
				return	beta;
			}
		}
#ifdef HT_AI_MINIMAX_HASH
		zhashtable_put(aim->tp_tab, b_old, beta,
			HT_AIM_ZHT_BETA,
			(unsigned short)(depthmax - depth));
#endif
		return	beta;
	}
}

/*
 * Searches b_orig for moves in a minimax search down to a maximum depth
 * of depth_cur.
 *
 * The best move is sorted first in aim->opt_moves (aim->opt_moves[0] is
 * always the best move that this function could find).
 *
 * If this function sets win_found to true, then aim->opt_moves[0] is
 * not only the best move that it could find, but also a certain win
 * that should be carried out immediately without calling this function
 * again.
 */
static
enum HT_FR
aiminimax_search (
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
	const struct board * const HT_RSTR	b_orig,
	const unsigned short			depth_cur,
/*@in@*/
/*@notnull@*/
	HT_BOOL * const HT_RSTR			win_found,
/*@in@*/
/*@notnull@*/
	HT_BOOL * const				ui_stop
	)
/*@globals errno, fileSystem, stdout@*/
/*@modifies errno, fileSystem, stdout, * interf, * aim, * win_found@*/
/*@modifies * ui_stop@*/
{
	int			alpha	= HT_AIM_VALUE_INFMIN;
	const int		beta	= HT_AIM_VALUE_WIN;
	enum HT_FR		fr	= HT_FR_NONE;
	int			i;
	struct board *		b_new	= NULL;
	int			value	= alpha;
	struct move * HT_RSTR	mtmp;
	struct listm *		moves	= NULL;
	struct move		mcache;
#ifdef HT_AI_MINIMAX_HASH
	unsigned int		hashkey, hashlock;
#endif
	/*
	 * aim->opt_moves is the list of moves that we are using in the
	 * search.
	 *
	 * mcache is the previous best move. Since the algorithm will
	 * check the best move from the previous search first, it will
	 * also discard that move first if all moves are bad. That's why
	 * we remember that move in case the computer decides that all
	 * moves are bad. If so, the previously apparently least bad
	 * move will be returned. This is important if the computer
	 * thinks further than the human, because the human may not have
	 * realized that the move is bad. Otherwise the computer will
	 * make random moves when it realizes that defeat is certain.
	 * By doing this, it won't appear to give up as fast.
	 */
#ifndef HT_UNSAFE
	if (NULL == aim || NULL == b_orig)
	{
		return	HT_FR_FAIL_NULLPTR;
	}
#endif

#ifdef HT_AI_MINIMAX_HASH
	hashkey		= zhashkey	(aim->tp_tab, b_orig);
	hashlock	= zhashlock	(aim->tp_tab, b_orig);
#endif

	moves		= aim->opt_moves;
	mcache.pos	= moves->elems[0].pos;
	mcache.dest	= moves->elems[0].dest;

	fr	= ui_ai_progress(interf, HT_AIPROG_START);
	if (HT_FR_SUCCESS != fr)
	{
		return	fr;
	}
#ifndef HT_UNSAFE
	fr	= HT_FR_NONE;
#endif

	for (i = 0; i < (int)moves->elemc; i++)
	{
		fr = ui_ai_stop(g, interf, ui_stop);
		if (HT_FR_SUCCESS != fr)
		{
			return	fr;
		}
#ifndef HT_UNSAFE
		fr	= HT_FR_NONE;
#endif
		if (* ui_stop)
		{
			break;
		}

		b_new	= aim->opt_buf_board[0];
		mtmp	= & moves->elems[i];

		board_copy(b_orig, b_new, g->rules->opt_blen);

		fr = board_move_unsafe(g, b_new, aim->opt_movehist,
			mtmp->pos, mtmp->dest
#ifdef HT_AI_MINIMAX_HASH
			, aim->tp_tab , & hashkey, & hashlock
#endif
			);
		if (HT_FR_SUCCESS != fr)
		{
			return	fr;
		}
#ifndef HT_UNSAFE
		fr	= HT_FR_NONE;
#endif

#ifndef HT_AI_MINIMAX_UNSAFE
		fr	= HT_FR_SUCCESS;
#endif
		value = aiminimax_min(g, b_new, aim, alpha, beta,
			(unsigned short)1, depth_cur
#ifdef HT_AI_MINIMAX_HASH
			, hashkey, hashlock
#endif
#ifndef HT_AI_MINIMAX_UNSAFE
			, & fr
#endif
			);
#ifndef HT_AI_MINIMAX_UNSAFE
		if (HT_FR_SUCCESS != fr)
		{
			return	fr;
		}
#ifndef HT_UNSAFE
		fr	= HT_FR_NONE;
#endif
#else /* HT_AI_MINIMAX_UNSAFE */
		if (HT_FR_SUCCESS != opt_fr)
		{
			return	opt_fr;
		}
#endif /* HT_AI_MINIMAX_UNSAFE */

		/*
		 * A move was added by board_move_unsafe(). Remove it:
		 */
		aim->opt_movehist->elemc--;

		if (value > alpha)
		{
			/*
			 * New best move found.
			 */
			fr	= listm_swaptofr(moves, (size_t)i);
			if (HT_FR_SUCCESS != fr)
			{
				return	fr;
			}
#ifndef HT_UNSAFE
			fr	= HT_FR_NONE;
#endif
			alpha	= value;
		}

		if (alpha >= beta)
		{
			/*
			 * Winning move found. Stop searching.
			 */
			* win_found	= HT_TRUE;
			break;
		}

		if (value <= HT_AIM_VALUE_INFMIN)
		{
			if (moves->elemc < (size_t)2)
			{
				/*
				 * The computer wants to remove all
				 * moves. Therefore use the previously
				 * cached move.
				 */
				moves->elems[0].pos	= mcache.pos;
				moves->elems[0].dest	= mcache.dest;
				break;
			}
			fr	= listm_remove(moves, (size_t)i);
			if (HT_FR_SUCCESS != fr)
			{
				return	fr;
			}
#ifndef HT_UNSAFE
			fr	= HT_FR_NONE;
#endif

			/*
			 * listm_remove changes elemc, and i now points
			 * at the next move in the list.
			 *
			 * Note that we can not break here if
			 * moves->elemc <= 1, because then we won't know
			 * if the only remaining move is worse than the
			 * previously best move (mcache). We have to
			 * "unnecessarily" check the last move too.
			 */
			i--;
		}

		/*
		 * i may be < 0 if we removed a move by listm_remove.
		 */
		fr = ui_ai_progress(interf, (int)(i < 0 ? 0 : i)
			* HT_AIPROG_MAX / (int)moves->elemc);
		if (HT_FR_SUCCESS != fr)
		{
			return	fr;
		}
#ifndef HT_UNSAFE
		fr	= HT_FR_NONE;
#endif
	}

	fr	= ui_ai_progress(interf, HT_AIPROG_DONE);
	if (HT_FR_SUCCESS != fr)
	{
		return	fr;
	}
#ifndef HT_UNSAFE
	fr	= HT_FR_NONE;
#endif

	return HT_FR_SUCCESS;
}

/*
 * Performs a minimax search with alpha-beta pruning and iterative
 * deepening.
 *
 * This function can set act->type to:
 *
 * *	HT_UI_ACT_MOVE (act->info.mov has details), if a move was found.
 *
 * *	HT_UI_ACT_UNK, if the algorithm failed to find a move. This
 *	should never happen, so do unplug the computer player if it
 *	does.
 *
 * Returns successfully and sets fail to false if act is set to an
 * action that the AI believes is valid (though you can't assume that it
 * is valid).
 *
 * Sets fail to true if the AI could not find an action, but no other
 * error occured. In this case it's appropriate to "unplug" the AI by
 * setting the player to a human controller instead, since this should
 * never happen.
 *
 * Returns !HT_FR_SUCCESS is fatal for the program.
 */
enum HT_FR
aiminimax_command (
	const struct game * const HT_RSTR	g,
	struct ui * const			interf,
	struct aiminimax * const		aim,
	struct ui_action * const HT_RSTR	act
	)
{
	enum HT_FR		fr		= HT_FR_NONE;
	const struct board *	b_orig		= NULL;
	unsigned short		depth_cur;
	HT_BOOL			win_found, ui_stop;
#ifndef HT_UNSAFE
	if (NULL == g	|| NULL == interf	|| NULL == aim
	|| NULL == act			|| NULL == aim->opt_moves
	|| NULL == aim->opt_buf_moves	|| NULL == aim->opt_buf_board)
	{
		return	HT_FR_FAIL_NULLPTR;
	}
#endif
	/*
	 * Set to *_UNK action type so we know if ui_ai_stop() modified
	 * it when we are about to return.
	 */
	act->type			= HT_UI_ACT_UNK;
	b_orig				= g->b;
	win_found	= ui_stop	= HT_FALSE;

	/*
	 * Move count is aim->opt_moves.elemc.
	 */
	listm_clear(aim->opt_moves);

	fr	= listmh_copy(g->movehist, aim->opt_movehist);
	if (HT_FR_SUCCESS != fr)
	{
		return	fr;
	}
#if !defined(HT_UNSAFE) && !defined(HT_AI_MINIMAX_UNSAFE)
	fr	= HT_FR_NONE;
#endif

	fr	= board_moves_get(g, b_orig, aim->opt_movehist,
		aim->opt_moves);
	if (HT_FR_SUCCESS != fr)
	{
		return	fr;
	}
#ifndef HT_UNSAFE
	fr	= HT_FR_NONE;
#endif

	if (aim->opt_moves->elemc < (size_t)1)
	{
		act->type	= HT_UI_ACT_UNK;
		return		HT_FR_SUCCESS;
	}

#ifdef HT_AI_MINIMAX_HASH
	fr	= zhashtable_clearunused(aim->tp_tab);
	if (HT_FR_SUCCESS != fr)
	{
		return	fr;
	}
#endif

	/*
	 * NOTE:	If aim->tp_tab->cols->elemc >=
	 *		aim->tp_tab->cols->capc, then we have depleted
	 *		all available memory for collisions, and
	 *		increasing HT_AI_MINIMAX_HASH_MEM_COL would
	 *		help.
	 */

	/*
	 * aim->opt_moves->elems[0] will always be the best move.
	 */
	for (depth_cur = (unsigned short)1; depth_cur <= aim->depth_max;
		depth_cur++)
	{
		if (aim->opt_moves->elemc < (size_t)2)
		{
			/*
			 * There's only one move (left). The algorithm
			 * is forced to do it, so stop searching.
			 */
			break;
		}

		fr = aiminimax_search(g, interf, aim, b_orig, depth_cur,
			& win_found, & ui_stop);
		if (HT_FR_SUCCESS != fr)
		{
			return	fr;
		}
#ifndef HT_UNSAFE
		fr	= HT_FR_NONE;
#endif

		if (HT_UI_ACT_UNK != act->type)
		{
			/*
			 * Don't select a move, because some other
			 * action has been selected (by the user, in the
			 * XLib interface, propagated from the
			 * ui_ai_stop() function).
			 */
			return	HT_FR_SUCCESS;
		}
		else if (ui_stop)
		{
			/*
			 * Force a move.
			 */
			break;
		}
/* splint incomplete logic */ /*@i1@*/\
		else if (win_found)
		{
			break;
		}
	}

#ifndef HT_UNSAFE
	if (aim->opt_moves->elemc < (size_t)1)
	{
		return	HT_FR_FAIL_ILL_STATE;
	}
	else
#endif
	if (HT_BOARDPOS_NONE	== aim->opt_moves->elems[0].pos
	|| HT_BOARDPOS_NONE	== aim->opt_moves->elems[0].dest)
	{
		act->type	= HT_UI_ACT_UNK;
		return		HT_FR_SUCCESS;
	}
	else
	{
		act->type		= HT_UI_ACT_MOVE;
		act->info.mov.pos	= aim->opt_moves->elems[0].pos;
		act->info.mov.dest	= aim->opt_moves->elems[0].dest;
		return	HT_FR_SUCCESS;
	}
}

#endif

