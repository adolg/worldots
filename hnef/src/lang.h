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
#ifndef HT_LANG_H
#define HT_LANG_H

	/*
	 * Used at various points when an error code is unknown but
	 * still has to be printed.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_FR_UNKNOWN;

	/*
	 * HT_FR_FAIL_ALLOC
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_FR_FAIL_ALLOC;

	/*
	 * HT_FR_FAIL_NULLPTR
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_FR_FAIL_NULLPTR;

	/*
	 * HT_FR_FAIL_ARROB
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_FR_FAIL_ARROB;

	/*
	 * HT_FR_FAIL_ENUM_UNK
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_FR_FAIL_ENUM_UNK;

	/*
	 * HT_FR_FAIL_ILL_ARG
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_FR_FAIL_ILL_ARG;

	/*
	 * HT_FR_FAIL_ILL_STATE
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_FR_FAIL_ILL_STATE;

	/*
	 * HT_FR_FAIL_IO_ENV_UNDEF
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_FR_FAIL_IO_ENV_UNDEF;

	/*
	 * HT_FR_FAIL_IO_FILE_R
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_FR_FAIL_IO_FILE_R;

	/*
	 * HT_FR_FAIL_IO_PRINT
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_FR_FAIL_IO_PRINT;

#ifdef HT_POSIX
	/*
	 * HT_FR_FAIL_POSIX
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_FR_FAIL_POSIX;
#endif

#ifdef HT_UI_CLI
	/*
	 * HT_FR_FAIL_IO_STDIN
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_FR_FAIL_IO_STDIN;
#endif

#ifdef HT_UI_XLIB

	/*
	 * HT_FR_FAIL_UIX
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_FR_FAIL_UIX;

	/*
	 * HT_FR_FAIL_UIX_DISPLAY
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_FR_FAIL_UIX_DISPLAY;

	/*
	 * HT_FR_FAIL_UIX_XPM_FILER
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_FR_FAIL_UIX_XPM_FILER;

#endif /* HT_UI_XLIB */

#ifdef HT_UI_MSQU
	/*
	 * HT_FR_FAIL_IO
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_FR_FAIL_IO;
#endif

/*
	 * HT_FR_FAIL_UI_NONE
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_FR_FAIL_UI_NONE;

	/*
	 * strtol (or similar) failed to convert a number.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_FAIL_GEN_STRTOL;

	/*
	 * A number was out of bounds.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_FAIL_GEN_NOOB;

	/*
	 * Not single bit.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_FAIL_GEN_BIT_SINGLE;

	/*
	 * HT_FR_FAIL_INVOC: HT_FAIL_INVOC_UNK
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_FAIL_INVOC_UNK;

	/*
	 * HT_FR_FAIL_INVOC: HT_FAIL_INVOC_UI_NONE
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_FAIL_INVOC_UI_NONE;

	/*
	 * HT_FR_FAIL_INVOC: HT_FAIL_INVOC_UI_UNK
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_FAIL_INVOC_UI_UNK;

	/*
	 * HT_FR_FAIL_INVOC: HT_FAIL_INVOC_RC_NONE
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_FAIL_INVOC_RC_NONE;

	/*
	 * invoc.info.nplay_print = HT_INV_PRINT_HELP
	 */
/*@unchecked@*/
	extern
	const char HT_L_INV_S_HELP;

	/*
	 * invoc.info.nplay_print = HT_INV_PRINT_NINE
	 */
/*@unchecked@*/
	extern
	const char HT_L_INV_S_NINE;

	/*
	 * invoc.file_rc
	 */
/*@unchecked@*/
	extern
	const char HT_L_INV_S_RUNCOM;

	/*
	 * invoc.info.play_ui
	 */
/*@unchecked@*/
	extern
	const char HT_L_INV_S_UI;

	/*
	 * invoc.info.nplay_print = HT_INV_PRINT_VERSION
	 */
/*@unchecked@*/
	extern
	const char HT_L_INV_S_VERSION;

#ifdef HT_UI_CLI
	/*
	 * invoc.info.play_ui = HT_UIT_CLI
	 */
/*@unchecked@*/
	extern
	const char HT_L_INV_S_UI_CLI;
#endif

#ifdef HT_UI_XLIB
	/*
	 * invoc.info.play_ui = HT_UIT_XLIB
	 */
/*@unchecked@*/
	extern
	const char HT_L_INV_S_UI_XLIB;
#endif

#ifdef HT_UI_MSQU
	/*
	 * invoc.info.play_ui = HT_UIT_MSQU
	 */
/*@unchecked@*/
	extern
	const char HT_L_INV_S_UI_MSQU;
#endif

	/*
	 * Help string for HT_L_INV_S_HELP.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_INV_HELP_S_HELP;

	/*
	 * Help string for HT_L_INV_S_VERSION.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_INV_HELP_S_VERSION;

	/*
	 * Help string for HT_L_INV_S_RUNCOM.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_INV_HELP_S_RUNCOM;

	/*
	 * Help string for HT_L_INV_S_UI.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_INV_HELP_S_UI;

	/*
	 * String inserted in HT_L_INV_S_UI; used to mark the default
	 * interface (which is selected based on which interfaces are
	 * compiled in).
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_INV_HELP_S_UI_DEF;

#ifdef HT_UI_CLI
	/*
	 * Help on the HT_L_INV_S_UI_CLI interface argument.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_INV_HELP_S_UI_CLI;
#endif

#ifdef HT_UI_XLIB
	/*
	 * Help on the HT_L_INV_S_UI_XLIB interface argument.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_INV_HELP_S_UI_XLIB;
#endif

#ifdef HT_UI_MSQU
	/*
	 * invoc.info.play_ui = HT_UIT_MSQU
	 */
/*@unchecked@*/
	extern
	const char * HT_L_INV_HELP_S_UI_MSQU;
#endif

	/*
	 * HT_FAIL_RREADER_ARG_DUP
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_FAIL_RREADER_ARG_DUP;

	/*
	 * HT_FAIL_RREADER_ARG_MISS
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_FAIL_RREADER_ARG_MISS;

	/*
	 * HT_FAIL_RREADER_ARG_EXCESS
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_FAIL_RREADER_ARG_EXCESS;

	/*
	 * HT_FAIL_RREADER_ARG_UNK
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_FAIL_RREADER_ARG_UNK;

	/*
	 * HT_FAIL_RREADER_TYPE_DUP
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_FAIL_RREADER_TYPE_DUP;

	/*
	 * HT_FAIL_RREADER_TYPE_ORDER
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_FAIL_RREADER_TYPE_ORDER;

	/*
	 * HT_FAIL_RREADER_TYPE_OVERFLOW
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_FAIL_RREADER_TYPE_OVERFLOW;

	/*
	 * HT_FAIL_RREADER_TYPE_UNDEF
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_FAIL_RREADER_TYPE_UNDEF;

	/*
	 * HT_FAIL_RREADER_NOSIZE
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_FAIL_RREADER_NOSIZE;

	/*
	 * HT_FAIL_RREADER_INCOMP
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_FAIL_RREADER_INCOMP;

	/*
	 * Ruleset file "id" argument.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_RREADER_CMD_ID;

	/*
	 * Ruleset file "hash" argument.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_RREADER_CMD_HASH;

	/*
	 * Ruleset file "name" argument.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_RREADER_CMD_NAME;

	/*
	 * Ruleset file "path" argument.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_RREADER_CMD_PATH;

	/*
	 * Ruleset file "width" argument.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_RREADER_CMD_WIDTH;

	/*
	 * Ruleset file "height" argument.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_RREADER_CMD_HEIGHT;

	/*
	 * Ruleset file "piece" argument.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_RREADER_CMD_PIECE;

	/*
	 * Ruleset file "pieces" argument.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_RREADER_CMD_PIECES;

	/*
	 * Ruleset file "forbid_repeat" argument.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_RREADER_CMD_FREPEAT;

	/*
	 * Ruleset file "square" argument.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_RREADER_CMD_SQUARE;

	/*
	 * Ruleset file "squares" argument.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_RREADER_CMD_SQUARES;

	/*
	 * Ruleset file "bit" argument.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_RREADER_CMDA_BIT;

	/*
	 * Ruleset file "capt_edge" argument.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_RREADER_CMDA_CAPT_EDGE;

	/*
	 * Ruleset file "capt_loss" argument.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_RREADER_CMDA_CAPT_LOSS;

	/*
	 * Ruleset file "capt_sides" argument.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_RREADER_CMDA_CAPT_SIDES;

	/*
	 * Ruleset file "captures" argument.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_RREADER_CMDA_CAPTURES;

	/*
	 * Ruleset file "anvils" argument.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_RREADER_CMDA_ANVILS;

	/*
	 * Ruleset file "hammers" argument.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_RREADER_CMDA_HAMMERS;

	/*
	 * Ruleset file "custodial" argument.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_RREADER_CMDA_CUSTODIAL;

	/*
	 * Ruleset file "dbl_trap" argument.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_RREADER_CMDA_DBL_TRAP;

	/*
	 * Ruleset file "dbl_trap_capt" argument.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_RREADER_CMDA_DBL_TRAP_CAPT;

	/*
	 * Ruleset file "dbl_trap_compl" argument.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_RREADER_CMDA_DBL_TRAP_COMPL;

	/*
	 * Ruleset file "dbl_trap_edge" argument.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_RREADER_CMDA_DBL_TRAP_EDGE;

	/*
	 * Ruleset file "dbl_trap_encl" argument.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_RREADER_CMDA_DBL_TRAP_ENCL;

	/*
	 * Ruleset file "escape" argument.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_RREADER_CMDA_ESCAPE;

	/*
	 * Ruleset file "noreturn" argument.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_RREADER_CMDA_NORETURN;

	/*
	 * Ruleset file "occupies" argument.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_RREADER_CMDA_OCCUPIES;

	/*
	 * Ruleset file "traverses" argument.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_RREADER_CMDA_TRAVERSES;

	/*
	 * Ruleset file "owner" argument.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_RREADER_CMDA_OWNER;

	/*
	 * !ruleset_valid_gen()
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_RVALID_GEN;

	/*
	 * !ruleset_valid_tpieces_arr()
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_RVALID_PIECEARR;

	/*
	 * !ruleset_valid_tpieces() or
	 * !ruleset_valid_tsquares(); or
	 * !ruleset_valid_bitarr() for ruleset->pieces or squares.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_RVALID_TYPE_INVALID;

	/*
	 * !ruleset_valid_bits_def_piece() or
	 * !ruleset_valid_bits_def_square()
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_RVALID_TYPE_UNDEF;

	/*
	 * !ruleset_valid_players()
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_RVALID_PLAYERS;

	/*
	 * !ruleset_valid_initpos()
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_RVALID_INITPOS;

#if defined(HT_UI_CLI) || defined(HT_UI_MSQU)

	/*
	 * Command line interface prompt. This is not language related.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_UIC_CMD_PROMPT;

	/*
	 * Asks the user if he wants to play again.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_UIC_PLAYAGAIN;

	/*
	 * Message when a human player makes an illegal move.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_UIC_ACT_MOVE_ILL_HUMAN;

#ifdef HT_AI_MINIMAX

	/*
	 * Message when a computer player makes an illegal move, and was
	 * disconnected because of it.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_UIC_ACT_MOVE_ILL_AI_UNS;

	/*
	 * Message when a computer player fails to find a move, and was
	 * disconnected because of it.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_UIC_ACT_MOVE_UNK_AI;

#endif /* HT_AI_MINIMAX */

	/*
	 * Message when player arguments are invalid when attempting to
	 * change controller of a player.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_UIC_ACT_CONTR_ILL;

	/*
	 * Identifier for human player.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_UIC_ACT_PLAYERS_HUMAN;

#ifdef HT_AI_MINIMAX
	/*
	 * Identifier for minimax computer player. Will be followed by
	 * maximum search depth.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_UIC_ACT_PLAYERS_AIM;
#endif

	/*
	 * Help on HT_L_UIC_ARG_MOVE.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_UIC_HELP_ARG_MOVE;

	/*
	 * Help on HT_L_UIC_ARG_CONTR with arguments to set player.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_UIC_HELP_ARG_CONTR;

	/*
	 * Help on HT_L_UIC_ARG_CONTR: HT_AIT_NONE.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_UIC_HELP_ARG_CONTR_NONE;

#ifdef HT_AI_MINIMAX
	/*
	 * Help on HT_L_UIC_ARG_CONTR: HT_AIT_MINIMAX.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_UIC_HELP_ARG_CONTR_MINIMAX;
#endif

	/*
	 * Help on HT_L_UIC_ARG_CONTR without arguments.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_UIC_HELP_ARG_PRINT_PLAYERS;

	/*
	 * Help on HT_L_UIC_ARG_HELP.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_UIC_HELP_ARG_PRINT_BOARD;

	/*
	 * Help on HT_L_UIC_ARG_QUIT.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_UIC_HELP_ARG_QUIT;

	/*
	 * UI_CLI command for HT_UI_ACT_CONTR.
	 */
/*@unchecked@*/
	extern
	const char HT_L_UIC_ARG_CONTR;

	/*
	 * UI_CLI command for HT_UI_ACT_MOVE.
	 */
/*@unchecked@*/
	extern
	const char HT_L_UIC_ARG_MOVE;

	/*
	 * UI_CLI command for HT_UIC_ACT_PRINT_BOARD
	 * (HT_UI_ACT_SPECIAL).
	 */
/*@unchecked@*/
	extern
	const char HT_L_UIC_ARG_PRINT_BOARD;

	/*
	 * UI_CLI command for HT_UI_ACT_QUIT.
	 */
/*@unchecked@*/
	extern
	const char HT_L_UIC_ARG_QUIT;

	/*
	 * UI_CLI answer "yes" to some question.
	 */
/*@unchecked@*/
	extern
	const char HT_L_UIC_ARG_Q_YES;

	/*
	 * UI_CLI answer "no" to some question.
	 */
/*@unchecked@*/
	extern
	const char HT_L_UIC_ARG_Q_NO;

	/*
	 * UI_CLI command for HT_UI_ACT_CONTR: HT_AIT_NONE.
	 */
/*@unchecked@*/
	extern
	const char HT_L_UIC_ARG_CONTR_NONE;

#ifdef HT_AI_MINIMAX
	/*
	 * UI_CLI command for HT_UI_ACT_CONTR: HT_AIT_MINIMAX.
	 */
/*@unchecked@*/
	extern
	const char HT_L_UIC_ARG_CONTR_MINIMAX;
#endif

#endif /* HT_UI_CLI */

#ifdef HT_UI_XLIB

	/*
	 * Asks the user if he wants to play again.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_UIX_PLAYAGAIN;

	/*
	 * UI_XLIB command for HT_UI_ACT_CONTR.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_UIX_ARG_CONTR;

#ifdef HT_AI_MINIMAX
	/*
	 * UI_XLIB command for uix_stop.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_UIX_ARG_STOP;
#endif

	/*
	 * UI_XLIB command for HT_UI_ACT_QUIT.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_UIX_ARG_QUIT;

	/*
	 * UI_XLIB answer "yes" to some question.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_UIX_ARG_Q_YES;

	/*
	 * UI_XLIB answer "yes" to some question.
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_UIX_ARG_Q_NO;

	/*
	 * UI_XLIB command for HT_UI_ACT_CONTR: HT_AIT_NONE
	 */
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_UIX_ARG_CONTR_NONE;

	/*
	 * UI_XLIB command for HT_UI_ACT_CONTR: HT_AIT_MINIMAX.
	 */
#ifdef HT_AI_MINIMAX
/*@observer@*/
/*@unchecked@*/
	extern
	const char * HT_L_UIX_ARG_CONTR_MINIMAX;
#endif

#endif /* HT_UI_XLIB */

#endif

