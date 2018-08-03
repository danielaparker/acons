#ifndef ACONS_NDARRAY_HPP
#define ACONS_NDARRAY_HPP

#include <memory>
#include <array>
#include <vector>
#include <typeinfo>
#include <stdarg.h>  
#include <assert.h>
#include <algorithm>
#include <initializer_list>
  
namespace acons {

template <typename T, size_t N, typename Order>
class ndarray;

struct row_major
{
    template <size_t N>
    static void calculate_strides(const std::array<size_t,N>& dim, std::array<size_t,N>& strides, size_t& size)
    {
        size = 1;
        for (size_t i = 0; i < N; ++i)
        {
            strides[N-i-1] = size;
            size *= dim[N-i-1];
        }
    }
};

struct column_major
{
    template <size_t N>
    static void calculate_strides(const std::array<size_t,N>& dim, std::array<size_t,N>& strides, size_t& size)
    {
        size = 1;
        for (size_t i = 0; i < N; ++i)
        {
            strides[i] = size;
            size *= dim[i];
        }
    }
};

template <size_t n,size_t m>
typename std::enable_if<m == n, size_t>::type
    get_offset(const std::array<size_t,n>& strides) 
{
    return 0;
}

template <size_t n,size_t m>
typename std::enable_if<m+1 == n, size_t>::type
    get_offset(const std::array<size_t,n>& strides, size_t index) 
{
    return index*strides[n-1];
}

template <size_t n,size_t m,typename... Indices>
typename std::enable_if<(m+1 < n), size_t>::type
    get_offset(const std::array<size_t,n>& strides, size_t index, Indices... indices)
{
    const size_t mplus1 = m + 1;
    size_t i = index*strides[m] + get_offset<n,mplus1>(strides,indices...);

    return i;
}

// ndarray

template <class T>
struct array_item
{
    typedef typename std::vector<array_item<T>>::iterator iterator;
    typedef typename std::vector<array_item<T>>::const_iterator const_iterator;

    bool is_array_;
    std::vector<array_item<T>> v_;
    T val_;

    array_item(const std::vector<array_item<T>>& a)
        : is_array_(true), v_(a), val_(0)
    {
    }

    array_item(std::initializer_list<array_item<T>> list)
        : is_array_(true), v_(list), val_(0)
    {
    }
    array_item(T val)
        : is_array_(false), val_(val)
    {
    }

    array_item() 
        : is_array_(false)
    {
    }
    array_item(const array_item&)  = default;
    array_item(array_item&&)  = default;

    bool is_array() const
    {
        return is_array_;
    }

    size_t size() const
    {
        return v_.size();
    }

    T value() const
    {
        return val_;
    }

    iterator begin()
    {
        return v_.begin();
    }

    iterator end()
    {
        return v_.end();
    }

    const_iterator begin() const
    {
        return v_.begin();
    }

    const_iterator end() const
    {
        return v_.end();
    }
};

template <typename T, size_t N, typename Order=row_major>
class ndarray 
{
    template<size_t Pos>
    struct init_helper
    {
        using next = init_helper<Pos - 1>;

        template <typename... Args>
        static void init(ndarray<T,N,Order>& a, size_t n, Args... args)
        {
            a.dim_[N-Pos] = n;
            next::init(a, args...);
        }
    };

    template<>
    struct init_helper<0>
    {
        static void init(ndarray<T,N,Order>& a)
        {
            a.init();
        }
        static void init(ndarray<T,N,Order>& a, const T& val)
        {
            a.init(val);
        }
    };

    std::vector<T> data_;
    std::array<size_t,N> dim_;
    std::array<size_t,N> strides_;
public:
    typedef T* iterator;
    typedef const T* const_iterator;

    ndarray()
    {
    }
    ndarray(const ndarray& a)
        : data_(a.data_), dim_(a.dim_), strides_(a.strides_)
    {
    }

    ndarray(ndarray&& a)
        : data_(std::move(a.data_)), dim_(std::move(a.dim_)), strides_(std::move(a.strides_)) 
    {
    }

    template <typename... Args>
    ndarray(size_t k, Args... args)
    {
        init_helper<N>::init(*this, k, args ...);
    }

    ndarray(const std::array<size_t,N>& a)
    {
        init();
    }

    ndarray(const std::array<size_t,N>& dim, T val)
    {
        init();
    }

    ndarray(std::array<size_t,N>&& dim)
    {
        init();
    }

    ndarray(std::array<size_t,N>&& dim, T val)
    {
        init();
    }

    ndarray(std::initializer_list<array_item<T>> list) 
    {
        bool is_array = false;

        size_t i = 0;
        size_t dim = 0;
        for (const auto& item : list)
        {
            if (i == 0)
            {
                is_array = item.is_array();
                if (dim < N)
                {
                    this->dim_[dim++] = list.size();
                    if (is_array)
                    {
                        init_dim(item, dim);
                    }
                }
            }
            else
            {
                if (is_array != item.is_array())
                {
                    throw std::runtime_error("Invalid initializer list");
                }
            }
            ++i;
        }

        // Initialize multipliers and size
        init();

        {
            size_t i = 0;
            for (const auto& item : list)
            {
                if (item.is_array())
                {
                    init_data(item,i);
                }
                else 
                {
                    if (i < data_.size())
                    {
                        data_[i] = item.value();
                        ++i;
                    }
                }
            }
        }
    }

    size_t size() const
    {
        return data_.size();
    }

    std::array<size_t,N> dimensions() const {return dim_;}
    std::array<size_t,N> strides() {return strides_;}

    T* data()
    {
        return data_.data();
    }

    const T* data() const 
    {
        return data_.data();
    }

    size_t size(size_t i) const
    {
        assert(i < dim_.size());
        return dim_[i];
    }

    template <size_t n=N, typename... Indices>
    typename std::enable_if<(n == N),T&>::type 
    operator()(Indices... indices) 
    {
        size_t off = get_offset<n,0>(strides_,indices...);
        assert(off < size());
        return data_[off];
    }

    template <size_t n=N, typename... Indices>
    typename std::enable_if<(n == N),const T&>::type 
    operator()(Indices... indices) const
    {
        size_t off = get_offset<n,0>(strides_,indices...);
        assert(off < size());
        return data_[off];
    }

    template <size_t n=N-1, typename... Indices>
    typename std::enable_if<(n == N-1),iterator>::type 
    begin(Indices... indices) 
    {
        size_t off = get_offset<n,0>(strides_,indices...);
        assert(off < size());
        return &data_[off];
    }

    template <size_t n=N-1, typename... Indices>
    typename std::enable_if<(n == N-1),iterator>::type 
    end(Indices... indices) 
    {
        size_t off = get_offset<n,0>(strides_,indices...);
        assert(off < size());
        return &data_[off] + dim_[n];
    }

    template <size_t n=N-1, typename... Indices>
    typename std::enable_if<(n == N-1),const_iterator>::type 
    begin(Indices... indices) const
    {
        size_t off = get_offset<n,0>(strides_,indices...);
        assert(off < size());
        return &data_[off];
    }

    template <size_t n=N-1, typename... Indices>
    typename std::enable_if<(n == N-1),const_iterator>::type 
    end(Indices... indices) const
    {
        size_t off = get_offset<n,0>(strides_,indices...);
        assert(off < size());
        return &data_[off] + dim_[n];
    }
/*
    bool operator!=(const ndarray_base<T,N,Order>& rhs) const 
    {
        return !(*this == rhs);
    }
*/
    ndarray& operator=(const ndarray&) = default;
    ndarray& operator=(ndarray&&) = default;
private:
    void init()
    {
        size_t size;
        Order::calculate_strides(dim_, strides_, size);
        data_.resize(size);
    }

    void init(const T& val)
    {
        size_t size;
        Order::calculate_strides(dim_, strides_, size);
        data_.resize(size, val);
    }

    void init_dim(const array_item<T>& init, size_t& dim)
    {
        bool is_array = false;

        size_t i = 0;
        for (const auto& item : init)
        {
            if (i == 0)
            {
                is_array = item.is_array();
                if (dim < N)
                {
                    this->dim_[dim++] = init.size();
                    if (is_array)
                    {
                        init_dim(item, dim);
                    }
                }
            }
            else
            {
                if (is_array != item.is_array())
                {
                    throw std::runtime_error("Invalid initializer list");
                }
            }
            ++i;
        }
    }

    void init_data(const array_item<T>& init, size_t& i)
    {
        for (const auto& item : init)
        {
            if (item.is_array())
            {
                init_data(item,i);
            }
            else 
            {
                if (i < size())
                {
                    data_[i] = item.value();
                    ++i;
                }
            }
        }
    }
};

template <typename T, size_t M, typename Order=row_major>
class ndarray_view  
{
    T* data_;
    size_t size_;
    std::array<size_t,M> dim_;
    std::array<size_t,M> strides_;
public:
    typedef T* iterator;
    typedef const T* const_iterator;

    template<size_t m = M, size_t N>
    ndarray_view(ndarray<T,N,Order>& a, const std::array<size_t,N>& indices, const std::array<size_t,M>& dim, 
               typename std::enable_if<m <= N>::type* = 0)
        : dim_(dim)
    {
        size_t offset = 0;
        for (size_t i = 0; i < N; ++i)
        {
            offset += a.strides()[i]*indices[i];
        }

        data_ = a.data() + offset;
        size_ = a.size() - offset;

        for (size_t i = 0; i < M; ++i)
        {
            strides_[i] = a.strides()[i];
        }
    }

    ndarray_view(T* data, const std::array<size_t,M>& dim) 
        : dim_(dim)
    {
        data_ = data;
        Order::calculate_strides(dim_,strides_,size_);
    }

    size_t size() const
    {
        return size_;
    }

    std::array<size_t,M> dimensions() const {return dim_;}
    std::array<size_t,M> strides() {return strides_;}

    T* data()
    {
        return data_.data();
    }

    const T* data() const 
    {
        return data_.data();
    }

    size_t size(size_t i) const
    {
        assert(i < dim_.size());
        return dim_[i];
    }

    template <size_t n=M, typename... Indices>
    typename std::enable_if<(n == M),T&>::type 
    operator()(Indices... indices) 
    {
        size_t off = get_offset<n,0>(strides_,indices...);
        assert(off < size());
        return data_[off];
    }

    template <size_t n=M, typename... Indices>
    typename std::enable_if<(n == M),const T&>::type 
    operator()(Indices... indices) const
    {
        size_t off = get_offset<n,0>(strides_,indices...);
        assert(off < size());
        return data_[off];
    }

    template <size_t n=M-1, typename... Indices>
    typename std::enable_if<(n == M-1),iterator>::type 
    begin(Indices... indices) 
    {
        size_t off = get_offset<n,0>(strides_,indices...);
        assert(off < size());
        return &data_[off];
    }

    template <size_t n=M-1, typename... Indices>
    typename std::enable_if<(n == M-1),iterator>::type 
    end(Indices... indices) 
    {
        size_t off = get_offset<n,0>(strides_,indices...);
        assert(off < size());
        return &data_[off] + dim_[n];
    }

    template <size_t n=M-1, typename... Indices>
    typename std::enable_if<(n == M-1),const_iterator>::type 
    begin(Indices... indices) const
    {
        size_t off = get_offset<n,0>(strides_,indices...);
        assert(off < size());
        return &data_[off];
    }

    template <size_t n=M-1, typename... Indices>
    typename std::enable_if<(n == M-1),const_iterator>::type 
    end(Indices... indices) const
    {
        size_t off = get_offset<n,0>(strides_,indices...);
        assert(off < size());
        return &data_[off] + dim_[n];
    }
/*
    bool operator!=(const ndarray_base<T,M,Order>& rhs) const 
    {
        return !(*this == rhs);
    }
*/
};

}

#endif
