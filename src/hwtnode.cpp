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

#include <iostream>
#include <cmath>
#include "hwt/hwtnode.hpp"

using namespace std;

/**
 *
 *  HWTInternal methods
 *
 **/

void hwt::HWTInternal::SetChildNode(const hw_t &key, HWTNode *node){

	m_childnodes[key] = node;
	
}

void hwt::HWTInternal::UnsetChildNode(const hw_t &key){
	m_childnodes.erase(key);
}

hwt::HWTNode* hwt::HWTInternal::AddEntry(const hc_t &entry, const hw_t &wts, HWTNode **next, int level){

	if (m_childnodes.count(wts) <= 0){
		m_childnodes[wts] = new HWTLeaf();
	}
	*next = m_childnodes[wts];
	return this;
}

hwt::HWTNode* hwt::HWTInternal::DelEntry(const hc_t &entry, const hw_t &wts, HWTNode **next, int level){
	*next = NULL;
	
	if (m_childnodes.count(wts) >= 0){
		*next = m_childnodes[wts];
	}

	return this;
}

void hwt::HWTInternal::AddEntries(vector<hc_t> &entries, const int level){

	for (const hc_t &e : entries){
		hw_t wts;
		calc_hwts(wts, e.code, level);

		HWTLeaf *leaf = NULL;
		if (m_childnodes.count(wts) > 0){
			leaf = (HWTLeaf*)m_childnodes[wts];
		}

		if (leaf == NULL) leaf = new HWTLeaf();

		leaf->AddEntry(e, wts, NULL, level+1);
	
		m_childnodes[wts] = leaf;
	}
}

void hwt::HWTInternal::GetChildNodes(queue<HWTNode*> &nodes){
	for (auto iter = m_childnodes.begin(); iter != m_childnodes.end(); iter++){
		nodes.push(iter->second);
	}
}

void hwt::HWTInternal::SelectChildNodes(const hw_t &wts, const int radius,
								   queue<HWTNode*> &next_nodes, int level){
	for (auto iter = m_childnodes.begin(); iter != m_childnodes.end(); ++iter){
		if (wts.distance(iter->first) <= radius){
			next_nodes.push(iter->second);
		}
	}
}

size_t hwt::HWTInternal::BytesUsed()const{
	size_t n_elems = m_childnodes.size();
	size_t n_buckets = m_childnodes.bucket_count();
	size_t sz_elem = sizeof(unordered_map<hw_t,HWTNode*,hwhasher_t>::value_type);
	
	size_t elem_sz = n_elems*(sz_elem + sizeof(void*));
	size_t tbl_sz = n_buckets*sizeof(void*);
	return elem_sz + tbl_sz;
}

bool hwt::HWTInternal::IsLeaf()const{
	return false;
}

/**
 *
 *  HWTLeaf methods
 *
 **/

hwt::HWTNode* hwt::HWTLeaf::AddEntry(const hc_t &entry, const hw_t &wts, HWTNode **next, int level){

	m_entries.push_back(entry);
	if (m_entries.size() <= LC || level >= log2(NDIMS)){
		if (next) *next = NULL;
		return this;
	} 

	HWTInternal *internal = new HWTInternal();

	internal->AddEntries(m_entries, level);

	*next = NULL;
	
	return internal;
	
}

hwt::HWTNode* hwt::HWTLeaf::DelEntry(const hc_t &entry, const hw_t &wts, HWTNode **next, int level){
	for (int i=0;i < (int)m_entries.size();i++){
		if (entry.distance(m_entries[i]) == 0 && entry.id == m_entries[i].id){
			m_entries[i] = m_entries[m_entries.size()-1];
			m_entries.pop_back();
			break;
		}
	}
	*next = NULL;
	if (m_entries.size() == 0){
		return NULL;
	}
	return this;
}

void hwt::HWTLeaf::GetEntries(vector<hc_t> &entries){
	for (const hc_t &e : m_entries){
		entries.push_back(e);
	}
}

void hwt::HWTLeaf::SelectEntries(const uint64_t target, const int radius, vector<hc_t> &results){

	for (const hc_t &e : m_entries){
		if (e.distance(target) <= radius){
			results.push_back(e);
		}
	}
}

size_t hwt::HWTLeaf::Size()const{
	return m_entries.size();
}

size_t hwt::HWTLeaf::BytesUsed()const{
	return m_entries.capacity()*sizeof(hc_t);
}

bool hwt::HWTLeaf::IsLeaf()const{
	return true;
}
