module;
#include <memory>
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

template <typename Allocator, typename SourceIterator,
          typename DestinationIterator>
  requires std::input_iterator<SourceIterator> &&
           std::input_or_output_iterator<DestinationIterator> &&
           AllocatorType<Allocator, std::iter_value_t<SourceIterator>>
auto uninitialized_copy(Allocator allocator, SourceIterator src_begin,
                        SourceIterator src_end, DestinationIterator dest_begin)
    -> void {}

} // namespace swtl::allocator_aware
