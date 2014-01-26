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
#ifdef HT_AI_MINIMAX_HASH

#include <errno.h>	/* errno */
#include <math.h>	/* sqrt */

#include "aimhash.h"
#include "config.h"	/* Validate HT_AI_MINIMAX_HASH_* values. */
#include "types.h"	/* type_index_get */

static
unsigned int
zhash (
/*@in@*/
/*@notnull@*/
	const struct zhashtable * const HT_RSTR	ht,
/*@in@*/
/*@notnull@*/
	unsigned int * * const HT_RSTR		zob,
/*@in@*/
/*@notnull@*/
	const struct board * const HT_RSTR	b
	)
/*@modifies nothing@*/
{
	unsigned int	key	= 0;
	unsigned short	i, j;
	for (i = (unsigned short)0; i < ht->z_i; i++)
	{
		for (j = (unsigned short)0; j < ht->z_j; j++)
		{
			if (HT_BIT_U8_EMPTY != b->pieces[j])
			{
				key ^= zob[type_index_get
					(b->pieces[j])][j];
			}
		}
	}
	return	key;
}

unsigned int
zhashkey (
	const struct zhashtable * const HT_RSTR	ht,
	const struct board * const HT_RSTR	b
	)
{
	return	zhash(ht, ht->zobrist_key, b);
}

unsigned int
zhashlock (
	const struct zhashtable * const HT_RSTR	ht,
	const struct board * const HT_RSTR	b
	)
{
	return	zhash(ht, ht->zobrist_lock, b);
}

static
size_t
zhashindex (
/*@in@*/
/*@notnull@*/
	const struct zhashtable * const HT_RSTR	ht,
	unsigned int				key
	)
/*@modifies nothing@*/
{
	/*
	 * `key & (ht->arrc - 1)` gives more collisions.
	 */
	return	(size_t)key % (size_t)ht->arrc;
}

/*
 * node->used is set to false.
 */
static
void
zhashnode_set (
/*@in@*/
/*@notnull@*/
	struct zhashnode * const HT_RSTR	node,
	const unsigned int			hashkey,
	const unsigned int			hashlock,
	const int				value,
	const enum ZVALUE_TYPE			value_type,
	unsigned short				p_turn,
	unsigned short				depthleft
	)
/*@modifies * node@*/
{
	node->hashkey		= hashkey;
	node->hashlock		= hashlock;
	node->value		= value;
	node->value_type	= value_type;
	node->p_turn		= p_turn;
	node->depthleft		= depthleft;
	node->used		= HT_FALSE;
}

/*
 * Checks if node may be overwritten with the given new values.
 *
 * Positive:	node may be overwritten by new values.
 * 0:		node may not be overwritten.
 * Negative:	new values can be discarded.
 */
static
int
zhashnode_action (
/*@in@*/
/*@notnull@*/
	const struct zhashnode * const HT_RSTR	node,
/*@in@*/
/*@notnull@*/
	const struct board * const HT_RSTR	b,
	const unsigned int			key,
	const unsigned int			lock,
	const enum ZVALUE_TYPE			value_type,
	unsigned short				depthleft
	)
/*@modifies nothing@*/
{
	if (HT_AIM_ZHT_VACANT == node->value_type)
	{
		return	1;
	}
	else
	{
		if (key == node->hashkey && lock == node->hashlock
			&& b->turn == node->p_turn)
		{
			/*
			 * Same board. Candidate for overwriting.
			 */
			if (HT_AIM_ZHT_EXACT == value_type
			&& (HT_AIM_ZHT_ALPHA == node->value_type
				|| HT_AIM_ZHT_BETA == node->value_type))
			{
				/*
				 * New value is more exact.
				 */
				return	1;
			}
			else if (value_type == node->value_type)
			{
				/*
				 * Same value type.
				 */
				return depthleft > node->depthleft ?
					1 : -1;
			}
			else
			{
				/*
				 * Different value type -- never allowed
				 * to overwrite.
				 */
				return	0;
			}
		}
		else
		{
			/*
			 * Not the same board -- may never overwrite.
			 */
			return	0;
		}
	}
}

/*
 * Sets b as a new hash in ht with value, value_type and depthleft.
 *
 * This may overwrite an existing node, if the value of the new one is
 * more exactly known and they are the same board position.
 *
 * NOTE:	If there is no more space in zhashcols->memchunk, then
 *		the node is not added, with no indication of failure.
 *
 * NOTE:	If value_type is HT_AIM_ZHT_VACANT, then the node is not
 *		added, with no indication of failure.
 */
void
zhashtable_put (
	struct zhashtable * const HT_RSTR	ht,
	const struct board * const HT_RSTR	b,
	const int				value,
	const enum ZVALUE_TYPE			value_type,
	unsigned short				depthleft
	)
{
	unsigned int			key	= zhashkey	(ht, b);
	unsigned int			lock	= zhashlock	(ht, b);
	size_t				index	= zhashindex(ht, key);
	struct zhashnode * HT_RSTR	node	= & ht->arr[index];

#ifndef HT_AI_MINIMAX_UNSAFE
	if (HT_AIM_ZHT_VACANT == value_type)
	{
		return;
	}
#endif

	for (;;)
	{
		const int act = zhashnode_action(node, b, key, lock,
			value_type, depthleft);
		if (act > 0)
		{
			/*
			 * Overwrite existing node with new values.
			 */
			zhashnode_set(node, key, lock, value,
				value_type, b->turn, depthleft);
			return;
		}
/* splint incomplete logic */ /*@i1@*/\
		else if (act < 0)
		{
			/*
			 * Discard new values.
			 */
			return;
		}
		/*
		 * Node may not be overwritten, nor discarded. Check
		 * next node.
		 */

		if (NULL != node->next)
		{
			node	= node->next;
		}
		else
		{
			break;
		}
	}

	/*
	 * No existing node could be overwritten, so see if there's room
	 * in memchunk for another one.
	 */
	if (ht->cols->elemc < ht->cols->capc)
	{
		node->next = & ht->cols->memchunk[ht->cols->elemc++];
		zhashnode_set(node->next, key, lock, value, value_type,
			b->turn, depthleft);
		/*
		 * NOTE:	We don't need to set node->next->next
		 *		to NULL since it should have been done
		 *		when clearing old nodes.
		 */
	}
	/*
	 * NOTE:	else: memchunk (collision list space) is
	 *		depleted, so we can't add any more nodes.
	 *		Increasing HT_AI_MINIMAX_HASH_MEM_COL would
	 *		help if this happens.
	 */
}

struct zhashnode *
zhashtable_get (
	const struct zhashtable * const HT_RSTR	ht,
	const unsigned int			key,
	const unsigned int			lock,
	const unsigned short			p_turn
	)
{
	struct zhashnode * HT_RSTR node =
		& ht->arr[zhashindex(ht, key)];
	if (HT_AIM_ZHT_VACANT == node->value_type)
	{
		return	NULL;
	}
	do
	{
		if (key == node->hashkey && lock == node->hashlock
		&& p_turn == node->p_turn)
		{
			break;
		}
	} while (NULL != (node = node->next));
	return	node;
}

static
void
zhashnode_copy (
/*@in@*/
/*@notnull@*/
	const struct zhashnode * const HT_RSTR	src,
/*@in@*/
/*@notnull@*/
	struct zhashnode * const HT_RSTR	dest
	)
/*@modifies * dest@*/
{
	/*
	 * Note that src->next points to a zhashnode that's either in
	 * zhashtable->arr, or zhashtable->cols->memchunk. Even though
	 * the pointer is lost, it doesn't matter because it will all be
	 * freed.
	 */
	dest->next		= src->next;
	dest->value_type	= src->value_type;
	dest->used		= src->used;
	dest->hashkey		= src->hashkey;
	dest->hashlock		= src->hashlock;
	dest->value		= src->value;
	dest->p_turn		= src->p_turn;
	dest->depthleft		= src->depthleft;
}

/*
 * zhashtable_clearunused uses the following method to remove unused
 * nodes:
 *
 * 1.	Swap the node to be removed (R) with the last used node in
 *	memchunk (L). This is done by simply copying L -> R, and setting
 *	R->next to NULL.
 *
 * 2.	When the nodes have been "swapped", the problem is that L is
 *	still pointed to by some zhashnode->next, since the node is in
 *	use. This is always the case, unless L == R (which happens
 *	rarely). If it is the case, then we have to find the node which
 *	points to L, and make it point to R instead, since L is replaced
 *	by R.
 *
 *	If it's not the case, then we can simply remove L and R since
 *	they are the same (and they are both the last node in memchunk),
 *	and we don't have to update stray nodes.
 *
 * 3.	The node which points to L is in the collision lists in bucket
 *	zhashindex(ht, L->hashkey), since only colliding nodes point to
 *	each other by zhashnode->next, and the node we're looking for
 *	points to L. Thus they are in the same bucket and that bucket's
 *	index is the zhashindex of L->hashkey. So all we have to do is
 *	go through that collision list and when we find the pointer to
 *	L (zstray_old), then make it point to R (zstray_new) instead.
 *
 * This gives O(1) removal of unused nodes, without having to make the
 * collision lists doubly linked, and without having to sacrifice O(1)
 * insertion.
 */
static
enum HT_FR
zhashnode_update_stray (
/*@in@*/
/*@notnull@*/
	struct zhashtable * const HT_RSTR	ht,
/*@in@*/
/*@notnull@*/
	const struct zhashnode * const HT_RSTR	zstray_old,
/*@dependent@*/
/*@in@*/
/*@notnull@*/
	struct zhashnode * const HT_RSTR	zstray_new
	)
/*@modifies * ht@*/
{
	const size_t zstray_ind	= zhashindex(ht, zstray_old->hashkey);
	struct zhashnode * HT_RSTR zstray_list = & ht->arr[zstray_ind];

	if (zstray_old == zstray_new)
	{
		return	HT_FR_FAIL_ILL_STATE;
	}

	while (NULL != zstray_list)
	{
		if (zstray_list->next == zstray_old)
		{
			zstray_list->next = zstray_new;
			return	HT_FR_SUCCESS;
		}
		zstray_list = zstray_list->next;
	}
	/*
	 * Not found!
	 */
	return	HT_FR_FAIL_ILL_STATE;
}

/*
 * Removes noden (node->next) from node's collision list, and sets
 * node->next to noden->next (node->next->next).
 *
 * This is done by swapping noden with the last node from memchunk,
 * removing the last node from memchunk, and updating the stray pointer,
 * if any.
 *
 * *	noden should be node->next.
 *
 * *	Therefore noden should be in node's collision list, following
 *	node.
 *
 * This shrinks memchunk.
 */
static
enum HT_FR
zhashnode_remove_next (
/*@in@*/
/*@notnull@*/
	struct zhashtable * const		ht,
/*@in@*/
/*@notnull@*/
	struct zhashnode * const HT_RSTR	node,
/*@dependent@*/
/*@in@*/
/*@notnull@*/
	struct zhashnode * const		noden
	)
/*@modifies * ht, * node, * noden@*/
{
	struct zhashnode *	last	= NULL;
	if (NULL == noden)
	{
		return	HT_FR_FAIL_NULLPTR;
	}
	node->next	= noden->next;

	if (ht->cols->elemc < (size_t)1)
	{
		return	HT_FR_FAIL_ILL_STATE;
	}
	last		= & ht->cols->memchunk[--ht->cols->elemc];

	/*
	 * "Swap" last and noden, by copying last into noden and then
	 * reducing elemc.
	 */
	if (last != noden)
	{
		zhashnode_copy	(last, noden);
		last->next	= NULL;
		return		zhashnode_update_stray(ht, last, noden);
	}
	else
	{
		/*
		 * noden and last are the same node. Just remove its
		 * reference (which we saved into node->next) and it can
		 * be considered deleted.
		 */
		last->next	= NULL;
		return		HT_FR_SUCCESS;
	}
}

/*
 * Clears all unused nodes in ht. Then sets all used nodes to unused, so
 * that they will be cleared the next time this function is called.
 *
 * For details on exactly how this is done, see the descriptions of
 * zhashnode_remove_next and zhashnode_update_stray.
 */
enum HT_FR
zhashtable_clearunused (
	struct zhashtable * const	ht
	)
{
	size_t i;
	for (i = (size_t)0; i < ht->arrc; i++)
	{
		struct zhashnode *	node	= & ht->arr[i];
		struct zhashnode *	noden	= node->next;
		if (HT_AIM_ZHT_VACANT == node->value_type)
		{
			continue;
		}

		/*
		 * First check all nodes that follow the first (id est
		 * all collisions).
		 */
		while (NULL != noden)
		{
			if (noden->used)
			{
				noden->used	= HT_FALSE;
				node		= noden;
				noden		= node->next;
			}
			else
			{
/* splint thinks noden can be NULL */
				const enum HT_FR fr =
					zhashnode_remove_next(ht, node,
/*@i1@*/				noden);
				if (HT_FR_SUCCESS != fr)
				{
					return	fr;
				}
				noden		= node->next;
			}
		}

		/*
		 * All collisions have been checked. Check first node.
		 */
		node	= & ht->arr[i];
		if (node->used)
		{
			node->used	= HT_FALSE;
		}
		else
		{
			noden	= node->next;
			if (NULL == noden)
			{
				/*
				 * There are no collisions: just set
				 * this one to unused and we're done.
				 */
				node->value_type = HT_AIM_ZHT_VACANT;
			}
			else
			{
				enum HT_FR	fr	= HT_FR_NONE;
				/*
				 * "Remove" this node by replacing it
				 * with its next node, and then removing
				 * the next node.
				 */
				zhashnode_copy	(noden, node);
				fr = zhashnode_remove_next(ht, node,
					noden);
				if (HT_FR_SUCCESS != fr)
				{
					return	fr;
				}
			}
		}
	}
	return	HT_FR_SUCCESS;
}

/*
 * This is used for initializing zhashtable->zobrist_key and
 * zobrist_lock.
 *
 * z_i and z_j are the desired lengths of the arrays.
 */
/*@null@*/
/*@only@*/
static
unsigned int * *
alloc_zobrist_init (
	const unsigned short	z_i,
	const unsigned short	z_j
	)
/*@globals internalState@*/
/*@modifies internalState@*/
{
	/*
	 * zobrist and zobrist_lock may be NULL.
	 */
	unsigned short i;
	unsigned int * * zobrist = malloc(sizeof(* zobrist) * z_i);
	if (NULL == zobrist)
	{
		return	NULL;
	}
	for (i = (unsigned short)0; i < z_i; i++)
	{
		unsigned short j;
		zobrist[i] = malloc(sizeof(* zobrist[i]) * z_j);
		if (NULL == zobrist[i])
		{
			unsigned short k;
			for (k = (unsigned short)0; k < i; k++)
			{
				free	(zobrist[k]);
			}
			free		(zobrist);
			return		NULL;
		}

		for (j = (unsigned short)0; j < z_j; j++)
		{
			zobrist[i][j] = (unsigned int)rand();
			zobrist[i][j] ^= (unsigned int)rand() << 15;
			zobrist[i][j] ^= (unsigned int)rand() << 30;
		}
	}
	return	zobrist;
}

/*
 * Allocates a zhashcols struct with capc HT_AI_MINIMAX_HASH_MEM_COL.
 */
/*@null@*/
/*@only@*/
static
struct zhashcols *
alloc_zhashcols_init (void)
/*@modifies nothing@*/
{
	struct zhashcols *	cols	= malloc(sizeof(* cols));
	if (NULL == cols)
	{
		return	NULL;
	}

	cols->elemc	= (size_t)0;
/* splint safer to use pointer in sizeof */
	cols->capc	= (size_t)
/*@i1@*/	(HT_AI_MINIMAX_HASH_MEM_COL / sizeof(struct zhashnode));
	if (cols->capc < (size_t)1)
	{
		free	(cols);
		return	NULL;
	}

	/*
	 * NOTE:	We depend on calloc to set zhashnode->next to
	 *		NULL by 0-initializing.
	 */
	cols->memchunk = calloc(cols->capc, sizeof(* cols->memchunk));
	if (NULL == cols->memchunk)
	{
		free	(cols);
		return	NULL;
	}

	return	cols;
}

/*
 * Checks if num is a prime.
 *
 * Returns false if num is < 2 (or can't be square-rooted due to being
 * negative).
 *
 * NOTE:	May be more approriate in num.c, but is here since it's
 *		only used for one thing.
 */
static
HT_BOOL
is_prime (
	const size_t	num
	)
/*@globals errno@*/
/*@modifies errno@*/
{
	size_t	divid;
	if (num < (size_t)2)
	{
		return	HT_FALSE;
	}
	{
		int		errno_old	= errno;
		double		tmp;
				errno		= 0;
				tmp		= sqrt((double)num);
		if (0 != errno)
		{
			/*
			 * Only happens if num is negative, which it
			 * can't be, being a size_t.
			 */
			return	HT_FALSE;
		}
		errno		= errno_old;
		divid		= (size_t)(tmp + 0.0001);
	}
	do
	{
		if (0 == num % divid)
		{
			return	HT_FALSE;
		}
	} while (--divid > (size_t)1);
	return	HT_TRUE;
}

/*
 * Finds next prime number >= num.
 *
 * NOTE:	May be more approriate in num.c, but is here since it's
 *		only used for one thing.
 */
static
size_t
next_prime_ge (
	size_t	num
	)
/*@globals errno@*/
/*@modifies errno@*/
{
	while (!is_prime(num))
	{
		num++;
	}
	return	num;
}

/*
 * Finds next prime number < num.
 *
 * NOTE:	May be more approriate in num.c, but is here since it's
 *		only used for one thing.
 */
static
size_t
next_prime_lt (
	size_t	num
	)
/*@globals errno@*/
/*@modifies errno@*/
{
	while (!is_prime(--num))
	{
	}
	return	num;
}

/*
 * 0 size is not allowed (returns NULL).
 *
 * All variables in ht->arr are 0-initialized by calloc.
 *
 * zhashtable->arr takes at most HT_AI_MINIMAX_HASH_MEM_TAB bytes
 * (plus/minus however much it takes to get zhashtable->arrc to the
 * nearest prime).
 *
 * zhashtable->cols takes at most HT_AI_MINIMAX_HASH_MEM_COL bytes.
 */
struct zhashtable *
alloc_zhashtable_init (
	const struct game * const HT_RSTR	g
	)
{
	struct zhashtable *	ht	= NULL;
	ht	= malloc(sizeof(* ht));
	if (NULL == ht)
	{
		return	NULL;
	}

/* splint safer to use pointer in sizeof */
	ht->arrc = (size_t)
/*@i1@*/	(HT_AI_MINIMAX_HASH_MEM_TAB / sizeof(struct zhashnode));
	if (ht->arrc < (size_t)1)
	{
		free	(ht);
		return	NULL;
	}

	{
		size_t	p_ge	= next_prime_ge(ht->arrc);
		size_t	p_lt	= next_prime_lt(ht->arrc);
		size_t	d_p_ge	= p_ge		- ht->arrc;
		size_t	d_p_lt	= ht->arrc	- p_lt;
		ht->arrc	= d_p_ge < d_p_lt ? p_ge : p_lt;
	}

	/*
	 * NOTE:	We depend on calloc to set zhashnode->next to
	 *		NULL and zhashnode->value_type to
	 *		HT_AIM_ZHT_VACANT by 0-initializing.
	 */
	ht->arr		= calloc(ht->arrc, sizeof(* ht->arr));
	if (NULL == ht->arr)
	{
		free	(ht);
		return	NULL;
	}

	/*
	 * Set to NULL for free_zhashtable and alloc_zobrist_init.
	 */
	ht->zobrist_key	= ht->zobrist_lock	= NULL;

	ht->cols	= alloc_zhashcols_init();
	if (NULL == ht->cols)
	{
		free_zhashtable	(ht);
		return		NULL;
	}

	ht->z_i	= g->rules->type_piecec;
	ht->z_j	= g->rules->opt_blen;

	ht->zobrist_key = alloc_zobrist_init(ht->z_i, ht->z_j);
	if (NULL == ht->zobrist_key)
	{
		free_zhashtable	(ht);
		return		NULL;
	}

	ht->zobrist_lock = alloc_zobrist_init(ht->z_i, ht->z_j);
	if (NULL == ht->zobrist_lock)
	{
		free_zhashtable	(ht);
		return		NULL;
	}

	return	ht;
}

/*
 * Frees ht and all collision lists.
 */
void
free_zhashtable (
	struct zhashtable * const ht
	)
{
	if (NULL != ht->cols)
	{
		free	(ht->cols->memchunk);
		free	(ht->cols);
	}

	if (NULL != ht->zobrist_key)
	{
		unsigned short i;
		for (i = (unsigned short)0; i < ht->z_i; i++)
		{
/* splint doesn't understand pointer to pointer */ /*@i2@*/\
			free	(ht->zobrist_key[i]);
		}
		free		(ht->zobrist_key);
	}

	if (NULL != ht->zobrist_lock)
	{
		unsigned short i;
		for (i = (unsigned short)0; i < ht->z_i; i++)
		{
/* splint doesn't understand pointer to pointer */ /*@i2@*/\
			free	(ht->zobrist_lock[i]);
		}
		free		(ht->zobrist_lock);
	}

	if (NULL != ht->arr)
	{
		free	(ht->arr);
	}
	free	(ht);
}

#endif

