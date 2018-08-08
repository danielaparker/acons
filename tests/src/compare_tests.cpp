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

    ndarray_view<double,1,row_major> u(a,{0,1},{2});
    ndarray_view<double,1,row_major> v(c,{0,2},{2});
    ndarray_view<double,1,row_major> w(c,{0,0},{2});

    bool test7 = u == v;
    bool test8 = u != v;
    CHECK(test7);
    CHECK(!test8);

    bool test9 = u == w;
    bool test10 = u != w;
    CHECK(!test9);
    CHECK(test10);
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

    ndarray_view<double,1,column_major> u(a,{0,1},{2});
    ndarray_view<double,1,column_major> v(c,{0,2},{2});
    ndarray_view<double,1,column_major> w(c,{0,0},{2});

    bool test7 = u == v;
    bool test8 = u != v;
    CHECK(test7);
    CHECK(!test8);

    bool test9 = u == w;
    bool test10 = u != w;
    CHECK(!test9);
    CHECK(test10);
}

