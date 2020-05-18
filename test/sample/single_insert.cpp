#include<iostream>
#include<time.h>
#include<cstdlib>
#include "sda.h"

using namespace std;


int main()
{
	int value = 0xBEEF;
	int seed = 0xFACE;

	srand(seed);

	size_t n;
	cin >> n;
	// eg: 300000

	sda<int> a;
	// reserve version
	// a.reserve(n);
	
	double start = clock();
	for(size_t i = 0; i < n; i++)
	{
		size_t pos = rand() % (a.size() + 1);
		a.insert(a.begin() + pos, value);
		// reserve version
		// a.join(a.begin() + pos, value);
	}

	double stop = clock();
	cout << (stop - start)/CLOCKS_PER_SEC << endl;
	cout << "capacity = " << a.capacity() << endl;
}
