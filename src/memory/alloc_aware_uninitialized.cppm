export module swtl.memory:alloc_aware_uninitialized;

import std;

import :concepts;
import :alloc_aware_destroy;
import :alloc_aware_guards;

namespace swtl
{

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
      detail::ElementGuard elem_guard{ allocator,
                                       std::to_address(destination),
                                       std::to_address(destination) };

      for (; src_begin != src_end; ++src_begin, ++elem_guard.finish)
      {
         a_traits::construct(allocator, elem_guard.finish, *src_begin);
      }

      elem_guard.dismiss();
      return DestinationIterator{ elem_guard.finish };
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
      detail::ElementGuard elem_guard{ allocator,
                                       std::to_address(destination),
                                       std::to_address(destination) };

      for (; src_begin != src_end; ++src_begin, ++elem_guard.finish)
      {
         a_traits::construct(
             allocator, elem_guard.finish, std::move(*src_begin));
      }

      elem_guard.dismiss();
      return DestinationIterator{ elem_guard.finish };
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

}  // namespace swtl
