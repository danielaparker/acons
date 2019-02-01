#include <catch/catch.hpp>
#include <iostream>
#include "acons/ndarray.hpp"
#include <iostream>

using namespace acons;

TEST_CASE("row major stride tests")
{
    std::array<size_t, 2> dim = { 2,3 };

    size_t size = 0;
    std::array<size_t, 2> strides;
    row_major::calculate_strides(dim, strides, size);

    CHECK(size == 6);
    CHECK(strides[0] == 3);
    CHECK(strides[1] == 1);

    CHECK(0 == get_offset<2, zero_based, 0>(strides, 0, 0));
    CHECK(1 == get_offset<2, zero_based, 0>(strides, 0, 1));
    CHECK(2 == get_offset<2, zero_based, 0>(strides, 0, 2));
    CHECK(3 == get_offset<2, zero_based, 0>(strides, 1, 0));
    CHECK(4 == get_offset<2, zero_based, 0>(strides, 1, 1));
    CHECK(5 == get_offset<2, zero_based, 0>(strides, 1, 2));
}

TEST_CASE("column major stride tests")
{
    std::array<size_t,2> dim = {2,3};
    
    size_t size = 0;
    std::array<size_t,2> strides;
    column_major::calculate_strides(dim,strides,size);

    CHECK(size == 6);
    CHECK(strides[0] == 1);
    CHECK(strides[1] == 2);

    CHECK(0 == get_offset<2, zero_based, 0>(strides, 0, 0));
    CHECK(1 == get_offset<2, zero_based, 0>(strides, 1, 0));
    CHECK(2 == get_offset<2, zero_based, 0>(strides, 0, 1));
    CHECK(3 == get_offset<2, zero_based, 0>(strides, 1, 1));
    CHECK(4 == get_offset<2, zero_based, 0>(strides, 0, 2));
    CHECK(5 == get_offset<2, zero_based, 0>(strides, 1, 2));
}

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

    ndarray_view<double,1,row_major> v2(a,{0},{{1,2}});
    CHECK(v2(0) == 1);
    CHECK(v2(1) == 2);

    ndarray_view<double,2,row_major> v3(a,{{0,1},{1,2}});
    CHECK(v3(0,0) == 1);

    ndarray_view<double,2,row_major> v4(a,{{0,1},{1,2}});
    CHECK(v4(0,0) == 1);
    CHECK(v4(0,1) == 2);

    ndarray_view<double,2,row_major> v5(a,{{0,1},{1,2}});
    CHECK(v5(0,0) == 1);
    CHECK(v5(0,1) == 2);

    ndarray_view<double,2,row_major> v6(a,{{1,2},{1,2}});
    CHECK(v6(0,0) == 4);
    CHECK(v6(0,1) == 5);
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

    ndarray_view<double,1,column_major> v(a,{0});
    CHECK(v(0) == 0);
    CHECK(v(1) == 1);
    CHECK(v(2) == 2);

    ndarray_view<double, 1, column_major> v1(a, { 1 });
    CHECK(v1(0) == 3);
    CHECK(v1(1) == 4);
    CHECK(v1(2) == 5);

    ndarray_view<double,1,column_major> v2(a,{0},{{1,2}});
    CHECK(v2(0) == 1);
    CHECK(v2(1) == 2);

    ndarray_view<double,2,column_major> v3(a,{{0,1},{1,2}});
    std::cout << "size 0: " << v3.size(0) << ", size 1: " << v3.size(1) << "\n";
    CHECK(v3(0,0) == 1); 

    ndarray_view<double,2,column_major> v4(a,{{0,1},{1,2}});
    CHECK(v4(0,0) == 1);
    CHECK(v4(0,1) == 2);

    ndarray_view<double,2,column_major> v5(a,{{0,1},{1,2}});
    CHECK(v5(0,0) == 1);
    CHECK(v5(0,1) == 2);

    ndarray_view<double,2,column_major> v6(a,{{1,1},{1,2}});
    CHECK(v6(0,0) == 4);
    CHECK(v6(0,1) == 5);
}

