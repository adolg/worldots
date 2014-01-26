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
#ifndef HT_BOARD_T_H
#define HT_BOARD_T_H

/*
 * 8 bit integer data types for single bits or bitmasks.
 *
 * A bit is a piece or square. For every single HT_BIT_U8 there is a
 * corresponding piece or square type. Thus there can be at most eight
 * piece or square types in any ruleset.
 *
 * This should be enforced in case unsigned char is greater than 1 byte.
 */
typedef unsigned char HT_BIT_U8;

/*
 * Value of a nonexistent bit. This can also refer to "no bits", id est
 * decimal 0 (or binary 00000000).
 */
/*@unchecked@*/
extern
const HT_BIT_U8 HT_BIT_U8_EMPTY;

/*@exposed@*/
struct board
{

	/*
	 * Array of HT_BIT_U8. This works like the HT_BIT_U8 array in
	 * ruleset and, in fact, is just a copy of it initially (before
	 * the game begins).
	 */
/*@notnull@*/
/*@owned@*/
	HT_BIT_U8 *	pieces;

	/*
	 * Current player's index in game->players array.
	 */
	unsigned short	turn;

};

#endif

