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
#include "config.h"

/*
 * 1 for present or 0 for not present, for every preprocessor macro in
 * the following order:
 *
 *	HT_C99
 *	HT_POSIX
 *	HT_UNSAFE
 *	HT_AI_MINIMAX
 *	HT_AI_MINIMAX_HASH
 *	HT_AI_MINIMAX_UNSAFE
 *	HT_UI_CLI
 *	HT_UI_XLIB
 *	HT_UI_MSQU
 *
 * This string gives no clue about (other than that they must have at
 * least the minimum allowed values if HT_AI_MINIMAX_HASH is 1):
 *
 *	HT_AI_MINIMAX_HASH_MEM_TAB
 *	HT_AI_MINIMAX_HASH_MEM_COL
 */
const char * HT_PREPROC =
#ifdef HT_C99
	"1"
#else
	"0"
#endif
#ifdef HT_POSIX
	"1"
#else
	"0"
#endif
#ifdef HT_UNSAFE
	"1"
#else
	"0"
#endif
#ifdef HT_AI_MINIMAX
	"1"
#else
	"0"
#endif
#ifdef HT_AI_MINIMAX_HASH
	"1"
#else
	"0"
#endif
#ifdef HT_AI_MINIMAX_UNSAFE
	"1"
#else
	"0"
#endif
#ifdef HT_UI_CLI
	"1"
#else
	"0"
#endif
#ifdef HT_UI_XLIB
	"1"
#else
	"0"
#endif
#ifdef HT_UI_MSQU
	"1"
#else
	"0"
#endif
;

