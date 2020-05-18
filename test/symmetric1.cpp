#include<iostream>
#include<time.h>
#include "sda.h"

using namespace std;

int main()
{
	sda<int> a, b;
	int value = 0xABCD;

	double start = clock();
	for(size_t i = 0; i < 1000000; i++)
		a.insert(a.begin() + a.size()/4, value);
	double stop = clock();
	cout << "time1 = " << (stop - start)/CLOCKS_PER_SEC << endl;
	start = clock();
	for(size_t i = 0; i < 1000000; i++)
		b.insert(b.begin() + b.size() - b.size()/4, value);
	stop = clock();
	cout << "time2 = " << (stop - start)/CLOCKS_PER_SEC << endl;
}
