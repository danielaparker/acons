#include <iostream>
#include <acons/ndarray.hpp>
#include <cassert>

namespace ac = acons;

void wrapping_a_c_array()
{
    double a[] = {0,1,2,3,4,5,6};

    // Elements of a can be modified through this 2 x 3 interface
    ac::ndarray_view<double,2> v(a,2,3); 
    v(0,2) = 9;

    // Elements of a cannot be modified through this 2 x 3 interface
    ac::const_ndarray_view<double,2> cv(a,2,3);

    std::cout << cv << "\n\n";
}

void zeros()
{
    // Construct an array of dimensions 2 x 3 with zeros
    ac::ndarray<double,2> a(2,3,0.0);

    std::cout << a << "\n\n";
}

void example1()
{
    ac::ndarray<double,1> a = {0,1,2,3,4,5,6,7,8,9};

    // Extracting a part of the array with a slice object
    ac::ndarray_view<double,1> v(a, {ac::slice(2,7,2)});
    std::cout << v << "\n\n";
}
/*
[2,4,6]
*/

void example2()
{
    // Construct a 2-dimensional 2 x 2 array 
    ac::ndarray<double,1> a = {0,1,2,3,4,5,6,7,8,9};

    std::cout << a(5) << "\n\n";

    // 5
}

void example3()
{
    // Construct a 2-dimensional 2 x 2 array 
    ac::ndarray<double,1> a = {0,1,2,3,4,5,6,7,8,9};

    ac::ndarray_view<double, 1> v(a, { ac::slice(2) });
    std::cout << v << "\n\n";

    // [2,3,4,5,6,7,8,9]
}

void example4()
{
    // Construct a 2-dimensional 2 x 2 array 
    ac::ndarray<double,1> a = {0,1,2,3,4,5,6,7,8,9};

    ac::ndarray_view<double, 1> v(a, { ac::slice(2,5) });
    std::cout << v << "\n\n";

    // [2,3,4]
}

void example5()
{
    // Construct a 2-dimensional 3 x 3 array 
    ac::ndarray<double,2> a = {{1,2,3},{3,4,5},{4,5,6}};

    ac::ndarray_view<double, 2> v(a, { ac::slice(1) });
    std::cout << v << "\n\n";

    // [[3,4,5],[4,5,6]]
}

void example6()
{
    // Construct a 2-dimensional 3 x 3 array 
    ac::ndarray<double,2> a = {{1,2,3},{3,4,5},{4,5,6}};

    // Slice all items from the second row
    ac::ndarray_view<double, 1> v(a, {1});
    std::cout << v << "\n\n";

    // [3 4 5]
}

void slicing_a_2d_array()
{
    // Construct a 2-dimensional 3 x 4 array 
    ac::ndarray<double,2> a = {{0,1,2,3},{4,5,6,7},{8,9,10,11}};

    // All items from row 1 and columns 0 and 1
    ac::ndarray_view<double,2> v1(a, {ac::slice(1,2),ac::slice(0,2)});
    std::cout << "(1) " << v1 << "\n\n";

    // All items from column 1 onwards
    ac::ndarray_view<double,2> v2(a, {ac::slice(),ac::slice(1)});
    std::cout << "(2) " << v2 << "\n\n";
}
/*
*/

void reduction()
{
    // Construct a 2-dimensional 3 x 4 array 
    ac::ndarray<double,2> a = {{0,1,2,3},{4,5,6,7},{8,9,10,11}};

    // Reduce the 2-dimensional array to a 1-dimensional array, along row 0
    ac::ndarray_view<double,1> v1(a, {0});
    std::cout << "(1) " << v1 << "\n\n";

    // Reduce the 2-dimensional array to a 1-dimensional array, along column 2
    ac::ndarray_view<double,1> v2(a, {ac::slice()}, {2});
    std::cout << "(2) " << v2 << "\n\n";
}

void reduction3()
{
    // Construct a 3-dimensional 2 x 3 x 4 array 
    typedef ac::ndarray<double,3> array_t;
    array_t a = { {{0,1,2,3},{4,5,6,7},{8,9,10,11}}, {{12,13,14,15},{16,17,18,19},{20,21,22,23}} };

    // Reduce the 3-dimensional array to a 2-dimensional view, along leftmost index 1
    array_t::view<2> v1(a, {1});
    std::cout << "(1) " << v1 << "\n\n";

    // Reduce the 2-dimensional view to a 1-dimensional view, along rightmost index 2
    array_t::view<1> v2(v1, {ac::slice()}, {2});
    std::cout << "(2) " << v2 << "\n\n";
}

/*
*/

void row_major_zero_based_example()
{
    // Construct a 3-dimensional 2 x 3 x 4 array 
    ac::ndarray<double, 3> a = {{{0,1,2,3},{4,5,6,7},{8,9,10,11}}, 
                            {{12,13,14,15},{16,17,18,19},{20,21,22,23}}};

    std::cout << "(1)\n" << a << "\n\n";

    // Construct a 3-dimensional 1 x 2 x 2 view on the array.
    // The second argument provides a list of index ranges 
    // on the three dimensions of the array.
    ac::ndarray_view<double,3> v(a, {ac::slice(1,2),ac::slice(1,3),ac::slice(0,4,2)});

    std::cout << "(2)\n" << v << "\n\n";

    // Construct a 2-dimensional 3 x 4 slice from the array
    ac::ndarray_view<double,2> s(a,{1});

    std::cout << "(3)\n" << s << "\n\n";

    // Change one of the view's elements
    s(1,0) = 99;

    // Verify that the change is reflected in the original array
    assert (a(1, 1, 0) = 99);
}

void column_major_one_based_example()
{
    // Construct a 3-dimensional 2 x 3 x 4 array 
    ac::ndarray<double, 3, ac::column_major, ac::one_based> a = {{{0,1,2,3},{4,5,6,7},{8,9,10,11}}, 
                                                     {{12,13,14,15},{16,17,18,19},{20,21,22,23}}};

    std::cout << "(1)\n" << a << "\n\n";

    // Construct a 3-dimensional 1 x 2 x 2 view on the array.
    // The second argument provides a list of index ranges 
    // on the three dimensions of the array.
    ac::ndarray_view<double, 3, ac::column_major, ac::one_based> v(a, {ac::slice(2,3),ac::slice(2,4),ac::slice(1,5,2)});

    std::cout << "(2)\n" << v << "\n\n";

    // Construct a 2-dimensional 3 x 4 slice from the array
    ac::ndarray_view<double, 2, ac::column_major, ac::one_based> s(a,{2});

    std::cout << "(3)\n" << s << "\n\n";

    // Change one of the view's elements
    s(2,1) = 99;

    // Verify that the change is reflected in the original array
    assert (a(2, 2, 1) = 99);
}

void shrink_array_example()
{
    // Construct a 2-dimensional 2 x 2 array 
    ac::ndarray<double,2> a = {{0, 1}, {2, 3}};

    // Shrink to 2 x 1
    a.resize({2,1});

    std::cout << a << "\n\n";
}

void enlarge_array_example()
{
    // Construct a 2-dimensional 2 x 2 array 
    ac::ndarray<double,2> a = {{0, 1}, {2, 3}};

    // Enlarge to 2 x 3
    a.resize({2,3});

    std::cout << a << "\n\n";
}

void create_a_3d_array()
{
    // Create a 3-dim array of shape 3 x 4 x 2
    ac::ndarray<double,3> a(3,4,2);

    // Assign values to the elements
    int x = 0;
    for (size_t i = 0; i < a.shape(0); ++i)
    {
        for (size_t j = 0; j < a.shape(1); ++j)
        {
            for (size_t k = 0; k < a.shape(2); ++k)
            {
                a(i,j,k) = x++;
            }
        }
    }

    // Print
    std::cout << a << "\n";
}

void row_major_iterator_example()
{
    // The third template parameter in ndarray indicates row 
    // major (C-style) storage ordering, this is the default
    typedef ac::ndarray<double,2,ac::row_major> array_t;

    // Construct a 2-dimensional 3 x 4 row-major array 
    array_t a = {{0,1,2,3},{4,5,6,7},{8,9,10,11}};
    std::cout << "(1) " << a << "\n\n";

    // All items from row 1 and columns 1 through 2
    array_t::view<2> v(a, {ac::slice(1,2),ac::slice(1,3)});
    std::cout << "(2) " << v << "\n\n";

    std::cout << "(3) ";
    for (auto it = a.begin(); it != a.end(); ++it)
    {
        if (it != a.begin())
        {
            std::cout << ",";
        }
        std::cout << *it;
    }
    std::cout << "\n\n";

    std::cout << "(4) ";
    for (auto it = v.begin(); it != v.end(); ++it)
    {
        if (it != v.begin())
        {
            std::cout << ",";
        }
        std::cout << *it;
    }
    std::cout << "\n\n"; 
}

void column_major_iterator_example()
{
    // The third template parameter in ndarray indicates column 
    // major (FORTRAN-style) storage ordering
    typedef ac::ndarray<double,2,ac::column_major> array_t;

    // Construct a 2-dimensional 3 x 4 column-major array 
    array_t a = {{0,1,2,3},{4,5,6,7},{8,9,10,11}};
    std::cout << "(1) " << a << "\n\n";

    // All items from row 2 and columns 0 and 2
    array_t::view<2> v(a, {ac::slice(2,3),ac::slice(0,4,2)});
    std::cout << "(2) " << v << "\n\n";

    std::cout << "(3) ";
    for (auto it = a.begin(); it != a.end(); ++it)
    {
        if (it != a.begin())
        {
            std::cout << ",";
        }
        std::cout << *it;
    }
    std::cout << "\n\n";

    std::cout << "(4) ";
    for (auto it = v.begin(); it != v.end(); ++it)
    {
        if (it != v.begin())
        {
            std::cout << ",";
        }
        std::cout << *it;
    }
    std::cout << "\n\n";
}

void row_major_and_column_major_iterator_example()
{
    typedef ac::ndarray<double,2> array_t;

    // Construct a 2-dimensional 3 x 4 row-major array 
    array_t a = {{0,1,2,3},{4,5,6,7},{8,9,10,11}};

    // All items from rows 1 and 2 
    array_t::view<2> v(a, {ac::slice(1,3),ac::slice()});
    std::cout << "(1) " << v << "\n\n";

    std::cout << "(2) ";
    auto first_by_row = make_row_major_iterator(v);
    auto last_by_row = end(first_by_row);
    for (auto it = first_by_row; it != last_by_row; ++it)
    {
        if (it != first_by_row)
        {
            std::cout << ",";
        }
        std::cout << *it;
    }
    std::cout << "\n\n";

    std::cout << "(3) ";
    auto first_by_column = make_column_major_iterator(v);
    auto last_by_column = end(first_by_column);
    for (auto it = first_by_column; it != last_by_column; ++it)
    {
        if (it != first_by_column)
        {
            std::cout << ",";
        }
        std::cout << *it;
    }

    std::cout << "\n\n"; 
}

int main()
{
    example1();
    example2();
    example3();
    example4();
    example5();
    example6();

    row_major_zero_based_example();
    column_major_one_based_example();
    shrink_array_example();
    enlarge_array_example();

    create_a_3d_array();

    slicing_a_2d_array();
    reduction();

    row_major_iterator_example();
    column_major_iterator_example();
    row_major_and_column_major_iterator_example();

    reduction3();

    zeros();

    wrapping_a_c_array();

    std::cout << "---\n";
}
