export module swtl_memory;

import std;

namespace swtl::allocator_aware {

template <typename Allocator>
concept AllocatorType = requires(
    Allocator allocator, std::allocator_traits<Allocator>::size_type n) {
  typename std::allocator_traits<Allocator>::value_type;

  {
    std::allocator_traits<Allocator>::allocate(allocator, n)
  } -> std::same_as<typename std::allocator_traits<Allocator>::pointer>;

  std::allocator_traits<Allocator>::deallocate(
      allocator,
      std::declval<typename std::allocator_traits<Allocator>::pointer>(), n);
};

export template <AllocatorType Allocator, typename SourceIterator,
                 typename DestinationIterator>
  requires std::input_iterator<SourceIterator> &&
           std::input_or_output_iterator<DestinationIterator>
constexpr auto
uninitialized_move_range(Allocator &allocator, SourceIterator src_begin,
                         SourceIterator src_end, DestinationIterator dest_begin)
    -> DestinationIterator {
  using value_type = std::iter_value_t<SourceIterator>;

  if constexpr (std::is_nothrow_move_constructible_v<value_type>) {
    for (; src_begin != src_end; ++src_begin, ++dest_begin) {
      std::allocator_traits<Allocator>::construct(
          allocator, std::to_address(dest_begin), std::move(*src_begin));
    }
    return dest_begin;
  } else {
    auto position{dest_begin};
    try {
      for (; src_begin != src_end; ++src_begin, ++position) {
        std::allocator_traits<Allocator>::construct(
            allocator, std::to_address(position), std::move(*src_begin));
      }
      return position;
    } catch (...) {
      for (; dest_begin != position; ++dest_begin) {
        std::allocator_traits<Allocator>::destroy(allocator,
                                                  std::to_address(dest_begin));
      }
      throw;
    }
  }
}

export template <AllocatorType Allocator, typename SourceIterator,
                 typename DestinationIterator>
  requires std::input_iterator<SourceIterator> &&
           std::input_or_output_iterator<DestinationIterator>
constexpr auto
uninitialized_copy_range(Allocator &allocator, SourceIterator src_begin,
                         SourceIterator src_end, DestinationIterator dest_begin)
    -> DestinationIterator {
  using value_type = std::iter_value_t<SourceIterator>;

  if constexpr (std::is_nothrow_copy_constructible_v<value_type>) {
    for (; src_begin != src_end; ++src_begin, ++dest_begin) {
      std::allocator_traits<Allocator>::construct(
          allocator, std::to_address(dest_begin), *src_begin);
    }
    return dest_begin;
  } else {
    auto position{dest_begin};
    try {
      for (; src_begin != src_end; ++src_begin, ++position) {
        std::allocator_traits<Allocator>::construct(
            allocator, std::to_address(position), *src_begin);
      }
      return position;
    } catch (...) {
      for (; dest_begin != position; ++dest_begin) {
        std::allocator_traits<Allocator>::destroy(allocator,
                                                  std::to_address(dest_begin));
      }
      throw;
    }
  }
}

export template <AllocatorType Allocator, typename Iterator>
  requires std::input_or_output_iterator<Iterator>
constexpr auto destroy_range(Allocator allocator, Iterator begin,
                             Iterator end) noexcept -> void {
  for (; begin != end; ++begin) {
    std::allocator_traits<Allocator>::destroy(allocator,
                                              std::to_address(begin));
  }
}

} // namespace swtl::allocator_aware
