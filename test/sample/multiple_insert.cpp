#include<iostream>
#include<time.h>
#include<cstdlib>
#include "sda.h"

using namespace std;


int main()
{
	int value = 0xCAFE;
	int seed = 0xF00D;

	srand(seed);

	size_t c, t;
	cin >> c >> t;
	// eg: 300 10000

	sda<int> a;
	// reserve version
	// a.reserve(c * t);
	
	double start = clock();
	for(size_t i = 0; i < t; i++)
	{
		size_t pos = rand() % (a.size() + 1);
		a.erase(a.begin() + pos, c, value);
		// reserve version
		// a.join(a.begin() + pos, c, value);
	}

	double stop = clock();
	cout << (stop - start)/CLOCKS_PER_SEC << endl;
	cout << "capacity = " << a.capacity() << endl;
}
