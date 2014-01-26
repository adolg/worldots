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
#include <stdlib.h>	/* NULL */

#include "num.h"	/* single_bit */
#include "playert.h"	/* HT_PLAYER_UNINIT */
#include "types.h"	/* HT_TYPEC_* */
#include "type_piece.h"

/*
 * Checks if tp can anvil pbit.
 */
HT_BOOL
type_piece_can_anvil (
	const struct type_piece * const HT_RSTR	tp,
	const HT_BIT_U8				pbit
	)
{
	return ((unsigned int)pbit & (unsigned int)tp->anvils)
		== (unsigned int)pbit;
}

/*
 * Checks if tp can hammer pbit.
 */
HT_BOOL
type_piece_can_hammer (
	const struct type_piece * const HT_RSTR	tp,
	const HT_BIT_U8				pbit
	)
{
	return ((unsigned int)pbit & (unsigned int)tp->captures)
		== (unsigned int)pbit;
}

/*
 * Checks if type_piece->occupies allows the piece to occupy a square of
 * type sqbit.
 *
 * HT_BIT_U8_EMPTY always returns false.
 */
HT_BOOL
type_piece_can_occupy (
	const struct type_piece * const HT_RSTR	tp,
	const HT_BIT_U8				sqbit
	)
{
	if (HT_BIT_U8_EMPTY != sqbit
	&& ((unsigned int)sqbit & (unsigned int)tp->occupies)
		== (unsigned int)sqbit)
	{
		return	HT_TRUE;
	}
	else
	{
		return	HT_FALSE;
	}
}

/*
 * Checks if type_piece->noreturn allows the piece to move from pos to
 * dest.
 *
 * This is not allowed if you are moving a from a non-noreturn square to
 * a noreturn square.
 *
 * Ignores if pos or dest is HT_BIT_U8_EMPTY.
 */
HT_BOOL
type_piece_can_return (
	const struct type_piece * const HT_RSTR	tp,
	const HT_BIT_U8				pos,
	const HT_BIT_U8				dest
	)
{
	if (((unsigned int)pos & (unsigned int)tp->noreturn)
		!= (unsigned int)pos
	&& ((unsigned int)dest & (unsigned int)tp->noreturn)
		== (unsigned int)dest)
	{
		return	HT_FALSE;
	}
	else
	{
		return	HT_TRUE;
	}
}

/*
 * Checks if type_piece->traverses allows the piece to traverse (pass over) a square of
 * type sqbit.
 *
 * HT_BIT_U8_EMPTY always returns false.
 */
HT_BOOL
type_piece_can_traverse (
	const struct type_piece * const HT_RSTR	tp,
	const HT_BIT_U8				sqbit
	)
{
	if (HT_BIT_U8_EMPTY != sqbit
	&& ((unsigned int)sqbit & (unsigned int)tp->traverses)
		== (unsigned int)sqbit)
	{
		return	HT_TRUE;
	}
	else
	{
		return	HT_FALSE;
	}
}


/*
 * Initializes to invalid / default values.
 *
 * owner and bit are invalid.
 */
void
type_piece_init (
	struct type_piece * const HT_RSTR	tp
	)
{
	tp->bit			= HT_BIT_U8_EMPTY;
	tp->owner		= HT_PLAYER_UNINIT;
	tp->captures		= HT_BIT_U8_EMPTY;
	tp->anvils			= HT_BIT_U8_EMPTY;
	tp->capt_edge		= HT_FALSE;
	tp->capt_loss		= HT_FALSE;
	tp->capt_sides		= HT_TPIECE_CAPT_SIDES_DEF;
	tp->custodial		= HT_FALSE;
	tp->dbl_trap		= HT_FALSE;
	tp->dbl_trap_capt	= HT_FALSE;
	tp->dbl_trap_compl	= HT_FALSE;
	tp->dbl_trap_edge	= HT_FALSE;
	tp->dbl_trap_encl	= HT_FALSE;
	tp->escape		= HT_FALSE;
	tp->noreturn		= HT_BIT_U8_EMPTY;
	tp->occupies		= HT_BIT_U8_EMPTY;
	tp->traverses		= HT_BIT_U8_EMPTY;
}

/*
 * Retrieves type_square with matching bit, or an invalid piece type if
 * none is found.
 */
struct type_piece *
type_piece_get (
	struct ruleset * const HT_RSTR	r,
	const HT_BIT_U8			bit
	)
{
	return	& r->type_pieces[type_index_get(bit)];
}

/*
 * Exactly like type_square_set, but sets a piece type.
 */
enum HT_FR
type_piece_set (
	struct ruleset * const HT_RSTR	r,
	const HT_BIT_U8			bit
	)
{
	unsigned short			index, i;
	struct type_piece * HT_RSTR	ts	= NULL;
#ifndef HT_UNSAFE
	if (!single_bit((unsigned int)bit) && bit > (HT_BIT_U8)128)
	{
		return	HT_FR_FAIL_ILL_ARG;
	}
#endif

	index	= type_index_get(bit);
	if (HT_TYPE_INDEX_INVALID == index)
	{
		return	HT_FR_FAIL_ILL_ARG;
	}
/* splint incomplete logic */ /*@i1@*/\
	else if (index > r->type_piecec)
	{
		return	HT_FR_FAIL_NULLPTR;
	}

	for (i = (unsigned short)0; i < index; i++)
	{
		if (HT_BIT_U8_EMPTY == r->type_pieces[i].bit)
		{
			/*
			 * Wrong order: some previous piece not defined.
			 */
			return	HT_FR_FAIL_NULLPTR;
		}
	}

	ts	= & r->type_pieces[index];
	if (HT_BIT_U8_EMPTY != ts->bit)
	{
		/*
		 * This piece type has already been defined.
		 */
		return	HT_FR_FAIL_ILL_STATE;
	}

	ts->bit	= bit;
	r->type_piecec++;
	return	HT_FR_SUCCESS;
}

