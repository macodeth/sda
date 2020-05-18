#include<iostream>
#include<time.h>
#include<cstdlib>
#include "sda.h"

using namespace std;


int main()
{
	int value = 0xBA5E;
	int seed = 0xCA5E;

	srand(seed);

	size_t n, t, r2;
	cin >> n >> t >> r2;
	// eg: 100000000 1800 100
	double r = 1.0 / r2;


	sda<int> a(n, value);
	
	double start = clock();
	for(size_t i = 0; i < t; i++)
	{
		size_t pos = rand() % size_t((1 - r) * a.size());
		size_t s = r * a.size();
		a.erase(a.begin() + pos, a.begin() + pos + s);
	}
	double stop = clock();
	cout << (stop - start)/CLOCKS_PER_SEC << endl;
}
