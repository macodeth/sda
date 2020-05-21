/*
MIT License

Copyright 2020 Than Minh Duy (macodeth)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
OR OTHER DEALINGS IN THE SOFTWARE.

*/


#ifndef SYMMETRIC_DYNAMIC_ARRAY
#define SYMMETRIC_DYNAMIC_ARRAY



#include<algorithm>
#include<memory>
#include<cstring>
#include<iterator>
#include<initializer_list>
#include<exception>
#include<type_traits>
#include<limits>



//
//                 ......12345678901234567890.......
//                 ^     ^                   ^      ^
//               head_ begin_              end_   tail_
//
//                 <---- front capacity ---->
//                       <----- back capacity ----->
//


template<class T, class Allocator = std::allocator<T>>
class sda
{
   public:
   typedef std::allocator_traits<Allocator> alloc_trait;
   typedef Allocator allocator_type;
   typedef T value_type;
   typedef typename alloc_trait::pointer pointer;
   typedef typename alloc_trait::const_pointer const_pointer;
   typedef typename alloc_trait::size_type size_type;
   typedef typename alloc_trait::difference_type difference_type;
   typedef value_type& reference;
   typedef const value_type& const_reference;
   typedef pointer iterator;
   typedef const_pointer const_iterator;
   typedef std::reverse_iterator<iterator> reverse_iterator;
   typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

   template<typename InputIterator>
   using RequireInputIterator = typename
      std::enable_if< std::is_convertible < typename
      std::iterator_traits<InputIterator>::iterator_category,
      std::input_iterator_tag>::value>::type;

   static constexpr bool trivial_copy = std::is_trivially_copyable<value_type>::value;


   private:
   struct Impl : public Allocator
   {
      pointer head_;
      pointer tail_;
      pointer begin_;
      pointer end_;

      Impl() : Allocator(), head_(nullptr), tail_(nullptr), begin_(nullptr), end_(nullptr) {}

      Impl(const allocator_type& alloc) : Allocator(alloc), head_(nullptr), tail_(nullptr), begin_(nullptr), end_(nullptr) {}

      Impl(allocator_type&& alloc) : Allocator(std::move(alloc)), head_(nullptr), tail_(nullptr), begin_(nullptr), end_(nullptr) {}

      Impl(Impl&& other) : Allocator(std::move(other)), head_(other.head_), tail_(other.tail_), begin_(other.begin_), end_(other.end_)
      {
         other.head_ = other.tail_ = other.begin_ = other.end_ = nullptr;
      }

      Impl(size_type n, const allocator_type& alloc = allocator_type()) : Allocator(alloc)
      {
         head_ = alloc_trait::allocate(*this, n);
         tail_ = head_ + n;
         begin_ = head_;
         end_ = tail_;
      }
      //   completely destroy, deallocate allocated memory
      void deallocate()
      {
         destroy(begin_, end_);
         alloc_trait::deallocate(*this, head_, tail_ - head_);
         head_ = tail_ = begin_ = end_ = nullptr;
      }
      //    destroy part of allocated memory
      void destroy(pointer start, pointer finish)
      {
         for(; start != finish; start++) alloc_trait::destroy(*this, start);
      }

      //---------------------------
      //    GROWTH FORMULA
      //---------------------------
      size_type new_capacity_front_growing()
      {
         size_type front_capacity = end_ - head_;
         return tail_ - end_ + front_capacity + (front_capacity >> 2) + 2;
      }
      size_type new_capacity_back_growing()
      {
         size_type back_capacity = tail_ - begin_;
         return begin_ - head_ + back_capacity + (back_capacity >> 2) + 2;
      }
      pointer balance_begin(pointer head, size_type size, size_type capacity)
      {
         return head + ((capacity - size) >> 1);
      }

      //-----------------------------------------------------------------
      //    GROW
      //    grow       : divide unused memory equally to both sides
      //    grow_front : change front capacity only
      //    grow_back  : change back capacity only
      //-----------------------------------------------------------------
      pointer grow(size_type capacity)
      {
         size_type size = end_ - begin_;

         pointer new_head = alloc_trait::allocate(*this, capacity);
         if(new_head)
         {
            pointer new_begin = balance_begin(new_head, size, capacity);
            uninitialized_move(begin_, end_, new_begin);
            deallocate();
            head_ = new_head;
            tail_ = head_ + capacity;
            begin_ = new_begin;
            end_ = begin_ + size;
         }
         return new_head;
      }
      pointer grow_front(size_type new_front_capacity = 0)
      {
         size_type capacity = new_front_capacity ? 
            tail_ - end_ + new_front_capacity : new_capacity_front_growing();
         size_type size = end_ - begin_;
         
         pointer new_head = alloc_trait::allocate(*this, capacity);
         if(new_head)
         {
            pointer new_tail = new_head + capacity;
            pointer new_end = new_tail - (tail_ - end_);
            pointer new_begin = new_end - size;
            uninitialized_move(begin_, end_, new_begin);
            deallocate();
            head_ = new_head;
            tail_ = new_tail;
            end_ = new_end;
            begin_ = new_begin;
         }
         return new_head;
      }
      pointer grow_back(size_type new_back_capacity = 0)
      {
         size_type capacity = new_back_capacity ? 
            begin_ - head_ + new_back_capacity : new_capacity_back_growing();
         size_type size = end_ - begin_;

         pointer new_head = alloc_trait::allocate(*this, capacity);
         if(new_head)
         {
            pointer new_begin = new_head + (begin_ - head_);
            uninitialized_move(begin_, end_, new_begin);
            deallocate();
            head_ = new_head;
            tail_ = head_ + capacity;
            begin_ = new_begin;
            end_ = begin_ + size;
         }
         return new_head;
      }
   } impl_;
   
   static void swap(Impl& a, Impl& b)
   {
      std::swap(a.head_, b.head_);
      std::swap(a.tail_, b.tail_);
      std::swap(a.begin_, b.begin_);
      std::swap(a.end_, b.end_);
   }

   //--------------------
   //    MOVE ASSIGN
   //--------------------
   void moveAssign(sda& other)
   {
      if constexpr (std::integral_constant<bool, 
          alloc_trait::propagate_on_container_move_assignment::value>() 
          or (impl_ == other.impl_))
      {
         impl_.deallocate();
         swap(impl_, other.impl_);
      }
      else
         assign(other.begin(), other.end());
   }
   
   //-------------------
   //    EXCEPTION
   //-------------------
   void throw_length_error()
   {
      throw std::length_error("std::length_error");
   }
   void throw_out_of_range()
   {
      throw std::out_of_range("std::out_of_range");
   }

   //----------------------------------------------
   //    HELPER FUNCTIONS
   //    move, copy, fill
   //----------------------------------------------
   
   //    separate, no overlapped - 3 functions
   //    data in moved region is already constructed
   //    or hasn't called destructor (can be dereferenced)
   static void move_separate(pointer first, pointer last, pointer d_first)
   {
      if constexpr (trivial_copy)
         memcpy(d_first, first, sizeof(value_type) * (last - first));
      else std::move(first, last, d_first);
   }
   static void move_backward_separate(pointer first, pointer last, pointer d_last)
   {
      if constexpr (trivial_copy)
         memcpy(d_last - (last - first), first, sizeof(value_type) * (last - first));
      else std::move_backward(first, last, d_last);
   }
   //    move to uninitialized region of data
   static void uninitialized_move(pointer first, pointer last, pointer d_first)
   {
      if constexpr (trivial_copy)
         memcpy(d_first, first, sizeof(value_type) * (last - first));
      else std::uninitialized_move(first, last, d_first);
   }

   // copy data in [first, last) to array begins at d_first
   static void copy(pointer first, pointer last, pointer d_first)
   {
      if constexpr (trivial_copy)
         memcpy(d_first, first, sizeof(value_type) * (last - first));
      else std::copy(first, last, d_first);
   }


   //------------------------------------------------
   //    use 2 functions to move data safely    
   //    unused data is also destroyed
   //------------------------------------------------

   //    move forward, maybe overlapped or not
   static void move_generic(allocator_type& alloc, pointer first, pointer last, pointer d_first)
   {
      size_type k = first - d_first;
      size_type n = last - first;
      if(k && n)
      {
         if(k <= n)
         {
            uninitialized_move(first, first + k, d_first);
            move_separate(first + k, last, first);
            for(pointer p = last - k; p != last; p++)
               std::allocator_traits<allocator_type>::destroy(alloc, p);
         }
         else
         {
            uninitialized_move(first, last, d_first);
            for(pointer p = first; p != last; p++)
               std::allocator_traits<allocator_type>::destroy(alloc, p);
         }
      }
   }
   //    move backward, maybe overlapped or not
   static void move_backward_generic(allocator_type& alloc, pointer first, pointer last, pointer d_last)
   {
      size_type k = d_last - last;
      size_type n = last - first;
      if(k && n)
      {      
         if(k <= n)
         {
            uninitialized_move(last - k, last, last);
            move_backward_separate(first, last - k, last);
            for(pointer p = first; p != first + k; p++)
               std::allocator_traits<allocator_type>::destroy(alloc, p);
         }
         else
         {
            uninitialized_move(first, last, d_last - n);
            for(pointer p = first; p != last; p++)
               std::allocator_traits<allocator_type>::destroy(alloc, p);
         }
      }
   }
   static void uninitialized_fill(allocator_type& alloc, pointer first, pointer last, const value_type& val)
   {
      for(; first != last; first++)
         std::allocator_traits<allocator_type>::construct(alloc, first, val);
   }
   static void fill(allocator_type& alloc, pointer first, pointer last, const value_type& val)
   {
      alloc.destroy(first, last);
      uninitialized_fill(first, last, val);
   }


   //-----------------------------------------------
   //    EMPLACE, INSERT
   //    current space is enough for new element(s)
   //    only construction or moving is needed
   //-----------------------------------------------
   template<class... Args>
   void emplace_back_construct(Args&&... args)
   {
      alloc_trait::construct(impl_, impl_.end_, std::forward<Args>(args)...);
      impl_.end_++;
   }   
   template<class... Args>
   void emplace_front_construct(Args&&... args)
   {
      impl_.begin_--;
      alloc_trait::construct(impl_, impl_.begin_, std::forward<Args>(args)...);
   }


   template<class... Args>
   void emplace_construct(size_type pos, Args&&... args)
   {
      bool near_end = is_back_smaller(pos);
      if(near_end)
      {
         move_backward_generic(impl_, impl_.begin_ + pos, impl_.end_, impl_.end_ + 1);
         impl_.end_++;            
      }
      else
      {
         move_generic(impl_, impl_.begin_, impl_.begin_ + pos, impl_.begin_ - 1);
         impl_.begin_--;
      }
      alloc_trait::construct(impl_, impl_.begin_ + pos, std::forward<Args>(args)...);
   }
   template<class... Args>
   void insert_multiple_construct(size_type pos, size_type n, const value_type& val)
   {
      bool near_end = is_back_smaller(pos);
      if(near_end)
      {
         move_backward_generic(impl_, impl_.begin_ + pos, impl_.end_, impl_.end_ + n);
         impl_.end_ += n;
      }
      else
      {
         move_generic(impl_, impl_.begin_, impl_.begin_ + pos, impl_.begin_ - n);
         impl_.begin_ -= n;
      }
      uninitialized_fill(impl_, impl_.begin_ + pos, impl_.begin_ + pos + n, val);
   }
   template<class InputIterator, typename = RequireInputIterator<InputIterator>>
   void insert_range_construct(size_type pos, InputIterator first, InputIterator last)
   {
      bool near_end = is_back_smaller(pos);
      size_type n = std::distance(first, last);
      if(near_end)
      {
         move_backward_generic(impl_, impl_.begin_ + pos, impl_.end_, impl_.end + n);
         impl_.end_ += n;
      }
      else
      {
         move_generic(impl_, impl_.begin_, impl_.begin_ + pos, impl_.begin_ - n);
         impl_.begin_ -= n;
      }
      copy(first, last, impl_.begin_ + pos);
   }



   //-----------------------------------------------
   //    EMPLACE, INSERT
   //    require reallocation
   //-----------------------------------------------
   template<class... Args>
   void emplace_back_realloc(Args&&... args)
   {
      pointer new_head = impl_.grow_back();
      if(new_head) emplace_back_construct(std::forward<Args>(args)...);
   }   
   template<class... Args>
   void emplace_front_realloc(Args&&... args)
   {
      pointer new_head = impl_.grow_front();
      if(new_head) emplace_front_construct(std::forward<Args>(args)...);
   }


   template<class... Args>
   void emplace_realloc(size_type pos, Args&&... args)
   {
      bool near_end = is_back_smaller(pos);
      if(near_end)
      {
         pointer new_head = impl_.grow_back();
         if(new_head)
            emplace_construct(pos, std::forward<Args>(args)...);            
      }
      else
      {
         pointer new_head = impl_.grow_front();
         if(new_head)
            emplace_construct(pos, std::forward<Args>(args)...);
      }
   }

   void insert_multiple_realloc(size_type pos, size_type n, const value_type& val)
   {
      bool near_end = is_back_smaller(pos);
      if(near_end)
      {
         pointer new_head = impl_.grow_back();
         while((empty_back_capacity() < n) && new_head)
            impl_.grow_back();
         if(new_head)
            insert_multiple_construct(pos, n, val);            
      }
      else
      {
         pointer new_head = impl_.grow_front();
         while((empty_front_capacity() < n) && new_head)
            impl_.grow_front();
         if(new_head)
            insert_multiple_construct(pos, n, val);
      }
   }
   template<class InputIterator, typename = RequireInputIterator<InputIterator>>
   void insert_range_realloc(size_type pos, InputIterator first, InputIterator last)
   {
      bool near_end = is_back_smaller(pos);
      size_type n = std::distance(first, last);
      if(near_end)
      {
         pointer new_head = impl_.grow_back();
         while((empty_back_capacity() < n) && new_head)
            impl_.grow_back();
         if(new_head)
            insert_range_construct(pos, first, last);
      }
      else
      {
         pointer new_head = impl_.grow_front();
         while((empty_front_capacity() < n) && new_head)
            impl_.grow_front();
         if(new_head)
            insert_range_construct(pos, first, last);
      }
   }

   bool is_back_smaller(size_type pos) const noexcept
   {
      return pos > (size() - pos);
   }

   bool is_back_smaller(const_iterator pos) const noexcept
   {
      return (pos - impl_.begin_) > (impl_.end_ - pos);
   }

   bool is_back_smaller(size_type first, size_type last) const noexcept
   {
      return first > (size() - last);
   }

   bool is_back_smaller(const_iterator first, const_iterator last) const noexcept
   {
      return (first - impl_.begin_) > (impl_.end_ - last);
   }

   /*
   ================================================================
   ================================================================


                        PUBLIC FUNCTION


   ================================================================
   ================================================================
   */


   public:
   //-------------------- 
   //    CONSTRUCTOR
   //--------------------
   sda() = default;

   explicit sda(const allocator_type& alloc) noexcept : impl_(alloc) {}

   sda(size_type n, const value_type& val, const allocator_type& alloc = allocator_type()) : impl_(n, alloc) 
   {
      uninitialized_fill(impl_, impl_.begin_, impl_.end_, val);
   }

   explicit sda(size_type n, const allocator_type& alloc = allocator_type()) : impl_(n, alloc) {}

   template<class InputIterator, typename = RequireInputIterator<InputIterator>>
   sda(InputIterator first, InputIterator last, const allocator_type& alloc = allocator_type())
   : impl_(std::distance(first, last), alloc)
   {
      std::uninitialized_copy(first, last, impl_.begin_);
   }
   
   sda(const sda& other) : sda(other, alloc_trait::select_on_container_copy_construction(other.get_allocator())) {}

   sda(const sda& other, const allocator_type& alloc) : sda(other.begin(), other.end(), alloc) {}
   
   sda(sda&& other) noexcept : impl_(std::move(other.impl_)) {}

   sda(sda&& other, const allocator_type& alloc) : impl_(std::move(other.impl_), alloc) {}
   
   sda(std::initializer_list<value_type> il, const allocator_type& alloc = allocator_type())
   : sda(il.begin(), il.end(), alloc) {}

   allocator_type get_allocator() const
   {
      return impl_;
   }

   //----------------
   //    ASSIGN
   //----------------
   template<class InputIterator, typename = RequireInputIterator<InputIterator>>
   void assign(InputIterator first, InputIterator last)
   {
      size_type n = std::distance(first, last);
      clear();
      reserve(n);
      if(capacity() >= n)
      {
         impl_.begin_ = impl_.balance_begin(impl_.head_, n);
         impl_.end_ = impl_.begin_ + n;
         copy(first, last, impl_.begin_);
      }
   }
   void assign(size_type n, const value_type& val)
   {
      clear();
      reserve(n);
      if(capacity() >= n)
      {
         impl_.begin_ = impl_.balance_begin(impl_.head_, n);
         impl_.end_ = impl_.begin_ + n;
         fill(impl_, impl_.begin_, impl_.end_, val);
      }
   }
   void assign(std::initializer_list<value_type> il)
   {
      assign(il.begin(), il.end());
   }

   //-------------------
   //    DESTRUCTOR
   //-------------------
   ~sda() = default;

   //-------------------
   //    OPERATOR =
   //-------------------
   sda& operator= (const sda& other)
   {
      if(alloc_trait::propagate_on_container_copy_assignment::value)
      {
         if (impl_ != other.impl_) clear();
         impl_ = other.impl_;
      }
      assign(other.begin(), other.end());
      return *this;
   }
   sda& operator= (sda&& other) noexcept
   {
      moveAssign(other);
      return *this;
   }
   sda& operator= (std::initializer_list<value_type> il)
   {
      assign(il.begin(), il.end());
      return *this;
   }


   //------------------
   //    ITERATORS
   //------------------
   iterator begin() noexcept
   {
      return iterator(impl_.begin_);
   }   
   const_iterator begin() const noexcept
   {
      return const_iterator(impl_.begin_);
   }
   iterator end() noexcept
   {
      return iterator(impl_.end_);
   }
   const_iterator end() const noexcept
   {
      return const_iterator(impl_.end_);
   }
   reverse_iterator rbegin() noexcept
   {
      return reverse_iterator(impl_.end_ - 1);
   }
   const_reverse_iterator rbegin() const noexcept
   {
      return const_reverse_iterator(impl_.end_ - 1);
   }
   reverse_iterator rend() noexcept
   {
      return reverse_iterator(impl_.begin_ - 1);
   }
   const_reverse_iterator rend() const noexcept
   {
      return const_reverse_iterator(impl_.begin_ - 1);
   }
   const_iterator cbegin() const noexcept
   {
      return const_iterator(impl_.begin_);
   }
   const_iterator cend() const noexcept
   {
      return const_iterator(impl_.end_);
   }
   const_reverse_iterator crbegin() const noexcept
   {
      return const_reverse_iterator(impl_.end_ - 1);
   }
   const_reverse_iterator crend() const noexcept
   {
      return const_reverse_iterator(impl_.begin_ - 1);
   }


   //------------------------
   //    CAPACITY
   //------------------------
   size_type size() const noexcept
   {
      return impl_.end_ - impl_.begin_;
   }
   size_type front_capacity() const noexcept
   {
      return impl_.end_ - impl_.head_;
   }
   size_type back_capacity() const noexcept
   {
      return impl_.tail_ - impl_.begin_;
   }
   size_type capacity() const noexcept
   {
      return impl_.tail_ - impl_.head_;
   }
   size_type empty_front_capacity() const noexcept
   {
      return impl_.begin_ - impl_.head_;
   }
   size_type empty_back_capacity() const noexcept
   {
      return impl_.tail_ - impl_.end_;
   }
   size_type empty_capacity() const noexcept
   {
      return empty_front_capacity() + empty_back_capacity();
   }
   size_type max_size() const noexcept
   {
      return std::numeric_limits<size_type>::max();
   }
   bool empty() const noexcept
   {
      return impl_.begin_ == impl_.end_;
   }

   //-------------------------
   //    RESERVE, SHRINK
   //-------------------------
   void reserve(size_type n)
   {
      if(n > max_size()) throw_length_error();
      if(n > capacity()) impl_.grow(n);
   }
   void reserve_back(size_type n)
   {
      if(n +  empty_front_capacity() > max_size()) throw_length_error();
      if(n > back_capacity()) impl_.grow_back(n);
   }
   void reserve_front(size_type n)
   {
      if(n + empty_back_capacity() > max_size()) throw_length_error();
      if(n > front_capacity()) impl_.grow_front(n);
   }
   void shrink_to_fit()
   {
      if(size() != capacity())
      {
         if(size()) impl_.grow(size());
         else impl_.deallocate();
      }
   }

   //-------------------------------------------------------------
   //    SLIDE
   //
   //    reduce back capacity, front capacity
   //    set empty back capacity = n or
   //    empty front capacity = n
   //    n must be smaller or equal to respective empty capacity
   //-------------------------------------------------------------
   void slide_to_back(size_type n = 0)
   {
      size_type size_temp = size();
      pointer old_begin = impl_.begin_;
      move_backward_generic(impl_, impl_.begin_, impl_.end_, impl_.tail_ - n);
      impl_.end_ = impl_.tail_ - n;
      impl_.begin_ = impl_.end_ - size_temp;
      impl_.destroy(old_begin, impl_.begin_);
      return;
   }
   void slide_to_front(size_type n = 0)
   {
      size_type size_temp = size();
      pointer old_end = impl_.end_;
      move_generic(impl_, impl_.begin_, impl_.end_, impl_.head_ + n);
      impl_.begin_ = impl_.head_ + n;
      impl_.end_ = impl_.begin_ + size_temp;
      impl_.destroy(impl_.end_, old_end);
      return;
   }

   //-----------------------
   //    ELEMENT ACCESS
   //-----------------------
   reference operator[] (size_type n)
   {
      return impl_.begin_[n];
   }
   const_reference operator[] (size_type n) const
   {
      return impl_.begin_[n];
   }
   reference at(size_type n)
   {
      if(n < size()) return impl_.begin_[n];
      throw_out_of_range();
   }
   const_reference at(size_type n) const
   {
      if(n < size()) return impl_.begin_[n];
      throw_out_of_range();
   }
   reference front()
   {
      return impl_.begin_[0];
   }
   const_reference front() const
   {
      return impl_.begin_[0];
   }
   reference back()
   {
      return impl_.end_[0];
   }
   const_reference back() const
   {
      return impl_.end_[0];
   }
   constexpr pointer data() noexcept
   {
      return impl_.begin_;
   }
   constexpr const_pointer data() const noexcept
   {
      return impl_.begin_;
   }




   //----------------
   //    INSERT
   //----------------
   void clear() noexcept
   {
      impl_.destroy(impl_.begin_, impl_.end_);
      impl_.end_ = impl_.begin_;
   }
   iterator insert(const_iterator pos, const value_type& val)
   {
      return emplace(pos, val);
   }
   iterator insert(const_iterator pos, value_type&& val )
   {
      return emplace(pos, std::move(val));
   }
   iterator insert(const_iterator pos, size_type n, const value_type& val)
   {
      bool near_end = is_back_smaller(pos);
      bool enough_space = near_end ? (empty_back_capacity() >= n) : (empty_front_capacity() >= n);
      size_type pos_i = pos - impl_.begin_;
      if(enough_space)
         insert_multiple_construct(pos_i, n, val);
      else
         insert_multiple_realloc(pos_i, n, val);
      return impl_.begin_ + pos_i;
   }

   template<class InputIterator, typename = RequireInputIterator<InputIterator>>
   iterator insert(const_iterator pos, InputIterator first, InputIterator last)
   {
      bool near_end = is_back_smaller(pos);
      size_type n = std::distance(first, last);
      bool enough_space = near_end ? (empty_back_capacity() >= n) : (empty_front_capacity() >= n);
      size_type pos_i = pos - impl_.begin_;
      if(enough_space)
         insert_range_construct(pos_i, first, last);
      else
         insert_range_realloc(pos_i, first, last);
      return impl_.begin_ + pos_i;
   }
   iterator insert(const_iterator pos, std::initializer_list<value_type> il)
   {
      return insert(pos, il.begin(), il.end());
   }

   //------------------------------------------
   //    JOIN
   //    current array must have
   //    enough memory (no reallocation)
   //------------------------------------------
   iterator join(const_iterator pos, const value_type& val)
   {
      return emjoin(pos, val);
   }
   iterator join(const_iterator pos, const value_type&& val)
   {
      return emjoin(pos, std::move(val));
   }
   iterator join(const_iterator pos, size_type n, const value_type& val)
   {
      bool near_end = is_back_smaller(pos);
      size_type pos_i = pos - impl_.begin_;
      if((near_end && (empty_back_capacity() >= n)) || 
         ((!near_end) && (empty_front_capacity() >= n)))
            return insert(pos, n, val);
      if(empty_back_capacity() >= n)
      {
         move_backward_generic(impl_, impl_.begin_ + pos_i, impl_.end_, impl_.end_ + n);
         impl_.end_ += n;
      }
      else if(empty_front_capacity() >= n)
      {
         move_generic(impl_, impl_.begin_, impl_.begin_ + pos_i, impl_.begin_ - n);
         impl_.begin_ -= n;
      }
      else
      {
         size_type empty_front = empty_front_capacity();
         move_generic(impl_, impl_.begin_, impl_.begin_ + pos_i, impl_.head_);
         impl_.begin_ = impl_.head_;
         size_type back_move_step = n - empty_front;
         move_backward_generic(impl_, impl_.begin_ + pos_i, impl_.end_, impl_.end_ + back_move_step);
         impl_.end_ += back_move_step;
      }
      uninitialized_fill(impl_, impl_.begin_ + pos_i, impl_.begin_ + pos_i + n, val);
      return impl_.begin_ + pos_i;
   }
   template<class InputIterator, typename = RequireInputIterator<InputIterator>>
   iterator join(const_iterator pos, InputIterator first, InputIterator last)
   {
      bool near_end = is_back_smaller(pos);
      size_type pos_i = pos - impl_.begin_;
      size_type n = std::distance(first, last);
      if((near_end && (empty_back_capacity() >= n)) || 
         ((!near_end) && (empty_front_capacity() >= n)))
            return insert(pos, first, last);
      if(empty_back_capacity() >= n)
      {
         move_backward_generic(impl_, impl_.begin_ + pos_i, impl_.end_, impl_.end_ + n);
         impl_.end_ += n;
      }
      else if(empty_front_capacity() >= n)
      {
         move_generic(impl_, impl_.begin_, impl_.begin_ + pos_i, impl_.begin_ - n);
         impl_.begin_ -= n;
      }
      else
      {
         size_type empty_front = empty_front_capacity();
         move_generic(impl_, impl_.begin_, impl_.begin_ + pos_i, impl_.head_);
         impl_.begin_ = impl_.head_;
         size_type back_move_step = n - empty_front;
         move_backward_generic(impl_, impl_.begin_ + pos_i, impl_.end_, impl_.end_ + back_move_step);
         impl_.end_ += back_move_step;
      }
      copy(first, last, impl_.begin_ + pos_i);
      return impl_.begin_ + pos_i;
      
   }
   iterator join(const_iterator pos, std::initializer_list<value_type> il)
   {
      return join(pos, il.begin(), il.end());
   }


   //--------------
   //    ERASE
   //--------------


   iterator erase(const_iterator pos)
   {
      bool near_end = is_back_smaller(pos);
      size_type pos_i = pos - impl_.begin_;
      alloc_trait::destroy(impl_, impl_.begin_ + pos_i);
      if(near_end)
      {
         move_generic(impl_, impl_.begin_ + pos_i + 1, impl_.end_, impl_.begin_ + pos_i);
         impl_.end_--;
      }
      else
      {
         move_backward_generic(impl_, impl_.begin_, impl_.begin_ + pos_i, impl_.begin_ + pos_i + 1);
         impl_.begin_++;   
      }
      return impl_.begin_ + pos_i;   
   }
   iterator erase(const_iterator first, const_iterator last)
   {
      bool near_end = is_back_smaller(first, last);
      size_type first_i = first - impl_.begin_;
      size_type last_i = last - impl_.begin_;
      size_type n = last - first;
      impl_.destroy(impl_.begin_ + first_i, impl_.begin_ + last_i);
      if(near_end)
      {
         move_generic(impl_, impl_.begin_ + last_i, impl_.end_, impl_.begin_ + first_i);
         impl_.end_ -= n;
      }
      else
      {
         move_backward_generic(impl_, impl_.begin_, impl_.begin_ + first_i, impl_.begin_ + last_i);
         impl_.begin_ += n;
      }
      return impl_.begin_ + first_i;
   }

   //--------------
   //    PUSH
   //--------------
   void push_back(const value_type& val)
   {
      if(impl_.end_ != impl_.tail_)
      {
         emplace_back_construct(val);
         return;
      }
      emplace_back_realloc(val);
   }
   void push_back(value_type&& val)
   {
      if(impl_.end_ != impl_.tail_)
      {
         emplace_back_construct(std::move(val));
         return;
      }
      emplace_back_realloc(std::move(val));
   }
   void push_front(const value_type& val)
   {
      if(impl_.begin_ != impl_.head_)
      {
         emplace_front_construct(val);
         return;
      }
      emplace_front_realloc(val);
   }
   void push_front(value_type&& val)
   {
      if(impl_.begin_ != impl_.head_)
      {
         emplace_front_construct(std::move(val));
         return;
      }
      emplace_front_realloc(std::move(val));
   }

   //-----------------
   //    EMPLACE
   //-----------------
   template<class... Args>
   iterator emplace_back(Args&&... args)
   {
      if(impl_.end_ != impl_.tail_)
      {
         emplace_back_construct(std::forward<Args>(args)...);
         return impl_.end_ - 1;
      }
      emplace_back_realloc(std::forward<Args>(args)...);
      return impl_.end_ - 1;
   }
   template<class... Args>
   iterator emplace_front(Args&&... args)
   {
      if(impl_.begin_ != impl_.head_)
      {
         emplace_front_construct(std::forward<Args>(args)...);
         return impl_.begin_;
      }
      emplace_front_realloc(std::forward<Args>(args)...);
      return impl_.begin_;
   }


   template<class... Args>
   iterator emplace(const_iterator pos, Args&&... args)
   {
      bool near_end = is_back_smaller(pos);
      bool enough_space = near_end ? (empty_back_capacity() > 0) : (empty_front_capacity() > 0);
      size_type pos_i = pos - impl_.begin_;
      if(enough_space)
         emplace_construct(pos_i, std::forward<Args>(args)...);
      else
         emplace_realloc(pos_i, std::forward<Args>(args)...);
      return impl_.begin_ + pos_i;
   }
   //    join version of emplace
   //    never reallocate, current array must have enough memory
   template<class... Args>
   iterator emjoin(const_iterator pos, Args&&... args)
   {
      bool near_end = is_back_smaller(pos);
      size_type pos_i = pos - impl_.begin_;
      if((near_end && empty_back_capacity()) || ((!near_end) && (empty_front_capacity())))
         return emplace(pos, std::forward<Args>(args)...);
      if(empty_back_capacity())
      {
         move_backward_generic(impl_, impl_.begin_ + pos_i, impl_.end_, impl_.end_ + 1);
         impl_.end_ ++;
         alloc_trait::construct(impl_, impl_.begin_ + pos_i, std::forward<Args>(args)...);
      }
      else
      {
         move_generic(impl_, impl_.begin_, impl_.begin_ + pos_i, impl_.begin_ - 1);
         impl_.begin_ --;
         alloc_trait::construct(impl_, impl_.begin_ + pos_i, std::forward<Args>(args)...);
      }
      return impl_.begin_ + pos_i;
   }

   //--------------
   //    POP
   //--------------
   void pop_back()
   {
      impl_.end_--;
      alloc_trait::destroy(impl_, impl_.end_);
   }
   void pop_front()
   {
      alloc_trait::destroy(impl_, impl_.begin_);
      impl_.begin_++;
   }

   //------------------------------------------------
   //    RESIZE
   //    resize_back: change size, back capacity
   //    resize_front: change size, front capacity
   //------------------------------------------------
   void resize_back(size_type n)
   {
      if(n <= size())
         impl_.destroy(impl_.begin_ + n, impl_.end_);
      else
      {
         reserve_back(n);
         for(pointer p = impl_.end_; p != impl_.begin_ + n; p++)
            alloc_trait::construct(impl_, p);
      }
      impl_.end_ = impl_.begin_ + n;
   }
   void resize_back(size_type n, const value_type& val)
   {
      if(n <= size())
         impl_.destroy(impl_.begin_ + n, impl_.end_);
      else
      {
         reserve_back(n);
         for(pointer p = impl_.end_; p != impl_.begin_ + n; p++)
            alloc_trait::construct(impl_, p, val);
      }
      impl_.end_ = impl_.begin_ + n;
   }
   void resize_front(size_type n)
   {
      if(n <= size())
         impl_.destroy(impl_.begin_, impl_.end_ - n);
      else
      {
         reserve_front(n);
         for(pointer p = impl_.end_ - n; p != impl_.begin_; p++)
            alloc_trait::construct(impl_, p);
      }
      impl_.begin_ = impl_.end_ - n;
   }
   void resize_front(size_type n, const value_type& val)
   {
      if(n <= size())
      {
         impl_.destroy(impl_.begin_, impl_.end_ - n);
      }
      else
      {
         reserve_front(n);
         for(pointer p = impl_.end_ - n; p != impl_.begin_; p++)
            alloc_trait::construct(impl_, p, val);
      }
      impl_.begin_ = impl_.end_ - n;
   }
   void swap(sda& other) noexcept
   {
      swap(impl_, other.impl_);
   }
};


#endif
