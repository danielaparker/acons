#include <catch/catch.hpp>
#include <iostream>
#include "acons/ndarray.hpp"
#include <sstream>

using namespace acons;

TEST_CASE("get_offset test")
{
    ndarray<double,3,row_major> a = {{{1.0,2.0},{3.0,4.0}},{{5.0,6.0},{7.0,8.0}}};

    std::array<size_t,3> indices = {0,2,0};
    size_t offset1 = get_offset<3, zero_based>(a.strides(),indices);
    size_t offset2 = get_offset<3, zero_based, 0>(a.strides(),0,2,0);

    CHECK(offset1 == 4);
    CHECK(offset2 == 4);
}

TEST_CASE("zero based 1-dim array test")
{
    ndarray<double,1,row_major,zero_based> a = {1.0,2.0,3.0,4.0};

    CHECK(a(0) == 1.0);
    CHECK(a(1) == 2.0);
    CHECK(a(2) == 3.0);
    CHECK(a(3) == 4.0);

    ndarray_view<double,1,row_major,zero_based> v(a,{{0,2}});

    CHECK(v(0) == 1.0);
    CHECK(v(1) == 2.0);
}

TEST_CASE("one based 1-dim array test")
{
    ndarray<double,1,row_major,one_based> a = {1.0,2.0,3.0,4.0};

    CHECK(a(1) == 1.0);
    CHECK(a(2) == 2.0);
    CHECK(a(3) == 3.0);
    CHECK(a(4) == 4.0);

    ndarray_view<double,1,row_major,one_based> v(a,{{1,2}});

    CHECK(v(1) == 1.0);
    CHECK(v(2) == 2.0);
}


