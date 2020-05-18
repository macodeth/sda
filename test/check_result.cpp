#include<iostream>
#include<algorithm>
#include<vector>

#include "sda.h"

using namespace std;

//
//
//	CHECK RESULT
//	print "WRONG" if sda didn't 
//	perform any operation correctly


template<class A, class B>
void check(const A& a, const B& b)
{
	for(size_t i = 0; i < a.size(); i++)
	{
		if(a[i] != b[i])
		{
			cout << "WRONG" << endl;
			return;
		}
	}
	cout << "RIGHT" << endl;
}

int main()
{
	const int n = 10000;

	vector<int> v1;
	sda<int> a1;

	//
	// single insert
	//
	for(int i = 0; i < n; i++)
	{
		size_t pos = rand() % (v1.size() + 1);
		int value = rand();
		v1.insert(v1.begin() + pos, value);
		a1.insert(a1.begin() + pos, value);
	}
	check(v1, a1);

	//
	// single insert with reserved space
	//
	vector<int> v2;
	sda<int> a2;
	v2.reserve(n);
	a2.reserve(n);
	for(int i = 0; i < n; i++)
	{
		size_t pos = rand() % (v2.size() + 1);
		int value = rand();
		v2.insert(v2.begin() + pos, value);
		a2.join(a2.begin() + pos, value);
	}
	check(v2, a2);

	//
	// multiple insert (fill insert)
	//
	vector<int> v3;
	vector<int> a3;
	for(int i = 0; i < n; i++)
	{
		int value = rand();
		size_t pos = rand() % (v3.size() + 1);
		size_t count = rand() % 11 + 2;
		v3.insert(v3.begin() + pos, count, value);
		a3.insert(a3.begin() + pos, count, value);
	}
	check(v3, a3);

	//
	// single delete
	//
	vector<int> v4(n);
	sda<int> a4(n);
	for(int i = 0; i < n; i++)
	{ // assign value firstly
		int value = rand();
		v4[i] = a4[i] = value;
	}
	for(int i = 0; i < n/10; i++)
	{
		size_t pos = rand() % a4.size();
		v4.erase(v4.begin() + pos);
		a4.erase(a4.begin() + pos);
	}
	check(v4, a4);
	

	//
	// range delete
	//
	vector<int> v5(n);
	sda<int> a5(n);
	for(int i = 0; i < n; i++)
	{ // assign value firstly
		int value = rand();
		v5[i] = a5[i] = value;
	}
	// each time delete a range containing 2% of total elements
	double ratio = 1.0 / 50;
	for(int i = 0; i < n/20; i++)
	{
		size_t pos = rand() % size_t((1 - ratio) * v5.size());
		size_t count = ratio * v5.size();
		v5.erase(v5.begin() + pos, v5.begin() + pos + count);
		a5.erase(a5.begin() + pos, a5.begin() + pos + count);
	}
	check(v5, a5);


	//
	// push
	//
	vector<int> v6;
	sda<int> a6;
	for(int i = 0; i < n; i++)
	{
		int value = rand();
		v6.push_back(value);
		a6.push_back(value);
	}
	check(v6, a6);

}
