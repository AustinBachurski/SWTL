export module swtl.test.helpers:iterators;

import std;

/// @cond INTERNAL_DOCUMENTATION

export namespace swtl::test_helpers
{

/// @brief A gimped input_iterator wrapping a raw pointer.
///
/// Satisfies the `input_iterator` concept and the `std::input_iterator_tag`
/// iterator category.
///
/// @warning This is not a valid std::input_iterator, it is basically a gimped
/// random_access_iterator for purposes of exercising branches that take lesser
/// iterators.
///
template <typename T>
class InputIterator
{
public:
   /// @name Iterator Traits
   /// @{

   /// Iterator concept.
   using iterator_concept = std::input_iterator_tag;

   /// Legacy iterator category.
   using iterator_category = std::input_iterator_tag;

   /// Value type.
   using value_type = std::remove_cv_t<T>;

   /// Signed difference type.
   using difference_type = std::ptrdiff_t;

   /// Pointer type.
   using pointer = T *;

   /// Reference type.
   using reference = T &;

   /// @}

   template <typename U>
   friend class InputIterator;

   /// @brief Implicit converting constructor from a non-const to a const
   /// iterator.
   ///
   /// @tparam U The value type of the source iterator.
   ///
   /// @param other The iterator to convert from.
   ///
   /// @note This constructor is only enabled when `T` is const-qualified and
   /// `U` is the same type without const qualification.
   ///
   template <typename U>
      requires std::is_const_v<T> && std::same_as<U, std::remove_const_t<T>>
   constexpr InputIterator(InputIterator<U> const &other)
       : m_ptr{ other.m_ptr }
   {}

   /// @brief Default constructor. Initializes with a null pointer.
   ///
   constexpr InputIterator() = default;

   /// @brief Constructs an iterator from a raw pointer.
   ///
   /// @param ptr The raw pointer to wrap.
   ///
   constexpr InputIterator(pointer ptr)
       : m_ptr{ ptr }
   {}

   /// @brief Dereferences the iterator.
   ///
   /// @pre The iterator must be dereferenceable (i.e., not null or end).
   ///
   /// @return A reference to the value pointed to by this iterator.
   ///
   [[nodiscard]]
   constexpr reference
   operator*() const noexcept
   {
      return *m_ptr;
   }

   /// @brief Pre-increment operator. Advances the iterator by one element.
   ///
   /// @pre The iterator must not be at the end of the underlying range.
   ///
   /// @return A reference to this updated iterator.
   ///
   constexpr InputIterator &
   operator++() noexcept
   {
      ++m_ptr;
      return *this;
   }

   /// @brief Post-increment operator. Advances the iterator by one element.
   ///
   /// @pre The iterator must not be at the end of the underlying range.
   ///
   /// @return A copy of the iterator prior to advancement.
   ///
   constexpr InputIterator
   operator++(int) noexcept
   {
      auto temp{ *this };
      ++m_ptr;
      return temp;
   }

   /// @brief Equality comparison operator.
   ///
   /// @param lhs The left-hand iterator.
   /// @param rhs The right-hand iterator.
   ///
   /// @return `lhs == rhs`.
   ///
   [[nodiscard]]
   constexpr friend bool
   operator==(InputIterator const &lhs, InputIterator const &rhs) noexcept
   {
      return lhs.m_ptr == rhs.m_ptr;
   }

private:
   pointer m_ptr{};  ///< @brief Underlying raw pointer to the element.
};

// Ensures that the iterator meets the
// requirements for the appropriate iterator tag.
static_assert(std::input_iterator<InputIterator<int>>);
static_assert(std::input_iterator<InputIterator<int const>>);

}  // namespace swtl::test_helpers

/// @endcond INTERNAL_DOCUMENTATION
