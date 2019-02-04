#include <catch/catch.hpp>
#include <iostream>
#include "acons/ndarray.hpp"
#include <iostream>

using namespace acons;

TEST_CASE("1-dim row_major ndarray ndarray_view size tests")
{
    typedef ndarray<double,1> an_array;
    an_array a = { 0,1,2,3 };

    SECTION("size tests")
    {
        an_array::view<1>::type v(a, {slice(1,3)});

        REQUIRE(v.size(0) == 2); 
        CHECK(v(0) == 1);
        CHECK(v(1) == 2);
    }
    SECTION("const_array_view<1>")
    {
        an_array::const_view<1>::type v(a.data()+1, {2});

        REQUIRE(v.size(0) == 2); 
        CHECK(v(0) == 1);
        CHECK(v(1) == 2);
    }
    SECTION("array_view<1>")
    {
        an_array::view<1>::type v(a.data()+1, {2});

        REQUIRE(v.size(0) == 2); 
        CHECK(v(0) == 1);
        CHECK(v(1) == 2);
    }
}

TEST_CASE("2-dim row_major ndarray ndarray_view size tests")
{
    typedef ndarray<double,2> an_array;
    an_array a = {{1.0,2.0,3.0,4.0},{5.0,6.0,7.0,8.0},{9.0,10.0,11.0,12.0}};

    SECTION("size tests")
    {
        an_array::view<1>::type v(a, {0}, {slice(2,4)});

        REQUIRE(v.size(0) == 2); 
        CHECK(v(0) == 3.0);
        CHECK(v(1) == 4.0);
    }

    SECTION("2-dim ndarray_view")
    {
        an_array::view<2>::type v(a, {slice(1,3), slice(0,4,2)});

        REQUIRE(v.size(0) == 2); 
        REQUIRE(v.size(1) == 2); 
        CHECK(v(0,0) == 5.0);
        CHECK(v(1,0) == 9.0);
        CHECK(v(0,1) == 7.0);
        CHECK(v(1,1) == 11.0);
    }
}

TEST_CASE("2-dim column_major ndarray ndarray_view size tests")
{
    typedef ndarray<double,2,column_major> an_array;
    an_array a = {{1.0,2.0,3.0,4.0},{5.0,6.0,7.0,8.0},{9.0,10.0,11.0,12.0}};

    SECTION("size tests")
    {
        an_array::view<1>::type v(a, {0}, {slice(2,4)});

        REQUIRE(v.size(0) == 2); 
        CHECK(v(0) == 3.0);
        CHECK(v(1) == 4.0);
    }

    SECTION("2-dim ndarray_view")
    {
        an_array::view<2>::type v(a, {slice(1,3), slice(0,4,2)});

        REQUIRE(v.size(0) == 2); 
        REQUIRE(v.size(1) == 2); 
        CHECK(v(0,0) == 5.0);
        CHECK(v(1,0) == 9.0);
        CHECK(v(0,1) == 7.0);
        CHECK(v(1,1) == 11.0);
    }
}
