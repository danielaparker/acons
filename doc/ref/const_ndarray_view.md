### acons::const_ndarray_view

```c++
template <typename T, size_t M, typename Order = row_major, typename Base = zero_based>
class const_ndarray_view;
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

#### Member types

Member type                         |Definition
------------------------------------|------------------------------
`ndim`|The number of dimensions of the array view
`value_type`|T
`const_reference`|const T&
`order_type`|Order
`base_type`|Base
`const_iterator`|
`iterator`|Same as `const_iterator`

#### Member functions

    template <typename Allocator>
    const_ndarray_view(const ndarray<T, M, Order, Base, Allocator>& a); // (1)

    template<typename Allocator>
    const_ndarray_view(const ndarray<T, M, Order, Base, Allocator>& a, 
                       const std::array<slice,M>& slices); // (2)

    template<size_t N, typename Allocator>
    const_ndarray_view(const ndarray<T, N, Order, Base, Allocator>& a, 
                       const std::array<size_t,N-M>& origin); // (3)

    template<typename OtherTPtr>
    const_ndarray_view(const ndarray_view_base<T, M, Order, Base, OtherTPtr>& other); // (4)

    template<typename OtherTPtr>
    const_ndarray_view(const ndarray_view_base<T, M, Order, Base, OtherTPtr>& other, 
                       const std::array<slice,M>& slices); // (5)

    template<size_t m = M, size_t N, typename OtherTPtr>
    const_ndarray_view(const ndarray_view_base<T, N, Order, Base, OtherTPtr>& other, 
                       const std::array<size_t,N-m>& origin); // (6)

    template<size_t N, typename Allocator>
    const_ndarray_view(const ndarray<T, N, Order, Base, Allocator>& other, 
                       const std::array<size_t,N-M>& origin,
                       const std::array<slice,M>& slices); // (7)

    template<size_t N, typename OtherTPtr>
    const_ndarray_view(const ndarray_view_base<T, N, Order, Base, OtherTPtr>& other, 
                       const std::array<size_t,N-M>& origin,
                       const std::array<slice,M>& slices); // (8)

    template<size_t N, typename Allocator>
    const_ndarray_view(const ndarray<T, N, Order, Base, Allocator>& other,
                       const std::array<slice,M>& slices, 
                       const std::array<size_t,N-M>& origin); // (9)

    template<size_t N, typename OtherTPtr>
    const_ndarray_view(const ndarray_view_base<T, N, Order, Base, OtherTPtr>& other,
                       const std::array<slice,M>& slices, 
                       const std::array<size_t,N-M>& origin); // (10)

    template<typename OtherTPtr>
    const_ndarray_view(OtherTPtr data, const std::array<size_t,M>& shape,
                       typename std::enable_if<std::is_convertible<OtherTPtr,const T*>::value>::type* = 0); // (11)

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

  

