#include <catch/catch.hpp>
#include <iostream>
#include "acons/ndarray.hpp"
#include <sstream>

using namespace acons;

TEST_CASE("1 dim row major iterator test")
{
    ndarray<double, 1, row_major> A = { 0,1,2,3 };
    ndarray_view<double, 1, row_major> v(A, {{1,2}});
    auto it = v.begin();
    auto end = v.end();

    while (it != end)
    {
        std::cout << *it++ << "\n";
    }

/*
    ndarray_view_iterator<double, 1, row_major> it(a.data());
    ndarray_view_iterator<double, 1, row_major> end(a.data() + a.size());

    while (it != end)
    {
        std::cout << *it << "\n";
        ++it;
    }
 */
}

TEST_CASE("2 dim row major iterator test")
{
    // Construct a 3-dimensional array with dimensions 2 x 3 x 4
    ndarray<double, 3> A(2,3,4);

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

    ndarray_view<double,3> v(A,{{0,2},{1,2},{0,4,2}});
    auto it = v.begin();
    auto end = v.end();

    while (it != end)
    {
        std::cout << *it++ << "\n";
    }
}

