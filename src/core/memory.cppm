export module swtl_memory;

import std;

namespace swtl::memory {

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

export template <AllocatorType Allocator>
constexpr auto
destroy_range(Allocator &allocator,
              typename std::allocator_traits<Allocator>::pointer begin,
              typename std::allocator_traits<Allocator>::pointer end) noexcept
    -> void;

export template <AllocatorType Allocator> struct AllocationGuard {
  using pointer = std::allocator_traits<Allocator>::pointer;
  using size_type = std::allocator_traits<Allocator>::size_type;

  constexpr AllocationGuard(Allocator &allocator, pointer ptr_to_guard,
                            size_type element_count)
      : alloc{allocator}, ptr{ptr_to_guard}, count{element_count} {}

  constexpr ~AllocationGuard() {
    if (ptr != nullptr) {
      std::allocator_traits<Allocator>::deallocate(alloc, ptr, count);
    }
  }

  AllocationGuard() = delete ("Must provide a reference to an allocator.");
  AllocationGuard(AllocationGuard const &other) = delete;
  AllocationGuard(AllocationGuard &&other) = delete;
  auto operator=(AllocationGuard const &other) = delete;
  auto operator=(AllocationGuard &&other) = delete;

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

  constexpr ElementGuard(Allocator &allocator, pointer begin_ptr,
                         pointer end_ptr)
      : alloc{allocator}, begin{begin_ptr}, end{end_ptr} {}

  constexpr ~ElementGuard() { destroy_range(alloc, begin, end); }

  ElementGuard() = delete ("Must provide a reference to an allocator.");
  ElementGuard(ElementGuard const &other) = delete;
  ElementGuard(ElementGuard &&other) = delete;
  auto operator=(ElementGuard const &other) = delete;
  auto operator=(ElementGuard &&other) = delete;

  constexpr auto dismiss() -> void { begin = end; };

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

export template <AllocatorType Allocator>
constexpr auto uninitialized_copy(
    Allocator &allocator,
    typename std::allocator_traits<Allocator>::pointer src_begin,
    typename std::allocator_traits<Allocator>::pointer src_end,
    typename std::allocator_traits<Allocator>::pointer dest_begin)
    -> std::allocator_traits<Allocator>::pointer {
  ElementGuard elem_guard{allocator, std::to_address(dest_begin),
                          std::to_address(dest_begin)};

  // By using the element guard's end member as the insertion point, we get
  // cleanup tracking for free.
  for (; src_begin != src_end; ++src_begin, ++elem_guard.end) {
    std::allocator_traits<Allocator>::construct(allocator, elem_guard.end,
                                                *src_begin);
  }

  elem_guard.dismiss();
  return elem_guard.end;
}

// TODO: Working here, ^v These two calls are ambiguous.

export template <AllocatorType Allocator, std::input_iterator SourceIterator,
                 std::sentinel_for<SourceIterator> Sentinel,
                 std::input_or_output_iterator DestinationIterator>
constexpr auto uninitialized_copy(Allocator &allocator,
                                  SourceIterator src_begin, Sentinel src_end,
                                  DestinationIterator dest_begin)
    -> DestinationIterator {
  ElementGuard elem_guard{allocator, std::to_address(dest_begin),
                          std::to_address(dest_begin)};

  // By using the element guard's end member as the insertion point, we get
  // cleanup tracking for free.
  for (; src_begin != src_end; ++src_begin, ++elem_guard.end) {
    std::allocator_traits<Allocator>::construct(allocator, elem_guard.end,
                                                *src_begin);
  }

  elem_guard.dismiss();
  return DestinationIterator{elem_guard.end};
}

export template <AllocatorType Allocator>
constexpr auto uninitialized_move(
    Allocator &allocator,
    typename std::allocator_traits<Allocator>::pointer src_begin,
    typename std::allocator_traits<Allocator>::pointer src_end,
    typename std::allocator_traits<Allocator>::pointer dest_begin)
    -> std::allocator_traits<Allocator>::pointer {
  ElementGuard elem_guard{allocator, std::to_address(dest_begin),
                          std::to_address(dest_begin)};

  // By using the element guard's end member as the insertion point, we get
  // cleanup tracking for free.
  for (; src_begin != src_end; ++src_begin, ++elem_guard.end) {
    std::allocator_traits<Allocator>::construct(allocator, elem_guard.end,
                                                std::move(*src_begin));
  }

  elem_guard.dismiss();
  return elem_guard.end;
}

export template <AllocatorType Allocator>
constexpr auto uninitialized_move_if_noexcept(
    Allocator &allocator,
    typename std::allocator_traits<Allocator>::pointer src_begin,
    typename std::allocator_traits<Allocator>::pointer src_end,
    typename std::allocator_traits<Allocator>::pointer dest_begin)
    -> std::allocator_traits<Allocator>::pointer {
  using value_type = std::allocator_traits<Allocator>::value_type;

  if constexpr (std::is_nothrow_move_constructible_v<value_type> ||
                !std::is_copy_constructible_v<value_type>) {
    return uninitialized_move(allocator, src_begin, src_end, dest_begin);
  } else {
    return uninitialized_copy(allocator, src_begin, src_end, dest_begin);
  }
}

} // namespace swtl::memory
