#include <catch/catch.hpp>
#include <iostream>
#include "acons/ndarray.hpp"
#include <sstream>

using namespace acons;

struct iterator_state2
{
    size_t first;
    size_t last;
    size_t current;
    size_t n;

    iterator_state2()
        : first(0), last(0), current(0), n(0)
    {
    }

    iterator_state2(size_t first, size_t last, size_t current)
        : first(first), last(last), current(current), n(0)
    {
    }

    iterator_state2(size_t first, size_t last, size_t current, size_t n)
        : first(first), last(last), current(current), n(n)
    {
    }

    iterator_state2(const iterator_state2&) = default;

    friend std::ostream& operator<<(std::ostream& os, const iterator_state2& s)
    {
        os << "first: " << s.first << ", last: " << s.last << ", current: " << s.current << ", n: " << s.n;
        return os;
    }
};

template <class T, size_t N, typename TPtr>
class row_major_iterator2
{
    TPtr data_;
    size_t num_elements_;

    std::array<size_t,N> shape_;
    std::array<size_t,N> strides_;
    std::array<size_t,N> offsets_;

    std::vector<iterator_state2> stack_; 

    iterator_one<T,TPtr> first_;
    iterator_one<T,TPtr> last_;
    iterator_one<T,TPtr> it_;
public:
    typedef T value_type;
    static constexpr size_t ndim = N;
    typedef std::ptrdiff_t difference_type;
    typedef TPtr pointer;
    typedef typename std::conditional<std::is_const<typename std::remove_pointer<TPtr>::type>::value,const T&,T&>::type reference;
    typedef std::input_iterator_tag iterator_category;

    row_major_iterator2(const row_major_iterator2<T,N,TPtr>& other, bool end)
        : data_(other.data_), num_elements_(other.num_elements_), shape_(other.shape_), strides_(other.strides_), offsets_(other.offsets_)
    {
        if (end)
        {
            std::array<size_t,N> origin;
            for (size_t i = 0; i < N; ++i)
            {
                origin[i] = shape_[i]-1;
            }
            size_t end_rel = get_offset<N,N,zero_based>(strides_,offsets_,origin);
            first_ = iterator_one<T,TPtr>(data_,strides_[N-1],end_rel + strides_[N-1]);
            last_ = first_;
            it_ = first_;
        }
        else
        {
            initialize(iterator_position::begin, std::integral_constant<bool,N==1>());
        }
    }

    row_major_iterator2(TPtr data,
                       size_t size, 
                       const std::array<size_t,N>& shape, 
                       const std::array<size_t,N>& strides, 
                       iterator_position dir)
        : data_(data), num_elements_(size), shape_(shape), strides_(strides)
    {
        offsets_.fill(0);
        initialize(dir, std::integral_constant<bool,N==1>());
    }

    row_major_iterator2(TPtr data,
                       size_t size, 
                       const std::array<size_t,N>& shape, 
                       const std::array<size_t,N>& strides, 
                       const std::array<size_t,N>& offsets, 
                       iterator_position dir)
        : data_(data), num_elements_(size), shape_(shape), strides_(strides), offsets_(offsets)
    {
        initialize(dir, std::integral_constant<bool,N==1>());
    }

    row_major_iterator2(const row_major_iterator2&) = default;
    row_major_iterator2(row_major_iterator2&&) = default;
    row_major_iterator2& operator=(const row_major_iterator2&) = default;
    row_major_iterator2& operator=(row_major_iterator2&&) = default;

    row_major_iterator2& operator++()
    {
        return increment(std::integral_constant<bool,N==1>());
    }

    row_major_iterator2 operator++(int) // postfix increment
    {
        row_major_iterator2 temp(*this);
        ++(*this);
        return temp;
    }

    row_major_iterator2& operator--()
    {
        return decrement(std::integral_constant<bool,N==1>());
    }

    row_major_iterator2 operator--(int) // postfix increment
    {
        row_major_iterator2 temp(*this);
        --(*this);
        return temp;
    }

    reference operator*() const
    {
        return *it_;
    }

    friend bool operator==(const row_major_iterator2& it1, const row_major_iterator2& it2)
    {
        return it1.it_ == it2.it_;
    }
    friend bool operator!=(const row_major_iterator2& it1, const row_major_iterator2& it2)
    {
        return !(it1 == it2);
    }

private:
    void initialize(iterator_position dir, std::false_type)
    {
        switch (dir)
        {
            case iterator_position::rbegin:
                //for (size_t i = 0; i < N; ++i)
                //{
                //    indices_[i] = shape_[i]-1;
                //}
                break;
            default:
                stack_.emplace_back(offsets_[0], offsets_[0]+strides_[0]*shape_[0], offsets_[0]);
                break;
        }

        increment(std::false_type());
    }

    void initialize(iterator_position dir, std::true_type)
    {
        //std::cout << "initialize: shape_[0]: " << shape_[0] << ", strides_[0]: " << strides_[0] << ", offsets_[0]: " << offsets_[0]  << ", offset_one_end_: " << offset_one_end_ << "\n"; 
        switch (dir)
        {
            case iterator_position::rbegin:
                //indices_[0] = shape_[0]-1;
                break;
            default:
                //indices_[0] = 0;
                break;
        }

        increment(std::false_type());
    }

    row_major_iterator2& increment(std::false_type)
    {
        if (it_ != last_)
        {
            ++it_;
        }
        if (it_ == last_ && !stack_.empty())
        {
            bool done = false;
            while (!stack_.empty() && !done)
            {
                iterator_state2 top = stack_.back();
                stack_.pop_back();
                if (top.n+1 < N)
                {
                    if (top.current != top.last)
                    {
                        iterator_state2 state(top.first,top.last,top.current+strides_[top.n],top.n);
                        stack_.push_back(state);
                        top.n = top.n+1;
                        top.first = top.current + offsets_[top.n];
                        top.last = top.first + strides_[top.n]*shape_[top.n];
                        top.current = top.first;
                        stack_.push_back(top);
                    }
                }
                else
                {
                    if (top.current != top.last)
                    {
                        first_ = iterator_one<T,TPtr>(data_,strides_[N-1],top.first);
                        last_ = iterator_one<T,TPtr>(data_,strides_[N-1],top.last);
                        it_ = first_;
                    }
                    done = true;
                }
            }
        }
        return *this;
    }

    row_major_iterator2& increment(std::true_type)
    {
        ++it_;
        return *this;
    }
};

template <class T, size_t N, class Order, class Base>
row_major_iterator2<T,N,const T*> make_row_major_iterator2(const ndarray<T,N,Order,Base>& v)
{
    return row_major_iterator2<T,N,const T*>(v.data(),v.size(),v.shape(),v.strides(),iterator_position::begin);
}

template <class T, size_t N, class Order, class Base>
row_major_iterator2<T,N,T*> make_row_major_iterator2(ndarray<T,N,Order,Base>& v)
{
    return row_major_iterator2<T,N,T*>(v.data(),v.size(),v.shape(),v.strides(),iterator_position::begin);
}

template <class T, size_t N, class Order, class Base, class TPtr>
row_major_iterator2<T,N,const T*> make_row_major_iterator2(const ndarray_view_base<T,N,Order,Base,TPtr>& v)
{
    return row_major_iterator2<T,N,const T*>(v.base_data(),v.base_size(),v.shape(),v.strides(),v.offsets(),iterator_position::begin);
}

template <class T, size_t N, class Order, class Base, class TPtr>
row_major_iterator2<T,N,TPtr> make_row_major_iterator2(ndarray_view_base<T,N,Order,Base,TPtr>& v)
{
    return row_major_iterator2<T,N,TPtr>(v.base_data(),v.base_size(),v.shape(),v.strides(),v.offsets(),iterator_position::begin);
}

template <class T, size_t N, typename TPtr>
row_major_iterator2<T,N,TPtr> end(const row_major_iterator2<T,N,TPtr>& it) noexcept
{
    return row_major_iterator2<T,N,TPtr>(it,true);
}

TEST_CASE("4 x 5 new row_major_iterator2 tests")
{
    typedef ndarray<double,2> array_t;

    array_t a = {{0,1,2,3,4},{5,6,7,8,9},{10,11,12,13,14},{15,16,17,18,19}};
    std::cout << "a: " << a << "\n\n";

    const double* data = a.data();
    size_t size = a.size();

    SECTION("2x2 view")
    {
        constexpr size_t N = 2;

        array_t::view<N> v(a, { slice(1,3),slice(2,4) });

        std::cout << "v: " << v << "\n";
        std::array<size_t,2> shape = v.shape();
        std::array<size_t,2> strides = v.strides();
        std::array<size_t,2> offsets = v.offsets();

        std::cout << "shape: " << shape << "\n";
        std::cout << "strides: " << strides << "\n";
        std::cout << "offsets: " << offsets << "\n";

        size_t n = 0;

        auto it = make_row_major_iterator2(v);
        auto last = end(it);
        std::cout << *it++ << "\n";
        std::cout << *it++ << "\n";
        std::cout << *it++ << "\n";
        std::cout << *it++ << "\n";
        CHECK(it == last);
    }
}
