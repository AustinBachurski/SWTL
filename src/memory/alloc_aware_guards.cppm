export module swtl.memory:alloc_aware_guards;

import std;

import :alloc_aware_destroy;

namespace swtl::detail
{

export template <AllocatorType Allocator>
struct AllocationGuard
{
   using pointer = std::allocator_traits<Allocator>::pointer;
   using size_type = std::allocator_traits<Allocator>::size_type;

   constexpr AllocationGuard(
       Allocator &allocator,
       pointer ptr_to_guard,
       size_type element_count) noexcept
       : alloc{ allocator }
       , ptr{ ptr_to_guard }
       , count{ element_count }
   {}

   constexpr ~AllocationGuard()
   {
      if (ptr != nullptr)
      {
         std::allocator_traits<Allocator>::deallocate(alloc, ptr, count);
      }
   }

   AllocationGuard() = delete ("Must provide a reference to an allocator.");
   AllocationGuard(AllocationGuard const &other) = delete;
   AllocationGuard(AllocationGuard &&other) = delete;
   AllocationGuard
   operator=(AllocationGuard const &other) = delete;
   AllocationGuard
   operator=(AllocationGuard &&other) = delete;

   constexpr void
   dismiss() noexcept
   {
      ptr = nullptr;
   }

   constexpr void
   switch_allocator(Allocator &new_allocator) noexcept
   {
      alloc = new_allocator;
   }

   constexpr void
   reassign(pointer ptr_to_guard, size_type element_count) noexcept
   {
      ptr = ptr_to_guard;
      count = element_count;
   }

   Allocator &alloc;
   pointer ptr;
   size_type count;
};

export template <AllocatorType Allocator>
struct ElementGuard
{
   using value_type = std::allocator_traits<Allocator>::value_type;
   using pointer = std::allocator_traits<Allocator>::pointer;

   constexpr ElementGuard(
       Allocator &allocator, pointer start_ptr, pointer finish_ptr) noexcept
       : alloc{ allocator }
       , start{ start_ptr }
       , finish{ finish_ptr }
   {}

   constexpr ~ElementGuard()
   {
      destroy(alloc, start, finish);
   }

   ElementGuard() = delete ("Must provide a reference to an allocator.");
   ElementGuard(ElementGuard const &other) = delete;
   ElementGuard(ElementGuard &&other) = delete;
   auto
   operator=(ElementGuard const &other) = delete;
   auto
   operator=(ElementGuard &&other) = delete;

   constexpr void
   dismiss() noexcept
   {
      start = finish;
   };

   constexpr void
   switch_allocator(Allocator &new_allocator) noexcept
   {
      alloc = new_allocator;
   }

   constexpr void
   reassign(pointer start_ptr, pointer finish_ptr) noexcept
   {
      start = start_ptr;
      finish = finish_ptr;
   }

   Allocator &alloc;
   pointer start;
   pointer finish;
};

}  // namespace swtl::detail
