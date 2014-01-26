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
#ifdef HT_UI_CLI

#ifndef HT_UIC_T_H
#define HT_UIC_T_H

/*
 * HT_UI_ACT_SPECIAL for UICLI.
 */
enum HT_UIC_ACTT
{

	/*
	 * Uninitialized.
	 */
	HT_UIC_ACT_UNK,

	/*
	 * Print board.
	 */
	HT_UIC_ACT_PRINT_BOARD,

	/*
	 * Print players.
	 */
	HT_UIC_ACT_PRINT_PLAYERS

};

/*
 * Data that the CLI needs.
 */
struct uic
{

	/*
	 * Line for all reading done by the interface.
	 */
	struct ht_line *	line;

	/*
	 * Symbols printed by uic_ai_progress.
	 */
	unsigned short		aiprog_symprtd;

};

#endif

#endif

