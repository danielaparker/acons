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

