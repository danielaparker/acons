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

struct zero_based
{
    static constexpr size_t origin() {return 0;}

    static size_t rebase_to_zero(size_t index)
    {
        return index;
    }
};

struct one_based
{
    static constexpr size_t origin() {return 1;}

    static size_t rebase_to_zero(size_t index)
    {
        return index - origin();
    }
};

template <typename TPtr, typename Enable = void>
struct is_pointer_to_const : std::false_type {};

template <typename TPtr>
struct is_pointer_to_const<TPtr, typename std::enable_if<std::is_const<typename std::remove_pointer<TPtr>::type>::value>::type> : std::true_type {};

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
    static constexpr size_t npos = size_t(-1);

    size_t start_;
    size_t stop_;
    size_t step_;
public:
    constexpr explicit slice(size_t start = npos, size_t stop = npos, size_t step=1)
        : start_(start), stop_(stop), step_(step)
    {
    }

    slice(const slice& other) = default;
    slice(slice&& other) = default;

    slice& operator=(const slice& other) = default;
    slice& operator=(slice&& other) = default;

    size_t start(size_t origin) const
    {
        return start_ == npos ? origin : start_;
    }

    size_t stop(size_t origin, size_t n) const
    {
        return stop_ == npos ? (origin+n) : stop_;
    }
    size_t step() const
    {
        return step_;
    }

    size_t length(size_t origin, size_t n) const
    {
        size_t y = stop(origin, n);
        size_t x = start(origin);

        assert(y >= x);
        size_t w = y - x;
        return w/step_ + (w % step_ != 0);
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

struct row_major
{
    template <size_t N>
    static void calculate_strides(const std::array<size_t,N>& shape, std::array<size_t,N>& strides, size_t& size)
    {
        size = 1;
        for (size_t i = 0; i < N; ++i)
        {
            strides[N-i-1] = size;
            size *= shape[N-i-1];
        }
    }

    template <size_t N>
    static void update_offsets(size_t rel, std::array<size_t,N>& offsets)
    {
        offsets[N-1] += rel;
    }

    template <size_t N, typename Base>
    static void calculate_offsets(size_t rel,
                                  const std::array<size_t,N>& strides, 
                                  const std::array<slice,N>& slices, 
                                  std::array<size_t,N>& offsets)
    {
        offsets[N-1] = rel + Base::rebase_to_zero(slices[N-1].start(Base::origin())) * strides[N-1];
        for (size_t i = 0; i+1 < N; ++i)
        {
            offsets[i] = Base::rebase_to_zero(slices[i].start(Base::origin())) * strides[i]; 
        }
    }

    template <class A, class B>
    static typename std::enable_if<(A::ndim == B::ndim && 
        std::is_same<typename A::order_type,typename B::order_type>::value &&
        std::is_same<typename A::value_type, typename B::value_type>::value &&
        (A::ndim>1)),bool>::type
    compare(const A& a, const B& b)
    {
        constexpr size_t N = A::ndim;
        typedef typename A::base_type base_type1;
        typedef typename B::base_type base_type2;

        if (a.size(0) != b.size(0))
        {
            return false;
        }
        for (size_t i = 0; i < a.size(0); ++i)
        {
            std::array<size_t,1> origin1 = {base_type1::origin() + i};
            std::array<size_t,1> origin2 = {base_type2::origin() + i};

            typename A::template const_view<N-1> u(a, origin1);
            typename B::template const_view<N-1> v(b, origin2);
            if (!compare(u, v))
            {
                return false;
            }
        }

        return true;
    }

    template <class A, class B>
    static typename std::enable_if<(A::ndim == B::ndim && A::ndim == 1),bool>::type
    compare(const A& a, const B& b)
    {
        if (a.size(0) != b.size(0))
        {
            return false;
        }
        auto it = b.begin();
        for (auto element : a)
        {
            if (element != *it++)
            {
                return false;
            }
        }

        return true;
    }
};

struct column_major
{
    template <size_t N>
    static void calculate_strides(const std::array<size_t,N>& shape, std::array<size_t,N>& strides, size_t& size)
    {
        size = 1;
        for (size_t i = 0; i < N; ++i)
        {
            strides[i] = size;
            size *= shape[i];
        }
    }

    template <size_t N>
    static void update_offsets(size_t rel, std::array<size_t,N>& offsets)
    {
        offsets[0] += rel;
    }

    template <size_t N, typename Base>
    static void calculate_offsets(size_t rel,
                                  const std::array<size_t,N>& strides, 
                                  const std::array<slice,N>& slices, 
                                  std::array<size_t,N>& offsets)
    {
        offsets[0] = rel + Base::rebase_to_zero(slices[0].start(Base::origin())) * strides[0];
        for (size_t i = 1; i < N; ++i)
        {
            offsets[i] = Base::rebase_to_zero(slices[i].start(Base::origin())) * strides[i]; 
        }
    }

    template <class A, class B>
    static typename std::enable_if<(A::ndim == B::ndim && 
        std::is_same<typename A::value_type, typename B::value_type>::value &&
        std::is_same<typename A::order_type,typename B::order_type>::value &&
                                   (A::ndim>1)),bool>::type
    compare(const A& a, const B& b)
    {
        constexpr size_t N = A::ndim;
        typedef typename A::base_type base_type1;
        typedef typename B::base_type base_type2;

        if (a.size(0) != b.size(0))
        {
            return false;
        }
        for (size_t i = 0; i < a.size(N-1); ++i)
        {
            std::array<size_t,1> origin1 = {base_type1::origin() + i};
            std::array<size_t,1> origin2 = {base_type2::origin() + i};

            std::array<slice,N-1> slices;

            typename A::template const_view<N-1> u(a, slices, origin1);
            typename B::template const_view<N-1> v(b, slices, origin2);
            if (!compare(u, v))
            {
                return false;
            }
        }

        return true;
    }

    template <class A, class B>
    static typename std::enable_if<(A::ndim == B::ndim && A::ndim == 1),bool>::type
    compare(const A& a, const B& b)
    {
        if (a.size(0) != b.size(0))
        {
            return false;
        }
        auto it = b.begin();
        for (auto element : a)
        {
            if (element != *it++)
            {
                return false;
            }
        }

        return true;
    }
};

template <typename T, size_t M, typename Order, typename Base, typename TPtr, typename Enable = void>
struct ndarray_view_member_types
{
    typedef T value_type;
    typedef typename std::conditional<std::is_const<typename std::remove_pointer<TPtr>::type>::value,const T&,T&>::type reference;
    typedef const T& const_reference;
    typedef void iterator;
};

// Forward declarations

template <typename T, size_t M, typename Order = row_major, typename Base = zero_based, typename TPtr = const T*>
class ndarray_view_base;

template <typename T, size_t N, typename Order = row_major, typename Base = zero_based, typename Allocator = std::allocator<T>>
class ndarray;

template <typename T, size_t M, typename Order = row_major, typename Base = zero_based>
class const_ndarray_view;

template <typename T, size_t M, typename Order = row_major, typename Base = zero_based>
class ndarray_view;

template <typename T, typename TPtr>
class iterator_one;

template <typename T, size_t M, typename Order, typename Base, typename TPtr>
struct ndarray_view_member_types<T,M,Order,Base,TPtr,typename std::enable_if<M == 1>::type>
{
    typedef T value_type;
    typedef typename std::conditional<std::is_const<typename std::remove_pointer<TPtr>::type>::value,const T&,T&>::type reference;
    typedef const T& const_reference;
    typedef iterator_one<T,TPtr> iterator;
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
    static void init(std::array<size_t,Array::ndim>& shape, Array& a, size_t n, Args... args)
    {
        shape[Array::ndim - Pos] = n;
        next::init(shape, a, args...);
    }
};

template<>
struct init_helper<0>
{
    template <typename Array>
    static void init(std::array<size_t, Array::ndim>&, Array& a)
    {
        a.init();
    }
    template <typename Array>
    static void init(std::array<size_t, Array::ndim>&, Array& a, typename Array::const_reference val)
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
    typedef T value_type;
    typedef T& reference;
    typedef const T& const_reference;
    static const size_t ndim = N;
    typedef T* iterator;
    typedef const T* const_iterator;
    typedef Order order_type;
    typedef Base base_type;

    template <size_t M> using view = ndarray_view<T,M,Order,Base>;
    template <size_t M> using const_view = const_ndarray_view<T,M,Order,Base>;
private:
    friend struct init_helper<0>;
    friend class ndarray_view<T, N, Order, Base>;
    friend class ndarray_view_base<T, N, Order, Base, T*>;
    friend class ndarray_view_base<T, N, Order, Base, const T*>;

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

    template <typename... Args>
    ndarray(const Allocator& alloc, size_t i, Args... args)
        : super_type(alloc) 
    {
        init_helper<N>::init(shape_, *this, i, args ...);
    }

    explicit ndarray(const std::array<size_t,N>& shape)
        : super_type(allocator_type()), 
          data_(nullptr), shape_(shape)
    {
        Order::calculate_strides(shape_, strides_, num_elements_);
        capacity_ = num_elements_;
        data_ = create(capacity_, get_allocator());
        std::fill(data_, data_+num_elements_, T());
    }

    ndarray(const std::array<size_t,N>& shape, const Allocator& alloc)
        : super_type(alloc), 
          data_(nullptr), shape_(shape)
    {
        Order::calculate_strides(shape_, strides_, num_elements_);
        capacity_ = num_elements_;
        data_ = create(capacity_, get_allocator());
        std::fill(data_, data_+num_elements_, T());
    }

    ndarray(const std::array<size_t,N>& shape, T val)
        : super_type(allocator_type()), 
          data_(nullptr), shape_(shape)
    {
        Order::calculate_strides(shape_, strides_, num_elements_);
        capacity_ = num_elements_;
        data_ = create(capacity_, get_allocator());
        std::fill(data_, data_+num_elements_,val);
    }

    ndarray(const std::array<size_t,N>& shape, T val, const Allocator& alloc)
        : super_type(alloc), 
          data_(nullptr), shape_(shape)
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
    ndarray(const ndarray_view_base<T,N,Order,Base,TPtr>& av)
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
    ndarray(const ndarray_view_base<T,N,Order,Base,TPtr>& av, 
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

    void resize(const std::array<size_t,N>& shape, T value = T())
    {
        T* old_data = data();
        size_t old_size = num_elements();

        shape_ = shape;
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

    void dim_from_initializer_list(const array_item<T>& init, size_t shape)
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
                if (shape < N)
                {
                    shape_[shape++] = init.size();
                    if (is_array)
                    {
                        dim_from_initializer_list(item, shape);
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
                else if (shape != N)
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
print(std::basic_ostream<CharT>& os, ndarray_view_base<T,M,Order,Base,TPtr>& v)
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
template <typename CharT, typename T, typename Order, typename Base, typename TPtr>
void print(std::basic_ostream<CharT>& os, ndarray_view_base<T,1,Order,Base,TPtr>& v)
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

template <typename T, typename TPtr>
class iterator_one
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

    iterator_one()
        : data_(nullptr), stride_(0), offset_(0), p_(nullptr)
    {
    }

    iterator_one(TPtr data, size_t stride, size_t offset)
        : data_(data), stride_(stride), offset_(offset)
    {
        p_ = data_ + offset_;
    }

    iterator_one(const iterator_one&) = default;
    iterator_one(iterator_one&&) = default;
    iterator_one& operator=(const iterator_one&) = default;
    iterator_one& operator=(iterator_one&&) = default;

    iterator_one& operator++()
    {
        p_ += stride_;
        return *this;
    }

    iterator_one operator++(int) // postfix increment
    {
        iterator_one temp(*this);
        ++(*this);
        return temp;
    }

    reference operator*() const
    {
        return *p_;
    }

    friend bool operator==(const iterator_one& it1, const iterator_one& it2)
    {
        return it1.p_ == it2.p_;
    }
    friend bool operator!=(const iterator_one& it1, const iterator_one& it2)
    {
        return !(it1 == it2);
    }
};

template <class T, size_t N, class Order, class Base, typename TPtr>
class row_major_iterator_base
{
    TPtr data_;
    size_t num_elements_;

    std::array<size_t,N> shape_;
    std::array<size_t,N> strides_;
    std::array<size_t,N> offsets_;
    std::array<size_t,N-1> indices_;

    iterator_one<T,TPtr> it_;
    iterator_one<T,TPtr> end_;
public:
    typedef T value_type;
    typedef std::ptrdiff_t difference_type;
    typedef TPtr pointer;
    typedef typename std::conditional<std::is_const<typename std::remove_pointer<TPtr>::type>::value,const T&,T&>::type reference;
    typedef std::input_iterator_tag iterator_category;

    row_major_iterator_base(TPtr data,
                            size_t size, 
                            const std::array<size_t,N>& shape, 
                            const std::array<size_t,N>& strides, 
                            bool end = false)
        : data_(data), num_elements_(size), shape_(shape), strides_(strides)
    {
        offsets_.fill(0);
        initialize(end, std::integral_constant<bool,N==1>());
    }

    row_major_iterator_base(TPtr data,
                            size_t size, 
                            const std::array<size_t,N>& shape, 
                            const std::array<size_t,N>& strides, 
                            const std::array<size_t,N>& offsets,
                            bool end = false)
        : data_(data), num_elements_(size), shape_(shape), strides_(strides), offsets_(offsets)          
    {
        initialize(end, std::integral_constant<bool,N==1>());
    }

    row_major_iterator_base(const row_major_iterator_base&) = default;
    row_major_iterator_base(row_major_iterator_base&&) = default;
    row_major_iterator_base& operator=(const row_major_iterator_base&) = default;
    row_major_iterator_base& operator=(row_major_iterator_base&&) = default;

    row_major_iterator_base& operator++()
    {
        return increment(std::integral_constant<bool,N==1>());
    }

    row_major_iterator_base operator++(int) // postfix increment
    {
        row_major_iterator_base temp(*this);
        ++(*this);
        return temp;
    }

    reference operator*() const
    {
        return *it_;
    }

    friend bool operator==(const row_major_iterator_base& it1, const row_major_iterator_base& it2)
    {
        return it1.it_ == it2.it_;
    }
    friend bool operator!=(const row_major_iterator_base& it1, const row_major_iterator_base& it2)
    {
        return !(it1 == it2);
    }
private:
    void initialize(bool end, std::false_type)
    {
        if (end)
        {
            for (size_t i = 0; i < N-2; ++i)
            {
                indices_[i] = shape_[i]-1;
            }
            indices_[N-2] = shape_[N-2];

        }
        else
        {
            indices_.fill(0);
        }
        ndarray_view_base<T,1,Order,Base,TPtr> v(data_, num_elements_, shape_, strides_, offsets_, indices_);
        it_ = v.begin();
        end_ = v.end();
    }

    void initialize(bool end, std::true_type)
    {
        ndarray_view_base<T,1,Order,Base,TPtr> v(data_, num_elements_, shape_, strides_, offsets_);
        it_ = end ? v.end() : v.begin();
        end_ = v.end();
    }

    row_major_iterator_base& increment(std::false_type)
    {
        if (it_ != end_)
        {
            ++it_;
        }
        if (it_ == end_)
        {
            for (size_t i = N-1; i-- > 0;)
            {
                if (indices_[i]+1 < shape_[i])
                {
                    ++indices_[i];
                    ndarray_view_base<T,1,Order,Base,TPtr> v(data_, num_elements_, shape_, strides_, offsets_, indices_);
                    it_ = v.begin();
                    end_ = v.end();
                    break;
                }
                else if (i > 0)
                {
                    indices_[i] = 0;
                }
            }
        }
        return *this;
    }

    row_major_iterator_base& increment(std::true_type)
    {
        ++it_;
        return *this;
    }
};

template <class T, size_t N, class Order, class Base, typename TPtr>
class column_major_iterator_base
{
    TPtr data_;
    size_t num_elements_;

    std::array<size_t,N> shape_;
    std::array<size_t,N> strides_;
    std::array<size_t,N> offsets_;
    std::array<size_t,N-1> indices_;

    iterator_one<T,TPtr> it_;
    iterator_one<T,TPtr> end_;
public:
    typedef T value_type;
    typedef std::ptrdiff_t difference_type;
    typedef TPtr pointer;
    typedef typename std::conditional<std::is_const<typename std::remove_pointer<TPtr>::type>::value,const T&,T&>::type reference;
    typedef std::input_iterator_tag iterator_category;

    column_major_iterator_base(TPtr data,
                            size_t size, 
                            const std::array<size_t,N>& shape, 
                            const std::array<size_t,N>& strides, 
                            bool end = false)
        : data_(data), num_elements_(size), shape_(shape), strides_(strides)
    {
        offsets_.fill(0);
        initialize(end, std::integral_constant<bool,N==1>());
    }

    column_major_iterator_base(TPtr data,
                            size_t size, 
                            const std::array<size_t,N>& shape, 
                            const std::array<size_t,N>& strides, 
                            const std::array<size_t,N>& offsets,
                            bool end = false)
        : data_(data), num_elements_(size), shape_(shape), strides_(strides), offsets_(offsets)          
    {
        initialize(end, std::integral_constant<bool,N==1>());
    }

    column_major_iterator_base(const column_major_iterator_base&) = default;
    column_major_iterator_base(column_major_iterator_base&&) = default;
    column_major_iterator_base& operator=(const column_major_iterator_base&) = default;
    column_major_iterator_base& operator=(column_major_iterator_base&&) = default;

    column_major_iterator_base& operator++()
    {
        return increment(std::integral_constant<bool,N==1>());
    }

    column_major_iterator_base operator++(int) // postfix increment
    {
        column_major_iterator_base temp(*this);
        ++(*this);
        return temp;
    }

    reference operator*() const
    {
        return *it_;
    }

    friend bool operator==(const column_major_iterator_base& it1, const column_major_iterator_base& it2)
    {
        return it1.it_ == it2.it_;
    }
    friend bool operator!=(const column_major_iterator_base& it1, const column_major_iterator_base& it2)
    {
        return !(it1 == it2);
    }
private:
    void initialize(bool end, std::false_type)
    {
        if (end)
        {
            for (size_t i = 0; i < N-2; ++i)
            {
                indices_[i] = shape_[i]-1;
            }
            indices_[N-2] = shape_[N-2];

        }
        else
        {
            indices_.fill(0);
        }
        ndarray_view_base<T,1,Order,Base,TPtr> v(data_, num_elements_, shape_, strides_, offsets_, indices_);
        it_ = v.begin();
        end_ = v.end();
    }

    void initialize(bool end, std::true_type)
    {
        ndarray_view_base<T,1,Order,Base,TPtr> v(data_, num_elements_, shape_, strides_, offsets_);
        it_ = end ? v.end() : v.begin();
        end_ = v.end();
    }

    column_major_iterator_base& increment(std::false_type)
    {
        if (it_ != end_)
        {
            ++it_;
        }
        if (it_ == end_)
        {
            for (size_t i = 0; i < N-1; ++i)
            {
                if (indices_[i]+1 < shape_[i])
                {
                    ++indices_[i];
                    ndarray_view_base<T,1,Order,Base,TPtr> v(data_, num_elements_, shape_, strides_, offsets_, indices_);
                    it_ = v.begin();
                    end_ = v.end();
                    break;
                }
                else if (i > 0)
                {
                    indices_[i] = 0;
                }
            }
        }
        return *this;
    }

    column_major_iterator_base& increment(std::true_type)
    {
        ++it_;
        return *this;
    }
};

template <class T, size_t N, class Order, class Base, typename TPtr>
class row_major_iterator : public row_major_iterator_base<T,N,Order,Base,T*>
{
    typedef row_major_iterator_base<T, N, Order, Base, T*> super_type;
public:
    using typename super_type::value_type;
    using typename super_type::difference_type;
    using typename super_type::pointer;
    using typename super_type::reference;
    using typename super_type::iterator_category;

    template <typename Allocator>
    row_major_iterator(ndarray<T, N, Order, Base, Allocator>& a, bool end = false)
        : super_type(a.data(), a.num_elements(), a.shape(), a.strides(), end)          
    {
    }
    row_major_iterator(ndarray_view<T, N, Order, Base>& a, bool end = false)
        : super_type(a.data(), a.num_elements(), a.shape(), a.strides(), a.offsets(), end)          
    {
    }
    row_major_iterator(const row_major_iterator&) = default;

    row_major_iterator& operator=(const row_major_iterator&) = default;
};


template <class T, size_t N, class Order, class Base, typename TPtr>
class column_major_iterator : public column_major_iterator_base<T,N,Order,Base,T*>
{
    typedef column_major_iterator_base<T, N, Order, Base, T*> super_type;
public:
    using typename super_type::value_type;
    using typename super_type::difference_type;
    using typename super_type::pointer;
    using typename super_type::reference;
    using typename super_type::iterator_category;

    template <typename Allocator>
    column_major_iterator(ndarray<T, N, Order, Base, Allocator>& a, bool end = false)
        : super_type(a.data(), a.num_elements(), a.shape(), a.strides(), end)          
    {
    }
    column_major_iterator(ndarray_view<T, N, Order, Base>& a, bool end = false)
        : super_type(a.data(), a.num_elements(), a.shape(), a.strides(), a.offsets(), end)          
    {
    }
    column_major_iterator(const column_major_iterator&) = default;

    column_major_iterator& operator=(const column_major_iterator&) = default;
};

// ndarray_view_base

template <typename T, size_t M, typename Order, typename Base, typename TPtr>
class ndarray_view_base 
{
    typedef ndarray_view_member_types<T,M,Order,Base,TPtr> member_types;
public:
    static const size_t ndim = M;
    typedef Order order_type;
    typedef Base base_type;
    typedef typename member_types::value_type value_type;
    typedef typename member_types::const_reference const_reference;
    typedef typename member_types::iterator const_iterator;

    template <size_t K> using const_view = const_ndarray_view<T,K,Order,Base>;
protected:
    TPtr data_;
    size_t num_elements_;
    std::array<size_t,M> shape_;
    std::array<size_t,M> strides_;
    std::array<size_t,M> offsets_;
public:
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

    template <typename TPtr2=TPtr>
    typename std::enable_if<!is_pointer_to_const<TPtr2>::value,T*>::type
    data()
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
protected:
public:
    ndarray_view_base(TPtr data, size_t size, const std::array<size_t,M>& shape, const std::array<size_t,M>& strides)
        : data_(data), num_elements_(size), shape_(shape), strides_(strides)          
    {
        offsets_.fill(0);
    }

    ndarray_view_base(TPtr data, size_t size, const std::array<size_t,M>& shape, const std::array<size_t,M>& strides, const std::array<size_t,M>& offsets)
        : data_(data), num_elements_(size), shape_(shape), strides_(strides), offsets_(offsets)          
    {
    }

    // data

    template<typename OtherTPtr>
    ndarray_view_base(OtherTPtr data, const std::array<size_t,M>& shape,
                       typename std::enable_if<std::is_convertible<OtherTPtr,TPtr>::value>::type* = 0) 
        : data_(data), shape_(shape)
    {
        offsets_.fill(0);
        Order::calculate_strides(shape_, strides_, num_elements_);
    }

    // first_dim

    template<size_t N>
    ndarray_view_base(TPtr data, size_t size, const std::array<size_t,N>& shape, const std::array<size_t,N>& strides,
                      const std::array<size_t,N-M>& first_dim)
        : data_(data), num_elements_(size)
    {
        size_t rel = get_offset<N,N-M,Base>(strides,first_dim);

        //std::cout << "offset: " << offset << "\n";

        for (size_t i = 0; i < M; ++i)
        {
            shape_[i] = shape[(N-M)+i];
            strides_[i] = strides[(N-M)+i];

            //std::cout << "shape " << i << ", size: " << shape_[i] << ", stride: " << strides_[i] << "\n";
        }
        offsets_.fill(0);
        order_type::update_offsets(rel,offsets_);
    }

    template<size_t N>
    ndarray_view_base(TPtr data, size_t size, const std::array<size_t,N>& shape, const std::array<size_t,N>& strides, const std::array<size_t,N>& offsets,
                      const std::array<size_t,N-M>& first_dim)
        : data_(data), num_elements_(size)
    {
        //std::cout << "ndarray_view_base strides: " << other.strides() << ", offsets: " << other.offsets() << ", first_dim: " << first_dim << ", data[0] " << data_[0] << ", size: " << size() << "\n";

        constexpr size_t K = N-M;
        size_t rel = get_offset<N,K,Base>(strides, offsets, first_dim);
        //std::cout << "rel: " << rel << "\n";

        for (size_t i = 0; i < M; ++i)
        {
            shape_[i] = shape[K+i];
            strides_[i] = strides[K+i];
            offsets_[i] = offsets[K+i];
            //std::cout << "shape " << i << ", size: " << shape_[i] << ", stride: " << strides_[i] << ", offset: " << offsets_[i] << "\n";
        }
        order_type::update_offsets(rel,offsets_);
    }

    // first_dim, slices

    template<size_t N>
    ndarray_view_base(TPtr data, size_t size, const std::array<size_t,N>& shape, const std::array<size_t,N>& strides,
                      const std::array<size_t,N-M>& first_dim,
                      const std::array<slice,M>& slices)
        : data_(data), num_elements_(size)
    {
        constexpr size_t K = N-M;
        size_t rel = get_offset<N,K,Base>(strides,first_dim);

        for (size_t i = 0; i < M; ++i)
        {
            shape_[i] = slices[i].length(Base::origin(), shape[K+i]);
            strides_[i] = strides[K+i];
        }
        Order::template calculate_offsets<M,Base>(rel, strides_, slices, offsets_);
        for (size_t i = 0; i < M; ++i)
        {
            strides_[i] *= slices[i].step();
        }
    }

    template<size_t N>
    ndarray_view_base(TPtr data, size_t size, const std::array<size_t,N>& shape, const std::array<size_t,N>& strides, const std::array<size_t,N>& offsets,
                      const std::array<size_t,N-M>& first_dim,
                      const std::array<slice,M>& slices)
        : data_(data), num_elements_(size)
    {
        constexpr size_t K = N-M;
        size_t rel = get_offset<N,K,Base>(strides, offsets ,first_dim);

        for (size_t i = 0; i < M; ++i)
        {
            shape_[i] = slices[i].length(Base::origin(), shape[K+i]);
            strides_[i] = strides[K+i];
        }
        Order::template calculate_offsets<M,Base>(rel, strides_, slices, offsets_);
        for (size_t i = 0; i < M; ++i)
        {
            strides_[i] *= slices[i].step();
        }
    }

    template<size_t N>
    ndarray_view_base(TPtr data, size_t size, const std::array<size_t,N>& shape, const std::array<size_t,N>& strides,
                      const std::array<slice,M>& slices, 
                      const std::array<size_t,N-M>& last_dim)
        : data_(data), num_elements_(size)
    {
        constexpr size_t K = N-M;

        std::array<size_t,N> indices;
        indices.fill(Base::origin());
        for (size_t i = 0; i < K; ++i)
        {
            indices[i+M] = last_dim[i];
        }

        size_t rel = get_offset<N,N,Base>(strides,indices);

        for (size_t i = 0; i < M; ++i)
        {
            shape_[i] = slices[i].length(Base::origin(), shape[i]);
            strides_[i] = strides[i];
        }
        Order::template calculate_offsets<M,Base>(rel, strides_, slices, offsets_);
        for (size_t i = 0; i < M; ++i)
        {
            strides_[i] *= slices[i].step();
        }
    }

    template<size_t N>
    ndarray_view_base(TPtr data, size_t size, const std::array<size_t,N>& shape, const std::array<size_t,N>& strides, const std::array<size_t,N>& offsets,
                      const std::array<slice,M>& slices, const std::array<size_t,N-M>& last_dim)
        : data_(data), num_elements_(size)
    {
        constexpr size_t K = N-M;

        std::array<size_t,N> indices;
        indices.fill(Base::origin());
        for (size_t i = 0; i < K; ++i)
        {
            indices[i+M] = last_dim[i];
        }

        size_t rel = get_offset<N,N,Base>(strides, offsets, indices);

        for (size_t i = 0; i < M; ++i)
        {
            shape_[i] = slices[i].length(Base::origin(), shape[i]);
            strides_[i] = strides[i];
        }
        Order::template calculate_offsets<M,Base>(rel, strides_, slices, offsets_);
        for (size_t i = 0; i < M; ++i)
        {
            strides_[i] *= slices[i].step();
        }
    }
    template <typename Allocator>
    ndarray_view_base& operator=(const ndarray<T, M, Order, Base, Allocator>& a) = delete;

    template<typename OtherTPtr>
    ndarray_view_base& operator=(const ndarray_view_base<T, M, Order, Base, OtherTPtr>& other) = delete;

    void assign(TPtr data, size_t size, const std::array<size_t,M>& shape, const std::array<size_t,M>& strides)
    {
        data_ = data; 
        num_elements_ = size; 
        shape_ = shape; 
        strides_ = strides;
        offsets_.fill(0);
    }

    void assign(TPtr data, size_t size, const std::array<size_t,M>& shape, const std::array<size_t,M>& strides, const std::array<size_t,M>& offsets)
    {
        data_ = data; 
        num_elements_ = size; 
        shape_ = shape; 
        strides_ = strides;
        offsets_ = offsets;
    }
};

template <typename CharT, typename T, size_t M, typename Order, typename Base, typename TPtr>
std::basic_ostream<CharT>& operator<<(std::basic_ostream<CharT>& os, 
                                      ndarray_view_base<T, M, Order, Base, TPtr>& v)
{
    print(os,v);
    return os;
}

template <typename T, size_t M, typename Order, typename Base>
class ndarray_view : public ndarray_view_base<T, M, Order, Base, T*>
{
    typedef ndarray_view_member_types<T,M,Order,Base,T*> member_types;
    typedef ndarray_view_base<T, M, Order, Base, T*> super_type;
public:
    using typename super_type::value_type;
    using typename super_type::order_type;
    using typename super_type::base_type;
    using typename super_type::const_reference;
    using typename super_type::const_iterator;
    typedef typename member_types::reference reference;
    typedef typename member_types::iterator iterator;

    template <size_t K> using view = ndarray_view<T,K,Order,Base>;

    template <size_t K> using const_view = const_ndarray_view<T,K,Order,Base>;

    template <typename Allocator>
    ndarray_view(ndarray<T, M, Order, Base, Allocator>& a)
        : super_type(a.data(), a.num_elements(), a.shape(), a.strides())          
    {
    }

    ndarray_view(ndarray_view<T, M, Order, Base>& a)
        : super_type(a.data(), a.num_elements(), a.shape(), a.strides(), a.offsets())          
    {
    }

    template<typename Allocator>
    ndarray_view(ndarray<T, M, Order, Base, Allocator>& a, 
                 const std::array<slice,M>& slices)
        : super_type(a.data(), a.num_elements(), a.shape(), a.strides(),
                     slices, std::array<size_t,0>())
    {
    }

    ndarray_view(ndarray_view<T, M, Order, Base>& a, 
                 const std::array<slice,M>& slices)
        : super_type(a.data(), a.num_elements(), a.shape(), a.strides(), a.offsets(),
                     slices, std::array<size_t,0>())
    {
    }

    template<size_t N, typename Allocator>
    ndarray_view(ndarray<T, N, Order, Base, Allocator>& a, 
                 const std::array<size_t,N-M>& first_dim)
        : super_type(a.data(), a.num_elements(), a.shape(), a.strides(),
                     first_dim)
    {
    }

    template<size_t N>
    ndarray_view(ndarray_view<T, N, Order, Base>& a, 
                 const std::array<size_t,N-M>& first_dim)
        : super_type(a.data(), a.num_elements(), a.shape(), a.strides(), a.offsets(),
                     first_dim)
    {
    }

    template<size_t N, typename Allocator>
    ndarray_view(ndarray<T, N, Order, Base, Allocator>& a, 
                 const std::array<size_t,N-M>& first_dim,
                 const std::array<slice,M>& slices)
        : super_type(a.data(), a.num_elements(), a.shape(), a.strides(),
                     first_dim, slices)
    {
    }

    template<size_t N>
    ndarray_view(ndarray_view<T, N, Order, Base>& a, 
                 const std::array<size_t,N-M>& first_dim,
                 const std::array<slice,M>& slices)
        : super_type(a.data(), a.num_elements(), a.shape(), a.strides(), a.offsets(),
                     first_dim, slices)
    {
    }

    template<size_t N, typename Allocator>
    ndarray_view(ndarray<T, N, Order, Base, Allocator>& a,
                 const std::array<slice,M>& slices, 
                 const std::array<size_t,N-M>& last_dim)
        : super_type(a.data(), a.num_elements(), a.shape(), a.strides(),
                     slices, last_dim)
    {
    }

    template<size_t N>
    ndarray_view(ndarray_view<T, N, Order, Base>& a,
                 const std::array<slice,M>& slices, 
                 const std::array<size_t,N-M>& last_dim)
        : super_type(a.data(), a.num_elements(), a.shape(), a.strides(), a.offsets(),
                     slices, last_dim)
    {
    }

    ndarray_view(T* data, const std::array<size_t,M>& shape) 
        : super_type(data, shape)
    {
    }

    template <typename Allocator>
    ndarray_view& operator=(ndarray<T, M, Order, Base, Allocator>& a)
    {
        this->assign(a.data(), a.num_elements(), a.shape(), a.strides());
        return *this;
    }

    ndarray_view& operator=(ndarray_view<T, M, Order, Base>& a)
    {
        this->assign(a.data(), a.num_elements(), a.shape(), a.strides(), a.offsets());
        return *this;
    }

    template <typename... Indices>
    T& operator()(size_t index, Indices... indices) 
    {
        size_t off = get_offset<M, Base, 0>(this->strides_, this->offsets_, index, indices...);
        assert(off < this->num_elements());
        return this->data_[off];
    }

    T& operator()(const std::array<size_t,M>& indices) 
    {
        size_t off = get_offset<M, M, Base>(this->strides_, this->offsets_, indices);
        assert(off < this->num_elements());
        return this->data_[off];
    }

    template <typename... Indices>
    const T& operator()(size_t index, Indices... indices) const
    {
        size_t off = get_offset<M, Base, 0>(this->strides_, this->offsets_, index, indices...);
        assert(off < this->num_elements());
        return this->data_[off];
    }

    const T& operator()(const std::array<size_t, M>& indices) const
    {
        size_t off = get_offset<M, M, Base>(this->strides_, this->offsets_, indices);
        assert(off < this->num_elements());
        return this->data_[off];
    }

    iterator begin()
    {
        return iterator(this->data_,this->strides_[0],this->offsets_[0]);
    }

    iterator end() 
    {
        return iterator(this->data_,this->strides_[0],(this->offsets_[0]+this->size(0)*this->strides_[0]));
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

template <typename T, size_t M, typename Order, typename Base>
class const_ndarray_view : public ndarray_view_base<T, M, Order, Base, const T*>
{
    typedef ndarray_view_member_types<T,M,Order,Base,const T*> member_types;
    typedef ndarray_view_base<T, M, Order, Base, const T*> super_type;
public:
    using typename super_type::value_type;
    using typename super_type::order_type;
    using typename super_type::base_type;
    using typename super_type::const_reference;
    using typename super_type::const_iterator;
    typedef typename member_types::reference reference;
    typedef typename member_types::iterator iterator;

    template <size_t K> using view = ndarray_view<T,K,Order,Base>;

    template <size_t K> using const_view = const_ndarray_view<T,K,Order,Base>;

    template <typename Allocator>
    const_ndarray_view(const ndarray<T, M, Order, Base, Allocator>& a)
        : super_type(a.data(), a.num_elements(), a.shape(), a.strides())        
    {
    }

    template<typename Allocator>
    const_ndarray_view(const ndarray<T, M, Order, Base, Allocator>& a, 
                       const std::array<slice,M>& slices)
        : super_type(a.data(), a.num_elements(), a.shape(), a.strides(),
                     slices, std::array<size_t,0>())        
    {
    }

    template<size_t N, typename Allocator>
    const_ndarray_view(const ndarray<T, N, Order, Base, Allocator>& a, 
                       const std::array<size_t,N-M>& first_dim)
        : super_type(a.data(), a.num_elements(), a.shape(), a.strides(),
                     first_dim)
    {
    }

    template<typename OtherTPtr>
    const_ndarray_view(const ndarray_view_base<T, M, Order, Base, OtherTPtr>& other)
        : super_type(other.data(), other.num_elements(), other.shape(), other.strides(), other.offsets())        
    {
    }

    template<typename OtherTPtr>
    const_ndarray_view(const ndarray_view_base<T, M, Order, Base, OtherTPtr>& other, 
                       const std::array<slice,M>& slices)
        : super_type(other.data(), other.num_elements(), other.shape(), other.strides(), other.offsets(),
                     slices, std::array<size_t,0>())        
    {
    }

    template<size_t m = M, size_t N, typename OtherTPtr>
    const_ndarray_view(const ndarray_view_base<T, N, Order, Base, OtherTPtr>& other, 
                       const std::array<size_t,N-m>& first_dim)
        : super_type(other.data(), other.num_elements(), other.shape(), other.strides(), other.offsets(),
                     first_dim)
    {
    }

    template<size_t N, typename Allocator>
    const_ndarray_view(const ndarray<T, N, Order, Base, Allocator>& other, 
                       const std::array<size_t,N-M>& first_dim,
                       const std::array<slice,M>& slices)
        : super_type(other.data(), other.num_elements(), other.shape(), other.strides(),
                     first_dim, slices)
    {
    }

    template<size_t N, typename OtherTPtr>
    const_ndarray_view(const ndarray_view_base<T, N, Order, Base, OtherTPtr>& other, 
                       const std::array<size_t,N-M>& first_dim,
                       const std::array<slice,M>& slices)
        : super_type(other.data(), other.num_elements(), other.shape(), other.strides(), other.offsets(),
                     first_dim, slices)
    {
    }

    template<size_t N, typename Allocator>
    const_ndarray_view(const ndarray<T, N, Order, Base, Allocator>& other,
                       const std::array<slice,M>& slices, 
                       const std::array<size_t,N-M>& last_dim)
        : super_type(other.data(), other.num_elements(), other.shape(), other.strides(),
                     slices, last_dim)
    {
    }

    template<size_t N, typename OtherTPtr>
    const_ndarray_view(const ndarray_view_base<T, N, Order, Base, OtherTPtr>& other,
                       const std::array<slice,M>& slices, 
                       const std::array<size_t,N-M>& last_dim)
        : super_type(other.data(), other.num_elements(), other.shape(), other.strides(), other.offsets(),
                     slices, last_dim)
    {
    }

    template<typename OtherTPtr>
    const_ndarray_view(OtherTPtr data, const std::array<size_t,M>& shape,
                       typename std::enable_if<std::is_convertible<OtherTPtr,const T*>::value>::type* = 0) 
        : super_type(data, shape)
    {
    }

    template <typename Allocator>
    const_ndarray_view& operator=(const ndarray<T, M, Order, Base, Allocator>& a)
    {
        this->assign(a.data(), a.num_elements(), a.shape(), a.strides());
        return *this;
    }

    template<typename OtherTPtr>
    const_ndarray_view& operator=(const ndarray_view_base<T, M, Order, Base, OtherTPtr>& other)
    {
        this->assign(other.data(), other.num_elements(), other.shape(), other.strides(), other.offsets());
        return *this;
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

   return Order::compare(lhs, rhs);

   //return Order::compare(lhs.data(), lhs.shape(), lhs.strides(), offsets,
   //                       rhs.data(), rhs.shape(), lhs.strides(), offsets);
}

template <typename T, size_t N, typename Order, typename Base, typename Allocator>
bool operator!=(const ndarray<T, N, Order, Base, Allocator>& lhs, 
                const ndarray<T, N, Order, Base, Allocator>& rhs)
{
    return !(lhs == rhs);
}

template <typename T, size_t M, typename Order, typename Base, typename TPtr>
bool operator==(const ndarray_view_base<T, M, Order, Base, TPtr>& lhs, 
                const ndarray_view_base<T, M, Order, Base, TPtr>& rhs)
{
    if (&lhs == &rhs)
    {
        return true;
    }

    return Order::compare(lhs, rhs);

    //return Order::compare(lhs.data(), lhs.shape(), lhs.strides(), lhs.offsets(),
    //                      rhs.data(), rhs.shape(), lhs.strides(), rhs.offsets());
}

template <typename T, size_t M, typename Order, typename Base, typename Allocator, typename TPtr>
bool operator==(const ndarray<T, M, Order, Base, Allocator>& lhs, 
                const ndarray_view_base<T, M, Order, Base, TPtr>& rhs)
{
    std::array<size_t,M> offsets;
    offsets.fill(0);

    return Order::compare(lhs, rhs);
    //return Order::compare(lhs.data(), lhs.shape(), lhs.strides(), offsets,
    //                      rhs.data(), rhs.shape(), lhs.strides(), rhs.offsets());
}

template <typename T, size_t M, typename Order, typename Base, typename Allocator, typename TPtr>
bool operator==(const ndarray_view_base<T, M, Order, Base, TPtr>& lhs, 
                const ndarray<T, M, Order, Base, Allocator>& rhs)
{
    std::array<size_t,M> offsets;
    offsets.fill(0);

    return Order::compare(lhs, rhs);
    //return Order::compare(lhs.data(), lhs.shape(), lhs.strides(), lhs.offsets(),
    //                      rhs.data(), rhs.shape(), lhs.strides(), offsets);
}

template <typename T, size_t M, typename Order, typename Base, typename TPtr>
bool operator!=(const ndarray_view_base<T, M, Order, Base, TPtr>& lhs, 
                const ndarray_view_base<T, M, Order, Base, TPtr>& rhs)
{
    return !(lhs == rhs);
}

template <typename T, size_t M, typename Order, typename Base, typename Allocator, typename TPtr>
bool operator!=(const ndarray<T, M, Order, Base, Allocator>& lhs, 
                const ndarray_view_base<T, M, Order, Base, TPtr>& rhs)
{
    return !(lhs == rhs);
}

template <typename T, size_t M, typename Order, typename Base, typename Allocator, typename TPtr>
bool operator!=(const ndarray_view_base<T, M, Order, Base, TPtr>& lhs, 
                const ndarray<T, M, Order, Base, Allocator>& rhs)
{
    return !(lhs == rhs);
}

}

#endif
