#include<iostream>
#include<deque>
#include<iterator>
#include<memory>

using namespace std;


template<typename _Tp, typename _Alloc = std::allocator<_Tp>>
class deque2 : public deque<_Tp, _Alloc>
{
	public:
	typedef _Deque_base<_Tp, _Alloc> _Base;
	using _Base::_M_impl;

	// size of each array
	const static size_t buffsize = __deque_buf_size(sizeof(_Tp));
	// inherit constructors
	using deque<_Tp, _Alloc>::deque;
	
	deque2(deque<_Tp, _Alloc> && d) : deque<_Tp, _Alloc>(std::move(d))
	{
	}

	// only use this function when _GLIBCXX_DEQUE_BUF_SIZE (512)
	// is divisible by sizeof(_Tp)
	size_t capacity()
	{
		return (_M_impl._M_finish._M_node - _M_impl._M_start._M_node + 1) * buffsize;
	}
	// how to print all elements using pointers
	// _Tp must be primitive type (or overload << operator)
	void print()
	{
		int size1 = _M_impl._M_start._M_last - _M_impl._M_start._M_cur;
		if(_M_impl._M_start._M_node == _M_impl._M_finish._M_node)
		{
			size1 = _M_impl._M_finish._M_cur - _M_impl._M_start._M_cur;
			for(int i = 0; i < size1; i++)
				cout << _M_impl._M_start._M_cur[i] << endl;
		}
		else
		{
			for(int i = 0; i < size1; i++)
				cout << _M_impl._M_start._M_cur[i] << endl;
			for(auto p = _M_impl._M_start._M_node + 1; p < _M_impl._M_finish._M_node; p++)
			{
				for(int i = 0; i < buffsize; i++)
					cout << p[0][i] << endl;
			}
			int size2 = _M_impl._M_finish._M_cur - _M_impl._M_finish._M_first;
			for(int i = 0; i < size2; i++)
				cout << _M_impl._M_finish._M_first[i] << endl;
		}
	}
};


int main()
{
	// Example:
	deque<int> d;
	//
	//
	//    do something with this 'd' object
	//
	//

	// find capacity
	deque2<int> d2(move(d));
	cout << d2.capacity() << endl;
}
