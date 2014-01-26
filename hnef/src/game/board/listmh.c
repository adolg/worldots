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
#include <stdlib.h>	/* malloc, free */

#include "listmh.h"

/*
 * Grow factor for realloc when the array needs to grow.
 */
/*@unchecked@*/
static
const float HT_LISTMH_MEMC_GROWF = 1.5f;

/*
 * Allocates the returned pointer and its elems variable.
 *
 * Upon returning NULL, nothing is allocated because we're out of
 * dynamic memory.
 *
 * Upon returning non-NULL, the listm pointed to by the returned pointer
 * and its elems variable are allocated, and must be freed by
 * free_listm.
 *
 * The returned elems are uninitialized and elemc is 0.
 * Returns NULL if capc is 0.
 */
struct listmh *
alloc_listmh (
	const size_t capc
	)
{
	struct listmh * HT_RSTR	l	= NULL;

	if (capc < (size_t)1)
	{
		return	NULL;
	}

	l	= malloc(sizeof(* l));
	if (NULL == l)
	{
		return	NULL;
	}

	l->elems		= malloc(sizeof(* l->elems) * capc);
	if (NULL == l->elems)
	{
		free	(l);
		l	= NULL;
		return	NULL;
	}

	l->elemc	= 0;
	l->capc		= capc;

	return l;
}

/*
 * Frees everything in l and l itself.
 */
void
free_listmh (
	struct listmh * const l
	)
{
	if (NULL != l->elems)
	{
		free	(l->elems);
	}
	free	(l);
}

/*
 * Upon HT_FR_FAIL_ALLOC, the old l->elems is still valid and unchanged,
 * and l->elemc and l->capc are unchanged as well.
 */
static
enum HT_FR
listmh_grow (
/*@in@*/
/*@notnull@*/
	struct listmh * const	l,
	size_t			capc_new
	)
/*@modifies * l@*/
{
	struct moveh * elems_new	= NULL;
	if (capc_new <= l->capc)
	{
		capc_new = l->capc + 1;
	}

	elems_new = realloc(l->elems, sizeof(* l->elems) * capc_new);
	if (NULL == elems_new)
	{
/* splint realloc */ /*@i2@*/\
		return HT_FR_FAIL_ALLOC;
	}
	l->elems	= elems_new;
	l->capc		= capc_new;

/* splint realloc */ /*@i1@*/\
	return HT_FR_SUCCESS;
}

/*
 * Grows if needed.
 */
enum HT_FR
listmh_add (
	struct listmh * const	l,
	const unsigned short	pos,
	const unsigned short	dest,
	const HT_BOOL		irrev
	)
{
#ifndef HT_UNSAFE
	if (NULL == l->elems)
	{
		return HT_FR_FAIL_NULLPTR;
	}
#endif
	if (l->elemc + 1 > l->capc)
	{
		const enum HT_FR fr	= listmh_grow(l, (size_t)
			((float)l->capc * HT_LISTMH_MEMC_GROWF));
		if (HT_FR_SUCCESS != fr)
		{
			return fr;
		}
	}
	l->elems[l->elemc].irrev	= irrev;
	l->elems[l->elemc].pos		= pos;
	l->elems[l->elemc].dest		= dest;
	l->elemc++;
	return HT_FR_SUCCESS;
}

/*
 * Copies src into dest, possibly growing dest if dest->capc isn't large
 * enough to fit all elements (elemc) in src.
 *
 * Does not copy the garbage elements that are beyond src->elemc but
 * within src->capc, and does not allocate space for them.
 */
enum HT_FR
listmh_copy (
	const	struct listmh * const	src,
		struct listmh * const	dest
	)
{
	size_t		i;
	if (src->elemc > dest->capc)
	{
		const enum HT_FR fr	= listmh_grow(dest, src->elemc);
		if (HT_FR_SUCCESS != fr)
		{
			return	fr;
		}
	}
	dest->elemc	= src->elemc;
	for (i = (size_t)0; i < src->elemc; i++)
	{
		dest->elems[i].pos	= src->elems[i].pos;
		dest->elems[i].dest	= src->elems[i].dest;
		dest->elems[i].irrev	= src->elems[i].irrev;
	}
	return	HT_FR_SUCCESS;
}

