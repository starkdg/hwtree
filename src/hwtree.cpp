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

#include <queue>
#include "hwt/hwtree.hpp"

using namespace std;
using namespace hwt;

hwt::HWTree::HWTree(){
	m_top = NULL;
}

hwt::HWTree::~HWTree(){
	Clear();
}

void hwt::HWTree::Insert(const hc_t &e){

	if (m_top == NULL){
		hw_t wts;
		m_top = new HWTLeaf();
		m_top->AddEntry(e, wts, NULL, 0);
		return;
	}
	
	int level = 0;
	hw_t prev_wts;
	HWTNode *prev = NULL;
	HWTNode *current = m_top;
	while (current != NULL){
		hw_t current_wts;
		calc_hwts(current_wts, e.code, level);

		HWTNode *next = NULL;
		HWTNode *node = current->AddEntry(e, current_wts, &next, level);
		if (level == 0 && node != current)	{
			this->m_top = node;
		}

		if (node != current && level > 0){
			delete current;
			prev->SetChildNode(prev_wts, node);
		}
		
		prev = current;
		current = next;
		prev_wts = current_wts;
		level++;
	}
}

void hwt::HWTree::Delete(const hc_t &e){
	int level = 0;
	hw_t prev_wts;
	HWTNode *prev = NULL;
	HWTNode *current = m_top;

	while (current != NULL){
		hw_t current_wts;
		calc_hwts(current_wts, e.code, level);

		HWTNode *next;
		HWTNode *node = current->DelEntry(e, current_wts, &next, level);
		if (node == NULL){
			delete current;
			if (level == 0) {
				m_top = NULL;
			}
			if (level > 0){
				prev->UnsetChildNode(prev_wts);
			}
		}
		
		prev = current;
		current = next;
		prev_wts = current_wts;
		level++;
	}
}

vector<hc_t> hwt::HWTree::RangeSearch(const uint64_t target, const int radius)const{
	vector<hc_t> results;

	queue<HWTNode*> nodes, next_nodes;

	if (m_top != NULL) nodes.push(m_top);
		
	int level = 0;
	while (!nodes.empty()){

		hw_t target_wts;
		calc_hwts(target_wts, target, level);

		while (!nodes.empty()){
			HWTNode *current = nodes.front();
			if (current->IsLeaf()){
				((HWTLeaf*)current)->SelectEntries(target, radius, results);
			} else {
				((HWTInternal*)current)->SelectChildNodes(target_wts, radius, next_nodes, level);
			}

			nodes.pop();
		}
		level++;
		nodes = move(next_nodes);
	}

	return results;
}



const size_t hwt::HWTree::Size()const{

	queue<HWTNode*> nodes;
	if (m_top != NULL) nodes.push(m_top);

	size_t n_entries = 0;
	while (!nodes.empty()){
		HWTNode *current = nodes.front();
		if (current->IsLeaf()){
			n_entries += ((HWTLeaf*)current)->Size();
		} else {
			((HWTInternal*)current)->GetChildNodes(nodes);
		}
		nodes.pop();
	}

	return n_entries;
}

const size_t hwt::HWTree::MemoryUsage()const{

	queue<HWTNode*> nodes;

	if (m_top != NULL) nodes.push(m_top);
	size_t n_bytes = 0;

	while (!nodes.empty()){
		HWTNode *current = nodes.front();
		n_bytes += current->BytesUsed();
		if (!current->IsLeaf()){
			((HWTInternal*)current)->GetChildNodes(nodes);
		}
		nodes.pop();
	}
	return n_bytes + sizeof(HWTree);
}

void hwt::HWTree::Clear(){
	queue<HWTNode*> nodes;
	if (m_top) nodes.push(m_top);

	while (!nodes.empty()){
		HWTNode *current = nodes.front();
		if (!current->IsLeaf()){
			((HWTInternal*)current)->GetChildNodes(nodes);
		}
		delete current;
		nodes.pop();
	}
	m_top = NULL;
}

void hwt::HWTree::Print(ostream &ostrm)const{

	queue<HWTNode*> current_nodes, next_nodes;
	if (m_top) current_nodes.push(m_top);

	ostrm << "------------HWTree----------------" << endl;
	int level = 0;
	while (!current_nodes.empty()){

		while (!current_nodes.empty()){
			HWTNode *node = current_nodes.front();
			if (!node->IsLeaf()){
				((HWTInternal*)node)->GetChildNodes(next_nodes);
				ostrm << "(internal-" << level << ")-" << dec << next_nodes.size() << endl; 
			} else {
				HWTLeaf *leaf = (HWTLeaf*)node;
				ostrm << "(leaf-" << level << ")-" << dec << leaf->Size() << endl;

				vector<hc_t> entries;
				leaf->GetEntries(entries);
				for (hc_t &e : entries){
					ostrm << "    " << dec << "id = " << e.id << " code = " << hex << e.code << endl;
				}
				
			}
			current_nodes.pop();
		}
		
		current_nodes = move(next_nodes);
		level++;
	}
	ostrm << "------------------------------------" << endl;
}

