#include <catch/catch.hpp>
#include "acons/ndarray.hpp"
#include <iostream>

using namespace acons;

TEST_CASE("2D Array 3")
{
    ndarray<size_t, 2> a(3, 2, 7);

    a(1,0) = 6;
    CHECK(a(0, 0) == 7);
    CHECK(a(1, 0) == 6);

}

TEST_CASE("2D Array 4")
{
    ndarray<size_t, 2> a(3, 2);

    a(0,0) = 6;
    CHECK(a(0, 0) == 6);

}


TEST_CASE("3D Array")
{
    ndarray<double,3> a(2, 3, 4, 1.0);
    CHECK(a.size(0) == 2);
    CHECK(a.size(1) == 3);
    CHECK(a.size(2) == 4);

    for (size_t i = 0; i < a.size(0); ++i)
    {
        for (size_t j = 0; j < a.size(1); ++j)
        {
            for (size_t k = 0; k < a.size(2); ++k)
            {
                CHECK(a(i,j,k) == 1.0);
            }
        }
    }
}

TEST_CASE("TestArrayInitializerList")
{
    ndarray<double,1> a = {1.0,2.0,3.0,4.0};
    CHECK(a.size(0) == 4);

}

TEST_CASE("TestArray2DInitializerList")
{
    ndarray<double,2> a = {{1.0,2.0,3.0,4.0},{5.0,6.0,7.0,8.0}};

    CHECK(a.size(0) == 2);
    CHECK(a.size(1) == 4);
}

TEST_CASE("Test Array View 1")
{
    ndarray<double,2> a = {{1.0,2.0,3.0,4.0},{5.0,6.0,7.0,8.0}};

    CHECK(a.size(0) == 2);
    CHECK(a.size(1) == 4);

    CHECK(a(1,1) == 6.0);

    ndarray_view<double,1> v(a,{{1,1},{1,2}});

    REQUIRE(v.size(0) == 2); 
    CHECK(v(0) == 6.0); 
    CHECK(v(1) == 7.0); 
}

TEST_CASE("Test ndarray_view 2")
{
    ndarray<double,2> a = {{1.0,2.0,3.0,4.0},{5.0,6.0,7.0,8.0},{9.0,10.0,11.0,12.0}};

    CHECK(a.size(0) == 3);
    CHECK(a.size(1) == 4);

    ndarray_view<double,1> v(a,{{0,1},{2,2}});

    REQUIRE(v.size(0) == 2); 
    CHECK(v(0) == 3.0); 
    CHECK(v(1) == 4.0); 
}

TEST_CASE("Test Array View")
{
    std::vector<double> a = {0.0,1.0,2.0,3.0,4.0,5.0};

    ndarray_view<double,2> v(&a[0],{2,3});

    CHECK(v(0,0) == 0.0); 
    CHECK(v(0,1) == 1.0); 
    CHECK(v(0,2) == 2.0); 
    CHECK(v(1,0) == 3.0); 
    CHECK(v.size(0) == 2); 
    CHECK(v.size(1) == 3); 
}

