export module swtl_memory;

import std;

namespace swtl::memory {

constexpr auto
destroy_range(Allocator &allocator,
              typename std::allocator_traits<Allocator>::pointer begin,
              typename std::allocator_traits<Allocator>::pointer end) noexcept
    -> void;

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

export template <AllocatorType Allocator> struct AllocationGuard {
  using pointer = std::allocator_traits<Allocator>::pointer;
  using size_type = std::allocator_traits<Allocator>::size_type;

  AllocationGuard() = delete ("Must provide a reference to an allocator.");
  constexpr AllocationGuard(Allocator &allocator, pointer ptr_to_guard,
                            size_type element_count)
      : alloc{allocator}, ptr{ptr_to_guard}, count{element_count} {}
  constexpr ~AllocationGuard() {
    if (ptr != nullptr) {
      std::allocator_traits<Allocator>::deallocate(alloc, ptr, count);
    }
  }

  constexpr auto dismiss() -> void { ptr = nullptr; }

  constexpr auto reassign(pointer ptr_to_guard, size_type element_count)
      -> void {
    ptr = ptr_to_guard;
    count = element_count;
  }

  Allocator &alloc;
  pointer ptr;
  size_type count;
};

export template <AllocatorType Allocator> struct ElementGuard {
  using value_type = std::allocator_traits<Allocator>::value_type;
  using pointer = std::allocator_traits<Allocator>::pointer;

  ElementGuard() = delete ("Must provide a reference to an allocator.");
  constexpr ElementGuard(Allocator &allocator, pointer begin_ptr,
                         pointer end_ptr)
      : alloc{allocator}, begin{begin_ptr}, end{end_ptr} {}
  constexpr ~ElementGuard() { destroy_range(alloc, begin, end); }

  constexpr auto dismiss() -> void { begin = end = nullptr; };

  constexpr auto reassign(pointer begin_ptr, pointer end_ptr) -> void {
    begin = begin_ptr;
    end = end_ptr;
  }

  Allocator &alloc;
  pointer begin;
  pointer end;
};

export template <AllocatorType Allocator>
constexpr auto
destroy_range(Allocator &allocator,
              typename std::allocator_traits<Allocator>::pointer begin,
              typename std::allocator_traits<Allocator>::pointer end) noexcept
    -> void {
  for (; begin != end; ++begin) {
    std::allocator_traits<Allocator>::destroy(allocator,
                                              std::to_address(begin));
  }
}

export template <AllocatorType Allocator, std::input_iterator SourceIterator,
                 std::sentinel_for<SourceIterator> Sentinel,
                 std::input_or_output_iterator DestinationIterator>
constexpr auto
uninitialized_copy_range(Allocator &allocator, SourceIterator src_begin,
                         Sentinel src_end, DestinationIterator dest_begin)
    -> DestinationIterator {
  ElementGuard elem_guard{std::to_address(dest_begin),
                          std::to_address(dest_begin)};

  // By using the element guard's end member as the insertion point, we get
  // cleanup tracking for free.
  for (; src_begin != src_end; ++src_begin, ++elem_guard.end) {
    std::allocator_traits<Allocator>::construct(allocator, elem_guard.end,
                                                *src_begin);
  }

  auto last{elem_guard.end};
  elem_guard.dismiss();
  return last;
}

export template <AllocatorType Allocator, std::input_iterator SourceIterator,
                 std::sentinel_for<SourceIterator> Sentinel,
                 std::input_or_output_iterator DestinationIterator>
constexpr auto
uninitialized_move_range(Allocator &allocator, SourceIterator src_begin,
                         Sentinel src_end, DestinationIterator dest_begin)
    -> DestinationIterator {
  ElementGuard elem_guard{std::to_address(dest_begin),
                          std::to_address(dest_begin)};

  // By using the element guard's end member as the insertion point, we get
  // cleanup tracking for free.
  for (; src_begin != src_end; ++src_begin, ++elem_guard.end) {
    std::allocator_traits<Allocator>::construct(allocator, elem_guard.end,
                                                std::move(*src_begin));
  }

  auto last{elem_guard.end};
  elem_guard.dismiss();
  return last;
}

} // namespace swtl::memory
