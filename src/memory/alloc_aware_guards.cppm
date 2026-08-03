export module swtl.memory:alloc_aware_guards;

import std;

import :alloc_aware_destroy;

namespace swtl::detail
{

/// @internal
/// @brief RAII scope guard that deallocates uninitialized memory on destruction
/// unless dismissed.
///
/// If the lifetime of the guard ends without a call to `dismiss()`, the managed
/// memory block is returned to the allocator via
/// `std::allocator_traits<Allocator>::deallocate`.
///
/// @tparam Allocator The type of the allocator that was used to allocate the
/// memory to be managed.
///
/// @note AllocationGuard is strictly non-copyable and non-movable.
///
export template <AllocatorType Allocator>
struct AllocationGuard
{
   using pointer = std::allocator_traits<Allocator>::pointer;
   using size_type = std::allocator_traits<Allocator>::size_type;

   /// @brief Constructs a guard that manages the block of memory.
   ///
   /// @param allocator Reference to the allocator that allocated the memory.
   /// @param ptr_to_guard Pointer to the start of the memory block.
   /// @param element_count Capacity of the allocated block in number of
   /// elements.
   ///
   /// @warning The allocator reference passed to the constructor must outlive
   /// the guard.
   ///
   constexpr AllocationGuard(
       Allocator &allocator,
       pointer ptr_to_guard,
       size_type element_count) noexcept
       : alloc{ allocator }
       , ptr{ ptr_to_guard }
       , count{ element_count }
   {}

   /// @brief Deallocates the guarded memory block if `dismiss()` was not called
   /// prior to destruction.
   ///
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

   /// @brief Prevents the guard from deallocating memory when it is destroyed.
   ///
   /// @post `ptr == nullptr`
   ///
   constexpr void
   dismiss() noexcept
   {
      ptr = nullptr;
   }

   /// @brief Replaces the reference to the allocator used to deallocate memory.
   ///
   /// @param new_allocator Reference to the new allocator.
   ///
   /// @warning The allocator reference passed to this function must outlive the
   /// guard.
   ///
   constexpr void
   switch_allocator(Allocator &new_allocator) noexcept
   {
      alloc = new_allocator;
   }

   /// @brief Assigns the guard to a new memory block.
   ///
   /// @param ptr_to_guard Pointer to the start of the memory block.
   /// @param element_count Capacity of the allocated block in number of
   /// elements.
   ///
   /// @warning The allocator which allocated the memory must be the same as the
   /// reference stored in the guard.  If not, call `switch_allocator()`.
   ///
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

/// @internal
/// @brief RAII scope guard that destroys a range of objects on destruction
/// unless dismissed.
///
/// If the lifetime of the guard ends without a call to `dismiss()`, the managed
/// objects are destroyed via `std::allocator_traits<Allocator>::destroy`.
///
/// @tparam Allocator The type of the allocator that was used to construct the
/// managed objects.
///
/// @note ElementGuard is strictly non-copyable and non-movable.
///
export template <AllocatorType Allocator>
struct ElementGuard
{
   using value_type = std::allocator_traits<Allocator>::value_type;
   using pointer = std::allocator_traits<Allocator>::pointer;

   /// @brief Constructs a guard that manages the lifetime of a range of
   /// objects.
   ///
   /// @param allocator Reference to the allocator that constructed the
   /// managed objects.
   /// @param start Pointer to the start of the range of objects.
   /// @param finish Pointer to the end of the range of objects (one past the
   /// last object to be destroyed).
   ///
   /// @warning The allocator reference passed to the constructor must outlive
   /// the guard.
   ///
   constexpr ElementGuard(
       Allocator &allocator, pointer start_ptr, pointer finish_ptr) noexcept
       : alloc{ allocator }
       , start{ start_ptr }
       , finish{ finish_ptr }
   {}

   /// @brief Destroys the objects in the range `[start, finish)` if
   /// `dismiss()` was not called prior to destruction.
   ///
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

   /// @brief Prevents the guard from destroying any objects when it is
   /// destroyed.
   ///
   /// @post `start == finish`
   ///
   constexpr void
   dismiss() noexcept
   {
      start = finish;
   };

   /// @brief Replaces the reference to the allocator used to destroy objects.
   ///
   /// @param new_allocator Reference to the new allocator.
   ///
   /// @warning The allocator reference passed to this function must outlive the
   /// guard.
   ///
   constexpr void
   switch_allocator(Allocator &new_allocator) noexcept
   {
      alloc = new_allocator;
   }

   /// @brief Assigns the guard to a new range of objects.
   ///
   /// @param start Pointer to the start of the range of objects.
   /// @param finish Pointer to the end of the range of objects (one past the
   /// last object to be destroyed).
   ///
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
