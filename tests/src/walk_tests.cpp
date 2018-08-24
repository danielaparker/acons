#include <catch/catch.hpp>
#include <iostream>
#include "acons/ndarray.hpp"
#include <sstream>

using namespace acons;

// Calculate start/stop/stride

template <size_t N, typename Callable>
void rwalk(std::vector<snapshot2>& stack, 
          const std::array<size_t,N>& dimensions, 
          const std::array<size_t,N>& strides, 
          const std::array<size_t,N>& offsets, 
          size_t start,
          size_t size,
          size_t i,
          Callable callable)
{
    if (i+1 < N)
    {
        if (offsets[i] > 0)
        {
            for (size_t j = 0; j <= i; ++j)
            {
                start += offsets[i]*strides[j];
                size_t len = dimensions[j];
                rwalk(stack,dimensions,strides,offsets,start,len,i+1,callable);
            }
        }
        else
        {
            size_t l = 1;
            for (size_t j = 0; j <= i; ++j)
            {
                l *= dimensions[j];
            }
            rwalk(stack,dimensions,strides,offsets,start,l,i+1,callable);
        }
    }
    else
    {
        std::cout << "rwalk N-1: " << size << ", dimensions[N-1]: " << dimensions[N-1] << "\n\n";
        start += offsets[N-1];
        size_t len = size * dimensions[N-1];
        size_t stride = strides[N-1];
        callable(start, start + len*stride, stride);
    }
}
#if 0
TEST_CASE("rwalk 1 dim row_major view")
{
    ndarray<double, 1, row_major> a = { 0,1,2,3 };
    ndarray_view<double, 1, row_major> v(a, {{1,3}});

    std::cout << "v: " << v << "\n\n";

    std::cout << "dimensions: " << v.dimensions() << "\n\n";
    std::cout << "strides: " << v.strides() << "\n\n";
    std::cout << "offsets: " << v.offsets() << "\n\n";

    auto f = [&](size_t start, size_t stop, size_t stride)
    {
        std::cout << "f: start: " << start << ", stop: " << stop << ", stride: " << stride << "\n\n"; 

        size_t offset = start;
        CHECK(*(v.data()+offset) == 1);
        offset += stride;
        CHECK(*(v.data()+offset) == 2);
        offset += stride;
        CHECK(offset == stop);
    };

    std::vector<snapshot2> stack;
    rwalk(stack,v.dimensions(),v.strides(),v.offsets(),0,1,0,f);
}

TEST_CASE("walk 1 dim row_major view")
{
    ndarray<double, 1, row_major> a = { 0,1,2,3 };
    ndarray_view<double, 1, row_major> v(a, {{1,3}});

    std::cout << "v: " << v << "\n\n";

    std::cout << "dimensions: " << v.dimensions() << "\n\n";
    std::cout << "strides: " << v.strides() << "\n\n";
    std::cout << "offsets: " << v.offsets() << "\n\n";

    auto f = [&](size_t start, size_t stop, size_t stride)
    {
        std::cout << "f: start: " << start << ", stop: " << stop << ", stride: " << stride << "\n\n"; 

        size_t offset = start;
        CHECK(*(v.data()+offset) == 1);
        offset += stride;
        CHECK(*(v.data()+offset) == 2);
        offset += stride;
        CHECK(offset == stop);
    };

    std::vector<snapshot2> stack;
    row_major::initialize_walk<1>(stack);
    row_major::walk(stack,v.dimensions(),v.strides(),v.offsets(),f);
}

TEST_CASE("walk 2x2 dim row_major view")
{
    ndarray<double, 2, row_major> a = { {0,1}, {2,3} };
    ndarray_view<double, 2, row_major> v(a);

    std::cout << "v: " << v << "\n\n";
    std::cout << "dimensions: " << v.dimensions() << "\n\n";
    std::cout << "strides: " << v.strides() << "\n\n";
    std::cout << "offsets: " << v.offsets() << "\n\n";

    auto f = [&](size_t start, size_t stop, size_t stride)
    {
        std::cout << "f: start: " << start << ", stop: " << stop << ", stride: " << stride << "\n\n"; 

        size_t offset = start;
        CHECK(*(v.data()+offset) == 0);
        offset += stride;
        CHECK(*(v.data()+offset) == 1);
        offset += stride;
        CHECK(*(v.data()+offset) == 2);
        offset += stride;
        CHECK(*(v.data()+offset) == 3);
        offset += stride;
        CHECK(offset == stop);
    };

    std::vector<snapshot2> stack;
    row_major::initialize_walk<2>(stack);
    row_major::walk(stack,v.dimensions(),v.strides(),v.offsets(),f);
}

TEST_CASE("rwalk 2x2 dim row_major view")
{
    ndarray<double, 2, row_major> a = { {0,1}, {2,3} };
    ndarray_view<double, 2, row_major> v(a);

    std::cout << "v: " << v << "\n\n";
    std::cout << "dimensions: " << v.dimensions() << "\n\n";
    std::cout << "strides: " << v.strides() << "\n\n";
    std::cout << "offsets: " << v.offsets() << "\n\n";

    auto f = [&](size_t start, size_t stop, size_t stride)
    {
        std::cout << "f: start: " << start << ", stop: " << stop << ", stride: " << stride << "\n\n"; 

        size_t offset = start;
        CHECK(*(v.data()+offset) == 0);
        offset += stride;
        CHECK(*(v.data()+offset) == 1);
        offset += stride;
        CHECK(*(v.data()+offset) == 2);
        offset += stride;
        CHECK(*(v.data()+offset) == 3);
        offset += stride;
        CHECK(offset == stop);
    };

    std::vector<snapshot2> stack;
    rwalk(stack,v.dimensions(),v.strides(),v.offsets(),0,1,0,f);
}

TEST_CASE("rwalk 2 dim row_major view")
{
    ndarray<double, 2, row_major> a = { {0,1,2,3}, {4,5,6,7}, {8,9,10,11} };
    ndarray_view<double, 2, row_major> v(a, {{1,2}, {0,4} });
    std::cout << "v: " << v << "\n\n";
    std::cout << "dimensions: " << v.dimensions() << "\n\n";
    std::cout << "strides: " << v.strides() << "\n\n";
    std::cout << "offsets: " << v.offsets() << "\n\n";

    auto f = [&](size_t start, size_t stop, size_t stride)
    {
        std::cout << "f: start: " << start << ", stop: " << stop << ", stride: " << stride << "\n\n"; 

        for (size_t offset = start; offset < stop; offset += stride)
        {
            if (offset != start)
            {
                std::cout << ",";
            }
            std::cout << *(v.data() + offset);
        }
        std::cout << "\n\n";
    };

    std::vector<snapshot2> stack;
    rwalk(stack,v.dimensions(),v.strides(),v.offsets(),0,1,0,f);
}

TEST_CASE("walk 2 dim row_major view")
{
    ndarray<double, 2, row_major> a = { {0,1,2,3}, {4,5,6,7}, {8,9,10,11} };
    ndarray_view<double, 2, row_major> v(a, {{1,2}, {0,4} });
    std::cout << "v: " << v << "\n\n";
    std::cout << "dimensions: " << v.dimensions() << "\n\n";
    std::cout << "strides: " << v.strides() << "\n\n";
    std::cout << "offsets: " << v.offsets() << "\n\n";

    auto f = [&](size_t start, size_t stop, size_t stride)
    {
        std::cout << "f: start: " << start << ", stop: " << stop << ", stride: " << stride << "\n\n"; 

        for (size_t offset = start; offset < stop; offset += stride)
        {
            if (offset != start)
            {
                std::cout << ",";
            }
            std::cout << *(v.data() + offset);
        }
        std::cout << "\n\n";
    };

    std::vector<snapshot2> stack;
    row_major::initialize_walk<2>(stack);
    row_major::walk(stack,v.dimensions(),v.strides(),v.offsets(),f);
}
#endif
TEST_CASE("walk 2x3x4 dim row major")
{
    ndarray<double, 3, row_major> a = { {{0,1,2,3},{4,5,6,7},{8,9,10,11}}, {{12,13,14,15},{16,17,18,19},{20,21,22,23}} };

    ndarray_view<double, 3> v(a,{{1,2},{1,3},{0,4,2}});

    std::cout << "v: " << v << "\n\n";
    std::cout << "dimensions: " << v.dimensions() << "\n\n";
    std::cout << "strides: " << v.strides() << "\n\n";
    std::cout << "offsets: " << v.offsets() << "\n\n";

    auto f = [&](size_t start, size_t stop, size_t stride)
    {
        std::cout << "f: start: " << start << ", stop: " << stop << ", stride: " << stride << "\n\n"; 

        for (size_t offset = start; offset < stop; offset += stride)
        {
            if (offset != start)
            {
                std::cout << ",";
            }
            std::cout << *(v.data() + offset);
        }
        std::cout << "\n\n";
    };

    std::vector<snapshot2> stack;
    row_major::initialize_walk<3>(stack);
    row_major::walk(stack,v.dimensions(),v.strides(),v.offsets(),f);
}

