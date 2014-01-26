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
#ifdef HT_UI_MSQU

#ifndef HT_UIMQ_T_H
#define HT_UIMQ_T_H
#include <stdio.h>
/*
 * HT_UI_ACT_SPECIAL for UICLI.
 */
enum HT_UIMQ_ACTT
{

	/*
	 * Uninitialized.
	 */
	HT_UIMQ_ACT_UNK,

	/*
	 * Print board.
	 */
	HT_UIMQ_ACT_PRINT_BOARD,

	/*
	 * Print players.
	 */
	HT_UIMQ_ACT_PRINT_PLAYERS

};

/*
 * Data that the CLI needs.
 */
struct uimq
{

	/*
	 * Line for all reading done by the interface.
	 */
	struct ht_line *	line;

	/*
	 * Line for sending a response to browser.
	 */
	char * line_out;

	/*
	 * Line for storing a dequeued message.
	 */
	char * line_in;

	/*
	 * Temporary (in-memory) file used to exchange data with hnefatafl internals.
	 */
	//struct FILE * memfl;
	/*
	 * Symbols printed by uic_ai_progress.
	 */
	unsigned short		aiprog_symprtd;

};

#endif

#endif

