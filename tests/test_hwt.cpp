#include <iostream>
#include <cassert>
#include <cstdint>
#include "hwt/hwt.hpp"

using namespace std;
using namespace hwt;


void test_hwt(){

	uint64_t code = 0xffffffffffffffffULL;

	for (int i=0;i < 7;i++){
		cout << "code = " << hex << code << " => ";
		hw_t hwts;
		calc_hwts(hwts, code, i);
		for (int j=0;j < NDIMS;j++){
			cout << dec << (int)hwts.wts[j] << " ";
		}
		cout << endl;
	}	  

	code = 0x0f0f0f0f0f0f0f0fULL;

	for (int i=0;i < 7;i++){
		cout << "code = " << hex << code << " => ";
		hw_t hwts;
		calc_hwts(hwts, code, i);
		for (int j=0;j < NDIMS;j++){
			cout << dec << (int)hwts.wts[j] << " ";
		}
		cout << endl;
	}	  
}



int main(int argc, char **argv){

	test_hwt();
	
	return 0;
}
