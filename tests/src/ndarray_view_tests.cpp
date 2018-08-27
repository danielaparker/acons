#include <catch/catch.hpp>
#include <iostream>
#include "acons/ndarray.hpp"
#include <iostream>

using namespace acons;

// size tests

TEST_CASE("ndarray_view row major size tests")
{
    ndarray<double, 1, row_major> a = { 0,1,2,3 };
    ndarray_view<double, 1, row_major> v(a, {{1,3}});

    CHECK(v(0) == 1);
    CHECK(v(1) == 2);
}
