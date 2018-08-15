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
#include <functional>
#include <stdexcept>
#include <type_traits>
#include <iterator>
  
namespace acons {

template <typename T, size_t N>
class array_wrapper
{
    std::array<T,N> a_;
public:
    array_wrapper() = default;

    array_wrapper(const array_wrapper<T,N>& a) = default;

    array_wrapper(std::initializer_list<T> list)
    {
        if (list.size() != N)
        {
            throw std::invalid_argument("N-dimensional array requires N items.");
        }
        size_t i = 0;
        for (auto item : list)
        {
            a_[i++] = item;
        }
    }

    size_t size() const
    {
        return N;
    }

    T operator[](size_t i) const
    {
        return a_[i];
    }
};

class slice
{
    size_t start_;
    size_t size_;
    size_t stride_;
public:
    slice()
        : start_(0), size_(0), stride_(0)
    {
    }
    slice(size_t start, size_t size, size_t stride)
        : start_(start), size_(size), stride_(stride)
    {
    }
    slice(std::initializer_list<size_t> list)
    {
        if (list.size() <= 3)
        {
            if (list.size() < 2)
            {
                throw std::invalid_argument("Start and size are required");
            }
            auto it = list.begin();
            start_ = *it++;
            size_ = *it++;
            stride_ = list.size() == 3 ? *it : 1;
        }
        else
        {
            throw std::invalid_argument("Too many arguments to slice");
        }
    }
    slice(const slice& other)
        : start_(other.start_), size_(other.size_), stride_(other.stride_)
    {
    }

    slice& operator=(const slice& other) = default;

    size_t start() const
    {
        return start_;
    }
    size_t size() const
    {
        return size_;
    }
    size_t stride() const
    {
        return stride_;
    }
};

template <typename T>
struct is_stateless
 : public std::integral_constant<bool,  
      (std::is_default_constructible<T>::value &&
#if defined(__GNUC__) && (__GNUC__ < 5)
#else
       std::is_trivially_constructible<T>::value &&
       std::is_trivially_copyable<T>::value &&
       std::is_class<T>::value &&
#endif
       std::is_trivially_destructible<T>::value &&
       std::is_empty<T>::value)>
{};

template <size_t n, typename Base, size_t m>
typename std::enable_if<m == n, size_t>::type
get_offset(const std::array<size_t,n>& strides) 
{
    return 0;
}

template <size_t n, typename Base, size_t m>
typename std::enable_if<m+1 == n, size_t>::type
get_offset(const std::array<size_t,n>& strides, 
           size_t index) 
{
    return Base::rebase_to_zero(index)*strides[n-1];
}

template <size_t n, typename Base, size_t m, typename... Indices>
typename std::enable_if<(m+1 < n), size_t>::type
get_offset(const std::array<size_t,n>& strides, 
           size_t index, Indices... indices)
{
    const size_t mplus1 = m + 1;
    size_t i = Base::rebase_to_zero(index)*strides[m] + get_offset<n, Base, mplus1>(strides,indices...);

    return i;
}

template <size_t N, size_t M, typename Base>
typename std::enable_if<M <= N,size_t>::type
get_offset(const std::array<size_t,N>& strides, 
           const std::array<size_t,M>& indices)
{
    size_t offset = 0;
    for (size_t i = 0; i < M; ++i)
    {
        offset += Base::rebase_to_zero(indices[i])*strides[i];
    }

    return offset;
}

template <size_t n, typename Base, size_t m>
typename std::enable_if<m == n, size_t>::type
get_offset(const std::array<size_t,n>& strides,
           const std::array<size_t,n>& offsets) 
{
    return 0;
}

template <size_t n, typename Base, size_t m>
typename std::enable_if<m+1 == n, size_t>::type
get_offset(const std::array<size_t,n>& strides,
           const std::array<size_t,n>& offsets, 
           size_t index) 
{
    return Base::rebase_to_zero(index+offsets[n-1])*strides[n-1];
}

template <size_t n, typename Base, size_t m, typename... Indices>
typename std::enable_if<(m+1 < n), size_t>::type
get_offset(const std::array<size_t,n>& strides, 
           const std::array<size_t, n>& offsets, 
           size_t index, Indices... indices)
{
    const size_t mplus1 = m + 1;
    size_t i = Base::rebase_to_zero((index+offsets[m])*strides[m]) + get_offset<n, Base, mplus1>(strides,offsets,indices...);

    return i;
}

template <size_t N, size_t M, typename Base>
typename std::enable_if<M <= N,size_t>::type
get_offset(const std::array<size_t,N>& strides, 
           const std::array<size_t, N>& offsets, 
           const std::array<size_t,M>& indices)
{
    size_t offset = 0;
    for (size_t i = 0; i < M; ++i)
    {
        offset += Base::rebase_to_zero(indices[i]+offsets[i])*strides[i];
    }

    return offset;
}

struct zero_based
{
    static size_t rebase_to_zero(size_t index)
    {
        return index;
    }
};

struct one_based
{
    static size_t rebase_to_zero(size_t index)
    {
        return index - 1;
    }
};

template <size_t N>
struct output_item
{
    static const size_t close_bracket = N+1;
    static const size_t insert_comma = N+2;

    std::array<size_t, N> indices;
    size_t index;

    output_item()
        : index(0)
    {
    }

    output_item(const output_item<N>& item)
        : indices(item.indices), index(item.index)
    {

    }

    output_item(const std::array<size_t,N>& x, size_t i)
        : indices(x), index(i)
    {
    }

    output_item(size_t i)
        : index(i)
    {
    }

};

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

    template <typename T, size_t N>
    static bool compare(const T* data1, const std::array<size_t,N>& dim1, const std::array<size_t,N>& strides1, const std::array<size_t,N>& offsets1, 
                        const T* data2, const std::array<size_t,N>& dim2, const std::array<size_t,N>& strides2, const std::array<size_t,N>& offsets2)
    {
        for (size_t i = 0; i < N; ++i)
        {
            if (dim1[i] != dim2[i])
            {
                return false;
            }
        }

        size_t stack_depth = 1;
        for (size_t i = 0; i+1 < dim1.size(); ++i)
        {
            stack_depth *= dim1[i];
        }
        std::vector<output_item<N>> stack(stack_depth);

        size_t count = 1;
        stack[0].index = 0;
        while (count != 0)
        {
            auto val = stack[count-1];
            --count;

            if (val.index+1 < N)
            {
                for (size_t i = dim1[val.index]; i-- > 0; )
                {
                    val.indices[val.index] = i; 
                    stack[count].indices = val.indices;
                    stack[count].index = val.index+1;
                    count++;
                }
            }
            else if (val.index+1 == N)
            {
                val.indices[val.index] = 0;
                size_t offset1 = get_offset<N,N,zero_based>(strides1, offsets1, val.indices);
                size_t offset2 = get_offset<N,N,zero_based>(strides2, offsets2, val.indices);
                const T* p1 = data1 + offset1;
                const T* p2 = data2 + offset2;
                size_t stride1 = strides1[N-1];
                size_t stride2 = strides2[N-1];

                val.indices[val.index] = dim1[val.index]-1;
                size_t end_offset1 = get_offset<N,N,zero_based>(strides1, offsets1, val.indices);
                const T* end = data1 + end_offset1;

                while (p1 <= end)
                {
                    if (*p1 != *p2)
                    {
                        return false;
                    }
                    p1 += stride1;
                    p2 += stride2;
                }
            }
        }
        return true;
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

    template <typename T, size_t N>
    static bool compare(const T* data1, const std::array<size_t,N>& dim1, const std::array<size_t,N>& strides1, const std::array<size_t,N>& offsets1, 
                        const T* data2, const std::array<size_t,N>& dim2, const std::array<size_t,N>& strides2, const std::array<size_t,N>& offsets2)
    {
        for (size_t i = 0; i < N; ++i)
        {
            if (dim1[i] != dim2[i])
            {
                return false;
            }
        }

        size_t stack_depth = 1;
        for (size_t i = 0; i+1 < dim1.size(); ++i)
        {
            stack_depth *= dim1[i];
        }
        std::vector<output_item<N>> stack(stack_depth);

        size_t count = 1;
        stack[0].index = N-1;
        while (count != 0)
        {
            auto val = stack[count-1];
            --count;

            if (val.index > 0)
            {
                for (size_t i = dim1[val.index]; i-- > 0; )
                {
                    val.indices[val.index] = i; 
                    stack[count].indices = val.indices;
                    stack[count].index = val.index-1;
                    count++;
                }
            }
            else 
            {
                val.indices[val.index] = 0;
                size_t offset1 = get_offset<N,N,zero_based>(strides1, offsets1, val.indices);
                size_t offset2 = get_offset<N,N,zero_based>(strides2, offsets2, val.indices);
                const T* p1 = data1 + offset1;
                const T* p2 = data2 + offset2;
                //const T* end = p1 + dim1[val.index];
                size_t stride1 = strides1[val.index]; 
                size_t stride2 = strides2[val.index]; 
                val.indices[val.index] = dim1[val.index] - 1;
                size_t end_offset1 = get_offset<N,N,zero_based>(strides1, offsets1, val.indices);
                const T* end = data1 + end_offset1;
                while (p1 <= end)
                {
                    if (*p1 != *p2)
                    {
                        return false;
                    }
                    p1 += stride1;
                    p2 += stride2;
                }
            }
        }
        return true;
    }
};

template <typename T, size_t N, typename Order=row_major, typename Base=zero_based, typename Allocator=std::allocator<T>>
class ndarray;

template <typename T, size_t M, typename Order=row_major, typename Base=zero_based>
class ndarray_view;  

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

// ndarray_base

template <class Allocator>
class ndarray_base
{
protected:
    Allocator allocator_;
public:
    typedef Allocator allocator_type;

    allocator_type get_allocator() const
    {
        return allocator_;
    }
protected:
    ndarray_base(const Allocator& alloc)
        : allocator_(alloc)
    {
    }
};

template<size_t Pos>
struct init_helper
{
    using next = init_helper<Pos - 1>;

    template <typename Array, typename... Args>
    static void init(std::array<size_t,Array::dimension>& dim, Array& a, size_t n, Args... args)
    {
        dim[Array::dimension - Pos] = n;
        next::init(dim, a, args...);
    }
};

template<>
struct init_helper<0>
{
    template <typename Array>
    static void init(std::array<size_t, Array::dimension>& dim, Array& a)
    {
        a.init();
    }
    template <typename Array>
    static void init(std::array<size_t, Array::dimension>& dim, Array& a, typename Array::const_reference val)
    {
        a.init(val);
    }
};

template <typename T, size_t N, typename Order, typename Base, typename Allocator>
class ndarray : public ndarray_base<Allocator>
{
    friend struct init_helper<0>;
    friend class ndarray_view<T, N, Order, Base>;

    T* data_;
    size_t size_;
    std::array<size_t,N> dim_;
    std::array<size_t,N> strides_;
public:
    using typename ndarray_base<Allocator>::allocator_type;
    using ndarray_base<Allocator>::get_allocator;

    typedef T value_type;
    typedef const T& const_reference;
    static const size_t dimension = N;

    typedef T* iterator;
    typedef const T* const_iterator;

    ndarray()
        : ndarray_base<Allocator>(allocator_type()),
          data_(nullptr), size_(0)
    {
        dim_.fill(0);
        strides_.fill(0);
    }

    template <typename... Args>
    ndarray(size_t i, Args... args)
        : ndarray_base<Allocator>(allocator_type()) 
    {
        init_helper<N>::init(dim_, *this, i, args ...);
    }

    explicit ndarray(const std::array<size_t,N>& dim)
        : ndarray_base<Allocator>(allocator_type()), 
          data_(nullptr), dim_(dim)
    {
        Order::calculate_strides(dim_, strides_, size_);
        data_ = get_allocator().allocate(size_);
    }

    ndarray(const std::array<size_t,N>& dim, const Allocator& alloc)
        : ndarray_base<Allocator>(alloc), 
          data_(nullptr), dim_(dim)
    {
        Order::calculate_strides(dim_, strides_, size_);
        data_ = get_allocator().allocate(size_);
    }

    ndarray(const std::array<size_t,N>& dim, T val)
        : ndarray_base<Allocator>(allocator_type()), 
          data_(nullptr), dim_(dim)
    {
        Order::calculate_strides(dim_, strides_, size_);
        data_ = get_allocator().allocate(size_);
        std::fill(data_, data_+size_,val);
    }

    ndarray(const std::array<size_t,N>& dim, T val, const Allocator& alloc)
        : ndarray_base<Allocator>(alloc), 
          data_(nullptr), dim_(dim)
    {
        Order::calculate_strides(dim_, strides_, size_);
        data_ = get_allocator().allocate(size_);
        std::fill(data_, data_+size_,val);
    }

    ndarray(std::initializer_list<array_item<T>> list) 
        : ndarray_base<Allocator>(allocator_type())
    {
        dim_from_initializer_list(list, 0);

        Order::calculate_strides(dim_, strides_, size_);
        data_ = get_allocator().allocate(size_);
        std::array<size_t,N> indices;
        data_from_initializer_list(list,indices,0);
    }

    ndarray(std::initializer_list<array_item<T>> list, const Allocator& alloc) 
        : ndarray_base<Allocator>(alloc)
    {
        dim_from_initializer_list(list, 0);

        // Initialize multipliers and size
        Order::calculate_strides(dim_, strides_, size_);
        data_ = get_allocator().allocate(size_);
        std::array<size_t,N> indices;
        data_from_initializer_list(list,indices,0);
    }

    ndarray(const ndarray& a)
        : ndarray_base<Allocator>(a.get_allocator()), 
          data_(nullptr), size_(0), dim_(a.dim_), strides_(a.strides_)          
    {
        size_ = a.size();
        data_ = get_allocator().allocate(a.size());

#if defined(_MSC_VER)
        std::copy(a.data_, a.data_+a.size_,stdext::make_checked_array_iterator(data_,size_));
#else 
        std::copy(a.data_,a.data_+a.size_,data_);
#endif
    }

    ndarray(const ndarray& a, const Allocator& alloc)
        : ndarray_base<Allocator>(alloc), 
          data_(nullptr), size_(0), dim_(a.dim_), strides_(a.strides_)          
    {
        size_ = a.size();
        data_ = get_allocator().allocate(a.size());

#if defined(_MSC_VER)
        std::copy(a.data_, a.data_+a.size_,stdext::make_checked_array_iterator(data_,size_));
#else 
        std::copy(a.data_,a.data_+a.size_,data_);
#endif
    }

    ndarray(const ndarray_view<T,N,Order,Base>& a)
        : ndarray_base<Allocator>(allocator_type()), 
          data_(nullptr), size_(0), dim_(a.dimensions()), strides_(a.strides())          
    {
        size_ = a.size();
        data_ = get_allocator().allocate(a.size());

#if defined(_MSC_VER)
        std::copy(a.data(), a.data()+a.size(),stdext::make_checked_array_iterator(data_,size_));
#else 
        std::copy(a.data(),a.data()+a.size(),data_);
#endif
    }

    ndarray(const ndarray_view<T,N,Order,Base>& a, const Allocator& alloc)
        : ndarray_base<Allocator>(alloc), 
          data_(nullptr), size_(0), dim_(a.dim_), strides_(a.strides_)          
    {
        size_ = a.size();
        data_ = get_allocator().allocate(a.size());

#if defined(_MSC_VER)
        std::copy(a.data_, a.data_+a.size_,stdext::make_checked_array_iterator(data_,size_));
#else 
        std::copy(a.data_,a.data_+a.size_,data_);
#endif
    }

    ndarray(ndarray&& a)
        : ndarray_base<Allocator>(a.get_allocator()), 
          data_(a.data_), size_(a.size_), dim_(a.dim_), strides_(a.strides_)          
    {
        a.data_ = nullptr;
        a.size_ = 0;
        a.dim_.fill(0);
        a.strides_.fill(0);
    }

    ndarray(ndarray&& a, const Allocator& alloc)
        : ndarray_base<Allocator>(alloc), 
          data_(a.data_), size_(a.size_), dim_(a.dim_), strides_(a.strides_)          
    {
        a.data_ = nullptr;
        a.size_ = 0;
        a.dim_.fill(0);
        a.strides_.fill(0);
    }

    ~ndarray()
    {
        get_allocator().deallocate(data_,size_);
    }

    ndarray& operator=(const ndarray<T,N,Order,Base,Allocator>& other)
    {
        if (&other != this)
        {
            assign_copy(other,
                   typename std::allocator_traits<Allocator>::propagate_on_container_copy_assignment());
        }
        return *this;
    }

    ndarray& operator=(ndarray<T,N,Order,Base,Allocator>&& other) noexcept
    {
        if (&other != this)
        {
            assign_move(std::forward<ndarray>(other),
                   typename std::allocator_traits<Allocator>::propagate_on_container_move_assignment());
        }
        return *this;
    }

    ndarray& operator=(std::initializer_list<array_item<T>> list)
    {
        get_allocator().deallocate(data_,size_);
        dim_from_initializer_list(list, 0);

        Order::calculate_strides(dim_, strides_, size_);
        data_ = get_allocator().allocate(size_);
        std::array<size_t,N> indices;
        data_from_initializer_list(list,indices,0);
        return *this;
    }

    void swap(ndarray<T,N,Order,Base,Allocator>& other) noexcept
    {
        swap_allocator(other, is_stateless<Allocator>(), 
                       typename std::allocator_traits<allocator_type>::propagate_on_container_swap());
        std::swap(data_,other.data_);
        std::swap(size_,other.size_);
        std::swap(dim_,other.dim_);
        std::swap(strides_,other.strides_);

    }
    size_t size() const noexcept
    {
        return size_;
    }

    bool empty() const noexcept
    {
        return size_ == 0;
    }

    const std::array<size_t,N>& dimensions() const {return dim_;}

    const std::array<size_t,N>& strides() const {return strides_;}

    T* data()
    {
        return data_;
    }

    const T* data() const 
    {
        return data_;
    }

    size_t size(size_t i) const
    {
        assert(i < dim_.size());
        return dim_[i];
    }

    template <typename... Indices>
    T& operator()(size_t index, Indices... indices) 
    {
        size_t off = get_offset<N, Base, 0>(strides_, index, indices...);
        assert(off < size());
        return data_[off];
    }

    template <typename... Indices>
    const T& operator()(size_t index, Indices... indices) const
    {
        size_t off = get_offset<N, Base, 0>(strides_, index, indices...);
        assert(off < size());
        return data_[off];
    }

    T& operator()(const std::array<size_t,N>& indices) 
    {
        size_t off = get_offset<N, N, Base>(strides_,indices);
        assert(off < size());
        return data_[off];
    }

    const T& operator()(const std::array<size_t,N>& indices) const 
    {
        size_t off = get_offset<N, N, Base>(strides_,indices);
        assert(off < size());
        return data_[off];
    }

    template <size_t n=N, size_t K, typename... Indices>
    typename std::enable_if<(K < n),ndarray_view<T,N-K,Order,Base>>::type 
    subarray(const std::array<size_t,K>& indices) 
    {
        return ndarray_view<T,N-K,Order,Base>(*this,indices);
    }

private:

    void assign_move(ndarray<T,N,Order,Base,Allocator>&& other, std::true_type) noexcept
    {
        swap(other);
    }

    void assign_move(ndarray<T,N,Order,Base,Allocator>&& other, std::false_type) noexcept
    {
        if (size() != other.size())
        {
            get_allocator().deallocate(data_,size_);
            size_ = other.size();
            data_ = get_allocator().allocate(other.size());
        }
        dim_ = other.dimensions();
        strides_ = other.strides();
#if defined(_MSC_VER)
        std::copy(other.data_, other.data_+other.size_,stdext::make_checked_array_iterator(data_,size_));
#else 
        std::copy(other.data_,other.data_+other.size_,data_);
#endif
    }

    void assign_copy(const ndarray<T,N,Order,Base,Allocator>& other, std::true_type)
    {
        this->allocator_ = other.get_allocator();
        get_allocator().deallocate(data_,size_);
        size_ = other.size();
        data_ = get_allocator().allocate(other.size());
        dim_ = other.dimensions();
        strides_ = other.strides();
#if defined(_MSC_VER)
        std::copy(other.data_, other.data_+other.size_,stdext::make_checked_array_iterator(data_,size_));
#else 
        std::copy(other.data_,other.data_+other.size_,data_);
#endif
    }

    void assign_copy(const ndarray<T,N,Order,Base,Allocator>& other, std::false_type)
    {
        if (size() != other.size())
        {
            get_allocator().deallocate(data_,size_);
            size_ = other.size();
            data_ = get_allocator().allocate(other.size());
        }
        dim_ = other.dimensions();
        strides_ = other.strides();
#if defined(_MSC_VER)
        std::copy(other.data_, other.data_+other.size_,stdext::make_checked_array_iterator(data_,size_));
#else 
        std::copy(other.data_,other.data_+other.size_,data_);
#endif
    }

    void swap_allocator(ndarray<T,N,Order,Base,Allocator>& other, std::true_type, std::true_type) noexcept
    {
        // allocator is stateless, no need to swap it
    }

    void swap_allocator(ndarray<T,N,Order,Base,Allocator>& other, std::false_type, std::true_type) noexcept
    {
        using std::swap;
        swap(this->allocator_,other.allocator_);
    }

    void swap_allocator(ndarray<T,N,Order,Base,Allocator>& other, std::true_type, std::false_type) noexcept
    {
        // allocator is stateless, no need to swap it
    }

    void swap_allocator(ndarray<T,N,Order,Base,Allocator>& other, std::false_type, std::false_type) noexcept
    {
        // Undefined behavior
    }

    void init()
    {
        Order::calculate_strides(dim_, strides_, size_);
        data_ = get_allocator().allocate(size_);
    }

    void init(const T& val)
    {
        Order::calculate_strides(dim_, strides_, size_);
        data_ = get_allocator().allocate(size_);
        std::fill(data_, data_+size_,val);
    }

    void dim_from_initializer_list(const array_item<T>& init, size_t dim)
    {
        bool is_array = false;
        size_t size = 0;

        size_t i = 0;
        for (const auto& item : init)
        {
            if (i == 0)
            {
                is_array = item.is_array();
                size = item.size();
                if (dim < N)
                {
                    dim_[dim++] = init.size();
                    if (is_array)
                    {
                        dim_from_initializer_list(item, dim);
                    }
                }
            }
            else
            {
                if (is_array)
                {
                    if (!item.is_array() || item.size() != size)
                    {
                        throw std::invalid_argument("initializer list contains non-conforming shapes");
                    }
                }
                else if (dim != N)
                {
                    throw std::invalid_argument("initializer list incompatible with array dimensionality");
                }
            }
            ++i;
        }
    }

    void data_from_initializer_list(const array_item<T>& init, std::array<size_t,N>& indices, size_t index)
    {
        size_t i = 0;
        for (const auto& item : init)
        {
            indices[index] = i;
            if (item.is_array())
            {
                data_from_initializer_list(item,indices,index+1);
            }
            else 
            {
                size_t offset = get_offset<N,N,zero_based>(strides_,indices);
                if (offset < size())
                {
                    data_[offset] = item.value();
                }
            }
            ++i;
        }
    }
};

template <typename CharT, size_t N, typename Getter>
void print(std::basic_ostream<CharT>& os, const std::array<size_t,N>& dimensions, Getter getter)
{
    std::vector<output_item<N>> stack;
    stack.emplace_back(0);
    while (!stack.empty())
    {
        auto val = stack.back();
        stack.pop_back();

        if (val.index+1 < N)
        {
            os << '[';
            stack.push_back(output_item<N>(output_item<N>::close_bracket)); 
            for (size_t i = dimensions[val.index]; i-- > 0; )
            {
                val.indices[val.index] = i; 
                stack.push_back(output_item<N>(val.indices,val.index+1)); 
                if (i > 0)
                {
                    stack.push_back(output_item<N>(output_item<N>::insert_comma)); 
                }
            }
        }
        else if (val.index+1 == N)
        {
            os << '[';
            for (size_t i = 0; i < dimensions[val.index]; ++i)
            {
                val.indices[val.index] = i; 
                if (i > 0)
                {
                    os << ',';
                }
                os << getter(val.indices);
            }
            os << ']';
        }
        else if (val.index == output_item<N>::close_bracket)
        {
            os << ']';
        }
        else if (val.index == output_item<N>::insert_comma)
        {
            os << ',';
        }
    }
}

template <typename T, size_t N, typename Order, typename Base, typename Allocator, typename CharT>
std::basic_ostream<CharT>& operator<<(std::basic_ostream<CharT>& os, ndarray<T, N, Order, Base, Allocator>& a)
{
    auto f = [&](const std::array<size_t,N>& indices) 
    { 
        return a(indices);
    };
    print(os, a.dimensions(), f);
    return os;
}


template <typename T, size_t N, typename Order, typename Enable = void>
class ndarray_view_iterator;

template <typename T, size_t N, typename Order>
class ndarray_view_iterator<T, N, Order, 
                            typename std::enable_if<N==1 && std::is_same<Order,row_major>::value>::type>
{
T* p_;
public:
    typedef ptrdiff_t difference_type;
    typedef T& reference;
    typedef T* pointer;
    typedef std::forward_iterator_tag iterator_category;

    ndarray_view_iterator()
        : p_(nullptr)
    {
    }

    ndarray_view_iterator(T* data)
        : p_(data)
    {
    }

    ndarray_view_iterator(const ndarray_view_iterator<T,N,Order>& val) 
        : p_(val.p_)
    {
    } 

    ndarray_view_iterator<T, N, Order>& operator++()
    {
        ++p_;
        return *this;
    }

    ndarray_view_iterator<T, N, Order>& operator++(int)
    {
        ndarray_view_iterator<T,N,Order> temp(*this);
        ++p_;
        return temp;
    }

    reference operator*() const
    {
        return *p_;
    }

    friend bool operator==(const ndarray_view_iterator<T,N,Order>& it1, const ndarray_view_iterator& it2)
    {
        return it1.p_ == it2.p_;
    }

    friend bool operator!=(const ndarray_view_iterator<T,N,Order>& it1, const ndarray_view_iterator& it2)
    {
        return !(it1 == it2);
    }
};

template <typename T, size_t N, typename Order>
class ndarray_view_iterator<T, N, Order, 
                            typename std::enable_if<N != 1 && std::is_same<Order,row_major>::value>::type>
{
T* data_;
public:
    ndarray_view_iterator(T* data)
        : data_(data)
    {
    }
};

template <typename T, size_t N, typename Order>
class ndarray_view_iterator<T, N, Order, 
    typename std::enable_if<N == 1 && std::is_same<Order,column_major>::value>::type>
{
public:
};

template <typename T, size_t N, typename Order>
class ndarray_view_iterator<T, N, Order, 
    typename std::enable_if<N != 1 && std::is_same<Order,column_major>::value>::type>
{
public:
};

template <typename T, size_t M, typename Order, typename Base>
class ndarray_view 
{
    T* data_;
    size_t size_;
    std::array<size_t,M> dim_;
    std::array<size_t,M> strides_;
    std::array<size_t,M> offsets_;
public:
    typedef T* iterator;
    typedef const T* const_iterator;
    typedef array_wrapper<slice,M> slices_type;

    ndarray_view()
        : data_(nullptr), size_(0)
    {
        dim_.fill(0);
        strides_.fill(0);
        offsets_.fill(0);
    }

    template <typename Allocator>
    ndarray_view(ndarray<T, M, Order, Base, Allocator>& a)
        : data_(a.data_), size_(a.size_), dim_(a.dim_), strides_(a.strides_)          
    {
        offsets_.fill(0);
    }

    template<size_t m = M, size_t N, typename Allocator>
    ndarray_view(ndarray<T, N, Order, Base, Allocator>& a, 
                 const slices_type& slices, 
               typename std::enable_if<m == N>::type* = 0)
        : data_(a.data()), size_(a.size())
    {
        for (size_t i = 0; i < M; ++i)
        {
            dim_[i] = slices[i].size()/slices[i].stride();
            strides_[i] = a.strides()[i]*slices[i].stride();
            offsets_[i] = Base::rebase_to_zero(slices[i].start()); // a.strides()[i];
        }
    }

    template<size_t m = M, size_t N, typename Allocator>
    ndarray_view(ndarray_view<T, N, Order, Base>& a, 
                 const slices_type& slices, 
               typename std::enable_if<m == N>::type* = 0)
        : data_(a.data()), size_(a.size())
    {
        for (size_t i = 0; i < M; ++i)
        {
            dim_[i] = slices[i].size()/slices[i].stride();
            strides_[i] = a.strides()[i]*slices[i].stride();
            offsets_[i] = Base::rebase_to_zero(a.offsets()[i] + slices[i].start());
        }
    }

    template<size_t m = M, size_t N, typename Allocator>
    ndarray_view(ndarray<T, N, Order, Base, Allocator>& a, 
                 const std::array<size_t,N-M>& indices,
                 const slices_type& slices, 
               typename std::enable_if<m < N>::type* = 0)
        : data_(a.data()), size_(a.size())
    {
        size_t rel = get_offset<N,N-M,Base>(a.strides(),indices);

        for (size_t i = 0; i < M; ++i)
        {
            dim_[i] = slices[i].size()/slices[i].stride();
            strides_[i] = a.strides()[(N-M)+i]*slices[i].stride();
            offsets_[i] = rel + Base::rebase_to_zero(slices[i].start());
        }
    }

    template<size_t m = M, size_t N, typename Allocator>
    ndarray_view(ndarray_view<T, N, Order, Base>& a, 
                 const std::array<size_t,N-M>& indices,
                 const slices_type& slices, 
               typename std::enable_if<m < N>::type* = 0)
        : data_(a.data()), size_(a.size())
    {
        size_t rel = get_offset<N,N-M,Base>(a.strides(),a.offsets(),indices);

        for (size_t i = 0; i < M; ++i)
        {
            dim_[i] = slices[i].size()/slices[i].stride();
            strides_[i] = a.strides()[(N-M)+i]*slices[i].stride();
            offsets_[i] = rel + Base::rebase_to_zero(a.offsets()[(N-M)+i] + slices[i].start());
        }
    }

    template<size_t m = M, size_t N, typename Allocator>
    ndarray_view(ndarray<T, N, Order, Base, Allocator>& a, 
                 const std::array<size_t,N-M>& indices,
               typename std::enable_if<m < N>::type* = 0)
        : data_(a.data()), size_(a.size())
    {
        size_t rel = get_offset<N,N-M,Base>(a.strides(),indices);

        for (size_t i = 0; i < M; ++i)
        {
            dim_[i] = a.dimensions()[(N-M)+i];
            strides_[i] = a.strides()[(N-M)+i];
            offsets_[i] = rel;
        }
    }

    template<size_t m = M, size_t N, typename Allocator>
    ndarray_view(ndarray_view<T, N, Order, Base>& a, 
                 const std::array<size_t,N-M>& indices,
               typename std::enable_if<m < N>::type* = 0)
        : data_(a.data()), size_(a.size())
    {
        size_t rel = get_offset<N,N-M,Base>(a.strides(),a.offsets(),indices);

        for (size_t i = 0; i < M; ++i)
        {
            dim_[i] = a.dimension()[(N-M)+i];
            strides_[i] = a.strides()[(N-M)+i];
            offsets_[i] = rel + Base::rebase_to_zero(a.offsets()[(N-M)+i]);
        }
    }

    ndarray_view(T* data, const std::array<size_t,M>& dim) 
        : data_(data), dim_(dim)
    {
        offsets_.fill(0);
        Order::calculate_strides(dim_, strides_, size_);
    }
    size_t size() const noexcept
    {
        return size_;
    }

    bool empty() const noexcept
    {
        return size_ == 0;
    }

    const std::array<size_t,M>& dimensions() const {return dim_;}

    const std::array<size_t,M>& strides() const {return strides_;}

    const std::array<size_t,M>& offsets() const {return offsets_;}

    T* data()
    {
        return data_;
    }

    const T* data() const 
    {
        return data_;
    }

    size_t size(size_t i) const
    {
        assert(i < dim_.size());
        return dim_[i];
    }

    template <typename... Indices>
    T& operator()(size_t index, Indices... indices) 
    {
        size_t off = get_offset<M, Base, 0>(strides_, offsets_, index, indices...);
        assert(off < size());
        return data_[off];
    }

    template <typename... Indices>
    const T& operator()(size_t index, Indices... indices) const
    {
        size_t off = get_offset<n, Base, 0>(strides_, offsets_, index, indices...);
        assert(off < size());
        return data_[off];
    }

    T& operator()(const std::array<size_t,M>& indices) 
    {
        size_t off = get_offset<M, M, Base>(strides_, offsets_, indices);
        assert(off < size());
        return data_[off];
    }

    const T& operator()(const std::array<size_t,M>& indices) const 
    {
        size_t off = get_offset<M, M, Base>(strides_, offsets_, indices);
        assert(off < size());
        return data_[off];
    }

    template <typename CharT>
    friend std::basic_ostream<CharT>& operator<<(std::basic_ostream<CharT>& os, ndarray_view<T, M, Order, Base>& v)
    {
        auto f = [&](const std::array<size_t,M>& indices) 
        { 
            return v(indices);
        };
        print(os, v.dimensions(), f);
        return os;
    }
};

template <typename T, size_t N, typename Order, typename Base, typename Allocator>
bool operator==(const ndarray<T, N, Order, Base, Allocator>& lhs, const ndarray<T, N, Order, Base, Allocator>& rhs)
{
    if (&lhs == &rhs)
    {
        return true;
    }
    for (size_t i = 0; i < N; ++i)
    {
        if (lhs.size(i) != rhs.size(i))
        {
            return false;
        }
    }
    for (size_t i = 0; i < lhs.size(); ++i)
    {
        if (lhs.data()[i] != rhs.data()[i])
        {
            return false;
        }
    }
    return true;
}

template <typename T, size_t N, typename Order, typename Base, typename Allocator>
bool operator!=(const ndarray<T, N, Order, Base, Allocator>& lhs, const ndarray<T, N, Order, Base, Allocator>& rhs)
{
    return !(lhs == rhs);
}

template <typename T, size_t M, typename Order, typename Base>
bool operator==(const ndarray_view<T, M, Order, Base>& lhs, 
                const ndarray_view<T, M, Order, Base>& rhs)
{
    if (&lhs == &rhs)
    {
        return true;
    }

    return Order::compare(lhs.data(), lhs.dimensions(), lhs.strides(), lhs.offsets(),
                          rhs.data(), rhs.dimensions(), lhs.strides(), rhs.offsets());
}

template <typename T, size_t M, typename Order, typename Base, typename Allocator>
bool operator==(const ndarray<T, M, Order, Base, Allocator>& lhs, 
                const ndarray_view<T, M, Order, Base>& rhs)
{
    std::array<size_t,M> offsets;
    offsets.fill(0);
    return Order::compare(lhs.data(), lhs.dimensions(), lhs.strides(), offsets,
                          rhs.data(), rhs.dimensions(), lhs.strides(), rhs.offsets());
}

template <typename T, size_t M, typename Order, typename Base, typename Allocator>
bool operator==(const ndarray_view<T, M, Order, Base>& lhs, 
                const ndarray<T, M, Order, Base, Allocator>& rhs)
{
    std::array<size_t,M> offsets;
    offsets.fill(0);
    return Order::compare(lhs.data(), lhs.dimensions(), lhs.strides(), lhs.offsets(),
                          rhs.data(), rhs.dimensions(), lhs.strides(), offsets);
}

template <typename T, size_t M, typename Order, typename Base>
bool operator!=(const ndarray_view<T, M, Order, Base>& lhs, const ndarray_view<T, M, Order, Base>& rhs)
{
    return !(lhs == rhs);
}

template <typename T, size_t M, typename Order, typename Base, typename Allocator>
bool operator!=(const ndarray<T, M, Order, Base, Allocator>& lhs, 
                const ndarray_view<T, M, Order, Base>& rhs)
{
    return !(lhs == rhs);
}

template <typename T, size_t M, typename Order, typename Base, typename Allocator>
bool operator!=(const ndarray_view<T, M, Order, Base>& lhs, 
                const ndarray<T, M, Order, Base, Allocator>& rhs)
{
    return !(lhs == rhs);
}

}

#endif
