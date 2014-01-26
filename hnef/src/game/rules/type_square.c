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
#include "types.h"	/* HT_TYPEC_* */
#include "type_square.h"

unsigned short HT_TSQUARE_CAPT_SIDES_NONE	= (unsigned short)0;

HT_BOOL
type_square_can_capture (
	const struct type_square * const HT_RSTR	ts,
	const HT_BIT_U8					pbit
	)
{
	return ((unsigned int)pbit & (unsigned int)ts->captures)
		== (unsigned int)pbit;
}

/*
 * Initializes to invalid / default values.
 *
 * bit is invalid.
 */
void
type_square_init (
	struct type_square * const HT_RSTR	ts
	)
{
	ts->bit		= HT_BIT_U8_EMPTY;
	ts->captures	= HT_BIT_U8_EMPTY;
	ts->capt_sides	= HT_TSQUARE_CAPT_SIDES_NONE;
	ts->escape	= HT_FALSE;
}

/*
 * Retrieves type_square with matching bit, or an invalid square type if
 * none is found.
 */
struct type_square *
type_square_get (
	struct ruleset * const HT_RSTR	r,
	const HT_BIT_U8			bit
	)
{
	return	& r->type_squares[type_index_get(bit)];
}

/*
 * Sets the indicated type_square's bit to the parameter, indicating
 * that it may be used.
 *
 * *	If the bit is not a valid single bit, then it returns
 *	HT_FR_FAIL_ILL_ARG.
 *
 * *	If any previous square in r->type_squares is not defined (its
 *	bit is HT_BIT_U8_EMPTY, indicating that it has not been set
 *	using this function), it returns HT_FR_FAIL_NULLPTR.
 *
 * *	If the parameter bit has already been defined previously, then
 *	it returns HT_FR_FAIL_ILL_STATE.
 *
 * If the bit was successfully set (and r->type_squarec increases), it
 * returns HT_FR_SUCCESS.
 */
enum HT_FR
type_square_set (
	struct ruleset * const HT_RSTR	r,
	const HT_BIT_U8			bit
	)
{
	unsigned short			index, i;
	struct type_square * HT_RSTR	ts	= NULL;
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
	else if (index > r->type_squarec)
	{
		return	HT_FR_FAIL_NULLPTR;
	}

	for (i = (unsigned short)0; i < index; i++)
	{
		if (HT_BIT_U8_EMPTY == r->type_squares[i].bit)
		{
			/*
			 * Wrong order: some previous piece not defined.
			 */
			return	HT_FR_FAIL_NULLPTR;
		}
	}

	ts	= & r->type_squares[index];
	if (HT_BIT_U8_EMPTY != ts->bit)
	{
		/*
		 * This piece type has already been defined.
		 */
		return	HT_FR_FAIL_ILL_STATE;
	}

	ts->bit	= bit;
	r->type_squarec++;
	return	HT_FR_SUCCESS;
}

