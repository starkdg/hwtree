#include <cstdlib>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <random>
#include <vector>
#include <chrono>
#include <cassert>
#include <ratio>
#include "hwt/hwtree.hpp"

using namespace std;
using namespace hwt;

static random_device m_rd;
static mt19937_64 m_gen(m_rd());
static uniform_int_distribution<uint64_t> m_distrib(0);


static long long m_id = 1;
static long long g_id = 100000000;

struct perfmetric {
	double avg_build_ops;
	double avg_build_time;
	double avg_query_ops;
	double avg_query_time;
	size_t avg_memory_used;
};


int generate_data(vector<hc_t> &entries, const int n){

	for (int i=0;i < n;i++){
		entries.push_back({ m_id++, m_distrib(m_gen) });
	}

	return entries.size();
}

int generate_cluster(vector<hc_t> &entries, const uint64_t center, const int radius, int n){
	uniform_int_distribution<int> r(1, radius);
	uniform_int_distribution<int> bitindex(0, 63);

	uint64_t mask = 0x01;
	entries.push_back({ g_id++, center });

	for (int i=0;i < n-1;i++){
		uint64_t val = center;
		if (radius > 0){
			int dist = r(m_gen);
			for (int j=0;j < dist;j++){
				val ^= (mask << bitindex(m_gen));
			}
		}
		entries.push_back({ g_id++, val });
	}
	return n;
}

void do_run(const int index, const int n_entries, const int n_clusters,
			const int cluster_size, int radius, vector<perfmetric> &metrics){
	m_id = 1;
	g_id = 100000000;

	HWTree hwtree;

	vector<hc_t> entries;
	generate_data(entries, n_entries);
	assert((int)entries.size() == n_entries);

	int sz;
	chrono::duration<unsigned long, nano> total(0);
	hw_t::n_build_ops = 0;
	auto s = chrono::steady_clock::now();
	for (auto &e : entries){
		hwtree.Insert(e);
	}
	auto e = chrono::steady_clock::now();
	total += (e - s);
	sz = hwtree.Size();
	assert(sz == n_entries);
		   
	struct perfmetric m;
		   
	m.avg_build_ops = 100.0*((double)hw_t::n_build_ops/(double)sz); 
	m.avg_build_time = total.count()/(double)sz;
	m.avg_memory_used = hwtree.MemoryUsage();
		   
	cout << "(" << index << ") build tree: " << setw(10) << setprecision(6) << m.avg_build_ops << "% opers "
		   << setw(10) << setprecision(6) << m.avg_build_time << " nanosecs    ";

	uint64_t centers[n_clusters];
	for (int i=0;i < n_clusters;i++){
		centers[i] = m_distrib(m_gen);

		vector<hc_t> cluster;
		generate_cluster(cluster, centers[i], radius, cluster_size);
		assert((int)cluster.size() == cluster_size);
		for (auto &e : cluster){
			hwtree.Insert(e);
		}
		sz = hwtree.Size();
		assert(sz == n_entries + cluster_size*(i+1));
	}
		   
	hc_t::n_query_ops = 0;
	chrono::duration<double, milli> querytime(0);
	for (int i=0;i < n_clusters;i++){
		auto s = chrono::steady_clock::now();
		vector<hc_t> results = hwtree.RangeSearch(centers[i], radius);
		auto e = chrono::steady_clock::now();
		querytime += (e - s);

		int nresults = (int)results.size();
		assert(nresults >= cluster_size);
	}

	m.avg_query_ops = 100.0*((double)hc_t::n_query_ops/(double)n_clusters/(double)sz);
	m.avg_query_time = (double)querytime.count()/(double)n_clusters;

	cout << " query ops " << dec << setprecision(6) << m.avg_query_ops << "% opers   " 
		 << "query time: " << dec << setprecision(6) << m.avg_query_time << " millisecs" << endl;
	
	metrics.push_back(m);
	
	hwtree.Clear();
	sz = hwtree.Size();
	assert(sz == 0);
}

void do_experiment(const int idx, const int n_runs, const int n_entries,
				   const int n_clusters, const int cluster_size, const int radius){

	cout << "----------------Trial " << idx << "-------------------------------" << endl;
	cout << "data set size, N = " << n_entries << endl;
	cout << "no. clusters = " << n_clusters << " of size, " << cluster_size << " w/radius = " << radius << endl << endl;
	
	vector<perfmetric> metrics;
	for (int i=0;i < n_runs;i++){
		do_run(i, n_entries, n_clusters, cluster_size, radius, metrics);
	}
	
	double avg_build_ops = 0;
	double avg_build_time = 0;
	double avg_query_ops = 0;
	double avg_query_time = 0;
	double avg_memory = 0;
	for (struct perfmetric &m : metrics){
		avg_build_ops += m.avg_build_ops/n_runs;
		avg_build_time += m.avg_build_time/n_runs;
		avg_query_ops += m.avg_query_ops/n_runs;
		avg_query_time += m.avg_query_time/n_runs;
		avg_memory += (double)m.avg_memory_used/(double)n_runs;
	}

	cout << "no. runs: " << metrics.size() << endl;
	cout << "avg build:  " << avg_build_ops << "% opers " << avg_build_time << " nanosecs " << endl;
	cout << "avg query:  " << avg_query_ops << "% opers " << avg_query_time << " milliseconds" << endl;
	cout << "Memory Usage: " << fixed << setprecision(2) << avg_memory/1000000.0 << "MB" << endl;
	cout << "----------------------------------------------------" << endl;
}

int main(int argc, char **argv){

	const int n_runs = 5;
	const int n_experiments = 9;
	const int n_entries[n_experiments] = { 100000, 200000, 400000, 800000, 1000000, 2000000, 4000000, 8000000, 16000000 };
	const int n_clusters = 10;
	const int cluster_size = 10;
	const int radius = 5;

	cout << "HWTree Indexing data structure" << endl;
	cout << "leaf capacity: " << LC << endl;
	cout << "stats avg.'d over " << n_runs << " trials" << endl;

	cout << endl << endl;

	for (int i=0;i < n_experiments;i++){
		do_experiment(i+1, n_runs, n_entries[i], n_clusters, cluster_size, radius);
	}

 
	cout << endl << "Test for different radius values" << endl << endl;

	const int N = 4000000;
	const int n_rad = 7;
	const int rad[n_rad] = { 0, 2, 4, 6, 8, 10, 12 };

	for (int i=0;i < n_rad;i++){
		do_experiment(i+1, n_runs, N, n_clusters, cluster_size, rad[i]);
	}
	
	return 0;
}


