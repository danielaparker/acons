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

A `slice` object can be constructed with `start`, `stop` and `stride` parameters.
```c++
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

#### Indexing and slicing a 2-dimensional array

A slice object constructed with no parameters selects all the elements along the dimension of an array. 
```c++
int main()
{
    // Construct a 2-dimensional 3 x 3 array 
    ndarray<double,2> a = {{1,2,3},{4,5,6},{7,8,9}};

    // All items from the second row
    ndarray_view<double,1> v1(a, {1});
    std::cout << "(1) " << v1 << "\n\n";

    // All items from the second column
    ndarray_view<double,1> v2(a, {slice()}, {1});
    std::cout << "(2) " << v2 << "\n\n";

    // All items from column 1 onwards
    ndarray_view<double,2> v3(a, {slice(),slice(1)});
    std::cout << "(3) " << v3 << "\n\n";
}
```
Output:
```
(1) [4,5,6]

(2) [2,5,8]

(3) [[2,3],[5,6],[8,9]]
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

using namespace acons;

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

