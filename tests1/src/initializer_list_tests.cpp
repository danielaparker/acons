#include <catch/catch.hpp>
#include <iostream>
#include "acons/ndarray.hpp"
#include <sstream>

using namespace acons;

TEST_CASE("bad initializer list 1")
{
    REQUIRE_THROWS([&](){
        ndarray<double, 3> a= {{{0,1},{3,4,5}}};
    }());
}

TEST_CASE("bad initializer list 2")
{
    REQUIRE_THROWS([&](){
        ndarray<double, 3> a= {{0,1,2},{3,4,5}};
    }());
}

TEST_CASE("2x2x2 initializer list")
{
    ndarray<double, 3, row_major> a = { {{0,1},{2,3}}, {{4,5},{6,7}} };

    CHECK(a.size(0) == 2);
    CHECK(a.size(1) == 2);
    CHECK(a.size(2) == 2);

    CHECK(a(0,0,0) == 0);
    CHECK(a(0,0,1) == 1);
    CHECK(a(0,1,0) == 2);
    CHECK(a(0,1,1) == 3);

    CHECK(a(1,0,0) == 4);
    CHECK(a(1,0,1) == 5);
    CHECK(a(1,1,0) == 6);
    CHECK(a(1,1,1) == 7);
}

