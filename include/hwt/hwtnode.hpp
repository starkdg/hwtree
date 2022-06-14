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

#ifndef _HWTNODE_H
#define _HWTNODE_H

#include <cstdlib>
#include <unordered_map>
#include <vector>
#include <queue>
#include "hwt/hwt.hpp"


namespace hwt {
	
	class HWTNode {
	private:
	public:
		virtual ~HWTNode(){}
		virtual HWTNode* AddEntry(const hc_t &entry, const hw_t &wts, HWTNode **next, int level) = 0;
		virtual HWTNode* DelEntry(const hc_t &entry, const hw_t &wts, HWTNode **next, int level) = 0;
		virtual void SetChildNode(const hw_t &key, HWTNode *node) = 0;
		virtual void UnsetChildNode(const hw_t &key) = 0;
		virtual size_t BytesUsed()const=0;
		virtual bool IsLeaf()const = 0;
	};

	class HWTInternal : public HWTNode {
	private:
	
		std::unordered_map<hw_t, HWTNode*, hwhasher_t> m_childnodes;
	
	public:
		HWTInternal(){};
		~HWTInternal(){}
		HWTNode* AddEntry(const hc_t &entry, const hw_t &wts, HWTNode **next, int level);
		HWTNode* DelEntry(const hc_t &entry, const hw_t &wts, HWTNode **next, int level);
		void SetChildNode(const hw_t &key, HWTNode *node);
		void UnsetChildNode(const hw_t &key);
		void AddEntries(std::vector<hc_t> &entries, const int level);
	
		void GetChildNodes(std::queue<HWTNode*> &nodes);
	
		void SelectChildNodes(const hw_t &wts, const int radius, std::queue<HWTNode*> &next_nodes, int level);
		size_t BytesUsed()const;
		bool IsLeaf()const;
};

	class HWTLeaf : public HWTNode {
	private:
		
		std::vector<hc_t> m_entries;
	
	public:   
		HWTLeaf(){};
		~HWTLeaf(){}
		HWTNode* AddEntry(const hc_t &entry, const hw_t &wts, HWTNode **next, int level);
		HWTNode* DelEntry(const hc_t &entry, const hw_t &wts, HWTNode **next, int level);
		void SetChildNode(const hw_t &key, HWTNode *node){}
		void UnsetChildNode(const hw_t &key){};
	
		void Process(std::queue<HWTNode*> &nodes);
		void GetEntries(std::vector<hc_t> &entries);
		void SelectEntries(const uint64_t target, const int radius, std::vector<hc_t> &results);
		size_t Size()const;
		size_t BytesUsed()const;
		bool IsLeaf()const;
	};

}
	
#endif /* _HWTNODE_H */

