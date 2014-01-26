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

#ifndef HT_AIMHASH_T_H
#define HT_AIMHASH_T_H

#include <stdlib.h>	/* size_t */

#include "boolt.h"	/* HT_BOOL */

enum ZVALUE_TYPE
{

	/*
	 * The node is not used -- it's "empty" and can be overwritten.
	 */
	HT_AIM_ZHT_VACANT	= 0,

	/*
	 * Evaluated by being a leaf-node, either by aiminimax_evaluate
	 * or by the game being over.
	 */
	HT_AIM_ZHT_EXACT,

	/*
	 * Alpha cutoff: the square is at most equal to value.
	 */
	HT_AIM_ZHT_ALPHA,

	/*
	 * Beta cutoff: the square is at least equal to value.
	 */
	HT_AIM_ZHT_BETA

};

/*
 * The uninitialized values of every variable in this struct is 0 or
 * NULL (by calloc).
 *
 * NOTE:	The elements are arranged to minimize padding, assuming
 *		the sizes of the types are:
 *
 * 			pointer > enum / HT_BOOL > int > short
 *
 *		Note that pointer >= int, and (probably) enum == int.
 *		If pointer == enum == int == 4 byte and short == 2 byte,
 *		then the total size is:
 *		4 + 4 * 2 + 4 * 3 + 2 * 2 = 28 byte.
 *
 *		That gives, for the following sizes of zhashtable->arr:
 *
 *		8 MB:	  285 714.29	(Very high load in Tablut.)
 *		16 MB:	  571 428.57
 *		32 MB:	1 142 857.1	(Usually < 0.7 load in Tablut.)
 *		64 MB:	2 285 714.3
 */
/*@exposed@*/
struct zhashnode
{

	/*
	 * Singly linked list for collissions. (We never need to
	 * traverse this list backwards.)
	 */
/*@dependent@*/
/*@null@*/
	struct zhashnode *	next;

	/*
	 * Exactness of value.
	 */
	enum ZVALUE_TYPE	value_type;

	/*
	 * Whether this positions was used during the calculation of
	 * theprevious move.
	 *
	 * Upon being used (to retrieve a value), a position is set to
	 * true.
	 *
	 * On every new move, all hashnodes that are unused are removed,
	 * and all nodes that are used are flagged as unused. This
	 * ensures that only relevant nodes will be kept.
	 *
	 * The initial (uninitialized) value is HT_FALSE.
	 */
	HT_BOOL			used;

	/*
	 * Zobrist hash (zobrist_key).
	 */
	unsigned int		hashkey;

	/*
	 * Second Zobrist hash (zobrist_lock).
	 */
	unsigned int		hashlock;

	/*
	 * Value of this position.
	 */
	int			value;

	/*
	 * Player turn for the given board.
	 */
	unsigned short		p_turn;

	/*
	 * Depth left to search.
	 */
	unsigned short		depthleft;

};

/*
 * zhashtable->arr is the hash table array. This takes (up to, due to
 * rounding) HT_AI_MINIMAX_MEM_INIT bytes.
 *
 * zhashtable->cols->memchunk is space for the collision lists. This
 * takes up to (due to growing) HT_AI_MINIMAX_MEM_COL_MAX bytes.
 *
 * The table can not grow (arrc can not increase post-allocation).
 * Therefore it's useless to keep track of load factor.
 *
 * Both zhashtable->arr and zhashcols->memchunk store zhashnodes in a
 * continuous block of memory. That means that all zhashnodes will be
 * aligned to structure padding. A zhashnode with 32 bit pointers is 28
 * bytes (making some assumptions about data types), and with 64 bit
 * pointers 32 bytes. Thus no space is wasted when putting zhashnodes in
 * arrays.
 *
 * By contrast, if we had just naively used malloc every time we need to
 * add a new collision, we would allocate 32 bytes per zhashnode, since
 * malloc aligns to 16 bytes, thus wasting 4 bytes (16 * 2 - 28) per
 * zhashnode, in addition to causing heap fragmentation (though it
 * should be insignificant unless free immediately returns memory to the
 * operating system, which it doesn't on Linux at least).
 *
 * The benefit of collision lists is fast retrieval and deletion, but
 * zhashtable_clearunused achieves nearly equally fast deletion, and the
 * use of a single non-growable array for colliding nodes means no heap
 * fragmentation since they don't have to be allocated all the time.
 * Insertion is as fast.
 */
/*@exposed@*/
struct zhashtable
{

	/*
	 * The table. An array of hashnode pointers. Length = arrc.
	 *
	 * Unused ("empty") nodes have value_type HT_AIM_ZHT_VACANT and
	 * can be overwritten when something needs to be added.
	 */
/*@notnull@*/
/*@owned@*/
	struct zhashnode *	arr;

	/*
	 * Capacity of arr.
	 */
	size_t			arrc;

	/*
	 * Memory for collisions lists.
	 */
	struct zhashcols *	cols;

	/*
	 * Numbers used to generate the zobrist keys.
	 *
	 * Length is zobrist[z_i][z_j].
	 *
	 * First dimension (i) is piece type's index from 0 to 7
	 * converted from its bit value (HT_BIT_U8), which is
	 * 1, 2, 4...128.
	 *
	 * Second dimension (j) is board position.
	 */
/*@notnull@*/
/*@owned@*/
	unsigned int * *	zobrist_key;

	/*
	 * Works exactly like zobrist_key but is generated from
	 * different random numbers.
	 */
/*@notnull@*/
/*@owned@*/
	unsigned int * *	zobrist_lock;

	/*
	 * Length of the first array in zobrist_key. This is equal to
	 * ruleset->type_piecec.
	 */
	unsigned short		z_i;

	/*
	 * Length of the second array in zobrist_key. This is equal to
	 * the board length (ruleset->pieces, or bwidth * bheight).
	 */
	unsigned short		z_j;

};

/*
 * Memory for collision lists in zhashtable.
 *
 * When a new node is added to memchunk, it's always appended last.
 *
 * When a move is removed from memchunk, the last node's (at index
 * memchunk[elemc - 1]) values are copied into the removed node, and
 * then elemc reduces by 1. This works because the nodes in memchunk are
 * randomly ordered, and only referred to from linked collision lists
 * (zhashnode->next).
 *
 * NOTE:	This has one weakness: it depends on zhashnode pointers
 *		(reached from zhashtable->arr[x].zhashnode->next)
 *		pointing to memory in zhashcols->memchunk. Thus you can
 *		never re-alloc memchunk, since that will invalidate all
 *		old pointers. You could probably re-generate them, but
 *		it's far easier to require the user to set fixed sizes
 *		for the hash table and collision lists.
 */
struct zhashcols
{

	/*
	 * Memory for colliding zhashnodes (zhashnode->next) that are
	 * not stored in zhashtable->arr.
	 *
	 * Note that nodes in memchunk are not flagged with value_type
	 * HT_AIM_ZHT_VACANT. It's not necessary since all nodes in
	 * memchunk up to `elemc - 1` will be non-vacant, and all nodes
	 * from `elemc` and onwards will be vacant.
	 */
/*@notnull@*/
/*@owned@*/
	struct zhashnode *	memchunk;

	/*
	 * Amount of used zhashnodes in memchunk. elemc <= capc.
	 */
	size_t			elemc;

	/*
	 * Length of memchunk.
	 */
	size_t			capc;

};

#endif

#endif

