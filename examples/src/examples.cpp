#include <acons/ndarray.hpp>
#include <iostream>

using namespace acons;

void simple_example()
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

void array_view()
{
    // Construct a 3-dimensional array with dimensions 2 x 2 x 2
    ndarray<double, 3> a = { {{0,1},{2,3}}, {{4,5},{6,7}} };
    ndarray_view<double,1> v(a,{0,1,0},{2});


    // Write to stdout
    std::cout << a << std::endl;
    std::cout << v << std::endl;
}

int main()
{
    simple_example();
    array_view();
}
