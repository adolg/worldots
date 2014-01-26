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
#ifndef HT_INVOC_T_H
#define HT_INVOC_T_H

#include "boolt.h"	/* HT_BOOL */
#include "uit.h"	/* HT_UI */

/*
 * What to print, if !invoc.play.
 */
enum HT_INV_PRINT
{

	/*
	 * Print help. HT_L_INV_S_HELP
	 */
	HT_INV_PRINT_HELP,

	/*
	 * HT_L_INV_S_NINE
	 */
	HT_INV_PRINT_NINE,

	/*
	 * Print version. HT_L_INV_S_VERSION
	 */
	HT_INV_PRINT_VERSION

};

/*
 * Invocation parameters parsed from main's argc and argv.
 */
struct invoc
{

	/*
	 * Extra information when play is set or not set.
	 */
	union
	{

		/*
		 * If not play, then what to print?
		 */
		enum HT_INV_PRINT	nplay_print;

		/*
		 * If play, then which interface?
		 */
		enum HT_UIT		play_ui;

	} info;

	/*
	 * Pointer to one of the argv strings in main. This points to a
	 * main argv invocation parameter.
	 *
	 * This is never NULL, but it can be an empty string (strlen is
	 * 0) if you should read from the default RC file in $HOME.
	 */
/*@notnull@*/
/*@exposed@*/
	const char *	file_rc;

	/*
	 * Whether to start the game (HT_TRUE) or print some information
	 * and quit immediately (HT_FALSE).
	 *
	 * The union info contains more information in either case.
	 */
	HT_BOOL		play;

};

#endif

