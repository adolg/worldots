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
#ifndef HT_UIT_H
#define HT_UIT_H

#include <stdlib.h>	/* size_t */

#include "ait.h"	/* aitype */
#include "movet.h"	/* move */
#ifdef HT_UI_CLI
#include "uict.h"	/* uic */
#endif
#ifdef HT_UI_XLIB
#include "uixt.h"	/* uix */
#endif
#ifdef HT_UI_MSQU
#include "uimqt.h"	/* msg queue */
#endif

/*
 * Value passed to ui_ai_progress (and therefore ui#_ai_progress)
 * meaning that the thinking begins.
 */
/*@unchecked@*/
extern
const int HT_AIPROG_START;

/*
 * Value passed to ui_ai_progress (and therefore ui#_ai_progress)
 * meaning that the thinking is done.
 */
/*@unchecked@*/
extern
const int HT_AIPROG_DONE;

/*
 * 0% progress.
 */
/*@unchecked@*/
extern
const int HT_AIPROG_MIN;

/*
 * 100% progress.
 */
/*@unchecked@*/
extern
const int HT_AIPROG_MAX;

/*
 * Action types that a user interface is expected to recognize.
 */
enum HT_UI_ACTT
{

	/*
	 * No action to take, but some input was still correctly
	 * recognized. This can be returned instead of HT_UI_ACT_UNK if
	 * the player cancelled his input (since HT_UI_ACT_UNK indicates
	 * a failure).
	 */
	HT_UI_ACT_NONE,

	/*
	 * Not possible to determine action based on user input (user
	 * input is invalid or malformed), or uninitialized value.
	 *
	 * The computer player can return this value to indicate that it
	 * was unable to find a move, and that the interface is free to
	 * unplug it and continue.
	 */
	HT_UI_ACT_UNK,

	/*
	 * Some non-generic action defined by the interface.
	 */
	HT_UI_ACT_SPECIAL,

	/*
	 * Action to change controller (human or computer) of a player.
	 * Does not necessarily indicate valid controller arguments.
	 *
	 * The computer player can not give this command.
	 */
	HT_UI_ACT_CONTR,

	/*
	 * Action to move a piece on the board.
	 *
	 * May indicate an invalid move.
	 *
	 * If a computer player issues an action to perform an invalid
	 * move, then the computer controller should be unplugged from
	 * that player struct (in favor of a human controller), and then
	 * the player (which is now a human) should be asked to make a
	 * move.
	 */
	HT_UI_ACT_MOVE,

	/*
	 * Action to quit the ongoing game (and possibly the program).
	 * The interface should immediately quit the ongoing game, and
	 * ask the user if he wants to play again; and if applicable,
	 * the interface should interpret another "quit" command as
	 * "no".
	 *
	 * The computer player can not give this command.
	 */
	HT_UI_ACT_QUIT

};

#ifdef HT_AI_MINIMAX

/*
 * Arguments required for creating and initializing an aiminimax struct.
 */
struct ai_minimax_args
{

	/*
	 * aiminimax->depth_max
	 */
	unsigned short	depth_max;

};

#endif

/*
 * Information about the HT_UI_ACT_CONTR command: ui_action.info.contr.
 */
struct ui_action_contr
{

#ifdef HT_AI_MINIMAX
	/*
	 * Arguments for creating the computer player.
	 * The human player requires no arguments.
	 */
	union
	{

		/*
		 * Arguments for HT_AIT_MINIMAX.
		 */
		struct ai_minimax_args	minimax;

	}			args;
#endif

	enum AITYPE		type;

	unsigned short		p_index;

};

/*
 * An interface action; can be given by human or computer players.
 */
struct ui_action
{

	/*
	 * Information about the action type, if any is needed.
	 */
	union
	{

		/*
		 * Information about HT_UI_ACT_CONTR.
		 */
		struct ui_action_contr	contr;

		/*
		 * Information about HT_UI_ACT_MOVE.
		 */
		struct move		mov;

	}			info;

	/*
	 * Type of action to take.
	 */
	enum HT_UI_ACTT		type;

};

/*
 * User interface identifier.
 */
enum HT_UIT
{

	/*
	 * No / uninitialized interface.
	 */
	HT_UIT_NONE	= 0

#ifdef HT_UI_CLI
	,
	/*
	 * Command line interface.
	 */
	HT_UIT_CLI
#endif

#ifdef HT_UI_XLIB
	,
	/*
	 * XLib interface.
	 */
	HT_UIT_XLIB
#endif

#ifdef HT_UI_MSQU
	,
	/*
	 * Generic Message Queue interface.
	 */
	HT_UIT_MSQU
#endif

};

/*
 * A pointer to data about one of user interfaces.
 */
struct ui
{

	union
	{

#ifdef HT_UI_CLI
		/*
		 * Available for type = HT_UIT_CLI.
		 */
		struct uic *	ui_c;
#endif

#ifdef HT_UI_XLIB
		/*
		 * Available for type = HT_UIT_XLIB.
		 */
		struct uix *	ui_x;
#endif

#ifdef HT_UI_MSQU
		/*
		 * Available for type = HT_UIT_MSQU.
		 */
		struct uimq *	ui_mq;
#endif

	}		data;

	/*
	 * HT_UIT_CLI, HT_UI_XLIB or HT_UI_MSQU are the only valid values.
	 */
	enum HT_UIT	type;

};

#endif

