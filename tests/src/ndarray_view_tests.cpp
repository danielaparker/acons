#include <catch/catch.hpp>
#include "acons/ndarray.hpp"
#include <iostream>

using namespace acons;

TEST_CASE("2x3 dim row major zero_based, with right offsets")
{
    ndarray<double, 2, row_major> a = { {0,1,2,3}, {4,5,6,7}, {8,9,10,11} };
    ndarray_view<double, 2, row_major> v(a, {{0,2}, {1,3}});

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

    std::cout << "v: " << v << "\n\n";
    std::cout << "dimensions: " << v.dimensions() << "\n\n";
    std::cout << "strides: " << v.strides() << "\n\n";
    std::cout << "offsets: " << v.offsets() << "\n\n";
}

#if 0
TEST_CASE("2x3x4 dim row major zero_based, slices")
{
    // Construct a 3-dimensional array with dimensions 2 x 3 x 4
    ndarray<double,3,row_major,zero_based> a = { {{0,1,2,3},{4,5,6,7},{8,9,10,11}}, {{12,13,14,15},{16,17,18,19},{20,21,22,23}} };

    ndarray_view<double,3,row_major,zero_based> v(a,{{1,2},{1,3},{0,4,2}});

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

TEST_CASE("2x3x4 dim column major one_based, slices")
{
    // Construct a 3-dimensional array with dimensions 2 x 3 x 4
    ndarray<double,3,column_major,one_based> a = { {{0,1,2,3},{4,5,6,7},{8,9,10,11}}, {{12,13,14,15},{16,17,18,19},{20,21,22,23}} };

    ndarray_view<double,3,column_major,one_based> v(a,{{2,3},{2,4},{1,5,2}});

    REQUIRE(v.size(0) == 1);
    REQUIRE(v.size(1) == 2);
    REQUIRE(v.size(2) == 2);
    CHECK(v(1,1,1) == 16);
    CHECK(v(1,1,2) == 18);
    CHECK(v(1,2,1) == 20);
    CHECK(v(1,2,2) == 22);

    auto it = v.begin();
    auto end = v.end();
    CHECK(*it++ == 16);
    CHECK(*it++ == 20);
    CHECK(*it++ == 18);
    CHECK(*it++ == 22);
    CHECK(it == end);
}

TEST_CASE("2x3x4 dim row major zero_based, origin")
{
    // Construct a 3-dimensional array with dimensions 2 x 3 x 4
    ndarray<double,3,row_major,zero_based> a = { {{0,1,2,3},{4,5,6,7},{8,9,10,11}}, {{12,13,14,15},{16,17,18,19},{20,21,22,23}} };

    ndarray_view<double,2,row_major,zero_based> v(a,{1});
    REQUIRE(v.size(0) == 3);
    REQUIRE(v.size(1) == 4);

    std::cout << "v: " << v << "\n\n";

    CHECK(v(0,0) == 12);
    CHECK(v(0,1) == 13);
    CHECK(v(0,2) == 14);
    CHECK(v(0,3) == 15);
    CHECK(v(1,0) == 16);
    CHECK(v(1,1) == 17);
    CHECK(v(1,2) == 18);
    CHECK(v(1,3) == 19);
    CHECK(v(2,0) == 20);
    CHECK(v(2,1) == 21);
    CHECK(v(2,2) == 22);
    CHECK(v(2,3) == 23);

    auto it = v.begin();
    auto end = v.end();
    CHECK(*it++ == 12);
    CHECK(*it++ == 13);
    CHECK(*it++ == 14);
    CHECK(*it++ == 15);
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

TEST_CASE("2x3x4 dim column major one_based, origin")
{
    // Construct a 3-dimensional array with dimensions 2 x 3 x 4
    ndarray<double,3,column_major,one_based> a = { {{0,1,2,3},{4,5,6,7},{8,9,10,11}}, {{12,13,14,15},{16,17,18,19},{20,21,22,23}} };

    ndarray_view<double,3,column_major,one_based> v(a,{{2,3},{2,4},{1,5,2}});

    REQUIRE(v.size(0) == 1);
    REQUIRE(v.size(1) == 2);
    REQUIRE(v.size(2) == 2);
    CHECK(v(1,1,1) == 16);
    CHECK(v(1,1,2) == 18);
    CHECK(v(1,2,1) == 20);
    CHECK(v(1,2,2) == 22);

    auto it = v.begin();
    auto end = v.end();
    CHECK(*it++ == 16);
    CHECK(*it++ == 20);
    CHECK(*it++ == 18);
    CHECK(*it++ == 22);
    CHECK(it == end);
}
#endif
