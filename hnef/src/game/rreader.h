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
#ifndef HT_RREADER_H
#define HT_RREADER_H

#include "funct.h"		/* HT_FR, HT_FAIL_RREADER */
#include "gamet.h"		/* game */
#include "iotextparset.h"	/* ht_line */

/*@-protoparamname@*/
/*@in@*/
/*@null@*/
/*@only@*/
extern
struct game *
alloc_game_read (
/*@in@*/
/*@notnull@*/
/*@observer@*/
	const char *			file_rc,
/*@in@*/
/*@notnull@*/
	struct ht_line * const		line,
/*@in@*/
/*@notnull@*/
	enum HT_FR * const		fr,
/*@in@*/
/*@notnull@*/
	enum HT_FAIL_RREADER * const	fail_read
	)
/*@globals fileSystem, errno@*/
/*@modifies fileSystem, errno, * line, * fr, * fail_read@*/
;
/*@=protoparamname@*/

#endif

