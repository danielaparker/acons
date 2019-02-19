# acons

acons is a modern C++ header-only library for constructing N-dimensional arrays.

It is distributed under the [MIT License](https://opensource.org/licenses/MIT).

## Get acons

Download the latest [single header file](https://raw.githubusercontent.com/danielaparker/acons/master/include/acons/ndarray.hpp). 

## How to use it

[Examples](#ExamplesLabel)  
[Reference](doc/ref/index.md)

## Supported compilers

 Compiler     | Versions      | Operating System
--------------|---------------|-----------------
 VS           | 14.0          | Windows 10       
 g++-         | 4.8, 6, 7,8   | Ubuntu           
 clang        | 3.8, 5.0. 6.0 |

<div id="ExamplesLabel"/>

### Examples

#### Create a 3-dim array

```c++
#include <iostream>
#include <acons/ndarray.hpp>

using namespace acons; // For convenience

int main()
{
    // Create a 3-dim array of shape 3 x 4 x 2
    acons::ndarray<double,3> a(3,4,2);

    // Assign values to the elements
    int x = 0;
    for (size_t i = 0; i < a.size(0); ++i)
    {
        for (size_t j = 0; j < a.size(1); ++j)
        {
            for (size_t k = 0; k < a.size(2); ++k)
            {
                a(i,j,k) = x++;
            }
        }
    }

    // Print
    std::cout << a << "\n";
}
```
Output:
```
[[[0,1],[2,3],[4,5],[6,7]],[[8,9],[10,11],[12,13],[14,15]],[[16,17],[18,19],[20,21],[22,23]]]
```

#### Slicing a 1-dimensional array

A `slice` object can be constructed with `start`, `stop` and `step` parameters.
```c++
#include <iostream>
#include <acons/ndarray.hpp>

using namespace acons; // For convenience

int main()
{
    ndarray<double,1> a = {0,1,2,3,4,5,6,7,8,9};

    // Extracting a part of the array with a slice object
    ndarray_view<double,1> v(a, {slice(2,7,2)});
    std::cout << v << "\n\n";
}
```
Output:
```
[2,4,6]
```

#### Slicing a 2-dimensional array

A slice object constructed with no parameters selects all the elements along the dimension of an array. 
```c++
#include <iostream>
#include <acons/ndarray.hpp>

using namespace acons; // For convenience

int main()
{
    // Construct a 2-dimensional 3 x 4 array 
    ndarray<double,2> a = {{0,1,2,3},{4,5,6,7},{8,9,10,11}};

    // All items from row 1 and columns 0 and 1
    ndarray_view<double,2> v1(a, {slice(1,2),slice(0,2)});
    std::cout << "(1) " << v1 << "\n\n";

    // All items from column 1 onwards
    ndarray_view<double,2> v2(a, {slice(),slice(1)});
    std::cout << "(2) " << v2 << "\n\n";
}
```
Output:
```
(1) [[4,5]]

(2) [[1,2,3],[5,6,7],[9,10,11]]
```

#### Reduction

```c++
#include <iostream>
#include <acons/ndarray.hpp>

using namespace acons; // For convenience

int main()
{
    // Construct a 2-dimensional 3 x 4 array 
    ndarray<double,2> a = {{0,1,2,3},{4,5,6,7},{8,9,10,11}};

    // Reduce the 2-dimensional array to a 1-dimensional array, along row 0
    ndarray_view<double,1> v1(a, {0});
    std::cout << "(1) " << v1 << "\n\n";

    // Reduce the 2-dimensional array to a 1-dimensional array, along column 2
    ndarray_view<double,1> v2(a, {slice()}, {2});
    std::cout << "(2) " << v2 << "\n\n";
}
```
Output:
```
(1) [0,1,2,3]

(2) [2,6,10]
```

#### Iterate over row major arrays and columns

Row major (C-style) arrays are traversed in memory order by rows.

```c++
#include <iostream>
#include <acons/ndarray.hpp>

using namespace acons; // For convenience

int main()
{
    typedef ndarray<double,2,row_major> array_t;

    // Construct a 2-dimensional 3 x 4 row-major array 
    array_t a = {{0,1,2,3},{4,5,6,7},{8,9,10,11}};
    std::cout << "(1) " << a << "\n\n";

    // All items from row 1 and columns 1 through 2
    array_t::view<2> v(a, {slice(1,2),slice(1,3)});
    std::cout << "(2) " << v << "\n\n";

    std::cout << "(3) ";
    for (auto it = a.begin(); it != a.end(); ++it)
    {
        if (it != a.begin())
        {
            std::cout << ",";
        }
        std::cout << *it;
    }
    std::cout << "\n\n";

    std::cout << "(4) ";
    for (auto it = v.begin(); it != v.end(); ++it)
    {
        if (it != v.begin())
        {
            std::cout << ",";
        }
        std::cout << *it;
    }
    std::cout << "\n\n"; 
}
```
Output:
```
(1) [[0,1,2,3],[4,5,6,7],[8,9,10,11]]

(2) [[5,6]]

(3) 0,1,2,3,4,5,6,7,8,9,10,11

(4) 5,6
```
#### Iterate over column major array and views

Column major (Fortran-style) arrays and views are traversed in memory order by columns.

```c++
#include <iostream>
#include <acons/ndarray.hpp>

using namespace acons; // For convenience

int main()
{
    typedef ndarray<double,2,column_major> array_t;

    // Construct a 2-dimensional 3 x 4 column-major array 
    array_t a = {{0,1,2,3},{4,5,6,7},{8,9,10,11}};
    std::cout << "(1) " << a << "\n\n";

    // All items from row 2 and columns 0 and 2
    array_t::view<2> v(a, {slice(2,3),slice(0,4,2)});
    std::cout << "(2) " << v << "\n\n";

    std::cout << "(3) ";
    for (auto it = a.begin(); it != a.end(); ++it)
    {
        if (it != a.begin())
        {
            std::cout << ",";
        }
        std::cout << *it;
    }
    std::cout << "\n\n";

    std::cout << "(4) ";
    for (auto it = v.begin(); it != v.end(); ++it)
    {
        if (it != v.begin())
        {
            std::cout << ",";
        }
        std::cout << *it;
    }
    std::cout << "\n\n";
}
```
Output:
```
(1) [[0,1,2,3],[4,5,6,7],[8,9,10,11]]

(2) [[8,10]]

(3) 0,4,8,1,5,9,2,6,10,3,7,11

(4) 8,10
```

#### Creating ndarrays in managed shared memory with Boost interprocess allocators

```c++
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <cstdlib> //std::system
#include <acons/ndarray.hpp>
#include <iostream>

using namespace acons; // For convenience

typedef boost::interprocess::allocator<double,
        boost::interprocess::managed_shared_memory::segment_manager> shmem_allocator;

typedef ndarray<double,2,row_major,zero_based,shmem_allocator> ndarray_shm;

int main(int argc, char *argv[])
{
    typedef std::pair<double, int> MyType;

    if(argc == 1) //Parent process
    {  
       //Remove shared memory on construction and destruction
       struct shm_remove
       {
          shm_remove() { boost::interprocess::shared_memory_object::remove("MySharedMemory"); }
          ~shm_remove(){ boost::interprocess::shared_memory_object::remove("MySharedMemory"); }
       } 
       remover;

       //Construct managed shared memory
       boost::interprocess::managed_shared_memory segment(boost::interprocess::create_only, 
                                                          "MySharedMemory", 65536);

       //Initialize shared memory STL-compatible allocator
       const shmem_allocator allocator(segment.get_segment_manager());

       // Create ndarray with all dynamic allocations in shared memory
       ndarray_shm* pA = segment.construct<ndarray_shm>("my ndarray")(allocator, 2, 2, 0.0);
       ndarray_shm& a = *pA;

       a(0,0) = 0;
       a(0,1) = 1;
       a(1,0) = 2;
       a(1,1) = 3;

       std::pair<ndarray_shm*, boost::interprocess::managed_shared_memory::size_type> res;
       res = segment.find<ndarray_shm>("my ndarray");

       std::cout << "Parent process:\n";
       std::cout << *(res.first) << "\n";

       //Launch child process
       std::string s(argv[0]); s += " child ";
       if(0 != std::system(s.c_str()))
          return 1;

       //Check child has destroyed all objects
       if(segment.find<MyType>("my ndarray").first)
          return 1;
    }
    else
    {
       //Open managed shared memory
       boost::interprocess::managed_shared_memory segment(boost::interprocess::open_only, 
                                                          "MySharedMemory");

       std::pair<ndarray_shm*, boost::interprocess::managed_shared_memory::size_type> res;
       res = segment.find<ndarray_shm>("my ndarray");

       if (res.first != nullptr)
       {
           std::cout << "\nChild process:\n";
           std::cout << *(res.first) << "\n";
       }
       else
       {
           std::cout << "Result is null\n";
       }

       //We're done, delete all the objects
       segment.destroy<ndarray_shm>("my ndarray");
    }
}
```
Output:
```
Parent process:
[[0,1],[2,3]]

Child process:
[[0,1],[2,3]]
```

