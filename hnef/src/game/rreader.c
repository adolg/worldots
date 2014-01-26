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
#include <limits.h>		/* *_MAX */
#include <string.h>		/* strlen */

#ifdef HT_POSIX
#include <errno.h>		/* errno */
#include <unistd.h>		/* symlink */
#endif /* HT_POSIX */

#include "config.h"		/* HT_RSTR, HT_POSIX */
#include "game.h"		/* alloc_game, free_game, ... */
#include "info.h"		/* HT_FILE_RC_DEF */
#include "iotextparse.h"	/* ht_line_readline */
#include "lang.h"		/* HT_L_* */
#include "movet.h"		/* HT_BOARDPOS_NONE */
#include "num.h"		/* texttoushort */
#include "rreader.h"
#include "rreadert.h"		/* rread_info */
#include "ruleset.h"		/* valid_size */
#include "type_piece.h"		/* type_piece_* */
#include "type_square.h"	/* type_square_* */
#include "types.h"		/* HT_TYPEC_* */

/*
 * Reads "width" or "height" into the "size" pointer.
 *
 * "size" is a pointer to rread_info->bwidth or rread_info->bheight.
 */
static
enum HT_FR
parse_size (
/*@in@*/
/*@notnull@*/
	const struct ht_line * const HT_RSTR	line,
/*@in@*/
/*@notnull@*/
	unsigned short * const			size,
/*@in@*/
/*@notnull@*/
	enum HT_FAIL_RREADER * const HT_RSTR	fail_read
	)
/*@globals errno@*/
/*@modifies errno, * size, * fail_read@*/
{
	if (line->wordc			< (size_t)2
	|| strlen(line->words[1].chars)	< (size_t)1)
	{
		* fail_read	= HT_FAIL_RREADER_ARG_MISS;
		return		HT_FR_FAIL_RREADER;
	}
/* splint incomplete logic */ /*@i2@*/\
	else if (HT_BOARDPOS_NONE != * size)
	{
		* fail_read	= HT_FAIL_RREADER_ARG_DUP;
		return		HT_FR_FAIL_RREADER;
	}

	if (!texttoushort(line->words[1].chars, size))
	{
#ifndef HT_UNSAFE
		* size		= HT_BOARDPOS_NONE;
#endif
		* fail_read	= HT_FAIL_RREADER_STRTOL;
		return		HT_FR_FAIL_RREADER;
	}
/* splint incomplete logic */ /*@i2@*/\
	else if (!valid_size(* size))
	{
		* size		= HT_BOARDPOS_NONE;
		* fail_read	= HT_FAIL_RREADER_NOOB;
		return		HT_FR_FAIL_RREADER;
	}
	return HT_FR_SUCCESS;
}

/*
 * Reads the pieces or squares array in ruleset. If read_sq, then read
 * g->rules->squares; else read g->rules->pieces.
 */
static
enum HT_FR
parse_arrb (
/*@in@*/
/*@notnull@*/
	const struct ht_line * const HT_RSTR	line,
/*@in@*/
/*@notnull@*/
	struct game * const HT_RSTR		g,
	const HT_BOOL				read_sq,
/*@in@*/
/*@notnull@*/
	enum HT_FAIL_RREADER * const HT_RSTR	fail_read
	)
/*@globals errno@*/
/*@modifies errno, * g, * fail_read@*/
{
	unsigned short		num;
	size_t			i;
	HT_BIT_U8 * HT_RSTR	arr;
#ifndef HT_UNSAFE
	if (NULL == g)
	{
		return	HT_FR_FAIL_NULLPTR;
	}
#endif
	arr	= read_sq ? g->rules->squares : g->rules->pieces;
	if (line->wordc - 1 < (size_t)g->rules->opt_blen)
	{
		* fail_read	= HT_FAIL_RREADER_ARG_MISS;
		return		HT_FR_FAIL_RREADER;
	}

	/*
	 * First word is "pieces" / "squares" argument, so skip it.
	 */
	for (i = (size_t)1; i < line->wordc; i++)
	{
		if (!texttoushort(line->words[i].chars, & num))
		{
			* fail_read	= HT_FAIL_RREADER_STRTOL;
			return		HT_FR_FAIL_RREADER;
		}
		else if (num > (unsigned short)128
		|| (0 != num && !single_bit((unsigned int)num)))
		{
			* fail_read	= HT_FAIL_RREADER_BIT_SINGLE;
			return		HT_FR_FAIL_RREADER;
		}
		else if (i - 1 >= (size_t)g->rules->opt_blen)
		{
			* fail_read	= HT_FAIL_RREADER_ARG_EXCESS;
			return		HT_FR_FAIL_RREADER;
		}
		else
		{
			arr[i - 1]	= (HT_BIT_U8)num;
		}
	}
	return	HT_FR_SUCCESS;
}

/*
 * Parses "piece/square bit <arg>".
 *
 * The <arg> is added as a type_piece or type_square in g->rules.
 *
 * If read_sq, then it's a square type, else it's a piece type.
 */
static
enum HT_FR
parse_type_bit (
/*@in@*/
/*@notnull@*/
	const struct ht_line * const HT_RSTR	line,
/*@in@*/
/*@notnull@*/
	struct game * const			g,
	const HT_BOOL				read_sq,
/*@in@*/
/*@notnull@*/
	enum HT_FAIL_RREADER * const HT_RSTR	fail_read
	)
/*@globals errno@*/
/*@modifies errno, * g, * fail_read@*/
{
	const char * HT_RSTR	arg2;
	unsigned short		num;
#ifndef HT_UNSAFE
	if (NULL == g)
	{
		return	HT_FR_FAIL_NULLPTR;
	}
#endif
	if (line->wordc			< (size_t)3
	|| strlen(line->words[2].chars)	< (size_t)1)
	{
		* fail_read	= HT_FAIL_RREADER_ARG_MISS;
		return		HT_FR_FAIL_RREADER;
	}
/* splint incomplete logic */ /*@i1@*/\
	else if (g->rules->type_piecec >= HT_TYPE_MAX)
	{
		* fail_read	= HT_FAIL_RREADER_TYPE_OVERFLOW;
		return		HT_FR_FAIL_RREADER;
	}
	arg2	= line->words[2].chars;

	if (!texttoushort(arg2, & num))
	{
		* fail_read	= HT_FAIL_RREADER_STRTOL;
		return		HT_FR_FAIL_RREADER;
	}
	else if (num > (unsigned short)128
	|| !single_bit((unsigned int)num))
	{
		* fail_read	= HT_FAIL_RREADER_BIT_SINGLE;
		return		HT_FR_FAIL_RREADER;
	}
	else
	{
		const HT_BIT_U8 bit	= (HT_BIT_U8)num;
		enum HT_FR fr		= HT_FR_NONE;
		if (read_sq)
		{
			fr	= type_square_set	(g->rules, bit);
		}
		else
		{
			fr	= type_piece_set	(g->rules, bit);
		}

		if (HT_FR_FAIL_ILL_ARG == fr)
		{
			* fail_read	= HT_FAIL_RREADER_BIT_SINGLE;
			return		HT_FR_FAIL_RREADER;
		}
		else if (HT_FR_FAIL_ILL_STATE == fr)
		{
			* fail_read	= HT_FAIL_RREADER_TYPE_DUP;
			return		HT_FR_FAIL_RREADER;
		}
		else if (HT_FR_FAIL_NULLPTR == fr)
		{
			* fail_read	= HT_FAIL_RREADER_TYPE_ORDER;
			return		HT_FR_FAIL_RREADER;
		}
		else
		{
			return	fr;
		}
	}
}

/*
 * Parses "piece/square x <arg>", where <arg> is a HT_BIT_U8 (pointed to
 * by bit).
 *
 * Only makes sure that the bit can be converted from a string. Does not
 * validate the value in any way.
 *
 * bit is unchanged if it fails.
 */
static
enum HT_FR
parse_type_bitu8 (
/*@in@*/
/*@notnull@*/
	const char * const HT_RSTR		arg,
/*@in@*/
/*@notnull@*/
	HT_BIT_U8 * const HT_RSTR		bit,
/*@in@*/
/*@notnull@*/
	enum HT_FAIL_RREADER * const HT_RSTR	fail_read
	)
/*@globals errno@*/
/*@modifies errno, * bit, * fail_read@*/
{
	unsigned short num;
	if (!texttoushort(arg, & num))
	{
		* fail_read	= HT_FAIL_RREADER_STRTOL;
		return		HT_FR_FAIL_RREADER;
	}
/* splint incomplete logic */ /*@i1@*/\
	else if (num > (unsigned short)UCHAR_MAX)
	{
		* fail_read	= HT_FAIL_RREADER_NOOB;
		return		HT_FR_FAIL_RREADER;
	}
	// this helps with keeping captures/hammers/anvils consistent,
	// and also with occupies=>traverses optimization (see rreader.c parse_piece_arg)

	// As an added benefit, you can now make rules more human-readable by splitting
	// bit arguments into one line per each bit:
	// piece 1 occupies 1
	// piece 1 occupies 4
	* bit	|= (HT_BIT_U8)num;
	return	HT_FR_SUCCESS;
}

/*
 * Parses "piece/square x <arg>", where <arg> is an unsigned short
 * (pointed to by shrt).
 *
 * Only makes sure that the number can be converted from a string. Does
 * not validate the value in any way.
 */
static
enum HT_FR
parse_type_ushrt (
/*@in@*/
/*@notnull@*/
	const char * const HT_RSTR		arg,
/*@in@*/
/*@notnull@*/
	unsigned short * const			shrt,
/*@in@*/
/*@notnull@*/
	enum HT_FAIL_RREADER * const HT_RSTR	fail_read
	)
/*@globals errno@*/
/*@modifies errno, * shrt, * fail_read@*/
{
	if (!texttoushort(arg, shrt))
	{
		* fail_read	= HT_FAIL_RREADER_STRTOL;
		return		HT_FR_FAIL_RREADER;
	}
	return	HT_FR_SUCCESS;
}

/*
 * Parses "piece x <arg>", where x is a single bit, and tp is the piece
 * corresponding to bit x.
 */
static
enum HT_FR
parse_piece_arg (
/*@in@*/
/*@notnull@*/
	const struct ht_line * const	line,
/*@in@*/
/*@notnull@*/
	struct type_piece * const	tp,
/*@in@*/
/*@notnull@*/
	enum HT_FAIL_RREADER * const	fail_read
	)
/*@globals errno@*/
/*@modifies errno, * tp, * fail_read@*/
{
	const char * arg2, * arg3;
	if (line->wordc			< (size_t)3
	|| strlen(line->words[2].chars)	< (size_t)1)
	{
		* fail_read	= HT_FAIL_RREADER_ARG_MISS;
		return		HT_FR_FAIL_RREADER;
	}
	arg2	= line->words[2].chars;
	arg3	= NULL;

	if	(0 == strcmp(arg2, HT_L_RREADER_CMDA_CAPT_EDGE))
	{
		tp->capt_edge	= HT_TRUE;
		return		HT_FR_SUCCESS;
	}
	else if	(0 == strcmp(arg2, HT_L_RREADER_CMDA_CAPT_LOSS))
	{
		tp->capt_loss	= HT_TRUE;
		return		HT_FR_SUCCESS;
	}
	else if	(0 == strcmp(arg2, HT_L_RREADER_CMDA_CUSTODIAL))
	{
		tp->custodial	= HT_TRUE;
		return		HT_FR_SUCCESS;
	}
	else if (0 == strcmp(arg2, HT_L_RREADER_CMDA_DBL_TRAP))
	{
		tp->dbl_trap	= HT_TRUE;
		return		HT_FR_SUCCESS;
	}
	else if (0 == strcmp(arg2, HT_L_RREADER_CMDA_DBL_TRAP_CAPT))
	{
		tp->dbl_trap_capt	= HT_TRUE;
		return			HT_FR_SUCCESS;
	}
	else if (0 == strcmp(arg2, HT_L_RREADER_CMDA_DBL_TRAP_COMPL))
	{
		tp->dbl_trap_compl	= HT_TRUE;
		return			HT_FR_SUCCESS;
	}
	else if (0 == strcmp(arg2, HT_L_RREADER_CMDA_DBL_TRAP_EDGE))
	{
		tp->dbl_trap_edge	= HT_TRUE;
		return			HT_FR_SUCCESS;
	}
	else if (0 == strcmp(arg2, HT_L_RREADER_CMDA_DBL_TRAP_ENCL))
	{
		tp->dbl_trap_encl	= HT_TRUE;
		return			HT_FR_SUCCESS;
	}
/* splint incomplete logic */ /*@i1@*/\
	else if (0 == strcmp(arg2, HT_L_RREADER_CMDA_ESCAPE))
	{
		tp->escape		= HT_TRUE;
		return			HT_FR_SUCCESS;
	}
/*
 * NOTE:	Additional 3-word arguments go here.
 *		"piece <tp->bit> <arg2>"
 */

	if (line->wordc			> (size_t)3
	|| strlen(line->words[3].chars)	> (size_t)0)
	{
		arg3	= line->words[3].chars;
	}
	else
	{
		* fail_read	= HT_FAIL_RREADER_ARG_MISS;
		return		HT_FR_FAIL_RREADER;
	}

	if	(0 == strcmp(arg2, HT_L_RREADER_CMDA_CAPTURES))
	{
		enum HT_FR result =	parse_type_bitu8(arg3, & tp->captures,
			fail_read);
		tp->anvils |= tp->captures;
		return result;
	}
	else if	(0 == strcmp(arg2, HT_L_RREADER_CMDA_ANVILS))
	{
		return	parse_type_bitu8(arg3, & tp->anvils,
			fail_read);
	}
	else if	(0 == strcmp(arg2, HT_L_RREADER_CMDA_HAMMERS))
	{
		return	parse_type_bitu8(arg3, & tp->captures,
			fail_read);
	}
	else if	(0 == strcmp(arg2, HT_L_RREADER_CMDA_NORETURN))
	{
		return	parse_type_bitu8(arg3, & tp->noreturn,
			fail_read);
	}
	else if	(0 == strcmp(arg2, HT_L_RREADER_CMDA_OCCUPIES))
	{
		// we assume here that if a piece is allowed to occupy a square,
		// then it can go past it as well. This helps with optimizing
		// original ruleset without adding "traverses" everywhere.
		// If there will be a game where piece is allowed to enter, but
		// not pass through a square, then this needs to be reconsidered
		enum HT_FR result =	parse_type_bitu8(arg3, & tp->occupies,
			fail_read);
		tp->traverses |= tp->occupies;
		return result;
	}
	else if	(0 == strcmp(arg2, HT_L_RREADER_CMDA_TRAVERSES))
	{
		return	parse_type_bitu8(arg3, & tp->traverses,
			fail_read);
	}
	else if	(0 == strcmp(arg2, HT_L_RREADER_CMDA_CAPT_SIDES))
	{
		return	parse_type_ushrt(arg3, & tp->capt_sides,
			fail_read);
	}
	else if	(0 == strcmp(arg2, HT_L_RREADER_CMDA_OWNER))
	{
		return	parse_type_ushrt(arg3, & tp->owner,
			fail_read);
	}
/*
 * NOTE:	Additional 4-word arguments go here.
 *		"piece <tp->bit> <arg2> <arg3>"
 */
	else
	{
		* fail_read	= HT_FAIL_RREADER_ARG_UNK;
		return		HT_FR_FAIL_RREADER;
	}
}

/*
 * Parses "square x <arg>", where x is a single bit, and ts is the
 * square corresponding to bit x.
 */
static
enum HT_FR
parse_square_arg (
/*@in@*/
/*@notnull@*/
	const struct ht_line * const	line,
/*@in@*/
/*@notnull@*/
	struct type_square * const	ts,
/*@in@*/
/*@notnull@*/
	enum HT_FAIL_RREADER * const	fail_read
	)
/*@globals errno@*/
/*@modifies errno, * ts, * fail_read@*/
{
	const char * arg2, * arg3;
	if (line->wordc			< (size_t)3
	|| strlen(line->words[2].chars)	< (size_t)1)
	{
		* fail_read	= HT_FAIL_RREADER_ARG_MISS;
		return		HT_FR_FAIL_RREADER;
	}
	arg2	= line->words[2].chars;
	arg3	= NULL;

	if (0 == strcmp(arg2, HT_L_RREADER_CMDA_ESCAPE))
	{
		ts->escape		= HT_TRUE;
		return			HT_FR_SUCCESS;
	}
/*
 * NOTE:	Additional 3-word arguments go here.
 *		"square <ts->bit> <arg2>"
 */


	if (line->wordc			> (size_t)3
	|| strlen(line->words[3].chars)	> (size_t)0)
	{
		arg3	= line->words[3].chars;
	}
	else
	{
		* fail_read	= HT_FAIL_RREADER_ARG_MISS;
		return		HT_FR_FAIL_RREADER;
	}

	if	(0 == strcmp(arg2, HT_L_RREADER_CMDA_CAPTURES))
	{
		return	parse_type_bitu8(arg3, & ts->captures,
			fail_read);
	}
	else if	(0 == strcmp(arg2, HT_L_RREADER_CMDA_CAPT_SIDES))
	{
		return	parse_type_ushrt(arg3, & ts->capt_sides,
			fail_read);
	}
/*
 * NOTE:	Additional 4-word arguments go here.
 *		"square <ts->bit> <arg2> <arg3>"
 */
	else
	{
		* fail_read	= HT_FAIL_RREADER_ARG_UNK;
		return		HT_FR_FAIL_RREADER;
	}
}

/*
 * Parses "piece/square <arg>", where <arg> is either a HT_BIT_U8 or an
 * argument.
 *
 * read_sq is true if we're parsing a square bit; false if it's a piece
 * bit.
 */
static
enum HT_FR
parse_type (
/*@in@*/
/*@notnull@*/
	const struct ht_line * const	line,
/*@in@*/
/*@notnull@*/
	struct game * const		g,
	const HT_BOOL			read_sq,
/*@in@*/
/*@notnull@*/
	enum HT_FAIL_RREADER * const	fail_read
	)
/*@globals errno@*/
/*@modifies errno, * g, * fail_read@*/
{
	const char * arg1;
#ifndef HT_UNSAFE
	if (NULL == g)
	{
		return	HT_FR_FAIL_NULLPTR;
	}
#endif
	if (line->wordc			< (size_t)2
	|| strlen(line->words[1].chars)	< (size_t)1)
	{
		* fail_read	= HT_FAIL_RREADER_ARG_MISS;
		return		HT_FR_FAIL_RREADER;
	}
	arg1	= line->words[1].chars;

	if	(0 == strcmp(arg1, HT_L_RREADER_CMDA_BIT))
	{
		return	parse_type_bit(line, g, read_sq, fail_read);
	}
	else
	{
		HT_BIT_U8	bit;
		{
			unsigned short	num;
			if (!texttoushort(arg1, & num))
			{
				* fail_read = HT_FAIL_RREADER_STRTOL;
				return	HT_FR_FAIL_RREADER;
			}
/* splint incomplete logic */ /*@i1@*/\
			else if (num > (unsigned short)128
			|| !single_bit((unsigned int)num))
			{
				* fail_read =
					HT_FAIL_RREADER_BIT_SINGLE;
				return	HT_FR_FAIL_RREADER;
			}
			bit	= (HT_BIT_U8)num;
		}
		if (read_sq)
		{
			struct type_square * const ts =
				type_square_get(g->rules, bit);
			if (HT_BIT_U8_EMPTY == ts->bit)
			{
				* fail_read =
					HT_FAIL_RREADER_TYPE_UNDEF;
				return	HT_FR_FAIL_RREADER;
			}
			return parse_square_arg(line, ts, fail_read);
		}
		else
		{
			struct type_piece * const tp =
				type_piece_get(g->rules, bit);
			if (HT_BIT_U8_EMPTY == tp->bit)
			{
				* fail_read =
					HT_FAIL_RREADER_TYPE_UNDEF;
				return	HT_FR_FAIL_RREADER;
			}
			return parse_piece_arg(line, tp, fail_read);
		}
	}
}

static
enum HT_FR
parse_strarg (
/*@in@*/
/*@notnull@*/
	const struct ht_line * const HT_RSTR	line,
/*@out@*/
/*@notnull@*/
	char * HT_RSTR * const HT_RSTR		ptr,
/*@in@*/
/*@notnull@*/
	enum HT_FAIL_RREADER * const HT_RSTR	fail_read
	)
/*@modifies * info, * fail_read@*/
{
	char *	arg1;
	size_t	arg1len;
	if (line->wordc			< (size_t)2
	|| strlen(line->words[1].chars)	< (size_t)1)
	{
		* fail_read	= HT_FAIL_RREADER_ARG_MISS;
		return		HT_FR_FAIL_RREADER;
	}
	arg1	= line->words[1].chars;
	arg1len	= strlen(arg1) + 1;

	* ptr	= malloc(sizeof(* * ptr) * arg1len);
	if (NULL == * ptr)
	{
/* splint bug: pointer to pointer */ /*@i1@*/\
		return	HT_FR_FAIL_ALLOC;
	}

	strncpy	(* ptr, arg1, arg1len);
	return	HT_FR_SUCCESS;
}

/*
 * Parses a line in the ruleset file.
 *
 * "width" and "height" must come before all arguments that depend on a
 * struct game being allocated (such as g->ruleset->pieces and
 * g->ruleset->squares). We know that width and height have not been set
 * -- and that the game g has not been allocated -- if g is NULL.
 */
static
enum HT_FR
parseline (
/*@in@*/
/*@notnull@*/
	const struct ht_line * const	line,
/*@partial@*/
/*@notnull@*/
	struct rread_info * const	info,
/*@null@*/
	struct game * const		g,
/*@in@*/
/*@notnull@*/
	enum HT_FAIL_RREADER * const	fail_read
	)
/*@globals errno@*/
/*@modifies errno, * info, * g, * fail_read@*/
{
	/*
	 * g is NULL until bwidth and bheight in info are set.
	 */
	char * arg0;
#ifndef HT_UNSAFE
	if (line->wordc			< (size_t)1
	|| strlen(line->words[0].chars)	< (size_t)1)
	{
		/*
		 * Empty line: can't happen (already checked).
		 */
		return HT_FR_FAIL_ILL_ARG;
	}
#endif
	arg0	= line->words[0].chars;

	if	(0 == strcmp(arg0, HT_L_RREADER_CMD_ID))
	{
		if (NULL != info->id)
		{
			* fail_read	= HT_FAIL_RREADER_ARG_DUP;
			return		HT_FR_FAIL_RREADER;
		}
		return	parse_strarg(line, & info->id, fail_read);
	}
	else if	(0 == strcmp(arg0, HT_L_RREADER_CMD_NAME))
	{
		if (NULL != info->name)
		{
			* fail_read	= HT_FAIL_RREADER_ARG_DUP;
			return		HT_FR_FAIL_RREADER;
		}
		return	parse_strarg(line, & info->name, fail_read);
	}
	else if	(0 == strcmp(arg0, HT_L_RREADER_CMD_PATH))
	{
		if (NULL != info->path)
		{
			* fail_read	= HT_FAIL_RREADER_ARG_DUP;
			return		HT_FR_FAIL_RREADER;
		}
		return	parse_strarg(line, & info->path, fail_read);
	}
	else if	(0 == strcmp(arg0, HT_L_RREADER_CMD_WIDTH))
	{
		return	parse_size(line, & info->bwidth, fail_read);
	}
/* splint incomplete logic */ /*@i1@*/\
	else if	(0 == strcmp(arg0, HT_L_RREADER_CMD_HEIGHT))
	{
		return	parse_size(line, & info->bheight, fail_read);
	}

	if (NULL == g)
	{
		* fail_read	= HT_FAIL_RREADER_NOSIZE;
		return		HT_FR_FAIL_RREADER;
	}

	if	(0 == strcmp(arg0, HT_L_RREADER_CMD_PIECES))
	{
		return	parse_arrb(line, g, HT_FALSE, fail_read);
	}
	else if	(0 == strcmp(arg0, HT_L_RREADER_CMD_SQUARES))
	{
		return	parse_arrb(line, g, HT_TRUE, fail_read);
	}
	else if	(0 == strcmp(arg0, HT_L_RREADER_CMD_PIECE))
	{
		return	parse_type(line, g, HT_FALSE, fail_read);
	}
	else if	(0 == strcmp(arg0, HT_L_RREADER_CMD_SQUARE))
	{
		return	parse_type(line, g, HT_TRUE, fail_read);
	}
	else if	(0 == strcmp(arg0, HT_L_RREADER_CMD_FREPEAT))
	{
		g->rules->forbid_repeat	= HT_TRUE;
		return			HT_FR_SUCCESS;
	}
/*
 * NOTE:	Additional arguments go here.
 *		"<arg0>"
 */
	else
	{
		* fail_read	= HT_FAIL_RREADER_ARG_UNK;
		return		HT_FR_FAIL_RREADER;
	}
}

/*
 * Resets / initializes info.
 */
static
void
rread_info_init (
/*@notnull@*/
/*@partial@*/
	struct rread_info * const HT_RSTR	info
	)
/*@modifies *info@*/
{
	info->bwidth = info->bheight	= HT_BOARDPOS_NONE;
	info->id	= info->name	= info->path		= NULL;
}

/*@null@*/
/*@partial@*/
/*@only@*/
static
struct rread_info *
alloc_rread_info (void)
/*@modifies nothing@*/
{
	struct rread_info * const info = malloc(sizeof(* info));
	return	info;
}

static
void
free_rread_info (
/*@notnull@*/
/*@owned@*/
/*@special@*/
	struct rread_info * const	info
	)
/*@modifies info->path, info@*/
/*@releases info->path, info@*/
{
	if (NULL != info->path)
	{
		free	(info->path);
	}
	free	(info);
}

/*
 * Returns default RC file (in $HOME).
 *
 * This function return non-NULL if fr is set to success, or NULL if fr
 * is set to failure.
 *
 * If this function returns non-NULL, then the return value must be
 * freed by the caller.
 *
 * NOTE:	This function will always fail if getenv("HOME") doesn't
 *		return a valid directory, or if "/" is not the path
 *		separator.
 */
/*@null@*/
/*@only@*/
static
char *
alloc_file_rc_def (
/*@in@*/
/*@notnull@*/
	enum HT_FR * const HT_RSTR	fr
	)
/*@modifies * fr@*/
{
	size_t			len;
	/*
	 * '/' as a path separator should apparently work on all
	 * systems, but it's only tested on Linux.
	 */
	const char		path_sep	= '/';
	const char * const	path_home	= getenv("HOME");
	char *			file_rc_def;
	if (NULL == path_home)
	{
		* fr	= HT_FR_FAIL_IO_ENV_UNDEF;
		return	NULL;
	}

	len	= strlen(path_home) + 1 /* strlen(path_sep) */
		+ strlen(HT_FILE_RC_DEF);
	file_rc_def	= malloc(sizeof(* file_rc_def) * (len + 1));
	if (NULL == file_rc_def)
	{
		* fr	= HT_FR_FAIL_ALLOC;
		return	NULL;
	}

/* splint: snprintf doesn't exist in C89. */ /*@i1@*/\
	if (sprintf(file_rc_def, "%s%c%s", path_home, path_sep,
		HT_FILE_RC_DEF) < 0)
	{
		* fr	= HT_FR_FAIL_IO_PRINT;
		free	(file_rc_def);
		return	NULL;
	}

	* fr	= HT_FR_SUCCESS;
	return	file_rc_def;
}

#ifdef HT_POSIX

/*
 * Checks if file_rc_def exists. If not, makes a new symlink out of it,
 * pointing to HT_FILE_RULE_DEF.
 *
 * Note that file_rc_def is HT_FILE_RC_DEF resolved against ${HOME}.
 * This function will therefore create the RC file the home directory of
 * whatever user runs the program.
 */
static
enum HT_FR
file_rc_def_checkcreate (
/*@in@*/
/*@notnull@*/
	const char * const file_rc_def
	)
/*@globals errno@*/
/*@modifies errno@*/
{
	const int	errno_old	= errno;
			errno		= 0;
/* splint POSIX bug */ /*@i1@*/\
	if (symlink(HT_FILE_RULE_DEF, file_rc_def) == -1)
	{
/* splint POSIX bug */ /*@i1@*/\
		if (EEXIST != errno)
		{
			/*
			 * errno will carry information to the user.
			 */
			return	HT_FR_FAIL_POSIX;
		}
		/*
		 * else: symlink already exists and was not overwritten.
		 * This is fine because we just need the link to exist.
		 */
	}
	/*
	 * else: symlink successfully created.
	 */
	errno	= errno_old;
	return	HT_FR_SUCCESS;
}

#endif /* HT_POSIX */

/*
 * Helper for alloc_game_read. Opens file_rc or the default rc file.
 *
 * If fr is success, then returns non-NULL. If fr is fail, then returns
 * NULL.
 *
 * You have to close the returned file, if non-NULL.
 *
 * NOTE:	This function will fail on non-POSIX systems if the RC
 *		file specified by file_rc doesn't exist or can't be
 *		opened for reading.
 */
/*@null@*/
/*@dependent@*/
static
FILE *
fopen_file_rc (
/*@in@*/
/*@notnull@*/
	const char * const	file_rc,
/*@in@*/
/*@notnull@*/
	enum HT_FR * const	fr
	)
#ifdef HT_POSIX
/*@globals fileSystem, errno@*/
#else
/*@globals fileSystem@*/
#endif
/*@modifies fileSystem, * fr@*/
#ifdef HT_POSIX
/*@modifies errno@*/
#endif
{
	FILE * file	= NULL;
	/*
	 * file_rc uses default RC file if empty.
	 */
	if (strlen(file_rc) < (size_t)1
	|| NULL == (file = fopen(file_rc, "r")))
	{
		char * file_rc_def	= alloc_file_rc_def(fr);
		if (HT_FR_SUCCESS != * fr || NULL == file_rc_def)
		{
#ifndef HT_UNSAFE
			if (NULL != file_rc_def)
			{
				free	(file_rc_def);
			}
#endif
			return	NULL;
		}
#ifndef HT_UNSAFE
		* fr	= HT_FR_NONE;
#endif

#ifdef HT_POSIX
		* fr	= file_rc_def_checkcreate(file_rc_def);
		if (HT_FR_SUCCESS != * fr)
		{
			free	(file_rc_def);
			return	NULL;
		}
#endif

		file	= fopen(file_rc_def, "r");
		free	(file_rc_def);
		if (NULL == file)
		{
			* fr	= HT_FR_FAIL_IO_FILE_R;
			return	NULL;
		}
	}

	* fr	= HT_FR_SUCCESS;
	return	file;
}

/*
 * Returns a game struct if it was successfully allocated, initialized
 * and configured according to the ruleset file indicated by file_rc.
 *
 * Returns NULL if the game could not be allocated, initialized or
 * configured according to file_rc for any reason, in which case fr will
 * be set (even for allocation failures), and fail_read may be.
 *
 * Note that this function does not guarantee that the ruleset is valid.
 * A successful return only indicates the ruleset file was correctly
 * formatted.
 *
 * Failure to retrieve the RC file can fail in the following ways:
 *
 * 1a.	If the RC file is specified by invocation parameter, but fails
 *	to open, then it will look for a default RC file.
 *
 * 1b.	If no RC file was specified by invocation parameter, then it
 *	will look for a default RC file.
 *
 * 2.	The default RC file is retrieved by looking at the $HOME
 *	variable. This will fail on non-POSIX systems with
 *	HT_FR_FAIL_IO_ENV_UNDEF since there is no such environment
 *	variable.
 *
 * 3.	If the default RC file doesn't exist or can't be read (but $HOME
 *	exists), then it will return HT_FR_FAIL_IO_FILE_R.
 *	It does not automatically create a default RC file.
 */
struct game *
alloc_game_read (
	const char *			file_rc,
	struct ht_line * const		line,
	enum HT_FR * const		fr,
	enum HT_FAIL_RREADER * const	fail_read
	)
{
	FILE *			file	= NULL;
	struct rread_info *	info	= NULL;
	struct game *		g	= NULL;

#ifndef HT_UNSAFE
	* fail_read	= HT_FAIL_RREADER_NONE;	/* Should already be. */
	if (NULL == line || NULL == file_rc || NULL == fr)
	{
		* fr	= HT_FR_FAIL_NULLPTR;
		return NULL;
	}
#endif

	file	= fopen_file_rc(file_rc, fr);
	if (HT_FR_SUCCESS != * fr || NULL == file)
	{
#ifndef HT_UNSAFE
		if (NULL != file)
		{
			(void) fclose(file);
		}
#endif
		return	NULL;
	}

	info	= alloc_rread_info();
	if (NULL == info)
	{
		(void) fclose	(file);
		* fr		= HT_FR_FAIL_ALLOC;
		return		NULL;
	}
	rread_info_init(info);

	while (0 == feof(file))
	{
		* fr	= ht_line_readline(file, line);
		if (HT_FR_SUCCESS != * fr)
		{
			(void) fclose	(file);
			free_rread_info	(info);
			if (NULL != g)
			{
				free_game	(g);
			}
			return	NULL;
		}
#ifndef HT_UNSAFE
		* fr	= HT_FR_NONE;
#endif

		if (0 != ferror(file))
		{
			* fr	= HT_FR_FAIL_IO_FILE_R;
			(void) fclose	(file);
			free_rread_info	(info);
			if (NULL != g)
			{
				free_game	(g);
			}
			return	NULL;
		}

		if (ht_line_empty(line))
		{
			continue;
		}

		* fr	= parseline(line, info, g, fail_read);
		if (HT_FR_SUCCESS != * fr)
		{
			(void) fclose	(file);
			free_rread_info	(info);
			if (NULL != g)
			{
				free_game	(g);
			}
			return	NULL;
		}
#ifndef HT_UNSAFE
		* fr	= HT_FR_NONE;
#endif

		if	(NULL == g
		//&&	(NULL			!= info->id) || (NULL != info->hash)
		&&	NULL			!= info->name
		&&	NULL			!= info->path
		&&	HT_BOARDPOS_NONE	!= info->bwidth
		&&	HT_BOARDPOS_NONE	!= info->bheight)
		{
			g = alloc_game(info->bwidth, info->bheight);
			if (NULL == g)
			{
				* fr		= HT_FR_FAIL_ALLOC;
				(void) fclose	(file);
				free_rread_info	(info);
				return		NULL;
			}
			game_init(g);
			/*
			 * g->rules is now responsible for path, name
			 * and id.
			 */
			g->rules->id	= info->id;
			// TODO fill g->rules->hash (compute hash function
			// from contents of entire file)
			g->rules->name	= info->name;
			g->rules->path	= info->path;
			info->id	= NULL;
			info->name	= NULL;
			info->path	= NULL;
		}
	}

	/*
	 * Check if we could at least allocate a game struct.
	 */
	if (HT_FR_SUCCESS == * fr || HT_FR_NONE == * fr)
	{
		if (NULL == g)
		{
			* fail_read	= HT_FAIL_RREADER_INCOMP;
			* fr		= HT_FR_FAIL_RREADER;
		}
		else
		{
			* fr		= HT_FR_SUCCESS;
		}
	}

	if (0 != fclose(file))
	{
		/*
		 * Don't overwrite a previous fail value.
		 */
		* fr	= HT_FR_SUCCESS != * fr ?
			* fr : HT_FR_FAIL_IO_FILE_R;
		if (NULL != g)
		{
			free_game	(g);
			g		= NULL;
		}
	}
	free_rread_info	(info);
	return		g;
}

