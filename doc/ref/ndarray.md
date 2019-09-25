### acons::ndarray

```c++
template<
    typename T, 
    size_t N, 
    typename Order = row_major, 
    typename Base = zero_based, 
    typename Allocator = std::allocator<T>
> class ndarray;
```
The `ndarray` class represents an N-dimensional array.

#### Header
```c++
#include <acons/ndarray.hpp>
```

#### Template parameters

Member type                         |Definition|Notes
------------------------------------|----------|--------------------
`T`|The type of the elements.|
`N`|The number of dimensions of the array|
`Order`|`row_major` or `column_major` storage order|`row_major` is C storage order and `column_major` is FORTRAN storage order 
`Base`|`zero_based` or `one_based`|
`Allocator`||`Allocator::value_type` must be the same as `T`

#### Member types

Member type                         |Definition
------------------------------------|------------------------------
`ndim`|The number of dimensions of the array
`allocator_type`|Allocator
`element_type`|`T`
`pointer`|std::allocator_traits<allocator_type>::pointer
`value_type`|If `ndim` == 1, then this is `T`, otherwise it is `ndarray_view<T,N-1,Order,Base>`
`reference`|T&
`const_reference`|const T&
`order_type`|Order
`base_type`|Base
`iterator`|T*
`const_iterator`|const T*

#### Member functions

    ndarray(); // (1)

    template <typename... Args>
    ndarray(size_t i, Args... args); // (2)

    template <typename... Args>
    ndarray(std::allocator_arg_t, const Allocator& alloc, size_t i, Args... args); // (3)

    explicit ndarray(const extents_t<N>& shape); // (4)

    ndarray(std::allocator_arg_t, const Allocator& alloc, const extents_t<N>& shape); // (5)

    ndarray(const extents_t<N>& shape, T val); // (6)

    ndarray(std::allocator_arg_t, const Allocator& alloc, const extents_t<N>& shape, T val); // (7)

    ndarray(std::initializer_list<array_item<T>> list); // (8) 

    ndarray(std::allocator_arg_t, const Allocator& alloc, std::initializer_list<array_item<T>> list); // (9) 

    ndarray(const ndarray& other); // (10)

    ndarray(std::allocator_arg_t, const Allocator& alloc, const ndarray& other); // (11)

    ndarray(ndarray&& other); // (12)

    ndarray(std::allocator_arg_t, const Allocator& alloc, ndarray&& other); // (13)

    template <typename TPtr>
    ndarray(const ndarray_view_base<T,N,Order,Base,TPtr>& av); // (14)

    template <typename TPtr>
    ndarray(std::allocator_arg_t, const Allocator& alloc, const ndarray_view_base<T,N,Order,Base,TPtr>& av); // (15)

Constructs a new N-dimensional array, optionally using a user supplied allocator.

(1) Default constructor. Constructs an empty N-dimensional array.

(2) The first N arguments specify the shape of the N-dimensional array, 
an optional (N+1)th argument specifies an initial value.

(3) The first argument specifies an allocator, the next N arguments specify the shape of the N-dimensional array, 
and an optional last argument specifies an initial value.

(4) Constructs an N-dimensional array with shape specified by `shape`
    and values initialized to `T()`.

(5) Constructs an N-dimensional array with shape specified by `shape`
    and values initialized to `T()` and using the supplied allocator.

(6) Constructs an N-dimensional array with shape specified by `shape`
    and values initialized to `val`.

(7) Constructs an N-dimensional array with shape specified by `shape`
    and values initialized to `val` and using the supplied allocator.

(8) Constructs an N-dimensional array from the initializer list init.

(9) Constructs an N-dimensional array from the initializer list init
    using the supplied allocator.

Exceptions:

(1) - (8) Calls to `Allocator::allocate` may throw.
(7) - (8) Throws `std::invalid_argument` if the initializer list contains non-comforming shapes.

    ndarray& operator=(const ndarray& other);

    ndarray& operator=(ndarray&& other);

    ndarray& operator=(std::initializer_list<array_item<T>> list);

    ~ndarray();

##### Capacity

    bool empty() const noexcept;

    size_t size() const noexcept;

##### Element access

    T* data();
    const T* data() const;

    size_t size() const;
    Number of elements in the array, equivalent to the product of the array's dimensions.

    std::array<size_t,N> shape() const;
    Returns an array of N items specifying the size of each dimension.

    size_t shape(size_t i) const;
    The size of the ith dimension, equivalent to `shape()[i]`.

    template <typename... Indices>
    T& operator()(size_t index, Indices... indices); 

    template <typename... Indices>
    const T& operator()(size_t index, Indices... indices) const;

    T& operator()(const std::array<size_t,N>& indices); 

    const T& operator()(const std::array<size_t,N>& indices) const; 

    template <size_t n=N, size_t K>
    typename std::enable_if<(K < n),ndarray_view<T,N-K,Order,Base>>::type 
    subarray(const std::array<size_t,K>& origin);

    template <size_t n=N, size_t K>
    typename std::enable_if<(K < n),const_ndarray_view<T,N-K,Order,Base>>::type 
    subarray(const std::array<size_t,K>& origin) const 

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

    void swap(ndarray& other) noexcept
Swaps the contents of the two N-dimensional arrays

    void resize(const std::array<size_t,N>& dim, T value = T());
Resizes the shape of the array. If the array size is shrunk, 
the array is flattened in the order that the elements are stored in memory.
If the array size is expanded, storage is reallocated if required, and
additional elements are filled with `value`.

#### See also

### Examples
  
#### resize examples

```c++
// Construct a 2-dimensional 2 x 2 array 
ndarray<double,2> a = {{0, 1}, {2, 3}};

// Shrink to 2 x 1
a.resize({2,1});

std::cout << a << "\n\n";
```

Output:
```
[[0],[1]]
```

```c++
// Construct a 2-dimensional 2 x 2 array 
ndarray<double,2> a = {{0, 1}, {2, 3}};

// Enlarge to 2 x 3
a.resize({2,3});

std::cout << a << "\n\n";
```

Output:
```
[[0,1,2],[3,0,0]]
```

