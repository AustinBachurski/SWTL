export module swtl.memory:alloc_aware_destroy;

import std;

import :concepts;

namespace swtl
{

export template <
    AllocatorType Allocator,
    std::input_or_output_iterator SourceIterator,
    std::sentinel_for<SourceIterator> Sentinel
>
constexpr void
destroy(Allocator &allocator, SourceIterator begin, Sentinel end) noexcept
{
   for (; begin != end; ++begin)
   {
      std::allocator_traits<Allocator>::destroy(
          allocator, std::to_address(begin));
   }
}

}  // namespace swtl
