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
#ifndef HT_CONFIG_H
#define HT_CONFIG_H

/*
 * NOTE:	Version should be defined as a number in `config.mk`. It
 *		should be in the format "yymmdd", for example 990930.
 *
 *		Refuse to compile without it!
 */
#ifndef	HT_VERSION
#error	"HT_VERSION not defined."
#endif

/*
 * NOTE:	_POSIX_C_SOURCE should be defined for HT_POSIX, and
 *		__STDC_VERSION__ >= 199901L for HT_C99. We don't check
 *		for this, but trust that the user will define them if
 *		appropriate.
 */

/*
 * Default rc file, if no such invocation switch is given (or if it
 * can't be found).
 *
 * This is a relative path that will later be resolved against
 * getenv("HOME").
 *
 * NOTE:	Default RC file should be defined as a string literal in
 *		`config.mk`.
 *
 *		Refuse to compile without it!
 */
#ifndef HT_FILE_RC_DEF
#error	"HT_FILE_RC_DEF not defined."
#endif

/*
 * Absolute path to default ruleset file. HT_FILE_RULE_DEF will be a
 * symlink to this file.
 *
 * NOTE:	Default ruleset file should be defined as a string
 *		literal in `config.mk`.
 *
 *		Refuse to compile without it!
 */
#ifndef	HT_FILE_RULE_DEF
#error	"HT_FILE_RULE_DEF not defined."
#endif

/*
 * restrict was added in C99.
 */
#ifdef	HT_C99
#define	HT_RSTR	restrict
#else
#define	HT_RSTR
#endif

/*
 * Check that at least one user interface is compiled in. The program
 * will complain and fail to start if you attempt to use it without a
 * user interface anyway.
 */
#if	!defined(HT_UI_CLI)	&& !defined(HT_UI_XLIB) && !defined(HT_UI_MSQU)
#error	"No user interface. Use HT_UI_CLI, HT_UI_XLIB or HT_UI_MSQU."
#endif

/*
 * Disable computer player features if not enabled.
 */
#ifndef	HT_AI_MINIMAX
#undef	HT_AI_MINIMAX_UNSAFE
#undef	HT_AI_MINIMAX_HASH
#endif

/*
 * Undefine memory settings if transposition table is disabled.
 */
#ifndef	HT_AI_MINIMAX_HASH
#undef	HT_AI_MINIMAX_MEM_INIT
#undef	HT_AI_MINIMAX_MEM_COL_INIT
#undef	HT_AI_MINIMAX_MEM_COL_GROW
#undef	HT_AI_MINIMAX_MEM_COL_MAX
#endif

/*
 * Space is given in bytes.
 *
 * *_HASH_MEM_TAB is the space for the hash table.
 *
 * *_HASH_MEM_COL is the space for collision lists.
 */
#ifdef	HT_AI_MINIMAX_HASH
#if	HT_AI_MINIMAX_HASH_MEM_TAB	< 8000000 \
	|| HT_AI_MINIMAX_HASH_MEM_COL	< 8000000
#error	"Insufficient memory for transposition table."\
	"Disable HT_AI_MINIMAX_HASH or use at least 8 + 8 MB."
#endif
#endif
/*@observer@*/
/*@unchecked@*/
extern
const char * HT_PREPROC;

#endif

