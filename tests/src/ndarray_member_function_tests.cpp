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

TEST_CASE("constructor 2a")
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

    CHECK_FALSE(a.empty());
    CHECK(a.size() == 6);
    CHECK(a.size(0) == 1);
    CHECK(a.size(1) == 2);
    CHECK(a.size(2) == 3);

    for (size_t i = 0; i < a.size(); ++i)
    {
        CHECK(a.data()[i] == i);
    }
}

TEST_CASE("constructor 2b")
{
    ndarray<double,3> a(1,2,3,10.0);

    CHECK_FALSE(a.empty());
    CHECK(a.size() == 6);
    CHECK(a.size(0) == 1);
    CHECK(a.size(1) == 2);
    CHECK(a.size(2) == 3);

    for (size_t i = 0; i < a.size(); ++i)
    {
        CHECK(a.data()[i] == 10.0);
    }
}

TEST_CASE("constructor 3")
{
    std::array<size_t,3> dim{ 1,2,3 };
    ndarray<double, 3> a(dim);

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

    CHECK_FALSE(a.empty());
    CHECK(a.size() == 6);
    CHECK(a.size(0) == 1);
    CHECK(a.size(1) == 2);
    CHECK(a.size(2) == 3);

    for (size_t i = 0; i < a.size(); ++i)
    {
        CHECK(a.data()[i] == i);
    }
}

TEST_CASE("constructor 4")
{
    std::array<size_t,3> dim{ 1,2,3 };
    ndarray<double, 3> a(dim,
                         std::allocator<double>());

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

    CHECK_FALSE(a.empty());
    CHECK(a.size() == 6);
    CHECK(a.size(0) == 1);
    CHECK(a.size(1) == 2);
    CHECK(a.size(2) == 3);

    for (size_t i = 0; i < a.size(); ++i)
    {
        CHECK(a.data()[i] == i);
    }
}

TEST_CASE("constructor 5")
{
    std::array<size_t,3> dim{ 1,2,3 };
    ndarray<double, 3> a(dim, 10.0);

    CHECK_FALSE(a.empty());
    CHECK(a.size() == 6);
    CHECK(a.size(0) == 1);
    CHECK(a.size(1) == 2);
    CHECK(a.size(2) == 3);

    for (size_t i = 0; i < a.size(); ++i)
    {
        CHECK(a.data()[i] == 10.0);
    }
}

TEST_CASE("constructor 6")
{
    std::array<size_t,3> dim{ 1,2,3 };
    ndarray<double, 3> a(dim, 10.0, std::allocator<double>());

    CHECK_FALSE(a.empty());
    CHECK(a.size() == 6);
    CHECK(a.size(0) == 1);
    CHECK(a.size(1) == 2);
    CHECK(a.size(2) == 3);

    for (size_t i = 0; i < a.size(); ++i)
    {
        CHECK(a.data()[i] == 10.0);
    }
}

TEST_CASE("constructor 7")
{
    ndarray<double, 3> a= {{{0,1,2},{3,4,5}}};

    CHECK_FALSE(a.empty());
    CHECK(a.size() == 6);
    CHECK(a.size(0) == 1);
    CHECK(a.size(1) == 2);
    CHECK(a.size(2) == 3);

    for (size_t i = 0; i < a.size(); ++i)
    {
        CHECK(a.data()[i] == i);
    }
}

TEST_CASE("constructor 8")
{
    ndarray<double, 3> a({{{0,1,2},{3,4,5}}}, std::allocator<double>());

    CHECK_FALSE(a.empty());
    CHECK(a.size() == 6);
    CHECK(a.size(0) == 1);
    CHECK(a.size(1) == 2);
    CHECK(a.size(2) == 3);

    for (size_t i = 0; i < a.size(); ++i)
    {
        CHECK(a.data()[i] == i);
    }
}

TEST_CASE("constructor 9")
{
    ndarray<double, 3> a = {{{0,1,2},{3,4,5}}};

    ndarray<double, 3> b(a);

    CHECK(a.size() == b.size());
    for (size_t i = 0; i < 3; ++i)
    {
        CHECK(a.size(i) == b.size(i));
    }
    for (size_t i = 0; i < a.size(); ++i)
    {
        CHECK(a.data()[i] == b.data()[i]);
    }
    CHECK(a == b);
}

TEST_CASE("constructor 11")
{
    ndarray<double, 3> a = {{{0,1,2},{3,4,5}}};

    CHECK_FALSE(a.empty());
    CHECK(a.size() == 6);
    CHECK(a.size(0) == 1);
    CHECK(a.size(1) == 2);
    CHECK(a.size(2) == 3);

    ndarray<double, 3> b(std::move(a));

    CHECK_FALSE(b.empty());
    CHECK(b.size() == 6);
    CHECK(b.size(0) == 1);
    CHECK(b.size(1) == 2);
    CHECK(b.size(2) == 3);

    for (size_t i = 0; i < b.size(); ++i)
    {
        CHECK(b.data()[i] == i);
    }

    CHECK(a.empty());
    CHECK(a.size() == 0);
    CHECK(a.size(0) == 0);
    CHECK(a.size(1) == 0);
    CHECK(a.size(2) == 0);
}

TEST_CASE("constructor 12")
{
    ndarray<double, 3> a = {{{0,1,2},{3,4,5}}};

    ndarray<double, 3> b(std::move(a), std::allocator<double>());

    CHECK_FALSE(b.empty());
    CHECK(b.size() == 6);
    CHECK(b.size(0) == 1);
    CHECK(b.size(1) == 2);
    CHECK(b.size(2) == 3);

    for (size_t i = 0; i < b.size(); ++i)
    {
        CHECK(b.data()[i] == i);
    }

    CHECK(a.empty());
    CHECK(a.size() == 0);
    CHECK(a.size(0) == 0);
    CHECK(a.size(1) == 0);
    CHECK(a.size(2) == 0);
}

// operator==

TEST_CASE("assignment 1")
{
    ndarray<double, 3> a = {{{0,1,2},{3,4,5}}};
    ndarray<double, 3> b;

    b = a;

    CHECK(b == a);

    ndarray<double, 3> c;
    c = std::move(b);
    CHECK(c == a);

    CHECK(b.empty());

    b = {{{0,1,2},{3,4,5}}};
    CHECK(b == a);
}

// operator()

TEST_CASE("indexing operator 1")
{
    ndarray<double, 3> A(2,3,4);

    int init = 0;
    for (size_t i = 0; i < A.size(0); ++i)
    {
        for (size_t j = 0; j < A.size(1); ++j)
        {
            for (size_t k = 0; k < A.size(2); ++k)
            {
                A(i,j,k) = init++;
            }
        }
    }
    std::cout << "A: " << A << "\n\n";

    ndarray_view<double,2> v = A.subarray(std::array<size_t,1>{0});

    std::cout << "v: " << v << "\n\n";

    ndarray<double,2> u = A.subarray(std::array<size_t,1>{0});

    std::cout << "u: " << u << "\n\n";
}

