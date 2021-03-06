#include <acons/ndarray.hpp>
#include <iostream>
#include "boost/multi_array.hpp"
#include <cassert>

using namespace acons;

void acons_example()
{
    // Construct a 3-dimensional array with dimensions 2 x 3 x 4
    ndarray<double, 3> a(2,3,4);

    // Assign values to the elements
    int init = 0;
    for (size_t i = 0; i < a.shape(0); ++i)
    {
        for (size_t j = 0; j < a.shape(1); ++j)
        {
            for (size_t k = 0; k < a.shape(2); ++k)
            {
                a(i,j,k) = init++;
            }
        }
    }

    // Write to stdout
    std::cout << a << std::endl;

    std::cout << "dimensions: ";
    for (size_t i = 0; i < a.shape().size(); ++i)
    {
        if (i > 0)
        {
            std::cout << ",";
        }
        std::cout << a.shape()[i];
    }
    std::cout << "\n\n";

    std::cout << "strides: ";
    for (size_t i = 0; i < a.strides().size(); ++i)
    {
        if (i > 0)
        {
            std::cout << ",";
        }
        std::cout << a.strides()[i];
    }
    std::cout << "\n\n";

    ndarray_view<double,3> v(a,{slice(0,2),slice(1,3),slice(0,4,2)});

    std::cout << "dimensions: ";
    for (size_t i = 0; i < v.shape().size(); ++i)
    {
        if (i > 0)
        {
            std::cout << ",";
        }
        std::cout << v.shape()[i];
    }
    std::cout << "\n\n";

    std::cout << "strides: ";
    for (size_t i = 0; i < v.strides().size(); ++i)
    {
        if (i > 0)
        {
            std::cout << ",";
        }
        std::cout << v.strides()[i];
    }
    std::cout << "\n\n";

    std::cout << "offsets: ";
    for (size_t i = 0; i < v.offsets().size(); ++i)
    {
        if (i > 0)
        {
            std::cout << ",";
        }
        std::cout << v.offsets()[i];
    }
    std::cout << "\n\n";

    std::cout << "v(0,0,0): " << v(0,0,0) << "\n";
    std::cout << "v(0,0,1): " << v(0,0,1) << "\n";
    std::cout << "v(0,1,0): " << v(0,1,0) << "\n";
    std::cout << "v(0,1,1): " << v(0,1,1) << "\n";

    std::cout << "v(1,0,0): " << v(1,0,0) << "\n";
    std::cout << "v(1,0,1): " << v(1,0,1) << "\n";
    std::cout << "v(1,1,0): " << v(1,1,0) << "\n";
    std::cout << "v(1,1,1): " << v(1,1,1) << "\n";

    // Write to stdout
    std::cout << v << std::endl;

    // array_view dims: [base,bound) (ndim striding default = 1)
    // dim 0: [0,2) 
    // dim 1: [1,3) 
    // dim 2: [0,4) (strided by 2), 
    // 

}

void boost_example()
{
    // Create a 3D array that is 2 x 3 x 4
    typedef boost::multi_array<double, 3> array_type;
    typedef array_type::index index;
    array_type a(boost::extents_t[2][3][4]);

    // Assign values to the elements
    int values = 0;
    for(index i = 0; i != 2; ++i) 
        for(index j = 0; j != 3; ++j)
            for(index k = 0; k != 4; ++k)
                a[i][j][k] = values++;

    // Verify values
    int verify = 0;
    for(index i = 0; i != 2; ++i) 
    {
        for(index j = 0; j != 3; ++j)
        {
            for(index k = 0; k != 4; ++k)
            {
                if (k > 0)
                {
                    std::cout << ",";
                }
                std::cout << a[i][j][k];
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }

    typedef boost::multi_array_types::index_range range;
    array_type::array_view<3>::type v = a[ boost::indices[range(0,2)][range(1,3)][range(0,4,2)]];

    for (array_type::index i = 0; i != 2; ++i)
    {
        for (array_type::index j = 0; j != 2; ++j)
        {
            for (array_type::index k = 0; k != 2; ++k) 
            {
                assert(v[i][j][k] == a[i][j+1][k*2]);
            }
        }
    }

    // subarray
    //typedef boost::multi_array_types::index_range range;
    std::cout << "subarray\n";
    array_type::subarray<2>::type s = a[0];
    for (array_type::index i = 0; i < s.shape()[0]; ++i)
    {
        for (array_type::index j = 0; j < s.shape()[1]; ++j)
        {
            if (j > 0)
            {
                std::cout << ",";
            }
            std::cout << s[i][j];
        }
        std::cout << "\n";
    }
    std::cout << "\n";
/*
    for (array_type::index i = 0; i != 2; ++i)
    {
        for (array_type::index j = 0; j != 2; ++j)
        {
            for (array_type::index k = 0; k != 2; ++k) 
            {
                assert(v[i][j][k] == a[i][j+1][k*2]);
            }
        }
    }
*/
    for (array_type::index i = 0; i != 2; ++i)
    {
        for (array_type::index j = 0; j != 2; ++j)
        {
            for (array_type::index k = 0; k != 2; ++k) 
            {
                if (k > 0)
                {
                    std::cout << ",";
                }
                std::cout << v[i][j][k];
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }
}

void boost_example_fortran()
{
    // Create a 3D array that is 2 x 3 x 4
    typedef boost::multi_array<double, 3> array_type;
    typedef array_type::index index;
    array_type a(boost::extents_t[2][3][4],boost::fortran_storage_order());

    // Assign values to the elements
    int values = 0;
    for(index i = 0; i != 2; ++i) 
        for(index j = 0; j != 3; ++j)
            for(index k = 0; k != 4; ++k)
                a[i][j][k] = values++;

    // Verify values
    int verify = 0;
    for(index i = 0; i != 2; ++i) 
    {
        for(index j = 0; j != 3; ++j)
        {
            for(index k = 0; k != 4; ++k)
            {
                if (k > 0)
                {
                    std::cout << ",";
                }
                std::cout << a[i][j][k];
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }

    typedef boost::multi_array_types::index_range range;
    array_type::array_view<3>::type v = a[ boost::indices[range(0,2)][range(1,3)][range(0,4,2)]];

    for (array_type::index i = 0; i != 2; ++i)
    {
        for (array_type::index j = 0; j != 2; ++j)
        {
            for (array_type::index k = 0; k != 2; ++k) 
            {
                assert(v[i][j][k] == a[i][j+1][k*2]);
            }
        }
    }

    // subarray
    //typedef boost::multi_array_types::index_range range;
    std::cout << "subarray\n";
    array_type::subarray<2>::type s = a[0];
    for (array_type::index i = 0; i < s.shape()[0]; ++i)
    {
        for (array_type::index j = 0; j < s.shape()[1]; ++j)
        {
            if (j > 0)
            {
                std::cout << ",";
            }
            std::cout << s[i][j];
        }
        std::cout << "\n";
    }
    std::cout << "\n";
    for (array_type::index i = 0; i != 2; ++i)
    {
        for (array_type::index j = 0; j != 2; ++j)
        {
            for (array_type::index k = 0; k != 2; ++k) 
            {
                if (k > 0)
                {
                    std::cout << ",";
                }
                std::cout << v[i][j][k];
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }
}

void boost_example2()
{
    // Create a 2D array that is 2 x 3
    typedef boost::multi_array<double, 2> array_type;
    typedef array_type::index index;
    array_type a(boost::extents_t[2][3]);

    // Assign values to the elements
    int values = 0;
    for(index i = 0; i < 2; ++i) 
    {
        for(index j = 0; j < 3; ++j)
        {
                a[i][j] = values++;
        }
    }

    typedef boost::multi_array_types::index_range range;
    array_type::array_view<2>::type v = a[ boost::indices[range(0,1)][range(1,2)]];

    for (array_type::index i = 0; i < 1; ++i)
    {
        for (array_type::index j = 0; j < 1; ++j)
        {
            if (j > 0)
            {
                std::cout << ",";
            }
            std::cout << v[i][j];
        }
        std::cout << "\n";
    }
}

int main()
{
    std::cout << "acons" << "\n\n";
    acons_example();
    std::cout << "\n\nboost" << "\n\n";
    boost_example();
    std::cout << "\n\nboost 2" << "\n\n";
    boost_example_fortran();
    std::cout << "\n\nboost 2" << "\n\n";
    boost_example2();
}
