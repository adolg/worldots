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
#include <string.h>		/* strlen, memset */

#include "boolt.h"		/* HT_BOOL */
#include "config.h"		/* HT_RSTR */
#include "iotextparse.h"

/*
 * Escape character.
 */
/*@unchecked@*/
static
const char HT_LINE_CH_ESCAPE = '\\';

/*
 * Comment character.
 */
/*@unchecked@*/
static
const char HT_LINE_CH_COMMENT = '#';

/*
 * Initial capacity for ht_str (ht_str->capc).
 *
 * This value is 3 excluding '\0', which is tuned for reading ruleset
 * files where there will be a large array (board width * board height
 * long) consisting of up to three chars (the unsigned 8 bit value
 * "128", which is the maximum value of HT_BIT_U8). Otherwise there will
 * be a lot of unnecessary calls to realloc ht_str->chars (as
 * ht_str->capc needs to grow from 1 -> 2 -> 3 -> 4, if HT_STR_MEMC_DEF
 * would start on 1).
 */
/*@unchecked@*/
static
const size_t HT_STR_MEMC_DEF = (size_t)4;

/*
 * Grow factor for realloc when ht_str->capc needs to grow.
 */
/*@unchecked@*/
static
const float HT_STR_MEMC_GROWF = 1.5f;

/*
 * Grow factor for realloc when ht_line->capc needs to grow.
 */
/*@unchecked@*/
static
const float HT_LINE_MEMC_GROWF = 1.5f;

/*
 * Clears s->chars, without de-allocating anything.
 * This will make strlen return 0 for s->chars.
 */
static
void
ht_str_clear (
/*@notnull@*/
/*@partial@*/
	struct ht_str * const s
	)
/*@modifies s->chars@*/
{
#ifndef HT_UNSAFE
	memset(s->chars, (int)'\0', s->capc);
#else
	s->chars[0]	= '\0';
#endif
}

/*
 * Allocates ht_str->chars in l->words[it] to l->words[l->capc] to cap
 * characters.
 *
 * Upon returning failure, everything that was allocated by this
 * function is de-allocated and set to NULL.
 */
static
enum HT_FR
alloc_ht_str_chars (
/*@notnull@*/
/*@partial@*/
	struct ht_line * const	l,
	const size_t		cap,
	const size_t		it
	)
/*@modifies l->words@*/
{
	size_t i = it;
	for (; i < l->capc; i++)
	{
		struct ht_str * const s	= & l->words[i];
		s->capc			= cap;
		s->chars = malloc(sizeof(* s->chars) * s->capc);
		if (NULL == s->chars)
		{
			size_t	j;
			for (j = it; j < i; j++)
			{
				free		(l->words[j].chars);
				l->words[j].chars	= NULL;
			}
			return	HT_FR_FAIL_ALLOC;
		}
		/*
		 * Set '\0' as first character.
		 */
		ht_str_clear(s);
	}
	return	HT_FR_SUCCESS;
}

/*
 * Allocates a ht_line with:
 *
 * *	ht_line->capc ht_line_capc.
 *
 * *	ht_line->wordc 0.
 *
 * *	Each ht_str pointed to by ht_line->words is initialized to:
 *
 *	*	ht_str->chars[0] = L'\0'.
 *
 *	*	ht_str->capc ht_str_capc.
 *
 * When a ht_str is added to ht_line->words because of automatic growth
 * in ht_line_readline, it will get ht_str->capc = HT_STR_MEMC_DEF --
 * not ht_str_capc!
 *
 * Both capacities must be at least 1, or NULL will be returned.
 *
 * When you are going to read ruleset files, you are recommended to give
 * ht_line_capc = 3 and ht_str_capc = 18 (at least), because the first
 * three words will likely be up to 17 characters in length (consider
 * that the ruleset arguments in lang.c may be translated and thus even
 * longer). As ht_line->capc later grows, the ht_str->capc of every word
 * will be HT_STR_MEMC_DEF (4), which is suitable for reading pieces and
 * squares HT_BIT_U8 arrays (and whatever bitmasks and size_t indices
 * that are used for other arguments).
 */
struct ht_line *
alloc_ht_line (
	const size_t	ht_line_capc,
	const size_t	ht_str_capc
	)
{
	struct ht_line * l;

	if (ht_line_capc < (size_t)1 || ht_str_capc < (size_t)1)
	{
		return NULL;
	}

	l	= malloc(sizeof(* l));
	if (NULL == l)
	{
		return	NULL;
	}

	l->capc		= ht_line_capc;
	l->wordc	= 0;
	l->words	= malloc(sizeof(* l->words) * l->capc);
	if (NULL == l->words)
	{
		free	(l);
		return	NULL;
	}

	if (HT_FR_SUCCESS != alloc_ht_str_chars(l, ht_str_capc, 0))
	{
		free_ht_line	(l);
		return		NULL;
	}

	return l;
}

/*
 * Defined in C99 but this is C89. In C99, remove this function and
 * use "isblank" from <ctype.h> instead.
 * A blank char is a space or tab.
 */
static
HT_BOOL
ht_isblank (
	const char ch
	)
/*@modifies nothing@*/
{
	return ' ' == ch || '\t' == ch;
}

/* Clears l->words, without de-allocating anything. */
static
void
ht_line_clear (
/*@notnull@*/
	struct ht_line * const HT_RSTR l
	)
/*@modifies l->wordc@*/
{
	l->wordc	= (size_t)0;
}

/*
 * Grows l by HT_LINE_MEMC_GROWF, or leaves it intact upon failing.
 */
static
enum HT_FR
ht_line_grow (
/*@notnull@*/
/*@partial@*/
	struct ht_line * const l
	)
/*@modifies l->words, l->capc@*/
{
	struct ht_str *	words_new;
	const size_t	capc_old	= l->capc;
	size_t capc_new = (size_t)((float)capc_old
		* HT_LINE_MEMC_GROWF);
	if (capc_new <= capc_old)
	{
		capc_new = capc_old + 1;
	}

/* splint realloc */ /*@i1@*/\
	words_new = realloc(l->words, sizeof(* l->words) * capc_new);
	if (NULL == words_new)
	{
		return HT_FR_FAIL_ALLOC;
	}
	l->words	= words_new;
	l->capc		= capc_new;

	return alloc_ht_str_chars(l, HT_STR_MEMC_DEF, capc_old);
}

static
enum HT_FR
ht_str_grow (
/*@notnull@*/
	struct ht_str *	s,
	const size_t	capc_req
	)
/*@modifies s->chars, s->capc@*/
{
	const size_t	capc_old = s->capc;
	char *		chars_new;
	size_t capc_new = (size_t)((float)capc_old
		* HT_STR_MEMC_GROWF);
	if (capc_new <= capc_req)
	{
		capc_new = capc_req + 1;
	}

	chars_new = realloc(s->chars, sizeof(* s->chars) * capc_new);
	if (NULL == chars_new)
	{
/* splint realloc */ /*@i2@*/\
		return HT_FR_FAIL_ALLOC;
	}
	s->chars	= chars_new;
	s->capc		= capc_new;

#ifndef HT_UNSAFE
	{
		size_t	i;
		for (i = capc_old; i < capc_new; i++)
		{
			s->chars[i]	= '\0';
		}
	}
#endif
/* splint realloc */ /*@i1@*/\
	return HT_FR_SUCCESS;
}

/*
 * Adds char ch to s->chars, and appends '\0' after it. Assumes that the
 * s->chars[charc] was previously '\0'.
 *
 * Upon failing to grow, s is intact at its previous value.
 */
static
enum HT_FR
ht_str_addchar (
/*@in@*/
/*@notnull@*/
	struct ht_str *	s,
	const char	ch,
/*@in@*/
/*@notnull@*/
	size_t * const	charc
	)
/*@modifies s->chars, s->capc, * charc@*/
{
#ifndef HT_UNSAFE
	if (* charc >= s->capc || '\0' != s->chars[* charc])
	{
		return HT_FR_FAIL_ILL_ARG;
	}
#endif

	if (* charc + 1 >= s->capc)
	{
		const enum HT_FR fr = ht_str_grow(s, * charc + 1);
		if (HT_FR_SUCCESS != fr)
		{
			return fr;
		}
	}

	s->chars[* charc]	= ch;
	s->chars[++(* charc)]	= '\0';
	return	HT_FR_SUCCESS;
}

/*
 * Reads and ignores all characters up until "to" in f.
 */
static
enum HT_FR
ht_line_ignore (
/*@notnull@*/
	FILE * const	f,
	const char	to
	)
/*@globals fileSystem, errno@*/
/*@modifies fileSystem, errno, * f@*/
{
	int read;
	while (EOF != (read = fgetc(f)) && to != (char)read)
	{
		/*
		 * Ignore every char until to.
		 */
	}
	return 0 == ferror(f) ? HT_FR_SUCCESS : HT_FR_FAIL_IO_FILE_R;
}

static
enum HT_FR
ht_line_parsechar (
/*@notnull@*/
	FILE * const		f,
/*@notnull@*/
	struct ht_line * const	l,
	const char		ch,
/*@in@*/
/*@notnull@*/
	size_t * const		charc, /* l->words[x].chars to '\0'. */
/*@in@*/
/*@notnull@*/
	HT_BOOL * const HT_RSTR	escape,
/*@in@*/
/*@notnull@*/
	HT_BOOL * const HT_RSTR	eol
	)
/*@globals fileSystem, errno@*/
/*@modifies fileSystem, errno, * f, * l, * charc, * escape, * eol@*/
{
	if (!(* escape))
	{
		if (HT_LINE_CH_ESCAPE == ch)
		{
			* escape = HT_TRUE;
			return	HT_FR_SUCCESS;
		}
		else if ('\n' == ch)
		{
			* eol	= HT_TRUE;
			return	HT_FR_SUCCESS;
		}
		else if (HT_LINE_CH_COMMENT == ch)
		{
			* eol	= HT_TRUE;
			return ht_line_ignore(f, '\n');
		}
/* splint incomplete logic */ /*@i1@*/\
		else if (ht_isblank(ch))
		{
			if (* charc > 0)
			{
				if (l->wordc + 1 >= l->capc)
				{
					enum HT_FR fr = ht_line_grow(l);
					if (HT_FR_SUCCESS != fr)
					{
						return fr;
					}
				}
				l->wordc++;
				ht_str_clear(& l->words[l->wordc - 1]);
				* charc	= 0;
			}
			return	HT_FR_SUCCESS;
		}
	}
	else	/* escape */
	{
		* escape	= HT_FALSE;
		if ('\n' == ch)
		{
			return	HT_FR_SUCCESS;
		}
	}
	return ht_str_addchar(& l->words[l->wordc - 1], ch, charc);
}

/*
 * Reads a line and stores the words in l, growing it as needed.
 *
 * The file should be opened as "r" (at least).
 */
enum HT_FR
ht_line_readline (
	FILE * const		f,
	struct ht_line * const	l
	)
{
	enum HT_FR	fr	= HT_FR_NONE;
	int		ch;
	size_t		charc	= 0;
	HT_BOOL	escape,	eol;
	/*
	 * l->words[x].chars[0] = '\0'.
	 */

#ifndef HT_UNSAFE
	if (NULL == f || NULL == l)
	{
		return HT_FR_FAIL_NULLPTR;
	}
/* splint incomplete logic */ /*@i2@*/\
	else if (l->capc < (size_t)1 || l->words[0].capc < (size_t)1)
	{
		return HT_FR_FAIL_ILL_ARG;
	}
#endif

	ht_line_clear	(l);
	l->wordc++;
	ht_str_clear	(& l->words[0]);
	charc		= 0;
	escape	= eol	= HT_FALSE;
	/*
	 * l->wordc = 0.
	 */

	while (EOF != (ch = fgetc(f)))
	{
		fr = ht_line_parsechar(f, l, (char)ch, & charc,
			& escape, & eol);
		if (HT_FR_SUCCESS != fr)
		{
			return fr;
		}
#ifndef HT_UNSAFE
		fr	= HT_FR_NONE;
#endif
		if (eol)
		{
			break;
		}
	}

	return 0 == ferror(f) ? HT_FR_SUCCESS : HT_FR_FAIL_IO_FILE_R;
}

/*
 * Frees l and all owned ht_strs.
 */
void
free_ht_line (
	struct ht_line * const l
	)
{
	if (NULL != l->words)
	{
		size_t i;
		for (i = 0; i < l->capc; i++)
		{
			if (NULL != l->words[i].chars)
			{
				free	(l->words[i].chars);
			}
		}
		free	(l->words);
	}
	free	(l);
}

HT_BOOL
ht_line_empty (
	const struct ht_line * const l
	)
{
	return (size_t)1 == l->wordc
	&& strlen(l->words[0].chars) < (size_t)1 ? HT_TRUE : HT_FALSE;
}

