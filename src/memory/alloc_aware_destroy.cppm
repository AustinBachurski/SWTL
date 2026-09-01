module;

#include <cassert>

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
/// @return An unmodified copy of `first`.
///
/// @pre `[first, last)` must denote a valid range.
///
export template <
    AllocatorType Allocator,
    std::input_or_output_iterator Iterator,
    std::sentinel_for<Iterator> Sentinel
>
constexpr Iterator
destroy(Allocator &allocator, Iterator first, Sentinel last) noexcept
{
   if constexpr (std::sized_sentinel_for<Sentinel, Iterator>)
   {
      assert(last - first >= 0 && "`last` must be reachable from `first`");
   }

   auto const destroy_start{ first };

   while (first != last)
   {
      std::allocator_traits<Allocator>::destroy(
          allocator, std::to_address(first++));
   }

   return destroy_start;
}

}  // namespace swtl
