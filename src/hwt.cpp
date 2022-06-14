/**
    HWTree - hamming weight indexing tree for 64-bit integer types
    Copyright (C) 2022  David G. Starkweather

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.  **/

#include <cmath>
#include "hwt/hwt.hpp"


unsigned long hwt::hc_t::n_query_ops = 0;

unsigned long hwt::hw_t::n_build_ops = 0;

/** calc hamming weights for hw_t  **/
void hwt::calc_hwts(struct hw_t &hwts, const uint64_t code, const int level){

	uint64_t mask = 0xffffffffffffffffULL;

	if (level == 0){
		hwts.wts[0] = (uint8_t)__builtin_popcountll(code);
		return;
	}
	
	int shiftby = 0;
	int n = 1;
	while (n <= level) shiftby += NDIMS/(1 << n++);

	mask <<= shiftby;
	shiftby = NDIMS - shiftby;

	int index = 0;
	while (mask){
		hwts.wts[index++] = (uint8_t)__builtin_popcountll(mask&code);
		mask >>= shiftby;
	}	
}

	


