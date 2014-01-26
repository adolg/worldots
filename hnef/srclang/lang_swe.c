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
#include "lang.h"

/***********************************************************************
 * Function Return general.
 **********************************************************************/

	const char * HT_L_FR_UNKNOWN = "?";

/***********************************************************************
 * Function Return failure.
 **********************************************************************/

	const char * HT_L_FR_FAIL_ALLOC = "Slut på dynamiskt minne";

	const char * HT_L_FR_FAIL_NULLPTR = "NULL-pekare.";

	const char * HT_L_FR_FAIL_ARROB = "Matrislängd överskriden.";

	const char * HT_L_FR_FAIL_ENUM_UNK = "Okänd enum.";

	const char * HT_L_FR_FAIL_ILL_ARG = "Otillåtet argument.";

	const char * HT_L_FR_FAIL_ILL_STATE = "Otillåtet tillstånd.";

	const char * HT_L_FR_FAIL_IO_ENV_UNDEF =
		"Miljövariabel ej definierad.";

	const char * HT_L_FR_FAIL_IO_FILE_R = "Kunde inte läsa fil.";

	const char * HT_L_FR_FAIL_IO_PRINT = "Kunde inte skriva ut.";

#ifdef HT_POSIX
	const char * HT_L_FR_FAIL_POSIX = "Fel i POSIX.";
#endif

#ifdef HT_UI_CLI
	const char * HT_L_FR_FAIL_IO_STDIN = "Kunde inte läsa stdin.";
#endif

#ifdef HT_UI_MSQU
	const char * HT_L_FR_FAIL_IO = "Kunde inte läsa fil.";
#endif

#ifdef HT_UI_XLIB
	const char * HT_L_FR_FAIL_UIX = "Fel i XLib.";

	const char * HT_L_FR_FAIL_UIX_DISPLAY = "Kan ej öppna display.";

	const char * HT_L_FR_FAIL_UIX_XPM_FILER =
		"Kunde inte öppna XPM-fil.";
#endif

	const char * HT_L_FR_FAIL_UI_NONE =
		"Inget användargränssnitt har kompilerats in.";

/***********************************************************************
 * Generic failure detail.
 **********************************************************************/

	const char * HT_L_FAIL_GEN_STRTOL =
		"Kunde inte omvandla nummer.";

	const char * HT_L_FAIL_GEN_NOOB = "Nummervärde överskridet.";

	const char * HT_L_FAIL_GEN_BIT_SINGLE = "Ej enskild bit.";

/***********************************************************************
 * Function Return failure detail for HT_FR_FAIL_INVOC.
 **********************************************************************/

	const char * HT_L_FAIL_INVOC_UNK = "Okänt startargument.";

	const char * HT_L_FAIL_INVOC_UI_NONE =
		"Startargument för gränssnitt saknas.";

	const char * HT_L_FAIL_INVOC_UI_UNK =
		"Okänt startargument för gränssnitt.";

	const char * HT_L_FAIL_INVOC_RC_NONE =
		"Startargument för regelfil saknas.";

/***********************************************************************
 * Invocation parameter switches.
 *
 * These are automatically preceded by '-'.
 *
 * No two HT_L_INV_S_* values may equal each other, except for
 * HT_L_INV_S_UI_CLI and HT_L_INV_S_UI_XLIB, which may not equal each
 * other, but may equal the other.
 **********************************************************************/

	const char HT_L_INV_S_HELP	= 'h';
	const char HT_L_INV_S_NINE	= '9';
	const char HT_L_INV_S_RUNCOM	= 'r';
	const char HT_L_INV_S_UI	= 'i';
	const char HT_L_INV_S_VERSION	= 'v';
#ifdef HT_UI_CLI
	const char HT_L_INV_S_UI_CLI		= 'c';
#endif
#ifdef HT_UI_XLIB
	const char HT_L_INV_S_UI_XLIB		= 'x';
#endif
#ifdef HT_UI_MSQU
	const char HT_L_INV_S_UI_MSQU		= 'm';
#endif

/***********************************************************************
 * Invocation parameter help.
 *
 * Run program with -HT_L_INV_S_HELP to see how it prints.
 **********************************************************************/

	const char * HT_L_INV_HELP_S_HELP = "Skriv ut hjälp";

	const char * HT_L_INV_HELP_S_RUNCOM = "Regelfil";

	const char * HT_L_INV_HELP_S_VERSION = "Skriv ut version";

	const char * HT_L_INV_HELP_S_UI = "Användargränssnitt";

	const char * HT_L_INV_HELP_S_UI_DEF = " (förval)";

#ifdef HT_UI_CLI
	const char * HT_L_INV_HELP_S_UI_CLI = "Kommandorad";
#endif

#ifdef HT_UI_XLIB
	const char * HT_L_INV_HELP_S_UI_XLIB = "XLib";
#endif

#ifdef HT_UI_MSQU
	const char * HT_L_INV_HELP_S_UI_MSQU = "meddelandekön";
#endif
/***********************************************************************
 * Function Return failure detail for HT_FR_FAIL_RREADER.
 **********************************************************************/

	const char * HT_L_FAIL_RREADER_ARG_DUP = "Dubbelt argument.";

	const char * HT_L_FAIL_RREADER_ARG_MISS = "Argument saknas.";

	const char * HT_L_FAIL_RREADER_ARG_EXCESS =
		"För många argument.";

	const char * HT_L_FAIL_RREADER_ARG_UNK = "Okänt argument.";

	const char * HT_L_FAIL_RREADER_TYPE_DUP = "Dubbla typer.";

	const char * HT_L_FAIL_RREADER_TYPE_ORDER =
		"Typer deklarerade i otillåten ordning.";

	const char * HT_L_FAIL_RREADER_TYPE_OVERFLOW =
		"För många typer definierade.";

	const char * HT_L_FAIL_RREADER_TYPE_UNDEF = "Odefinierad typ.";

	const char * HT_L_FAIL_RREADER_NOSIZE =
		"Storlek ej deklarerad.";

	const char * HT_L_FAIL_RREADER_INCOMP =
		"Ofullständigt regelverk..";

/***********************************************************************
 * Ruleset arguments for rreader.
 *
 * The arguments below, that are grouped together separated by newlines,
 * may not equal each other.
 **********************************************************************/

	const char * HT_L_RREADER_CMD_PATH	= "path";
	const char * HT_L_RREADER_CMD_WIDTH	= "width";
	const char * HT_L_RREADER_CMD_HEIGHT	= "height";
	const char * HT_L_RREADER_CMD_PIECE	= "piece";
	const char * HT_L_RREADER_CMD_PIECES	= "pieces";
	const char * HT_L_RREADER_CMD_FREPEAT	= "forbid_repeat";
	const char * HT_L_RREADER_CMD_SQUARE	= "square";
	const char * HT_L_RREADER_CMD_SQUARES	= "squares";

	const char * HT_L_RREADER_CMDA_BIT	= "bit";

	const char * HT_L_RREADER_CMDA_CAPT_EDGE	= "capt_edge";
	const char * HT_L_RREADER_CMDA_CAPT_LOSS	= "capt_loss";
	const char * HT_L_RREADER_CMDA_CAPT_SIDES	= "capt_sides";
	const char * HT_L_RREADER_CMDA_CAPTURES		= "captures";
	const char * HT_L_RREADER_CMDA_ANVILS		= "anvils";
	const char * HT_L_RREADER_CMDA_HAMMERS		= "hammers";
	const char * HT_L_RREADER_CMDA_CUSTODIAL	= "custodial";
	const char * HT_L_RREADER_CMDA_DBL_TRAP		= "dbl_trap";
	const char * HT_L_RREADER_CMDA_DBL_TRAP_CAPT = "dbl_trap_capt";
	const char * HT_L_RREADER_CMDA_DBL_TRAP_COMPL =
						"dbl_trap_compl";
	const char * HT_L_RREADER_CMDA_DBL_TRAP_EDGE = "dbl_trap_edge";
	const char * HT_L_RREADER_CMDA_DBL_TRAP_ENCL = "dbl_trap_encl";
	const char * HT_L_RREADER_CMDA_ESCAPE		= "escape";
	const char * HT_L_RREADER_CMDA_NORETURN		= "noreturn";
	const char * HT_L_RREADER_CMDA_OCCUPIES		= "occupies";
	const char * HT_L_RREADER_CMDA_TRAVERSES	= "traverses";
	const char * HT_L_RREADER_CMDA_OWNER		= "owner";

/***********************************************************************
 * Ruleset validation strings for invalid rulesets.
 **********************************************************************/

	const char * HT_L_RVALID_GEN = "Felaktig sökväg";

	const char * HT_L_RVALID_PIECEARR =
		"En pjäs börjar på en otillåten ruta.";

	const char * HT_L_RVALID_TYPE_INVALID =
		"Otillåten typ av pjäs eller ruta.";

	const char * HT_L_RVALID_TYPE_UNDEF =
		"Odefinierad pjäs eller ruta påträffades.";

	const char * HT_L_RVALID_PLAYERS =
	"Felaktiga inställningar för spelare eller dubbelfångst.";

	const char * HT_L_RVALID_INITPOS =
		"Partiet är över vid spelets början.";

/***********************************************************************
 * Command line interface general.
 *
 * HT_L_UIC_CMD_PROMPT is not language-related and can be left as it is.
 **********************************************************************/

#ifdef HT_UI_CLI

	const char * HT_L_UIC_CMD_PROMPT = "> ";

	const char * HT_L_UIC_PLAYAGAIN = "Nytt parti?";

/***********************************************************************
 * Command line interface action feedback.
 **********************************************************************/

	const char * HT_L_UIC_ACT_MOVE_ILL_HUMAN = "Otillåtet drag.";

#ifdef HT_AI_MINIMAX
	const char * HT_L_UIC_ACT_MOVE_ILL_AI_UNS =
		"Datorspelaren valde ett otillåtet drag och kopplades "
		"ur.";

	const char * HT_L_UIC_ACT_MOVE_UNK_AI =
		"Datorspelaren hittade inget drag och kopplades ur.";
#endif

	const char * HT_L_UIC_ACT_CONTR_ILL =
		"Otillåtna argument för spelare.";

	const char * HT_L_UIC_ACT_PLAYERS_HUMAN = "Människa";

#ifdef HT_AI_MINIMAX
	const char * HT_L_UIC_ACT_PLAYERS_AIM = "Minimax";
#endif

/***********************************************************************
 * Command line interface command help.
 **********************************************************************/

	const char * HT_L_UIC_HELP_ARG_MOVE =
		"Flytta från x1,y1 till x2,y2.";

	const char * HT_L_UIC_HELP_ARG_CONTR =
		"Ställ in spelare x till typ y med argument z.";

	const char * HT_L_UIC_HELP_ARG_CONTR_NONE = "Människa\t--";

#ifdef HT_AI_MINIMAX
	const char * HT_L_UIC_HELP_ARG_CONTR_MINIMAX = "Minimax\t\t";
#endif

	const char * HT_L_UIC_HELP_ARG_PRINT_PLAYERS =
		"Skriv ut spelare.";

	const char * HT_L_UIC_HELP_ARG_PRINT_BOARD = "Skriv ut bräde.";

	const char * HT_L_UIC_HELP_ARG_QUIT = "Avsluta.";

/***********************************************************************
 * Command line interface commands.
 *
 * No two HT_L_UIC_ARG_* values may equal each other, except for:
 *
 * HT_L_UIC_ARG_CONTR_NONE and HT_L_UIC_ARG_CONTR_MINIMAX may not equal
 * each other, but may equal the other.
 *
 * HT_L_UIC_ARG_Q_* may not equal each other or HT_L_UIC_ARG_QUIT, but
 * may equal the other (but HT_L_UIC_ARG_QUIT may not equal the other).
 * This is because *_QUIT will be interpreted as *_Q_NO, but *_Q_NO will
 * not be interpreted as *_QUIT.
 *
 * HT_L_UIC_ARG_Q_* may equal HT_L_UIC_ARG_CONTR_*.
 *
 * "Print help" doesn't have a char, since help is printed on all
 * unrecognized chars in an "else" block using exactly the same logic.
 * Thus all other chars than the ones defined below work like "print
 * help".
 **********************************************************************/

	const char HT_L_UIC_ARG_CONTR		= 'p';
	const char HT_L_UIC_ARG_MOVE		= 'm';
	const char HT_L_UIC_ARG_PRINT_BOARD	= 'b';
	const char HT_L_UIC_ARG_QUIT		    = 'q';
	const char HT_L_UIC_ARG_Q_YES			= 'y';
	const char HT_L_UIC_ARG_Q_NO			= 'n';
	const char HT_L_UIC_ARG_CONTR_NONE			= 'h';
#ifdef HT_AI_MINIMAX
	const char HT_L_UIC_ARG_CONTR_MINIMAX			= 'm';
#endif

#endif /* HT_UI_CLI */

/***********************************************************************
 * XLib interface general.
 *
 * HT_L_UIX_PLAYAGAIN must reflect HT_L_UIX_ARG_Q_YES and
 * HT_L_UIX_ARG_Q_NO. (Note that HT_L_UIX_ARG_QUIT is equivalent to
 * HT_L_UIX_ARG_Q_NO, but it doesn't have to be reflected by this
 * string).
 **********************************************************************/

#ifdef HT_UI_XLIB

	const char * HT_L_UIX_PLAYAGAIN = "Nytt parti? [y/n]";

/***********************************************************************
 * Command line interface commands.
 *
 * These should all be single ASCII (not UTF-8!) characters.
 *
 * HT_L_UIX_ARG_* may not equal each other, but may equal
 * HT_L_UIX_ARG_Q_*, except for HT_L_UIX_ARG_QUIT AND HT_L_UIX_ARG_Q_NO
 * (which may not equal each other).
 *
 * HT_L_UIC_ARG_CONTR_* may not equal each other.
 **********************************************************************/

	const char * HT_L_UIX_ARG_CONTR		= "p";
#ifdef HT_AI_MINIMAX
	const char * HT_L_UIX_ARG_STOP		= "f";
#endif
	const char * HT_L_UIX_ARG_QUIT		= "q";
	const char * HT_L_UIX_ARG_Q_YES		    = "y";
	const char * HT_L_UIX_ARG_Q_NO		    = "n";
	const char * HT_L_UIX_ARG_CONTR_NONE		= "h";
#ifdef HT_AI_MINIMAX
	const char * HT_L_UIX_ARG_CONTR_MINIMAX		= "m";
#endif

#endif /* HT_UI_XLIB */

