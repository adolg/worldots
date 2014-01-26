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

#include "playert.h"

/*
 * Maximum (and minimum) amount of players.
 *
 * There are things in the game that depends on the player count being
 * exactly 2, so you can't change this easily.
 */
const unsigned short HT_PLAYERS_MAX	= (unsigned short)2;

/*
 * Value for "no player" or uninitialized.
 */
const unsigned short HT_PLAYER_UNINIT	= USHRT_MAX;

