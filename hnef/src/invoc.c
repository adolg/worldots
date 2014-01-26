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
#include <stdlib.h>	/* NULL */
#include <string.h>	/* strlen */

#include "config.h"	/* HT_RSTR */
#include "lang.h"	/* HT_L_* */
#include "invoc.h"
#include "ui.h"		/* ui_def */

/*
 * Helper for invoc_read to read inv->info.ui argument. i is the
 * position in argv where the next argument is expected to be, but i may
 * be out of bounds (exceed argc).
 *
 * inv->info.play_ui is set to an enum HT_UIT upon success, or
 * HT_UIT_NONE upon any failure.
 */
static
enum HT_FR
invoc_read_ui (
	const size_t				argc,
/*@in@*/
/*@notnull@*/
/*@observer@*/
	const char * *				argv,
/*@in@*/
/*@notnull@*/
	struct invoc * const HT_RSTR		inv,
	const size_t				i,
/*@in@*/
/*@notnull@*/
	enum HT_FAIL_INVOC * const HT_RSTR	invfail
	)
/*@modifies inv->info, * invfail@*/
{
#ifndef HT_UNSAFE
	if (NULL == argv || NULL == inv || NULL == invfail)
	{
		if (NULL != inv)
		{
			inv->info.play_ui	= HT_UIT_NONE;
		}
		return HT_FR_FAIL_NULLPTR;
	}
#endif

	/*
	 * Default value:
	 */
	inv->info.play_ui	= HT_UIT_NONE;

	if (i < argc)
	{
		const char * const	argn	= argv[i];
		char			uisym;
		if (strlen(argn) < (size_t)1)
		{
			* invfail	= HT_FAIL_INVOC_UI_NONE;
			return		HT_FR_FAIL_INVOC;
		}
		uisym			= argn[0];
#ifdef HT_UI_CLI
		if (HT_L_INV_S_UI_CLI == uisym)
		{
			inv->info.play_ui	= HT_UIT_CLI;
		}
		else
#endif
#ifdef HT_UI_XLIB
		if (HT_L_INV_S_UI_XLIB == uisym)
		{
			inv->info.play_ui	= HT_UIT_XLIB;
		}
		else
#endif
#ifdef HT_UI_MSQU
		if (HT_L_INV_S_UI_MSQU == uisym)
		{
			inv->info.play_ui	= HT_UIT_MSQU;
		}
		else
#endif
		{
			* invfail		= HT_FAIL_INVOC_UI_UNK;
			return			HT_FR_FAIL_INVOC;
		}
		return		HT_FR_SUCCESS;
	}
	else
	{
		* invfail	= HT_FAIL_INVOC_UI_NONE;
		return		HT_FR_FAIL_INVOC;
	}
}

/*
 * Helper for invoc_read to read inv->file_rc argument. i is the
 * position in argv where the next argument is expected to be, but i may
 * be out of bounds (exceed argc).
 *
 * inv->file_rc is non-NULL upon success, or NULL upon failure.
 */
static
enum HT_FR
invoc_read_rc (
	const size_t				argc,
/*@in@*/
/*@notnull@*/
/*@observer@*/
	const char * *				argv,
/*@in@*/
/*@notnull@*/
	struct invoc * const HT_RSTR		inv,
	const size_t				i,
/*@in@*/
/*@notnull@*/
	enum HT_FAIL_INVOC * const HT_RSTR	invfail
	)
/*@modifies inv->file_rc, * invfail@*/
{
#ifndef HT_UNSAFE
	if (NULL == argv || NULL == inv || NULL == invfail)
	{
		if (NULL != inv)
		{
			inv->file_rc	= "";
		}
		return HT_FR_FAIL_NULLPTR;
	}
#endif

	/*
	 * Default value:
	 */
	inv->file_rc	= "";

	if (i < argc)
	{
		const char * const argn	= argv[i];
		if (strlen(argn) < (size_t)1)
		{
			* invfail	= HT_FAIL_INVOC_RC_NONE;
			return		HT_FR_FAIL_INVOC;
		}
		inv->file_rc	= argn;

		return HT_FR_SUCCESS;
	}
	else
	{
		* invfail	= HT_FAIL_INVOC_RC_NONE;
		return		HT_FR_FAIL_INVOC;
	}
}

/*
 * Helper for invoc_read. Reads a switch (argv[i]).
 *
 * i is the iterator for the loop. It's already advanced properly, but
 * this function may need to advance it further to check the next
 * argument.
 *
 * inv and invfail are set appropriately.
 */
static
enum HT_FR
invoc_read_arg (
	const size_t			argc,
/*@in@*/
/*@notnull@*/
/*@observer@*/
	const char * *			argv,
/*@in@*/
/*@notnull@*/
	struct invoc * const		inv,
/*@notnull@*/
	size_t * const			i,
/*@in@*/
/*@notnull@*/
	enum HT_FAIL_INVOC * const	invfail
	)
/*@modifies * inv, * i, * invfail@*/
{
	const char * const	arg	= argv[* i];
	char			argsw;

#ifndef HT_UNSAFE
	if (NULL == argv || NULL == inv || NULL == i || NULL == invfail)
	{
		return HT_FR_FAIL_NULLPTR;
	}
#endif

	if (strlen(arg) < (size_t)2 || '-' != arg[0])
	{
		* invfail	= HT_FAIL_INVOC_UNK;
		return		HT_FR_FAIL_INVOC;
	}
	argsw	= arg[1];

	if (HT_L_INV_S_HELP == argsw)
	{
		inv->info.nplay_print	= HT_INV_PRINT_HELP;
		inv->play		= HT_FALSE;
	}
	else if (HT_L_INV_S_NINE == argsw)
	{
		inv->info.nplay_print	= HT_INV_PRINT_NINE;
		inv->play		= HT_FALSE;
	}
	else if (HT_L_INV_S_VERSION == argsw)
	{
		inv->info.nplay_print	= HT_INV_PRINT_VERSION;
		inv->play		= HT_FALSE;
	}
	else if (HT_L_INV_S_UI == argsw)
	{
		/*
		 * inv->play
		 */
		const enum HT_FR fr = invoc_read_ui(argc, argv, inv,
			++(* i), invfail);
		if (HT_FR_SUCCESS != fr)
		{
			/*
			 * invfail is set.
			 */
			return fr;
		}
	}
	else if (HT_L_INV_S_RUNCOM == argsw)
	{
		/*
		 * inv->play
		 */
		const enum HT_FR fr = invoc_read_rc(argc, argv, inv,
			++(* i), invfail);
		if (HT_FR_SUCCESS != fr)
		{
			/*
			 * invfail is set.
			 */
			return fr;
		}
	}
	else
	{
		* invfail	= HT_FAIL_INVOC_UNK;
		return		HT_FR_FAIL_INVOC;
	}
	return 			HT_FR_SUCCESS;
}

/*
 * argc and argv are from main.
 *
 * invfail is set to something else than HT_FAIL_INVOC_NONE if this
 * function fails because there is something wrong with the invocation
 * arguments in argv. If so, it returns HT_FR_FAIL_INVOC (which you can
 * pass to htfail_invoc_what).
 *
 * Other HT_FR_FAIL_* values (which you can pass to htfr_fail_what) are
 * returned for failures that are not related to argv.
 *
 * inv is set by this function. inv->file_rc is NULL if this function
 * fails.
 *
 * If inv->play, then inv->info.play_ui is set to the default interface.
 * Which that is depends:
 *
 * 1.	If HT_UI_CLI, then it defaults to HT_UIT_CLI.
 *
 * 2.	Else if HT_UI_XLIB, then it defaults to HT_UIT_XLIB.
 *
 * 3.	Else it's a failure because then there's no interface.
 */
enum HT_FR
invoc_read (
	const size_t			argc,
	const char * *			argv,
	struct invoc * const		inv,
	enum HT_FAIL_INVOC * const	invfail
	)
{
	size_t i;
	enum HT_FR fr	= HT_FR_NONE;

#ifndef HT_UNSAFE
	if (NULL == argv || NULL == inv || NULL == invfail)
	{
		if (NULL != inv)
		{
			inv->info.nplay_print	= HT_INV_PRINT_HELP;
			inv->file_rc		= "";
			inv->play		= HT_FALSE;
		}
		return HT_FR_FAIL_NULLPTR;
	}
#endif

	/*
	 * If !inv->play, return immediately to print whatever.
	 * Default values:
	 */
	* invfail		= HT_FAIL_INVOC_NONE;
	inv->info.nplay_print	= HT_INV_PRINT_HELP;
	inv->file_rc		= "";
	inv->play		= HT_TRUE;

	for (i = (size_t)1; i < argc && inv->play; i++)
	{
		fr	= invoc_read_arg(argc, argv, inv, & i, invfail);
		if (HT_FR_SUCCESS != fr)
		{
			return fr;
		}
#ifndef HT_UNSAFE
		fr	= HT_FR_NONE;
#endif
	}

	if (inv->play && HT_UIT_NONE == inv->info.play_ui)
	{
		/*
		 * Set default interface, if one can be found.
		 */
		inv->info.play_ui	= ui_def();
		if (HT_UIT_NONE == inv->info.play_ui)
		{
			return	HT_FR_FAIL_UI_NONE;
		}
	}

	return		HT_FR_SUCCESS;
}

