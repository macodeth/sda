#include<iostream>
#include<time.h>
#include "sda.h"

using namespace std;

int main()
{

	sda<int> a(1000000, 0xABCD);
	sda<int> b(1000000, 0xABCD);

	double start = clock();
	for(size_t i = 0; i < 300000; i++)
		a.erase(a.begin() + a.size()/4);
	double stop = clock();
	cout << "time1 = " << (stop - start)/CLOCKS_PER_SEC << endl;

	
	start = clock();
	for(size_t i = 0; i < 300000; i++)
		b.erase(b.begin() + b.size()/4);
	stop = clock();
	cout << "time2 = " << (stop - start)/CLOCKS_PER_SEC << endl;
}
