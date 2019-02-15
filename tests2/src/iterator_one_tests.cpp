#include <catch/catch.hpp>
#include <iostream>
#include "acons/ndarray.hpp"
#include <sstream>

using namespace acons;

TEST_CASE("1-dim ndarray ndarray_view iterator tests")
{
    ndarray<double, 1, row_major> a = {0,1,2,3,4,5};
    REQUIRE(a.size(0) == 6);

    SECTION("const_ndarray_view slice(1,3)")
    {
        const_ndarray_view<double, 1, row_major> v(a, {slice(1,3)});
        REQUIRE(v.size(0) == 2);
        CHECK(v(0) == 1);
        CHECK(v(1) == 2);

        auto it = v.begin();
        CHECK((*it++ == 1));
        CHECK((*it++ == 2));
        CHECK((it == v.end()));
    }

    SECTION("const_ndarray_view slice(1,5,2)")
    {
        const_ndarray_view<double, 1, row_major> v(a, {slice(1,5,2)});
        REQUIRE(v.size(0) == 2);
        CHECK(v(0) == 1);
        CHECK(v(1) == 3);

        auto it = v.begin();
        CHECK((*it++ == 1));
        CHECK((*it++ == 3));
        CHECK((it == v.end()));
    }

    SECTION("ndarray_view slice(1,3)")
    {
        ndarray_view<double, 1, row_major> v(a, {slice(1,3)});
        REQUIRE(v.size(0) == 2);
        CHECK(v(0) == 1);
        CHECK(v(1) == 2);

        auto it = v.begin();
        CHECK((*it++ == 1));
        CHECK((*it++ == 2));
        CHECK((it == v.end()));
    }
    SECTION("ndarray_view slice(1,5,2)")
    {
        ndarray_view<double, 1, row_major> v(a, {slice(1,5,2)});
        REQUIRE(v.size(0) == 2);
        CHECK(v(0) == 1);
        CHECK(v(1) == 3);

        auto it = v.begin();
        CHECK((*it++ == 1));
        CHECK((*it++ == 3));
        CHECK((it == v.end()));
    }
    SECTION("ndarray_view slice(1,5,2) mutate")
    {
        ndarray_view<double, 1, row_major> v(a, {slice(1,5,2)});
        REQUIRE(v.size(0) == 2);
        CHECK(v(0) == 1);
        CHECK(v(1) == 3);

        auto it = v.begin();
        *it = 8;
        CHECK((*it++ == 8));
        *it = 9;
        CHECK((*it++ == 9));
        CHECK((it == v.end()));
    }

}

TEST_CASE("2-dim ndarray ndarray_view iterator tests")
{
    ndarray<double,2> a = {{1.0,2.0,3.0,4.0},{5.0,6.0,7.0,8.0},{9.0,10.0,11.0,12.0}};

    REQUIRE(a.size(0) == 3);
    REQUIRE(a.size(1) == 4);

    SECTION("ndarray_view row 0 slice(2,4)")
    {
        ndarray_view<double,1> v(a,{0},{slice(2,4)});

        REQUIRE(v.size(0) == 2); 
        CHECK(v(0) == 3.0); 
        CHECK(v(1) == 4.0); 

        auto it = v.begin();
        CHECK((*it++ == 3.0));
        CHECK((*it++ == 4.0));
        CHECK((it == v.end()));
    }
    SECTION("ndarray_view row 1 slice(2,4)")
    {
        ndarray_view<double,1> v(a,{1},{slice(2,4)});

        REQUIRE(v.size(0) == 2); 
        CHECK(v(0) == 7.0); 
        CHECK(v(1) == 8.0); 

        auto it = v.begin();
        CHECK((*it++ == 7.0));
        CHECK((*it++ == 8.0));
        CHECK((it == v.end()));
    }
}
TEST_CASE("2-dim column_major ndarray ndarray_view iterator tests")
{
    ndarray<double,2,column_major> a = {{1.0,2.0,3.0,4.0},{5.0,6.0,7.0,8.0},{9.0,10.0,11.0,12.0}};

    REQUIRE(a.size(0) == 3);
    REQUIRE(a.size(1) == 4);
    SECTION("ndarray_view row 0 slice(2,4)")
    {
        ndarray_view<double,1,column_major> v(a,{0},{slice(2,4)});

        REQUIRE(v.size(0) == 2); 
        //std::cout << "v(0)\n";
        CHECK(v(0) == 3.0); 
        //std::cout << "end v(0)\n";
        CHECK(v(1) == 4.0); 

        auto it = v.begin();
        CHECK((*it++ == 3.0));
        CHECK((*it++ == 4.0));
        CHECK((it == v.end()));
    }
    SECTION("ndarray_view row 0 slice(2,4)")
    {
        ndarray_view<double,1,column_major> v(a,{0},{slice(1,5,2)});

        REQUIRE(v.size(0) == 2); 
        //std::cout << "v(0)\n";
        CHECK(v(0) == 2.0); 
        //std::cout << "end v(0)\n";
        CHECK(v(1) == 4.0); 

        auto it = v.begin();
        CHECK((*it++ == 2.0));
        CHECK((*it++ == 4.0));
        CHECK((it == v.end()));
    }
}

