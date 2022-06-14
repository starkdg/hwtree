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

#ifndef _HWTREE_H
#define _HWTREE_H

#include <ostream>
#include <cstdlib>
#include <cstdint>
#include <vector>
#include "hwt/hwtnode.hpp"
#include "hwt/hwt.hpp"

namespace hwt {

	class HWTree {
	private:
		
		HWTNode *m_top;
	
	public:
		HWTree();

		~HWTree();
		
		void Insert(const hc_t &e);
	
		void Delete(const hc_t &e);
		
		std::vector<hc_t> RangeSearch(const std::uint64_t target, const int radius)const;
	
		const std::size_t Size()const;
	
		const std::size_t MemoryUsage()const;
		
		void Clear();

		void Print(std::ostream &ostrm)const;
	
	};
}

#endif /* _HWTREE_H */

