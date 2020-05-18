#include<iostream>
#include<time.h>
#include<cstdlib>
#include "sda.h"

using namespace std;


int main()
{
	int value = 0x50DA;


	size_t n;
	cin >> n;
	// eg: 100000000
	
	
	sda<int> a;
	// reserve version
	// a.reserve_back(n);

	double start = clock();
	for(size_t i = 0; i < n; i++)
		a.push_back(value);

	double stop = clock();
	cout << (stop - start)/CLOCKS_PER_SEC << endl;
	cout << "capacity = " << a.capacity() << endl;
}
