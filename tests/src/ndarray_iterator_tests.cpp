#include <catch/catch.hpp>
#include <iostream>
#include "acons/ndarray.hpp"
#include <sstream>

using namespace acons;

TEST_CASE("2-dim 3 x 4 ndarray iterator tests")
{
    ndarray<double,2> a = {{1.0,2.0,3.0,4.0},{5.0,6.0,7.0,8.0},{9.0,10.0,11.0,12.0}};
    ndarray_view<double,2> v(a);

    REQUIRE(v.size(0) == 3);
    REQUIRE(v.size(1) == 4);

    SECTION("row_major_iterator test")
    {
        row_major_iterator<double,2,row_major,zero_based,double*> it(v);
        row_major_iterator<double,2,row_major,zero_based,double*> end(v,true);

        CHECK(*it++ == 1.0);
        CHECK(*it++ == 2.0);
        CHECK(*it++ == 3.0);
        CHECK(*it++ == 4.0);
        CHECK(*it++ == 5.0);
        CHECK(*it++ == 6.0);
        CHECK(*it++ == 7.0);
        CHECK(*it++ == 8.0);
        CHECK(*it++ == 9.0);
        CHECK(*it++ == 10.0);
        CHECK(*it++ == 11.0);
        CHECK(*it++ == 12.0);
        CHECK(it == end);
    }
}

TEST_CASE("3-dim 2x3x4 ndarray iterator tests")
{
    typedef ndarray<double,3,row_major> an_array;
    an_array a = { {{0,1,2,3},{4,5,6,7},{8,9,10,11}}, {{12,13,14,15},{16,17,18,19},{20,21,22,23}} };
    ndarray_view<double,3> v(a);

    REQUIRE(v.size(0) == 2);
    REQUIRE(v.size(1) == 3);
    REQUIRE(v.size(2) == 4);

    SECTION("row_major_iterator test")
    {
        row_major_iterator<double,3,row_major,zero_based,double*> it(v);
        row_major_iterator<double,3,row_major,zero_based,double*> end(v,true);

        CHECK(*it++ == 0.0);
        CHECK(*it++ == 1.0);
        CHECK(*it++ == 2.0);
        CHECK(*it++ == 3.0);
        CHECK(*it++ == 4.0);
        CHECK(*it++ == 5.0);
        CHECK(*it++ == 6.0);
        CHECK(*it++ == 7.0);
        CHECK(*it++ == 8.0);
        CHECK(*it++ == 9.0);
        CHECK(*it++ == 10.0);
        CHECK(*it++ == 11.0);
        CHECK(*it++ == 12.0);
        CHECK(*it++ == 13.0);
        CHECK(*it++ == 14.0);
        CHECK(*it++ == 15.0);
        CHECK(*it++ == 16.0);
        CHECK(*it++ == 17.0);
        CHECK(*it++ == 18.0);
        CHECK(*it++ == 19.0);
        CHECK(*it++ == 20.0);
        CHECK(*it++ == 21.0);
        CHECK(*it++ == 22.0);
        CHECK(*it++ == 23.0);
        CHECK(it == end);
    }
}

