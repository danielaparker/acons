#include <catch/catch.hpp>
#include <iostream>
#include "acons/ndarray.hpp"
#include <sstream>

using namespace acons;

TEST_CASE("1 dim row major iterator test")
{
    ndarray<double, 1, row_major> a = { 0,1,2,3 };
    ndarray_view<double, 1, row_major> v(a, {{1,2}});
    auto it = v.begin();
    auto end = v.end();

    CHECK(*it++ == v(0));
    CHECK(*it++ == v(1));
    CHECK(it == end);
}

TEST_CASE("2x3x4 dim row major iterator test")
{
    // Construct a 3-dimensional array with dimensions 2 x 3 x 4
    ndarray<double, 3, row_major> a = { {{0,1,2,3},{4,5,6,7},{8,9,10,11}}, {{12,13,14,15},{16,17,18,19},{20,21,22,23}} };

    ndarray_view<double, 3> v(a,{{1,1},{1,2},{0,4,2}});
    auto it = v.begin();
    auto end = v.end();

    REQUIRE(v.size(0) == 1);
    REQUIRE(v.size(1) == 2);
    REQUIRE(v.size(2) == 2);
    CHECK(v(0,0,0) == 16);
    CHECK(v(0,0,1) == 18);
    CHECK(v(0,1,0) == 20);
    CHECK(v(0,1,1) == 22);

    CHECK(*it++ == 16);
    CHECK(*it++ == 18);
    CHECK(*it++ == 20);
    CHECK(*it++ == 22);
    CHECK(it == end);
}

TEST_CASE("2x3x4 dim column major iterator test")
{
    // Construct a 3-dimensional array with dimensions 2 x 3 x 4
    ndarray<double,3,column_major> a = { {{0,1,2,3},{4,5,6,7},{8,9,10,11}}, {{12,13,14,15},{16,17,18,19},{20,21,22,23}} };

    ndarray_view<double,3,column_major> v(a,{{1,1},{1,2},{0,4,2}});

    REQUIRE(v.size(0) == 1);
    REQUIRE(v.size(1) == 2);
    REQUIRE(v.size(2) == 2);
    CHECK(v(0,0,0) == 16);
    CHECK(v(0,0,1) == 18);
    CHECK(v(0,1,0) == 20);
    CHECK(v(0,1,1) == 22);

    auto it = v.begin();
    auto end = v.end();
    CHECK(*it++ == 16);
    CHECK(*it++ == 20);
    CHECK(*it++ == 18);
    CHECK(*it++ == 22);
    CHECK(it == end);
}

