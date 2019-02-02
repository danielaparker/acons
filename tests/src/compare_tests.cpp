#include <catch/catch.hpp>
#include <iostream>
#include "acons/ndarray.hpp"
#include <iostream>

using namespace acons;

TEST_CASE("compare 2 x 2 row major ndarray")
{
    ndarray<double,2,row_major> a = {{0,1},{2,3}};
    ndarray<double,2,row_major> b = {{0,1},{2,4}};

    ndarray_view<double,2,row_major> v(a);
    ndarray_view<double,2,row_major> w(b);

    CHECK_FALSE(v == w);
}
TEST_CASE("compare 2 x 2 column major ndarray")
{
    ndarray<double,2,column_major> a = {{0,1},{2,3}};
    ndarray<double,2,column_major> b = {{0,1},{2,4}};

    ndarray_view<double,2,column_major> v(a);
    ndarray_view<double,2,column_major> w(b);

    CHECK_FALSE(v == w);
}

TEST_CASE("compare 2 x 3 x 4 row major ndarray")
{
    ndarray<double,3,row_major> a(2, 3, 4, 1.0);
    ndarray<double,3,row_major> b(a);
    const ndarray<double,3,row_major> c(a);
    ndarray_view<double,3,row_major> v(a);
    ndarray_view<double,3,row_major> w(b);

    CHECK(c == b);
    CHECK(c == v);
    CHECK(c == w);

    v(0,0,0) = 2;
    w(1,2,3) = 2;
    CHECK_FALSE(c == v);
    CHECK_FALSE(c == w);
}

TEST_CASE("compare 2 x 2 ndarray_view on 2 x 3 row major")
{
    ndarray<double,2,row_major> a = {{0,1,2},{3,4,5}};
    ndarray<double,2,row_major> b = a;
    ndarray_view<double,2,row_major> v(a, {slice(0, 2), slice(0,2)});
    ndarray_view<double,2,row_major> w(b, {slice(0, 2), slice(0,2)});

    a(0,2) = 6;
    a(1,2) = 7;

    b(0,2) = 8;
    b(1,2) = 9;

    CHECK(v == w);
}

TEST_CASE("compare 2 x 2 ndarray_view on 4 x 4 row major")
{
    ndarray<double,2,row_major> a = {{0, 1, 2, 3}, {4, 5, 6, 7}, {8, 9, 10, 11}, {12, 13, 14, 15}};
    ndarray<double,2,row_major> b = a;
    ndarray_view<double,2,row_major> v(a, {slice(1, 2), slice(1,2)});
    ndarray_view<double,2,row_major> w(b, {slice(1, 2), slice(1,2)});

    for (size_t j = 0; j < 4; ++j)
    {
        a(0,j) = 16;
        a(3,j) = 16;
        a(j,0) = 17;
        a(j,3) = 17;
    }

    for (size_t j = 0; j < 4; ++j)
    {
        b(0,j) = 26;
        b(3,j) = 26;
        b(j,0) = 27;
        b(j,3) = 27;
    }

    //std::cout << "a: " << a << "\n";
    //std::cout << "b: " << b << "\n";
    //std::cout << "v: " << v << "\n";
    //std::cout << "w: " << w << "\n";

    CHECK(v == w);
}

TEST_CASE("compare 2 x 3 x 4 column major ndarray")
{
    ndarray<double,3,column_major> a(2, 3, 4, 1.0);
    ndarray<double,3,column_major> b(a);
    const ndarray<double,3,column_major> c(a);
    ndarray_view<double,3,column_major> v(a);
    ndarray_view<double,3,column_major> w(b);

    CHECK(c == b);
    CHECK(c == v);
    CHECK(c == w);

    v(0,0,0) = 2;
    w(1,2,3) = 2;
    CHECK_FALSE(c == v);
    CHECK_FALSE(c == w);
}

TEST_CASE("compare row major ndarray")
{
    ndarray<double,2,row_major> a = {{1.0,2.0,3.0,4.0},{5.0,6.0,7.0,8.0}};
    ndarray<double,2,row_major> b = {{1.0,2.0,3.0,4.0},{5.0,6.0,7.0,8.0}};
    ndarray<double,2,row_major> c = {{1.0,4.0,2.0,3.0},{5.0,8.0,6.0,7.0}};
    ndarray<double,2,row_major> d = {{1.0,2.0,3.0,0.0},{5.0,6.0,7.0,8.0}};

    CHECK(a == b);
    CHECK_FALSE(a != b);

    CHECK_FALSE(a == c);
    CHECK(a != c);

    CHECK_FALSE(a == d);
    CHECK(a != d);

    ndarray_view<double, 1, row_major> u(a, { 1 }, {slice(0,2)});
    ndarray_view<double, 1, row_major> v(c, { 1 }, {slice(0,2)});
    ndarray_view<double, 1, row_major> w(c, { 1 }, {slice(0,2)});

    CHECK_FALSE(u == v);
    CHECK(u != v);

    CHECK(v == w);
    CHECK_FALSE(v != w);
}

TEST_CASE("compare column major ndarray")
{
    ndarray<double,2,column_major> a = {{1.0,2.0,3.0,4.0},{5.0,6.0,7.0,8.0}};
    ndarray<double,2,column_major> b = {{1.0,2.0,3.0,4.0},{5.0,6.0,7.0,8.0}};
    ndarray<double,2,column_major> c = {{1.0,4.0,2.0,3.0},{5.0,8.0,6.0,7.0}};
    ndarray<double,2,column_major> d = {{1.0,2.0,3.0,0.0},{5.0,6.0,7.0,8.0}};

    CHECK((a == b));
    CHECK_FALSE(a != b);

    CHECK_FALSE(a == c);
    CHECK(a != c);

    CHECK_FALSE(a == d);
    CHECK(a != d);

    ndarray_view<double,1,column_major> u(a,{1},{slice(0,2)});
    ndarray_view<double,1,column_major> v(c,{1},{slice(0,2)});
    ndarray_view<double,1,column_major> w(c,{1},{slice(0,2)});

    CHECK_FALSE(u == v);
    CHECK(u != v);

    CHECK(v == w);
    CHECK_FALSE(v != w);
}


