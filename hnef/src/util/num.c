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
#include <errno.h>	/* errno */
#include <limits.h>	/* *_MAX */
#include <stdlib.h>	/* strtoul */
#include <string.h>	/* strlen */

#include "num.h"

unsigned short
max (
	const unsigned short num1,
	const unsigned short num2
	)
{
	return num1 > num2 ? num1 : num2;
}

unsigned short
min (
	const unsigned short num1,
	const unsigned short num2
	)
{
	return num1 > num2 ? num2 : num1;
}

/*
 * Returns HT_TRUE if bitmask is a single bit, and not 0.
 */
HT_BOOL
single_bit (
	const unsigned int bitmask
	)
{
	return 0 != bitmask && 0 == (bitmask & (bitmask - 1));
}

/*
 * Sets num to USHRT_MAX upon failure.
 */
HT_BOOL
texttoushort (
	const char * const HT_RSTR	str_num,
	unsigned short * const HT_RSTR	num
	)
{
	char *		str_end;
	unsigned long	n;
	int		errno_old	= errno;
			errno		= 0;

	n	= strtoul(str_num, & str_end, 10);
	if (0 != errno		|| strlen(str_end) > 0
	|| ULONG_MAX == n	|| n > (unsigned long)USHRT_MAX)
	{
		errno	= errno_old;
		* num	= USHRT_MAX;
		return HT_FALSE;
	}
	* num	= (unsigned short)n;
	return HT_TRUE;
}

