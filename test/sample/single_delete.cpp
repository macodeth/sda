#include<iostream>
#include<time.h>
#include<cstdlib>
#include "sda.h"

using namespace std;


int main()
{
	int value = 0xDEAD;
	int seed = 0xFADE;

	srand(seed);

	size_t n, t;
	cin >> n >> t;
	// eg: 300000 100000


	sda<int> a(n, value);
	
	
	double start = clock();
	for(size_t i = 0; i < t; i++)
	{
		size_t pos = rand() % a.size();
		a.erase(a.begin() + pos);
	}

	double stop = clock();
	cout << (stop - start)/CLOCKS_PER_SEC << endl;
}
