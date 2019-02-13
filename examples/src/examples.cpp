#include <iostream>
#include <acons/ndarray.hpp>
#include <cassert>

using namespace acons;

void example1()
{
    ndarray<double,1> a = {0,1,2,3,4,5,6,7,8,9};

    ndarray_view<double, 1> v(a, { slice(2,7,2)});
    std::cout << v << "\n\n";

    // [2,4,6]
}

void example2()
{
    // Construct a 2-dimensional 2 x 2 array 
    ndarray<double,1> a = {0,1,2,3,4,5,6,7,8,9};

    std::cout << a(5) << "\n\n";

    // 5
}

void example3()
{
    // Construct a 2-dimensional 2 x 2 array 
    ndarray<double,1> a = {0,1,2,3,4,5,6,7,8,9};

    ndarray_view<double, 1> v(a, { slice(2) });
    std::cout << v << "\n\n";

    // [2,3,4,5,6,7,8,9]
}

void example4()
{
    // Construct a 2-dimensional 2 x 2 array 
    ndarray<double,1> a = {0,1,2,3,4,5,6,7,8,9};

    ndarray_view<double, 1> v(a, { slice(2,5) });
    std::cout << v << "\n\n";

    // [2,3,4]
}

void example5()
{
    // Construct a 2-dimensional 3 x 3 array 
    ndarray<double,2> a = {{1,2,3},{3,4,5},{4,5,6}};

    ndarray_view<double, 2> v(a, { slice(1) });
    std::cout << v << "\n\n";

    // [[3,4,5],[4,5,6]]
}

void example6()
{
    // Construct a 2-dimensional 3 x 3 array 
    ndarray<double,2> a = {{1,2,3},{3,4,5},{4,5,6}};

    // Slice all items from the second row
    ndarray_view<double, 1> v(a, {1});
    std::cout << v << "\n\n";

    // [3 4 5]
}

void example7()
{
    // Construct a 2-dimensional 3 x 3 array 
    ndarray<double,2> a = {{1,2,3},{4,5,6},{7,8,9}};

    std::cout << "The (1,2) element\n";
    std::cout << a(1,2) << "\n\n";

    std::cout << "All items from the second row\n";
    ndarray_view<double,1> v1(a, {1});
    std::cout << v1 << "\n\n";

    std::cout << "All items from the second column\n";
    ndarray_view<double,1> v2(a, {slice()}, {1});
    std::cout << v2 << "\n\n";

    std::cout << "All items from column 1 onwards\n";
    ndarray_view<double,2> v3(a, {slice(),slice(1)});
    std::cout << v3 << "\n\n";
}

/*
The (1,2) element
6
 
All items from the second row
[4,5,6]

All items from the second column
[2,5,8]

All items from column 1 onwards
[[2,3],[5,6],[8,9]]
*/

void row_major_zero_based_example()
{
    // Construct a 3-dimensional 2 x 3 x 4 array 
    ndarray<double, 3> a = {{{0,1,2,3},{4,5,6,7},{8,9,10,11}}, 
                            {{12,13,14,15},{16,17,18,19},{20,21,22,23}}};

    std::cout << "(1)\n" << a << "\n\n";

    // Construct a 3-dimensional 1 x 2 x 2 view on the array.
    // The second argument provides a list of index ranges 
    // on the three dimensions of the array.
    ndarray_view<double,3> v(a, {slice(1,2),slice(1,3),slice(0,4,2)});

    std::cout << "(2)\n" << v << "\n\n";

    // Construct a 2-dimensional 3 x 4 slice from the array
    ndarray_view<double,2> s(a,{1});

    std::cout << "(3)\n" << s << "\n\n";

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
    ndarray_view<double, 3, column_major, one_based> v(a, {slice(2,3),slice(2,4),slice(1,5,2)});

    std::cout << "(2)\n" << v << "\n\n";

    // Construct a 2-dimensional 3 x 4 slice from the array
    ndarray_view<double, 2, column_major, one_based> s(a,{2});

    std::cout << "(3)\n" << s << "\n\n";

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
    example1();
    example2();
    example3();
    example4();
    example5();
    example6();
    example7();

    std::cout << "---\n";

    row_major_zero_based_example();
    column_major_one_based_example();
    shrink_array_example();
    enlarge_array_example();
}
