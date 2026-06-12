export module swtl_memory;

import std;

namespace swtl::allocator_aware {

template <typename Allocator, typename T>
concept AllocatorType = requires(
    Allocator allocator, T t, std::allocator_traits<Allocator>::pointer ptr) {
  {
    std::allocator_traits<Allocator>::allocate(allocator, 1)
  } -> std::same_as<typename std::allocator_traits<Allocator>::pointer>;

  std::allocator_traits<Allocator>::construct(allocator, ptr, t);
  std::allocator_traits<Allocator>::destroy(allocator, ptr);
  std::allocator_traits<Allocator>::deallocate(allocator, ptr, 1);
};

export template <typename Allocator, typename SourceIterator,
                 typename DestinationIterator>
  requires std::input_iterator<SourceIterator> &&
           std::input_or_output_iterator<DestinationIterator> &&
           AllocatorType<Allocator, std::iter_value_t<SourceIterator>>
constexpr auto uninitialized_copy(Allocator allocator, SourceIterator src_begin,
                                  SourceIterator src_end,
                                  DestinationIterator dest_begin) -> void {}

export template <typename Allocator, typename Iterator>
  requires std::input_or_output_iterator<Iterator> &&
           AllocatorType<Allocator, std::iter_value_t<Iterator>>
constexpr auto destroy_range(Allocator allocator, Iterator begin,
                             Iterator end) noexcept -> void {
  for (; begin != end; ++begin) {
    std::allocator_traits<Allocator>::destroy(allocator,
                                              std::to_address(begin));
  }
}

} // namespace swtl::allocator_aware
