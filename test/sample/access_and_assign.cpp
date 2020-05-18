#include<iostream>
#include<time.h>
#include<cstdlib>
#include "sda.h"

using namespace std;


int main()
{
	int value = 0xFEED;
	int seed = 0x5AFE;

	srand(seed);

	size_t n;
	cin >> n;
	// eg: 100000000
	
	
	sda<int> a(n);

	double start = clock();
	for(size_t i = 0; i < n; i++)
	{
		size_t pos = rand() % a.size();
		a[pos] = value;
	}
	double stop = clock();
	cout << (stop - start)/CLOCKS_PER_SEC << endl;
	cout << "capacity = " << a.capacity() << endl;
}
