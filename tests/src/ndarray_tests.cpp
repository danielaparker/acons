#define CATCH_CONFIG_MAIN
#include <catch/catch.hpp>
#include "acons/ndarray.hpp"
#include <iostream>

using namespace acons;

TEST_CASE("2D Array")
{
    ndarray<double, 2> a(3, 2);

    a(0, 0) = 0;
    a(0, 1) = 1;
    a(1, 0) = 2;
    a(1, 1) = 3;
    a(2, 0) = 4;
    a(2, 1) = 5;

    std::cout << "a=" << a << std::endl;

    //array_ref<double, 1> b(a.data() + 2, 2 );

    ndarray_view<double,1> b(a,{1,0},{2});
    CHECK(b(0) == 2);
    CHECK(b(1) == 3);
    //std::cout << "b=" << b << std::endl;
}

TEST_CASE("2D Array 2")
{
    ndarray<double, 2> a(std::array<size_t,2>{3, 2});

    a(0, 0) = 0;
    a(0, 1) = 1;
    a(1, 0) = 2;
    a(1, 1) = 3;
    a(2, 0) = 4;
    a(2, 1) = 5;

    std::cout << "a=" << a << std::endl;

    //array_ref<double, 1> b(a.data() + 2, { 2 });

    ndarray_view<double,1> b(a,{1,0},{2});
    CHECK(b(0) == 2);
    CHECK(b(1) == 3);
    std::cout << "b=" << b << std::endl;
}

TEST_CASE("2D Array 3")
{
    ndarray<size_t, 2> a(3, 2, 7);

    a(1,0) = 6;
    CHECK(a(0, 0) == 7);
    CHECK(a(1, 0) == 6);

    std::cout << "a=" << a << std::endl;

}

TEST_CASE("2D Array 4")
{
    ndarray<size_t, 2> a(3, 2);

    a(0,0) = 6;
    CHECK(a(0, 0) == 6);

    std::cout << "a=" << a << std::endl;

}

TEST_CASE("3D Array")
{
    ndarray<double, 3> a(2, 2, 2);

    double n1 = a(0, 0, 0);
    double n2 = a(0, 0, 1);
    double n3 = a(0, 1, 0);
    double n4 = a(0, 1, 1);
    double n5 = a(1, 0, 0);
    double n6 = a(1, 0, 1);

    std::cout << n1 << "," << n2 << "," << n3 << "," << n4 << "," << n5 << "," << n6 << std::endl;
}

TEST_CASE("TestArrayInitializerList")
{
    ndarray<double,1> a = {1.0,2.0,3.0,4.0};
    CHECK(a.size(0) == 4);

    std::cout << "a" << std::endl;
    std::cout << a << std::endl;
}

TEST_CASE("TestArray2DInitializerList")
{
    ndarray<double,2> a = {{1.0,2.0,3.0,4.0},{5.0,6.0,7.0,8.0}};

    CHECK(a.size(0) == 2);
    CHECK(a.size(1) == 4);

    std::cout << "a" << std::endl;
    std::cout << a << std::endl;
}

TEST_CASE("Test Array View 1")
{
    ndarray<double,2> a = {{1.0,2.0,3.0,4.0},{5.0,6.0,7.0,8.0}};

    CHECK(a.size(0) == 2);
    CHECK(a.size(1) == 4);

    ndarray_view<double,2> v(a,{1,1},{1,3});

    CHECK(v(0,0) == 6.0); 
    CHECK(v(0,1) == 7.0); 
    CHECK(v(0,2) == 8.0); 

    CHECK(v.size() == 3); 
    CHECK(v.size(0) == 1); 
    CHECK(v.size(1) == 3); 
}

TEST_CASE("Test Array View 2")
{
    ndarray<double,2> a = {{1.0,2.0,3.0,4.0},{5.0,6.0,7.0,8.0}};

    CHECK(a.size(0) == 2);
    CHECK(a.size(1) == 4);

    ndarray_view<double,1> v(a,{1,1},{3});

    CHECK(v(0) == 6.0); 
    CHECK(v(1) == 7.0); 
    CHECK(v(2) == 8.0); 
    CHECK(v.size() == 3); 
    CHECK(v.size(0) == 3); 
}

TEST_CASE("Test Array View 3")
{
    std::vector<double> a = {0.0,1.0,2.0,3.0,4.0,5.0};

    ndarray_view<double,2> v(&a[0],{2,3});

    CHECK(v(0,0) == 0.0); 
    CHECK(v(0,1) == 1.0); 
    CHECK(v(0,2) == 2.0); 
    CHECK(v(1,0) == 3.0); 
    CHECK(v.size() == 6); 
    CHECK(v.size(0) == 2); 
    CHECK(v.size(1) == 3); 

}

