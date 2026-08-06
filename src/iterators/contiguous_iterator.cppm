export module swtl.contiguous_iterator;

import std;

namespace swtl
{

/// @brief A lightweight contiguous and random-access iterator wrapping a raw
/// pointer.
///
/// Satisfies the `std::contiguous_iterator` iterator concept and the
/// `std::random_access_iterator` iterator category. Suitable for use with
/// contiguous containers such as vectors, arrays, or spans.
///
/// @tparam T The type of element referenced by this iterator.
///
export template <typename T>
class ContiguousIterator
{
public:
   /// @name Iterator Traits
   /// @{

   /// Iterator concept.
   using iterator_concept = std::contiguous_iterator_tag;
   /// Legacy iterator category.
   using iterator_category = std::random_access_iterator_tag;
   using value_type = std::remove_cv_t<T>;  ///< Value type.
   using difference_type = std::ptrdiff_t;  ///< Signed difference type.
   using pointer = T *;                     ///< Pointer type.
   using reference = T &;                   ///< Reference type.

   /// @}

   template <typename U>
   friend class ContiguousIterator;

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
   constexpr ContiguousIterator(ContiguousIterator<U> const &other)
       : ptr_{ other.ptr_ }
   {}

   /// @brief Default constructor. Initializes with a null pointer.
   ///
   constexpr ContiguousIterator() = default;

   /// @brief Constructs an iterator from a raw pointer.
   ///
   /// @param ptr The raw pointer to wrap.
   ///
   constexpr explicit ContiguousIterator(pointer ptr)
       : ptr_{ ptr }
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
      return *ptr_;
   }

   /// @brief Accesses a member of the underlying element.
   ///
   /// @pre The iterator must be dereferenceable (i.e., not null or end).
   ///
   /// @return A pointer to the element referenced by this iterator.
   ///
   [[nodiscard]]
   constexpr pointer
   operator->() const noexcept
   {
      return ptr_;
   }

   /// @brief Accesses the element at the specified relative index.
   ///
   /// @param idx The relative offset from the current iterator position.
   ///
   /// @pre (this + idx) must result in a dereferenceable iterator.
   ///
   /// @return A reference to the element at offset `idx`.
   ///
   [[nodiscard]]
   constexpr reference
   operator[](difference_type idx) const noexcept
   {
      return ptr_[idx];
   }

   /// @brief Pre-increment operator. Advances the iterator by one element.
   ///
   /// @pre The iterator must not be at the end of the underlying range.
   ///
   /// @return A reference to this updated iterator.
   ///
   constexpr ContiguousIterator &
   operator++() noexcept
   {
      ++ptr_;
      return *this;
   }

   /// @brief Post-increment operator. Advances the iterator by one element.
   ///
   /// @pre The iterator must not be at the end of the underlying range.
   ///
   /// @return A copy of the iterator prior to advancement.
   ///
   constexpr ContiguousIterator
   operator++(int) noexcept
   {
      auto temp{ *this };
      ++ptr_;
      return temp;
   }

   /// @brief Pre-decrement operator. Moves the iterator backward by one
   /// element.
   ///
   /// @pre The iterator must not be at the beginning of the underlying range.
   ///
   /// @return A reference to this updated iterator.
   ///
   constexpr ContiguousIterator &
   operator--() noexcept
   {
      --ptr_;
      return *this;
   }

   /// @brief Post-decrement operator. Moves the iterator backward by one
   /// element.
   ///
   /// @pre The iterator must not be at the beginning of the underlying range.
   ///
   /// @return A copy of the iterator prior to decrementing.
   ///
   constexpr ContiguousIterator
   operator--(int) noexcept
   {
      auto temp{ *this };
      --ptr_;
      return temp;
   }

   /// @brief Advances the iterator by a specified distance.
   ///
   /// @param distance The number of elements to advance forward.
   ///
   /// @pre Advancing by `distance` must not exceed the one-past-the-end
   /// position of the underlying range.
   ///
   /// @return A reference to this updated iterator.
   ///
   constexpr ContiguousIterator &
   operator+=(difference_type distance) noexcept
   {
      ptr_ += distance;
      return *this;
   }

   /// @brief Advances the iterator by a specified distance.
   ///
   /// @param distance The number of elements to advance forward.
   ///
   /// @pre Moving backward by `distance` must not precede the beginning of the
   /// underlying range.
   ///
   /// @return A reference to this updated iterator.
   ///
   constexpr ContiguousIterator &
   operator-=(difference_type distance) noexcept
   {
      ptr_ -= distance;
      return *this;
   }

   /// @brief Returns a new iterator advanced by `distance` elements.
   ///
   /// @param lhs The base iterator.
   /// @param distance The number of elements to advance.
   ///
   /// @pre Advancing by `distance` must not exceed the one-past-the-end
   /// position of the underlying range.
   ///
   /// @return A new `ContiguousIterator` at the advanced position.
   ///
   [[nodiscard]]
   constexpr friend ContiguousIterator
   operator+(ContiguousIterator const &lhs, difference_type distance) noexcept
   {
      return lhs.ptr_ + distance;
   }

   /// @brief Returns a new iterator advanced by `distance` elements.
   ///
   /// @param distance The number of elements to advance.
   /// @param rhs The base iterator.
   ///
   /// @pre Advancing by `distance` must not exceed the one-past-the-end
   /// position of the underlying range.
   ///
   /// @return A new `ContiguousIterator` at the advanced position.
   ///
   [[nodiscard]]
   constexpr friend ContiguousIterator
   operator+(difference_type distance, ContiguousIterator const &rhs) noexcept
   {
      return rhs + distance;
   }

   /// @brief Returns a new iterator moved backward by `distance` elements.
   ///
   /// @param lhs The base iterator.
   /// @param distance The number of elements to move backward.
   ///
   /// @pre Moving backward by `distance` must not precede the beginning of the
   /// underlying range.
   ///
   /// @return A new `ContiguousIterator` at the decremented position.
   ///
   [[nodiscard]]
   constexpr friend ContiguousIterator
   operator-(ContiguousIterator const &lhs, difference_type distance) noexcept
   {
      return lhs.ptr_ - distance;
   }

   /// @brief Computes the distance (number of elements) between two iterators.
   ///
   /// @param lhs The first iterator.
   /// @param rhs The second iterator.
   ///
   /// @pre The result of `lhs - rhs` must be representable by
   /// `difference_type`.
   ///
   /// @return The signed difference (`lhs - rhs`) in elements.
   ///
   [[nodiscard]]
   constexpr friend difference_type
   operator-(
       ContiguousIterator const &lhs, ContiguousIterator const &rhs) noexcept
   {
      return lhs.ptr_ - rhs.ptr_;
   }

   /// @brief Three-way comparison operator.
   ///
   /// @param lhs The left-hand iterator.
   /// @param rhs The right-hand iterator.
   ///
   /// @return The relative ordering of the underlying pointers.
   ///
   [[nodiscard]]
   constexpr friend auto
   operator<=>(
       ContiguousIterator const &lhs, ContiguousIterator const &rhs) noexcept
       = default;

private:
   pointer ptr_{};  ///< @brief Underlying raw pointer to the element.
};

// Ensures that the iterator meets the
// requirements for the appropriate iterator tag.
static_assert(std::contiguous_iterator<ContiguousIterator<int>>);
static_assert(std::contiguous_iterator<ContiguousIterator<int const>>);

}  // namespace swtl
