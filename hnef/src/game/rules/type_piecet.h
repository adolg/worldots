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
#ifndef HT_TYPE_PIECE_T_H
#define HT_TYPE_PIECE_T_H

#include "boardt.h"	/* HT_BIT_U8 */
#include "boolt.h"	/* HT_BOOL */

/*@exposed@*/
struct type_piece
{

	/*
	 * Piece's single bit.
	 */
	HT_BIT_U8	bit;

	/*
	 * Owning player's index.
	 */
	unsigned short	owner;

	/*
	 * Piece bitmask: pieces that this piece is hostile to (can "hammer" in a capture).
	 *
	 * No piece in this bitmask may belong to owner.
	 */
	HT_BIT_U8	captures;

	/*
	 * Piece bitmask: pieces that this piece can "anvil" in a capture.
	 *
	 * No piece in this bitmask may belong to owner.
	 */
	HT_BIT_U8	anvils;

	/*
	 * True if capturing rules should be relaxed along board edges
	 * to make capturing possible, even though it shouldn't be
	 * because there are not enough on-board squares adjacent to the
	 * piece.
	 *
	 * Requires capt_sides >= 2.
	 */
	HT_BOOL		capt_edge;

	/*
	 * True if the game is lost for the owning player if he loses
	 * all pieces of this type.
	 */
	HT_BOOL		capt_loss;

	/*
	 * Amount of sides that the piece has to be surrounded on to be
	 * captured.
	 */
	unsigned short	capt_sides;

	/*
	 * True if the piece has to be captured custodially. This can be
	 * relaxed if capt_edge is true.
	 *
	 * Requires capt_sides >= 2.
	 */
	HT_BOOL		custodial;

	/*
	 * Whether this piece can trigger double trap.
	 *
	 * For this piece to be captured by a double trap, dbl_trap_capt
	 * also has to be true.
	 */
	HT_BOOL		dbl_trap;

	/*
	 * Whether this piece is captured by double trap.
	 *
	 * If double trap is triggered by two pieces capable of
	 * triggering it, then this piece is captured.
	 */
	HT_BOOL		dbl_trap_capt;

	/*
	 * Whether this piece is a complement to a double trap trigger,
	 * and requires a non-complement piece to be trapped with it for
	 * the trap to trigger. Two complement pieces can not trigger a
	 * double trap.
	 */
	HT_BOOL		dbl_trap_compl;

	/*
	 * Whether double trap can trigger along the edges of the board
	 * for this piece, id est if the capt_sides and custodial
	 * requirements may be dropped due to off-board squares. This
	 * works similarly to capt_edge.
	 */
	HT_BOOL		dbl_trap_edge;

	/*
	 * Whether a full enclosure is required to double trap the
	 * piece.
	 *
	 * This effectively sets capt_sides to 4 and custodial to false
	 * when determining whether the piece triggers double trap.
	 */
	HT_BOOL		dbl_trap_encl;

	/*
	 * True if this piece can escape to a type_square with escape to
	 * win the game.
	 */
	HT_BOOL		escape;

	/*
	 * Square bitmask: the squares that the piece can occupy on the
	 * start of the game, but not return to once he has left them.
	 */
	HT_BIT_U8	noreturn;

	/*
	 * Square bitmask: the squares that the piece can occupy
	 * (normally).
	 */
	HT_BIT_U8	occupies;

	/*
	 * Square bitmask: the squares that the piece can pass, even if cannot occupy.
	 * If this includes a "noreturn" square, then this square is traversable, but not re-enterable.
	 */
	HT_BIT_U8	traverses;
};

#endif

