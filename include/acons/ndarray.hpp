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
std::ostream& operator<<(std::ostream& os, const std::array<T,N>& a)
{
    for (auto it = std::begin(a); it != std::end(a); ++it)
    {
        if (it != std::begin(a))
        {
            os << ",";
        }
        os << *it;
    }
    return os;
}

template<class Pointer> inline
typename std::pointer_traits<Pointer>::element_type* to_plain_pointer(Pointer ptr)
{       
    return (std::addressof(*ptr));
}

class slice
{
public:
    typedef std::ptrdiff_t index_type;
private:
    index_type start_;
    index_type stop_;
    index_type stride_;
public:
    slice()
        : start_(0), stop_(0), stride_(0)
    {
    }
    slice(index_type start, index_type stop, index_type step=1)
        : start_(start), stop_(stop), stride_(step)
    {
        assert(start < stop);
    }

    slice(const slice& other) = default;
    slice(slice&& other) = default;

    slice& operator=(const slice& other) = default;
    slice& operator=(slice&& other) = default;

    index_type start() const
    {
        return start_;
    }
    index_type stop() const
    {
        return stop_;
    }
    index_type stride() const
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
    size_t i = Base::rebase_to_zero(index)*strides[m] + offsets[m];
    //std::cout << "get_offset m: " << m  << ", index: " << index << ", stride " << strides[m] << ", offset " << offsets[m] << ", i: " << i << "\n";
    return i;
}

template <size_t n, typename Base, size_t m, typename... Indices>
typename std::enable_if<(m+1 < n), size_t>::type
get_offset(const std::array<size_t,n>& strides, 
           const std::array<size_t, n>& offsets, 
           size_t index, Indices... indices)
{
    const size_t mplus1 = m + 1;
    size_t i = offsets[m] + Base::rebase_to_zero(index)*strides[m] + get_offset<n, Base, mplus1>(strides,offsets,indices...);

    //std::cout << "get_offset m: " << m << ", index: " << index << ", stride " << strides[m] << ", offset " << offsets[m] << ", rest: " << get_offset<n, Base, mplus1>(strides,offsets,indices...) << ", i: " << i << "\n";

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
        offset += (offsets[i] + Base::rebase_to_zero(indices[i])*strides[i]);
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
struct snapshot
{
    static const size_t close_bracket = N+1;
    static const size_t insert_comma = N+2;

    std::array<size_t, N> indices;
    size_t index;

    snapshot()
        : index(0)
    {
    }

    snapshot(const snapshot<N>& item)
        : indices(item.indices), index(item.index)
    {

    }

    snapshot(const std::array<size_t,N>& x, size_t idx)
        : indices(x), index(idx)
    {
    }

    snapshot(size_t idx)
        : index(idx)
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

    template <size_t N, typename Base>
    static void update_offsets(const std::array<size_t,N>& strides, 
                               const std::array<slice,N>& slices, 
                               std::array<size_t,N>& offsets)
    {
        offsets[N-1] += Base::rebase_to_zero(slices[N-1].start()) * strides[N-1];
        for (size_t i = 0; i+1 < N; ++i)
        {
            offsets[i] += Base::rebase_to_zero(slices[i].start()) * strides[i]; 
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
        std::vector<snapshot<N>> stack(stack_depth);

        size_t count = 1;
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
                size_t offset1 = get_offset<N,N,zero_based>(strides1,offsets1,val.indices);
                size_t offset2 = get_offset<N,N,zero_based>(strides2,offsets2,val.indices);
                const T* p1 = data1 + offset1;
                const T* p2 = data2 + offset2;
                const T* end = p1 + dim1[val.index];
                while (p1 != end)
                {
                    if (*p1++ != *p2++)
                    {
                        return false;
                    }
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

    template <size_t N, typename Base>
    static void update_offsets(const std::array<size_t,N>& strides, 
                               const std::array<slice,N>& slices, 
                               std::array<size_t,N>& offsets)
    {
        offsets[0] += Base::rebase_to_zero(slices[0].start()) * strides[0];
        for (size_t i = 1; i < N; ++i)
        {
            offsets[i] += Base::rebase_to_zero(slices[i].start()) * strides[i]; 
        }
    }
    template <typename T, size_t N>
    static bool compare(const T* data1, const std::array<size_t, N>& dim1, const std::array<size_t, N>& strides1, const std::array<size_t,N>& offsets1,
                        const T* data2, const std::array<size_t, N>& dim2, const std::array<size_t, N>& strides2, const std::array<size_t,N>& offsets2)
    {
        for (size_t i = 0; i < N; ++i)
        {
            if (dim1[i] != dim2[i])
            {
                return false;
            }
        }
        std::array<size_t,N> indices;
        bool result = compare(dim1, data1, strides1, offsets1, data2, strides2, offsets2, indices, N-1); 
        return result;
    }

    template <typename T, size_t N>
    static bool compare(const std::array<size_t,N>& dim,
                        const T* data1, const std::array<size_t,N>& strides1, const std::array<size_t,N>& offsets1, 
                        const T* data2, const std::array<size_t,N>& strides2, const std::array<size_t,N>& offsets2, 
                        std::array<size_t,N>& indices, size_t index)
    {
    if (index == 0)
    {
        for (size_t i = 0; i < dim[index]; ++i)
        {
            indices[index] = 0;
            size_t offset1 = get_offset<N,N,zero_based>(strides1,offsets1,indices);
            size_t offset2 = get_offset<N,N,zero_based>(strides2,offsets2,indices);
            const T* p1 = data1 + offset1;
            const T* p2 = data2 + offset2;
            const T* end = p1 + dim[index];
            while (p1 != end)
            {
                if (*p1++ != *p2++)
                {
                    return false;
                }
            }
        }
    }
    else
    {
        for (size_t i = 0; i < dim[index]; ++i)
        {
            indices[index] = i;
            if (!compare(dim,data1,strides1,offsets1,data2,strides2,offsets2,indices,index-1))
            {
                return false;
            }
        }
    }
    return true;
}
};
template <typename T, size_t N, typename Order=row_major, typename Base=zero_based, typename Allocator=std::allocator<T>>
class ndarray;

template <typename T, size_t M, typename Order=row_major, typename Base=zero_based, typename TPtr=const T*>
class const_ndarray_view;  

template <typename T, size_t M, typename Order=row_major, typename Base=zero_based>
class ndarray_view;  

template <typename T, size_t M, typename TPtr>
class ndarray_view_iterator_one;

template <typename T, size_t M, typename Order, typename Base, typename TPtr, typename Enable = void>
struct ndarray_view_member_types
{
    typedef ndarray<T,M-1,Order,Base> value_type;
    typedef ndarray_view<T,M-1,Order,Base> reference;
    typedef const_ndarray_view<T,M-1,Order,Base,TPtr> const_reference;
    typedef void iterator;
};

template <typename T, size_t M, typename Order, typename Base, typename TPtr>
struct ndarray_view_member_types<T,M,Order,Base,TPtr,typename std::enable_if<M == 1>::type>
{
    typedef T value_type;
    typedef T& reference;
    typedef const T& const_reference;
    typedef ndarray_view_iterator_one<T,M,TPtr> iterator;
};

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
    typedef std::allocator_traits<allocator_type> allocator_traits_type;
    typedef typename allocator_traits_type::pointer pointer;

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
    static void init(std::array<size_t, Array::dimension>&, Array& a)
    {
        a.init();
    }
    template <typename Array>
    static void init(std::array<size_t, Array::dimension>&, Array& a, typename Array::const_reference val)
    {
        a.init(val);
    }
};

template <typename T, size_t N, typename Order, typename Base, typename Allocator>
class ndarray : public ndarray_base<Allocator>
{
    typedef ndarray_base<Allocator> super_type;
public:
    using typename super_type::allocator_type;
    using typename super_type::pointer;
    typedef T element_type;
    typedef T& reference;
    typedef const T& const_reference;
    static const size_t dimension = N;
    typedef T* iterator;
    typedef const T* const_iterator;
    typedef Order order_type;
    typedef Base base_type;

    template <size_t M>
    struct view
    {
        typedef ndarray_view<T,M,Order,Base> type;
    };

    template <size_t M>
    struct const_view
    {
        typedef const_ndarray_view<T,M,Order,Base> type;
    };
private:
    friend struct init_helper<0>;
    friend class ndarray_view<T, N, Order, Base>;
    friend class const_ndarray_view<T, N, Order, Base, T*>;
    friend class const_ndarray_view<T, N, Order, Base, const T*>;

    pointer data_;
    size_t num_elements_;
    size_t capacity_;
    std::array<size_t,N> shape_;
    std::array<size_t,N> strides_;
public:
    using super_type::get_allocator;

    ndarray()
        : super_type(allocator_type()),
          data_(nullptr), num_elements_(0), capacity_(0)
    {
        shape_.fill(0);
        strides_.fill(0);
    }

    template <typename... Args>
    ndarray(size_t i, Args... args)
        : super_type(allocator_type()) 
    {
        init_helper<N>::init(shape_, *this, i, args ...);
    }

    explicit ndarray(const std::array<size_t,N>& dim)
        : super_type(allocator_type()), 
          data_(nullptr), shape_(dim)
    {
        Order::calculate_strides(shape_, strides_, num_elements_);
        capacity_ = num_elements_;
        data_ = create(capacity_, get_allocator());
        std::fill(data_, data_+num_elements_, T());
    }

    ndarray(const std::array<size_t,N>& dim, const Allocator& alloc)
        : super_type(alloc), 
          data_(nullptr), shape_(dim)
    {
        Order::calculate_strides(shape_, strides_, num_elements_);
        capacity_ = num_elements_;
        data_ = create(capacity_, get_allocator());
        std::fill(data_, data_+num_elements_, T());
    }

    ndarray(const std::array<size_t,N>& dim, T val)
        : super_type(allocator_type()), 
          data_(nullptr), shape_(dim)
    {
        Order::calculate_strides(shape_, strides_, num_elements_);
        capacity_ = num_elements_;
        data_ = create(capacity_, get_allocator());
        std::fill(data_, data_+num_elements_,val);
    }

    ndarray(const std::array<size_t,N>& dim, T val, const Allocator& alloc)
        : super_type(alloc), 
          data_(nullptr), shape_(dim)
    {
        Order::calculate_strides(shape_, strides_, num_elements_);

        capacity_ = num_elements_;
        data_ = create(capacity_, get_allocator());
        std::fill(data_, data_+num_elements_, val);
    }

    ndarray(std::initializer_list<array_item<T>> list) 
        : super_type(allocator_type())
    {
        dim_from_initializer_list(list, 0);

        Order::calculate_strides(shape_, strides_, num_elements_);
        capacity_ = num_elements_;
        data_ = create(capacity_, get_allocator());
        std::array<size_t,N> indices;
        data_from_initializer_list(list,indices,0);
    }

    ndarray(std::initializer_list<array_item<T>> list, const Allocator& alloc) 
        : super_type(alloc)
    {
        dim_from_initializer_list(list, 0);

        // Initialize multipliers and size
        Order::calculate_strides(shape_, strides_, num_elements_);
        capacity_ = num_elements_;
        data_ = create(capacity_, get_allocator());
        std::array<size_t,N> indices;
        data_from_initializer_list(list,indices,0);
    }

    ndarray(const ndarray& other)
        : super_type(std::allocator_traits<allocator_type>::select_on_container_copy_construction(other.get_allocator())), 
          data_(nullptr), num_elements_(other.num_elements()), capacity_(0), shape_(other.shape_), strides_(other.strides_)          
    {
        capacity_ = num_elements_;
        data_ = create(capacity_, get_allocator());

#if defined(_MSC_VER)
        std::copy(other.data_, other.data_+other.num_elements_,stdext::make_checked_array_iterator(data_,num_elements_));
#else 
        std::copy(other.data_,other.data_+other.num_elements_,data_);
#endif
    }

    ndarray(const ndarray& other, const Allocator& alloc)
        : super_type(alloc), 
          data_(nullptr), num_elements_(other.num_elements()), shape_(other.shape_), strides_(other.strides_)          
    {
        capacity_ = num_elements_;
        data_ = create(capacity_, get_allocator());

#if defined(_MSC_VER)
        std::copy(other.data_, other.data_+other.num_elements_,stdext::make_checked_array_iterator(data_,num_elements_));
#else 
        std::copy(other.data_,other.data_+other.num_elements_,data_);
#endif
    }

    ndarray(ndarray&& other)
        : super_type(other.get_allocator()), 
          data_(other.data_), num_elements_(other.num_elements_), capacity_(other.capacity_), shape_(other.shape_), strides_(other.strides_)          
    {
        other.data_ = nullptr;
        other.num_elements_ = 0;
        other.capacity_ = 0;
        other.shape_.fill(0);
        other.strides_.fill(0);
    }

    ndarray(ndarray&& other, const Allocator& alloc)
        : super_type(alloc), 
          data_(nullptr), num_elements_(other.num_elements()), capacity_(other.capacity_), shape_(other.shape_), strides_(other.strides_)          
    {
        if (alloc == other.get_allocator())
        {
            data_ = other.data_;
            other.data_ = nullptr;
            other.num_elements_ = 0;
            other.capacity_ = 0;
            other.shape_.fill(0);
            other.strides_.fill(0);
        }
        else
        {
            capacity_ = num_elements_;
            data_ = create(capacity_, get_allocator());
#if defined(_MSC_VER)
            std::copy(other.data_, other.data_+other.num_elements_,stdext::make_checked_array_iterator(data_,num_elements_));
#else 
            std::copy(other.data_,other.data_+other.num_elements_,data_);
#endif
        }
    }

    template <typename TPtr>
    ndarray(const const_ndarray_view<T,N,Order,Base,TPtr>& av)
        : super_type(allocator_type()), 
          data_(nullptr), num_elements_(0), capacity_(0), shape_(av.shape()), strides_(av.strides())          
    {
        num_elements_ = av.num_elements();
        capacity_ = num_elements_;
        data_ = create(capacity_, get_allocator());

#if defined(_MSC_VER)
        std::copy(av.data(), av.data()+av.num_elements(),stdext::make_checked_array_iterator(data_,num_elements_));
#else 
        std::copy(av.data(),av.data()+av.num_elements(),data_);
#endif
    }

    template <typename TPtr>
    ndarray(const const_ndarray_view<T,N,Order,Base,TPtr>& av, 
            const Allocator& alloc)
        : super_type(alloc), 
          data_(nullptr), num_elements_(0), capacity_(0), shape_(av.shape_), strides_(av.strides_)          
    {
        num_elements_ = av.num_elements();
        capacity_ = num_elements_;
        data_ = create(capacity_, get_allocator());

#if defined(_MSC_VER)
        std::copy(av.data(), av.data()+av.num_elements(),stdext::make_checked_array_iterator(data_,num_elements_));
#else 
        std::copy(av.data(),av.data()+av.num_elements(),data_);
#endif
    }

    ~ndarray()
    {
        get_allocator().deallocate(to_plain_pointer(data_), capacity_);
    }

    template <size_t K>
    ndarray_view<T,N-K,Order,Base> subarray(const std::array<size_t,K>& origin)
    {
        return ndarray_view<T,N-K,Order,Base>(*this, origin);
    }

    template <size_t K>
    const_ndarray_view<T,N-K,Order,Base> subarray(const std::array<size_t,K>& origin) const
    {
        return const_ndarray_view<T,N-K,Order,Base>(*this, origin);
    }

    iterator begin()
    {
        return to_plain_pointer(data_);
    }

    iterator end()
    {
        return to_plain_pointer(data_) + num_elements_;
    }

    const_iterator begin() const
    {
        return to_plain_pointer(data_);
    }

    const_iterator end() const
    {
        return to_plain_pointer(data_) + num_elements_;
    }

    const_iterator cbegin() const
    {
        return to_plain_pointer(data_);
    }

    const_iterator cend() const
    {
        return to_plain_pointer(data_) + num_elements_;
    }

    void resize(const std::array<size_t,N>& dim, T value = T())
    {
        T* old_data = data();
        size_t old_size = num_elements();

        shape_ = dim;
        Order::calculate_strides(shape_, strides_, num_elements_);

        if (num_elements_ > capacity_)
        {
            capacity_ = num_elements_;
            data_ = create(capacity_, get_allocator());
        }

        size_t len = (std::min)(old_size,num_elements_);

#if defined(_MSC_VER)
        std::copy(old_data, old_data+len,stdext::make_checked_array_iterator(data_,num_elements_));
#else 
        std::copy(old_data, old_data+len,data_);
#endif
        if (len < num_elements_)
        {
            std::fill(data_ + len, data_+num_elements_, value);
        }

        if (num_elements_ > old_size)
        {
            get_allocator().deallocate(to_plain_pointer(old_data), old_size);
        }
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
        get_allocator().deallocate(to_plain_pointer(data_),capacity_);
        dim_from_initializer_list(list, 0);

        Order::calculate_strides(shape_, strides_, num_elements_);
        capacity_ = num_elements_;
        data_ = create(capacity_, get_allocator());
        std::array<size_t,N> indices;
        data_from_initializer_list(list,indices,0);
        return *this;
    }

    void swap(ndarray<T,N,Order,Base,Allocator>& other) noexcept
    {
        swap_allocator(other, is_stateless<Allocator>(), 
                       typename std::allocator_traits<allocator_type>::propagate_on_container_swap());
        std::swap(data_,other.data_);
        std::swap(num_elements_,other.num_elements_);
        std::swap(capacity_,other.capacity_);
        std::swap(shape_,other.shape_);
        std::swap(strides_,other.strides_);

    }

    bool empty() const noexcept
    {
        return num_elements_ == 0;
    }

    size_t num_elements() const noexcept
    {
        return num_elements_;
    }

    size_t capacity() const noexcept
    {
        return capacity_;
    }

    const std::array<size_t,N>& shape() const {return shape_;}

    const std::array<size_t,N>& strides() const {return strides_;}

    T* data()
    {
        return to_plain_pointer(data_);
    }

    const T* data() const 
    {
        return to_plain_pointer(data_);
    }

    size_t size(size_t i) const
    {
        assert(i < shape_.size());
        return shape_[i];
    }

    template <typename... Indices>
    T& operator()(size_t index, Indices... indices) 
    {
        size_t off = get_offset<N, Base, 0>(strides_, index, indices...);
        assert(off < num_elements());
        return data_[off];
    }

    template <typename... Indices>
    const T& operator()(size_t index, Indices... indices) const
    {
        size_t off = get_offset<N, Base, 0>(strides_, index, indices...);
        assert(off < num_elements());
        return data_[off];
    }

    T& operator()(const std::array<size_t,N>& indices) 
    {
        size_t off = get_offset<N, N, Base>(strides_,indices);

        assert(off < num_elements());
        return data_[off];
    }

    const T& operator()(const std::array<size_t,N>& indices) const 
    {
        size_t off = get_offset<N, N, Base>(strides_,indices);
        assert(off < num_elements());
        return data_[off];
    }
private:

    static pointer create(size_t size, const Allocator& allocator)
    {
        allocator_type alloc(allocator);
        pointer ptr = alloc.allocate(size);
        return ptr;
    }

    void assign_move(ndarray<T,N,Order,Base,Allocator>&& other, std::true_type) noexcept
    {
        swap(other);
    }

    void assign_move(ndarray<T,N,Order,Base,Allocator>&& other, std::false_type) noexcept
    {
        if (num_elements() != other.num_elements())
        {
            get_allocator().deallocate(to_plain_pointer(data_),capacity_);
            num_elements_ = other.num_elements();
            capacity_ = num_elements_;
            data_ = create(capacity_, get_allocator());
        }
        shape_ = other.shape();
        strides_ = other.strides();
#if defined(_MSC_VER)
        std::copy(other.data_, other.data_+other.num_elements_,stdext::make_checked_array_iterator(data_,num_elements_));
#else 
        std::copy(other.data_,other.data_+other.num_elements_,data_);
#endif
    }

    void assign_copy(const ndarray<T,N,Order,Base,Allocator>& other, std::true_type)
    {
        this->allocator_ = other.get_allocator();
        get_allocator().deallocate(to_plain_pointer(data_),capacity_);
        num_elements_ = other.num_elements();
        capacity_ = num_elements_;
        data_ = create(capacity_, get_allocator());
        shape_ = other.shape();
        strides_ = other.strides();
#if defined(_MSC_VER)
        std::copy(other.data_, other.data_+other.num_elements_,stdext::make_checked_array_iterator(data_,num_elements_));
#else 
        std::copy(other.data_,other.data_+other.num_elements_,data_);
#endif
    }

    void assign_copy(const ndarray<T,N,Order,Base,Allocator>& other, std::false_type)
    {
        if (num_elements() != other.num_elements())
        {
            get_allocator().deallocate(to_plain_pointer(data_),capacity_);
            num_elements_ = other.num_elements();
            capacity_ = num_elements_;
            data_ = create(capacity_, get_allocator());
        }
        shape_ = other.shape();
        strides_ = other.strides();
#if defined(_MSC_VER)
        std::copy(other.data_, other.data_+other.num_elements_,stdext::make_checked_array_iterator(data_,num_elements_));
#else 
        std::copy(other.data_,other.data_+other.num_elements_,data_);
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

    void swap_allocator(ndarray<T,N,Order,Base,Allocator>&, std::false_type, std::false_type) noexcept
    {
        // Undefined behavior
    }

    void init()
    {
        Order::calculate_strides(shape_, strides_, num_elements_);
        capacity_ = num_elements_;
        data_ = create(capacity_, get_allocator());
    }

    void init(const T& val)
    {
        Order::calculate_strides(shape_, strides_, num_elements_);
        capacity_ = num_elements_;
        data_ = create(capacity_, get_allocator());
        std::fill(data_, data_+num_elements_,val);
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
                    shape_[dim++] = init.size();
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
                if (offset < num_elements())
                {
                    data_[offset] = item.value();
                }
            }
            ++i;
        }
    }
};

template <typename CharT, typename T, size_t M, typename Order, typename Base, typename TPtr>
typename std::enable_if<(M>1),void>::type
print(std::basic_ostream<CharT>& os, const_ndarray_view<T,M,Order,Base,TPtr>& v)
{
    //std::cout << "const_ndarray_view<T,M,Order,Base,TPtr> M > 1\n";
    os << '[';
    for (size_t i = 0; i < v.size(0); ++i)
    {
        if (i > 0)
        {
            os << ',';
        }
        std::array<size_t,1> a = {i};
        const_ndarray_view<T,M-1,Order,Base,const T*> w(v,a);
        print(os,w);
    }
    os << ']';
}
template <typename CharT, typename T, size_t M, typename Order, typename Base, typename TPtr>
typename std::enable_if<M == 1,void>::type
print(std::basic_ostream<CharT>& os, const_ndarray_view<T,M,Order,Base,TPtr>& v)
{
    //std::cout << "const_ndarray_view<T,M,Order,Base,TPtr> M == 1\n";
    //std::cout << "size: " << v.size() << "\n";
    os << '[';
    bool begin = true;
    for (auto element : v)
    {
        if (!begin)
        {
            os << ',';
        }
        else
        {
            begin = false;
        }
        os << element;
    }
    os << ']';
}

template <typename CharT, typename T, size_t M, typename Order, typename Base, typename Allocator>
typename std::enable_if<(M>1),void>::type
print(std::basic_ostream<CharT>& os, ndarray<T,M,Order,Base,Allocator>& v)
{
    os << '[';
    for (size_t i = 0; i < v.size(0); ++i)
    {
        if (i > 0)
        {
            os << ',';
        }
        std::array<size_t,1> a = {i};
        const_ndarray_view<T,M-1,Order,Base> w(v,a);
        print(os,w);
    }
    os << ']';
}
template <typename CharT, typename T, size_t M, typename Order, typename Base, typename Allocator>
typename std::enable_if<M == 1,void>::type
print(std::basic_ostream<CharT>& os, ndarray<T,M,Order,Base,Allocator>& v)
{
    os << '[';
    bool begin = true;
    for (auto element : v)
    {
        if (!begin)
        {
            os << ',';
        }
        else
        {
            begin = false;
        }
        os << element;
    }
    os << ']';
}

template <typename T, size_t N, typename Order, typename Base, typename Allocator, typename CharT>
std::basic_ostream<CharT>& operator<<(std::basic_ostream<CharT>& os, ndarray<T, N, Order, Base, Allocator>& a)
{
    print(os,a);
    return os;
}

template <typename T, size_t M, typename TPtr>
class ndarray_view_iterator_one
{
    TPtr data_;
    size_t stride_;
    size_t offset_;
    TPtr p_;
public:
    typedef T value_type;
    typedef std::ptrdiff_t difference_type;
    typedef TPtr pointer;
    typedef typename std::conditional<std::is_const<typename std::remove_pointer<TPtr>::type>::value,const T&,T&>::type reference;
    typedef std::input_iterator_tag iterator_category;

    ndarray_view_iterator_one(TPtr data, size_t stride, size_t offset)
        : data_(data), stride_(stride), offset_(offset)
    {
        p_ = data_ + offset_;
    }

    ndarray_view_iterator_one(const ndarray_view_iterator_one&) = default;
    ndarray_view_iterator_one(ndarray_view_iterator_one&&) = default;
    ndarray_view_iterator_one& operator=(const ndarray_view_iterator_one&) = default;
    ndarray_view_iterator_one& operator=(ndarray_view_iterator_one&&) = default;

    ndarray_view_iterator_one& operator++()
    {
        p_ += stride_;
        return *this;
    }

    ndarray_view_iterator_one operator++(int) // postfix increment
    {
        ndarray_view_iterator_one temp(*this);
        ++(*this);
        return temp;
    }

    reference operator*() const
    {
        return *p_;
    }

    friend bool operator==(const ndarray_view_iterator_one& it1, const ndarray_view_iterator_one& it2)
    {
        return it1.p_ == it2.p_;
    }
    friend bool operator!=(const ndarray_view_iterator_one& it1, const ndarray_view_iterator_one& it2)
    {
        return !(it1 == it2);
    }
};

template <typename T, size_t M, typename Order, typename Base, typename TPtr>
class const_ndarray_view 
{
    typedef ndarray_view_member_types<T,M,Order,Base,TPtr> member_types;
public:
    static const size_t dimension = M;
    typedef T element_type;
    typedef Order order_type;
    typedef Base base_type;
    typedef typename member_types::value_type value_type;
    typedef typename member_types::const_reference const_reference;
    typedef typename member_types::iterator const_iterator;
protected:
    TPtr data_;
    size_t num_elements_;
    std::array<size_t,M> shape_;
    std::array<size_t,M> strides_;
    std::array<size_t,M> offsets_;
public:
    template <typename Allocator>
    const_ndarray_view(ndarray<T, M, Order, Base, Allocator>& a)
        : data_(a.data()), num_elements_(a.num_elements()), shape_(a.shape()), strides_(a.strides())          
    {
        offsets_.fill(0);
    }

    template<typename Allocator>
    const_ndarray_view(ndarray<T, M, Order, Base, Allocator>& a, 
                       const std::array<slice,M>& slices)
        : data_(a.data()), num_elements_(a.num_elements())
    {
        for (size_t i = 0; i < M; ++i)
        {
            shape_[i] = (slices[i].stop() - slices[i].start()) /slices[i].stride();
            strides_[i] = a.strides()[i];
        }
        offsets_.fill(0);
        Order::update_offsets<M,Base>(strides_, slices, offsets_);
        for (size_t i = 0; i < M; ++i)
        {
            strides_[i] *= slices[i].stride();
        }
    }

    template<size_t m = M, size_t N, typename Allocator>
    const_ndarray_view(ndarray<T, N, Order, Base, Allocator>& a, 
                       const std::array<size_t,N-m>& origin)
        : data_(nullptr), num_elements_(0)
    {
        size_t offset = get_offset<N,N-M,Base>(a.strides(),origin);

        //std::cout << "offset: " << offset << "\n";

        data_ = a.data() + offset;
        num_elements_ = a.num_elements() - offset;

        for (size_t i = 0; i < M; ++i)
        {
            shape_[i] = a.shape()[(N-M)+i];
            strides_[i] = a.strides()[(N-M)+i];

            //std::cout << "dim " << i << ", size: " << shape_[i] << ", stride: " << strides_[i] << "\n";
        }
        offsets_.fill(0);
    }

    template<size_t m = M, size_t N, typename Allocator>
    const_ndarray_view(ndarray<T, N, Order, Base, Allocator>& a, 
                       const std::array<size_t,N-m>& origin,
                       const std::array<slice,M>& slices)
        : data_(nullptr), num_elements_(0)
    {
        size_t offset = get_offset<N,N-M,Base>(a.strides(),origin);

        data_ = a.data() + offset;
        num_elements_ = a.num_elements() - offset;

        for (size_t i = 0; i < M; ++i)
        {
            shape_[i] = (slices[i].stop() - slices[i].start())/slices[i].stride();
            strides_[i] = a.strides()[(N-M)+i];
        }
        offsets_.fill(0);
        Order::update_offsets<M,Base>(strides_, slices, offsets_);
        for (size_t i = 0; i < M; ++i)
        {
            strides_[i] *= slices[i].stride();
        }
    }

    template<typename OtherTPtr>
    const_ndarray_view(const const_ndarray_view<T, M, Order, Base, OtherTPtr>& other)
        : data_(other.data_), num_elements_(other.num_elements()), shape_(other.shape()), strides_(other.strides()), offsets_(other.offsets())          
    {
    }

    template<typename OtherTPtr>
    const_ndarray_view(const const_ndarray_view<T, M, Order, Base, OtherTPtr>& other, 
                       const std::array<slice,M>& slices)
        : data_(other.data_), num_elements_(other.num_elements()), offsets_(other.offsets())
    {
        for (size_t i = 0; i < M; ++i)
        {
            shape_[i] = (slices[i].stop() - slices[i].start())/slices[i].stride();
            strides_[i] = other.strides()[i];
        }
        offsets_.fill(0);
        Order::update_offsets<M,Base>(strides_, slices, offsets_);
        for (size_t i = 0; i < M; ++i)
        {
            strides_[i] *= slices[i].stride();
        }
    }

    template<size_t m = M, size_t N, typename OtherTPtr>
    const_ndarray_view(const const_ndarray_view<T, N, Order, Base, OtherTPtr>& other, 
                       const std::array<size_t,N-m>& origin)
        : data_(other.data()), num_elements_(other.num_elements())
    {
        //std::cout << "const_ndarray_view strides: " << other.strides() << ", offsets: " << other.offsets() << ", origin: " << origin << ", data[0] " << data_[0] << ", size: " << num_elements() << "\n";
 
        size_t rel = get_offset<N,N-M,Base>(other.strides(),other.offsets(),origin);
        //std::cout << "rel: " << rel << "\n";
        data_ = other.data() + rel;
        num_elements_ = other.num_elements() - rel;

        for (size_t i = 0; i < M; ++i)
        {
            shape_[i] = other.shape()[(N-M)+i];
            strides_[i] = other.strides()[(N-M)+i];
            offsets_[i] = /*rel + */ other.offsets()[(N-M)+i];
            //std::cout << "dim " << i << ", size: " << shape_[i] << ", stride: " << strides_[i] << ", offset: " << offsets_[i] << "\n";
        }
    }

    template<size_t m = M, size_t N, typename OtherTPtr>
    const_ndarray_view(const const_ndarray_view<T, N, Order, Base, OtherTPtr>& other, 
                       const std::array<size_t,N-m>& origin,
                       const std::array<slice,M>& slices)
        : data_(other.data_), num_elements_(other.num_elements())
    {
        size_t rel = get_offset<N,N-M,Base>(other.strides(),other.offsets(),origin);
        data_ = other.data() + rel;
        num_elements_ = other.num_elements() - rel;

        for (size_t i = 0; i < M; ++i)
        {
            shape_[i] = (slices[i].stop() - slices[i].start())/slices[i].stride();
            strides_[i] = other.strides()[(N-M)+i];
        }
        Order::update_offsets<M,Base>(strides_, slices, offsets_);
        for (size_t i = 0; i < M; ++i)
        {
            strides_[i] *= slices[i].stride();
        }
    }

    template<typename OtherTPtr>
    const_ndarray_view(OtherTPtr data, const std::array<size_t,M>& dim,
                       typename std::enable_if<std::is_convertible<OtherTPtr,TPtr>::value>::type* = 0) 
        : data_(data), shape_(dim)
    {
        offsets_.fill(0);
        Order::calculate_strides(shape_, strides_, num_elements_);
    }

    size_t num_elements() const noexcept
    {
        return num_elements_;
    }

    bool empty() const noexcept
    {
        return num_elements_ == 0;
    }

    const std::array<size_t,M>& shape() const {return shape_;}

    const std::array<size_t,M>& strides() const {return strides_;}

    const std::array<size_t,M>& offsets() const {return offsets_;}

    const T* data() const 
    {
        return data_;
    }

    size_t size(size_t i) const
    {
        assert(i < shape_.size());
        return shape_[i];
    }

    template <typename... Indices>
    const T& operator()(size_t index, Indices... indices) const
    {
        size_t off = get_offset<M, Base, 0>(strides_, offsets_, index, indices...);

        //std::cout << "operator() strides: " << strides_ << ", offsets: " << offsets_ << ", index: " << index << "\n";
        //std::cout << "off: " << off << ", size: " << size() << "\n";

        assert(off < num_elements());
        return data_[off];
    }

    const T& operator()(const std::array<size_t,M>& indices) const 
    {
        size_t off = get_offset<M, M, Base>(strides_, offsets_, indices);
        assert(off < num_elements());
        return data_[off];
    }

    const_iterator begin() const
    {
        return const_iterator(data_,strides_[0],offsets_[0]);
    }

    const_iterator end() const
    {
        return const_iterator(data_,strides_[0],(offsets_[0]+size(0)*strides_[0]));
    }
    const_iterator cbegin() const
    {
        return const_iterator(data_,strides_[0],offsets_[0]);
    }

    const_iterator cend() const
    {
        return const_iterator(data_,strides_[0],(offsets_[0]+size(0)*strides_[0]));
    }
};

template <typename CharT, typename T, size_t M, typename Order, typename Base, typename TPtr>
std::basic_ostream<CharT>& operator<<(std::basic_ostream<CharT>& os, 
                                      const_ndarray_view<T, M, Order, Base, TPtr>& v)
{
    print(os,v);
    return os;
}

template <typename T, size_t M, typename Order, typename Base>
class ndarray_view : public const_ndarray_view<T, M, Order, Base, T*>
{
    typedef ndarray_view_member_types<T,M,Order,Base,T*> member_types;
    typedef const_ndarray_view<T, M, Order, Base, T*> super_type;
public:
    using typename super_type::element_type;
    using typename super_type::order_type;
    using typename super_type::base_type;
    using typename super_type::const_reference;
    using typename super_type::const_iterator;
    typedef typename member_types::reference reference;
    typedef typename member_types::iterator iterator;
public:
    template <typename Allocator>
    ndarray_view(ndarray<T, M, Order, Base, Allocator>& a)
        : super_type(a)          
    {
    }

    template<size_t N, typename Allocator>
    ndarray_view(ndarray<T, N, Order, Base, Allocator>& a, 
                       const std::array<slice,M>& slices)
        : super_type(a, slices)
    {
    }

    template<size_t N, typename Allocator>
    ndarray_view(ndarray<T, N, Order, Base, Allocator>& a, 
                 const std::array<size_t,N-M>& origin)
        : super_type(a, origin)
    {
    }

    template<size_t N, typename Allocator>
    ndarray_view(ndarray<T, N, Order, Base, Allocator>& a, 
                 const std::array<size_t,N-M>& origin,
                 const std::array<slice,M>& slices)
        : super_type(a, origin, slices)
    {
    }

    template <typename OtherTPtr>
    ndarray_view(const const_ndarray_view<T, M, Order, Base, OtherTPtr>& a)
        : super_type(a)          
    {
    }

    template<size_t N, typename OtherTPtr>
    ndarray_view(const const_ndarray_view<T, N, Order, Base, OtherTPtr>& a, 
                 const std::array<size_t,N-M>& origin)
        : super_type(a, origin)
    {
    }

    template<size_t N, typename OtherTPtr>
    ndarray_view(const const_ndarray_view<T, N, Order, Base, OtherTPtr>& a, 
                 const std::array<slice,M>& slices)
        : super_type(a, slices)
    {
    }

    template<size_t N, typename OtherTPtr>
    ndarray_view(const const_ndarray_view<T, N, Order, Base, OtherTPtr>& a, 
                 const std::array<size_t,N-M>& origin,
                 const std::array<slice,M>& slices)
        : super_type(a, origin, slices)
    {
    }

    ndarray_view(T* data, const std::array<size_t,M>& dim) 
        : super_type(data, dim)
    {
    }
    using super_type::data; 
    using super_type::num_elements;
    using super_type::size;
    using super_type::empty;
    using super_type::shape;
    using super_type::strides;
    using super_type::offsets;
    using super_type::cbegin;
    using super_type::cend;

    T* data()
    {
        return this->data_;
    }

    template <typename... Indices>
    T& operator()(size_t index, Indices... indices) 
    {
        size_t off = get_offset<M, Base, 0>(this->strides_, this->offsets_, index, indices...);
        assert(off < num_elements());
        return this->data_[off];
    }

    T& operator()(const std::array<size_t,M>& indices) 
    {
        size_t off = get_offset<M, M, Base>(this->strides_, this->offsets_, indices);
        assert(off < num_elements());
        return this->data_[off];
    }

    template <typename... Indices>
    const T& operator()(size_t index, Indices... indices) const
    {
        size_t off = get_offset<M, Base, 0>(this->strides_, this->offsets_, index, indices...);
        assert(off < num_elements());
        return this->data_[off];
    }

    const T& operator()(const std::array<size_t, M>& indices) const
    {
        size_t off = get_offset<M, M, Base>(this->strides_, this->offsets_, indices);
        assert(off < num_elements());
        return this->data_[off];
    }

    iterator begin()
    {
        return iterator(this->data_,this->strides_[0],this->offsets_[0]);
    }

    iterator end() 
    {
        return iterator(this->data_,this->strides_[0],(this->offsets_[0]+size(0)*this->strides_[0]));
    }

    const_iterator begin() const
    {
        return super_type::begin();
    }

    const_iterator end() const
    {
        return super_type::end();
    }
};

template <typename T, size_t N, typename Order, typename Base, typename Allocator>
bool operator==(const ndarray<T, N, Order, Base, Allocator>& lhs, const ndarray<T, N, Order, Base, Allocator>& rhs)
{
    if (&lhs == &rhs)
    {
        return true;
    }

    std::array<size_t,N> offsets;
    offsets.fill(0);

    return Order::compare(lhs.data(), lhs.shape(), lhs.strides(), offsets,
                          rhs.data(), rhs.shape(), lhs.strides(), offsets);
}

template <typename T, size_t N, typename Order, typename Base, typename Allocator>
bool operator!=(const ndarray<T, N, Order, Base, Allocator>& lhs, 
                const ndarray<T, N, Order, Base, Allocator>& rhs)
{
    return !(lhs == rhs);
}

template <typename T, size_t M, typename Order, typename Base, typename TPtr>
bool operator==(const const_ndarray_view<T, M, Order, Base, TPtr>& lhs, 
                const const_ndarray_view<T, M, Order, Base, TPtr>& rhs)
{
    if (&lhs == &rhs)
    {
        return true;
    }

    return Order::compare(lhs.data(), lhs.shape(), lhs.strides(), lhs.offsets(),
                          rhs.data(), rhs.shape(), lhs.strides(), rhs.offsets());
}

template <typename T, size_t M, typename Order, typename Base, typename Allocator, typename TPtr>
bool operator==(const ndarray<T, M, Order, Base, Allocator>& lhs, 
                const const_ndarray_view<T, M, Order, Base, TPtr>& rhs)
{
    std::array<size_t,M> offsets;
    offsets.fill(0);
    return Order::compare(lhs.data(), lhs.shape(), lhs.strides(), offsets,
                          rhs.data(), rhs.shape(), lhs.strides(), rhs.offsets());
}

template <typename T, size_t M, typename Order, typename Base, typename Allocator, typename TPtr>
bool operator==(const const_ndarray_view<T, M, Order, Base, TPtr>& lhs, 
                const ndarray<T, M, Order, Base, Allocator>& rhs)
{
    std::array<size_t,M> offsets;
    offsets.fill(0);
    return Order::compare(lhs.data(), lhs.shape(), lhs.strides(), lhs.offsets(),
                          rhs.data(), rhs.shape(), lhs.strides(), offsets);
}

template <typename T, size_t M, typename Order, typename Base, typename TPtr>
bool operator!=(const const_ndarray_view<T, M, Order, Base, TPtr>& lhs, 
                const const_ndarray_view<T, M, Order, Base, TPtr>& rhs)
{
    return !(lhs == rhs);
}

template <typename T, size_t M, typename Order, typename Base, typename Allocator, typename TPtr>
bool operator!=(const ndarray<T, M, Order, Base, Allocator>& lhs, 
                const const_ndarray_view<T, M, Order, Base, TPtr>& rhs)
{
    return !(lhs == rhs);
}

template <typename T, size_t M, typename Order, typename Base, typename Allocator, typename TPtr>
bool operator!=(const const_ndarray_view<T, M, Order, Base, TPtr>& lhs, 
                const ndarray<T, M, Order, Base, Allocator>& rhs)
{
    return !(lhs == rhs);
}

}

#endif
