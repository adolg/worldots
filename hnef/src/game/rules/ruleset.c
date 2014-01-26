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
#include <stdlib.h>	/* malloc */

#include "ruleset.h"
#include "type_piece.h"		/* type_piece_init */
#include "type_square.h"	/* type_square_init */
#include "types.h"		/* HT_TYPEC_* */

/*
 * Maximum allowed board size.
 */
/*@unchecked@*/
static
const unsigned short SIZE_BOARD_MAX = (unsigned short)31;

/*
 * Minimum allowed board size.
 */
/*@unchecked@*/
static
const unsigned short SIZE_BOARD_MIN = (unsigned short)1;

/*
 * Returns HT_TRUE if board size (which may be width or height) is
 * allowed; false otherwise.
 */
HT_BOOL
valid_size (
	const unsigned short w_or_h
	)
{
	return w_or_h >= SIZE_BOARD_MIN && w_or_h <= SIZE_BOARD_MAX;
}

/*
 * Initializes optimization variables.
 */
void
ruleset_initopt (
	struct ruleset * const HT_RSTR r
	)
{
	unsigned short i;

	r->opt_blen	= (unsigned short)(r->bwidth * r->bheight);

	r->opt_tp_dbl_trap = r->opt_tp_dbl_trap_capt =
		r->opt_tp_dbl_trap_compl = r->opt_ts_capt_sides =
		HT_BIT_U8_EMPTY;

	for (i = (unsigned short)0; i < r->type_piecec; i++)
	{
		const struct type_piece * const HT_RSTR t =
			& r->type_pieces[i];

		if (t->dbl_trap)
		{
			r->opt_tp_dbl_trap = (HT_BIT_U8)
				((unsigned int)r->opt_tp_dbl_trap
				| (unsigned int)t->bit);
		}
		if (t->dbl_trap_capt)
		{
			r->opt_tp_dbl_trap_capt = (HT_BIT_U8)
				((unsigned int)r->opt_tp_dbl_trap_capt
				| (unsigned int)t->bit);
		}
		if (t->dbl_trap_compl)
		{
			r->opt_tp_dbl_trap_compl = (HT_BIT_U8)
				((unsigned int)r->opt_tp_dbl_trap_compl
				| (unsigned int)t->bit);
		}
		if (t->escape)
		{
			r->opt_tp_escape = (HT_BIT_U8)
				((unsigned int)r->opt_tp_escape
				| (unsigned int)t->bit);
		}
	}

	for (i = (unsigned short)0; i < r->type_squarec; i++)
	{
		const struct type_square * const HT_RSTR t =
			& r->type_squares[i];
		if (HT_TSQUARE_CAPT_SIDES_NONE != t->capt_sides)
		{
			r->opt_ts_capt_sides = (HT_BIT_U8)
				((unsigned int)r->opt_ts_capt_sides
				| (unsigned int)t->bit);
		}
		if (t->escape)
		{
			r->opt_ts_escape = (HT_BIT_U8)
				((unsigned int)r->opt_ts_escape
				| (unsigned int)t->bit);
		}
	}
}

/*
 * Fills the pieces and squares arrays with HT_BIT_U8_EMPTY.
 *
 * Initializes type_pieces and type_squares to invalid, but initialized,
 * values. Every bit is HT_BIT_U8_EMPTY.
 */
void
ruleset_init (
	struct ruleset * const HT_RSTR r
	)
{
	unsigned short i;

	r->forbid_repeat	= HT_FALSE;

	for (i = (unsigned short)0; i < r->opt_blen; i++)
	{
		r->pieces[i] = r->squares[i] = HT_BIT_U8_EMPTY;
	}

	for (i = (unsigned short)0; i <
		(unsigned short)(HT_TYPE_MAX + 1); i++)
	{
		type_piece_init	(& r->type_pieces	[i]);
		type_square_init(& r->type_squares	[i]);
	}

	r->opt_tp_dbl_trap		= HT_BIT_U8_EMPTY;
	r->opt_tp_dbl_trap_capt		= HT_BIT_U8_EMPTY;
	r->opt_tp_dbl_trap_compl	= HT_BIT_U8_EMPTY;
	r->opt_tp_escape		= HT_BIT_U8_EMPTY;
	r->opt_ts_escape		= HT_BIT_U8_EMPTY;
}

/*
 * Returns NULL if failed to allocate, or if bwidth or bheight is
 * invalid.
 */
struct ruleset *
alloc_ruleset (
	const unsigned short	bwidth,
	const unsigned short	bheight
	)
{
	const unsigned short blen = (unsigned short)(bwidth * bheight);
	struct ruleset *	r	= NULL;

	if (!valid_size(bwidth) || !valid_size(bheight))
	{
		return NULL;
	}

	r	= malloc(sizeof(* r));
	if (NULL == r)
	{
		return	NULL;
	}
	r->bwidth	= bwidth;
	r->bheight	= bheight;
	r->opt_blen	= blen;
	r->path		= NULL;

	r->pieces	= malloc(sizeof(* r->pieces) * r->opt_blen);
	if (NULL == r->pieces)
	{
		free	(r);
		return	NULL;
	}
	r->squares	= malloc(sizeof(* r->squares) * r->opt_blen);
	if (NULL == r->squares)
	{
		free	(r->pieces);
		free	(r);
		return NULL;
	}

	r->type_piecec	= 0;
	r->type_squarec	= 0;

	return r;
}

void
free_ruleset (
	struct ruleset * const r
	)
{
	if (NULL != r->squares)
	{
		free	(r->squares);
	}
	if (NULL != r->pieces)
	{
		free	(r->pieces);
	}
	if (NULL != r->path)
	{
		free	(r->path);
	}
	free	(r);
}

