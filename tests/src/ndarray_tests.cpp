#include <catch/catch.hpp>
#include <iostream>
#include "acons/ndarray.hpp"
#include <iostream>

using namespace acons;

template <class T>
struct MyAlloc
{
    using value_type = T;
    using size_type = std::size_t;
    using propagate_on_container_move_assignment = std::true_type;

    MyAlloc(int) {}

    template< class U >
    MyAlloc(const MyAlloc<U>&) noexcept {}

    T* allocate(size_type n)
    {
        return static_cast<T*>(::operator new(n * sizeof(T)));
    }

    void deallocate(T* ptr, size_type) noexcept
    {
        ::operator delete(ptr);
    }

    bool operator==(const MyAlloc&) const { return true; }
    bool operator!=(const MyAlloc&) const { return false; }
};

TEST_CASE("Array constructor tests")
{
    ndarray<size_t, 2> a(3, 2, 7);
    ndarray<size_t, 2, row_major, zero_based, MyAlloc<size_t>> b(std::allocator_arg, MyAlloc<size_t>(1), 3, 2);

    SECTION("initial value")
    {
        CHECK(a(0, 0) == 7);
        CHECK(a(1, 0) == 7);
        CHECK(a(2, 0) == 7);
        CHECK(a(0, 1) == 7);
        CHECK(a(1, 1) == 7);
        CHECK(a(2, 1) == 7);
    }

    SECTION("Allocator")
    {
    }
}

TEST_CASE("2D Array 4")
{
    ndarray<size_t, 2> a(3, 2);

    a(0,0) = 6;
    CHECK(a(0, 0) == 6);

}

TEST_CASE("3D Array")
{
    ndarray<double,3> a(2, 3, 4, 1.0);
    CHECK(a.shape(0) == 2);
    CHECK(a.shape(1) == 3);
    CHECK(a.shape(2) == 4);

    for (size_t i = 0; i < a.shape(0); ++i)
    {
        for (size_t j = 0; j < a.shape(1); ++j)
        {
            for (size_t k = 0; k < a.shape(2); ++k)
            {
                CHECK(a(i,j,k) == 1.0);
            }
        }
    }
}

TEST_CASE("TestArrayInitializerList")
{
    ndarray<double,1> a = {1.0,2.0,3.0,4.0};
    CHECK(a.shape(0) == 4);

}

TEST_CASE("TestArray2DInitializerList")
{
    ndarray<double,2> a = {{1.0,2.0,3.0,4.0},{5.0,6.0,7.0,8.0}};

    CHECK(a.shape(0) == 2);
    CHECK(a.shape(1) == 4);
}

TEST_CASE("Test Array View 1")
{
    ndarray<double,2> a = {{1.0,2.0,3.0,4.0},{5.0,6.0,7.0,8.0}};

    CHECK(a.shape(0) == 2);
    CHECK(a.shape(1) == 4);

    CHECK(a(1,1) == 6.0);

    ndarray_view<double,1> v(a,{1},{slice(1,3)});

    REQUIRE(v.shape(0) == 2); 
    CHECK(v(0) == 6.0); 
    CHECK(v(1) == 7.0); 
}

TEST_CASE("Test ndarray_view 2")
{
    ndarray<double,2> a = {{1.0,2.0,3.0,4.0},{5.0,6.0,7.0,8.0},{9.0,10.0,11.0,12.0}};

    CHECK(a.shape(0) == 3);
    CHECK(a.shape(1) == 4);

    ndarray_view<double,1> v(a,{0},{slice(2,4)});

    REQUIRE(v.shape(0) == 2); 
    CHECK(v(0) == 3.0); 
    CHECK(v(1) == 4.0); 
}

TEST_CASE("Test Array View")
{
    std::vector<double> a = {0.0,1.0,2.0,3.0,4.0,5.0};

    ndarray_view<double,2> v(&a[0],{2,3});

    CHECK(v(0,0) == 0.0); 
    CHECK(v(0,1) == 1.0); 
    CHECK(v(0,2) == 2.0); 
    CHECK(v(1,0) == 3.0); 
    CHECK(v.shape(0) == 2); 
    CHECK(v.shape(1) == 3); 
}

