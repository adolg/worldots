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
#include <limits.h>	/* USHRT_MAX */

#include "types.h"	/* HT_TYPEC_* */

/*
 * Max value for type_square/type_piece.capt_sides.
 */
const unsigned short HT_TPIECE_CAPT_SIDES_MAX	= (unsigned short)4;

/*
 * Min value for type_square/type_piece.capt_sides.
 */
const unsigned short HT_TPIECE_CAPT_SIDES_MIN	= (unsigned short)1;

/*
 * Default value for type_piece->capt_sides.
 */
const unsigned short HT_TPIECE_CAPT_SIDES_DEF	= (unsigned short)2;

/*
 * Returns index based on bit.
 *
 * The bit must be a single unsigned 1 byte bit that's not 0, id est
 * 1, 2, 4 ... 128. Else HT_TYPEC_INDEX_INVALID is returned -- and this
 * index does indeed point to a type_piece/square in ruleset: the
 * invalid piece/square type.
 *
 * This could be done by exploiting the fact that 2^index = bit, and
 * thus log(bit) / log(2) = index, but I think this is faster.
 */
unsigned short
type_index_get (
	const HT_BIT_U8	bit
	)
{
	switch (bit)
	{
		case 1:
			return (unsigned short)0;
		case 2:
			return (unsigned short)1;
		case 4:
			return (unsigned short)2;
		case 8:
			return (unsigned short)3;
		case 16:
			return (unsigned short)4;
		case 32:
			return (unsigned short)5;
		case 64:
			return (unsigned short)6;
		case 128:
			return (unsigned short)7;
		default:
			return HT_TYPE_INDEX_INVALID;
	}
}

