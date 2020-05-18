## LIST OF ALL MEMBER FUNCTIONS



### CONSTRUCTOR

```c++
sda()
sda(size_type n, const value_type& val, const allocator_type& alloc = allocator_type())
explicit sda(size_type n, const allocator_type& alloc = allocator_type())
template<class InputIterator>
sda(InputIterator first, InputIterator last, const allocator_type& alloc = allocator_type())
sda(const sda& other)
sda(const sda& other, const allocator_type& alloc)
sda(sda&& other) noexcept
sda(sda&& other, const allocator_type& alloc)
sda(std::initializer_list<value_type> il, const allocator_type& alloc = allocator_type())
```



### DESTRUCTOR

```c++
~sda()
```



### ASSIGN

```c++
template<class InputIterator>
void assign(InputIterator first, InputIterator last)
void assign(size_type n, const value_type& val)
void assign(std::initializer_list<value_type> il)
```



### OPERATOR=

```c++
sda& operator= (const sda& other)
sda& operator= (sda&& other) noexcept
sda& operator= (std::initializer_list<value_type> il)
```



### ITERATORS

```c++
iterator begin() noexcept
const_iterator begin() const noexcept
iterator end() noexcept
const_iterator end() const noexcept
reverse_iterator rbegin() noexcept
const_reverse_iterator rbegin() const noexcept
reverse_iterator rend() noexcept
const_reverse_iterator rend() const noexcept
const_iterator cbegin() const noexcept
const_iterator cend() const noexcept
const_reverse_iterator crbegin() const noexcept
const_reverse_iterator crend() const noexcept
```



### SIZE, CAPACITY

```c++
size_type size() const noexcept
size_type front_capacity() const noexcept
size_type back_capacity() const noexcept
size_type capacity() const noexcept
size_type empty_front_capacity() const noexcept
size_type empty_back_capacity() const noexcept
size_type empty_capacity() const noexcept
size_type max_size() const noexcept
bool empty() const noexcept
```



### RESERVE, SHRINK, RESIZE

```c++
void reserve(size_type n)
void reserve_back(size_type n)
void reserve_front(size_type n)
void shrink_to_fit()
void resize_back(size_type n)
void resize_back(size_type n, const value_type& val)
void resize_front(size_type n)
void resize_front(size_type n, const value_type& val)
```



### SLIDE

```c++
void slide_to_back(size_type n = 0)
void slide_to_front(size_type n = 0)
```



### ACCESS

```c++
reference operator[] (size_type n)
const_reference operator[] (size_type n) const
reference at(size_type n)
const_reference at(size_type n) const
reference front()
const_reference front() const
reference back()
const_reference back() const
constexpr pointer data() noexcept
constexpr const_pointer data() const noexcept
```



### INSERT, EMPLACE

```c++
void clear() noexcept
iterator insert(const_iterator pos, const value_type& val)
iterator insert(const_iterator pos, value_type&& val )
iterator insert(const_iterator pos, size_type n, const value_type& val)
template<class InputIterator>
iterator insert(const_iterator pos, InputIterator first, InputIterator last)
iterator insert(const_iterator pos, std::initializer_list<value_type> il)
template<class... Args>
iterator emplace_back(Args&&... args)
template<class... Args>
iterator emplace_front(Args&&... args)
template<class... Args>
iterator emplace(const_iterator pos, Args&&... args)
```



### JOIN, EMJOIN

```c++
iterator join(const_iterator pos, const value_type& val)
iterator join(const_iterator pos, const value_type&& val)
iterator join(const_iterator pos, size_type n, const value_type& val)
template<class InputIterator>
iterator join(const_iterator pos, InputIterator first, InputIterator last)
iterator join(const_iterator pos, std::initializer_list<value_type> il)
template<class... Args>
iterator emjoin(const_iterator pos, Args&&... args)
```



### PUSH, POP

```c++
void push_back(const value_type& val)
void push_back(value_type&& val)
void push_front(const value_type& val)
void push_front(value_type&& val)
void pop_back()
void pop_front()
```



### ERASE

```c++
iterator erase(const_iterator pos)
iterator erase(const_iterator first, const_iterator last)
```



### SWAP

```c++
void swap(sda& other) noexcept
```



### ALLOCATOR

```c++
allocator_type get_allocator() const
```



### FORMULA

```c++
void set_front_growth_formula(double rate, double extra)
void set_back_growth_formula(double rate, double extra)
void reset_growth_formula()
```

