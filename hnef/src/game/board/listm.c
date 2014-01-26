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

#include "listm.h"

/*
 * Grow factor for realloc when the array needs to grow.
 */
/*@unchecked@*/
static
const float HT_LISTM_MEMC_GROWF = 1.5f;

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
struct listm *
alloc_listm (
	const size_t capc
	)
{
	struct listm * HT_RSTR	l	= NULL;

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
free_listm (
	struct listm * const l
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
listm_grow (
/*@in@*/
/*@notnull@*/
	struct listm * const l
	)
/*@modifies * l@*/
{
	struct move * elems_new	= NULL;
	size_t capc_new	= (size_t)
		((float)l->capc * HT_LISTM_MEMC_GROWF);
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
listm_add (
	struct listm * const	l,
	const unsigned short	pos,
	const unsigned short	dest
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
		const enum HT_FR fr	= listm_grow(l);
		if (HT_FR_SUCCESS != fr)
		{
			return fr;
		}
	}
	l->elems[l->elemc].pos	= pos;
	l->elems[l->elemc].dest	= dest;
	l->elemc++;
	return HT_FR_SUCCESS;
}

/*
 * Clears the list from elements, but does not reduce capc.
 */
void
listm_clear (
	struct listm * const HT_RSTR l
	)
{
	l->elemc	= 0;
}

/*
 * Moves l->elems[index] to l->elems[0], and moves all elements that
 * were previously at l->elems[0] to l->elems[index - 1] one index
 * forward.
 *
 * Returns HT_FR_FAIL_ILL_ARG if index >= l->elemc.
 */
enum HT_FR
listm_swaptofr
	(
	struct listm * const HT_RSTR	l,
	const size_t			index
	)
{
	size_t		i, iprev;
	struct move	mtmp;
#ifndef HT_UNSAFE
	if (NULL == l)
	{
		return	HT_FR_FAIL_NULLPTR;
	}
	else
#endif
/* splint incomplete logic */ /*@i1@*/\
	if (index >= l->elemc)
	{
		return	HT_FR_FAIL_ILL_ARG;
	}

	for (i = (size_t)index; i > 0; i--)
	{
		iprev	= i - 1;

		mtmp.pos		= l->elems[iprev].pos;
		mtmp.dest		= l->elems[iprev].dest;

		l->elems[iprev].pos	= l->elems[i].pos;
		l->elems[iprev].dest	= l->elems[i].dest;

		l->elems[i].pos		= mtmp.pos;
		l->elems[i].dest	= mtmp.dest;
	}
	return	HT_FR_SUCCESS;
}

/*
 * Removes the element at index.
 *
 * *	All elements after index in the list are offset by -1 backwards
 *	(so the element that was at index + 1 will now be at index, and
 *	so on).
 *
 * *	l->elemc decreases by -1.
 *
 * Returns HT_FR_FAIL_ILL_ARG if index >= l->elemc.
 */
enum HT_FR
listm_remove
	(
	struct listm * const HT_RSTR	l,
	const size_t			index
	)
{
	size_t		i, inext;
	struct move	mtmp;
#ifndef HT_UNSAFE
	if (NULL == l)
	{
		return	HT_FR_FAIL_NULLPTR;
	}
	else
#endif
/* splint incomplete logic */ /*@i1@*/\
	if (index >= l->elemc)
	{
		return	HT_FR_FAIL_ILL_ARG;
	}

	for (i = (size_t)index; i + 1 < l->elemc; i++)
	{
		inext	= i + 1;

		mtmp.pos		= l->elems[inext].pos;
		mtmp.dest		= l->elems[inext].dest;

		l->elems[inext].pos	= l->elems[i].pos;
		l->elems[inext].dest	= l->elems[i].dest;

		l->elems[i].pos		= mtmp.pos;
		l->elems[i].dest	= mtmp.dest;
	}

	l->elemc--;

	return	HT_FR_SUCCESS;
}

