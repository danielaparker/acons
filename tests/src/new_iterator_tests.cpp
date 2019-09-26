#include <catch/catch.hpp>
#include <iostream>
#include "acons/ndarray.hpp"
#include <sstream>

using namespace acons;

#if 0

struct state
{
    size_t first;
    size_t last;
    size_t current;

    state()
        : first(0), last(0), current(0)
    {

    }
};

std::ostream& operator<<(std::ostream& os, const state& s)
{
    os << s.first << " " << s.last << " " << s.current;
    return os;
}

template <size_t N>
void initialize(const std::array<size_t,N>& shape,
               const std::array<size_t,N>& strides,
               const std::array<size_t,N>& offsets,
               size_t n,
               std::array<state,N>& stack)
{
    std::cout << "iterate_n stack: " << stack << "\n\n";
    if (n+1 < N)
    {
        stack[n + 1].first = stack[n].first + offsets[n+1];
        stack[n + 1].last = stack[n + 1].first + strides[n+1]*shape[n+1];
        stack[n].current = stack[n].first;
        while (stack[n].current != stack[n].last)
        {
            iterate_n(shape, strides, offsets, n+1, stack);
            stack[n + 1].first += strides[n];
            stack[n + 1].last += strides[n];
            stack[n].current += strides[n];
        }
    }
    else
    {
        stack[n].current = stack[n].first;
    }
}

template <size_t N>
void iterate_n(const std::array<size_t,N>& shape,
               const std::array<size_t,N>& strides,
               const std::array<size_t,N>& offsets,
               size_t n,
               std::array<state,N>& stack)
{
    std::cout << "iterate_n stack 1: " << stack << "\n\n";
    if (n+1 < N) 
    { 
        stack[n + 1].first = stack[n].first + offsets[n+1];
        stack[n + 1].last = stack[n + 1].first + strides[n+1]*shape[n+1];
        stack[n + 1].current = stack[n].current + offsets[n+1];

        std::cout << "iterate_n stack 2: " << stack << "\n\n";
        while (stack[n].current != stack[n].last)
        {
            iterate_n(shape, strides, offsets, n+1, stack);
            stack[n + 1].first += strides[n];
            stack[n + 1].last += strides[n];
            stack[n].current += strides[n];
            stack[n + 1].current = stack[n].current + offsets[n+1];
        }
    }
    else
    {
        for (; stack[n].current != stack[n].last; stack[n].current += strides[n])
        {
            std::cout << stack[n].current << " ";
        }
        std::cout << "\n";
    }
}

TEST_CASE("4 x 5 new iterator tests")
{
    typedef ndarray<double,2> array_t;

    array_t a = {{0,1,2,3,4},{5,6,7,8,9},{10,11,12,13,14},{15,16,17,18,19}};
    std::cout << "a: " << a << "\n\n";

    const double* data = a.data();
    size_t size = a.size();

    SECTION("2x2 view")
    {
        constexpr size_t N = 2;

        array_t::view<N> v(a, { slice(1,3),slice(2,4) });

        std::cout << "v: " << v << "\n";
        std::array<size_t,2> shape = v.shape();
        std::array<size_t,2> strides = v.strides();
        std::array<size_t,2> offsets = v.offsets();

        std::cout << "shape: " << shape << "\n";
        std::cout << "strides: " << strides << "\n";
        std::cout << "offsets: " << offsets << "\n";

        size_t n = 0;

        std::array<state,N> stack; 
        stack[0].first = offsets[n];
        stack[0].current = stack[0].first;
        stack[0].last = stack[0].first + strides[0]*shape[0];

        //initialize(shape, strides, offsets, n, stack);
        iterate_n(shape, strides, offsets, n, stack);
    }
}
#endif
