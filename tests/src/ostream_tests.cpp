#include <catch/catch.hpp>
#include "acons/ndarray.hpp"
#include <iostream>
#include <sstream>

using namespace acons;

TEST_CASE("2x2x2 ostream test")
{
    ndarray<double, 3, row_major> a = { {{0,1},{2,3}}, {{4,5},{6,7}} };
    CHECK(a.size(0) == 2);
    CHECK(a.size(1) == 2);
    CHECK(a.size(2) == 2);

    CHECK(a(0,0,0) == 0);
    CHECK(a(0,0,1) == 1);
    CHECK(a(0,1,0) == 2);
    CHECK(a(0,1,1) == 3);

    CHECK(a(1,0,0) == 4);
    CHECK(a(1,0,1) == 5);
    CHECK(a(1,1,0) == 6);
    CHECK(a(1,1,1) == 7);

    std::ostringstream os;
    os << a;
    CHECK(os.str() == std::string("[[[0,1],[2,3]],[[4,5],[6,7]]]"));
}

TEST_CASE("ostream test")
{
    ndarray<double,1,row_major> a0 = {1.0,2.0};
    std::ostringstream os0;
    os0 << a0;
    CHECK(os0.str() == std::string("[1,2]"));

    ndarray<double,1,row_major> a1 = {1.0,2.0,3.0,4.0};
    std::ostringstream os1;
    os1 << a1;
    CHECK(os1.str() == std::string("[1,2,3,4]"));

    ndarray<double,2,row_major> a2 = {{1.0,2.0,3.0,4.0},{5.0,6.0,7.0,8.0}};
    std::ostringstream os2;
    os2 << a2;
    CHECK(os2.str() == std::string("[[1,2,3,4],[5,6,7,8]]"));

    ndarray<double,3,row_major> a3 = {{{1.0,2.0},{3.0,4.0}},{{5.0,6.0},{7.0,8.0}}};
    CHECK(a3(0, 0, 0) == 1.0);
    CHECK(a3(0, 0, 1) == 2.0);
    CHECK(a3(0, 1, 0) == 3.0);
    CHECK(a3(0, 1, 1) == 4.0);
    CHECK(a3(1, 0, 0) == 5.0);
    CHECK(a3(1, 0, 1) == 6.0);
    CHECK(a3(1, 1, 0) == 7.0);
    CHECK(a3(1, 1, 1) == 8.0);

    std::ostringstream os3;
    os3 << a3;
    CHECK(os3.str() == std::string("[[[1,2],[3,4]],[[5,6],[7,8]]]"));
}

