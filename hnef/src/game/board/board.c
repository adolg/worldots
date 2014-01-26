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
#include <stdlib.h>	/* malloc, free */
#include <string.h>	/* memset */

#ifdef HT_AI_MINIMAX
#include "types.h"		/* type_index_get */
#endif
#include "board.h"
#include "listm.h"		/* listm_add */
#include "listmh.h"		/* listmh_add */
#include "num.h"		/* min, max */
#include "type_piece.h"		/* type_piece_get */
#include "type_square.h"	/* type_square_get */

#ifdef HT_AI_MINIMAX

/*
 * Copies src into dest.
 *
 * NOTE:	This function is called lots of times by aimalgo.c. It
 *		should be as fast as possible.
 */
void
board_copy (
	const struct board * const HT_RSTR	src,
	struct board * const HT_RSTR		dest,
	const unsigned short			blen
	)
{
	memcpy(dest->pieces, src->pieces, (size_t)blen);
	dest->turn	= src->turn;
}

#endif /* HT_AI_MINIMAX */

/*
 * Advances the turn in b.
 *
 * For efficiency reasons, this ignores game->playerc, and instead looks
 * at HT_PLAYERS_MAX, abusing the fact that they must be equal.
 * Otherwise this function would need a struct game parameter for
 * game->playerc.
 */
static
void
board_turn_adv (
/*@in@*/
/*@notnull@*/
	struct board * const HT_RSTR	b
	)
/*@modifies b->turn@*/
{
	if (++b->turn >= HT_PLAYERS_MAX)
	{
		b->turn	= 0;
	}
}

#ifdef HT_AI_MINIMAX_HASH

/*
 * Flags a piece in hashkey and hashlock as captured.
 */
static
void
board_hash_xor (
/*@in@*/
/*@notnull@*/
	const struct zhashtable * const HT_RSTR	hashtable,
/*@in@*/
/*@notnull@*/
	unsigned int * const HT_RSTR		hashkey,
/*@in@*/
/*@notnull@*/
	unsigned int * const HT_RSTR		hashlock,
	const unsigned short			piece_index,
	const unsigned short			pos
	)
/*@modifies * hashkey, * hashlock@*/
{
	* hashkey ^= hashtable->zobrist_key	[piece_index][pos];
	* hashlock ^= hashtable->zobrist_lock	[piece_index][pos];
}

#endif /* HT_AI_MINIMAX_HASH */

/*
 * Checks if pt_adj (at b->pieces[adj]) is captured by an enemy piece
 * (whose type_piece is irrelevant) moving to dest. Returns HT_TRUE if
 * so.
 *
 * This function assumes:
 *
 * *	pb_mov (the moving piece, who moved to dest) is a non-empty
 *	piece on the board, that can (potentially) capture pt_adj.
 *
 * *	pb_adj is a non-empty piece that can be captured by pb_mov.
 *
 * If that does not hold true, then this function may not be called.
 *
 * skipx and skipy are used to determine double-trapping. If you are not
 * checking a double trap, both should be 0. If you are checking a
 * double trap, then skipx should be -1 if the piece adjacent to pt_adj
 * (which may be surrounded by the double trap) is to the left of
 * pt_adj; +1 if it's to the right, and so on. This simply tells that
 * this function to hop one step further when checking in that
 * direction.
 *
 *	. . m .	// When checking if x is captured, you can give -1 skipx
 *	n y x m	// to move one more step in the x-axis to the left when
 *	. . m . // determining if x is surrounded (thus checking if x is
 *		// surrounded by m and n).
 *
 * skipx and skipy will maintain custodial capture rules, even when
 * hopping over several pieces.
 *
 * If either skipx or skipy is not 0, then this function may adapt
 * capt_sides and custodial (of pt_adj) to double trap rules. If both
 * are 0, this is not done.
 *
 * When using double capture rules, the following always happens (other
 * than maybe using other capt_sides and custodial rules):
 *
 * *	Lax custodial capture requirement is used. Normally the moving
 *	piece has to be the piece that surrounds custodially, but in
 *	this case it's enough that the piece has been surrounded
 *	custodially by any two pieces (not necessarily the moving one).
 *	The normal checking can be called "strict" custodial checking.
 *
 * (skipx and skipy can simply be negative, positive or 0. It doesn't
 * distinguish between -100 and -1 -- they are both treated as -1. Thus
 * you can only "hop" over one piece using skipx / skipy.)
 *
 * If a non-empty piece or square type that is on the board is not
 * defined in ruleset, then it always returns HT_FALSE, without giving
 * an indication of failure.
 */
static
HT_BOOL
board_is_capt (
/*@in@*/
/*@notnull@*/
	const struct game * const HT_RSTR	g,
/*@in@*/
/*@notnull@*/
	const struct board * const HT_RSTR	b,
	const unsigned short			dest,
/*@in@*/
/*@notnull@*/
	const struct type_piece * const HT_RSTR	pt_adj,
	const unsigned short			adj,
	const int				skipx,
	const int				skipy
	)
/*@modifies nothing@*/
{
	/*
	 * Assumes that g, b and pt_adj are not NULL, and that adj and
	 * dest are within bounds.
	 */
	HT_BIT_U8	pb_adj;
	unsigned short	adjx,	adjy,	offbx,	offby,
			capt_sides,	surrxaxis,	surryaxis;
	int		i;
	HT_BOOL		custodial, cust_strict, capt_edge, dtrap;

	dtrap	= skipx != 0 || skipy != 0;

	if (dtrap)
	{
		/*
		 * Use double trap rules (and lax custodial checking).
		 */
		capt_edge	= pt_adj->dbl_trap_edge;
		capt_sides	= (unsigned short)(pt_adj->dbl_trap_encl
			? (unsigned short)4 : pt_adj->capt_sides);
		custodial	= pt_adj->dbl_trap_encl
				? HT_FALSE : pt_adj->custodial;
		cust_strict	= HT_FALSE;
	}
	else
	{
		/*
		 * Do not use double trap rules (and strict custodial
		 * checking).
		 */
		capt_edge	= pt_adj->capt_edge;
		capt_sides	= pt_adj->capt_sides;
		custodial	= pt_adj->custodial;
		cust_strict	= HT_TRUE;
	}

	{
		/*
		 * Don't override if double trapping and dbl_trap_encl
		 * makes the piece have to be surrounded.
		 *
		 * Note that if the square is among opt_ts_capt_sides,
		 * then the square type does override capt_sides and
		 * indicates a valid amount of sides.
		 */
		const HT_BIT_U8	sb_adj	= g->rules->squares[adj];
		if (((unsigned int)sb_adj &
			(unsigned int)g->rules->opt_ts_capt_sides)
			== (unsigned int)sb_adj
		&& !(dtrap && pt_adj->dbl_trap_encl))
		{
			capt_sides = type_square_get(g->rules, sb_adj)
					->capt_sides;
			if (capt_sides >= (unsigned short)4
			|| capt_sides < (unsigned short)2)
			{
				custodial	= HT_FALSE;
			}
		}
	}
	pb_adj		= b->pieces[adj];
	adjx		= adj % g->rules->bwidth;
	adjy		= adj / g->rules->bwidth;

	/*
	 * Note that custodial is not dropped by setting custodial to
	 * false if it's not possible due do a custodial capture due to
	 * off-board squares. This particular situation is checked
	 * later, without altering the value of custodial.
	 *
	 * If a square overrides capt_sides to 1 or 4, however, then
	 * custodial is set to false.
	 */

	/*
	 * Amount of squares adjacent to adjx, adjy that are off-board.
	 * HT_BIT_U8_EMPTY are added to these counts later.
	 */
	offbx	= offby	= 0;
	if (adjx < (unsigned short)1
	|| adjx >= g->rules->bwidth - 1)
	{
		offbx++;
	}
	if (adjy < (unsigned short)1
	|| adjy >= g->rules->bheight - 1)
	{
		offby++;
	}

	/*
	 * Amount of pieces that have surrounded pt_adj in x and
	 * y-axises. Used to determine custodial capture.
	 */
	surrxaxis	= surryaxis	= 0;

	/*
	 * Check every piece adjacent to pt_adj (on adjx, adjy).
	 */
	for (i = 0; i < 4; i++)
	{
		const HT_BOOL	axisx	= i < 2;
		unsigned short	adjadjx	= adjx;
		unsigned short	adjadjy	= adjy;
		unsigned short	adjadj;
		unsigned short	empty_adj;
		unsigned short	surr_tot;
		HT_BIT_U8	pb_adjadj, sb_adjadj;
		switch (i)
		{
			case 0:
				adjadjx = (unsigned short)
					(adjadjx + (skipx > 0 ? 2 : 1));
/* splint break */ /*@i1@*/	break;
			case 1:
				adjadjx = (unsigned short)
					(adjadjx - (skipx < 0 ? 2 : 1));
/* splint break */ /*@i1@*/	break;
			case 2:
				adjadjy = (unsigned short)
					(adjadjy + (skipy > 0 ? 2 : 1));
/* splint break */ /*@i1@*/	break;
			default:
				adjadjy = (unsigned short)
					(adjadjy - (skipy < 0 ? 2 : 1));
/* splint break */ /*@i1@*/	break;
		}

		/*
		 * Note that adjadjx and adjadjy will roll over instead
		 * of becoming negative, thus exceeding g->rules->bwidth
		 * or bheight.
		 */
		if (adjadjx >= g->rules->bwidth
		|| adjadjy >= g->rules->bheight)
		{
			continue;
		}

		adjadj		= (unsigned short)
			(adjadjy * g->rules->bwidth + adjadjx);
		sb_adjadj	= g->rules->squares	[adjadj];
		pb_adjadj	= b->pieces		[adjadj];
		if (HT_BIT_U8_EMPTY == sb_adjadj)
		{
			switch (axisx)
			{
				case HT_TRUE:
					offbx++;
/* splint break */ /*@i1@*/		break;
				case HT_FALSE:
				default:
					offby++;
/* splint break */ /*@i1@*/		break;
			}
		}
		else if (dest == adjadj)
		{
			switch (axisx)
			{
				case HT_TRUE:
					surrxaxis++;
/* splint break */ /*@i1@*/		break;
				case HT_FALSE:
				default:
					surryaxis++;
/* splint break */ /*@i1@*/		break;
			}
		}
		else if (HT_BIT_U8_EMPTY != pb_adjadj)
		{
			/*
			 * Assumes pt_adjadj is a defined piece.
			 */
			const struct type_piece * const HT_RSTR
				pt_adjadj =
				type_piece_get(g->rules, pb_adjadj);
#if !defined(HT_UNSAFE) && !defined(HT_AI_MINIMAX_UNSAFE)
			if (HT_BIT_U8_EMPTY == pt_adjadj->bit)
			{
				return	HT_FALSE;
			}
#endif
			if (type_piece_can_anvil(pt_adjadj,
				pt_adj->bit))
			{
				switch (axisx)
				{
					case HT_TRUE:
						surrxaxis++;
/* splint break */ /*@i1@*/			break;
					case HT_FALSE:
					default:
						surryaxis++;
/* splint break */ /*@i1@*/			break;
				}
			}
		}
		else	/* Empty on-board square. */
		{
			const struct type_square * const HT_RSTR
				st_adjadj =
				type_square_get(g->rules, sb_adjadj);
#if !defined(HT_UNSAFE) && !defined(HT_AI_MINIMAX_UNSAFE)
			if (HT_BIT_U8_EMPTY == st_adjadj->bit)
			{
				return	HT_FALSE;
			}
#endif
			if (type_square_can_capture(st_adjadj, pb_adj))
			{
				switch (axisx)
				{
					case HT_TRUE:
						surrxaxis++;
/* splint break */ /*@i1@*/			break;
					case HT_FALSE:
					default:
						surryaxis++;
/* splint break */ /*@i1@*/			break;
				}
			}
		}

		empty_adj = (unsigned short)(4 - (offbx + offby));
		if (capt_edge)	/* Relax capt_sides? */
		{
			if (capt_sides > empty_adj)
			{
				capt_sides = empty_adj;
			}
		}

		/*
		 * Note that custodial requirement is dropped if
		 * capt_edge, and [the piece is completely surrounded on
		 * all on-board adjacent squares, or custodial capture
		 * is impossible due to off-board squares].
		 */
		surr_tot = (unsigned short)(surrxaxis + surryaxis);
		if (surr_tot >= capt_sides)
		{
			if (custodial
			&& !(capt_edge && surr_tot >= empty_adj)
			&& !(capt_edge && offbx > 0 && offby > 0))
			{
				/*
				 * Custodial capture applies. Is dest on
				 * the same row as adj (we already know
				 * that they are orthogonally adjacent)?
				 * If so, and if the piece is surrounded
				 * by 2 pieces along the row, then it's
				 * captured custodially.
				 */
				if (cust_strict)
				{
					const HT_BOOL xaxisdest =
					dest	% g->rules->bwidth
					!= adj % g->rules->bwidth;
					if ((surrxaxis >
						(unsigned short)1
						&& xaxisdest)
					|| (surryaxis >
						(unsigned short)1
						&& !xaxisdest))
					{
						return	HT_TRUE;
					}
				}
/* splint incomplete logic */ /*@i1@*/\
				else if (surrxaxis > (unsigned short)1
				||	surryaxis > (unsigned short)1)
				{
					return	HT_TRUE;
				}
				/*
				 * Else: not captured custodially by
				 * strict or lax checking.
				 */
			}
			else
			{
				return	HT_TRUE;
			}
		}
	}
	return	HT_FALSE;
}

/*
 * Checks if pt_adj (at b->pieces[adj]) is captured by an enemy piece
 * (whose type_piece is irrelevant) moving to dest, according to double
 * trap capturing rules. Returns HT_TRUE if so.
 *
 * This function assumes:
 *
 * *	pb_mov (the moving piece, who moved to dest) is a non-empty
 *	piece on the board, that can (potentially) capture pt_adj.
 *
 * *	pb_adj is a non-empty piece that can be captured by pb_mov.
 *
 * If that does not hold true, then this function may not be called.
 *
 * The following is checked by this function, and does not have to be
 * checked in advance:
 *
 * *	If pt_adj is not double-trappable, then this funciton returns
 *	false immediately.
 *
 * Further, two pieces are not even considered for double trap if any of
 * the following conditions is true:
 *
 * *	The pieces are owned by different players.
 *
 * *	Either piece can not trigger double trap.
 *
 * *	None of the pieces are captured by a double trap.
 *
 * *	Both pieces are double trap complements (can't be trapped on
 *	their own).
 *
 * This function calls board_is_capt() with double trap parameters. Thus
 * lax custodial checking will be used (according to non-0 skipx/skipy
 * parameters) and double trap settings may override capt_sides and
 * custodial.
 *
 * Returns HT_TRUE if adj is captured in double trap; false if not.
 */
static
HT_BOOL
board_do_dtrap (
/*@in@*/
/*@notnull@*/
	const struct game * const HT_RSTR	g,
/*@in@*/
/*@notnull@*/
	struct board * const			b,
	const unsigned short			dest,
/*@in@*/
/*@notnull@*/
	const struct type_piece * const HT_RSTR	pt_adj,
	const unsigned short			adj
#ifdef HT_AI_MINIMAX_HASH
	,
/*@in@*/
/*@null@*/
	const struct zhashtable * const HT_RSTR	hashtable,
/*@in@*/
/*@null@*/
	unsigned int * const			hashkey,
/*@in@*/
/*@null@*/
	unsigned int * const			hashlock
#endif
	)
/*@modifies * b@*/
#ifdef HT_AI_MINIMAX_HASH
/*@modifies * hashkey, * hashlock@*/
#endif
{
	/*
	 * Assumes that dest and adj are within bounds (this is
	 * guaranteed by board_do_capture right before calling this
	 * function), and that g, b, and pt_mov are not NULL.
	 *
	 * pt_adj is the piece adjacent to the moving, hostile piece.
	 *
	 * pt_adjadj is the piece adjacent to pt_adj, which may be
	 * double-trapped with pt_adj.
	 */

	HT_BIT_U8	pb_adj,	padj_opt_owned;
	unsigned short	adjx,	adjy;
	int		i;

	if (!pt_adj->dbl_trap)
	{
		return	HT_FALSE;
	}

	pb_adj		= b->pieces[adj];
	padj_opt_owned	= g->players[pt_adj->owner]->opt_owned;
	adjx		= adj % g->rules->bwidth;
	adjy		= adj / g->rules->bwidth;
	for (i = 0; i < 4; i++)
	{
		unsigned short	adjadjx	= adjx;
		unsigned short	adjadjy	= adjy;
		unsigned short	adjadj;
		HT_BIT_U8	pb_adjadj, sb_adjadj;
		int		skipx	= 0;
		int		skipy	= 0;
		switch (i)
		{
			case 0:
				skipx	++;
/* splint break */ /*@i1@*/	break;
			case 1:
				skipx	--;
/* splint break */ /*@i1@*/	break;
			case 2:
				skipy	++;
/* splint break */ /*@i1@*/	break;
			default:
				skipy	--;
/* splint break */ /*@i1@*/	break;
		}
		adjadjx	= (unsigned short)(adjadjx + skipx);
		adjadjy	= (unsigned short)(adjadjy + skipy);

		/*
		 * Note that adjadjx and adjadjy will roll over instead
		 * of becoming negative, thus exceeding g->rules->bwidth
		 * or bheight.
		 */
		if (adjadjx >= g->rules->bwidth
		|| adjadjy >= g->rules->bheight)
		{
			continue;
		}

		adjadj		= (unsigned short)
			(adjadjy * g->rules->bwidth + adjadjx);
		sb_adjadj	= g->rules->squares	[adjadj];
		pb_adjadj	= b->pieces		[adjadj];
		if (HT_BIT_U8_EMPTY == sb_adjadj)
		{
			continue;
		}
/* splint incomplete logic */ /*@i1@*/\
		else if (HT_BIT_U8_EMPTY == pb_adjadj
/*
 * pb_adj and pb_adjadj are owned by different players:
 */
		|| ((unsigned int)padj_opt_owned & (unsigned int)
			pb_adjadj) != (unsigned int)pb_adjadj
/*
 * pb_adjadj is not dbl_trap (we already know that pb_adj is):
 */
		|| ((unsigned int)g->rules->opt_tp_dbl_trap &
			(unsigned int)pb_adjadj) !=
			(unsigned int)pb_adjadj
/*
 * Neither piece is dbl_trap_capt:
 */
		|| (((unsigned int)g->rules->opt_tp_dbl_trap_capt &
			(unsigned int)pb_adj) != (unsigned int)pb_adj
			&& ((unsigned int)
			g->rules->opt_tp_dbl_trap_capt &
			(unsigned int)pb_adjadj) !=
			(unsigned int)pb_adjadj)
/*
 * Both pieces are dbl_trap_compl:
 */
		|| (((unsigned int)g->rules->opt_tp_dbl_trap_compl &
			(unsigned int)pb_adj) == (unsigned int)pb_adj
			&& ((unsigned int)
			g->rules->opt_tp_dbl_trap_compl &
			(unsigned int)pb_adjadj) ==
			(unsigned int)pb_adjadj))
		{
			continue;
		}

		{
			const struct type_piece * const HT_RSTR
				pt_adjadj =
				type_piece_get(g->rules, pb_adjadj);

			if (board_is_capt(g, b, dest, pt_adj, adj,
				skipx, skipy)
			&& board_is_capt(g, b, dest,
				pt_adjadj, adjadj, -skipx, -skipy))
			{
#ifdef HT_AI_MINIMAX_HASH
				if (NULL != hashkey
#if !defined(HT_UNSAFE) && !defined(HT_AI_MINIMAX_UNSAFE)
				&& NULL != hashlock && NULL != hashtable
#endif
				)
				{
					if (pt_adj->dbl_trap_capt)
					{
						board_hash_xor(
						hashtable, hashkey,
						hashlock, type_index_get
						(pb_adj), adj);
					}
					if (pt_adjadj->dbl_trap_capt)
					{
						board_hash_xor(
						hashtable, hashkey,
						hashlock, type_index_get
						(pb_adjadj), adjadj);
					}
				}
#endif /* HT_AI_MINIMAX */
				if (pt_adj->dbl_trap_capt)
				{
					b->pieces[adj] =
						HT_BIT_U8_EMPTY;
				}
				if (pt_adjadj->dbl_trap_capt)
				{
					b->pieces[adjadj] =
						HT_BIT_U8_EMPTY;
				}
				return	HT_TRUE;
			}
		}
	}
	return	HT_FALSE;
}

/*
 * Checks if the piece at adj is captured (either normally or by double
 * trap) by pt_mov moving to dest, and if so, removes it from the board.
 *
 * Returns true if any piece was captured.
 *
 * hashtable may be NULL if hashkey also is NULL, as in
 * board_do_captures.
 *
 * Returns false if any parameter is invalid, with no other indication
 * of failure.
 *
 * NOTE:	Checking for double trap takes precedence over checking
 *		for ordinary captures. If a double trap triggers (and
 *		pt_adj is not captured, even though pt_adj triggers the
 *		double trap), then it doesn't even check if pt_adj is
 *		captured according to normal capturing rules.
 *
 *		It's up to the ruleset writer to make sure that this
 *		doesn't cause strange behavior in the game.
 */
static
HT_BOOL
board_do_capture (
/*@in@*/
/*@notnull@*/
	const struct game * const HT_RSTR	g,
/*@in@*/
/*@notnull@*/
	struct board * const			b,
/*@in@*/
/*@notnull@*/
	const struct type_piece * const HT_RSTR	pt_mov,
	const unsigned short			dest,
	const unsigned short			adj
#ifdef HT_AI_MINIMAX_HASH
	,
/*@in@*/
/*@null@*/
	const struct zhashtable * const HT_RSTR	hashtable,
/*@in@*/
/*@null@*/
	unsigned int * const			hashkey,
/*@in@*/
/*@null@*/
	unsigned int * const			hashlock
#endif
	)
/*@modifies * b@*/
#ifdef HT_AI_MINIMAX_HASH
/*@modifies * hashkey, * hashlock@*/
#endif
{
	HT_BIT_U8				pb_adj;
	const struct type_piece * HT_RSTR	pt_adj	= NULL;

	/*
	 * Assumes that g, b and pt_mov are not NULL.
	 */
#if !defined(HT_UNSAFE) && !defined(HT_AI_MINIMAX_UNSAFE)
	{
		if (	dest	>= g->rules->opt_blen
		||	adj	>= g->rules->opt_blen)
		{
			/*
			 * Illegal argument(s).
			 */
			return	HT_FALSE;
		}
	}
#endif

	pb_adj	= b->pieces[adj];
	if (HT_BIT_U8_EMPTY == pb_adj
	|| !type_piece_can_hammer(pt_mov, pb_adj))
	{
		return	HT_FALSE;
	}

	pt_adj	= type_piece_get(g->rules, pb_adj);
#if !defined(HT_UNSAFE) && !defined(HT_AI_MINIMAX_UNSAFE)
	if (HT_BIT_U8_EMPTY == pt_adj->bit)
	{
		/*
		 * Illegal game state: all pieces must be defined.
		 */
		return	HT_FALSE;
	}
#endif

	/*
	 * If pt_mov can't capture pb_adj, then don't check double trap
	 * or ordinary capture.
	 */

	if (board_do_dtrap(g, b, dest, pt_adj, adj
#ifdef HT_AI_MINIMAX_HASH
		, hashtable, hashkey, hashlock
#endif
		))
	{
		return	HT_TRUE;
	}

	if (board_is_capt(g, b, dest, pt_adj, adj, 0, 0))
	{
#ifdef HT_AI_MINIMAX_HASH
		if (NULL != hashkey
#if !defined(HT_UNSAFE) && !defined(HT_AI_MINIMAX_UNSAFE)
		&& NULL != hashlock && NULL != hashtable
#endif
		)
		{
			board_hash_xor(hashtable, hashkey,
				hashlock, type_index_get(pb_adj), adj);
		}
#endif
		b->pieces[adj]	= HT_BIT_U8_EMPTY;
		return		HT_TRUE;
	}
	else
	{
		return		HT_FALSE;
	}
}

/*
 * Checks if a piece that moved to dest results in pieces being
 * captured.
 *
 * Assumes that the moving piece at dest is owned by the player whose
 * turn it currently is according to b->turn. Otherwise this function
 * will fail to capture enemy pieces and instead capture friendly
 * pieces.
 *
 * This is a pretty slow function, but there's nothing that can be done
 * about that since we need to cover all possible cases based on ruleset
 * options.
 *
 * hashtable may be NULL if hashkey is also NULL, like in
 * board_move_unsafe.
 *
 * opt_pt_mov is the moving piece. It's given as a parameter as an
 * optimization because move_unsafe() has to retrieve it anyway, which
 * saves us one lookup.
 *
 * Returns true if some piece was captured.
 *
 * Returns false if some argument or state is invalid, with no other
 * indication of failure.
 */
static
HT_BOOL
board_do_captures (
/*@in@*/
/*@notnull@*/
	const struct game * const HT_RSTR	g,
/*@in@*/
/*@notnull@*/
	struct board * const			b,
/*@in@*/
/*@notnull@*/
	const struct type_piece * const HT_RSTR	opt_pt_mov,
	const unsigned short			dest
#ifdef HT_AI_MINIMAX_HASH
	,
/*@in@*/
/*@null@*/
	const struct zhashtable * const HT_RSTR	hashtable,
/*@in@*/
/*@null@*/
	unsigned int * const			hashkey,
/*@in@*/
/*@null@*/
	unsigned int * const			hashlock
#endif
	)
/*@modifies * b@*/
#ifdef HT_AI_MINIMAX_HASH
/*@modifies * hashkey, * hashlock@*/
#endif
{
	HT_BOOL		captured = HT_FALSE;
	unsigned short	destx, desty;
	int		i;
	/*
	 * Assumes that parameters are not NULL.
	 */

#if !defined(HT_UNSAFE) && !defined(HT_AI_MINIMAX_UNSAFE)
	if (HT_BIT_U8_EMPTY == opt_pt_mov->bit)
	{
		/*
		 * Illegal argument: empty piece indicated as moving.
		 */
		return	HT_FALSE;
	}
#endif

	destx	= dest % g->rules->bwidth;
	desty	= dest / g->rules->bwidth;

	for (i = 0; i < 4; i++)
	{
		unsigned short	adjx	= destx;
		unsigned short	adjy	= desty;
		unsigned short	adj;

		switch (i)
		{
			case 0:
				adjx++;
/* splint break */ /*@i1@*/	break;
			case 1:
				adjx--;
/* splint break */ /*@i1@*/	break;
			case 2:
				adjy++;
/* splint break */ /*@i1@*/	break;
			default:
				adjy--;
/* splint break */ /*@i1@*/	break;
		}

		if (adjx >= g->rules->bwidth
		|| adjy >= g->rules->bheight)
		{
			continue;
		}
		adj = (unsigned short)(adjy * g->rules->bwidth + adjx);

		captured = board_do_capture(g, b, opt_pt_mov, dest, adj
#ifdef HT_AI_MINIMAX_HASH
			, hashtable, hashkey, hashlock
#endif
			) || captured;
	}
	return	captured;
}

/*
 * Checks if pos to dest constitutes repetition, considering the given
 * move history.
 *
 * MOVE	POS		DEST	REP?	TYPE		CHECK
 * 1	A	->	B	No	threat		. -> B
 * 2	X	->	Y	No	defense			. -> Y
 * 3	B	->	C	No	revert-threat	B -> C
 * 4	Y	->	Z	No	revert-defense		Y -> .
 * 5	C	->	B	Yes	repeat-threat	C -> B
 *
 * (Note that MOVE 5 is pos / dest. Also note that we don't need to know
 * if pos / dest is a reversible move, because we know that if it moves
 * from C to B, and previously moved from ? -> B and then B -> C and
 * MOVE 1 and 3 are reversible, then MOVE 5 must also be reversible.)
 *
 * *	If any of the moves 1-4 is irreversible, then it's not
 *	repetition.
 *
 * *	A does not have to (but may) equal C. We already know it is the
 *	same piece.
 *	Thus move 3 is not necessarily strictly a reversion from C to A.
 *
 * *	X does not have to (but may) equal Z. We already know it is the
 *	same piece.
 *	Thus move 4 is not necessarily strictly a reversion from Y to X.
 *
 * *	A, X and Z are completely irrelevant since they don't have
 *	anything to do with the threat-defense exchange; for example, a
 *	piece could move from random square A to threat-square B, then
 *	to random square C (A != C) and back to threat-square B, and it
 *	would still be repetition. X and Z do not matter since the piece
 *	that moves to / from Y is assumed to defend, and the purpose of
 *	the rule is to forbit perpetual threats (you can usually choose
 *	to stop threatening without immediate loss, but that's typically
 *	not the case when defending -- thus the attacker is expected to
 *	alternate).
 *
 * NOTE:	Note that a piece moving from pos to dest, where dest is
 *		an escape square for the piece, causes the game to be
 *		over (won). Then no more moves will be made.
 *		moveh are only added by two functions: board_move_safe()
 *		and board_move_unsafe(). moveh structs are never
 *		retrieved from board_moves_get().
 *		Putting these facts together, it follows that an
 *		escaping move will never be the last move in a listmh
 *		when this function is called. The move to be evaluated
 *		is pos / dest, and if this function returns true, then
 *		it's implied that pos / dest does not constitute an
 *		escaping move.
 *		Therefore (in board_move_(un)safe()) we never check if a
 *		move is escaping when determining moveh->irrev. It will
 *		not affect the behavior of this function in any possible
 *		situation.
 *
 *		Also, if you moved from B to C, and are now moving back
 *		from C (pos) to B (dest), then C can not be a noreturn
 *		square for that piece while B is not -- then the move
 *		from B to C would've been illegal. Therefore we don't
 *		have to check for that in this function -- but we do
 *		need to check for it in board_move_unsafe() (which we
 *		also do).
 *
 * NOTE:	There may be odd situations where this gives false
 *		positives; and there may be odd situations where this
 *		doesn't properly trigger repetition.
 */
static
HT_BOOL
board_is_repeat (
/*@in@*/
/*@notnull@*/
	const struct game * const HT_RSTR	g,
	const unsigned short			pos,
	const unsigned short			dest,
/*@in@*/
/*@notnull@*/
	const struct listmh * const HT_RSTR	movehist
	)
/*@modifies nothing@*/
{
	unsigned short	pos_b,	pos_c,	pos_y;
	size_t		i;
	if (!g->rules->forbid_repeat || movehist->elemc < (size_t)4)
	{
		/*
		 * Repetition not forbidden, or too few moves.
		 */
		return	HT_FALSE;
	}
	pos_b	= pos_c	= pos_y	= HT_BOARDPOS_NONE;

	/*
	 * The last move in the move history that we are considering, is
	 * the move that we are about to make, id est the pos / dest
	 * parameters.
	 */
	pos_b	= dest;
	pos_c	= pos;

	/*
	 * Go from last move and check backwards. i = 1 is last move.
	 */
	for (i = (size_t)1; i <= (size_t)4; i++)
	{
		const struct moveh * const HT_RSTR mh =
			& movehist->elems[movehist->elemc - i];
		if (mh->irrev)
		{
			/*
			 * Game end is irreversible, and the last move
			 * should not trigger repetition if it causes
			 * win or loss (which is impossible because then
			 * the win should have happened several moves
			 * ago, but still).
			 */
			return	HT_FALSE;
		}

		switch (i)
		{
			case 1:
				pos_y	= mh->pos;
/*@i1@*/			break;
			case 2:
				if (mh->pos != pos_b
				|| mh->dest != pos_c)
				{
					return	HT_FALSE;
				}
/*@i1@*/			break;
			case 3:
				if (mh->dest != pos_y)
				{
					return	HT_FALSE;
				}
/*@i1@*/			break;
			/* case 4: */
			default:
				if (mh->dest != pos_b)
				{
					return	HT_FALSE;
				}
/*@i1@*/			break;
		}
	}
	return	HT_TRUE;
}

/*
 * Returns true if the move from pos to dest is legal; else returns
 * false.
 *
 * This function also returns false if a piece on the board was not
 * found as a type_piece in g.
 *
 * Assumes that the piece at pos is the moving piece, and that the
 * player who owns that piece type is the player to move in b (else the
 * move is of course never legal -- though it is valid to call this
 * function in that case).
 *
 * A move can be illegal for the following reasons:
 *
 * *	pos is dest.
 *
 * *	pos or dest is out of board bounds.
 *
 * *	pos to dest indicates a non-orthogonal move.
 *
 * *	The piece on pos (the moving piece) is HT_BIT_U8_EMPTY.
 *
 * *	The moving piece traverses from a no-noreturn square to a
 *	noreturn square.
 *
 * *	The moving piece traverses a non-occupiable (according to
 *	piece_type->occupies) or off-board (HT_BIT_U8_EMPTY) square.
 *
 * *	The moving piece traverses a square that is already occupied by
 *	some piece.
 *
 * *	The player whose turn it is does not own the moving piece.
 *
 * index is whose turn it is. If it's the current player's turn, then
 * give board->turn as index parameter.
 * (This exists so that the computer player can check how many moves
 * both players have, in which case it has to assume that both players
 * can move at any given board position.)
 *
 * This function does not check if the game is over. You are not allowed
 * to call this function if the game is over.
 *
 * NOTE:	This function assumes that pieces can only move
 *		orthogonally. If you add functionality for pieces that
 *		can move diagonally or in any other way, then this
 *		function will not work any more.
 */
static
HT_BOOL
board_move_legal (
/*@in@*/
/*@notnull@*/
	const struct game * const HT_RSTR	g,
/*@in@*/
/*@notnull@*/
	const struct board * const HT_RSTR	b,
/*@in@*/
/*@notnull@*/
	const struct listmh * const HT_RSTR	movehist,
	const unsigned short			index,
	const unsigned short			pos,
	const unsigned short			dest,
/*@in@*/
/*@notnull@*/
	const struct type_piece * const HT_RSTR	opt_pt_mov,
	HT_BOOL * const HT_RSTR			impeded
	)

{
	unsigned short			posx, posy, destx, desty;
	HT_BOOL				axisx;
	/*
	 * Assume the variables are not NULL since this is called very
	 * often by the computer player.
	 */
	if (pos == dest)
	{
		return	HT_FALSE;
	}

	if (pos >= g->rules->opt_blen || dest >= g->rules->opt_blen)
	{
		return	HT_FALSE;
	}

	/*
	 * opt_pt_mov is an optimization. This is the code used for
	 * retrieving them without using the parameters:
	 *
	 *	pb_mov	= b->pieces[pos];
	 *	if (HT_BIT_U8_EMPTY == pb_mov) { ...fail... }
	 *
	 *	pt_mov	= type_piece_get(g->rules, pb_mov);
	 *	if (NULL == pt_mov) { ...fail... }
	 *
	 */
	if (opt_pt_mov->owner != index)
	{
		return	HT_FALSE;
	}

	posx	= pos	% g->rules->bwidth;
	posy	= pos	/ g->rules->bwidth;
	destx	= dest	% g->rules->bwidth;
	desty	= dest	/ g->rules->bwidth;
	{
		const unsigned short dx =
		(unsigned short)(max(posx, destx) - min(posx, destx));
		const unsigned short dy =
		(unsigned short)(max(posy, desty) - min(posy, desty));
		if (dx > 0 && dy > 0)
		{
			return	HT_FALSE;
		}
		axisx	= dx > 0;
	}

	if (axisx)	/* posy == desty */
	{
		HT_BIT_U8	sbdest, pbdest;
		const HT_BOOL	dir_e	= destx > posx;
		unsigned short	tmpx	= posx;
		unsigned short	tmppos, tmpdest;	/* Start, new */
		tmpdest			= pos;	/* Start position */
		do
		{
			tmpx	= (unsigned short)
				(tmpx + (dir_e ? 1 : -1));

			tmppos	= tmpdest;	/* Old dest. */
			tmpdest	= (unsigned short)
				(posy * g->rules->bwidth + tmpx);
			pbdest	= b->pieces[tmpdest];

			if (HT_BIT_U8_EMPTY != pbdest)
			{
				// not allowed to jump over other pieces, or replace them
				return	HT_FALSE;
			}
			sbdest	= g->rules->squares[tmpdest];
			// if can traverse, unimpede
			if ((* impeded = !type_piece_can_traverse(opt_pt_mov, sbdest)) || tmpx == destx)
			{
				// our piece either: has arrived at its destination,
				// or wasn't allowed to "traverse" - "noreturn" and "occupies" rules apply normally
				if (!type_piece_can_occupy(opt_pt_mov, sbdest) || !type_piece_can_return(opt_pt_mov,
					g->rules->squares[tmppos], sbdest))
				{
					return	HT_FALSE;
				}
			}

		} while (tmpx != destx);
	}
	else		/* posx == destx */
	{
		HT_BIT_U8	sbdest, pbdest;
		const HT_BOOL	dir_s	= desty > posy;
		unsigned short	tmpy	= posy;
		unsigned short	tmppos, tmpdest;	/* Start, new */
		tmpdest			= pos;	/* Start position */
		do
		{
			tmpy	= (unsigned short)
				(tmpy + (dir_s ? 1 : -1));

			tmppos	= tmpdest;	/* Old dest. */
			tmpdest	= (unsigned short)
				(tmpy * g->rules->bwidth + posx);

			pbdest	= b->pieces[tmpdest];

			if (HT_BIT_U8_EMPTY != pbdest)
			{
				// not allowed to jump over other pieces, or replace them
				return	HT_FALSE;
			}
			sbdest	= g->rules->squares[tmpdest];
			// if can traverse, unimpede
			if ((* impeded = !type_piece_can_traverse(opt_pt_mov, sbdest)) || tmpy == desty)
			{
				// our piece either: has arrived at its destination,
				// or wasn't allowed to "traverse" - "noreturn" and "occupies" rules apply normally
				if (!type_piece_can_occupy(opt_pt_mov, sbdest) || !type_piece_can_return(opt_pt_mov,
					g->rules->squares[tmppos], sbdest))
				{
					return	HT_FALSE;
				}
			}

		} while (tmpy != desty);
	}

	// unimpede, this square is only unreachable because of repetition rules!
	* impeded = HT_FALSE;
	if (board_is_repeat(g, pos, dest, movehist))
	{
		return	HT_FALSE;
	}
	else
	{
		return	HT_TRUE;
	}
}

/*
 * Makes a move on b from pos to dest, ignoring if it's legal.
 *
 * The move is added to movehist.
 *
 * This should only be done after retrieving a list of moves that are
 * known to be legal. This function exists for the minimax computer
 * player to reduce the time it takes to check if every move is legal.
 *
 * This will even ignore if pos and dest are out of bounds coordinates,
 * and so may write past the end of allocated memory if you give it bad
 * coordinates.
 *
 * The hashkey is allowed to be NULL. If so it will be ignored, and
 * hashtable will also be expected to be NULL (but you are allowed to
 * pass a non-NULL hashtable anyway). If the hashkey is non-NULL, then
 * the hashtable must also be a valid non-NULL hashtable.
 */
enum HT_FR
board_move_unsafe (
	const struct game * const HT_RSTR	g,
	struct board * const			b,
	struct listmh * const			movehist,
	const unsigned short			pos,
	const unsigned short			dest
#ifdef HT_AI_MINIMAX_HASH
	,
	const struct zhashtable * const HT_RSTR	hashtable,
	unsigned int * const			hashkey,
	unsigned int * const			hashlock
#endif
	)
{
	const struct type_piece * HT_RSTR	pt_mov	= NULL;
	HT_BOOL					irrev	= HT_FALSE;
#if !defined(HT_UNSAFE) && !defined(HT_AI_MINIMAX_UNSAFE)
	if (NULL == g || NULL == b)
	{
		return	HT_FR_FAIL_NULLPTR;
	}
#endif

	pt_mov	= type_piece_get(g->rules, b->pieces[pos]);
#if !defined(HT_UNSAFE) && !defined(HT_AI_MINIMAX_UNSAFE)
	if (HT_BIT_U8_EMPTY == pt_mov->bit)
	{
		return	HT_FR_FAIL_NULLPTR;
	}
#endif

	/*
	 * Set the hashkey at dest and unset it at pos.
	 *
	 * NOTE:	This assumes that dest was empty before the
	 *		move, id est that pieces can not move to
	 *		non-empty squares.
	 */
#ifdef HT_AI_MINIMAX_HASH
	if (NULL != hashkey
#if !defined(HT_UNSAFE) && !defined(HT_AI_MINIMAX_UNSAFE)
	&& NULL != hashlock && NULL != hashtable
#endif
	)
	{
		const unsigned short pb_ind =
			type_index_get(b->pieces[pos]);
		board_hash_xor(hashtable, hashkey,
			hashlock, pb_ind, dest);
		board_hash_xor(hashtable, hashkey,
			hashlock, pb_ind, pos);
	}
#endif

	b->pieces[dest]	= b->pieces[pos];
	b->pieces[pos]	= HT_BIT_U8_EMPTY;

	/*
	 * If the move is a capture, or moves from a noreturn square to
	 * a !noreturn-square (and therefore can't move back), then the
	 * move is not reversible.
	 */
	irrev	= board_do_captures(g, b, pt_mov, dest
#ifdef HT_AI_MINIMAX_HASH
		, hashtable, hashkey, hashlock
#endif
		)
		|| (((unsigned int)pt_mov->noreturn &
			(unsigned int)g->rules->squares[pos])
			== (unsigned int)pt_mov->noreturn
		&& ((unsigned int)pt_mov->noreturn &
			(unsigned int)g->rules->squares[dest])
			!= (unsigned int)pt_mov->noreturn);

	board_turn_adv(b);

	return	listmh_add(movehist, pos, dest, irrev);
}

/*
 * Makes a move on b from pos to dest, if it's legal. If so, sets legal
 * to true. Else sets legal to false and doesn't make the move.
 *
 * NOTE:	This function does not set hashkeys.
 */
static
enum HT_FR
board_move_safe (
/*@in@*/
/*@notnull@*/
	const struct game * const HT_RSTR	g,
/*@in@*/
/*@notnull@*/
	struct board * const			b,
/*@in@*/
/*@notnull@*/
	struct listmh * const			movehist,
	const unsigned short			pos,
	const unsigned short			dest,
/*@out@*/
/*@notnull@*/
	HT_BOOL * const HT_RSTR			legal
	)
/*@modifies * b, * movehist, * legal@*/
{
	HT_BIT_U8				pb_mov;
	const struct type_piece * HT_RSTR	pt_mov	= NULL;
#ifndef HT_UNSAFE
	if (NULL == g || NULL == b || NULL == legal)
	{
		if (NULL != legal)
		{
			* legal	= HT_FALSE;
		}
/* splint out storage not defined: legal */ /*@i1@*/\
		return	HT_FR_FAIL_NULLPTR;
	}
#endif

	pb_mov	= b->pieces[pos];
	if (HT_BIT_U8_EMPTY == pb_mov)
	{
		* legal	= HT_FALSE;
		return	HT_FR_SUCCESS;
	}
	pt_mov	= type_piece_get(g->rules, pb_mov);

#ifndef HT_UNSAFE
	if (HT_BIT_U8_EMPTY == pt_mov->bit)
	{
		* legal	= HT_FALSE;
		return	HT_FR_FAIL_NULLPTR;
	}
#endif
	HT_BOOL impeded	= HT_FALSE;
	* legal	= board_move_legal(g, b, movehist, b->turn,
		pos, dest, pt_mov, & impeded);
	if (!(* legal))
	{
		return	HT_FR_SUCCESS;
	}

	return	board_move_unsafe(g, b, movehist, pos, dest
#ifdef HT_AI_MINIMAX_HASH
		, NULL, NULL, NULL
#endif
		);
}

/*
 * Delegates board_move_safe with g->b and g->movehist.
 */
enum HT_FR
game_move (
	struct game * const HT_RSTR	g,
	const unsigned short		pos,
	const unsigned short		dest,
	HT_BOOL * const HT_RSTR		legal
	)
{
	return	board_move_safe(g, g->b, g->movehist, pos, dest, legal);
}

#ifdef HT_AI_MINIMAX

/*
 * Adds all moves that are possible for the current player to move in b
 * (according to b->turn) to list.
 *
 * Does not clear moves before adding.
 *
 * NOTE:	This function assumes that pieces can only move
 *		orthogonally, and will not work if you add ability to
 *		move in other directions.
 *		It further assumes that a piece can never move past a
 *		square that it can't occupy.
 *
 * NOTE:	Any changes to this function must be reflected by
 *		board_movec_get.
 */
enum HT_FR
board_moves_get (
	const struct game * const HT_RSTR	g,
	const struct board * const HT_RSTR	b,
	const struct listmh * const HT_RSTR	movehist,
	struct listm * const			list
	)
{
	/*
	 * Algorithm:
	 *
	 * 1.	For every piece on the board owned by the player to
	 *	move:
	 *
	 * 2.	For every square in every direction (north, east, south,
	 *	west) from the piece's position to the edge of the
	 *	board, check if the piece can move to that destination.
	 *	If not, then stop checking in that direction.
	 */
	unsigned short	pos, posx, posy, dest, destx, desty;
	HT_BIT_U8	opt_p_owned, p_pos;
	int		i, modx, mody;
	const struct type_piece * HT_RSTR	opt_pt_mov	= NULL;
#ifndef HT_AI_MINIMAX_UNSAFE
	enum HT_FR				fr	= HT_FR_NONE;
#endif

#if !defined(HT_UNSAFE) && !defined(HT_AI_MINIMAX_UNSAFE)
	if (NULL == g || NULL == b || NULL == list)
	{
		return	HT_FR_FAIL_NULLPTR;
	}
#endif
	opt_p_owned	= g->players[b->turn]->opt_owned;

	for (pos = (unsigned short)0; pos < g->rules->opt_blen; pos++)
	{
		p_pos				= b->pieces[pos];
		if (HT_BIT_U8_EMPTY == p_pos
		|| ((unsigned int)opt_p_owned & (unsigned int)p_pos)
			!= (unsigned int)p_pos)
		{
			continue;
		}
		posx	= (unsigned short)(pos % g->rules->bwidth);
		posy	= (unsigned short)(pos / g->rules->bwidth);

		for (i = 0; i < 4; i++)
		{
			modx	= (unsigned short)0;
			mody	= (unsigned short)0;
			switch (i)
			{
				case 0:
					modx++;
/* splint break */ /*@i1@*/		break;
				case 1:
					modx--;
/* splint break */ /*@i1@*/		break;
				case 2:
					mody++;
/* splint break */ /*@i1@*/		break;
				default:
					mody--;
/* splint break */ /*@i1@*/		break;
			}
			destx	= (unsigned short)(posx + modx);
			desty	= (unsigned short)(posy + mody);
			/*
			 * Check every square from pos to the edge of
			 * the board. Break the loop as soon as we hit a
			 * square that we're not allowed to move to.
			 */
			while (destx	< g->rules->bwidth
			&& desty	< g->rules->bheight)
			{
				dest = (unsigned short)(desty
					* g->rules->bwidth + destx);
				if (NULL == opt_pt_mov
				|| p_pos != opt_pt_mov->bit)
				{
					/*
					 * Lazily retrieve opt_pt_mov if
					 * needed.
					 */
					opt_pt_mov = type_piece_get(
						g->rules, p_pos);
#if !defined(HT_UNSAFE) && !defined(HT_AI_MINIMAX_UNSAFE)
					if (HT_BIT_U8_EMPTY
						== opt_pt_mov->bit)
					{
						return
						HT_FR_FAIL_NULLPTR;
					}
#endif
				}
				HT_BOOL impediment = HT_TRUE;
				if (board_move_legal(g, b, movehist,
					b->turn, pos, dest, opt_pt_mov, & impediment))
				{
#ifdef HT_AI_MINIMAX_UNSAFE
					if (HT_FR_SUCCESS !=
					listm_add(list, pos, dest))
					{
						return HT_FR_FAIL_ALLOC;
					}
#else
					fr = listm_add(list, pos, dest);
					if (HT_FR_SUCCESS != fr)
					{
						return	fr;
					}
#ifndef HT_UNSAFE
					fr	= HT_FR_NONE;
#endif
#endif /* HT_AI_MINIMAX_UNSAFE */
				}
				else /* Stop checking this direction. */
				{
/* splint break nested loop */ /*@innerbreak@*/
					if (impediment)
						break;
					// if we are not impeded by an obstacle, continue checking legal moves...
				}
				destx = (unsigned short)(destx + modx);
				desty = (unsigned short)(desty + mody);
			}
		}
	}
	return	HT_FR_SUCCESS;
}

#endif /* HT_AI_MINIMAX */

/*
 * Works like board_moves_get, but returns the move count (movec).
 *
 * piecemask is the bitmask of pieces to get move count for.
 *
 * Negative return value is a failure.
 *
 * NOTE:	Any changes to this function must be reflected by
 *		board_moves_get.
 */
int
board_movec_get (
	const struct game * const HT_RSTR	g,
	const struct board * const HT_RSTR	b,
	const struct listmh * const HT_RSTR	movehist,
	const HT_BIT_U8				piecemask
	)
{
	unsigned short	pos, posx, posy, dest, destx, desty;
	HT_BIT_U8	p_pos;
	int		i, modx, mody;
	const struct type_piece * HT_RSTR	opt_pt_mov	= NULL;
	int					movec		= 0;

	for (pos = (unsigned short)0; pos < g->rules->opt_blen; pos++)
	{
		p_pos				= b->pieces[pos];
		if (HT_BIT_U8_EMPTY == p_pos
		|| ((unsigned int)piecemask & (unsigned int)p_pos)
			!= (unsigned int)p_pos)
		{
			continue;
		}
		posx	= (unsigned short)(pos % g->rules->bwidth);
		posy	= (unsigned short)(pos / g->rules->bwidth);

		for (i = 0; i < 4; i++)
		{
			modx	= (unsigned short)0;
			mody	= (unsigned short)0;
			switch (i)
			{
				case 0:
					modx++;
/* splint break */ /*@i1@*/		break;
				case 1:
					modx--;
/* splint break */ /*@i1@*/		break;
				case 2:
					mody++;
/* splint break */ /*@i1@*/		break;
				default:
					mody--;
/* splint break */ /*@i1@*/		break;
			}
			destx	= (unsigned short)(posx + modx);
			desty	= (unsigned short)(posy + mody);
			/*
			 * Check every square from pos to the edge of
			 * the board. Break the loop as soon as we hit a
			 * square that we're not allowed to move to.
			 */
			while (destx	< g->rules->bwidth
			&& desty	< g->rules->bheight)
			{
				dest = (unsigned short)(desty
					* g->rules->bwidth + destx);
				if (NULL == opt_pt_mov
				|| p_pos != opt_pt_mov->bit)
				{
					/*
					 * Lazily retrieve opt_pt_mov if
					 * needed.
					 */
					opt_pt_mov = type_piece_get(
						g->rules, p_pos);
#if !defined(HT_UNSAFE) && !defined(HT_AI_MINIMAX_UNSAFE)
					if (HT_BIT_U8_EMPTY
						== opt_pt_mov->bit)
					{
						return -1;
					}
#endif
				}
				HT_BOOL impediment = HT_TRUE;
				if (board_move_legal(g, b, movehist,
					b->turn, pos, dest, opt_pt_mov, & impediment))
				{
					movec++;
				}
				else /* Stop checking this direction. */
				{
/* splint break nested loop */ /*@innerbreak@*/
					if (impediment)
						break;
					// if we are not impeded by an obstacle, continue checking legal moves...

				}
				destx = (unsigned short)(destx + modx);
				desty = (unsigned short)(desty + mody);
			}
		}
	}
	return	movec;
}

/*
 * Checks if the game is over because a piece has escaped.
 *
 * One piece being on an escape square is always enough to trigger
 * victory. If several pieces are on escape squares, then the first one
 * found on the board will trigger victory for the owning player, so
 * this function assumes that there will only be one piece, that can
 * escape, on an escape square.
 */
static
HT_BOOL
board_game_over_escape (
/*@in@*/
/*@notnull@*/
	const struct game * const HT_RSTR	g,
/*@in@*/
/*@notnull@*/
	const struct board * const HT_RSTR	b,
/*@in@*/
/*@notnull@*/
	unsigned short * const HT_RSTR		winner
	)
/*@modifies * winner@*/
{
	unsigned short	i;

	for (i = (unsigned short)0; i < g->rules->opt_blen; i++)
	{
		const HT_BIT_U8 pbit	= b->pieces[i];

		if (HT_BIT_U8_EMPTY != pbit
		&& ((unsigned int)pbit
			& (unsigned int)g->rules->opt_tp_escape)
			== (unsigned int)pbit)
		{
			const HT_BIT_U8 sbit	= g->rules->squares[i];
			if (((unsigned int)sbit
				& (unsigned int)g->rules->opt_ts_escape)
				== (unsigned int)sbit)
			{
				* winner = type_piece_get
					(g->rules, pbit)->owner;
				return	HT_TRUE;
			}
		}
	}

	return	HT_FALSE;
}

/*
 * Checks if the game is over due to a particular type of piece being
 * captured.
 *
 * This requirement all pieces on the board of that type to be captured.
 * If there is one piece on the board left of that type, then the game
 * is not over.
 */
static
HT_BOOL
board_game_over_captloss (
/*@in@*/
/*@notnull@*/
	const struct game * const HT_RSTR	g,
/*@in@*/
/*@notnull@*/
	const struct board * const HT_RSTR	b,
/*@in@*/
/*@notnull@*/
	unsigned short * const HT_RSTR		winner
	)
/*@modifies * winner@*/
{
	unsigned short	i, j;
	HT_BIT_U8	pbit;

	for (i = (unsigned short)0; i < g->rules->type_piecec; i++)
	{
		if (!g->rules->type_pieces[i].capt_loss)
		{
			continue;
		}
		pbit	= g->rules->type_pieces[i].bit;

		for (j = (unsigned short)0; j < g->rules->opt_blen; j++)
		{
			if (b->pieces[j] == pbit)
			{
				goto CONTINUE_MAINLOOP;
			}
		}

		/*
		 * No pieces of type pbit left on the board.
		 */
		* winner = (unsigned short)(0 == b->turn ? 1 : 0);
		return	HT_TRUE;

		/*
		 * GCC requires semicolon after label.
		 */
		CONTINUE_MAINLOOP:;
	}
	return	HT_FALSE;
}

/*
 * Returns true if game is over. If so, winner is set. If not, winner is
 * not set.
 *
 * The game is over if:
 *
 * *	The player to move in b doesn't have any moves left (he loses).
 *
 *	Note that we only check if the player whose move it is has any
 *	moves left. We don't care if the other player has moves left,
 *	since he has no obligation to move during this turn. If the
 *	other player can't move, then the game is not over, unless he
 *	still can't move when it's his turn (this can happen but should
 *	be very rare).
 *
 * *	Any piece has escaped (according to board_game_over_escape).
 *
 * *	All pieces of a single type_piece with capt_loss are captured
 *	(according to board_game_over_captloss).
 *
 * NOTE:	This function assumes that there are only two players.
 *		If one has won (or lost), then the other has lost (or
 *		won).
 */
HT_BOOL
board_game_over (
	const struct game * const HT_RSTR	g,
	const struct board * const HT_RSTR	b,
	const struct listmh * const HT_RSTR	movehist,
	unsigned short * const			winner
	)
{
	/*
	 * NOTE:	-1 moves left indicates a failure, but there's
	 *		nothing we can do to report it. However it's
	 *		guaranteed to not occur because all pieces must
	 *		be defined, and the failure can only happen if
	 *		type_piece_get fails to return a type_piece.
	 */
	if (board_movec_get(g, b, movehist,
		g->players[b->turn]->opt_owned) < 1)
	{
		* winner = (unsigned short)(0 == b->turn ? 1 : 0);
		return	HT_TRUE;
	}

	return	board_game_over_escape	(g, b, winner)
	||	board_game_over_captloss(g, b, winner);
}

/*
 * Delegates board_game_over with g->b and g->movehist.
 */
HT_BOOL
game_over (
	const struct game * const HT_RSTR	g,
	unsigned short * const			winner
	)
{
	return	board_game_over(g, g->b, g->movehist, winner);
}

/*
 * Sets turn to 0 (player 0 always starts) and all pieces to
 * HT_BIT_U8_EMPTY.
 */
void
board_init (
	struct board * const HT_RSTR	b,
	const unsigned short		blen
	)
{
	memset	(b->pieces, (int)HT_BIT_U8_EMPTY, (size_t)blen);
	b->turn	= (unsigned short)0;
}

struct board *
alloc_board (
	const unsigned short blen
	)
{
	struct board * b	= malloc(sizeof(* b));
	if (NULL == b)
	{
		return	NULL;
	}

	b->pieces		= malloc(sizeof(* b->pieces) * blen);
	if (NULL == b->pieces)
	{
		free	(b);
		return	NULL;
	}

	return b;
}

void
free_board (
	struct board * const b
	)
{
	if (NULL != b->pieces)
	{
		free	(b->pieces);
	}
	free	(b);
}

