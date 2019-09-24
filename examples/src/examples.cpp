#include <iostream>
#include <acons/ndarray.hpp>
#include <cassert>

void wrapping_a_c_array()
{
    double a[] = {0,1,2,3,4,5,6};

    // Elements of a can be modified through this 2 x 3 interface
    acons::ndarray_view<double,2> v(a,2,3); 
    v(0,2) = 9;

    // Elements of a cannot be modified through this 2 x 3 interface
    acons::const_ndarray_view<double,2> cv(a,2,3);

    std::cout << cv << "\n\n";
}

void zeros()
{
    // Construct an array of dimensions 2 x 3 with zeros
    acons::ndarray<double,2> a(2,3,0.0);

    std::cout << a << "\n\n";
}

void example1()
{
    acons::ndarray<double,1> a = {0,1,2,3,4,5,6,7,8,9};

    // Extracting a part of the array with a slice object
    acons::ndarray_view<double,1> v(a, {acons::slice(2,7,2)});
    std::cout << v << "\n\n";
}
/*
[2,4,6]
*/

void example2()
{
    // Construct a 2-dimensional 2 x 2 array 
    acons::ndarray<double,1> a = {0,1,2,3,4,5,6,7,8,9};

    std::cout << a(5) << "\n\n";

    // 5
}

void example3()
{
    // Construct a 2-dimensional 2 x 2 array 
    acons::ndarray<double,1> a = {0,1,2,3,4,5,6,7,8,9};

    acons::ndarray_view<double, 1> v(a, { acons::slice(2) });
    std::cout << v << "\n\n";

    // [2,3,4,5,6,7,8,9]
}

void example4()
{
    // Construct a 2-dimensional 2 x 2 array 
    acons::ndarray<double,1> a = {0,1,2,3,4,5,6,7,8,9};

    acons::ndarray_view<double, 1> v(a, { acons::slice(2,5) });
    std::cout << v << "\n\n";

    // [2,3,4]
}

void example5()
{
    // Construct a 2-dimensional 3 x 3 array 
    acons::ndarray<double,2> a = {{1,2,3},{3,4,5},{4,5,6}};

    acons::ndarray_view<double, 2> v(a, { acons::slice(1) });
    std::cout << v << "\n\n";

    // [[3,4,5],[4,5,6]]
}

void example6()
{
    // Construct a 2-dimensional 3 x 3 array 
    acons::ndarray<double,2> a = {{1,2,3},{3,4,5},{4,5,6}};

    // Slice all items from the second row
    acons::ndarray_view<double, 1> v(a, acons::indices_t<1>{1});
    std::cout << v << "\n\n";

    // [3 4 5]
}

void slicing_a_2d_array()
{
    // Construct a 2-dimensional 3 x 4 array 
    acons::ndarray<double,2> a = {{0,1,2,3},{4,5,6,7},{8,9,10,11}};

    // All items from row 1 and columns 0 and 1
    acons::ndarray_view<double,2> v1(a, {acons::slice(1,2),acons::slice(0,2)});
    std::cout << "(1) " << v1 << "\n\n";

    // All items from column 1 onwards
    acons::ndarray_view<double,2> v2(a, {acons::slice(),acons::slice(1)});
    std::cout << "(2) " << v2 << "\n\n";
}
/*
*/

void reduction()
{
    // Construct a 2-dimensional 3 x 4 array 
    acons::ndarray<double,2> a = {{0,1,2,3},{4,5,6,7},{8,9,10,11}};

    // Reduce the 2-dimensional array to a 1-dimensional array, along row 0
    acons::ndarray_view<double,1> v1(a, acons::indices_t<1>{0});
    std::cout << "(1) " << v1 << "\n\n";

    // Reduce the 2-dimensional array to a 1-dimensional array, along column 2
    acons::ndarray_view<double,1> v2(a, {acons::slice()}, acons::indices_t<1>{2});
    std::cout << "(2) " << v2 << "\n\n";
}

void reduction3()
{
    // Construct a 3-dimensional 2 x 3 x 4 array 
    typedef acons::ndarray<double,3> array_t;
    array_t a = { {{0,1,2,3},{4,5,6,7},{8,9,10,11}}, {{12,13,14,15},{16,17,18,19},{20,21,22,23}} };

    // Reduce the 3-dimensional array to a 2-dimensional view, along leftmost index 1
    array_t::view<2> v1(a, acons::indices_t<1>{1});
    std::cout << "(1) " << v1 << "\n\n";

    // Reduce the 2-dimensional view to a 1-dimensional view, along rightmost index 2
    array_t::view<1> v2(v1, {acons::slice()}, acons::indices_t<1>{2});
    std::cout << "(2) " << v2 << "\n\n";
}

/*
*/

void row_major_zero_based_example()
{
    // Construct a 3-dimensional 2 x 3 x 4 array 
    acons::ndarray<double, 3> a = {{{0,1,2,3},{4,5,6,7},{8,9,10,11}}, 
                            {{12,13,14,15},{16,17,18,19},{20,21,22,23}}};

    std::cout << "(1)\n" << a << "\n\n";

    // Construct a 3-dimensional 1 x 2 x 2 view on the array.
    // The second argument provides a list of index ranges 
    // on the three dimensions of the array.
    acons::ndarray_view<double,3> v(a, {acons::slice(1,2),acons::slice(1,3),acons::slice(0,4,2)});

    std::cout << "(2)\n" << v << "\n\n";

    // Construct a 2-dimensional 3 x 4 slice from the array
    acons::ndarray_view<double,2> s(a, acons::indices_t<1>{1});

    std::cout << "(3)\n" << s << "\n\n";

    // Change one of the view's elements
    s(1,0) = 99;

    // Verify that the change is reflected in the original array
    assert (a(1, 1, 0) = 99);
}

void column_major_one_based_example()
{
    // Construct a 3-dimensional 2 x 3 x 4 array 
    acons::ndarray<double, 3, acons::column_major, acons::one_based> a = {{{0,1,2,3},{4,5,6,7},{8,9,10,11}}, 
                                                     {{12,13,14,15},{16,17,18,19},{20,21,22,23}}};

    std::cout << "(1)\n" << a << "\n\n";

    // Construct a 3-dimensional 1 x 2 x 2 view on the array.
    // The second argument provides a list of index ranges 
    // on the three dimensions of the array.
    acons::ndarray_view<double, 3, acons::column_major, acons::one_based> v(a, {acons::slice(2,3),acons::slice(2,4),acons::slice(1,5,2)});

    std::cout << "(2)\n" << v << "\n\n";

    // Construct a 2-dimensional 3 x 4 slice from the array
    acons::ndarray_view<double, 2, acons::column_major, acons::one_based> s(a, acons::indices_t<1>{2});

    std::cout << "(3)\n" << s << "\n\n";

    // Change one of the view's elements
    s(2,1) = 99;

    // Verify that the change is reflected in the original array
    assert (a(2, 2, 1) = 99);
}

void shrink_array_example()
{
    // Construct a 2-dimensional 2 x 2 array 
    acons::ndarray<double,2> a = {{0, 1}, {2, 3}};

    // Shrink to 2 x 1
    a.resize(acons::extents_t<2>(2,1));

    std::cout << a << "\n\n";
}

void enlarge_array_example()
{
    // Construct a 2-dimensional 2 x 2 array 
    acons::ndarray<double,2> a = {{0, 1}, {2, 3}};

    // Enlarge to 2 x 3
    a.resize(acons::extents_t<2>{2,3});

    std::cout << a << "\n\n";
}

void create_a_3d_array()
{
    // Create a 3-dim array of shape 3 x 4 x 2
    acons::ndarray<double,3> a(3,4,2);

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

    reduction3();

    zeros();

    wrapping_a_c_array();

    std::cout << "---\n";
}
