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
#ifndef HT_TYPE_SQUARE_T_H
#define HT_TYPE_SQUARE_T_H

#include "boardt.h"	/* HT_BIT_U8 */

/*@exposed@*/
struct type_square
{

	/*
	 * Square's single bit.
	 */
	HT_BIT_U8	bit;

	/*
	 * Piece bitmask: pieces that the square is hostile to.
	 *
	 * These can be owned by any player. This settings is also
	 * overridden by the piece's captures if a piece is standing on
	 * the square.
	 */
	HT_BIT_U8	captures;

	/*
	 * Unless HT_TSQUARE_CAPT_SIDES_NONE, this square overrides
	 * type_piece.capt_sides when a piece (any piece, owned by any
	 * player) is standing on it.
	 *
	 * A square type always overrides capt_sides for a piece, unless
	 * double trap requires the piece to be fully surrounded and
	 * we're checking for a double trap.
	 *
	 * type_piece->custodial is automatically dropped if this is 4
	 * sides.
	 */
	unsigned short	capt_sides;

	/*
	 * True if type_pieces with escape can escape to this square to
	 * win the game.
	 */
	HT_BOOL		escape;

};

#endif

