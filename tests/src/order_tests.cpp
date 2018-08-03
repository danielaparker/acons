#include <catch/catch.hpp>
#include "acons/ndarray.hpp"
#include <iostream>

using namespace acons;

TEST_CASE("row major tests")
{
    ndarray<double,2,row_major> a(2, 3);

    a(0, 0) = 0;
    a(0, 1) = 1;
    a(0, 2) = 2;
    a(1, 0) = 3;
    a(1, 1) = 4;
    a(1, 2) = 5;

    REQUIRE(a.size() == 6);
    CHECK(a.data()[0] == 0);
    CHECK(a.data()[1] == 1);
    CHECK(a.data()[2] == 2);
    CHECK(a.data()[3] == 3);
    CHECK(a.data()[4] == 4);
    CHECK(a.data()[5] == 5);

    subarray<double,1,row_major> v(a,{0,0},{3});
    CHECK(v(0) == 0);
    CHECK(v(1) == 1);
    CHECK(v(2) == 2);
}

TEST_CASE("column major tests")
{
    ndarray<double,2,column_major> a(2, 3);

    a(0, 0) = 0;
    a(0, 1) = 1;
    a(0, 2) = 2;
    a(1, 0) = 3;
    a(1, 1) = 4;
    a(1, 2) = 5;

    REQUIRE(a.size() == 6);
    CHECK(a.data()[0] == 0);
    CHECK(a.data()[1] == 3);
    CHECK(a.data()[2] == 1);
    CHECK(a.data()[3] == 4);
    CHECK(a.data()[4] == 2);
    CHECK(a.data()[5] == 5);

    subarray<double,1,column_major> v(a,{0,0},{3});
    CHECK(v(0) == 0);
    CHECK(v(1) == 1);
    CHECK(v(2) == 2);
}

