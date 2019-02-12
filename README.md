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

#### A slice object can be constructed with start, stop and stride parameters
```c++
int main()
{
    ndarray<double,1> a = {0,1,2,3,4,5,6,7,8,9};

    ndarray_view<double, 1> v(a, {slice(2,7,2)});
    std::cout << v << "\n\n";
}
```
Output:
```
[2,4,6]
```

#### A slice object constructed with no parameters selects all the elements along the dimension of an array. 
```c++
int main()
{
    // Construct a 2-dimensional 3 x 3 array 
    ndarray<double,2> a = {{1,2,3},{4,5,6},{7,8,9}};

    std::cout << "All items from the second row\n";
    ndarray_view<double,1> v1(a, {1});
    std::cout << v1 << "\n\n";

    std::cout << "All items from the second column\n";
    ndarray_view<double,1> v2(a, {slice()}, {1});
    std::cout << v2 << "\n\n";

    std::cout << "All items from column 1 onwards\n";
    ndarray_view<double,2> v3(a, {slice(),slice(1)});
    std::cout << v3 << "\n\n";
}
```
Output:
```
The (1,2) element
6
 
All items from the second row
[4,5,6]

All items from the second column
[2,5,8]

All items from column 1 onwards
[[2,3],[5,6],[8,9]]
```


