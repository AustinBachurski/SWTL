export module swtl.contiguous_iterator;

import std;

namespace swtl
{

///
export template <typename T>
class ContiguousIterator
{
public:
   using iterator_concept = std::contiguous_iterator_tag;
   using iterator_category = std::random_access_iterator_tag;
   using value_type = std::remove_cv_t<T>;
   using difference_type = std::ptrdiff_t;
   using pointer = T *;
   using reference = T &;

   template <typename U>
   friend class ContiguousIterator;

   template <typename U>
      requires std::is_const_v<T> && std::same_as<U, std::remove_const_t<T>>
   constexpr ContiguousIterator(ContiguousIterator<U> const &other)
       : ptr_{ other.ptr_ }
   {}

   constexpr ContiguousIterator() = default;

   constexpr explicit ContiguousIterator(pointer ptr)
       : ptr_{ ptr }
   {}

   [[nodiscard]]
   constexpr reference
   operator*() const noexcept
   {
      return *ptr_;
   }

   [[nodiscard]]
   constexpr pointer
   operator->() const noexcept
   {
      return ptr_;
   }

   [[nodiscard]]
   constexpr reference
   operator[](difference_type idx) const noexcept
   {
      return ptr_[idx];
   }

   constexpr ContiguousIterator &
   operator++() noexcept
   {
      ++ptr_;
      return *this;
   }

   constexpr ContiguousIterator
   operator++(int) noexcept
   {
      auto temp{ *this };
      ++ptr_;
      return temp;
   }

   constexpr ContiguousIterator &
   operator--() noexcept
   {
      --ptr_;
      return *this;
   }

   constexpr ContiguousIterator
   operator--(int) noexcept
   {
      auto temp{ *this };
      --ptr_;
      return temp;
   }

   constexpr ContiguousIterator &
   operator+=(difference_type distance) noexcept
   {
      ptr_ += distance;
      return *this;
   }

   constexpr ContiguousIterator &
   operator-=(difference_type distance) noexcept
   {
      ptr_ -= distance;
      return *this;
   }

   [[nodiscard]]
   constexpr friend auto
   operator+(ContiguousIterator const &lhs, difference_type distance) noexcept
       -> ContiguousIterator
   {
      return ContiguousIterator{ lhs.ptr_ + distance };
   }

   [[nodiscard]]
   constexpr friend auto
   operator+(difference_type distance, ContiguousIterator const &rhs) noexcept
       -> ContiguousIterator
   {
      return ContiguousIterator{ rhs + distance };
   }

   [[nodiscard]]
   constexpr friend auto
   operator-(ContiguousIterator const &lhs, difference_type distance) noexcept
       -> ContiguousIterator
   {
      return ContiguousIterator{ lhs.ptr_ - distance };
   }

   [[nodiscard]]
   constexpr friend auto
   operator-(
       ContiguousIterator const &lhs, ContiguousIterator const &rhs) noexcept
       -> difference_type
   {
      return lhs.ptr_ - rhs.ptr_;
   }

   [[nodiscard]]
   constexpr friend auto
   operator<=>(
       ContiguousIterator const &lhs, ContiguousIterator const &rhs) noexcept
       = default;

private:
   pointer ptr_{};
};

// Ensures that the iterator meets the
// requirements for the appropriate iterator tag.
static_assert(std::contiguous_iterator<ContiguousIterator<int>>);
static_assert(std::contiguous_iterator<ContiguousIterator<int const>>);

}  // namespace swtl
