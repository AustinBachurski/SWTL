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

export template <AllocatorType Allocator,
                 std::input_or_output_iterator SourceIterator,
                 std::sentinel_for<SourceIterator> Sentinel>
constexpr auto destroy(Allocator &allocator, SourceIterator begin,
                       Sentinel end) noexcept -> void;

export template <AllocatorType Allocator> struct AllocationGuard {
  using pointer = std::allocator_traits<Allocator>::pointer;
  using size_type = std::allocator_traits<Allocator>::size_type;

  constexpr AllocationGuard(Allocator &allocator, pointer ptr_to_guard,
                            size_type element_count) noexcept
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

  constexpr auto dismiss() noexcept -> void { ptr = nullptr; }

  constexpr auto switch_allocator(Allocator &new_allocator) noexcept -> void {
    alloc = new_allocator;
  }

  constexpr auto reassign(pointer ptr_to_guard,
                          size_type element_count) noexcept -> void {
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
                         pointer end_ptr) noexcept
      : alloc{allocator}, begin{begin_ptr}, end{end_ptr} {}

  constexpr ~ElementGuard() { destroy(alloc, begin, end); }

  ElementGuard() = delete ("Must provide a reference to an allocator.");
  ElementGuard(ElementGuard const &other) = delete;
  ElementGuard(ElementGuard &&other) = delete;
  auto operator=(ElementGuard const &other) = delete;
  auto operator=(ElementGuard &&other) = delete;

  constexpr auto dismiss() noexcept -> void { begin = end; };

  constexpr auto switch_allocator(Allocator &new_allocator) noexcept -> void {
    alloc = new_allocator;
  }

  constexpr auto reassign(pointer begin_ptr, pointer end_ptr) noexcept -> void {
    begin = begin_ptr;
    end = end_ptr;
  }

  Allocator &alloc;
  pointer begin;
  pointer end;
};

export template <AllocatorType Allocator,
                 std::input_or_output_iterator SourceIterator,
                 std::sentinel_for<SourceIterator> Sentinel>
constexpr auto destroy(Allocator &allocator, SourceIterator begin,
                       Sentinel end) noexcept -> void {
  for (; begin != end; ++begin) {
    std::allocator_traits<Allocator>::destroy(allocator,
                                              std::to_address(begin));
  }
}

export template <AllocatorType Allocator, std::input_iterator SourceIterator,
                 std::sentinel_for<SourceIterator> Sentinel,
                 std::input_or_output_iterator DestinationIterator>
constexpr auto uninitialized_copy(Allocator &allocator,
                                  SourceIterator src_begin, Sentinel src_end,
                                  DestinationIterator destination)
    -> DestinationIterator {
  using value_type = std::allocator_traits<Allocator>::value_type;

  if constexpr (std::is_nothrow_copy_constructible_v<value_type>) {
    for (; src_begin != src_end; ++src_begin, ++destination) {
      std::allocator_traits<Allocator>::construct(
          allocator, std::to_address(destination), *src_begin);
    }
    return DestinationIterator{destination};
  } else {
    ElementGuard elem_guard{allocator, std::to_address(destination),
                            std::to_address(destination)};

    for (; src_begin != src_end; ++src_begin, ++elem_guard.end) {
      std::allocator_traits<Allocator>::construct(allocator, elem_guard.end,
                                                  *src_begin);
    }

    elem_guard.dismiss();
    return DestinationIterator{elem_guard.end};
  }
}

export template <AllocatorType Allocator, std::input_iterator SourceIterator,
                 std::sentinel_for<SourceIterator> Sentinel,
                 std::input_or_output_iterator DestinationIterator>
constexpr auto uninitialized_move(Allocator &allocator,
                                  SourceIterator src_begin, Sentinel src_end,
                                  DestinationIterator destination)
    -> DestinationIterator {
  using value_type = std::allocator_traits<Allocator>::value_type;

  if constexpr (std::is_nothrow_move_constructible_v<value_type>) {
    for (; src_begin != src_end; ++src_begin, ++destination) {
      std::allocator_traits<Allocator>::construct(
          allocator, std::to_address(destination), std::move(*src_begin));
    }

    return DestinationIterator{destination};
  } else {
    ElementGuard elem_guard{allocator, std::to_address(destination),
                            std::to_address(destination)};

    for (; src_begin != src_end; ++src_begin, ++elem_guard.end) {
      std::allocator_traits<Allocator>::construct(allocator, elem_guard.end,
                                                  std::move(*src_begin));
    }

    elem_guard.dismiss();
    return DestinationIterator{elem_guard.end};
  }
}

export template <AllocatorType Allocator, std::input_iterator SourceIterator,
                 std::sentinel_for<SourceIterator> Sentinel,
                 std::input_or_output_iterator DestinationIterator>
constexpr auto uninitialized_move_if_noexcept(Allocator &allocator,
                                              SourceIterator src_begin,
                                              Sentinel src_end,
                                              DestinationIterator destination)
    -> DestinationIterator {
  using value_type = std::allocator_traits<Allocator>::value_type;

  if constexpr (std::is_nothrow_move_constructible_v<value_type> ||
                !std::is_copy_constructible_v<value_type>) {
    return uninitialized_move(allocator, src_begin, src_end, destination);
  } else {
    return uninitialized_copy(allocator, src_begin, src_end, destination);
  }
}

} // namespace swtl::memory
