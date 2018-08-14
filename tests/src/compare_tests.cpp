#include <catch/catch.hpp>
#include "acons/ndarray.hpp"
#include <iostream>

using namespace acons;

TEST_CASE("compare 2 x 2 row major ndarray")
{
    ndarray<double,2,row_major> a = {{0,1},{2,3}};
    ndarray<double,2,row_major> b = {{0,1},{2,4}};

    ndarray_view<double,2,row_major> v(a);
    ndarray_view<double,2,row_major> w(b);

    bool test1 = v == w;

    CHECK(!test1);
}

TEST_CASE("compare 2 x 2 column major ndarray")
{
    ndarray<double,2,column_major> a = {{0,1},{2,3}};
    ndarray<double,2,column_major> b = {{0,1},{2,4}};

    ndarray_view<double,2,column_major> v(a);
    ndarray_view<double,2,column_major> w(b);

    bool test1 = v == w;

    CHECK(!test1);
}

TEST_CASE("compare 2 x 3 x 4 row major ndarray")
{
    ndarray<double,3,row_major> a(2, 3, 4, 1.0);
    ndarray<double,3,row_major> b(a);
    const ndarray<double,3,row_major> c(a);
    ndarray_view<double,3,row_major> v(a);
    ndarray_view<double,3,row_major> w(b);

    bool test1 = c == b;
    bool test2 = c == v;
    bool test3 = c == w;
    CHECK(test1);
    CHECK(test2);
    CHECK(test3);

    v(0,0,0) = 2;
    w(1,2,3) = 2;
    bool test4 = c == v;
    bool test5 = c == w;
    CHECK(!test4);
    CHECK(!test5);
}

TEST_CASE("compare 2 x 2 ndarray_view on 2 x 3 row major")
{
    ndarray<double,2,row_major> a = {{0,1,2},{3,4,5}};
    ndarray<double,2,row_major> b = a;
    ndarray_view<double,2,row_major> v(a, {{0, 2}, {0,2}});
    ndarray_view<double,2,row_major> w(b, {{0, 2}, {0,2}});

    a(0,2) = 6;
    a(1,2) = 7;

    b(0,2) = 8;
    b(1,2) = 9;

    bool test = v == w;
    CHECK(test);
}

TEST_CASE("compare 2 x 2 ndarray_view on 4 x 4 row major")
{
    ndarray<double,2,row_major> a = {{0, 1, 2, 3}, {4, 5, 6, 7}, {8, 9, 10, 11}, {12, 13, 14, 15}};
    ndarray<double,2,row_major> b = a;
    ndarray_view<double,2,row_major> v(a, {{1, 2}, {1,2}});
    ndarray_view<double,2,row_major> w(b, {{1, 2}, {1,2}});

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

    bool test = v == w;
    CHECK(test);
}

TEST_CASE("compare 2 x 3 x 4 column major ndarray")
{
    ndarray<double,3,column_major> a(2, 3, 4, 1.0);
    ndarray<double,3,column_major> b(a);
    const ndarray<double,3,column_major> c(a);
    ndarray_view<double,3,column_major> v(a);
    ndarray_view<double,3,column_major> w(b);

    bool test1 = c == b;
    bool test2 = c == v;
    bool test3 = c == w;
    CHECK(test1);
    CHECK(test2);
    CHECK(test3);

    v(0,0,0) = 2;
    w(1,2,3) = 2;
    bool test4 = c == v;
    bool test5 = c == w;
    CHECK(!test4);
    CHECK(!test5);
}

TEST_CASE("compare row major ndarray")
{
    ndarray<double,2,row_major> a = {{1.0,2.0,3.0,4.0},{5.0,6.0,7.0,8.0}};
    ndarray<double,2,row_major> b = {{1.0,2.0,3.0,4.0},{5.0,6.0,7.0,8.0}};
    ndarray<double,2,row_major> c = {{1.0,4.0,2.0,3.0},{5.0,8.0,6.0,7.0}};
    ndarray<double,2,row_major> d = {{1.0,2.0,3.0,0.0},{5.0,6.0,7.0,8.0}};

    bool test1 = a == b;
    bool test2 = a != b;
    CHECK(test1);
    CHECK(!test2);

    bool test3 = a == c;
    bool test4 = a != c;
    CHECK(!test3);
    CHECK(test4);

    bool test5 = a == d;
    bool test6 = a != d;
    CHECK(!test5);
    CHECK(test6);

    ndarray_view<double, 1, row_major> u(a, { 1 }, { { 0,2 } });
    ndarray_view<double, 1, row_major> v(c, { 1 }, { { 0,2 } });
    ndarray_view<double, 1, row_major> w(c, { 1 }, { { 0,2 } });

    bool test7 = u == v;
    bool test8 = u != v;
    CHECK(!test7);
    CHECK(test8);

    bool test9 = v == w;
    bool test10 = v != w;
    CHECK(test9);
    CHECK(!test10);
}

TEST_CASE("compare column major ndarray")
{
    ndarray<double,2,column_major> a = {{1.0,2.0,3.0,4.0},{5.0,6.0,7.0,8.0}};
    ndarray<double,2,column_major> b = {{1.0,2.0,3.0,4.0},{5.0,6.0,7.0,8.0}};
    ndarray<double,2,column_major> c = {{1.0,4.0,2.0,3.0},{5.0,8.0,6.0,7.0}};
    ndarray<double,2,column_major> d = {{1.0,2.0,3.0,0.0},{5.0,6.0,7.0,8.0}};

    bool test1 = a == b;
    bool test2 = a != b;
    CHECK(test1);
    CHECK(!test2);

    bool test3 = a == c;
    bool test4 = a != c;
    CHECK(!test3);
    CHECK(test4);

    bool test5 = a == d;
    bool test6 = a != d;
    CHECK(!test5);
    CHECK(test6);

    ndarray_view<double,1,column_major> u(a,{1},{{0,2}});
    ndarray_view<double,1,column_major> v(c,{1},{{0,2}});
    ndarray_view<double,1,column_major> w(c,{1},{{0,2}});

    bool test7 = u == v;
    bool test8 = u != v;
    CHECK(!test7);
    CHECK(test8);

    bool test9 = v == w;
    bool test10 = v != w;
    CHECK(test9);
    CHECK(!test10);
}

