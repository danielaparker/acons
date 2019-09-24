### acons::const_ndarray_view

```c++
template<
    typename T, 
    size_t M, 
    typename Order = row_major, 
    typename Base = zero_based
> class const_ndarray_view;
```
The `const_ndarray_view` class represents a const view on an N-dimensional array.

#### Header
```c++
#include <acons/ndarray.hpp>
```

#### Base class

[ndarray_view_base](ndarray_view_base.md)

#### Template parameters

Member type                         |Definition|Notes
------------------------------------|----------|--------------------
`T`|The type of the elements.|
`M`|The number of dimensions of the array view|
`Order`|`row_major` or `column_major`|
`Base`|`zero_based` or `one_based`|
`TPtr`

#### Member types

Member type                         |Definition
------------------------------------|------------------------------
`ndim`|The number of dimensions of the array view
`value_type`|If `ndim` == 1, then this is `T`, otherwise it is `ndarray_view_base<T,N-1,Order,Base,TPtr>`
`const_reference`|const T&
`order_type`|Order
`base_type`|Base
`const_iterator`|
`iterator`|

#### Member functions

    template <typename Allocator>
    const_ndarray_view(const ndarray<T, M, Order, Base, Allocator>& a); // (1)

    template<typename OtherTPtr>
    const_ndarray_view(const ndarray_view_base<T, M, Order, Base, OtherTPtr>& other); // (2)

    const_ndarray_view(const_ndarray_view<T, M, Order, Base>&& a); // (3)

    template<typename Allocator>
    const_ndarray_view(const ndarray<T, M, Order, Base, Allocator>& a, 
                       const std::array<slice,M>& slices); // (4)

    template<size_t N, typename Allocator>
    const_ndarray_view(const ndarray<T, N, Order, Base, Allocator>& a, 
                       const indices_t<N-M>& first_dim); // (5)

    template<typename OtherTPtr>
    const_ndarray_view(const ndarray_view_base<T, M, Order, Base, OtherTPtr>& other, 
                       const std::array<slice,M>& slices); // (6)

    template<size_t m = M, size_t N, typename OtherTPtr>
    const_ndarray_view(const ndarray_view_base<T, N, Order, Base, OtherTPtr>& other, 
                       const indices_t<N-m>& first_dim); // (7)

    template<size_t N, typename Allocator>
    const_ndarray_view(const ndarray<T, N, Order, Base, Allocator>& other, 
                       const indices_t<N-M>& first_dim,
                       const std::array<slice,M>& slices); // (8)

    template<size_t N, typename OtherTPtr>
    const_ndarray_view(const ndarray_view_base<T, N, Order, Base, OtherTPtr>& other, 
                       const indices_t<N-M>& first_dim,
                       const std::array<slice,M>& slices); // (9)

    template<size_t N, typename Allocator>
    const_ndarray_view(const ndarray<T, N, Order, Base, Allocator>& other,
                       const std::array<slice,M>& slices, 
                       const indices_t<N-M>& last_dim); // (10)

    template<size_t N, typename OtherTPtr>
    const_ndarray_view(const ndarray_view_base<T, N, Order, Base, OtherTPtr>& other,
                       const std::array<slice,M>& slices, 
                       const indices_t<N-M>& last_dim); // (11)

    const_ndarray_view(const T* data, const extents_t<M>& shape); // (12) 

    template <typename... Args>
    const_ndarray_view(const T* data, size_t i, Args... args); // (13) 

    template <typename Allocator>
    const_ndarray_view& operator=(const ndarray<T, M, Order, Base, Allocator>& a); // (14)

    template<typename OtherTPtr>
    const_ndarray_view& operator=(const ndarray_view_base<T, M, Order, Base, OtherTPtr>& other); // (15)

Constructs a new M-dimensional array view.

    const_ndarray_view& operator=(const const_ndarray_view& other);

    const_ndarray_view& operator=(const_ndarray_view&& other);

    const_ndarray_view& operator=(std::initializer_list<array_item<T>> list);

##### Capacity

    bool empty() const noexcept;

    size_t size() const noexcept;

##### Element access

    const T* base_data() const;
    Base data of array that owns the data 

    size_t base_size() const;
    Number of elements in the array that owns the data

    T* data();
    const T* data() const;
    Returns the address of the first element in the view

    size_t size() const;
    Number of elements in the view, equivalent to the product of the view's dimensions.

    std::array<size_t,M> shape() const;
    Returns an array of M items specifying the size of each dimension.

    size_t shape(size_t i) const;
    The size of the ith dimension, equivalent to `shape()[i]`.

    size_t size(size_t i) const;

    template <typename... Indices>
    T& operator()(size_t index, Indices... indices); 

    template <typename... Indices>
    const T& operator()(size_t index, Indices... indices) const;

    T& operator()(const std::array<size_t,M>& indices); 

    const T& operator()(const std::array<size_t,M>& indices) const; 

##### Iterators
    const_iterator begin() const;
    const_iterator cbegin() const;
Returns an iterator to the first element in the view. The iteration order is in memory order (`row_major` or `column_major`)
to allow elements to be accessed with high locality.

    const_iterator end() const;
    const_iterator cend() const;

  

