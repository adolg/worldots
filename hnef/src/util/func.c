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
#include "func.h"
#include "lang.h"	/* HT_L_* */

/*
 * Reason for why reading invoc from argc and argv failed.
 */
const char *
fail_invoc_what (
	const enum HT_FAIL_INVOC code
	)
{
	switch (code)
	{
		case HT_FAIL_INVOC_UNK:
			return HT_L_FAIL_INVOC_UNK;
		case HT_FAIL_INVOC_UI_NONE:
			return HT_L_FAIL_INVOC_UI_NONE;
		case HT_FAIL_INVOC_UI_UNK:
			return HT_L_FAIL_INVOC_UI_UNK;
		case HT_FAIL_INVOC_RC_NONE:
			return HT_L_FAIL_INVOC_RC_NONE;
		case HT_FAIL_INVOC_NONE:
		default:
			return HT_L_FR_UNKNOWN;
	}
}

/*
 * Reason for why reading a ruleset file from invoc->file_rc failed.
 */
const char *
fail_rreader_what (
	const enum HT_FAIL_RREADER code
	)
{
	switch (code)
	{
		case HT_FAIL_RREADER_ARG_DUP:
			return HT_L_FAIL_RREADER_ARG_DUP;
		case HT_FAIL_RREADER_ARG_MISS:
			return HT_L_FAIL_RREADER_ARG_MISS;
		case HT_FAIL_RREADER_ARG_EXCESS:
			return HT_L_FAIL_RREADER_ARG_EXCESS;
		case HT_FAIL_RREADER_ARG_UNK:
			return HT_L_FAIL_RREADER_ARG_UNK;
		case HT_FAIL_RREADER_TYPE_DUP:
			return HT_L_FAIL_RREADER_TYPE_DUP;
		case HT_FAIL_RREADER_TYPE_ORDER:
			return HT_L_FAIL_RREADER_TYPE_ORDER;
		case HT_FAIL_RREADER_TYPE_OVERFLOW:
			return HT_L_FAIL_RREADER_TYPE_OVERFLOW;
		case HT_FAIL_RREADER_TYPE_UNDEF:
			return HT_L_FAIL_RREADER_TYPE_UNDEF;
		case HT_FAIL_RREADER_NOSIZE:
			return HT_L_FAIL_RREADER_NOSIZE;
		case HT_FAIL_RREADER_INCOMP:
			return HT_L_FAIL_RREADER_INCOMP;
		case HT_FAIL_RREADER_STRTOL:
			return HT_L_FAIL_GEN_STRTOL;
		case HT_FAIL_RREADER_NOOB:
			return HT_L_FAIL_GEN_NOOB;
		case HT_FAIL_RREADER_BIT_SINGLE:
			return HT_L_FAIL_GEN_BIT_SINGLE;
		case HT_FAIL_RREADER_NONE:
		default:
			return HT_L_FR_UNKNOWN;
	}
}

HT_BOOL
fr_fail_invoc (
	const enum HT_FR code
	)
{
	return HT_FR_FAIL_INVOC == code;
}

HT_BOOL
fr_fail_rreader (
	const enum HT_FR code
	)
{
	return HT_FR_FAIL_RREADER == code;
}

/*
 * The following values will return a nonsense string because you are
 * not supposed to give them to this function:
 *
 * *	HT_FR_NONE		Undetermined / uninitialized.
 *
 * *	HT_FR_SUCCESS		Not a failure.
 *
 * *	HT_FR_FAIL_INVOC	fr_fail_invoc -> fail_invoc_what
 */
const char *
fr_fail_what (
	const enum HT_FR code
	)
{
	switch (code)
	{
		case HT_FR_FAIL_ALLOC:
			return HT_L_FR_FAIL_ALLOC;
		case HT_FR_FAIL_NULLPTR:
			return HT_L_FR_FAIL_NULLPTR;
		case HT_FR_FAIL_ARROB:
			return HT_L_FR_FAIL_ARROB;
		case HT_FR_FAIL_ENUM_UNK:
			return HT_L_FR_FAIL_ENUM_UNK;
		case HT_FR_FAIL_ILL_ARG:
			return HT_L_FR_FAIL_ILL_ARG;
		case HT_FR_FAIL_ILL_STATE:
			return HT_L_FR_FAIL_ILL_STATE;
		case HT_FR_FAIL_IO_ENV_UNDEF:
			return HT_L_FR_FAIL_IO_ENV_UNDEF;
		case HT_FR_FAIL_IO_FILE_R:
			return HT_L_FR_FAIL_IO_FILE_R;
		case HT_FR_FAIL_IO_PRINT:
			return HT_L_FR_FAIL_IO_PRINT;
		case HT_FR_FAIL_UI_NONE:
			return HT_L_FR_FAIL_UI_NONE;
#ifdef HT_POSIX
		case HT_FR_FAIL_POSIX:
			return HT_L_FR_FAIL_POSIX;
#endif
#ifdef HT_UI_CLI
		case HT_FR_FAIL_IO_STDIN:
			return HT_L_FR_FAIL_IO_STDIN;
#endif
#ifdef HT_UI_XLIB
		case HT_FR_FAIL_UIX:
			return HT_L_FR_FAIL_UIX;
		case HT_FR_FAIL_UIX_DISPLAY:
			return HT_L_FR_FAIL_UIX_DISPLAY;
		case HT_FR_FAIL_UIX_XPM_FILER:
			return HT_L_FR_FAIL_UIX_XPM_FILER;
#endif
/*
#ifdef HT_UI_MSQU
		case HT_FR_FAIL_IO:
			return HT_L_FR_FAIL_IO;
#endif
*/
		case HT_FR_NONE:		/* Uninitialized. */
		case HT_FR_SUCCESS:		/* Not a failure. */
		case HT_FR_FAIL_INVOC:		/* fail_invoc_what */
		case HT_FR_FAIL_RREADER:	/* fail_rreader_what */
		default:
			return HT_L_FR_UNKNOWN;
	}
}

