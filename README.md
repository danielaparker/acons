# acons

acons is a modern C++, header-only library for constructing N-dimensional arrays.

It is distributed under the [MIT License](https://opensource.org/licenses/MIT).

## Get acons

Download the latest [single header file](https://raw.githubusercontent.com/danielaparker/acons/master/include/acons/ndarray.hpp). 

## Supported compilers

Compiler      | Versions      |Operating System
--------------|---------------|-----------------
 VS           | 14.0          |Windows 10       
 g++-         | 4.8, 6, 7,8   |Ubuntu           
 clang        | 3.8, 5.0. 6.0 |

## Examples

### Row major, zero based indexing example

```c++
#include <acons/ndarray.hpp>
#include <iostream>

using namespace acons;

int main()
{
    // Construct a 3-dimensional 2 x 3 x 4 array 
    ndarray<double, 3> a = {{{0,1,2,3},{4,5,6,7},{8,9,10,11}}, 
                            {{12,13,14,15},{16,17,18,19},{20,21,22,23}}};

    std::cout << "(1)\n" << a << "\n\n";

    // Construct a 3-dimensional 1 x 2 x 2 view on the array.
    // The second argument provides a list of index ranges 
    // on the three dimensions of the array.
    ndarray_view<double,3> v(a, {{1,2},{1,3},{0,4,2}});

    std::cout << "(2)\n" << v << "\n\n";

    // Construct a 2-dimensional 3 x 4 slice from the array
    ndarray_view<double,2> s(a,{1});

    std::cout << "(3)\n" << s << "\n\n";

    // Iterate over the 2-dimensional view

    std::cout << "(4)\n";

    auto begin = s.begin();
    auto end = s.end();
    for (auto it = begin; it != end; ++it)
    {
        if (it != begin)
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
(1)
[[[0,1,2,3],[4,5,6,7],[8,9,10,11]],[[12,13,14,15],[16,17,18,19],[20,21,22,23]]]

(2)
[[[0,2],[4,6]]]

(3)
[[12,13,14,15],[16,17,18,19],[20,21,22,23]]

(4)
12,13,14,15,16,17,18,19,20,21,22,23
```

### Column major, one based indexing example

```c++
#include <acons/ndarray.hpp>
#include <iostream>

using namespace acons;

int main()
{
    // Construct a 3-dimensional 2 x 3 x 4 array 
    ndarray<double, 3, column_major, one_based> a = {{{0,1,2,3},{4,5,6,7},{8,9,10,11}}, 
                                                     {{12,13,14,15},{16,17,18,19},{20,21,22,23}}};

    std::cout << "(1)\n" << a << "\n\n";

    // Construct a 3-dimensional 1 x 2 x 2 view on the array.
    // The second argument provides a list of index ranges 
    // on the three dimensions of the array.
    ndarray_view<double, 3, column_major, one_based> v(a, {{2,3},{2,4},{1,5,2}});

    std::cout << "(2)\n" << v << "\n\n";

    // Construct a 2-dimensional 3 x 4 slice from the array
    ndarray_view<double, 2, column_major, one_based> s(a,{2});

    std::cout << "(3)\n" << s << "\n\n";

    // Iterate over the 2-dimensional view

    std::cout << "(4)\n";

    auto begin = s.begin();
    auto end = s.end();
    for (auto it = begin; it != end; ++it)
    {
        if (it != begin)
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
(1)
[[[0,1,2,3],[4,5,6,7],[8,9,10,11]],[[12,13,14,15],[16,17,18,19],[20,21,22,23]]]

(2)
[[[0,2],[4,6]]]

(3)
[[12,13,14,15],[16,17,18,19],[20,21,22,23]]

(4)
12,16,20,13,17,21,14,18,22,15,19,23
```

### Creating ndarrays in managed shared memory with Boost interprocess allocators

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
       ndarray_shm* pA = segment.construct<ndarray_shm>("my ndarray")(std::array<size_t,2>{2,2}, 0.0, allocator);
       ndarray_shm& A = *pA;

       A(0,0) = 0;
       A(0,1) = 1;
       A(1,0) = 2;
       A(1,1) = 3;

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

