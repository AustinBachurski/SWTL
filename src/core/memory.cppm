export module swtl_memory;

import std;

namespace swtl::memory
{

template <typename Allocator>
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

export template <
    AllocatorType Allocator,
    std::input_or_output_iterator SourceIterator,
    std::sentinel_for<SourceIterator> Sentinel
>
constexpr void
destroy(Allocator &allocator, SourceIterator begin, Sentinel end) noexcept;

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
       Allocator &allocator, pointer begin_ptr, pointer end_ptr) noexcept
       : alloc{ allocator }
       , begin{ begin_ptr }
       , end{ end_ptr }
   {}

   constexpr ~ElementGuard()
   {
      destroy(alloc, begin, end);
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
      begin = end;
   };

   constexpr void
   switch_allocator(Allocator &new_allocator) noexcept
   {
      alloc = new_allocator;
   }

   constexpr void
   reassign(pointer begin_ptr, pointer end_ptr) noexcept
   {
      begin = begin_ptr;
      end = end_ptr;
   }

   Allocator &alloc;
   pointer begin;
   pointer end;
};

export template <
    AllocatorType Allocator,
    std::input_or_output_iterator SourceIterator,
    std::sentinel_for<SourceIterator> Sentinel
>
constexpr void
destroy(Allocator &allocator, SourceIterator begin, Sentinel end) noexcept
{
   for (; begin != end; ++begin)
   {
      std::allocator_traits<Allocator>::destroy(
          allocator, std::to_address(begin));
   }
}

export template <
    AllocatorType Allocator,
    std::input_iterator SourceIterator,
    std::sentinel_for<SourceIterator> Sentinel,
    std::input_or_output_iterator DestinationIterator
>
constexpr DestinationIterator
uninitialized_copy(
    Allocator &allocator,
    SourceIterator src_begin,
    Sentinel src_end,
    DestinationIterator destination)
{
   using a_traits = std::allocator_traits<Allocator>;
   using value_type = a_traits::value_type;

   if constexpr (std::is_nothrow_copy_constructible_v<value_type>)
   {
      for (; src_begin != src_end; ++src_begin, ++destination)
      {
         a_traits::construct(
             allocator, std::to_address(destination), *src_begin);
      }
      return DestinationIterator{ destination };
   }
   else
   {
      ElementGuard elem_guard{ allocator,
                               std::to_address(destination),
                               std::to_address(destination) };

      for (; src_begin != src_end; ++src_begin, ++elem_guard.end)
      {
         a_traits::construct(allocator, elem_guard.end, *src_begin);
      }

      elem_guard.dismiss();
      return DestinationIterator{ elem_guard.end };
   }
}

export template <
    AllocatorType Allocator,
    std::input_iterator SourceIterator,
    std::sentinel_for<SourceIterator> Sentinel,
    std::input_or_output_iterator DestinationIterator
>
constexpr DestinationIterator
uninitialized_move(
    Allocator &allocator,
    SourceIterator src_begin,
    Sentinel src_end,
    DestinationIterator destination)
{
   using a_traits = std::allocator_traits<Allocator>;
   using value_type = a_traits::value_type;

   if constexpr (std::is_nothrow_move_constructible_v<value_type>)
   {
      for (; src_begin != src_end; ++src_begin, ++destination)
      {
         a_traits::construct(
             allocator, std::to_address(destination), std::move(*src_begin));
      }

      return DestinationIterator{ destination };
   }
   else
   {
      ElementGuard elem_guard{ allocator,
                               std::to_address(destination),
                               std::to_address(destination) };

      for (; src_begin != src_end; ++src_begin, ++elem_guard.end)
      {
         a_traits::construct(allocator, elem_guard.end, std::move(*src_begin));
      }

      elem_guard.dismiss();
      return DestinationIterator{ elem_guard.end };
   }
}

export template <
    AllocatorType Allocator,
    std::input_iterator SourceIterator,
    std::sentinel_for<SourceIterator> Sentinel,
    std::input_or_output_iterator DestinationIterator
>
constexpr DestinationIterator
uninitialized_move_if_noexcept(
    Allocator &allocator,
    SourceIterator src_begin,
    Sentinel src_end,
    DestinationIterator destination)
{
   using value_type = std::allocator_traits<Allocator>::value_type;

   if constexpr (
       std::is_nothrow_move_constructible_v<value_type>
       || !std::is_copy_constructible_v<value_type>)
   {
      return uninitialized_move(allocator, src_begin, src_end, destination);
   }
   else
   {
      return uninitialized_copy(allocator, src_begin, src_end, destination);
   }
}

}  // namespace swtl::memory
