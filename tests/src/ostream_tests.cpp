#include <catch/catch.hpp>
#include "acons/ndarray.hpp"
#include <iostream>
#include <sstream>

using namespace acons;

TEST_CASE("get_offset test")
{
    ndarray<double,3,row_major> a = {{{1.0,2.0},{3.0,4.0}},{{5.0,6.0},{7.0,8.0}}};

    std::array<size_t,3> indices = {0,2,0};
    size_t offset1 = get_offset<3,zero_based>(a.strides(),indices);
    size_t offset2 = get_offset<3, zero_based, 0>(a.strides(),0,2,0);

    CHECK(offset1 == 4);
    CHECK(offset2 == 4);
}

TEST_CASE("ostream test")
{
    ndarray<double,1,row_major> a1 = {1.0,2.0,3.0,4.0};
    std::ostringstream os1;
    os1 << a1;
    CHECK(os1.str() == std::string("[1,2,3,4]"));

    ndarray<double,2,row_major> a2 = {{1.0,2.0,3.0,4.0},{5.0,6.0,7.0,8.0}};
    std::ostringstream os2;
    os2 << a2;
    CHECK(os2.str() == std::string("[[1,2,3,4],[5,6,7,8]]"));

    ndarray<double,3,row_major> a3 = {{{1.0,2.0},{3.0,4.0}},{{5.0,6.0},{7.0,8.0}}};
    CHECK(a3(0, 0, 0) == 1.0);
    CHECK(a3(0, 0, 1) == 2.0);
    CHECK(a3(0, 1, 0) == 3.0);
    CHECK(a3(0, 1, 1) == 4.0);
    CHECK(a3(1, 0, 0) == 5.0);
    CHECK(a3(1, 0, 1) == 6.0);
    CHECK(a3(1, 1, 0) == 7.0);
    CHECK(a3(1, 1, 1) == 8.0);

    std::ostringstream os3;
    os3 << a3;
    CHECK(os3.str() == std::string("[[[1,2],[3,4]],[[5,6],[7,8]]]"));
}

