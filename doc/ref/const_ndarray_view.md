### acons::const_ndarray_view

```c++
template <typename T, size_t M, typename Order=row_major, typename Base=zero_based, typename TPtr=const T*>
class const_ndarray_view;  
```
The `const_ndarray_view` class represents a const view on an N-dimensional array.

#### Header
```c++
#include <acons/ndarray.hpp>
```

#### Template parameters

Member type                         |Definition|Notes
------------------------------------|----------|--------------------
`T`|The type of the elements.|
`M`|The number of dimensions of the array view|
`Order`|`row_major` or `column_major`|
`Base`|`zero_based` or `one_based`|
`TPtr`|

#### Member types

Member type                         |Definition
------------------------------------|------------------------------
`ndim`|The number of dimensions of the array view
`value_type`|T
`const_reference`|const T&
`order_type`|T
`base_type`|T
`const_iterator`|

#### Member functions

    const_ndarray_view(); // (1)

    template <typename Allocator>
    const_ndarray_view(ndarray<T, M, Order, Base, Allocator>& a); // (2)

    template<size_t m = M, size_t N, typename Allocator>
    const_ndarray_view(ndarray<T, N, Order, Base, Allocator>& a, 
                       const slices_type& slices); // (3)

    template<size_t m = M, size_t N, size_t K, typename Allocator>
    const_ndarray_view(ndarray<T, N, Order, Base, Allocator>& a, 
                       const std::array<size_t,K>& origin); // (4)

    template<size_t m = M, size_t N, size_t K, typename Allocator>
    const_ndarray_view(ndarray<T, N, Order, Base, Allocator>& a, 
                       const std::array<size_t,K>& origin,
                       const slices_type& slices); // (5)

    template<typename OtherTPtr>
    const_ndarray_view(const_ndarray_view<T, M, Order, Base, OtherTPtr>& other); // (6)

    template<size_t m = M, size_t N, typename OtherTPtr>
    const_ndarray_view(const_ndarray_view<T, N, Order, Base, OtherTPtr>& other, 
                       const slices_type& slices); // (7)

    template<size_t m = M, size_t N, size_t K, typename OtherTPtr>
    const_ndarray_view(const_ndarray_view<T, N, Order, Base, OtherTPtr>& other, 
                       const std::array<size_t,K>& origin); // (8)

    template<size_t m = M, size_t N, size_t K, typename OtherTPtr>
    const_ndarray_view(const_ndarray_view<T, N, Order, Base, OtherTPtr>& other, 
                       const std::array<size_t,K>& origin,
                       const slices_type& slices); // (9)

    template<typename OtherTPtr>
    const_ndarray_view(OtherTPtr data, const std::array<size_t,M>& dim,
                       typename std::enable_if<std::is_convertible<OtherTPtr,TPtr>::value>::type* = 0); // (10) 

Constructs a new M-dimensional array view.

(1) Default constructor. Constructs an empty M-dimensional array view.

    const_ndarray_view& operator=(const const_ndarray_view& other);

    const_ndarray_view& operator=(const_ndarray_view&& other);

    const_ndarray_view& operator=(std::initializer_list<array_item<T>> list);

##### Capacity

    bool empty() const noexcept;

    size_t num_elements() const noexcept;

##### Element access

    size_t size(size_t i) const;

    template <typename... Indices>
    T& operator()(size_t index, Indices... indices); 

    template <typename... Indices>
    const T& operator()(size_t index, Indices... indices) const;

    T& operator()(const std::array<size_t,M>& indices); 

    const T& operator()(const std::array<size_t,M>& indices) const; 

##### Iterators

    iterator begin();
    const_iterator begin() const;
    const_iterator cbegin() const;

    iterator end();
    const_iterator end() const;
    const_iterator cend() const;

##### Modifiers

    void swap(ndarray<T,M,Order,Base,Allocator>& other) noexcept
Swaps the contents of the two M-dimensional array views

#### See also

### Examples
  

