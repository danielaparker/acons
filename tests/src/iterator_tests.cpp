#include <catch/catch.hpp>
#include <iostream>
#include "acons/ndarray.hpp"
#include <sstream>

using namespace acons;

TEST_CASE("1-dim row_major ndarray iterator tests")
{
    ndarray<double,1> a = {0,1,2,3,4,5,6,7};
    ndarray_view<double,1> v(a);

    REQUIRE(v.shape(0) == 8);

    SECTION("test 1")
    {
        auto it = make_row_major_iterator(v);
        auto last = acons::end(it);

        CHECK(*it++ == 0.0);
        CHECK(*it++ == 1.0);
        CHECK(*it++ == 2.0);
        CHECK(*it++ == 3.0);
        CHECK(*it++ == 4.0);
        CHECK(*it++ == 5.0);
        CHECK(*it++ == 6.0);
        CHECK(*it++ == 7.0);
        CHECK(it == last);
    }

    SECTION("test 2")
    {
        auto it = make_row_major_iterator(v);

        size_t i = 0;
        for (auto element : it)
        {
            CHECK(element == a(i++));
        }
        CHECK(i == a.shape(0));
    }
    
    SECTION("test 3")
    {
        column_major_iterator<double,1,double*> it = make_column_major_iterator(v);

        size_t i = 0;
        for (auto element : it)
        {
            CHECK(element == a(i++));
        }
        CHECK(i == a.shape(0));
    }
    
}
TEST_CASE("1-dim column_major ndarray iterator tests")
{
    ndarray<double,1,column_major> a = {0,1,2,3,4,5,6,7};
    ndarray_view<double,1,column_major> v(a);

    REQUIRE(v.shape(0) == 8);

    SECTION("test 1")
    {
        auto it = make_row_major_iterator(v);
        auto last = acons::end(it);

        CHECK(*it++ == 0.0);
        CHECK(*it++ == 1.0);
        CHECK(*it++ == 2.0);
        CHECK(*it++ == 3.0);
        CHECK(*it++ == 4.0);
        CHECK(*it++ == 5.0);
        CHECK(*it++ == 6.0);
        CHECK(*it++ == 7.0);
        CHECK(it == last);
    }
}

TEST_CASE("2-dim 3 x 4 ndarray iterator tests")
{
    ndarray<double,2> a = {{1.0,2.0,3.0,4.0},{5.0,6.0,7.0,8.0},{9.0,10.0,11.0,12.0}};
    ndarray_view<double,2> v(a);

    REQUIRE(v.shape(0) == 3);
    REQUIRE(v.shape(1) == 4);

    SECTION("row_major_iterator test")
    {
        auto it = make_row_major_iterator(v);
        auto last = end(it);

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
        CHECK(it == last);

    }
}

TEST_CASE("2-dim 3 x 3 row major iterator tests")
{
    typedef ndarray<double,2,row_major> array_t;

    // Construct a 2-dimensional 3 x 3 row-major array 
    array_t a = {{0,1,2},{4,5,6},{8,9,10}};
    //std::cout << "a: " << a << "\n\n";

    // All items from row 1 and columns 0 and 1
    array_t::view<2> v(a, {slice(1,2),slice(0,2)});
    //std::cout << "v: " << v << "\n\n";

    REQUIRE(v.shape(0) == 1);
    REQUIRE(v.shape(1) == 2);

    SECTION("test 1")
    {
        auto it = make_row_major_iterator(v);
        auto last = end(it);

        CHECK(*it++ == 4);
        CHECK(*it++ == 5);
        CHECK(it == last);
    }
}

TEST_CASE("2-dim 3 x 4 row major iterator tests")
{
    typedef ndarray<double,2,row_major> array_t;

    // Construct a 2-dimensional 3 x 3 row-major array 
    array_t a = {{0,1,2,3},{4,5,6,7},{8,9,10,11}};
    //std::cout << "a: " << a << "\n\n";

    // All items from row 1 and columns 0 and 1
    array_t::view<2> v(a, {slice(1,2),slice(1,3)});
    //std::cout << "v: " << v << "\n\n";

    REQUIRE(v.shape(0) == 1);
    REQUIRE(v.shape(1) == 2);

    SECTION("test 1")
    {
        auto it = make_row_major_iterator(v);
        auto last = end(it);

        CHECK(*it++ == 5);
        CHECK(*it++ == 6);
        CHECK(it == last);
    }
}

TEST_CASE("2-dim 3 x 3 column major iterator tests")
{
    typedef ndarray<double,2,column_major> array_t;

    // Construct a 2-dimensional 3 x 3 column-major array 
    array_t a = {{0,1,2},{4,5,6},{8,9,10}};
    std::cout << "a: " << a << "\n\n";

    // All items from row 1 and columns 0 and 1
    ndarray_view<double,2,column_major> v(a, {slice(1,2),slice(0,2)});
    std::cout << "v: " << v << "\n\n";

    REQUIRE(v.shape(0) == 1);
    REQUIRE(v.shape(1) == 2);
    CHECK(v(0,0) == 4);
    CHECK(v(0,1) == 5);

    SECTION("test 1")
    {
        auto it = make_column_major_iterator(v);

        auto last = end(it);
        CHECK(*it++ == 4);
        CHECK(*it++ == 5);
        CHECK(it == last);

    }
}

TEST_CASE("2-dim 3 x 4 column_major ndarray iterator tests")
{
    ndarray<double,2,column_major> a = {{1.0,2.0,3.0,4.0},{5.0,6.0,7.0,8.0},{9.0,10.0,11.0,12.0}};
    ndarray_view<double,2,column_major> v(a);

    REQUIRE(v.shape(0) == 3);
    REQUIRE(v.shape(1) == 4);

    SECTION("column_major_iterator test")
    {
        auto it = make_column_major_iterator(v);
        auto last = end(it);

        CHECK(*it++ == 1.0);
        CHECK(*it++ == 5.0);
        CHECK(*it++ == 9.0);
        CHECK(*it++ == 2.0);
        CHECK(*it++ == 6.0);
        CHECK(*it++ == 10.0);
        CHECK(*it++ == 3.0);
        CHECK(*it++ == 7.0);
        CHECK(*it++ == 11.0);
        CHECK(*it++ == 4.0);
        CHECK(*it++ == 8.0);
        CHECK(*it++ == 12.0);
        CHECK(it == last);
    }
}

TEST_CASE("3-dim 2x3x4 ndarray iterator tests")
{
    typedef ndarray<double,3,row_major> an_array;
    an_array a = { {{0,1,2,3},{4,5,6,7},{8,9,10,11}}, {{12,13,14,15},{16,17,18,19},{20,21,22,23}} };
    ndarray_view<double,3> v(a);

    REQUIRE(v.shape(0) == 2);
    REQUIRE(v.shape(1) == 3);
    REQUIRE(v.shape(2) == 4);

    SECTION("row_major_iterator test")
    {
        auto it = make_row_major_iterator(v);
        auto last = end(it);

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
        CHECK(it == last);
    }
}

