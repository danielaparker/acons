#include <catch/catch.hpp>
#include <iostream>
#include "acons/ndarray.hpp"
#include <sstream>

using namespace acons;

// row_major tests
TEST_CASE("dim:1 row major iterator test")
{
    ndarray<double, 1, row_major> a = { 0,1,2,3 };
    ndarray_view<double, 1, row_major> v(a, {{1,3}});
    std::cout << "a: " << a << "\n\n";
    std::cout << "v: " << v << "\n\n";
    std::cout << "dimensions: " << v.dimensions() << "\n\n";
    std::cout << "strides: " << v.strides() << "\n\n";
    std::cout << "offsets: " << v.offsets() << "\n\n";

    auto it = v.begin();
    auto end = v.end();

    CHECK(v(0) == 1);
    CHECK(v(1) == 2);

    CHECK(*it++ == v(0));
    CHECK(*it++ == v(1));
    CHECK(it == end);
}

TEST_CASE("dim:1 row_major stride:2 iterator test")
{
    ndarray<double, 1, row_major> a = { 0,1,2,3,4,5 };
    ndarray_view<double, 1, row_major> v(a, {{1,5,2}});

    std::cout << "v data: ";
    for (size_t i = 0; i < v.size(); ++i)
    {
        if (i > 0)
        {
            std::cout << ",";
        }
        std::cout << v.data()[i];
    }
    std::cout << "\n\n";

    std::cout << "a: " << a << "\n\n";
    std::cout << "v: " << v << "\n\n";
    std::cout << "dimensions: " << v.dimensions() << "\n\n";
    std::cout << "strides: " << v.strides() << "\n\n";
    std::cout << "offsets: " << v.offsets() << "\n\n";

    auto it = v.begin();
    auto end = v.end();

    CHECK(v(0) == 1);
    CHECK(v(1) == 3);

    CHECK(*it++ == v(0));
    CHECK(*it++ == v(1));
    CHECK(it == end);
}

TEST_CASE("dim: 2x3 row major zero_based, 1st dim offset, 2nd dim stride, (dimensions[1]*strides[1] == strides[0])")
{
    ndarray<double, 2, row_major> a = { {0,1,2,3}, {4,5,6,7}, {8,9,10,11} };
    ndarray_view<double, 2, row_major> v(a, {{1,3}, {0,4,2}});

    std::cout << "data: ";
    for (size_t i = 0; i < v.size(); ++i)
    {
        if (i > 0)
        {
            std::cout << ",";
        }
        std::cout << v.data()[i];
    }
    std::cout << "\n\n";

    std::cout << "a: " << a << "\n\n";
    std::cout << "v: " << v << "\n\n";
    std::cout << "dimensions: " << v.dimensions() << "\n\n";
    std::cout << "strides: " << v.strides() << "\n\n";
    std::cout << "offsets: " << v.offsets() << "\n\n";

    auto it = v.begin();
    auto end = v.end();
        
    CHECK(*it++ == v(0,0));
    CHECK(*it++ == v(0,1));
    CHECK(*it++ == v(1,0));
    CHECK(*it++ == v(1,1));
    CHECK(it == end);
}

TEST_CASE("dim: 2x3 row major zero_based, 1st dim offset, 2nd dim stride, (dimensions[1]*strides[1] == strides[0]) etc")
{
    ndarray<double, 2, row_major> a = { {0,1,2,3}, {4,5,6,7}, {8,9,10,11} };
    ndarray_view<double, 2, row_major> v(a, {{1,3}, {0,4,2}});

    std::cout << "data: ";
    for (size_t i = 0; i < v.size(); ++i)
    {
        if (i > 0)
        {
            std::cout << ",";
        }
        std::cout << v.data()[i];
    }
    std::cout << "\n\n";

    std::cout << "a: " << a << "\n\n";
    std::cout << "v: " << v << "\n\n";
    std::cout << "dimensions: " << v.dimensions() << "\n\n";
    std::cout << "strides: " << v.strides() << "\n\n";
    std::cout << "offsets: " << v.offsets() << "\n\n";

    auto it = v.begin();
    auto end = v.end();
        
    CHECK(*it++ == v(0,0));
    CHECK(*it++ == v(0,1));
    CHECK(*it++ == v(1,0));
    CHECK(*it++ == v(1,1));
    CHECK(it == end);
}

TEST_CASE("2x3 dim row major zero_based, 1st dim offset, 2nd dim stride")
{
    ndarray<double, 2, row_major> a = { {0,1,2,3}, {4,5,6,7}, {8,9,10,11} };
    ndarray_view<double, 2, row_major> v(a, {{1,3}, {0,2,2}});

    std::cout << "data: ";
    for (size_t i = 0; i < v.size(); ++i)
    {
        if (i > 0)
        {
            std::cout << ",";
        }
        std::cout << v.data()[i];
    }
    std::cout << "\n\n";

    std::cout << "a: " << a << "\n\n";
    std::cout << "v: " << v << "\n\n";
    std::cout << "dimensions: " << v.dimensions() << "\n\n";
    std::cout << "strides: " << v.strides() << "\n\n";
    std::cout << "offsets: " << v.offsets() << "\n\n";

    auto it = v.begin();
    auto end = v.end();
        
    CHECK(*it++ == v(0,0));
    CHECK(*it++ == v(1,0));
    CHECK(it == end);
}
TEST_CASE("2x3 dim row major zero_based, with 1st dimension offsets")
{
    ndarray<double, 2, row_major> a = { {0,1,2,3}, {4,5,6,7}, {8,9,10,11} };
    ndarray_view<double, 2, row_major> v(a, {{1,3}, {0,2}});

    std::cout << "data: ";
    for (size_t i = 0; i < v.size(); ++i)
    {
        if (i > 0)
        {
            std::cout << ",";
        }
        std::cout << v.data()[i];
    }
    std::cout << "\n\n";

    std::cout << "a: " << a << "\n\n";
    std::cout << "v: " << v << "\n\n";
    std::cout << "dimensions: " << v.dimensions() << "\n\n";
    std::cout << "strides: " << v.strides() << "\n\n";
    std::cout << "offsets: " << v.offsets() << "\n\n";

    auto it = v.begin();
    auto end = v.end();
        
    CHECK(*it++ == v(0,0));
    CHECK(*it++ == v(0,1));
    CHECK(*it++ == v(1,0));
    CHECK(*it++ == v(1,1));
    CHECK(it == end);
}

TEST_CASE("2x3 dim row major zero_based, with 2nd dimension offsets")
{
    ndarray<double, 2, row_major> a = { {0,1,2,3}, {4,5,6,7}, {8,9,10,11} };
    ndarray_view<double, 2, row_major> v(a, {{0,2}, {1,3}});
    std::cout << "a: " << a << "\n\n";
    std::cout << "v: " << v << "\n\n";
    std::cout << "dimensions: " << v.dimensions() << "\n\n";
    std::cout << "strides: " << v.strides() << "\n\n";
    std::cout << "offsets: " << v.offsets() << "\n\n";

    auto it = v.begin();
    auto end = v.end();
        
    CHECK(*it++ == v(0,0));
    CHECK(*it++ == v(0,1));
    CHECK(*it++ == v(1,0));
    CHECK(*it++ == v(1,1));
    CHECK(it == end);
}

TEST_CASE("1 dim slices")
{
    ndarray<double, 3, row_major> a = { {{0,1,2,3},{4,5,6,7},{8,9,10,11}}, {{12,13,14,15},{16,17,18,19},{20,21,22,23}} };
    ndarray_view<double, 3> v(a,{{1,2},{1,3},{0,4,2}});
    std::cout << "dimensions: " << v.dimensions() << "\n\n";
    std::cout << "strides: " << v.strides() << "\n\n";
    std::cout << "offsets: " << v.offsets() << "\n\n";

    std::cout << "v data: ";
    for (size_t i = 0; i < v.size(); ++i)
    {
        if (i > 0)
        {
            std::cout << ",";
        }
        std::cout << v.data()[i];
    }
    std::cout << "\n\n";

    SECTION("u(v, { 0,0 })")
    {
        std::cout << "HERE\n\n";
        const_ndarray_view<double, 1> u(v, std::array<size_t, 2>{0, 0});

        REQUIRE(u.size(0) == 2);
        CHECK(u(0) == 16);
        CHECK(u(1) == 18);

        auto it = u.begin();
        CHECK(*it++ == 16);
        CHECK(*it++ == 18);
        CHECK(it == u.end());
    }
}

TEST_CASE("2x3x4 dim row major iterator test stride 1")
{
    // Construct a 3-dimensional array with dimensions 2 x 3 x 4
    ndarray<double, 3, row_major> a = { {{0,1,2,3},{4,5,6,7},{8,9,10,11}}, {{12,13,14,15},{16,17,18,19},{20,21,22,23}} };
    ndarray_view<double, 3> v(a,{{1,2},{1,3},{0,4}});

    std::cout << "a: " << a << "\n\n";
    std::cout << "v: " << v << "\n\n";
    std::cout << "dimensions: " << v.dimensions() << "\n\n";
    std::cout << "strides: " << v.strides() << "\n\n";
    std::cout << "offsets: " << v.offsets() << "\n\n";

    auto it = v.begin();
    auto end = v.end();

    REQUIRE(v.size(0) == 1);
    REQUIRE(v.size(1) == 2);
    REQUIRE(v.size(2) == 4);
    CHECK(v(0,0,0) == 16);
    CHECK(v(0,0,1) == 17);
    CHECK(v(0,0,2) == 18);
    CHECK(v(0,0,3) == 19);
    CHECK(v(0,1,0) == 20);
    CHECK(v(0,1,1) == 21);
    CHECK(v(0,1,2) == 22);
    CHECK(v(0,1,3) == 23);

    CHECK(*it++ == 16);
    CHECK(*it++ == 17);
    CHECK(*it++ == 18);
    CHECK(*it++ == 19);
    CHECK(*it++ == 20);
    CHECK(*it++ == 21);
    CHECK(*it++ == 22);
    CHECK(*it++ == 23);
    CHECK(it == end);
}

TEST_CASE("2x3x4 dim row major iterator test stride 2")
{
    // Construct a 3-dimensional array with dimensions 2 x 3 x 4
    ndarray<double, 3, row_major> a = { {{0,1,2,3},{4,5,6,7},{8,9,10,11}}, {{12,13,14,15},{16,17,18,19},{20,21,22,23}} };
    ndarray_view<double, 3> v(a,{{1,2},{1,3},{0,4,2}});

    std::cout << "a: " << a << "\n\n";
    std::cout << "v: " << v << "\n\n";
    std::cout << "dimensions: " << v.dimensions() << "\n\n";
    std::cout << "strides: " << v.strides() << "\n\n";
    std::cout << "offsets: " << v.offsets() << "\n\n";

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

// column_major tests

TEST_CASE("1 dim column major iterator test")
{
    ndarray<double, 1, column_major> a = { 0,1,2,3 };
    ndarray_view<double, 1, column_major> v(a, {{1,3}});
    std::cout << "a: " << a << "\n\n";
    std::cout << "v: " << v << "\n\n";
    std::cout << "dimensions: " << v.dimensions() << "\n\n";
    std::cout << "strides: " << v.strides() << "\n\n";
    std::cout << "offsets: " << v.offsets() << "\n\n";

    auto it = v.begin();
    auto end = v.end();

    CHECK(*it++ == v(0));
    CHECK(*it++ == v(1));
    CHECK(it == end);
}

TEST_CASE("2x3 dim column major zero_based, 1st dim offset, 2nd dim stride, (dimensions[1]*strides[1] == strides[0])")
{
    ndarray<double, 2, column_major> a = { {0,1,2,3}, {4,5,6,7}, {8,9,10,11} };
    ndarray_view<double, 2, column_major> v(a, {{1,3}, {0,4,2}});

    std::cout << "data: ";
    for (size_t i = 0; i < v.size(); ++i)
    {
        if (i > 0)
        {
            std::cout << ",";
        }
        std::cout << v.data()[i];
    }
    std::cout << "\n\n";

    std::cout << "a: " << a << "\n\n";
    std::cout << "v: " << v << "\n\n";
    std::cout << "dimensions: " << v.dimensions() << "\n\n";
    std::cout << "strides: " << v.strides() << "\n\n";
    std::cout << "offsets: " << v.offsets() << "\n\n";

    auto it = v.begin();
    auto end = v.end();
        
    CHECK(*it++ == v(0,0));
    CHECK(*it++ == v(1,0));
    CHECK(*it++ == v(0,1));
    CHECK(*it++ == v(1,1));
    CHECK(it == end);
}
TEST_CASE("2x3 dim column major zero_based, 1st dim offset, 2nd dim stride")
{
    ndarray<double, 2, column_major> a = { {0,1,2,3}, {4,5,6,7}, {8,9,10,11} };
    ndarray_view<double, 2, column_major> v(a, {{1,3}, {0,2,2}});

    std::cout << "data: ";
    for (size_t i = 0; i < v.size(); ++i)
    {
        if (i > 0)
        {
            std::cout << ",";
        }
        std::cout << v.data()[i];
    }
    std::cout << "\n\n";

    std::cout << "a: " << a << "\n\n";
    std::cout << "v: " << v << "\n\n";
    std::cout << "dimensions: " << v.dimensions() << "\n\n";
    std::cout << "strides: " << v.strides() << "\n\n";
    std::cout << "offsets: " << v.offsets() << "\n\n";

    auto it = v.begin();
    auto end = v.end();
        
    CHECK(*it++ == v(0,0));
    CHECK(*it++ == v(1,0));
    CHECK(it == end);
}
TEST_CASE("2x3 dim column major zero_based, with 1st dimension offsets")
{
    ndarray<double, 2, column_major> a = { {0,1,2,3}, {4,5,6,7}, {8,9,10,11} };
    ndarray_view<double, 2, column_major> v(a, {{1,3}, {0,2}});

    std::cout << "data: ";
    for (size_t i = 0; i < v.size(); ++i)
    {
        if (i > 0)
        {
            std::cout << ",";
        }
        std::cout << v.data()[i];
    }
    std::cout << "\n\n";

    std::cout << "a: " << a << "\n\n";
    std::cout << "v: " << v << "\n\n";
    std::cout << "dimensions: " << v.dimensions() << "\n\n";
    std::cout << "strides: " << v.strides() << "\n\n";
    std::cout << "offsets: " << v.offsets() << "\n\n";

    auto it = v.begin();
    auto end = v.end();
        
    CHECK(*it++ == v(0,0));
    CHECK(*it++ == v(1,0));
    CHECK(*it++ == v(0,1));
    CHECK(*it++ == v(1,1));
    CHECK(it == end);
}

TEST_CASE("2x3 dim column major zero_based, with 2nd dimension offsets")
{
    ndarray<double, 2, column_major> a = { {0,3,6,9}, 
                                           {1,4,7,10}, 
                                           {2,5,8,11} };
    ndarray_view<double, 2, column_major> v(a, {{0,2}, {1,3}});
    //std::cout << "a: " << a << "\n\n";
    //std::cout << "v: " << v << "\n\n";
    std::cout << "dimensions: " << v.dimensions() << "\n\n";
    std::cout << "strides: " << v.strides() << "\n\n";
    std::cout << "offsets: " << v.offsets() << "\n\n";

    auto it = v.begin();
    auto end = v.end();

    CHECK(v(0,0) == 3);
    CHECK(v(1,0) == 4);
    CHECK(v(0,1) == 6);
    CHECK(v(1,1) == 7);
        
    CHECK(*it++ == v(0,0));
    CHECK(*it++ == v(1,0));
    CHECK(*it++ == v(0,1));
    CHECK(*it++ == v(1,1));
    CHECK(it == end);
}

TEST_CASE("2x3x4 dim column major iterator test, stride 1")
{
    // Construct a 3-dimensional array with dimensions 2 x 3 x 4
    ndarray<double, 3, column_major> a = { {{0,1,2,3},{4,5,6,7},{8,9,10,11}}, {{12,13,14,15},{16,17,18,19},{20,21,22,23}} };
    ndarray_view<double, 3, column_major> v(a,{{1,2},{1,3},{0,4}});

    std::cout << "a: " << a << "\n\n";
    std::cout << "v: " << v << "\n\n";
    std::cout << "dimensions: " << v.dimensions() << "\n\n";
    std::cout << "strides: " << v.strides() << "\n\n";
    std::cout << "offsets: " << v.offsets() << "\n\n";

    auto it = v.begin();
    auto end = v.end();

    REQUIRE(v.size(0) == 1);
    REQUIRE(v.size(1) == 2);
    REQUIRE(v.size(2) ==4);
    CHECK(v(0,0,0) == 16);
    CHECK(v(0,1,0) == 20);
    CHECK(v(0,0,1) == 17);
    CHECK(v(0,1,1) == 21);
    CHECK(v(0,0,2) == 18);
    CHECK(v(0,1,2) == 22);
    CHECK(v(0,0,3) == 19);
    CHECK(v(0,1,3) == 23);

    CHECK(*it++ == 16);
    CHECK(*it++ == 20);
    CHECK(*it++ == 17);
    CHECK(*it++ == 21);
    CHECK(*it++ == 18);
    CHECK(*it++ == 22);
    CHECK(*it++ == 19);
    CHECK(*it++ == 23);
    CHECK(it == end);
}

TEST_CASE("2x3x4 dim column major iterator test")
{
    // Construct a 3-dimensional array with dimensions 2 x 3 x 4
    ndarray<double, 3, column_major> a = { {{0,1,2,3},{4,5,6,7},{8,9,10,11}}, {{12,13,14,15},{16,17,18,19},{20,21,22,23}} };
    ndarray_view<double, 3, column_major> v(a,{{1,2},{1,3},{0,4,2}});

    std::cout << "a: " << a << "\n\n";
    std::cout << "v: " << v << "\n\n";
    std::cout << "dimensions: " << v.dimensions() << "\n\n";
    std::cout << "strides: " << v.strides() << "\n\n";
    std::cout << "offsets: " << v.offsets() << "\n\n";

    auto it = v.begin();
    auto end = v.end();

    REQUIRE(v.size(0) == 1);
    REQUIRE(v.size(1) == 2);
    REQUIRE(v.size(2) == 2);
    CHECK(v(0,0,0) == 16);
    CHECK(v(0,1,0) == 20);
    CHECK(v(0,0,1) == 18);
    CHECK(v(0,1,1) == 22);

    CHECK(*it++ == 16);
    CHECK(*it++ == 20);
    CHECK(*it++ == 18);
    CHECK(*it++ == 22);
    CHECK(it == end);
}

