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
`N`|The number of dimensions of the array|
`Order`|`row_major` or `column_major`|
`Base`|`zero_based` or `one_based`|
`TPtr`|

#### Member types

Member type                         |Definition
------------------------------------|------------------------------
`value_type`|T
`reference`|T&
`const_reference`|const T&
`iterator`|T*
`const_iterator`|const T*

#### Member functions

    ndarray(); // (1)

    template <typename... Args>
    ndarray(size_t i, Args... args); // (2)

    explicit ndarray(const std::array<size_t,N>& dimensions); // (3)

    ndarray(const std::array<size_t,N>& dimensions, 
            const Allocator& alloc); // (4)

    ndarray(const std::array<size_t,N>& dimensions,
            T val); // (5)

    ndarray(const std::array<size_t,N>& dimensions, 
            T val,
            const Allocator& alloc); // (6)

    ndarray(std::initializer_list<array_item<T>> list); // (7)

    ndarray(std::initializer_list<array_item<T>> list, 
            const Allocator& alloc); // (8)

    ndarray(const ndarray& a); // (9)

    ndarray(const ndarray& a, 
            const Allocator& alloc); // (10)

    ndarray(ndarray&& a); // (11)

    ndarray(ndarray&& a, 
            const Allocator& alloc); // (12)

Constructs a new N-dimensional array, optionally using a user supplied allocator.

(1) Default constructor. Constructs an empty N-dimensional array.

(2) The first N arguments specify the dimensions of the N-dimensional array, 
and an optional (N+1)th argument specifies an initial value.

(3) Constructs an N-dimensional array with dimensions specified by `dimensions`.

(4) Constructs an N-dimensional array with dimensions specified by `dimensions`
    using the supplied allocator.

(5) Constructs an N-dimensional array with dimensions specified by `dimensions`
    and values initialized to `val`.

(6) Constructs an N-dimensional array with dimensions specified by `dimensions`
    and values initialized to `val` using the supplied allocator.

(7) Constructs an N-dimensional array from the initializer list init.

(8) Constructs an N-dimensional array from the initializer list init
    using the supplied allocator.

Exceptions:

(1) - (8) Calls to `Allocator::allocate` may throw.
(7) - (8) Throws `std::invalid_argument` if the initializer list contains non-comforming shapes.

    ndarray& operator=(const ndarray& other);

    ndarray& operator=(ndarray&& other);

    ndarray& operator=(std::initializer_list<array_item<T>> list);

##### Capacity

    bool empty() const noexcept;

    size_t size() const noexcept;

##### Element access

    size_t size(size_t i) const;

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

    iterator end();
    const_iterator end() const;
    const_iterator cend() const;

##### Modifiers

    void swap(ndarray<T,N,Order,Base,Allocator>& other) noexcept
Swaps the contents of the two N-dimensional arrays

#### See also

### Examples
  

