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
#ifndef HT_AI_T_H
#define HT_AI_T_H

/*
 * Type of AI player.
 */
enum AITYPE
{

	/*
	 * No AI player: human player.
	 */
	HT_AIT_NONE,

	/*
	 * Uninitialized value.
	 */
	HT_AIT_UNK

#ifdef HT_AI_MINIMAX
	,
	HT_AIT_MINIMAX
#endif

};

#endif

