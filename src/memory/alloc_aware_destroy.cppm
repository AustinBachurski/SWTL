export module swtl.memory:alloc_aware_destroy;

import std;

import :concepts;

namespace swtl
{

/// @brief Destroys the elements in the range `[first, last)` via the allocator.
///
/// @note Destroys the elements but does not deallocate their underlying
/// storage.
///
/// Destroys elements in the range `[first, last)` using
/// `std::allocator_traits<Alloctaor>::destroy`.
///
/// @param allocator Reference to the allocator that constructed the elements.
/// @param first Iterator to the first element in the range to be destroyed.
/// @param last Sentinel marking the end of the range.
///
/// @pre `[first, last)` must denote a valid range.
///
export template <
    AllocatorType Allocator,
    std::input_or_output_iterator Iterator,
    std::sentinel_for<Iterator> Sentinel
>
constexpr void
destroy(Allocator &allocator, Iterator first, Sentinel last) noexcept
{
   if constexpr (std::sized_sentinel_for<Sentinel, Iterator>)
   {
      contract_assert(
            last - first >= 0
            && "Contract violation: `last` is not reachable from `first`."
            "Are your iterator arguments reversed?");
   }

   while (first != last)
   {
      std::allocator_traits<Allocator>::destroy(
          allocator, std::to_address(first++));
   }
}

}  // namespace swtl
