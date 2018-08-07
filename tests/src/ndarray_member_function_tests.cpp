#include <catch/catch.hpp>
#include <iostream>
#include "acons/ndarray.hpp"
#include <sstream>

using namespace acons;

// constructors

TEST_CASE("constructor 1")
{
    ndarray<double,3> a;

    CHECK(a.data() == nullptr);
    CHECK(a.empty());
    CHECK(a.size() == 0);
    CHECK(a.size(0) == 0);
    CHECK(a.size(1) == 0);
    CHECK(a.size(2) == 0);
    for (auto i : a.dimensions())
    {
        CHECK(i == 0);
    }
    for (auto i : a.strides())
    {
        CHECK(i == 0);
    }
}

TEST_CASE("constructor 2")
{
    ndarray<double,3> a(1,2,3);

    double x = 0;
    for (size_t i = 0; i < a.size(0); ++i)
    {
        for (size_t j = 0; j < a.size(1); ++j)
        {
            for (size_t k = 0; k < a.size(2); ++k)
            {
                a(i,j,k) = x++;
            }
        }
    }

    std::cout << a << std::endl;

    std::cout << "strides\n";
    for (auto i : a.strides())
    {
        std::cout << i << ",";
    }
    std::cout << "\n\n";

    CHECK(!a.empty());
    CHECK(a.size() == 6);
    CHECK(a.size(0) == 1);
    CHECK(a.size(1) == 2);
    CHECK(a.size(2) == 3);

    for (size_t i = 0; i < a.size(); ++i)
    {
        CHECK(a.data()[i] == i);
    }
}

TEST_CASE("constructor 3")
{
    ndarray<double, 3> a({ 1,2,3 });

    double x = 0;
    for (size_t i = 0; i < a.size(0); ++i)
    {
        for (size_t j = 0; j < a.size(1); ++j)
        {
            for (size_t k = 0; k < a.size(2); ++k)
            {
                a(i, j, k) = x++;
            }
        }
    }

    std::cout << a << std::endl;

    std::cout << "strides\n";
    for (auto i : a.strides())
    {
        std::cout << i << ",";
    }
    std::cout << "\n\n";
}

