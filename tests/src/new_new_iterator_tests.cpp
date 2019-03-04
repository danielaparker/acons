#include <catch/catch.hpp>
#include <iostream>
#include "acons/ndarray.hpp"
#include <sstream>

using namespace acons;

struct iterator_state
{
    size_t first;
    size_t last;
    size_t current;
    size_t n;

    iterator_state()
        : first(0), last(0), current(0), n(0)
    {
    }

    iterator_state(size_t first, size_t last, size_t current)
        : first(first), last(last), current(current), n(0)
    {
    }

    iterator_state(size_t first, size_t last, size_t current, size_t n)
        : first(first), last(last), current(current), n(n)
    {
    }

    iterator_state(const iterator_state&) = default;

    friend std::ostream& operator<<(std::ostream& os, const iterator_state& s)
    {
        os << "first: " << s.first << ", last: " << s.last << ", current: " << s.current << ", n: " << s.n;
        return os;
    }
};

template <size_t N>
static void initialize_walk(std::vector<iterator_state>& stack,
                            const std::array<size_t,N>& shape,
                            const std::array<size_t,N>& strides,
                            const std::array<size_t,N>& offsets)
{
    stack.emplace_back(offsets[0], offsets[0]+strides[0]*shape[0], offsets[0]);
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

        std::vector<iterator_state> stack; 

        initialize_walk(stack,shape,strides,offsets);

        while (!stack.empty())
        {
            iterator_state top = stack.back();
            stack.pop_back();
            //std::cout << "current: " << top << "\n";
            if (top.n+1 < N)
            {
                if (top.current != top.last)
                {
                    iterator_state state(top.first,top.last,top.current+strides[top.n],top.n);
                    stack.push_back(state);
                    ++top.n;
                    top.first = top.current + offsets[top.n];
                    top.last = top.first + strides[top.n]*shape[top.n];
                    top.current = top.last - strides[top.n];

                    while (true)
                    {
                        //std::cout << "push: " << top << "\n";
                        stack.push_back(top);
                        if (top.current == top.first)
                        {
                            break;
                        }
                        top.current -= strides[top.n];
                    };
                }
            }
            else
            {
                std::cout << top << "\n";
            }
        }

    }
}

