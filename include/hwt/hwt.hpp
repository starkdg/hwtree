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

#ifndef _HWT_H
#define _HWT_H

#include <cstdint>
#include <cstring>
#include <cmath>
#include <functional>

#define NDIMS 64
#define LC 10


namespace hwt {

	/** datapoint element  **/
	struct hc_t {
		static unsigned long n_query_ops;
		long long id;
		uint64_t code;
		hc_t():id(0),code(0){}
		hc_t(const long long id, const uint64_t &code):id(id),code(code){}
		hc_t(const hc_t &other){
			id = other.id;
			code = other.code;
		}
		hc_t& operator=(const hc_t &other){
			id = other.id;
			code = other.code;
			return *this;
		}
		bool operator==(const hc_t &other)const{
			return (id == other.id && code == other.code);
		}
		int distance(const hc_t other)const{
			return __builtin_popcountll(code^other.code);
		}
		int distance(const uint64_t other)const {
			hc_t::n_query_ops++;
		return __builtin_popcountll(code^other);
	}
};


	/** hamming weights data type **/
	struct hw_t {
		static unsigned long n_build_ops;
		uint8_t wts[NDIMS];
		hw_t(){
			for (int i=0;i<NDIMS;i++)
				wts[i] = 0;
		}
		hw_t(const hw_t &other){
			for (int i=0;i<NDIMS;i++)
				wts[i] = other.wts[i];
		}
		hw_t& operator=(const hw_t &other){
			for (int i=0;i<NDIMS;i++)
				wts[i] = other.wts[i];
			return *this;
		}
		bool operator==(const hw_t &other)const{
			return !memcmp(wts, other.wts, NDIMS);
		}
		int l1distance(const hw_t &other)const{
			hw_t::n_build_ops++;
			int sum = 0;
			for (int i=0;i < NDIMS;i++)
				sum += abs((int)wts[i] - (int)other.wts[i]);
			return sum;
		}
		int l2distance(const hw_t &other)const{
			hw_t::n_build_ops++;
			int sum = 0;
			for (int i=0;i < NDIMS;i++)
				sum += pow((int)wts[i] - (int)other.wts[i], 2.0);
			return sqrt(sum);
		}
		int distance(const hw_t &other)const{
			return l1distance(other);
		}
	};

	/* hasher functional to hash hw_t keys */
	struct hwhasher_t{
		size_t operator()(const hw_t &key) const{
			size_t hash = 0;
			for (int i=0;i < NDIMS;i++){
				hash ^= ((size_t)key.wts[i]) << i;
			}
			hash >>= 1;
			return hash;
		}
	};


	/** calc hamming weights for hw_t  **/
	void calc_hwts(struct hw_t &hwts, const uint64_t code, const int level);

}

#endif /* _HWT_H */

