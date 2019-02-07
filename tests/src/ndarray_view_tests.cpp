#include <catch/catch.hpp>
#include <iostream>
#include "acons/ndarray.hpp"
#include <iostream>

using namespace acons;

TEST_CASE("1-dim ndarray_view constructor tests")
{
    typedef ndarray<double,1> an_array;
    typedef an_array::view<1>::type a_view;
    typedef an_array::const_view<1>::type a_const_view;

    an_array a = {0,1,2,3,4,5,6,7};

    SECTION("test 1")
    {
        a_const_view v(a, {slice(1,7)});
        REQUIRE(v.size(0) == 6); 
        CHECK(v(0) == 1);
        CHECK(v(1) == 2);
        CHECK(v(2) == 3);
        CHECK(v(3) == 4);
        CHECK(v(4) == 5);
        CHECK(v(5) == 6);

        a_const_view u(v, {slice(1,3)});
    }

    SECTION("test 2")
    {
        a_view v(a, {slice(1,7)});
        REQUIRE(v.size(0) == 6); 
        CHECK(v(0) == 1);
        CHECK(v(1) == 2);
        CHECK(v(2) == 3);
        CHECK(v(3) == 4);
        CHECK(v(4) == 5);
        CHECK(v(5) == 6);

        a_const_view u(v, {slice(1,3)});
        a_view w(v, {slice(1,3)});

    }
}

TEST_CASE("1-dim row_major ndarray ndarray_view size tests")
{
    typedef ndarray<double,1> an_array;
    an_array a = { 0,1,2,3 };

    SECTION("size tests 3")
    {
        an_array::const_view<1>::type v(a, {slice(1,3)});

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
    SECTION("size tests")
    {
        an_array::view<1>::type v(a, {slice(1,3)});

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

TEST_CASE("3-dim 2x3x4 ndarray ndarray_view tests")
{
    typedef ndarray<double,3,row_major> an_array;
    an_array a = { {{0,1,2,3},{4,5,6,7},{8,9,10,11}}, {{12,13,14,15},{16,17,18,19},{20,21,22,23}} };
    REQUIRE(a.size(0) == 2);
    REQUIRE(a.size(1) == 3);
    REQUIRE(a.size(2) == 4);

    SECTION("size tests")
    {
        an_array::const_view<3>::type v3(a,{slice(1,2),slice(1,3),slice(0,4,2)});

        REQUIRE(v3.size(0) == 1); 
        REQUIRE(v3.size(1) == 2); 
        REQUIRE(v3.size(2) == 2); 

        CHECK(v3(0,0,0) == 16);
        CHECK(v3(0,0,1) == 18);
        CHECK(v3(0,1,0) == 20);
        CHECK(v3(0,1,1) == 22);

        //std::cout << "Here\n";
        an_array::const_view<2>::type v2(v3,{0});
        //std::cout << "After\n";
        REQUIRE(v2.size(0) == 2);
        REQUIRE(v2.size(1) == 2);
        CHECK(v2(0,0) == 16);
        //CHECK(v2(0,1) == 18);
        //CHECK(v2(1,0) == 20);
        //CHECK(v2(1,1) == 22);
    }

    SECTION("2-dim ndarray_view")
    {
        an_array::view<2>::type v(a, {0}, {slice(1, 3), slice(0, 4, 2)});

        REQUIRE(v.size(0) == 2); 
        REQUIRE(v.size(1) == 2); 
        CHECK(v(0,0) == 4.0);
        CHECK(v(1,0) == 8.0);
        CHECK(v(0,1) == 6.0);
        CHECK(v(1,1) == 10.0);
    }
}

