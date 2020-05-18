This library's design is very similar to C++ vector. People who are familiar with C++ vector can easily use this library. 



## GUIDE

[+]	All **insert**, **emplace**, **join**, **emjoin** functions return **iterator**

[+]	Default Growth Formulas:

```c++
front  : empty_back_capacity() + front_capacity() * 1.25 + 2
back   : empty_front_capacity() + back_capacity() * 1.25 + 2
```

[+]	All examples in the guide use this default **SDA**:

```c++
sda<int> a;
a.reserve_back(6);
for(int i = 1; i <= 4; i++) a.push_back(i);
a.reserve_front(8);
```

result:

```c++
[# # # # 1 2 3 4 # #]
```



### reserve, reserve_back, reserve_front

**reserve**: behave similarly to C++ vector's reserve. But unused space is divided equally for both sides

**reserve_back**: (reallocate if needed) increase **back_capacity()** so that it isn't smaller than specified new back capacity

**reserve_front**: (reallocate if needed) increase **front_capacity()** so that it isn't smaller than specified new front capacity

Example:

```c++
// a = [# # # # 1 2 3 4 # #]
a.reserve(12);         // result: [# # # # 1 2 3 4 # # # #]
a.reserve_back(10);    // result:  [# # # # 1 2 3 4 # # # # # #]
a.reserve_front(11);   // result:  [# # # # # # # 1 2 3 4 # # # # # #]
```



### slide_to_back, slide_to_front

move the array over one side to increase memory in the other side

Example:

```c++
// a = [# # # # 1 2 3 4 # #]
a.slide_to_back();   // result: [# # # # # 1 2 3 4]
a.slide_to_front(2); // result: [# # 1 2 3 4 # # #]
```



### insert, emplace

choose the nearest end to insert/construct element. grows memory if needed

Example:

```c++
// a = [# # # # 1 2 3 4 # #]
a.insert(a.begin() + 1, 0);   // result: [# # # 1 0 2 3 4 # #]
a.insert(a.begin() + 4, 9);   // result: [# # # 1 0 2 3 9 4 #]
a.insert(a.begin() + 4, 5);   // result: [# # # 1 0 2 3 5 9 4]
a.insert(a.begin() + 5, 1);   // back capacity = 7 
                              // new back capacity = 7 * 1.25 + 2 = 10
                              // result: [# # # 1 0 2 3 5 1 9 4 # #]
```

### join, emjoin

current array **must** have enough memory to insert new element(s)

**join**: call **insert** if the nearest end has enough space, otherwise insert at the end that has enough memory. If no sides have enough memory, create a 'hole' in array to insert (construct) new element(s)

Example:

```c++
// a = [# # # # 1 2 3 4 # #]
a.join(a.begin() + 1, 0);    // result: [# # # 1 0 2 3 4 # #]
a.join(a.begin() + 4, 9);    // result: [# # # 1 0 2 3 9 4 #]
a.join(a.begin() + 4, 5);    // result: [# # # 1 0 2 3 5 9 4]
a.join(a.begin() + 5, 1);    // result: [# # 1 0 2 3 5 1 9 4]
```



### erase

choose the best way to delete (try to move elements as little as possible)

Example:

```c++
// a = [# # # # 1 2 3 4 # #]
a.push_back(5);
a.push_back(6);     // result: [# # # # 1 2 3 4 5 6]
a.erase(a.begin() + 2, a.begin() + 5); // result:  [# # # # 1 2 6 # # #]
```



### resize_back, resize_front

similar to C++ vector's resize, but only back's capacity or front's capacity is affected

Example:

```c++
// a = [# # # # 1 2 3 4 # #]
a.resize_back(2);     // result:  [# # # # 1 2 # # # #]
a.resize_front(4, 3); // result:  [# # 3 3 1 2 # # # #]
```



## TIPS

1. Change growth formula when needed: decrease back growth rate, increase front growth rate when insertion mainly takes place at beginning of array and vice versa
2. If you are going to insert a lot of elements at random positions:
   - Reserve enough capacity then call **join**
   - Call insert instead if you don't care much about memory usage.

