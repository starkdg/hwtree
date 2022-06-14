#include <iostream>
#include <cstdint>
#include <random>
#include <cassert>
#include "hwt/hwtree.hpp"

using namespace std;
using namespace hwt;

const int radius = 10;
const int n_entries = 100;
const int n_clusters = 10;
const int cluster_size = 10;

static long long m_id = 1;
static long long g_id = 1000;

static random_device m_rd;
static mt19937_64 m_gen(m_rd());
static uniform_int_distribution<uint64_t> m_distrib(0);
static uniform_int_distribution<int> m_radius(1, radius);
static uniform_int_distribution<int> m_bitindex(0, 63);


int generate_data(vector<hc_t> &entries, const int n){

	for (int i=0;i < n;i++){
		entries.push_back({ m_id++, m_distrib(m_gen) });
	}

	return entries.size();
}

int generate_cluster(vector<hc_t> &entries, const uint64_t center, const int n){
		
	uint64_t mask = 0x01;

	entries.push_back({ g_id++, center });

	for (int i=0;i < n-1;i++){
		uint64_t code_value = center;
		int d = m_radius(m_gen);
		for (int j=0;j < d;j++){
			code_value ^= (mask << m_bitindex(m_gen));
		}
		entries.push_back({ g_id++, code_value });
	}
	return n;
}

int basic_test(){

	vector<hc_t> entries;
	generate_data(entries, n_entries);
	assert(entries.size() == n_entries);

	cout << "Build Tree of " << n_entries << " entries" << endl;
	HWTree tree;
	for (hc_t &e : entries){
		tree.Insert(e);
	}

	size_t sz = tree.Size();
	cout << "Size of tree: " << sz << endl;
	assert(sz == n_entries);

	size_t nbytes = tree.MemoryUsage();
	cout << "memory used: " << (double)nbytes/1000000.0 << " MB" << endl;
	
	cout << "Add " << n_clusters << " clusters of " << cluster_size << endl;
	
	uint64_t centers[n_clusters];
	for (int i=0;i < n_clusters;i++){
		centers[i] = m_distrib(m_gen);

		vector<hc_t> cluster;
		generate_cluster(cluster, centers[i], cluster_size);

		for (hc_t &e : cluster){
			tree.Insert(e);
		}

		sz = tree.Size();
		assert((int)sz == n_entries + (i+1)*cluster_size);
	}

	sz = tree.Size();
	assert((int)sz == n_entries + cluster_size*n_clusters);

	tree.Print(cout);
	
	for (int i=0;i < n_clusters;i++){
		cout << "query target = " <<  hex << centers[i] << " with radius = " << dec << radius << endl;
		vector<hc_t> results = tree.RangeSearch(centers[i], radius);
		assert(results.size() == cluster_size);
	}
		

	int delete_id = 1000;
	for (int i=0;i < n_clusters;i++){
		cout << "Delete id = " << dec << delete_id << " code = " << hex << centers[i] << endl;
		tree.Delete({ delete_id, centers[i]});

		delete_id += 10;
		
		sz = tree.Size();
		cout << "Size = " << dec << sz << endl;
		assert((int)sz == n_entries + n_clusters*cluster_size - (i + 1));
	}

	cout << "Clear" << endl;
	tree.Clear();
	sz = tree.Size();
	cout << "size = " << sz << endl;
	assert(sz == 0);
	
	return 0;
}


int main(int argc, char **argv){

	basic_test();
	
	return 0;
}
