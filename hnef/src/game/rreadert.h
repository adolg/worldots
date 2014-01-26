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
#ifndef HT_RREADER_T_H
#define HT_RREADER_T_H

/*
 * Information needed by the ruleset reader prior to allocating the game
 * struct.
 */
/*@exposed@*/
struct rread_info
{

	/*
	 * ruleset->path.
	 */
/*@null@*/
/*@owned@*/
	char *	path,
	* name,
	* id,
	* hash;


	/*
	 * ruleset->bwidth.
	 */
	unsigned short		bwidth;

	/*
	 * ruleset->height.
	 */
	unsigned short		bheight;

};

#endif

