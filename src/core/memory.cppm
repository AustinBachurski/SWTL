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

export template <AllocatorType Allocator> struct AllocationGuard {
  using pointer = std::allocator_traits<Allocator>::pointer;
  using size_type = std::allocator_traits<Allocator>::size_type;

  AllocationGuard() = delete ("Must provide a reference to an allocator.");
  constexpr AllocationGuard(Allocator const &allocator, pointer ptr_to_guard,
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
  constexpr ElementGuard(Allocator const &allocator, pointer begin_ptr,
                         pointer end_ptr)
      : alloc{allocator}, begin{begin_ptr}, end{end_ptr} {}
  constexpr ~ElementGuard() { destroy_range(alloc, begin, end); }

  constexpr auto dismiss() -> void { begin, end = nullptr };

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
              std::allocator_traits<Allocator>::pointer begin,
              std::allocator_traits<Allocator>::pointer end) noexcept -> void {
  for (; begin != end; ++begin) {
    std::allocator_traits<Allocator>::destroy(allocator,
                                              std::to_address(begin));
  }
}

} // namespace swtl::memory
