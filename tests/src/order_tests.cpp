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

    SECTION("v(a, indices_t<1>{0},{slice(1,3)})")
    {
        ndarray_view<double,1,row_major> v(a, indices_t<1>{0},{slice(1,3)});
        REQUIRE(v.shape(0) == 2);
        CHECK(v(0) == 1);
        CHECK(v(1) == 2);
    }

    SECTION("v(a,{slice(0,1),slice(1,2)})")
    {
        ndarray_view<double,2,row_major> v(a,{slice(0,1),slice(1,2)});
        REQUIRE(v.shape(0) == 1);
        REQUIRE(v.shape(1) == 1);
        CHECK(v(0,0) == 1);
    }

    SECTION("v(a,{slice(0,1),slice(1,3)})")
    {
        ndarray_view<double,2,row_major> v(a,{slice(0,1),slice(1,3)});
        REQUIRE(v.shape(0) == 1);
        REQUIRE(v.shape(1) == 2);
        CHECK(v(0,0) == 1);
        CHECK(v(0,1) == 2);
    }

    SECTION("v(a,{slice(0,1),slice(1,2)})")
    {
        ndarray_view<double,2,row_major> v(a,{slice(0,1),slice(1,2)});
        REQUIRE(v.shape(0) == 1);
        REQUIRE(v.shape(1) == 1);
        CHECK(v(0,0) == 1);
    }

    SECTION("v(a,{slice(1,2),slice(1,2)})")
    {
        ndarray_view<double,2,row_major> v(a,{slice(1,2),slice(1,2)});
        REQUIRE(v.shape(0) == 1);
        REQUIRE(v.shape(1) == 1);
        CHECK(v(0,0) == 4);
    }
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

    SECTION("v(a, indices_t<1>{0})")
    {
        ndarray_view<double,1,column_major> v(a, indices_t<1>{0});
        REQUIRE(v.shape(0) == 3);
        CHECK(v(0) == 0);
        CHECK(v(1) == 1);
        CHECK(v(2) == 2);
    }
    SECTION("v(a, indices_t<1>{1})")
    {
        ndarray_view<double, 1, column_major> v(a, indices_t<1>{1});
        REQUIRE(v.shape(0) == 3);
        CHECK(v(0) == 3);
        CHECK(v(1) == 4);
        CHECK(v(2) == 5);
    }

    SECTION("v(a, indices_t<1>{0},{slice(1,3)})")
    {
        ndarray_view<double,1,column_major> v(a, indices_t<1>{0},{slice(1,3)});
        
        REQUIRE(v.shape(0) == 2);
        //std::cout << "start\n"; 
        CHECK(v(0) == 1);
        //std::cout << "stop\n"; 
        CHECK(v(1) == 2);
    }

    SECTION("v(a,{slice(0,1),slice(1,2)})")
    {
        ndarray_view<double,2,column_major> v(a,{slice(0,1),slice(1,2)});
        REQUIRE(v.shape(0) == 1);
        REQUIRE(v.shape(1) == 1);
        //std::cout << "start\n"; 
        CHECK(v(0,0) == 1); 
        //std::cout << "stop\n"; 
    }

    SECTION("v(a,{slice(0,1),slice(1,3)})")
    {
        ndarray_view<double,2,column_major> v(a,{slice(0,1),slice(1,3)});
        REQUIRE(v.shape(0) == 1);
        REQUIRE(v.shape(1) == 2);
        CHECK(v(0,0) == 1);
        CHECK(v(0,1) == 2);
    }

    SECTION("v(a,{slice(0,1),slice(1,3)})")
    {
        ndarray_view<double,2,column_major> v(a,{slice(0,1),slice(1,3)});
        REQUIRE(v.shape(0) == 1);
        REQUIRE(v.shape(1) == 2);
        CHECK(v(0,0) == 1);
        CHECK(v(0,1) == 2);
    }

    SECTION("v(a,{slice(1,2),slice(1,3)})")
    {
        ndarray_view<double,2,column_major> v(a,{slice(1,2),slice(1,3)});
        REQUIRE(v.shape(0) == 1);
        REQUIRE(v.shape(1) == 2);
        CHECK(v(0,0) == 4);
        CHECK(v(0,1) == 5);
    }
}

