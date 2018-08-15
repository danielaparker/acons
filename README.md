# acons

acons is a C++, header-only library for constructing N-dimensional arrays.

It is distributed under the [MIT License](https://opensource.org/licenses/MIT).

## Get acons

Download the latest [single header file](https://raw.githubusercontent.com/danielaparker/acons/master/include/acons/ndarray.hpp). 

## Examples

### Construct a 3-dimensional array

```c++
#include <acons/ndarray.hpp>
#include <iostream>

using namespace acons;

int main()
{
    // Construct a 3-dimensional array with dimensions 3 x 4 x 2
    ndarray<double, 3> A(3,4,2);

    // Assign values to the elements
    int init = 0;
    for (size_t i = 0; i < A.size(0); ++i)
    {
        for (size_t j = 0; j < A.size(1); ++j)
        {
            for (size_t k = 0; k < A.size(2); ++k)
            {
                A(i,j,k) = init++;
            }
        }
    }

    // Write to stdout
    std::cout << A << std::endl;
}
```
Output:
```
[[[0,1],[2,3],[4,5],[6,7]],[[8,9],[10,11],[12,13],[14,15]],[[16,17],[18,19],[20,21],[22,23]]]
```
