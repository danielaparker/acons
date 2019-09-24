### acons::ndarray_view

```c++
template<typename T, 
    size_t M, 
    typename Order = row_major, 
    typename Base = zero_based
> class ndarray_view;
```
The `ndarray_view` class represents a view on an N-dimensional array.

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
`reference`|T&
`const_reference`|const T&
`order_type`|Order
`base_type`|Base
`iterator`|
`const_iterator`|

#### Member functions

    template <typename Allocator>
    ndarray_view(ndarray<T, M, Order, Base, Allocator>& a); // (1)

    ndarray_view(ndarray_view<T, M, Order, Base>& a); // (2)

    ndarray_view(ndarray_view<T, M, Order, Base>&& a); // (3)

    template<typename Allocator>
    ndarray_view(ndarray<T, M, Order, Base, Allocator>& a, 
                 const std::array<slice,M>& slices); // (4)

    ndarray_view(ndarray_view<T, M, Order, Base>& a, 
                 const std::array<slice,M>& slices); // (5)

    template<size_t N, typename Allocator>
    ndarray_view(ndarray<T, N, Order, Base, Allocator>& a, 
                 const indices_t<N-M>& first_dim); // (6)

    template<size_t N>
    ndarray_view(ndarray_view<T, N, Order, Base>& a, 
                 const indices_t<N-M>& first_dim); // (7)

    template<size_t N, typename Allocator>
    ndarray_view(ndarray<T, N, Order, Base, Allocator>& a, 
                 const indices_t<N-M>& first_dim,
                 const std::array<slice,M>& slices); // (8)

    template<size_t N>
    ndarray_view(ndarray_view<T, N, Order, Base>& a, 
                 const indices_t<N-M>& first_dim,
                 const std::array<slice,M>& slices); // (9)

    template<size_t N, typename Allocator>
    ndarray_view(ndarray<T, N, Order, Base, Allocator>& a,
                 const std::array<slice,M>& slices, 
                 const indices_t<N-M>& last_dim); // (10)

    template<size_t N>
    ndarray_view(ndarray_view<T, N, Order, Base>& a,
                 const std::array<slice,M>& slices, 
                 const indices_t<N-M>& last_dim); // (11)

    ndarray_view(T* data, const extents_t<M>& shape); // (12) 

    template <typename... Args>
    ndarray_view(T* data, size_t i, Args... args); // (13)

    template <typename Allocator>
    ndarray_view& operator=(ndarray<T, M, Order, Base, Allocator>& a); // (14)

    ndarray_view& operator=(ndarray_view<T, M, Order, Base>& a); // (15)

Constructs a new M-dimensional array view.

Exceptions:

    ndarray_view& operator=(const ndarray_view& other);

    ndarray_view& operator=(ndarray_view&& other);

    ndarray_view& operator=(std::initializer_list<array_item<T>> list);

##### Capacity

    bool empty() const noexcept;

    size_t size() const noexcept;

##### Element access

    T* base_data();
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

    template <typename... Indices>
    T& operator()(size_t index, Indices... indices); 

    template <typename... Indices>
    const T& operator()(size_t index, Indices... indices) const;

    T& operator()(const std::array<size_t,N>& indices); 

    const T& operator()(const std::array<size_t,N>& indices) const; 

##### Iterators

    iterator begin();
    const_iterator begin() const;
    const_iterator cbegin() const;
Returns an iterator to the first element in the view. The iteration order is in memory order (`row_major` or `column_major`)
to allow elements to be accessed with high locality.

    iterator end();
    const_iterator end() const;
    const_iterator cend() const;

##### Modifiers

    void swap(ndarray<T,M,Order,Base,Allocator>& other) noexcept
Swaps the contents of the two M-dimensional array views

#### See also

### Examples
  

