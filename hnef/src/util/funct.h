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
#ifndef HT_FUNC_T_H
#define HT_FUNC_T_H

/*
 * Fail detail for HT_FR_FAIL_INVOC.
 *
 * Why the ruleset file (indicated by invoc->file_rc) is bad.
 */
enum HT_FAIL_RREADER
{

	/*
	 * Uninitialized (or non-failure) value.
	 */
	HT_FAIL_RREADER_NONE	= 0,

	/*
	 * Some argument was (apparently) encountered twice. The ruleset
	 * reader may be able to figure this out by determining that a
	 * value has already been set, when it expects the value to not
	 * have been set.
	 */
	HT_FAIL_RREADER_ARG_DUP,

	/*
	 * Some argument is missing.
	 */
	HT_FAIL_RREADER_ARG_MISS,

	/*
	 * Encountered more arguments than expected.
	 */
	HT_FAIL_RREADER_ARG_EXCESS,

	/*
	 * Some argument is not recognized.
	 *
	 * This will probably happen a lot when people are translating
	 * "lang.c" but forget to translate the entries in the ruleset
	 * files.
	 */
	HT_FAIL_RREADER_ARG_UNK,

	/*
	 * A type_piece, type_square or player has been defined twice.
	 */
	HT_FAIL_RREADER_TYPE_DUP,

	/*
	 * Wrong declaration order.
	 *
	 * This happens if type_piece or type_square are not declared in
	 * the right order (from 1 to 128).
	 */
	HT_FAIL_RREADER_TYPE_ORDER,

	/*
	 * Too many type_pieces, type_squares or players defined.
	 */
	HT_FAIL_RREADER_TYPE_OVERFLOW,

	/*
	 * A type_piece, type_square or player has not been defined
	 * before being referred to.
	 */
	HT_FAIL_RREADER_TYPE_UNDEF,

	/*
	 * ruleset->width and ruleset->height were not set before other
	 * arguments that depend on them (pieces, squares, et c.) were
	 * encountered.
	 */
	HT_FAIL_RREADER_NOSIZE,

	/*
	 * The provided ruleset file does not give enough information to
	 * construct a game struct.
	 */
	HT_FAIL_RREADER_INCOMP,

	/*
	 * Some string could not be converted to a number.
	 */
	HT_FAIL_RREADER_STRTOL,

	/*
	 * Some number was out of bounds.
	 */
	HT_FAIL_RREADER_NOOB,

	/*
	 * Some number was not a single bit.
	 */
	HT_FAIL_RREADER_BIT_SINGLE

};

/*
 * Fail detail for HT_FR_FAIL_INVOC.
 *
 * Why some invocation parameter is bad.
 */
enum HT_FAIL_INVOC
{

	/*
	 * Uninitialized (or non-failure) value.
	 */
	HT_FAIL_INVOC_NONE	= 0,

	/*
	 * Unknown switch.
	 */
	HT_FAIL_INVOC_UNK,

	/*
	 * Failed to set invoc->info.ui because no parameter was given.
	 */
	HT_FAIL_INVOC_UI_NONE,

	/*
	 * Failed to set invoc->info.ui because the parameter was not
	 * recognized.
	 */
	HT_FAIL_INVOC_UI_UNK,

	/*
	 * Failed to set invoc->file_rc.
	 */
	HT_FAIL_INVOC_RC_NONE

};

/*
 * Generic failure code.
 *
 * This may be accompanied by more information, specifically the enums
 * in this file (the reason is because otherwise this enum would be huge
 * and the switch statement would be too big):
 *
 * HT_FAIL_*
 *
 * Whether it's fatal for the program depends on the type of error.
 */
enum HT_FR
{

	/*
	 * Initialize temporary variables to this value before (and
	 * after) passing them to a function.
	 */
	HT_FR_NONE	= -1,

	/*
	 * No error. You can check "if (HT_FR_SUCCESS != return_value)"
	 * to determine if a function failed.
	 */
	HT_FR_SUCCESS	= 0,

	/*
	 * NULL encountered when not allowed.
	 */
	HT_FR_FAIL_NULLPTR,

	/*
	 * Array out of bounds.
	 */
	HT_FR_FAIL_ARROB,

	/*
	 * Any dynamic memory allocation (malloc, calloc, realloc)
	 * failed.
	 *
	 * If a function returns an allocated value, which will be NULL
	 * upon failure to allocate, and also returns an HT_FR value,
	 * then that value sould be HT_FR_FAIL_ALLOC even though NULL
	 * means failed allocation. This only applies if that function
	 * happens to return an HT_FR value as well -- otherwise it's
	 * fine to just return NULL like malloc.
	 */
	HT_FR_FAIL_ALLOC,

	/*
	 * Default case in enum switch (or similar) reached.
	 */
	HT_FR_FAIL_ENUM_UNK,

	/*
	 * Argument passed to function is illegal somehow.
	 */
	HT_FR_FAIL_ILL_ARG,

	/*
	 * Program's state is invalid in some way.
	 */
	HT_FR_FAIL_ILL_STATE,

	/*
	 * An environment variable was not found.
	 */
	HT_FR_FAIL_IO_ENV_UNDEF,

	/*
	 * Failed to read file for some reason -- like, the file doesn't
	 * exist or can't be opened.
	 */
	HT_FR_FAIL_IO_FILE_R,

	/*
	 * Failed to print to stdout using any function (puts, printf
	 * or other). Also used for stderr failures and string
	 * formatting.
	 */
	HT_FR_FAIL_IO_PRINT,

#ifdef HT_POSIX
	/*
	 * Any POSIX failure. errno will probably convey further
	 * information to the user.
	 */
	HT_FR_FAIL_POSIX,
#endif

#ifdef HT_UI_CLI
	/*
	 * Failed to read from stdin using any function.
	 */
	HT_FR_FAIL_IO_STDIN,
#endif

#ifdef HT_UI_MSQU
	/*
	 * Failed to read from a file using any function.
	 */
	HT_FR_FAIL_IO,
#endif
	/*
	 * Invocation failed for some reason.
	 *
	 * This is fatal for the program since some invocation parameter
	 * does not work.
	 *
	 * More information carried by HTFAIL_INVOC.
	 */
	HT_FR_FAIL_INVOC,

	/*
	 * Ruleset file is malformed.
	 *
	 * This is fatal for the program since it will fail to create a
	 * game struct.
	 *
	 * More information carried by HTFAIL_RREADER.
	 */
	HT_FR_FAIL_RREADER,

	/*
	 * No interface compiled in.
	 */
	HT_FR_FAIL_UI_NONE

#ifdef HT_UI_XLIB
	/*
	 * XLib failed for some reason.
	 *
	 * This is fatal for the program since XLib fails.
	 */
	,
	HT_FR_FAIL_UIX,

	/*
	 * XOpenDisplay failed, probably because X is not running.
	 */
	HT_FR_FAIL_UIX_DISPLAY,

	/*
	 * XpmReadFileToImage failed, or the file didn't exist (even
	 * though the reading function didn't fail).
	 */
	HT_FR_FAIL_UIX_XPM_FILER
#endif

#ifdef HT_UI_MSQU
	,
	/*
	 * Some PPAPI failure.
	 */
	HT_FR_FAIL_UIMQ_API
#endif
};

#endif
