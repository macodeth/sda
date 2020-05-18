**computer** : Dell Inspiron 15 3000 Series
**processor** : Intel® Core™ i5-7200U CPU @ 2.50GHz × 4 
**os**             : Ubuntu 18.04.4 LTS 64-bit
**compiler** : g++ 7.5.0
**compile** : g++ -O2 -std=c++17 -Wpedantic -Wall -Wextra -Werror
**method**  : clock() function from time.h



### **BENCHMARK:**

**data type:**

- **int** (4 bytes)
- custom class (non trivially copyable, ...) { **incomplete** }

**container:** 

- sda
- std::vector (libstdc++)
- std::deque (libstdc++)

**operation:** 

​           **single insert** :     insert at a random position many times

​           **multiple insert** : each time fill insert many elements at a random position

​           **single delete** :    delete at a random position many times

​           **range delete** :    delete a large random range of elements in array many times

​           **push back**, **push front** : insert at end or begin

​           **pop back**, **pop front** : delete at end or begin until array is empty

​           **emplace** :  construct an object in-place            

​           **access and assign** :   access at a random position and assign a value (times = size)

**?**		number in [] is **unused** (wasted) capacity

**?**		time in **seconds**

**?**		reserve time is not included

**default growth formula:**

```c++
front  : empty_back_capacity() + front_capacity() * 1.25 + 2
back   : empty_front_capacity() + back_capacity() * 1.25 + 2
```

**cache:** 

- run script *clearcache.sh*  before any test, if you don't use GNU/Linux, just ignore it. 

- only run **one** operation on **one** container each test



## int



**1. SINGLE INSERT**

​		value = 0xBEEF

​		seed = 0xFACE

| times            | 300,000           | 600,000           | 1,000,000        | 1,500,000         | 2,000,000         | 2,500,000           | 3,000,000           |
| ---------------- | ----------------- | ----------------- | ---------------- | ----------------- | ----------------- | ------------------- | ------------------- |
| sda              | 1.20079 [25,252]  | 5.19093 [131,796] | 16.8916 [97,648] | 55.9712 [146,470] | 139.982 [469,901] | 263.141 [561,743]   | 430.605 [704,517]   |
| vector           | 2.41263 [224,288] | 10.5469 [448,576] | 34.3772 [48,576] | 122.939 [597,152] | 292.044 [97,152]  | 534.822 [1,694,304] | 866.574 [1,194,304] |
| deque            | 2.18154 [160]     | 9.03561 [64]      | 31.5675 [64]     | 105.746 [32]      | 248.689 [128]     | 455.697 [96]        | 725.059 [192]       |
| sda (reserve)    | 1.20164 [0]       | 5.1275 [0]        | 16.9275 [0]      | 56.9249 [0]       | 139.001 [0]       | 264.8 [0]           | 433.502 [0]         |
| vector (reserve) | 2.40136 [0]       | 10.4528 [0]       | 34.1968 [0]      | 120.838 [0]       | 286.157 [0]       | 535.395 [0]         | 869.288 [0]         |

note: 

- use **join** in sda (reserve), **join**'s algorithm is (theoretically) slower than **insert** 

- vector (reserve) speed sometimes is slower than empty vector, this proves two things:
  1. reserving space in vector brings (alsolutely) no speed benefit in random insert.
  2. there's always some minor difference between tests we run each time (some seconds may seem large but in total serveral minutes, it's very small)

**2. MULTIPLE INSERT**

​		value = 0xCAFE

​		seed = 0xF00D

| count            | 300                 | 100                 | 2000                 | 4000                 | 5000                 | 30,000                |
| ---------------- | ------------------- | ------------------- | -------------------- | -------------------- | -------------------- | --------------------- |
| **times**        | **10,000**          | **35,000**          | **10,000**           | **15,000**           | **20,000**           | **10,000**            |
| sda              | 1.48343 [977,283]   | 6.51943 [645,834]   | 15.4313 [3,212,944]  | 72.2227 [14,582,591] | 158.981 [24,334,055] | 251.942 [27,451,316]  |
| vector           | 3.02453 [1,915,200] | 12.9285 [3,053,600] | 30.3297 [12,768,000] | 143.787 [5,536,000]  | 316.088 [63,840,000] | 506.425 [191,520,000] |
| deque            | 2.1805 [192]        | 10.4949 [32]        | 17.275 [128]         | 78.5807 [128]        | 172.735 [128]        | 264.847 [128]         |
| sda (reserve)    | 1.45707 [0]         | 6.54542 [0]         | 15.2226 [0]          | 73.7815 [0]          | 161.291 [0]          | 264.447 [0]           |
| vector (reserve) | 2.97349 [0]         | 12.9776 [0]         | 30.302 [0]           | 143.871 [0]          | 319.164 [0]          | 504.786 [0]           |

note: same as single insert



**3. SINGLE DELETE**

​		value = 0xDEAD

​		seed = 0xFADE

| size      | 300,000     | 1,000,000   | 1,500,000   | 2,000,000   | 2,500,000   | 3,000,000     |
| --------- | ----------- | ----------- | ----------- | ----------- | ----------- | ------------- |
| **times** | **100,000** | **300,000** | **450,000** | **600,000** | **800,000** | **1,000,000** |
| sda       | 0.714593    | 10.0099     | 35.9565     | 94.5378     | 180.965     | 292.175       |
| vector    | 1.44898     | 19.8418     | 81.9591     | 190.636     | 352.05      | 573.543       |
| deque     | 1.17304     | 14.9224     | 47.7632     | 106.662     | 199.657     | 316.666       |

note: this test is obvious. sda moves smaller amount of elements, it also benifits from its contiguous memory layout.



**4. RANGE DELETE**

​		value = 0xBA5E

​		seed = 0xCA5E

| size      | 100,000,000   | 200,000,000   | 230,000,000   | 260,000,000   | 300,000,000    | 350,000,000    |
| --------- | ------------- | ------------- | ------------- | ------------- | -------------- | -------------- |
| **times** | **1,800**     | **7,600**     | **11,500**    | **15,400**    | **19,500**     | **29,500**     |
| **ratio** | **1.0 / 100** | **1.0 / 400** | **1.0 / 600** | **1.0 / 800** | **1.0 / 1000** | **1.0 / 1500** |
| sda       | 1.84808       | 15.2865       | 24.9607       | 36.0491       | 51.8336        | 90.602         |
| vector    | 4.15207       | 29.8163       | 51.0841       | 76.4191       | 102.965        | 184.897        |
| deque     | 1.97567       | 16.2194       | 26.4846       | 38.0789       | 54.4921        | 94.9902        |

*ratio = random_range.size / current_array.size : delete a random range has size equals to *ratio* of current array's size (range maybe empty)



**5. PUSH BACK**

​		value = 0x50DA

| times              | 100,000,000           | 150,000,000            | 200,000,000          | 250,000,000          | 300,000,000           |
| ------------------ | --------------------- | ---------------------- | -------------------- | -------------------- | --------------------- |
| sda                | 1.16052 [21,673,483]  | 1.50044 [2,091,855]    | 2.26194 [37,643,527] | 2.84497 [47,054,410] | 3.76763 [71,318,014]  |
| vector             | 0.579277 [34,217,728] | 0.994098 [118,435,456] | 1.14289 [68,435,456] | 1.29255 [18,435,456] | 2.18364 [236,870,912] |
| deque              | 0.329412 [128]        | 0.488005 [128]         | 0.655341 [128]       | 0.881483 [128]       | 1.07849 [128]         |
| sda (reserve_back) | 0.311081 [0]          | 0.449977 [0]           | 0.59708 [0]          | 0.745428 [0]         | 0.893562 [0]          |
| vector (reserve)   | 0.299035 [0]          | 0.468497 [0]           | 0.593812 [0]         | 0.736835 [0]         | 0.880434 [0]          |

note: sda uses default growth formula, so (obviously) pushing elements into it is slower (1.25 vs 2)



**6. PUSH FRONT**

​		value = 0x50FA

| times               | 100,000,000          | 150,000,000         | 200,000,000         | 250,000,000          | 300,000,000          |
| ------------------- | -------------------- | ------------------- | ------------------- | -------------------- | -------------------- |
| sda                 | 1.17465 [21,673,483] | 1.47869 [2,091,855] | 2.2892 [37,643,527] | 2.85641 [47,054,410] | 3.74924 [71,318,014] |
| deque               | 0.324824 [128]       | 0.501901 [128]      | 0.643912 [128]      | 0.91443 [128]        | 1.08147 [128]        |
| sda (reserve_front) | 0.359113 [0]         | 0.529735 [0]        | 0.726702 [0]        | 0.901044 [0]         | 1.04793 [0]          |

note: same as **PUSH BACK**



**7. POP BACK**

​		value = 0xD1CE

| size   | 100,000,000 | 150,000,000 | 200,000,000 | 250,000,000 | 300,000,000 |
| ------ | ----------- | ----------- | ----------- | ----------- | ----------- |
| sda    | 0.000002    | 0.000001    | 0.000001    | 0.000002    | 0.000003    |
| vector | 0.000002    | 0.000001    | 0.000002    | 0.000002    | 0.000002    |
| deque  | 0.210624    | 0.316784    | 0.423526    | 0.527724    | 0.63352     |



**8. POP FRONT**

​		value = 0x5EED

| size  | 100,000,000 | 150,000,000 | 200,000,000 | 250,000,000 | 300,000,000 |
| ----- | ----------- | ----------- | ----------- | ----------- | ----------- |
| sda   | 0.000001    | 0.000002    | 0.000003    | 0.000003    | 0.000003    |
| deque | 0.228291    | 0.342578    | 0.456481    | 0.571723    | 0.686627    |



**9. ACCESS and ASSIGN**

​		value = 0xFEED

​		seed = 0x5AFE

| size   | 100,000,000 | 150,000,000 | 200,000,000 | 250,000,000 | 300,000,000 |
| ------ | ----------- | ----------- | ----------- | ----------- | ----------- |
| sda    | 2.15983     | 3.26107     | 4.77358     | 6.60997     | 8.95708     |
| vector | 2.04386     | 3.09008     | 4.72612     | 6.37354     | 8.61067     |
| deque  | 8.68424     | 16.977      | 25.965      | 35.5256     | 45.0617     |







## custom class

... Working ...