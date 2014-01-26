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
#ifdef HT_UI_XLIB

#ifndef HT_UIX_T_H
#define HT_UIX_T_H

#include "X11/Xlib.h"	/* Display, Window, ... */

#include "boardt.h"	/* HT_BIT_U8 */
#include "boolt.h"	/* HT_BOOL */
#include "uit.h"	/* ui_action */

#ifdef HT_AI_MINIMAX

/*
 * Used to keep track of whether uix_ai_stop() has received the "force
 * computer thinking" command from the user.
 */
enum UIX_STOP
{

	/*
	 * uix_ai_stop() has not been called.
	 */
	HT_UIX_STOP_NOT_ASKED,

	/*
	 * uix_ai_stop() has been called and it's allowed to listen for
	 * the "stop" key, and set the variable to HT_UIX_STOP_ANS_Y.
	 */
	HT_UIX_STOP_ASKED,

	/*
	 * Indicates to force computer thinking.
	 */
	HT_UIX_STOP_ANS_Y

};

#endif /* HT_AI_MINIMAX */

/*
 * Used to keep track of whether uix_play_again() has prompted a yes/no
 * reply from the user.
 */
enum UIX_PLAYAGAIN
{

	/*
	 * uix_play_again() has not asked the user whether to play
	 * again.
	 */
	HT_UIX_PA_NOT_ASKED,

	/*
	 * uix_play_again() has asked the user whether to play again,
	 * and the only acceptable input should be "yes" or "no".
	 */
	HT_UIX_PA_ASKED,

	/*
	 * Answer "yes" to HT_UIX_PA_ASKED.
	 */
	HT_UIX_PA_ANS_Y,

	/*
	 * Answer "no" to HT_UIX_PA_ASKED.
	 */
	HT_UIX_PA_ANS_N

};

/*
 * The beginning of a ui_action. Since ui_actions are supposed to come
 * to the function that deals with them in a complete state, a temporary
 * incomplete ui_action is required until the command is completed.
 *
 * Since all the values in ui_action_contr can have uninitialized
 * values, we actually only need one variable for this: the incomplete
 * "controller" command (HT_UIX_ACT_INCPT_CONTR).
 *
 * In the XLib interface, ui_action.info.contr is available when
 * uix->act_incpt == HT_UIX_ACT_INCPT_CONTR.
 */
enum UIX_ACTION_INCPT
{

	/*
	 * No incomplete command.
	 */
	HT_UIX_ACT_INCPT_NONE,

	/*
	 * Currently setting player controller (ui_action_contr).
	 *
	 * 1.	p_index before setting is HT_PLAYER_UNINIT.
	 *
	 * 2.	type before setting is HT_AIT_UNK.
	 *
	 * 3.	args.minimax.depth_max before setting is
	 *	HT_AIM_DEPTHMAX_UNINIT.
	 *
	 * When all are set, uix_action_incomp should be set to
	 * HT_UIX_ACT_INCPT_NONE, and ui_action.type to HT_UI_ACT_CONTR.
	 */
	HT_UIX_ACT_INCPT_CONTR

};

/*
 * A piece image. Can be transparent or opaque.
 */
/*@exposed@*/
struct img_piece
{

	/*
	 * type_piece->bit
	 */
	HT_BIT_U8	bit;

	/*
	 * If transparent, transp is HT_TRUE and clipmask is available.
	 *
	 * If opaque, transp is HT_FALSE and clipmask may not be used.
	 */
/*@null@*/
/*@owned@*/
	XImage *	img;

	/*
	 * Available if img is transparent.
	 */
	Pixmap		clipmask;

	/*
	 * HT_TRUE if img is transparent.
	 */
	HT_BOOL		transp;

};

/*
 * A square image. These are always opaque.
 */
/*@exposed@*/
struct img_square
{

	/*
	 * type_square->bit
	 */
	HT_BIT_U8	bit;

/*@null@*/
/*@owned@*/
	XImage *	img;

};

/*
 * Data that the XLib interface needs.
 */
/*@exposed@*/
struct uix
{

	/*
	 * Window's bottom margin.
	 *
	 * Fits the computer player's progress bar.
	 */
	unsigned int		wmarg_bottom;

	/*
	 * Window's top margin.
	 */
	unsigned int		wmarg_top;

	/*
	 * Window's left margin.
	 */
	unsigned int		wmarg_left;

	/*
	 * Window's right margin.
	 */
	unsigned int		wmarg_right;

	/*
	 * The X server connection.
	 */
/*@in@*/
/*@notnull@*/
/*@owned@*/
	Display *		display;

	/*
	 * The program window.
	 */
	Window			window;

	/*
	 * window's GC.
	 *
	 * (GC is a pointer to a struct.)
	 */
/*@in@*/
/*@notnull@*/
/*@owned@*/
	GC			gc;

	/*
	 * Default screen of display.
	 */
	int			screen;

	/*
	 * White pixel of screen.
	 */
	unsigned long		px_white;

	/*
	 * Black pixel of screen.
	 */
	unsigned long		px_black;

	/*
	 * An XEvent allocated for all events. This can be shared
	 * throughout the program since it's single-threaded.
	 */
/*@in@*/
/*@notnull@*/
/*@owned@*/
	XEvent *		event;

	/*
	 * Window attributes.
	 */
/*@in@*/
/*@notnull@*/
/*@owned@*/
	XWindowAttributes *	watts;

	/*
	 * An array of img_piece structs.
	 */
/*@in@*/
/*@notnull@*/
/*@owned@*/
	struct img_piece *	img_pieces;

	/*
	 * Elements in img_pieces.
	 */
	unsigned short		img_piecec;

	/*
	 * Capacity of img_pieces.
	 */
	unsigned short		img_piececapc;

	/*
	 * An array of img_square structs.
	 */
/*@in@*/
/*@notnull@*/
/*@owned@*/
	struct img_square *	img_squares;

	/*
	 * Elements in img_squares.
	 */
	unsigned short		img_squarec;

	/*
	 * Capacity of img_squares.
	 */
	unsigned short		img_squarecapc;

	/*
	 * Window width.
	 */
	int			w_width;

	/*
	 * Window height.
	 */
	int			w_height;

	/*
	 * Distance between left side of window and left side of board.
	 * Based on wmarg_*.
	 */
	int			b_x_offs;

	/*
	 * Distance between bottom side of window and bottom side of
	 * board. Based on wmarg_*.
	 */
	int			b_y_offs;

	/*
	 * Width of a square / piece.
	 */
	int			sq_w;

	/*
	 * Height of a square / piece.
	 */
	int			sq_h;

	/*
	 * Copy of game->board->pieces, as they were last painted.
	 *
	 * This helps to paint only the pieces that have changed since
	 * the last paint.
	 *
	 * The length if game->rules->bwidth * game->rules->bheight.
	 */
/*@notnull@*/
/*@owned@*/
	HT_BIT_U8 *		pieces_last;

	/*
	 * Selected position on the board.
	 */
	unsigned short		pos_sel;

	/*
	 * Action to take.
	 */
	struct ui_action *	act;

	/*
	 * Incomplete action, if any.
	 */
	enum UIX_ACTION_INCPT	act_incpt;

	/*
	 * Whether to play again.
	 */
	enum UIX_PLAYAGAIN	act_playagain;

#ifdef HT_AI_MINIMAX
	/*
	 * Whether to force computer thinking.
	 */
	enum UIX_STOP		act_stop;
#endif

};

#endif

#endif

