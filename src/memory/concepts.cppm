export module swtl.memory:concepts;

import std;

namespace swtl
{

export template <typename Allocator>
concept AllocatorType = requires(
    Allocator allocator, std::allocator_traits<Allocator>::size_type count) {
   typename std::allocator_traits<Allocator>::value_type;

   {
      std::allocator_traits<Allocator>::allocate(allocator, count)
   } -> std::same_as<typename std::allocator_traits<Allocator>::pointer>;

   std::allocator_traits<Allocator>::deallocate(
       allocator,
       std::declval<typename std::allocator_traits<Allocator>::pointer>(),
       count);
};

}  // namespace swtl
