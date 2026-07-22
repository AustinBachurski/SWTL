# Strictly Worse Template Library

The Strictly Worse Template Library is my learning playground for implementing C++ Standard Template Library components for the purpose of learning their internals.

# swtl::Vector

Currently working on an allocator aware clone of std::vector.

```cpp
export template <typename T, typename Allocator = std::allocator<T>>
class Vector : protected VectorBase<T, Allocator>
{
private:
   using Base = VectorBase<T, Allocator>;
   using a_traits = std::allocator_traits<Allocator>;

public:
   // ** MEMBER TYPES **
   using value_type = std::remove_cv_t<T>;
   using allocator_type = Base::allocator_type;
   using size_type = std::size_t;
   using difference_type = std::ptrdiff_t;
   using reference = value_type &;
   using const_reference = value_type const &;
   using pointer = Base::pointer;
   using const_pointer = a_traits::const_pointer;
   using iterator = VectorIterator<T>;
   using const_iterator = VectorIterator<T const>;
   using reverse_iterator = std::reverse_iterator<iterator>;
   using const_reverse_iterator = std::reverse_iterator<const_iterator>;
```
