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
    size_t stop_;
    size_t step_;
public:
    slice()
        : start_(0), stop_(0), step_(0)
    {
    }
    slice(size_t start, size_t stop, size_t step)
        : start_(start), stop_(stop), step_(step)
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
            stop_ = *it++;
            step_ = list.size() == 3 ? *it : 1;
        }
        else
        {
            throw std::invalid_argument("Too many arguments to slice");
        }
    }
    slice(const slice& other)
        : start_(other.start_), stop_(other.stop_), step_(other.step_)
    {
    }

    slice& operator=(const slice& other) = default;

    size_t start() const
    {
        return start_;
    }
    size_t stop() const
    {
        return stop_;
    }
    size_t stride() const
    {
        return step_;
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
    return (Base::rebase_to_zero(index)+offsets[n-1])*strides[n-1];
}

template <size_t n, typename Base, size_t m, typename... Indices>
typename std::enable_if<(m+1 < n), size_t>::type
get_offset(const std::array<size_t,n>& strides, 
           const std::array<size_t, n>& offsets, 
           size_t index, Indices... indices)
{
    const size_t mplus1 = m + 1;
    size_t i = (Base::rebase_to_zero(index)+offsets[m])*strides[m] + get_offset<n, Base, mplus1>(strides,offsets,indices...);

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
        offset += (Base::rebase_to_zero(indices[i])+offsets[i])*strides[i];
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

struct walk_snapshot
{
    size_t start;
    size_t size;
    size_t dim;

    walk_snapshot()
        : start(0), size(0), dim(0)
    {
    }

    walk_snapshot(size_t start, size_t size, size_t dim)
        : start(start), size(size), dim(dim)
    {
    }

    walk_snapshot(const walk_snapshot&) = default;

    friend std::ostream& operator<<(std::ostream& os, const walk_snapshot& s)
    {
        os << "[" << s.start << "," << s.size << "," << s.dim << "]";
        return os;
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

    template <size_t N>
    static void initialize_walk(std::vector<walk_snapshot>& stack,
                                const std::array<size_t,N>& dimensions,
                                const std::array<size_t,N>& strides,
                                const std::array<size_t,N>& offsets)
    {
        stack.emplace_back(offsets[0]*strides[0], dimensions[0], 0);
    }

    template <size_t N, typename Callable>
    static void walk(std::vector<walk_snapshot>& stack, 
                     const std::array<size_t,N>& dimensions, 
                     const std::array<size_t,N>& strides, 
                     const std::array<size_t,N>& offsets, 
                     Callable callable)
    {
        while (!stack.empty())
        {
            walk_snapshot current = stack.back();
            /*std::cout << "stack: ";
            for (size_t i = 0; i < stack.size(); ++i)
            {
                if (i > 0)
                {
                    std::cout << ",";
                }
                std::cout << stack[i];
            }
            std::cout << "\n";*/
            stack.pop_back();

            if (current.dim+1 < N)
            {
                if (dimensions[current.dim+1]*strides[current.dim+1] != strides[current.dim])
                {
                    for (size_t j = dimensions[current.dim]; j-- > 0;)
                    {
                        size_t start = current.start + offsets[current.dim+1]*strides[current.dim+1] + j*strides[current.dim];
                        size_t len = dimensions[current.dim+1];

                        /* std::cout << "current: " 
                                  << current 
                                  << ", j: " 
                                  << j 
                                  << ", offsets[current.dim+1]: " 
                                  << offsets[current.dim+1] 
                                  << ", start: " 
                                  << start << "\n"; */
                        stack.emplace_back(start,len,current.dim+1);
                    }
                }
                else
                {
                    size_t start = current.start + offsets[current.dim+1]*strides[current.dim+1];
                    //std::cout << "path2 current:" << current << "\n";
                    //size_t len = 1;
                    //for (size_t j = 0; j <= current.dim+1; ++j)
                    //{
                    //    len *= dimensions[j];
                    //}
                    size_t len = current.size * dimensions[current.dim+1];
                    stack.emplace_back(start,len,current.dim+1);
                }
            }
            else
            {
                size_t stride = strides[N-1];

                //std::cout << "current: " << current  << ", stride: " << stride << "\n";
                callable(current.start, current.start + current.size*stride, stride);
                break;
            }
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

    template <size_t N>
    static void initialize_walk(std::vector<walk_snapshot>& stack,
                                const std::array<size_t,N>& dimensions,
                                const std::array<size_t,N>& strides,
                                const std::array<size_t,N>& offsets)
    {
        stack.emplace_back(offsets[N-1]*strides[N-1], dimensions[N-1], N-1);
    }

    template <size_t N, typename Callable>
    static void walk(std::vector<walk_snapshot>& stack, 
                     const std::array<size_t,N>& dimensions, 
                     const std::array<size_t,N>& strides, 
                     const std::array<size_t,N>& offsets, 
                     Callable callable)
    {
        while (!stack.empty())
        {
            walk_snapshot current = stack.back();
            /*std::cout << "stack: ";
            for (size_t i = 0; i < stack.size(); ++i)
            {
                if (i > 0)
                {
                    std::cout << ",";
                }
                std::cout << stack[i];
            }
            std::cout << "\n";*/
            stack.pop_back();

            if (current.dim > 0)
            {
                if (dimensions[current.dim-1]*strides[current.dim-1] != strides[current.dim])
                {
                    for (size_t j = dimensions[current.dim]; j-- > 0;)
                    {
                        size_t start = current.start + offsets[current.dim-1]*strides[current.dim-1] + j*strides[current.dim];
                        size_t len = dimensions[current.dim-1];

                        /*std::cout << "current: " 
                                  << current 
                                  << ", j: " 
                                  << j 
                                  << ", offsets[current.dim-1]: " 
                                  << offsets[current.dim-1] 
                                  << ", start: " 
                                  << start << "\n";*/
                        stack.emplace_back(start,len,current.dim-1);
                    }
                }
                else
                {
                    size_t start = current.start + offsets[current.dim-1]*strides[current.dim-1];
                    //std::cout << "path2 current:" << current << "\n";
                    //size_t len = 1;
                    //for (size_t j = 0; j <= current.dim; ++j)
                    //{
                    //    len *= dimensions[j];
                    //}
                    //size_t len = current.size * dimensions[current.dim+1];
                    size_t len = current.size * dimensions[current.dim-1];
                    stack.emplace_back(start,len,current.dim-1);
                }
            }
            else
            {
                size_t stride = strides[0];

                //std::cout << "current: " << current  << ", stride: " << stride << "\n";
                callable(current.start, current.start + current.size*stride, stride);
                break;
            }
        }
    }
};

template <typename T, size_t N, typename Order=row_major, typename Base=zero_based, typename Allocator=std::allocator<T>>
class ndarray;

template <typename T, size_t M, typename Order=row_major, typename Base=zero_based, typename TPtr=const T*>
class const_ndarray_view;  

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
    typedef ndarray_base<Allocator> super_type;
public:
    using typename super_type::allocator_type;
    using typename super_type::pointer;
    typedef T value_type;
    typedef T& reference;
    typedef const T& const_reference;
    static const size_t dimension = N;
    typedef T* iterator;
    typedef const T* const_iterator;
    typedef Order order_type;
    typedef Base base_type;
private:
    friend struct init_helper<0>;
    friend class ndarray_view<T, N, Order, Base>;
    friend class const_ndarray_view<T, N, Order, Base, T*>;
    friend class const_ndarray_view<T, N, Order, Base, const T*>;

    pointer data_;
    size_t size_;
    size_t capacity_;
    std::array<size_t,N> dim_;
    std::array<size_t,N> strides_;
public:
    using super_type::get_allocator;

    ndarray()
        : super_type(allocator_type()),
          data_(nullptr), size_(0), capacity_(0)
    {
        dim_.fill(0);
        strides_.fill(0);
    }

    template <typename... Args>
    ndarray(size_t i, Args... args)
        : super_type(allocator_type()) 
    {
        init_helper<N>::init(dim_, *this, i, args ...);
    }

    explicit ndarray(const std::array<size_t,N>& dim)
        : super_type(allocator_type()), 
          data_(nullptr), dim_(dim)
    {
        Order::calculate_strides(dim_, strides_, size_);
        capacity_ = size_;
        data_ = create(capacity_, get_allocator());
        std::fill(data_, data_+size_, T());
    }

    ndarray(const std::array<size_t,N>& dim, const Allocator& alloc)
        : super_type(alloc), 
          data_(nullptr), dim_(dim)
    {
        Order::calculate_strides(dim_, strides_, size_);
        capacity_ = size_;
        data_ = create(capacity_, get_allocator());
        std::fill(data_, data_+size_, T());
    }

    ndarray(const std::array<size_t,N>& dim, T val)
        : super_type(allocator_type()), 
          data_(nullptr), dim_(dim)
    {
        Order::calculate_strides(dim_, strides_, size_);
        capacity_ = size_;
        data_ = create(capacity_, get_allocator());
        std::fill(data_, data_+size_,val);
    }

    ndarray(const std::array<size_t,N>& dim, T val, const Allocator& alloc)
        : super_type(alloc), 
          data_(nullptr), dim_(dim)
    {
        Order::calculate_strides(dim_, strides_, size_);

        capacity_ = size_;
        data_ = create(capacity_, get_allocator());
        std::fill(data_, data_+size_, val);
    }

    ndarray(std::initializer_list<array_item<T>> list) 
        : super_type(allocator_type())
    {
        dim_from_initializer_list(list, 0);

        Order::calculate_strides(dim_, strides_, size_);
        capacity_ = size_;
        data_ = create(capacity_, get_allocator());
        std::array<size_t,N> indices;
        data_from_initializer_list(list,indices,0);
    }

    ndarray(std::initializer_list<array_item<T>> list, const Allocator& alloc) 
        : super_type(alloc)
    {
        dim_from_initializer_list(list, 0);

        // Initialize multipliers and size
        Order::calculate_strides(dim_, strides_, size_);
        capacity_ = size_;
        data_ = create(capacity_, get_allocator());
        std::array<size_t,N> indices;
        data_from_initializer_list(list,indices,0);
    }

    ndarray(const ndarray& other)
        : super_type(std::allocator_traits<allocator_type>::select_on_container_copy_construction(other.get_allocator())), 
          data_(nullptr), size_(other.size()), capacity_(0), dim_(other.dim_), strides_(other.strides_)          
    {
        capacity_ = size_;
        data_ = create(capacity_, get_allocator());

#if defined(_MSC_VER)
        std::copy(other.data_, other.data_+other.size_,stdext::make_checked_array_iterator(data_,size_));
#else 
        std::copy(other.data_,other.data_+other.size_,data_);
#endif
    }

    ndarray(const ndarray& other, const Allocator& alloc)
        : super_type(alloc), 
          data_(nullptr), size_(other.size()), dim_(other.dim_), strides_(other.strides_)          
    {
        capacity_ = size_;
        data_ = create(capacity_, get_allocator());

#if defined(_MSC_VER)
        std::copy(other.data_, other.data_+other.size_,stdext::make_checked_array_iterator(data_,size_));
#else 
        std::copy(other.data_,other.data_+other.size_,data_);
#endif
    }

    ndarray(ndarray&& other)
        : super_type(other.get_allocator()), 
          data_(other.data_), size_(other.size_), capacity_(other.capacity_), dim_(other.dim_), strides_(other.strides_)          
    {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
        other.dim_.fill(0);
        other.strides_.fill(0);
    }

    ndarray(ndarray&& other, const Allocator& alloc)
        : super_type(alloc), 
          data_(nullptr), size_(other.size()), capacity_(other.capacity_), dim_(other.dim_), strides_(other.strides_)          
    {
        if (alloc == other.get_allocator())
        {
            data_ = other.data_;
            other.data_ = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
            other.dim_.fill(0);
            other.strides_.fill(0);
        }
        else
        {
            capacity_ = size_;
            data_ = create(capacity_, get_allocator());
#if defined(_MSC_VER)
            std::copy(other.data_, other.data_+other.size_,stdext::make_checked_array_iterator(data_,size_));
#else 
            std::copy(other.data_,other.data_+other.size_,data_);
#endif
        }
    }

    template <typename TPtr>
    ndarray(const const_ndarray_view<T,N,Order,Base,TPtr>& av)
        : super_type(allocator_type()), 
          data_(nullptr), size_(0), capacity_(0), dim_(av.dimensions()), strides_(av.strides())          
    {
        size_ = av.size();
        capacity_ = size_;
        data_ = create(capacity_, get_allocator());

#if defined(_MSC_VER)
        std::copy(av.data(), av.data()+av.size(),stdext::make_checked_array_iterator(data_,size_));
#else 
        std::copy(av.data(),av.data()+av.size(),data_);
#endif
    }

    template <typename TPtr>
    ndarray(const const_ndarray_view<T,N,Order,Base,TPtr>& av, 
            const Allocator& alloc)
        : super_type(alloc), 
          data_(nullptr), size_(0), capacity_(0), dim_(av.dim_), strides_(av.strides_)          
    {
        size_ = av.size();
        capacity_ = size_;
        data_ = create(capacity_, get_allocator());

#if defined(_MSC_VER)
        std::copy(av.data(), av.data()+av.size(),stdext::make_checked_array_iterator(data_,size_));
#else 
        std::copy(av.data(),av.data()+av.size(),data_);
#endif
    }

    ~ndarray()
    {
        get_allocator().deallocate(to_plain_pointer(data_), capacity_);
    }

    iterator begin()
    {
        return data_;
    }

    const_iterator begin() const
    {
        return data_;
    }

    iterator end()
    {
        return data_ + size_;
    }

    const_iterator end() const
    {
        return data_ + size_;
    }

    void resize(const std::array<size_t,N>& dim, T value = T())
    {
        T* old_data = data();
        size_t old_size = size();

        dim_ = dim;
        Order::calculate_strides(dim_, strides_, size_);

        if (size_ > capacity_)
        {
            capacity_ = size_;
            data_ = create(capacity_, get_allocator());
        }

        size_t len = (std::min)(old_size,size_);

#if defined(_MSC_VER)
        std::copy(old_data, old_data+len,stdext::make_checked_array_iterator(data_,size_));
#else 
        std::copy(old_data, old_data+len,data_);
#endif
        if (len < size_)
        {
            std::fill(data_ + len, data_+size_, value);
        }

        if (size_ > old_size)
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

        Order::calculate_strides(dim_, strides_, size_);
        capacity_ = size_;
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
        std::swap(size_,other.size_);
        std::swap(capacity_,other.capacity_);
        std::swap(dim_,other.dim_);
        std::swap(strides_,other.strides_);

    }

    bool empty() const noexcept
    {
        return size_ == 0;
    }

    size_t size() const noexcept
    {
        return size_;
    }

    size_t capacity() const noexcept
    {
        return capacity_;
    }

    const std::array<size_t,N>& dimensions() const {return dim_;}

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
        if (size() != other.size())
        {
            get_allocator().deallocate(to_plain_pointer(data_),capacity_);
            size_ = other.size();
            capacity_ = size_;
            data_ = create(capacity_, get_allocator());
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
        get_allocator().deallocate(to_plain_pointer(data_),capacity_);
        size_ = other.size();
        capacity_ = size_;
        data_ = create(capacity_, get_allocator());
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
            get_allocator().deallocate(to_plain_pointer(data_),capacity_);
            size_ = other.size();
            capacity_ = size_;
            data_ = create(capacity_, get_allocator());
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
        capacity_ = size_;
        data_ = create(capacity_, get_allocator());
    }

    void init(const T& val)
    {
        Order::calculate_strides(dim_, strides_, size_);
        capacity_ = size_;
        data_ = create(capacity_, get_allocator());
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

template <typename CharT, typename A>
void print(std::basic_ostream<CharT>& os, A& a)
{
    os << '[';
    if (A::dimension == 1)
    {
        auto begin = a.begin();
        auto end = a.end();
        for (auto it = a.begin(); it != a.end(); ++it)
        {
            if (it != begin)
            {
                os << ',';
            }
            os << *it;
        }
    }
    else
    {
        std::array<size_t,A::dimension-1> indices;
        indices.fill(0);
        print(os, a, indices, 0);
    }
    os << ']';
}


template <typename CharT, typename A>
void print(std::basic_ostream<CharT>& os, 
           A& a, 
           std::array<size_t,A::dimension-1>& indices, 
           size_t i)
{
    if (i+1 < A::dimension)
    {
        for (size_t j = 0; j < a.size(i); ++j)
        {
            if (j > 0)
            {
                os << ',';
            }
            os << '[';
            indices[i] = j;
            print(os, a, indices, i+1);
            os << ']';
        }
    }
    else
    {
        const_ndarray_view<typename A::value_type, 1, typename A::order_type, typename A::base_type> v(a,indices);

        auto begin = v.begin();
        auto end = v.end();
        for (auto it = begin; it != end; ++it)
        {
            if (it != begin)
            {
                os << ',';
            }
            os << *it;
        }
/*
        for (size_t j = 0; j < a.size(i); ++j)
        {
            if (j > 0)
            {
                os << ',';
            }
            indices[i] = j;
            os << a(indices);
        }
 */
    }   
}
 

template <typename T, size_t N, typename Order, typename Base, typename Allocator, typename CharT>
std::basic_ostream<CharT>& operator<<(std::basic_ostream<CharT>& os, ndarray<T, N, Order, Base, Allocator>& a)
{
    //auto f = [&](const std::array<size_t,N>& indices) 
    //{ 
    //    size_t off = get_offset<N, N, zero_based>(a.strides(),indices);
    //    return a.data()[off];
    //};
    //print(os, a.dimensions(), f);
    print(os, a);
    return os;
}

template <typename T, size_t N, typename Order, bool IsConst = false>
class slice_iterator
{
    static const size_t npos = size_t(-1);
public:
    typedef std::forward_iterator_tag iterator_category;
    typedef ptrdiff_t difference_type;
    typedef typename std::conditional<IsConst, const T&, T&>::type reference;
    typedef typename std::conditional<IsConst, const T*, T*>::type pointer;
private:
    pointer data_;
    std::array<size_t,N> dim_;
    std::array<size_t,N> strides_;
    std::array<size_t,N> offsets_;
    std::vector<walk_snapshot> stack_;

    size_t offset_;
    size_t end_offset_;
    size_t step_;
public:

    slice_iterator()
        : data_(nullptr), offset_(npos), end_offset_(npos), step_(0)
    {
    }

    slice_iterator(pointer data, 
                   const std::array<size_t,N>& dimensions, 
                   const std::array<size_t,N>& strides, 
                   const std::array<size_t,N>& offsets)
        : data_(data), dim_(dimensions), strides_(strides), offsets_(offsets), 
          offset_(0), end_offset_(0), step_(0)
    {
        Order:: template initialize_walk(stack_, dim_, strides_, offsets_);
        auto f = [&](size_t o, size_t endo, size_t stride)
        {
            offset_ = o;
            end_offset_ = endo;
            step_ = stride;
        };
        Order::walk(stack_, dim_, strides_, offsets_, f);
    }

    slice_iterator(const slice_iterator<T,N,Order>& other) 
        : data_(other.data_), dim_(other.dim_), strides_(other.strides_), offsets_(other.offsets_), 
          stack_(other.stack_), 
          offset_(other.offset_), end_offset_(other.end_offset_), step_(other.step_)
    {
    } 

    slice_iterator<T, N, Order, IsConst>& operator++()
    {
        if (offset_ + step_ < end_offset_)
        {
            offset_ += step_;
        }
        else
        {
            offset_ = end_offset_ = npos;
            auto f = [&](size_t o, size_t endo, size_t stride)
            {
                offset_ = o;
                end_offset_ = endo;
                step_ = stride;
            };
            Order::walk(stack_, dim_, strides_, offsets_, f);
        }
        return *this;
    }

    slice_iterator<T, N, Order, IsConst> operator++(int)
    {
        slice_iterator<T,N,Order,IsConst> temp(*this);
        ++(*this);
        return temp;
    }

    reference operator*() const
    {
        return *(data_+offset_);
    }

    friend bool operator==(const slice_iterator<T,N,Order,IsConst>& it1, const slice_iterator<T,N,Order,IsConst>& it2)
    {
        return it1.offset_ == it2.offset_;
    }

    friend bool operator!=(const slice_iterator<T,N,Order,IsConst>& it1, const slice_iterator<T,N,Order,IsConst>& it2)
    {
        return !(it1 == it2);
    }
private:
};

template <typename T, size_t N, typename Order>
bool compare(const T* data1, const std::array<size_t,N>& dim1, const std::array<size_t,N>& strides1, const std::array<size_t,N>& offsets1, 
             const T* data2, const std::array<size_t,N>& dim2, const std::array<size_t,N>& strides2, const std::array<size_t,N>& offsets2)
{
    for (size_t i = 0; i < N; ++i)
    {
        if (dim1[i] != dim2[i])
        {
            return false;
        }
    }

    slice_iterator<T,N,Order,true> it1(data1,dim1,strides1,offsets1);
    slice_iterator<T,N,Order,true> end;
    slice_iterator<T,N,Order,true> it2(data2,dim2,strides2,offsets2);

    bool are_equal = true;
    while (are_equal && (it1 != end))
    {
        if (*it1++ != *it2++)
        {
            are_equal = false;
        }
    }
    return are_equal;
}

template <typename T, size_t M, typename Order, typename Base, typename TPtr>
class const_ndarray_view 
{
public:
    static const size_t dimension = M;
    typedef T value_type;
    typedef const T& const_reference;
    typedef Order order_type;
    typedef Base base_type;
    typedef slice_iterator<T,M,Order,true> const_iterator;
    typedef array_wrapper<slice,M> slices_type;
protected:
    TPtr data_;
    size_t size_;
    std::array<size_t,M> dim_;
    std::array<size_t,M> strides_;
    std::array<size_t,M> offsets_;
public:
    const_ndarray_view()
        : data_(nullptr), size_(0)
    {
        dim_.fill(0);
        strides_.fill(0);
        offsets_.fill(0);
    }

    template <typename Allocator>
    const_ndarray_view(ndarray<T, M, Order, Base, Allocator>& a)
        : data_(a.data()), size_(a.size()), dim_(a.dimensions()), strides_(a.strides())          
    {
        offsets_.fill(0);
    }

    template<size_t m = M, size_t N, typename Allocator>
    const_ndarray_view(ndarray<T, N, Order, Base, Allocator>& a, 
                       const slices_type& slices)
        : data_(a.data()), size_(a.size())
    {
        static_assert(m == N, "A view constructed from a list of index ranges on an array must have the same dimensionality as the array");
        for (size_t i = 0; i < M; ++i)
        {
            dim_[i] = (slices[i].stop() - slices[i].start()) /slices[i].stride();
            strides_[i] = a.strides()[i]*slices[i].stride();
            offsets_[i] = Base::rebase_to_zero(slices[i].start()); 
        }
    }

    template<size_t m = M, size_t N, size_t K, typename Allocator>
    const_ndarray_view(ndarray<T, N, Order, Base, Allocator>& a, 
                       const std::array<size_t,K>& origin)
        : data_(nullptr), size_(0)
    {
        static_assert(m == N-K, "View must have dimension N-K");
        size_t offset = get_offset<N,N-M,Base>(a.strides(),origin);

        data_ = a.data() + offset;
        size_ = a.size() - offset;

        for (size_t i = 0; i < M; ++i)
        {
            dim_[i] = a.dimensions()[(N-M)+i];
            strides_[i] = a.strides()[(N-M)+i];
        }
        offsets_.fill(0);
    }

    template<size_t m = M, size_t N, size_t K, typename Allocator>
    const_ndarray_view(ndarray<T, N, Order, Base, Allocator>& a, 
                       const std::array<size_t,K>& origin,
                       const slices_type& slices)
        : data_(nullptr), size_(0)
    {
        static_assert(m == N-K, "View must have dimension N-K");

        size_t offset = get_offset<N,N-M,Base>(a.strides(),origin);

        data_ = a.data() + offset;
        size_ = a.size() - offset;

        for (size_t i = 0; i < M; ++i)
        {
            dim_[i] = (slices[i].stop() - slices[i].start())/slices[i].stride();
            strides_[i] = a.strides()[(N-M)+i]*slices[i].stride();
            offsets_[i] = Base::rebase_to_zero(slices[i].start());
        }
    }

    template<typename OtherTPtr>
    const_ndarray_view(const_ndarray_view<T, M, Order, Base, OtherTPtr>& other)
        : data_(other.data()), size_(other.size()), dim_(other.dimensions()), strides_(other.strides())          
    {
        offsets_.fill(0);
    }

    template<size_t m = M, size_t N, typename OtherTPtr>
    const_ndarray_view(const_ndarray_view<T, N, Order, Base, OtherTPtr>& other, 
                       const slices_type& slices)
        : data_(other.data()), size_(other.size())
    {
        static_assert(m == N, "A view constructed from a list of index ranges on another view must have the same dimensionality as the other view");

        for (size_t i = 0; i < M; ++i)
        {
            dim_[i] = (slices[i].stop() - slices[i].start())/slices[i].stride();
            strides_[i] = other.strides()[i]*slices[i].stride();
            offsets_[i] = other.offsets()[i] + Base::rebase_to_zero(slices[i].start());
        }
    }

    template<size_t m = M, size_t N, size_t K, typename OtherTPtr>
    const_ndarray_view(const_ndarray_view<T, N, Order, Base, OtherTPtr>& other, 
                       const std::array<size_t,K>& origin)
        : data_(other.data()), size_(other.size())
    {
        static_assert(m == N-K, "View must have dimension N-K");

        size_t rel = get_offset<N,N-M,Base>(other.strides(),other.offsets(),origin);

        for (size_t i = 0; i < M; ++i)
        {
            dim_[i] = other.dimensions()[(N-M)+i];
            strides_[i] = other.strides()[(N-M)+i];
            offsets_[i] = rel + other.offsets()[(N-M)+i];
        }
    }

    template<size_t m = M, size_t N, size_t K, typename OtherTPtr>
    const_ndarray_view(const_ndarray_view<T, N, Order, Base, OtherTPtr>& other, 
                       const std::array<size_t,K>& origin,
                       const slices_type& slices)
        : data_(other.data()), size_(other.size())
    {
        static_assert(m == N-K, "View must have dimension N-K");

        size_t rel = get_offset<N,N-M,Base>(other.strides(),other.offsets(),origin);

        for (size_t i = 0; i < M; ++i)
        {
            dim_[i] = (slices[i].stop() - slices[i].start())/slices[i].stride();
            strides_[i] = other.strides()[(N-M)+i]*slices[i].stride();
            offsets_[i] = rel + other.offsets()[(N-M)+i] + Base::rebase_to_zero(slices[i].start());
        }
    }

    template<typename OtherTPtr>
    const_ndarray_view(OtherTPtr data, const std::array<size_t,M>& dim) 
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

    const T* data() const 
    {
        return data_;
    }

    size_t size(size_t i) const
    {
        assert(i < dim_.size());
        return dim_[i];
    }

    const_iterator begin() const
    {
        return const_iterator(data_, dim_, strides_, offsets_);
    }

    const_iterator end() const
    {
        return const_iterator();
    }

    const_iterator cbegin() const
    {
        return const_iterator(data_, dim_, strides_, offsets_);
    }

    const_iterator cend() const
    {
        return const_iterator();
    }

    template <typename... Indices>
    const T& operator()(size_t index, Indices... indices) const
    {
        size_t off = get_offset<M, Base, 0>(strides_, offsets_, index, indices...);
        assert(off < size());
        return data_[off];
    }

    const T& operator()(const std::array<size_t,M>& indices) const 
    {
        size_t off = get_offset<M, M, Base>(strides_, offsets_, indices);
        assert(off < size());
        return data_[off];
    }
};

template <typename CharT, typename T, size_t M, typename Order, typename Base, typename TPtr>
std::basic_ostream<CharT>& operator<<(std::basic_ostream<CharT>& os, 
                                      const_ndarray_view<T, M, Order, Base, TPtr>& v)
{
    /* auto f = [&](const std::array<size_t,M>& indices) 
    { 
        size_t off = get_offset<M, M, zero_based>(v.strides(), v.offsets(), indices);
        return v.data()[off];
    };
    print(os, v.dimensions(), f);*/
    print(os, v);
    return os;
}

template <typename T, size_t M, typename Order, typename Base>
class ndarray_view : public const_ndarray_view<T, M, Order, Base, T*>
{
    typedef const_ndarray_view<T, M, Order, Base, T*> super_type;
public:
    typedef slice_iterator<T,M,Order,true> iterator;
    using typename super_type::const_iterator;
    using typename super_type::slices_type;
    using typename super_type::value_type;
    using typename super_type::order_type;
    using typename super_type::base_type;
public:
    ndarray_view()
        : super_type()
    {
    }

    template <typename Allocator>
    ndarray_view(ndarray<T, M, Order, Base, Allocator>& a)
        : super_type(a)          
    {
    }

    template<size_t N, typename Allocator>
    ndarray_view(ndarray<T, N, Order, Base, Allocator>& a, 
                 const slices_type& slices)
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
                 const slices_type& slices)
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
                 const slices_type& slices)
        : super_type(a, slices)
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
                 const std::array<size_t,N-M>& origin,
                 const slices_type& slices)
        : super_type(a, origin, slices)
    {
    }

    ndarray_view(T* data, const std::array<size_t,M>& dim) 
        : super_type(data, dim)
    {
    }

    using super_type::data; 
    using super_type::size;
    using super_type::empty;
    using super_type::dimensions;
    using super_type::strides;
    using super_type::offsets;
    using super_type::begin;
    using super_type::end;
    using super_type::operator();

    T* data()
    {
        return this->data_;
    }

    iterator begin()
    {
        return iterator(this->data_, this->dim_, this->strides_, this->offsets_);
    }

    iterator end()
    {
        return iterator();
    }

    template <typename... Indices>
    T& operator()(size_t index, Indices... indices) 
    {
        size_t off = get_offset<M, Base, 0>(this->strides_, this->offsets_, index, indices...);
        assert(off < size());
        return this->data_[off];
    }

    T& operator()(const std::array<size_t,M>& indices) 
    {
        size_t off = get_offset<M, M, Base>(this->strides_, this->offsets_, indices);
        assert(off < size());
        return this->data_[off];
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

    return compare<T,M,Order>(lhs.data(), lhs.dimensions(), lhs.strides(), lhs.offsets(),
                   rhs.data(), rhs.dimensions(), lhs.strides(), rhs.offsets());
}

template <typename T, size_t M, typename Order, typename Base, typename Allocator, typename TPtr>
bool operator==(const ndarray<T, M, Order, Base, Allocator>& lhs, 
                const const_ndarray_view<T, M, Order, Base, TPtr>& rhs)
{
    std::array<size_t,M> offsets;
    offsets.fill(0);
    return compare<T,M,Order>(lhs.data(), lhs.dimensions(), lhs.strides(), offsets,
                          rhs.data(), rhs.dimensions(), lhs.strides(), rhs.offsets());
}

template <typename T, size_t M, typename Order, typename Base, typename Allocator, typename TPtr>
bool operator==(const const_ndarray_view<T, M, Order, Base, TPtr>& lhs, 
                const ndarray<T, M, Order, Base, Allocator>& rhs)
{
    std::array<size_t,M> offsets;
    offsets.fill(0);
    return compare<T,M,Order>(lhs.data(), lhs.dimensions(), lhs.strides(), lhs.offsets(),
                          rhs.data(), rhs.dimensions(), lhs.strides(), offsets);
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
