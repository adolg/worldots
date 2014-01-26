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

#include <errno.h>	/* errno */
#include <limits.h>	/* ULONG_MAX */
#include <stdio.h>	/* sprintf */
#include <stdlib.h>	/* NULL */
#include <string.h>	/* strlen */

#include "X11/Xutil.h"	/* XTextProperty, XSetWMName, ... */
#include "X11/xpm.h"	/* XpmReadFileToImage */

#ifdef HT_AI_MINIMAX
#include "aimalgo.h"	/* aiminimax_command */
#include "aiminimax.h"	/* aiminimax_depth_max_valid */
#endif
#include "board.h"	/* board_* */
#include "num.h"	/* single_bit */
#include "game.h"	/* game_board_reset */
#include "gamet.h"	/* game */
#include "info.h"	/* HT_PROGNAME */
#include "lang.h"	/* HT_L_* */
#include "player.h"	/* player_index_valid */
#include "playert.h"	/* player */
#include "type_piece.h"	/* type_piece_get */
#include "types.h"	/* HT_TYPE_MAX */
#include "ui.h"		/* ui_action_init */
#include "uix.h"

/*
 * NOTE:	For XLib functions returning Status, 0 means failure.
 *		Unfortunately they are very poorly documented, and
 *		sometimes != 0 or 1 means failure. In those cases I have
 *		assumed that it's a boolean value and that success is ==
 *		1. This may be wrong.
 *
 * NOTE:	XLib is not annotated for splint, so most ordinary
 *		statements have to be suppressed, which is done in this
 *		file without explanations. This may hide real errors.
 *
 * NOTE:	XCreatePixmap appears to leak memory even though
 *		XFreePixmap is called, but this memory seems to be freed
 *		by the X server once the program exits (and I think
 *		Valgrind just gets confused because Pixmaps are
 *		allocated on the X server -- not by the program).
 *		There are also some warnings about uninitialized values
 *		in Valgrind when using some XPM files (for img_pieces
 *		when img_piece->transp). Some of these are already
 *		suppressed, so it's probably in order.
 *		Both these Valgrind warnings are resolved by using fully
 *		opaque XPM files for pieces (in which case no Pixmaps
 *		are created), and are probably all false positives (but
 *		might be real errors).
 */

/*
 * A value used for uix->pieces_last to indicate that it should be
 * updated.
 *
 * The value 3 is chosen because all values in board->pieces must be
 * single bits or 0. 3 is multiple bits.
 */
/*@unchecked@*/
static
const HT_BIT_U8	HT_BIT_U8_UPDATE = (HT_BIT_U8)3;

/*
 * Default capacity for uix->img_squarecapc and img_piececapc.
 */
/*@unchecked@*/
static
const unsigned short HT_UIX_IMGS_CAP_DEF	= (unsigned short)3;

/*
 * Grow factor for uix->img_squarecapc and img_piececapc.
 */
/*@unchecked@*/
static
const float HT_UIX_IMGS_CAP_GROWF	= 1.5;

/*
 * Uninitialized value for uix->sq_w and sq_h.
 */
/*@unchecked@*/
static
const int HT_UIX_SIZE_UNINIT		= -1;

/*
 * In addition to ui_action_init (to reset ui_x->act), this function
 * also resets ui_x->act_incpt, which is required on all occasions where
 * ui_x->act is reset.
 */
static
void
uix_action_reset (
/*@in@*/
/*@notnull@*/
	struct uix * const	ui_x
	)
/*@modifies * ui_x@*/
{
	ui_action_init	(ui_x->act);
	ui_x->act_incpt	= HT_UIX_ACT_INCPT_NONE;
}

/*
 * Prints '\a' to stdout. This may or may not beep.
 */
static
enum HT_FR
beep (void)
/*@globals fileSystem, errno, stdout@*/
/*@modifies fileSystem, errno, stdout@*/
{
	if (EOF == fputc('\a', stdout))
	{
		return	HT_FR_FAIL_IO_PRINT;
	}
	return EOF == fflush(stdout) ? HT_FR_FAIL_IO_PRINT :
		HT_FR_SUCCESS;
}

/*
 * Updates ui_x->w_width, w_height, b_x_offs, b_y_offs.
 *
 * Give NULL changed parameter if you don't care about it.
 */
static
enum HT_FR
size_update (
/*@in@*/
/*@notnull@*/
	const struct game * const HT_RSTR	g,
/*@in@*/
/*@notnull@*/
	struct uix * const			ui_x,
/*@in@*/
/*@null@*/
	HT_BOOL * const HT_RSTR			changed
	)
/*@modifies * ui_x, * changed@*/
{
	HT_BOOL	ch;
#ifndef HT_UNSAFE
	if (NULL == g || NULL == ui_x)
	{
		return	HT_FR_FAIL_NULLPTR;
	}
#endif

/*@i1@*/if (0 == XGetWindowAttributes(ui_x->display, ui_x->window,
		ui_x->watts))
	{
		return	HT_FR_FAIL_UIX;
	}

	ch	= HT_UIX_SIZE_UNINIT	== ui_x->w_width
		|| HT_UIX_SIZE_UNINIT	== ui_x->w_height
		|| ui_x->watts->width	!= ui_x->w_width
		|| ui_x->watts->height	!= ui_x->w_height;
	if (ch)
	{
		const int bw_px	= g->rules->bwidth	* ui_x->sq_w;
		const int bh_px	= g->rules->bheight	* ui_x->sq_h;

		ui_x->w_width	= ui_x->watts->width;
		ui_x->w_height	= ui_x->watts->height;

		ui_x->b_x_offs = (ui_x->w_width - bw_px -
			(int)ui_x->wmarg_left - (int)ui_x->wmarg_right)
			/ 2 + (int)ui_x->wmarg_left;
		ui_x->b_y_offs = (ui_x->w_height - bh_px -
			(int)ui_x->wmarg_top - (int)ui_x->wmarg_bottom)
			/ 2 + (int)ui_x->wmarg_top;
	}
	if (NULL != changed)
	{
		* changed	= ch;
	}
	return	HT_FR_SUCCESS;
}

/*
 * Gets the img_piece for bit, or NULL if not found.
 */
/*@null@*/
static
struct img_piece *
get_img_piece (
/*@in@*/
/*@notnull@*/
/*@returned@*/
	struct uix * const HT_RSTR	ui_x,
	const HT_BIT_U8			bit
	)
/*@modifies nothing@*/
{
	unsigned short index	= type_index_get(bit);
	if (index >= ui_x->img_piecec)
	{
		return	NULL;;
	}
	return	& ui_x->img_pieces[index];
}

/*
 * Gets the img_square for bit, or NULL if not found.
 */
/*@null@*/
static
struct img_square *
get_img_square (
/*@in@*/
/*@notnull@*/
/*@returned@*/
	struct uix * const HT_RSTR	ui_x,
	const HT_BIT_U8			bit
	)
/*@modifies nothing@*/
{
	unsigned short index	= type_index_get(bit);
	if (index >= ui_x->img_squarec)
	{
		return	NULL;
	}
	return	& ui_x->img_squares[index];
}

/*
 * Paints the square at pos, and the piece on it if any.
 *
 * This function does not check if pos changed.
 */
static
enum HT_FR
paint_square (
/*@in@*/
/*@notnull@*/
	const struct game * const HT_RSTR	g,
/*@in@*/
/*@notnull@*/
	struct uix * const			ui_x,
	const unsigned short			pos
	)
/*@modifies nothing@*/
{
	/*
	 * lastmove is true if we should highlight the last move on this
	 * pos.
	 */
	HT_BOOL			lastmove;
	HT_BIT_U8		pbit,	sqbit;
	unsigned short		posx,	posy;
	int			x,	y;
#ifndef HT_UNSAFE
	if (NULL == g || NULL == ui_x)
	{
		return	HT_FR_FAIL_NULLPTR;
	}
#endif
	posx	= pos % g->rules->bwidth;
	posy	= pos / g->rules->bwidth;
	x	= ui_x->sq_w * posx + ui_x->b_x_offs;
	y	= ui_x->sq_h * posy + ui_x->b_y_offs;
	sqbit	= g->rules->squares	[pos];
	pbit	= g->b->pieces		[pos];
	if (g->movehist->elemc > 0)
	{
		const struct moveh * const HT_RSTR m =
			& g->movehist->elems[g->movehist->elemc - 1];
		lastmove	= pos == m->pos || pos == m->dest;
	}
	else
	{
		lastmove	= HT_FALSE;
	}

#ifndef HT_UNSAFE
	if (HT_BIT_U8_EMPTY == sqbit
	|| !single_bit((unsigned int)sqbit)
	|| (HT_BIT_U8_EMPTY != pbit && !single_bit((unsigned int)pbit)))
	{
		return	HT_FR_FAIL_ILL_ARG;
	}
#endif

	/*
	 * Paint square.
	 */
	{
		struct img_square * const sqt =
			get_img_square(ui_x, sqbit);
		if (NULL == sqt || NULL == sqt->img)
		{
			return	HT_FR_FAIL_NULLPTR;
		}
/*@i1@*/	if (1 == XPutImage(ui_x->display, ui_x->window,
			ui_x->gc, sqt->img, 0, 0, x, y,
			(unsigned int)ui_x->sq_w,
			(unsigned int)ui_x->sq_h))
		{
			return	HT_FR_FAIL_UIX;
		}
	}

	/*
	 * Paint image.
	 */
	if (HT_BIT_U8_EMPTY != pbit)
	{
		int	xp	= x;
		int	yp	= y;
		struct img_piece * const pt = get_img_piece(ui_x, pbit);
		if (NULL == pt || NULL == pt->img)
		{
			return	HT_FR_FAIL_NULLPTR;
		}

		{
			/*
			 * The piece may be smaller than the board.
			 */
			int tmp	= ui_x->sq_w - pt->img->width;
			if (0 != tmp)
			{
				xp += tmp / 2;
			}
			tmp	= ui_x->sq_h - pt->img->height;
			if (0 != tmp)
			{
				yp += tmp / 2;
			}
		}

		if (pt->transp)
		{
/*@i1@*/		if (0 == XSetClipMask(ui_x->display, ui_x->gc,
				pt->clipmask))
			{
				return	HT_FR_FAIL_UIX;
			}
/*@i1@*/		if (0 == XSetClipOrigin(ui_x->display, ui_x->gc,
				xp, yp))
			{
				return	HT_FR_FAIL_UIX;
			}
		}

/*@i1@*/	if (1 == XPutImage(ui_x->display, ui_x->window,
			ui_x->gc, pt->img, 0, 0, xp, yp,
			(unsigned int)ui_x->sq_w,
			(unsigned int)ui_x->sq_h))
		{
			return	HT_FR_FAIL_UIX;
		}

		if (pt->transp)
		{
/*@i1@*/		if (0 == XSetClipMask(ui_x->display, ui_x->gc,
				None))
			{
				return	HT_FR_FAIL_UIX;
			}
		}
	}

	if (pos == ui_x->pos_sel || lastmove)
	{
/*@i1@*/	if (0 == XDrawRectangle(ui_x->display, ui_x->window,
			ui_x->gc, (int)x, (int)y,
			(unsigned int)(ui_x->sq_w - 1),
			(unsigned int)(ui_x->sq_h - 1)))
		{
			return	HT_FR_FAIL_UIX;
		}
	}

	return	HT_FR_SUCCESS;
}

/*
 * Paints the board.
 *
 * Only pieces that have changed (the HT_BIT_U8 in uix->pieces_last that
 * don't match board->pieces) are repainted, unless repaint_all, in
 * which case all pieces are repainted. This function updates
 * pieces_last automatically for the next time it's called.
 *
 * Assumes that sizes are update (size_update has been called).
 *
 * NOTE:	This function assumes that multiple bits
 *		(HT_BIT_U8_UPDATE)can never occur in board->pieces, and
 *		thus never in uix->pieces_last.
 */
static
enum HT_FR
paint_board (
/*@in@*/
/*@notnull@*/
	const struct game * const	g,
/*@in@*/
/*@notnull@*/
	struct uix * const		ui_x,
	const HT_BOOL			repaint_all
	)
/*@modifies * ui_x@*/
{
	enum HT_FR	fr	= HT_FR_NONE;
	unsigned short	pos;
#ifndef HT_UNSAFE
	if (NULL == g || NULL == ui_x)
	{
		return	HT_FR_FAIL_NULLPTR;
	}
#endif

	for (pos = (unsigned short)0; pos < g->rules->opt_blen; pos++)
	{
		const HT_BIT_U8 sqbit	= g->rules->squares	[pos];
		const HT_BIT_U8 pbit	= g->b->pieces		[pos];
		const HT_BIT_U8 pbit_l	= ui_x->pieces_last	[pos];
		if (HT_BIT_U8_EMPTY == sqbit
		|| (!repaint_all && pbit == pbit_l))
		{
			/*
			 * pbit != pbit_l implies HT_BIT_U8_UPDATE.
			 */
			continue;
		}

		fr	= paint_square(g, ui_x, pos);
		if (HT_FR_SUCCESS != fr)
		{
			return	fr;
		}
#ifndef HT_UNSAFE
		fr	= HT_FR_NONE;
#endif
	}

	memcpy(ui_x->pieces_last, g->b->pieces,
		(size_t)g->rules->opt_blen);
	return	HT_FR_SUCCESS;
}

/*
 * Paints everything.
 *
 * If repaint_all, then every square on the board is repainted.
 */
static
enum HT_FR
paint (
/*@in@*/
/*@notnull@*/
	const struct game * const	g,
/*@in@*/
/*@notnull@*/
	struct uix * const		ui_x,
	const HT_BOOL			repaint_all
	)
/*@modifies * ui_x@*/
{
	enum HT_FR	fr	= HT_FR_NONE;
#ifndef HT_UNSAFE
	if (NULL == g || NULL == ui_x)
	{
		return	HT_FR_FAIL_NULLPTR;
	}
#endif

	if (repaint_all)
	{
		/*
		 * If the window is larger than the board, then
		 * "Play again?" may not be cleared by paint_board.
		 */
/*@i1@*/	if (0 == XClearArea(ui_x->display, ui_x->window, 0, 0,
			(unsigned int)ui_x->w_width,
			(unsigned int)ui_x->w_height, False))
		{
			return	HT_FR_FAIL_UIX;
		}
	}

	fr	= paint_board(g, ui_x, repaint_all);
	if (HT_FR_SUCCESS != fr)
	{
		return	fr;
	}
#ifndef HT_UNSAFE
	fr	= HT_FR_NONE;
#endif

	if (HT_UIX_PA_ASKED == ui_x->act_playagain)
	{
		/*
		 * Getting text size requires messing around with fonts,
		 * which seems to error-prone in X. So assume that the
		 * descent is 2 px. Some of the text may be chopped off
		 * below the baseline.
		 */
/*@i1@*/	if (1 == XDrawImageString(ui_x->display, ui_x->window,
			ui_x->gc, 0, ui_x->w_height - 2,
			HT_L_UIX_PLAYAGAIN,
			(int)strlen(HT_L_UIX_PLAYAGAIN)))
		{
			return	HT_FR_FAIL_UIX;
		}
	}
	return	HT_FR_SUCCESS;
}

/*
 * Sets the selection (uix->pos_sel) to pos_new.
 *
 * Give HT_BOARDPOS_NONE to deselect.
 */
static
enum HT_FR
pos_selected_set (
/*@in@*/
/*@notnull@*/
	const struct game * const	g,
/*@in@*/
/*@notnull@*/
	struct uix * const		ui_x,
	const unsigned short		pos_new
	)
/*@modifies * ui_x@*/
{
	HT_BOOL changed, onboard;
#ifndef HT_UNSAFE
	if (NULL == g || NULL == ui_x)
	{
		return	HT_FR_FAIL_NULLPTR;
	}
/* splint incomplete logic */ /*@i1@*/\
	else if (HT_BOARDPOS_NONE != pos_new
	&& pos_new >= (unsigned short)
		(g->rules->bwidth * g->rules->bheight))
	{
		return	HT_FR_FAIL_ILL_ARG;
	}
#endif

	onboard	= ui_x->pos_sel != HT_BOARDPOS_NONE;
	changed	= ui_x->pos_sel != pos_new;

	if (changed)
	{
		if (onboard)
		{
			ui_x->pieces_last[ui_x->pos_sel] =
				HT_BIT_U8_UPDATE;
		}
		ui_x->pos_sel	= pos_new;
		if (HT_BOARDPOS_NONE != pos_new)
		{
			ui_x->pieces_last[pos_new] =
				HT_BIT_U8_UPDATE;
		}
	}
	else
	{
		/*
		 * Deselect.
		 */
		ui_x->pos_sel			= HT_BOARDPOS_NONE;
		ui_x->pieces_last[pos_new]	= HT_BIT_U8_UPDATE;
	}
	return	paint(g, ui_x, HT_FALSE);
}

/*
 * x and y are screen coordinates.
 *
 * NOTE:	This function assumes that you can't move a piece to a
 *		square on which another piece, that you own, is already
 *		standing.
 */
static
enum HT_FR
handle_event_click (
/*@in@*/
/*@notnull@*/
	const struct game * const	g,
/*@in@*/
/*@notnull@*/
	struct uix * const		ui_x,
	const int			x,
	const int			y
	)
/*@globals fileSystem, errno, stdout@*/
/*@modifies fileSystem, errno, stdout, * ui_x@*/
{
	HT_BOOL		human_turn;
	int		bw, bh, bx, by;
#ifndef HT_UNSAFE
	if (NULL == g || NULL == ui_x)
	{
		return	HT_FR_FAIL_NULLPTR;
	}
#endif

	if (HT_UIX_PA_NOT_ASKED != ui_x->act_playagain)
	{
		/*
		 * Mouse is "disabled" when asking to play again.
		 */
		return	beep();
	}

	human_turn = HT_AIT_NONE == g->players[g->b->turn]->contr;
	if (!human_turn)
	{
		/*
		 * Only the human player can click the board.
		 */
		return	beep();
	}

	bw	= g->rules->bwidth	* ui_x->sq_w;
	bh	= g->rules->bheight	* ui_x->sq_h;
	bx	= x			- ui_x->b_x_offs;
	by	= y			- ui_x->b_y_offs;

	if (	bx >= 0	&& bx <= bw
	&&	by >= 0	&& by <= bh)
	{
		const unsigned short posx = (unsigned short)
			(bx / ui_x->sq_w);
		const unsigned short posy = (unsigned short)
			(by / ui_x->sq_h);
		const unsigned short pos = (unsigned short)
			(posy * g->rules->bwidth + posx);
		const HT_BIT_U8 pbit	= g->b->pieces		[pos];

		if (pos == ui_x->pos_sel)
		{
			return pos_selected_set(g, ui_x,
				HT_BOARDPOS_NONE);
		}
		else if (HT_BOARDPOS_NONE != ui_x->pos_sel)
		{
			if (HT_BIT_U8_EMPTY != pbit
			&& ((unsigned int)g->players[g->b->turn]
				->opt_owned & (unsigned int)pbit)
				== (unsigned int)pbit)
			{
				/*
				 * Click on own piece: select it.
				 */
				return	pos_selected_set(g, ui_x, pos);
			}
			else
			{
				/*
				 * Selected own piece, click on empty
				 * square -> move it.
				 */
				ui_x->act->type	= HT_UI_ACT_MOVE;
				ui_x->act->info.mov.pos	= ui_x->pos_sel;
				ui_x->act->info.mov.dest = pos;
				return	HT_FR_SUCCESS;
			}
		}
		else
		{
			/*
			 * Nothing selected.
			 */
			const struct type_piece * const pt =
				type_piece_get(g->rules, pbit);
			if (HT_BIT_U8_EMPTY == pbit)
			{
				return	beep();
			}
#ifndef HT_UNSAFE
			if (HT_BIT_U8_EMPTY == pt->bit)
			{
				return	HT_FR_FAIL_NULLPTR;
			}
#endif
			if (pt->owner == g->b->turn)
			{
				return pos_selected_set(g, ui_x, pos);
			}
			else
			{
				return	beep();
			}
		}
	}
	else
	{
		/*
		 * Off-board.
		 */
		return	beep();
	}
}

static
enum HT_FR
handle_event_key (
/*@in@*/
/*@notnull@*/
	struct uix * const		ui_x,
/*@in@*/
/*@notnull@*/
	const char *			key
	)
/*@globals fileSystem, errno, stdout@*/
/*@modifies fileSystem, errno, stdout, * ui_x@*/
{
#ifndef HT_UNSAFE
	if (NULL == ui_x || NULL == key)
	{
		return	HT_FR_FAIL_NULLPTR;
	}
#endif

	if (HT_UIX_PA_ASKED == ui_x->act_playagain)
	{
		/*
		 * Intercept potential answer for uix_play_again().
		 */
		if (	0 == strcmp(key, HT_L_UIX_ARG_QUIT)
		||	0 == strcmp(key, HT_L_UIX_ARG_Q_NO))
		{
			ui_x->act_playagain	= HT_UIX_PA_ANS_N;
			return	HT_FR_SUCCESS;
		}
		else if (0 == strcmp(key, HT_L_UIX_ARG_Q_YES))
		{
			ui_x->act_playagain	= HT_UIX_PA_ANS_Y;
			return	HT_FR_SUCCESS;
		}
		else
		{
			/*
			 * Unrecognized or disallowed key: ignore
			 * silently.
			 */
			return	HT_FR_SUCCESS;
		}
	}

	if (HT_UIX_ACT_INCPT_CONTR == ui_x->act_incpt)
	{
		struct ui_action_contr * const actc =
			& ui_x->act->info.contr;
		if (HT_PLAYER_UNINIT == actc->p_index)
		{
			if (!texttoushort(key, & actc->p_index)
			|| !player_index_valid(actc->p_index))
			{
				uix_action_reset(ui_x);
				return	beep();
			}
			return	HT_FR_SUCCESS;
		}
		else if (HT_AIT_UNK == actc->type)
		{
			if (0 == strcmp(key, HT_L_UIX_ARG_CONTR_NONE))
			{
				actc->type	= HT_AIT_NONE;
				ui_x->act_incpt	= HT_UIX_ACT_INCPT_NONE;
				ui_x->act->type	= HT_UI_ACT_CONTR;
			}
#ifdef HT_AI_MINIMAX
			else if (0 == strcmp(key,
				HT_L_UIX_ARG_CONTR_MINIMAX))
			{
				actc->type	= HT_AIT_MINIMAX;
			}
#endif
			else
			{
				/*
				 * Unknown AI type key.
				 */
				uix_action_reset(ui_x);
				return	beep();
			}
			return	HT_FR_SUCCESS;
		}
#ifdef HT_AI_MINIMAX
		else if (HT_AIM_DEPTHMAX_UNINIT ==
			actc->args.minimax.depth_max)
		{
			/*
			 * Reading actc->args.minimax.depth_max, which
			 * is HT_AIM_DEPTHMAX_UNINIT.
			 *
			 * NOTE:	We're converting key from base
			 *		36 here for simplicity. So the
			 *		possible depth range is 1 to 35
			 *		(Z), rather than 1 to 9, had we
			 *		used base 10 (texttoushort).
			 *		Below is old code for base 10
			 *		(which only allows depth 1 to
			 *		9):
			 *
			 * if (!texttoushort(key,
			 *	& actc->args.minimax.depth_max))
			 * {
			 *	uix_action_reset(ui_x);
			 *	return	beep();
			 * }
			 * actc->type	= HT_AIT_MINIMAX;
			 * ui_x->act_incpt	= HT_UIX_ACT_INCPT_NONE;
			 * ui_x->act->type	= HT_UI_ACT_CONTR;
			 * return	HT_FR_SUCCESS;
			 */
			char *		str_end;
			unsigned long	num;
			int		errno_old	= errno;
					errno		= 0;
			num	= strtoul(key, & str_end, 36);
			actc->args.minimax.depth_max =
				(unsigned short)num;
			if (0 != errno	|| strlen(str_end) > 0
			|| ULONG_MAX == num
			|| !aiminimax_depth_max_valid
				(actc->args.minimax.depth_max))
			{
				errno	= errno_old;
				uix_action_reset(ui_x);
				return	beep();
			}
			errno		= errno_old;
			actc->type	= HT_AIT_MINIMAX;
			ui_x->act_incpt	= HT_UIX_ACT_INCPT_NONE;
			ui_x->act->type	= HT_UI_ACT_CONTR;
			return	HT_FR_SUCCESS;
		}
#endif
		else
		{
			return	HT_FR_FAIL_ILL_STATE;
		}
	}
	else	/* HT_UIX_ACT_INCPT_NONE == ui_x->act_incpt */
	{
		/*
		 * Not entering incomplete command.
		 */
		if (0 == strcmp(key, HT_L_UIX_ARG_CONTR))
		{
/*
 * NOTE:	There was a problem with changing the player controller
 *		while the computer is thinking. I think it is fixed (at
 *		least it hasn't happened again), but if it does occur,
 *		then just uncomment the following code to forbid
 *		changing the player while the AI is thinking:
 *
 *			if (HT_UIX_STOP_ASKED == ui_x->act_stop)
 *			{
 *				return	beep();
 *			}
 */
			/*
			 * Set ui_action_contr to uninitialized values.
			 */
			uix_action_reset(ui_x);
			ui_x->act_incpt	= HT_UIX_ACT_INCPT_CONTR;
			return		HT_FR_SUCCESS;
		}
		else if (0 == strcmp(key, HT_L_UIX_ARG_QUIT))
		{
			ui_x->act->type	= HT_UI_ACT_QUIT;
			return		HT_FR_SUCCESS;
		}
#ifdef HT_AI_MINIMAX
		else if (0 == strcmp(key, HT_L_UIX_ARG_STOP))
		{
			if (HT_UIX_STOP_ASKED == ui_x->act_stop)
			{
				ui_x->act_stop	= HT_UIX_STOP_ANS_Y;
				return		HT_FR_SUCCESS;
			}
			else
			{
				return	beep();
			}
		}
#endif
		else
		{
			/*
			 * Unrecognized key: ignore silently.
			 */
			return HT_FR_SUCCESS;
		}
	}
}

/*
 * Handles the given event, no matter how it has arisen.
 *
 * This may happen during the computer player's thinking loop.
 */
static
enum HT_FR
handle_event (
/*@in@*/
/*@notnull@*/
	const struct game * const	g,
/*@in@*/
/*@notnull@*/
	struct uix * const		ui_x,
/*@in@*/
/*@notnull@*/
	XEvent * const			event
	)
/*@globals fileSystem, errno, stdout@*/
/*@modifies fileSystem, errno, stdout, * ui_x@*/
{
#ifndef HT_UNSAFE
	if (NULL == g || NULL == ui_x || NULL == event)
	{
		return	HT_FR_FAIL_NULLPTR;
	}
#endif

	if (MappingNotify == event->type)
	{
/*
 * NOTE:	See `KeyBind.c`: XRefreshKeyboardMapping always returns
 *		1. Check anyway for future-compatibility.
 */
/*@i1@*/	if (0 == XRefreshKeyboardMapping(& event->xmapping))
		{
			return	HT_FR_FAIL_UIX;
		}
	}
	else if (Expose == event->type)
	{
		if (0 == event->xexpose.count)
		{
			const enum HT_FR fr = size_update(g, ui_x,
				NULL);
			if (HT_FR_SUCCESS != fr)
			{
				return	fr;
			}
			return	paint(g, ui_x, HT_TRUE);
		}
	}
	else if (KeyPress == event->type)
	{
/*@i1@*/	const KeySym	ks = XLookupKeysym(& event->xkey, 0);
		const char *	s	= NULL;
		if ((KeySym)XK_Escape == ks)
		{
			uix_action_reset(ui_x);
			return	HT_FR_SUCCESS;
		}

/*@i1@*/	s	= XKeysymToString(ks);
		if (NULL == s)
		{
			/*
			 * Symbol is meaningless to the program. Ignore
			 * in order to not emit beeps all the time.
			 */
			return	HT_FR_SUCCESS;
		}
/*@i1@*/	return	handle_event_key(ui_x, s);
	}
/* splint incomplete logic */ /*@i1@*/\
	else if (ButtonPress == event->type)
	{
		return	handle_event_click(g, ui_x,
			event->xbutton.x, event->xbutton.y);
	}

	return	HT_FR_SUCCESS;
}

/*
 * Handles any events that are waiting, without waiting for new ones.
 */
static
enum HT_FR
event_flush_noblock (
/*@in@*/
/*@notnull@*/
	const struct game * const	g,
/*@in@*/
/*@notnull@*/
	struct uix * const		ui_x
	)
/*@globals fileSystem, errno, stdout@*/
/*@modifies fileSystem, errno, stdout, * ui_x@*/
{
	enum HT_FR	fr	= HT_FR_NONE;
#ifndef HT_UNSAFE
	if (NULL == g || NULL == ui_x)
	{
		return	HT_FR_FAIL_NULLPTR;
	}
#endif

/*@i2@*/while (XPending(ui_x->display) > 0)
	{
/*@i1@*/	if (1 == XNextEvent(ui_x->display, ui_x->event))
		{
			return	HT_FR_FAIL_UIX;
		}

		fr	= handle_event(g, ui_x, ui_x->event);
		if (HT_FR_SUCCESS != fr)
		{
			return	fr;
		}
#ifndef HT_UNSAFE
		fr	= HT_FR_NONE;
#endif
		if (HT_UI_ACT_UNK	!= ui_x->act->type
		&& HT_UI_ACT_NONE	!= ui_x->act->type)
		{
			break;
		}
	}

	return	HT_FR_SUCCESS;
}

/*
 * Handles any events that are waiting, or waits for new ones.
 */
static
enum HT_FR
event_flush_block (
/*@in@*/
/*@notnull@*/
	const struct game * const	g,
/*@in@*/
/*@notnull@*/
	struct uix * const		ui_x
	)
/*@globals fileSystem, errno, stdout@*/
/*@modifies fileSystem, errno, stdout, * ui_x@*/
{
	enum HT_FR	fr	= HT_FR_NONE;
#ifndef HT_UNSAFE
	if (NULL == g || NULL == ui_x)
	{
		return	HT_FR_FAIL_NULLPTR;
	}
#endif

	do
	{
/*@i2@*/	while (XPending(ui_x->display) > 0)
		{
			/*
			 * Discard excessive events.
			 */
/*@i1@*/		if (1 == XNextEvent(ui_x->display, ui_x->event))
			{
				return	HT_FR_FAIL_UIX;
			}
		}

/*@i1@*/	if (1 == XNextEvent(ui_x->display, ui_x->event))
		{
			return	HT_FR_FAIL_UIX;
		}

		fr	= handle_event(g, ui_x, ui_x->event);
		if (HT_FR_SUCCESS != fr)
		{
			return	fr;
		}
#ifndef HT_UNSAFE
		fr	= HT_FR_NONE;
#endif
	} while (	HT_UI_ACT_UNK	== ui_x->act->type
	||		HT_UI_ACT_NONE	== ui_x->act->type);

	return	HT_FR_SUCCESS;
}

static
enum HT_FR
action_perform (
/*@in@*/
/*@notnull@*/
	struct game * const	g,
/*@in@*/
/*@notnull@*/
	struct uix * const	ui_x
	)
/*@globals internalState, fileSystem, errno, stdout@*/
/*@modifies internalState, fileSystem, errno, stdout, * g, * ui_x@*/
{
	enum HT_FR fr	= HT_FR_NONE;
#ifndef HT_UNSAFE
	if (NULL == g || NULL == ui_x)
	{
		return	HT_FR_FAIL_NULLPTR;
	}
#endif

	if (HT_UI_ACT_MOVE == ui_x->act->type)
	{
		const struct move * const m	= & ui_x->act->info.mov;
		HT_BOOL legal	= HT_FALSE;

		fr = game_move(g, m->pos, m->dest, & legal);
		if (HT_FR_SUCCESS != fr)
		{
			return	fr;
		}
#ifndef HT_UNSAFE
		fr	= HT_FR_NONE;
#endif
		if (!legal)
		{
			const struct player * const playercur =
				g->players[g->b->turn];
			if (HT_AIT_NONE == playercur->contr)
			{
				return	beep();
			}
#ifdef HT_AI_MINIMAX
			else
			{
				fr = ui_player_contr_unset(g,
					g->b->turn);
				if (HT_FR_SUCCESS != fr)
				{
					return	fr;
				}
#ifndef HT_UNSAFE
				fr	= HT_FR_NONE;
#endif
				return	beep();
			}
#endif /* HT_AI_MINIMAX */
		}

		ui_x->pieces_last[m->pos]	= HT_BIT_U8_UPDATE;
		ui_x->pieces_last[m->dest]	= HT_BIT_U8_UPDATE;
		if (g->movehist->elemc > (size_t)1)
		{
			const struct moveh * const HT_RSTR mnl =
				& g->movehist->elems
				[g->movehist->elemc - 2];
			ui_x->pieces_last[mnl->pos] = HT_BIT_U8_UPDATE;
			ui_x->pieces_last[mnl->dest] = HT_BIT_U8_UPDATE;
		}

		if (HT_BOARDPOS_NONE != ui_x->pos_sel)
		{
			fr	= pos_selected_set(g, ui_x,
				HT_BOARDPOS_NONE);
		}
		else	/* Computer moved. */
		{
			fr	= paint(g, ui_x, HT_FALSE);
		}
/*@i2@*/	if (0 == XFlush(ui_x->display) && HT_FR_SUCCESS == fr)
		{
			return	HT_FR_FAIL_UIX;
		}
		if (HT_FR_SUCCESS != fr)
		{
			return	fr;
		}
#ifndef HT_UNSAFE
		fr	= HT_FR_NONE;
#endif
	}
/* splint incomplete logic */ /*@i1@*/\
	else if (HT_UI_ACT_CONTR == ui_x->act->type)
	{
		HT_BOOL valid	= HT_FALSE;
		fr = ui_player_contr_set(g, & ui_x->act->info.contr,
			& valid);
		if (HT_FR_SUCCESS != fr)
		{
			return fr;
		}
#ifndef HT_UNSAFE
		fr	= HT_FR_NONE;
#endif
		if (!valid)
		{
			return	beep();
		}
		if (HT_BOARDPOS_NONE != ui_x->pos_sel)
		{
			fr	= pos_selected_set(g, ui_x,
				HT_BOARDPOS_NONE);
			if (HT_FR_SUCCESS != fr)
			{
				return	fr;
			}
#ifndef HT_UNSAFE
			fr	= HT_FR_NONE;
#endif
/*@i1@*/		if (0 == XFlush(ui_x->display))
			{
				return	HT_FR_FAIL_UIX;
			}
		}
	}
/*
 * If there were any special actions, we'd check for them here:
 *	else if (HT_UI_ACT_SPECIAL == ui_x->act->type)
 */
/* splint incomplete logic */ /*@i2@*/\
	else if (HT_UI_ACT_UNK == ui_x->act->type)
	{
		/*
		 * Invalid input.
		 */
		if (HT_AIT_NONE == g->players[g->b->turn]->contr)
		{
			return	beep();
		}
#ifdef HT_AI_MINIMAX
		else
		{
			fr = ui_player_contr_unset(g, g->b->turn);
			if (HT_FR_SUCCESS != fr)
			{
				return	fr;
			}
#ifndef HT_UNSAFE
			fr	= HT_FR_NONE;
#endif
			return	beep();
		}
#endif /* HT_AI_MINIMAX */
	}
	/*
	 * else: HT_UI_ACT_NONE or HT_UI_ACT_QUIT -- do no more.
	 */

	return HT_FR_SUCCESS;
}

static
enum HT_FR
player_action (
/*@in@*/
/*@notnull@*/
	struct game * const		g,
/*@in@*/
/*@notnull@*/
	struct ui * const		interf
	)
/*@globals internalState, fileSystem, errno, stdout@*/
/*@modifies internalState, fileSystem, errno, stdout, * g, * interf@*/
{
	enum HT_FR		fr	= HT_FR_NONE;
	struct player *		playercur;
	struct uix *		ui_x;
#ifndef HT_UNSAFE
	if (NULL == g || NULL == interf)
	{
		return	HT_FR_FAIL_NULLPTR;
	}
#endif

	ui_x			= interf->data.ui_x;
	playercur		= g->players[g->b->turn];
	uix_action_reset	(ui_x);
	/*
	 * ui_x->act->type = HT_UI_ACT_UNK.
	 */

	if (HT_AIT_NONE == playercur->contr)
	{
		fr	= event_flush_block(g, ui_x);
		if (HT_FR_SUCCESS != fr)
		{
			return	fr;
		}
#ifndef HT_UNSAFE
		fr	= HT_FR_NONE;
#endif
	}
#ifdef HT_AI_MINIMAX
	else if (HT_AIT_MINIMAX == playercur->contr)
	{
		if (NULL == playercur->ai.minimax)
		{
			return	HT_FR_FAIL_NULLPTR;
		}

		fr = aiminimax_command(g, interf,
			playercur->ai.minimax, ui_x->act);
		if (HT_FR_SUCCESS != fr)
		{
			return	fr;
		}
#ifndef HT_UNSAFE
		fr	= HT_FR_NONE;
#endif
	}
#endif /* HT_AI_MINIMAX */
	else
	{
		/*
		 * Unrecognized controller.
		 */
		return	HT_FR_FAIL_ENUM_UNK;
	}

	return action_perform(g, ui_x);
}

/*
 * Plays a full game.
 *
 * XSelectInput is NoEvent until this is called, and after this returns.
 * Therefore: after uix_init() and before uix_play_new(), the XLib
 * interface will not handle any events.
 */
enum HT_FR
uix_play_new (
	struct game * const	g,
	struct ui * const	interf
	)
{
	enum HT_FR	fr	= HT_FR_NONE;
	struct uix *	ui_x	= NULL;
	unsigned short	winner	= HT_PLAYER_UNINIT;
#ifndef HT_UNSAFE
	if (NULL == g || NULL == interf)
	{
		return	HT_FR_FAIL_NULLPTR;
	}
#endif

	ui_x	= interf->data.ui_x;

	fr	= game_board_reset(g);
	if (HT_FR_SUCCESS != fr)
	{
		return	fr;
	}
#ifndef HT_UNSAFE
	fr	= HT_FR_NONE;
#endif

	fr	= size_update(g, ui_x, NULL);
	if (HT_FR_SUCCESS != fr)
	{
		return	fr;
	}
#ifndef HT_UNSAFE
	fr	= HT_FR_NONE;
#endif

	fr	= paint(g, ui_x, HT_TRUE);
	if (HT_FR_SUCCESS != fr)
	{
		return	fr;
	}
#ifndef HT_UNSAFE
	fr	= HT_FR_NONE;
#endif

/*@i1@*/if (0 == XSelectInput(ui_x->display, ui_x->window,
		(long)(	(unsigned long)ButtonPressMask		|
			(unsigned long)ExposureMask		|
			(unsigned long)KeyPressMask		)))
	{
		return	HT_FR_FAIL_UIX;
	}

	do
	{
		fr	= player_action(g, interf);
		if (HT_FR_SUCCESS != fr)
		{
			return		fr;
		}
#ifndef HT_UNSAFE
		fr	= HT_FR_NONE;
#endif

		if (HT_UI_ACT_QUIT == ui_x->act->type)
		{
			break;
		}
	} while (!game_over(g, & winner));
	/*
	 * NOTE:	winner is ignored.
	 */

/*@i1@*/if (0 == XSelectInput(ui_x->display, ui_x->window, NoEventMask))
	{
		return	HT_FR_FAIL_UIX;
	}

	return	HT_FR_SUCCESS;
}

/*
 * This function makes handle_event_key() and handle_event_click()
 * behave differently, and only respond to "yes" / "no" answers to the
 * question "play again?" (displayed by paint()).
 */
enum HT_FR
uix_play_again (
	const struct game * const	g,
	struct uix * const		ui_x,
	HT_BOOL * const HT_RSTR		again
	)
{
	enum HT_FR	fr	= HT_FR_NONE;
	* again			= HT_FALSE;
#ifndef HT_UNSAFE
	if (NULL == g || NULL == ui_x || NULL == again)
	{
		return	HT_FR_FAIL_NULLPTR;
	}
#endif

	/*
	 * This will cause handle_event() to ignore all events except
	 * "yes" or "no" answers. And it will cause paint() to draw the
	 * question string.
	 */
	ui_x->act_playagain	= HT_UIX_PA_ASKED;
	fr	= paint(g, ui_x, HT_FALSE);
	if (HT_FR_SUCCESS != fr)
	{
		return	fr;
	}
#ifndef HT_UNSAFE
	fr	= HT_FR_NONE;
#endif

/*@i1@*/if (0 == XSelectInput(ui_x->display, ui_x->window,
		(long)(	(unsigned long)ButtonPressMask		|
			(unsigned long)ExposureMask		|
			(unsigned long)KeyPressMask		)))
	{
		return	HT_FR_FAIL_UIX;
	}

	do
	{
		fr	= event_flush_block(g, ui_x);
		if (HT_FR_SUCCESS != fr)
		{
			return	fr;
		}
#ifndef HT_UNSAFE
		fr	= HT_FR_NONE;
#endif

		if (HT_UIX_PA_ANS_Y == ui_x->act_playagain)
		{
			* again	= HT_TRUE;
			break;
		}
	} while (HT_UIX_PA_ANS_N != ui_x->act_playagain);

	ui_x->act_playagain	= HT_UIX_PA_NOT_ASKED;

/*@i1@*/if (0 == XSelectInput(ui_x->display, ui_x->window, NoEventMask))
	{
		return	HT_FR_FAIL_UIX;
	}

	return	HT_FR_SUCCESS;
}

#ifdef HT_AI_MINIMAX

/*
 * This function can:
 *
 * *	Set stop to HT_TRUE without changing ui_x->act->type. If so, the
 *	computer should immediately select a move.
 *
 * *	Set stop to HT_TRUE and change ui_x->act->type. If so, the
 *	computer should return without selecting a move (because
 *	selecting a move would override the ui_x->act->type that has
 *	been selected by this function's call to handle_event()).
 *
 * *	Set stop to HT_FALSE, in which case the computer can continue
 *	thinking.
 */
enum HT_FR
uix_ai_stop (
	const struct game * const	g,
	struct uix * const		ui_x,
	HT_BOOL * const HT_RSTR		stop
	)
{
	enum HT_FR	fr	= HT_FR_NONE;
#ifndef HT_UNSAFE
	if (NULL == g || NULL == ui_x || NULL == stop)
	{
		return	HT_FR_FAIL_NULLPTR;
	}
#endif

	ui_x->act_stop	= HT_UIX_STOP_ASKED;

	fr	= event_flush_noblock(g, ui_x);
	if (HT_FR_SUCCESS != fr)
	{
		return	fr;
	}
#ifndef HT_UNSAFE
	fr	= HT_FR_NONE;
#endif

	if (HT_UIX_STOP_ANS_Y	== ui_x->act_stop
	|| HT_UI_ACT_UNK	!= ui_x->act->type)
	{
		* stop	= HT_TRUE;
	}
	ui_x->act_stop	= HT_UIX_STOP_NOT_ASKED;

	return	HT_FR_SUCCESS;
}

enum HT_FR
uix_ai_progress (
	struct uix * const	ui_x,
	const int		prog
	)
{
	int	width, posy;
#ifndef HT_UNSAFE
	if (NULL == ui_x)
	{
		return	HT_FR_FAIL_NULLPTR;
	}
#endif

	posy	= ui_x->w_height - (int)ui_x->wmarg_bottom;

	if (prog == HT_AIPROG_MIN
	|| HT_AIPROG_START == prog || HT_AIPROG_DONE == prog)
	{
/*@i1@*/	if (0 == XClearArea(ui_x->display, ui_x->window, 0,
			posy, (unsigned int)ui_x->w_width,
			ui_x->wmarg_bottom, False))
		{
			return	HT_FR_FAIL_UIX;
		}
		return	HT_FR_SUCCESS;
	}
/* splint incomplete logic */ /*@i1@*/\
	else if (prog < HT_AIPROG_MIN || prog > HT_AIPROG_MAX)
	{
		return	HT_FR_FAIL_ILL_ARG;
	}

	width	= (prog * ui_x->w_width) / HT_AIPROG_MAX;

/*@i1@*/if (0 == XFillRectangle(ui_x->display, ui_x->window, ui_x->gc,
		0, posy, (unsigned int)width,
		(unsigned int)ui_x->wmarg_bottom))
	{
		return	HT_FR_FAIL_UIX;
	}

/*@i1@*/if (0 == XFlush(ui_x->display))
	{
		return	HT_FR_FAIL_UIX;
	}

	return	HT_FR_SUCCESS;
}

#endif /* HT_AI_MINIMAX */

struct uix *
alloc_uix (
	const struct game * const g
	)
{
	unsigned short		i;
	struct uix * const	ui_x = malloc(sizeof(* ui_x));
	if (NULL == ui_x)
	{
		return	NULL;
	}

	ui_x->img_piecec	= ui_x->img_squarec = (unsigned short)0;
	ui_x->img_piececapc	= ui_x->img_squarecapc =
		HT_UIX_IMGS_CAP_DEF;

	ui_x->img_squares = malloc(sizeof(* ui_x->img_squares) *
			ui_x->img_squarecapc);
	if (NULL == ui_x->img_squares)
	{
		free	(ui_x);
		return	NULL;
	}

	ui_x->img_pieces = malloc(sizeof(* ui_x->img_pieces) *
			ui_x->img_piececapc);
	if (NULL == ui_x->img_pieces)
	{
		free	(ui_x->img_squares);
		free	(ui_x);
		return	NULL;
	}

	for (i = (unsigned short)0; i < ui_x->img_piececapc; i++)
	{
		ui_x->img_pieces[i].img = ui_x->img_squares[i].img =
			NULL;
	}

	ui_x->event	= malloc(sizeof(* ui_x->event));
	if (NULL == ui_x->event)
	{
		free	(ui_x->img_pieces);
		free	(ui_x->img_squares);
		free	(ui_x);
		return	NULL;
	}

	ui_x->pieces_last = malloc(sizeof(* ui_x->pieces_last)
		* g->rules->opt_blen);
	if (NULL == ui_x->pieces_last)
	{
		free	(ui_x->event);
		free	(ui_x->img_pieces);
		free	(ui_x->img_squares);
		free	(ui_x);
		return	NULL;
	}

	ui_x->watts	= malloc(sizeof(* ui_x->watts));
	if (NULL == ui_x->watts)
	{
		free	(ui_x->pieces_last);
		free	(ui_x->event);
		free	(ui_x->img_pieces);
		free	(ui_x->img_squares);
		free	(ui_x);
		return	NULL;
	}

	ui_x->act	= malloc(sizeof(* ui_x->act));
	if (NULL == ui_x->act)
	{
		free	(ui_x->watts);
		free	(ui_x->pieces_last);
		free	(ui_x->event);
		free	(ui_x->img_pieces);
		free	(ui_x->img_squares);
		free	(ui_x);
		return	NULL;
	}

	ui_x->w_width = ui_x->w_height = ui_x->b_x_offs =
		ui_x->b_y_offs = ui_x->sq_w = ui_x->sq_h =
		HT_UIX_SIZE_UNINIT;
	ui_x->act_playagain	= HT_UIX_PA_NOT_ASKED;
#ifdef HT_AI_MINIMAX
	ui_x->act_stop		= HT_UIX_STOP_NOT_ASKED;
#endif

	return			ui_x;
}

/*
 * Frees all memory that ui_x uses, and the XLib resources associated
 * with it.
 */
void
free_uix (
	struct uix * const	ui_x
	)
{
	if (NULL != ui_x->act)
	{
		free	(ui_x->act);
	}

	if (NULL != ui_x->watts)
	{
/*@i2@*/	free	(ui_x->watts);
	}

	if (NULL != ui_x->pieces_last)
	{
		free	(ui_x->pieces_last);
	}

	if (NULL != ui_x->display)
	{
/*@i2@*/	(void) XSync(ui_x->display, True);
	}

	if (NULL != ui_x->img_squares)
	{
		if (NULL != ui_x->display)
		{
			unsigned short i;
			for (i = (unsigned short)0;
				i < ui_x->img_squarec; i++)
			{
/*@i5@*/			XDestroyImage(ui_x->img_squares[i].img);
			}
		}
/*@i4@*/	free	(ui_x->img_squares);
	}

	if (NULL != ui_x->img_pieces)
	{
		if (NULL != ui_x->display)
		{
			unsigned short i;
			for (i = (unsigned short)0;
				i < ui_x->img_piecec; i++)
			{
/*@i5@*/			XDestroyImage(ui_x->img_pieces[i].img);
				if (ui_x->img_pieces[i].transp
				&& NULL != ui_x->display)
				{
/*@i2@*/				(void)XFreePixmap(ui_x->display,
					ui_x->img_pieces[i].clipmask);
				}
			}
		}
/*@i4@*/	free	(ui_x->img_pieces);
	}

	if (NULL != ui_x->display)
	{
		if (NULL != ui_x->gc)
		{
/*@i2@*/		(void) XFreeGC	(ui_x->display, ui_x->gc);
		}
/*@i2@*/	(void) XUnmapWindow	(ui_x->display, ui_x->window);
/*@i2@*/	(void) XDestroyWindow	(ui_x->display, ui_x->window);
/*@i2@*/	(void) XCloseDisplay	(ui_x->display);
	}
	if (NULL != ui_x->event)
	{
		free	(ui_x->event);
	}
	free		(ui_x);
}

static
enum HT_FR
gc_configure (
/*@in@*/
/*@notnull@*/
	const struct uix * const	ui_x,
/*@in@*/
/*@notnull@*/
	GC const			gc
	)
/*@modifies nothing@*/
{
/*@i1@*/if (0	== XSetForeground(ui_x->display, gc, ui_x->px_white)
/*@i1@*/|| 0	== XSetBackground(ui_x->display, gc, ui_x->px_black))
	{
		return	HT_FR_FAIL_UIX;
	}
	return	HT_FR_SUCCESS;
}

/*
 * Loads "path" + "p###.xpm" into * img, if it exists and is possible.
 *
 * img_piece->transp is HT_TRUE if the image is transparent, in which
 * case the pixmap is set. Else the pixmaps is not set.
 */
static
enum HT_FR
get_ximage_piece (
/*@in@*/
/*@notnull@*/
	const struct uix * const	ui_x,
/*@in@*/
/*@notnull@*/
	const char * const HT_RSTR	path,
/*@in@*/
/*@notnull@*/
	struct img_piece * const	imgp,
/*@in@*/
/*@notnull@*/
	GC * const			gc_piece
	)
/*@modifies * imgp, * gc_piece@*/
{
	XImage *	mask		= NULL;
	char *		filename	= NULL;
	size_t		filenlen;
#ifndef HT_UNSAFE
	if (NULL == path || NULL == imgp || NULL == gc_piece)
	{
		return	HT_FR_FAIL_NULLPTR;
	}
/* splint incomplete logic */ /*@i1@*/\
	else if (!single_bit((unsigned int)imgp->bit)
	|| imgp->bit > (HT_BIT_U8)128)
	{
		return	HT_FR_FAIL_ILL_ARG;
	}
#endif

	filenlen	= strlen(path) + 8;
	filename	= malloc(sizeof(* filename) * (filenlen + 1));
	if (NULL == filename)
	{
		return	HT_FR_FAIL_ALLOC;
	}

/* splint snprintf not available in C89 */ /*@i1@*/\
	if (sprintf(filename, "%sp%03d.xpm", path, (int)imgp->bit) < 0)
	{
		free	(filename);
		return	HT_FR_FAIL_IO_PRINT;
	}

/*@i3@*/\
	if (XpmReadFileToImage(ui_x->display, filename, & imgp->img,
/*@i2@*/\
		& mask, NULL))
	{
		free	(filename);
		return	HT_FR_FAIL_UIX_XPM_FILER;
	}

	if (NULL == imgp->img)
	{
		free	(filename);
		return	HT_FR_FAIL_UIX_XPM_FILER;
	}

	imgp->transp	= NULL == mask ? HT_FALSE : HT_TRUE;
	if (imgp->transp)
	{
/* splint doesn't recognize that mask != NULL */ /*@-nullderef@*/
/*@i1@*/	imgp->clipmask = XCreatePixmap(ui_x->display,
			ui_x->window, (unsigned int)mask->width,
			(unsigned int)mask->height,
			(unsigned int)mask->depth);

		if (NULL == * gc_piece)
		{
			/*
			 * Lazily initialize gc_piece only if needed.
			 */
			enum HT_FR	fr	= HT_FR_NONE;
/*@i2@*/		* gc_piece = XCreateGC(ui_x->display,
/*@i1@*/			imgp->clipmask, 0L, NULL);
			fr = gc_configure(ui_x, * gc_piece);
			if (HT_FR_SUCCESS != fr)
			{
/*@i3@*/			(void) XDestroyImage(mask);
				mask		= NULL;
/*@i2@*/			(void) XFreePixmap(ui_x->display,
					imgp->clipmask);
				free		(filename);
				return	fr;
			}
		}

/*@i1@*/	if (1 == XPutImage(ui_x->display, imgp->clipmask,
/*@i2@*/		* gc_piece, mask, 0, 0, 0, 0,
			(unsigned int)mask->width,
			(unsigned int)mask->height))
		{
/*@i3@*/		XDestroyImage	(mask);
			mask		= NULL;
/*@i2@*/		(void) XFreePixmap(ui_x->display,
					imgp->clipmask);
			free		(filename);
			return		HT_FR_FAIL_UIX;
		}

/*@i3@*/	XDestroyImage	(mask);
		mask		= NULL;
/*@=nullderef@*/
	}

	free	(filename);
	return	HT_FR_SUCCESS;
}

/*
 * Loads "path" + "s###.xpm" into * img, if it exists and is possible.
 */
static
enum HT_FR
get_ximage_square (
/*@in@*/
/*@notnull@*/
	struct uix * const		ui_x,
/*@in@*/
/*@notnull@*/
	const char * const HT_RSTR	path,
	const HT_BIT_U8			bit,
/*@in@*/
/*@null@*/
	XImage * * const		img
	)
/*@modifies * ui_x, * img@*/
{
	char *	filename	= NULL;
	size_t	filenlen;
#ifndef HT_UNSAFE
	if (NULL == path || NULL == img)
	{
		/*
		 * splint doesn't recognize that * img may be NULL, but
		 * img may not be.
		 */
		return	HT_FR_FAIL_NULLPTR;
	}
/* splint incomplete logic */ /*@i2@*/\
	else if (!single_bit((unsigned int)bit) || bit > (HT_BIT_U8)128)
	{
		return	HT_FR_FAIL_ILL_ARG;
	}
#endif

	filenlen	= strlen(path) + 8;
	filename	= malloc(sizeof(* filename) * (filenlen + 1));
	if (NULL == filename)
	{
		return	HT_FR_FAIL_ALLOC;
	}

/* splint snprintf not available in C89 */ /*@i1@*/\
	if (sprintf(filename, "%ss%03d.xpm", path, (int)bit) < 0)
	{
		free	(filename);
		return	HT_FR_FAIL_IO_PRINT;
	}

/*@i2@*/\
	if (XpmReadFileToImage(ui_x->display, filename, img,
/*@i2@*/\
		NULL, NULL))
	{
		free	(filename);
		return	HT_FR_FAIL_UIX_XPM_FILER;
	}

	if (NULL == img)
	{
		free	(filename);
		return	HT_FR_FAIL_UIX_XPM_FILER;
	}

	if (HT_UIX_SIZE_UNINIT	== ui_x->sq_w
	|| HT_UIX_SIZE_UNINIT	== ui_x->sq_h)
	{
		ui_x->sq_w	= (* img)->width;
		ui_x->sq_h	= (* img)->height;
	}

	free	(filename);
	return	HT_FR_SUCCESS;
/*@i1@*/\
}

/*
 * Grows (by re-allocating) uix->img_piececapc.
 *
 * Not allowed to grow past HT_TYPE_MAX.
 */
static
enum HT_FR
uix_img_piece_grow (
/*@in@*/
/*@notnull@*/
	struct uix * const	ui_x
	)
/*@modifies * ui_x@*/
{
	struct img_piece * pieces_new	= NULL;
	unsigned short capc_new = (unsigned short)
		(ui_x->img_piececapc * HT_UIX_IMGS_CAP_GROWF);
	if (ui_x->img_piececapc >= HT_TYPE_MAX)
	{
		return	HT_FR_FAIL_ILL_ARG;
	}
	if (capc_new <= ui_x->img_piececapc)
	{
		capc_new = (unsigned short)(ui_x->img_piececapc + 1);
	}
	if (capc_new > HT_TYPE_MAX)
	{
		capc_new	= HT_TYPE_MAX;
	}

/* splint realloc */ /*@i4@*/\
	pieces_new = realloc(ui_x->img_pieces,
		sizeof(* ui_x->img_pieces) * capc_new);
	if (NULL == pieces_new)
	{
/* splint realloc */ /*@i2@*/\
		return	HT_FR_FAIL_ALLOC;
	}
	ui_x->img_pieces	= pieces_new;
	ui_x->img_piececapc	= capc_new;

/* splint realloc */ /*@i1@*/\
	return	HT_FR_SUCCESS;
}

/*
 * Grows (by re-allocating) uix->img_squarecapc.
 *
 * Not allowed to grow past HT_TYPE_MAX.
 */
static
enum HT_FR
uix_img_square_grow (
/*@in@*/
/*@notnull@*/
	struct uix * const	ui_x
	)
/*@modifies * ui_x@*/
{
	struct img_square * squares_new	= NULL;
	unsigned short capc_new = (unsigned short)
		(ui_x->img_squarecapc * HT_UIX_IMGS_CAP_GROWF);
	if (ui_x->img_squarecapc >= HT_TYPE_MAX)
	{
		return	HT_FR_FAIL_ILL_ARG;
	}
	if (capc_new <= ui_x->img_squarecapc)
	{
		capc_new = (unsigned short)(ui_x->img_squarecapc + 1);
	}
	if (capc_new > HT_TYPE_MAX)
	{
		capc_new	= HT_TYPE_MAX;
	}

/* splint realloc */ /*@i4@*/\
	squares_new = realloc(ui_x->img_squares,
		sizeof(* ui_x->img_squares) * capc_new);
	if (NULL == squares_new)
	{
/* splint realloc */ /*@i2@*/\
		return	HT_FR_FAIL_ALLOC;
	}
	ui_x->img_squares	= squares_new;
	ui_x->img_squarecapc	= capc_new;

/* splint realloc */ /*@i1@*/\
	return	HT_FR_SUCCESS;
}

static
enum HT_FR
uix_init_data_img_pieces (
/*@in@*/
/*@notnull@*/
	const struct game * const	g,
/*@in@*/
/*@notnull@*/
	struct uix * const		ui_x
	)
/*@modifies * ui_x@*/
{
	GC		gc_piece	= NULL;
	enum HT_FR	fr		= HT_FR_NONE;
	unsigned short	i;
#ifndef HT_UNSAFE
	if (NULL == g || NULL == ui_x)
	{
		return	HT_FR_FAIL_NULLPTR;
	}
#endif

	for (i = (unsigned short)0; i < g->rules->type_piecec; i++)
	{
		if (ui_x->img_piecec >= ui_x->img_piececapc)
		{
			fr	= uix_img_piece_grow(ui_x);
			if (HT_FR_SUCCESS != fr)
			{
				unsigned short j;
				for (j = (unsigned short)0; j < i; j++)
				{
/*@i2@*/				XDestroyImage(
/*@i@*/					ui_x->img_pieces[j].img);
/*@i2@*/				ui_x->img_pieces[j].img = NULL;
					if (ui_x->img_pieces[j].transp
					&& NULL != ui_x->display)
					{
/*@i2@*/					(void) XFreePixmap
							(ui_x->display,
							ui_x->img_pieces
							[j].clipmask);
					}
				}
				ui_x->img_piecec	= 0;
				if (NULL != gc_piece)
				{
/*@i2@*/				(void) XFreeGC(ui_x->display,
						gc_piece);
				}
				return			fr;
			}
#ifndef HT_UNSAFE
			fr	= HT_FR_NONE;
#endif
		}

		ui_x->img_pieces[i].bit = g->rules->type_pieces[i].bit;

		fr = get_ximage_piece(ui_x, g->rules->path,
/*@i2@*/		& ui_x->img_pieces[i], & gc_piece);
		if (HT_FR_SUCCESS != fr)
		{
			unsigned short j;
			for (j = (unsigned short)0; j < i; j++)
			{
/*@i5@*/			XDestroyImage(ui_x->img_pieces[j].img);
/*@i2@*/			ui_x->img_pieces[j].img = NULL;
				if (ui_x->img_pieces[j].transp
				&& NULL != ui_x->display)
				{
/*@i2@*/				(void) XFreePixmap(ui_x->
						display, ui_x->
						img_pieces[j].clipmask);
				}
			}
			ui_x->img_piecec	= 0;
			if (NULL != gc_piece)
			{
/*@i2@*/			(void) XFreeGC(ui_x->display, gc_piece);
			}
			return			fr;
		}
#ifndef HT_UNSAFE
		fr	= HT_FR_NONE;
#endif
		ui_x->img_piecec++;
	}

	if (NULL != gc_piece)
	{
/*@i2@*/	(void) XFreeGC(ui_x->display, gc_piece);
	}
	return	HT_FR_SUCCESS;
}

/*
 * Reads all ui_x->img_squares XImages from g->rules->type_squares.
 *
 * The images are looked for in g->rules->path. See get_ximage_square.
 */
static
enum HT_FR
uix_init_data_img_squares (
/*@in@*/
/*@notnull@*/
	const struct game * const	g,
/*@in@*/
/*@notnull@*/
	struct uix * const		ui_x
	)
/*@modifies * ui_x@*/
{
	enum HT_FR	fr = HT_FR_NONE;
	unsigned short	i;
#ifndef HT_UNSAFE
	if (NULL == g || NULL == ui_x)
	{
		return	HT_FR_FAIL_NULLPTR;
	}
#endif

	for (i = (unsigned short)0; i < g->rules->type_squarec; i++)
	{
		if (ui_x->img_squarec >= ui_x->img_squarecapc)
		{
			fr	= uix_img_square_grow(ui_x);
			if (HT_FR_SUCCESS != fr)
			{
				unsigned short j;
				for (j = (unsigned short)0; j < i; j++)
				{
/*@i2@*/				XDestroyImage(
/*@i@*/					ui_x->img_squares[j].img);
/*@i2@*/				ui_x->img_squares[j].img = NULL;
				}
				ui_x->img_squarec	= 0;
				return			fr;
			}
#ifndef HT_UNSAFE
			fr	= HT_FR_NONE;
#endif
		}

		ui_x->img_squares[i].bit =
			g->rules->type_squares[i].bit;

		fr = get_ximage_square(ui_x, g->rules->path,
			ui_x->img_squares[i].bit,
/*@i2@*/		& ui_x->img_squares[i].img);
		if (HT_FR_SUCCESS != fr)
		{
			unsigned short j;
			for (j = (unsigned short)0; j < i; j++)
			{
/*@i5@*/			XDestroyImage(ui_x->img_squares[j].img);
/*@i2@*/			ui_x->img_squares[j].img = NULL;
			}
			ui_x->img_squarec	= 0;
			return			fr;
		}
#ifndef HT_UNSAFE
		fr	= HT_FR_NONE;
#endif
		ui_x->img_squarec++;
	}

	return	HT_FR_SUCCESS;
}

/*
 * Initializes the uix struct.
 *
 * Creates the window, but does not map (or configure) it.
 */
static
enum HT_FR
uix_init_data (
/*@in@*/
/*@notnull@*/
	const struct game * const	g,
/*@in@*/
/*@notnull@*/
	struct uix * const		ui_x
	)
/*@modifies * ui_x@*/
{
	enum HT_FR fr	= HT_FR_NONE;

	uix_action_reset(ui_x);
	ui_x->pos_sel	= HT_BOARDPOS_NONE;

	ui_x->wmarg_top = ui_x->wmarg_left = ui_x->wmarg_right =
		(unsigned int)0;
	ui_x->wmarg_bottom = (unsigned int)2;

	/*
	 * Use DISPLAY environment variable (NULL).
	 */
/*@i3@*/ui_x->display	= XOpenDisplay(NULL);
	if (NULL == ui_x->display)
	{
		return	HT_FR_FAIL_UIX_DISPLAY;
	}

/*@i1@*/ui_x->screen	= XDefaultScreen(ui_x->display);
/*@i1@*/ui_x->px_white	= XWhitePixel(ui_x->display, ui_x->screen);
/*@i1@*/ui_x->px_black	= XBlackPixel(ui_x->display, ui_x->screen);

/*@i1@*/ui_x->window	= XCreateSimpleWindow(ui_x->display,
/*@i1@*/	XDefaultRootWindow(ui_x->display), 0, 0,
		(unsigned int)1, (unsigned int)1, 0, 0, 0);

/*@i3@*/ui_x->gc = XCreateGC(ui_x->display, ui_x->window, 0l, NULL);

	fr	= gc_configure(ui_x, ui_x->gc);
	if (HT_FR_SUCCESS != fr)
	{
		return	fr;
	}
#ifndef HT_UNSAFE
	fr	= HT_FR_NONE;
#endif

	fr	= uix_init_data_img_squares(g, ui_x);
	if (HT_FR_SUCCESS != fr)
	{
		return	fr;
	}
#ifndef HT_UNSAFE
	fr	= HT_FR_NONE;
#endif

	fr	= uix_init_data_img_pieces(g, ui_x);
	if (HT_FR_SUCCESS != fr)
	{
		return	fr;
	}
#ifndef HT_UNSAFE
	fr	= HT_FR_NONE;
#endif

	return	HT_FR_SUCCESS;
}

static
enum HT_FR
uix_init_window_name (
/*@in@*/
/*@notnull@*/
	struct uix * const		ui_x
	)
/*@modifies nothing@*/
{
	char *		winname;
	size_t		winname_l	= strlen(HT_PROGNAME);
	XTextProperty	textprop;
#ifndef HT_UNSAFE
	if (NULL == ui_x)
	{
		return	HT_FR_FAIL_NULLPTR;
	}
#endif

	winname	= malloc(sizeof(* winname) * (winname_l + 1));
	if (NULL == winname)
	{
		return	HT_FR_FAIL_ALLOC;
	}

	strncpy(winname, HT_PROGNAME, (winname_l + 1));

/*@i2@*/if (0 == XStringListToTextProperty(& winname, 1, & textprop))
	{
		free	(winname);
		return	HT_FR_FAIL_UIX;
	}
/*@i3@*/XSetWMName	(ui_x->display, ui_x->window, & textprop);
/*@i3@*/XSetWMIconName	(ui_x->display, ui_x->window, & textprop);

/*@i3@*/(void) XFree	(textprop.value);
	free		(winname);
/*@i1@*/return		HT_FR_SUCCESS;
}

static
enum HT_FR
uix_init_window (
/*@in@*/
/*@notnull@*/
	const struct game * const	g,
/*@in@*/
/*@notnull@*/
	struct uix * const		ui_x
	)
/*@modifies * ui_x@*/
{
	enum HT_FR	fr	= HT_FR_NONE;
#ifndef HT_UNSAFE
	if (NULL == g || NULL == ui_x)
	{
		return	HT_FR_FAIL_NULLPTR;
	}
#endif

	fr	= uix_init_window_name(ui_x);
	if (HT_FR_SUCCESS != fr)
	{
		return	fr;
	}
#ifndef HT_UNSAFE
	fr	= HT_FR_NONE;
#endif

	{
		unsigned int	wprf, hprf;
		{
			fr	= size_update(g, ui_x, NULL);
			if (HT_FR_SUCCESS != fr)
			{
				return	fr;
			}
#ifndef HT_UNSAFE
			fr	= HT_FR_NONE;
#endif
		}

		wprf	= (unsigned int)(g->rules->bwidth * ui_x->sq_w
			+ (int)ui_x->wmarg_left +
			(int)ui_x->wmarg_right);
		hprf	= (unsigned int)(g->rules->bheight * ui_x->sq_h
			+ (int)ui_x->wmarg_top +
			(int)ui_x->wmarg_bottom);
/*@i1@*/	if (0 == XResizeWindow(ui_x->display, ui_x->window,
			wprf, hprf))
		{
			return	HT_FR_FAIL_UIX;
		}
	}

/*@i1@*/if (0 == XMapWindow(ui_x->display, ui_x->window))
	{
		return	HT_FR_FAIL_UIX;
	}

/*@i1@*/if (0 == XSelectInput(ui_x->display, ui_x->window,
		StructureNotifyMask))
	{
		return	HT_FR_FAIL_UIX;
	}

	do
	{
/*@i1@*/	if (1 == XNextEvent(ui_x->display, ui_x->event))
		{
			return	HT_FR_FAIL_UIX;
		}
	} while (MapNotify != ui_x->event->type);

/*@i1@*/if (0 == XSelectInput(ui_x->display, ui_x->window, NoEventMask))
	{
		return	HT_FR_FAIL_UIX;
	}

	return	HT_FR_SUCCESS;
}

enum HT_FR
uix_init (
	const struct game * const	g,
	struct uix * const		ui_x
	)
{
	enum HT_FR fr	= uix_init_data(g, ui_x);
	if (HT_FR_SUCCESS != fr)
	{
		return	fr;
	}
#ifndef HT_UNSAFE
	fr	= HT_FR_NONE;
#endif

	fr	= uix_init_window(g, ui_x);
	if (HT_FR_SUCCESS != fr)
	{
		return	fr;
	}
#ifndef HT_UNSAFE
	fr	= HT_FR_NONE;
#endif

	/*
	 * In case some image couldn't be loaded, synchronize before
	 * continuing to catch the error now.
	 */
/*@i1@*/if (0 == XSync(ui_x->display, True))
	{
		return	HT_FR_FAIL_UIX;
	}
	return	HT_FR_SUCCESS;
}

#endif

