export module swtl.memory:alloc_aware_uninitialized;

import std;

import :concepts;
import :alloc_aware_destroy;
import :alloc_aware_guards;

namespace swtl
{

/// @brief Copies elements in the range `[first, last)` to uninitialized memory
/// via the allocator.
///
/// Copy-constructs elements from the source range into uninitialized storage
/// starting at `dest` using `std::allocator_traits<Allocator>::construct`.
///
/// @note This function initializes object lifetimes but does not allocate
/// storage.
///
/// @param allocator Reference to the allocator that allocated the memory
/// pointed to by dest.
/// @param first Iterator to the first element in the source range.
/// @param last Sentinel marking the end of the source range.
/// @param dest Iterator to the start of the uninitialized destination range.
///
/// @return An iterator pointing one past the last element constructed in
/// `dest`.
///
/// @pre `[first, last)` must denote a valid range.
/// @pre The destination range must contain sufficient uninitialized storage.
/// @pre The source and destination ranges must not overlap.
///
/// @note Exception Safety: Strong guarantee.  If an exception is thrown during
/// construction, any elements already constructed in the destination range are
/// destroyed.  The original elements remain untouched.
///
export template <
    AllocatorType Allocator,
    std::input_iterator SourceIterator,
    std::sentinel_for<SourceIterator> Sentinel,
    std::input_or_output_iterator DestinationIterator
>
constexpr DestinationIterator
uninitialized_copy(
    Allocator &allocator,
    SourceIterator first,
    Sentinel last,
    DestinationIterator dest)
{
   using a_traits = std::allocator_traits<Allocator>;
   using value_type = a_traits::value_type;

   if constexpr (std::is_nothrow_copy_constructible_v<value_type>)
   {
      for (; first != last; ++first, ++dest)
      {
         a_traits::construct(allocator, std::to_address(dest), *first);
      }
      return DestinationIterator{ dest };
   }
   else
   {
      detail::ElementGuard elem_guard{ allocator,
                                       std::to_address(dest),
                                       std::to_address(dest) };

      for (; first != last; ++first, ++elem_guard.finish)
      {
         a_traits::construct(allocator, elem_guard.finish, *first);
      }

      elem_guard.dismiss();
      return DestinationIterator{ elem_guard.finish };
   }
}

/// @brief Moves elements in the range `[first, last)` to uninitialized memory
/// via the allocator.
///
/// Move-constructs elements from the source range into uninitialized storage
/// starting at `dest` using `std::allocator_traits<Allocator>::construct`.
///
/// @note This function initializes object lifetimes but does not allocate
/// storage.
///
/// @param allocator Reference to the allocator that allocated the memory
/// pointed to by dest.
/// @param first Iterator to the first element in the source range.
/// @param last Sentinel marking the end of the source range.
/// @param dest Iterator to the start of the uninitialized destination range.
///
/// @return An iterator pointing one past the last element constructed in
/// `dest`.
///
/// @pre `[first, last)` must denote a valid range.
/// @pre The destination range must contain sufficient uninitialized storage.
/// @pre The source and destination ranges must not overlap.
///
/// @note Exception Safety: Basic guarantee.  If an exception is thrown during
/// construction, any elements that were successfully constructed in the
/// destination range are destroyed.  Source elements that were already moved
/// from remain in a valid but unspecified state.
///
export template <
    AllocatorType Allocator,
    std::input_iterator SourceIterator,
    std::sentinel_for<SourceIterator> Sentinel,
    std::input_or_output_iterator DestinationIterator
>
constexpr DestinationIterator
uninitialized_move(
    Allocator &allocator,
    SourceIterator first,
    Sentinel last,
    DestinationIterator dest)
{
   using a_traits = std::allocator_traits<Allocator>;
   using value_type = a_traits::value_type;

   if constexpr (std::is_nothrow_move_constructible_v<value_type>)
   {
      for (; first != last; ++first, ++dest)
      {
         a_traits::construct(
             allocator, std::to_address(dest), std::move(*first));
      }

      return DestinationIterator{ dest };
   }
   else
   {
      detail::ElementGuard elem_guard{ allocator,
                                       std::to_address(dest),
                                       std::to_address(dest) };

      for (; first != last; ++first, ++elem_guard.finish)
      {
         a_traits::construct(allocator, elem_guard.finish, std::move(*first));
      }

      elem_guard.dismiss();
      return DestinationIterator{ elem_guard.finish };
   }
}

/// @brief Moves elements to uninitialized memory if moving is `noexcept`;
/// otherwise copies.
///
/// Dispatches to `uninitialized_move` if the element type is
/// nothrow_move_constructible or move-only.  Otherwise, falls back to
/// `uninitialized_copy` to protect source elements from modification if
/// construction throws.
///
/// @param allocator Reference to the allocator that allocated the memory
/// pointed to by dest.
/// @param first Iterator to the first element in the source range.
/// @param last Sentinel marking the end of the source range.
/// @param dest Iterator to the start of the uninitialized destination range.
///
/// @return An iterator pointing one past the last element constructed in
/// `dest`.
///
/// @pre `[first, last)` must denote a valid range.
/// @pre The destination range must contain sufficient uninitialized storage.
/// @pre The source and destination ranges must not overlap.
///
/// @note Exception Safety: Nothrow if object construction is noexcept; strong
/// guarantee for the source range when copying is used; otherwise basic
/// guarantee.  If an exception is thrown during construction, any elements that
/// were successfully constructed in the destination range are destroyed. Source
/// elements that were already moved from remain in a valid but unspecified
/// state.
///
export template <
    AllocatorType Allocator,
    std::input_iterator SourceIterator,
    std::sentinel_for<SourceIterator> Sentinel,
    std::input_or_output_iterator DestinationIterator
>
constexpr DestinationIterator
uninitialized_move_if_noexcept(
    Allocator &allocator,
    SourceIterator first,
    Sentinel last,
    DestinationIterator dest)
{
   using value_type = std::allocator_traits<Allocator>::value_type;

   if constexpr (
       std::is_nothrow_move_constructible_v<value_type>
       || !std::is_copy_constructible_v<value_type>)
   {
      return uninitialized_move(allocator, first, last, dest);
   }
   else
   {
      return uninitialized_copy(allocator, first, last, dest);
   }
}

}  // namespace swtl
