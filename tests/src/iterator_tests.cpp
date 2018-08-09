#include <catch/catch.hpp>
#include <iostream>
#include "acons/ndarray.hpp"
#include <sstream>

using namespace acons;

TEST_CASE("1 dim row major iterator test")
{
    ndarray<double, 1, row_major> a = { 0,1,2,3 };

    ndarray_view_iterator<double, 1, row_major> it(a.data());
    ndarray_view_iterator<double, 1, row_major> end(a.data() + a.size());

    while (it != end)
    {
        std::cout << *it << "\n";
        ++it;
    }
}

TEST_CASE("2 dim row major iterator test")
{
    ndarray<double, 2, row_major> a = { {0,1,2},{3,4,5} };

    ndarray_view<double, 2, row_major> v(a, {0,0}, {2,2});
    std::cout << v << "\n";
/*
    ndarray_view_iterator<double, 2, row_major> it(a.data());
    ndarray_view_iterator<double, 2, row_major> end(a.data() + a.size());

    while (it != end)
    {
        std::cout << *it << "\n";
        ++it;
    }
    */
}

