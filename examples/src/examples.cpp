#include <acons/ndarray.hpp>
#include <iostream>
#include <cassert>

using namespace acons;

void row_major_zero_based_example()
{
    // Construct a 3-dimensional 2 x 3 x 4 array 
    ndarray<double, 3> a = {{{0,1,2,3},{4,5,6,7},{8,9,10,11}}, 
                            {{12,13,14,15},{16,17,18,19},{20,21,22,23}}};

    std::cout << "(1)\n" << a << "\n\n";

    // Construct a 3-dimensional 1 x 2 x 2 view on the array.
    // The second argument provides a list of index ranges 
    // on the three dimensions of the array.
    ndarray_view<double,3> v(a, {{1,2},{1,3},{0,4,2}});

    std::cout << "(2)\n" << v << "\n\n";

    // Construct a 2-dimensional 3 x 4 slice from the array
    ndarray_view<double,2> s(a,{1});

    std::cout << "(3)\n" << s << "\n\n";

    // Iterate over the 2-dimensional view
    auto begin = s.begin();
    auto end = s.end();

    std::cout << "(4)\n";

    for (auto it = begin; it != end; ++it)
    {
        if (it != begin)
        {
            std::cout << ",";
        }
        std::cout << *it;
    }
    std::cout << "\n\n";

    // Change one of the view's elements
    s(1,0) = 99;

    // Verify that the change is reflected in the original array
    assert (a(1, 1, 0) = 99);
}

void column_major_one_based_example()
{
    // Construct a 3-dimensional 2 x 3 x 4 array 
    ndarray<double, 3, column_major, one_based> a = {{{0,1,2,3},{4,5,6,7},{8,9,10,11}}, 
                                                     {{12,13,14,15},{16,17,18,19},{20,21,22,23}}};

    std::cout << "(1)\n" << a << "\n\n";

    // Construct a 3-dimensional 1 x 2 x 2 view on the array.
    // The second argument provides a list of index ranges 
    // on the three dimensions of the array.
    ndarray_view<double, 3, column_major, one_based> v(a, {{2,3},{2,4},{1,5,2}});

    std::cout << "(2)\n" << v << "\n\n";

    // Construct a 2-dimensional 3 x 4 slice from the array
    ndarray_view<double, 2, column_major, one_based> s(a,{2});

    std::cout << "(3)\n" << s << "\n\n";

    // Iterate over the 2-dimensional view
    auto begin = s.begin();
    auto end = s.end();

    std::cout << "(4)\n";

    for (auto it = begin; it != end; ++it)
    {
        if (it != begin)
        {
            std::cout << ",";
        }
        std::cout << *it;
    }
    std::cout << "\n\n";

    // Change one of the view's elements
    s(2,1) = 99;

    // Verify that the change is reflected in the original array
    assert (a(2, 2, 1) = 99);
}

void shrink_array_example()
{
    // Construct a 2-dimensional 2 x 2 array 
    ndarray<double,2> a = {{0, 1}, {2, 3}};

    // Shrink to 2 x 1
    a.resize({2,1});

    std::cout << a << "\n\n";
}

void enlarge_array_example()
{
    // Construct a 2-dimensional 2 x 2 array 
    ndarray<double,2> a = {{0, 1}, {2, 3}};

    // Enlarge to 2 x 3
    a.resize({2,3});

    std::cout << a << "\n\n";
}

int main()
{
    row_major_zero_based_example();
    column_major_one_based_example();
    shrink_array_example();
    enlarge_array_example();
}
