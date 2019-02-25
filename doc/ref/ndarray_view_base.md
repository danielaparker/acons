### acons::ndarray_view_base

```c++
template<typename T, 
    size_t M, 
    typename Order, 
    typename Base,
    typename TPtr
> class ndarray_view_base;
```
The `ndarray_view_base` class represents a view on an N-dimensional array.

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
`reference`|T&
`const_reference`|const T&
`order_type`|Order
`base_type`|Base
`iterator`|
`const_iterator`|

#### Member functions

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

    void swap(ndarray_view_base& other) noexcept
Swaps the contents of the two M-dimensional array views

#### Non-member functions

#### See also
  

