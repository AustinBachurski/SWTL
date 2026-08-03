export module swtl.vector;

import std;

import swtl.container_concepts;
import swtl.contiguous_iterator;
import swtl.format;
import swtl.memory;

/// The Strictly Worse Template Library
namespace swtl
{

template <typename T, typename Allocator>
class VectorBase
{
public:
   using a_traits = std::allocator_traits<Allocator>;
   using allocator_type = Allocator;
   using value_type = a_traits::value_type;
   using size_type = a_traits::size_type;
   using difference_type = a_traits::difference_type;
   using pointer = a_traits::pointer;

   constexpr VectorBase() = default;

   constexpr VectorBase(Allocator const &allocator)
       : m_allocator{ allocator }
   {}

   constexpr VectorBase(VectorBase const &other) noexcept
       : m_allocator{ a_traits::select_on_container_copy_construction(
             other.m_allocator) }
   {}

   constexpr VectorBase(VectorBase &&other) noexcept
       : m_allocator{ std::move(other.m_allocator) }
       , m_start{ other.m_start }
       , m_finish{ other.m_finish }
       , m_end_of_storage{ other.m_end_of_storage }
   {
      other.m_start = other.m_finish = other.m_end_of_storage = nullptr;
   }

   constexpr VectorBase &
   operator=(VectorBase const &other)
       = delete ("Copy assignment must be handled by the Vector container.");

   constexpr VectorBase &
   operator=(VectorBase &&other)
       = delete ("Move assignment must be handled by the Vector container.");

   constexpr ~VectorBase()
   {
      if (m_start != nullptr)
      {
         a_traits::deallocate(
             m_allocator,
             m_start,
             static_cast<size_type>(m_end_of_storage - m_start));
      }
   }

protected:
   [[nodiscard]]
   constexpr std::allocation_result<pointer, size_type>
   allocate_memory_for_at_least(size_type num_elements)
   {
      if (num_elements == 0)
      {
         return { .ptr = nullptr, .count = 0UZ };
      }

      auto [ptr, count]{ a_traits::allocate_at_least(
          m_allocator, num_elements) };
      auto max{ max_allocatable_elements() };

      if (count > max)
      {
         count = max;
      }

      return { .ptr = ptr, .count = count };
   }

   [[nodiscard]]
   constexpr size_type
   allocated_capacity() const noexcept
   {
      return static_cast<size_type>(m_end_of_storage - m_start);
   }

   constexpr void
   create_storage(size_type num_elements)
   {
      auto [ptr, count]{ allocate_memory_for_at_least(num_elements) };

      m_start = m_finish = ptr;
      m_end_of_storage = m_start + count;
   }

   constexpr void
   deallocate_memory() noexcept
   {
      a_traits::deallocate(m_allocator, m_start, allocated_capacity());
      m_start = m_finish = m_end_of_storage = nullptr;
   }

   [[nodiscard]]
   constexpr size_type
   max_allocatable_elements() const noexcept
   {
      return std::min<size_type>(
          a_traits::max_size(m_allocator),
          std::numeric_limits<difference_type>::max() / sizeof(T));
   }

   [[no_unique_address]]
   Allocator m_allocator;
   pointer m_start{};
   pointer m_finish{};
   pointer m_end_of_storage{};
};

export template <typename T, typename Allocator = std::allocator<T>>
class Vector : protected VectorBase<T, Allocator>
{
private:
   using Base = VectorBase<T, Allocator>;
   using a_traits = std::allocator_traits<Allocator>;

public:
   // ** MEMBER TYPES **
   using value_type = std::remove_cv_t<T>;
   using allocator_type = Base::allocator_type;
   using size_type = std::size_t;
   using difference_type = std::ptrdiff_t;
   using reference = value_type &;
   using const_reference = value_type const &;
   using pointer = Base::pointer;
   using const_pointer = a_traits::const_pointer;
   using iterator = ContiguousIterator<T>;
   using const_iterator = ContiguousIterator<T const>;
   using reverse_iterator = std::reverse_iterator<iterator>;
   using const_reverse_iterator = std::reverse_iterator<const_iterator>;

public:
   // ** CONSTRUCTORS **
   constexpr Vector(Allocator const &allocator = Allocator())
       : Base{ allocator }
   {}

   // Should not be marked as explicit to allow for conversion from braced init
   // list.
   constexpr Vector(
       std::initializer_list<T> const &init_list,
       Allocator const &allocator = Allocator())
       : Base{ allocator }
   {
      this->create_storage(init_list.size());
      this->m_finish = uninitialized_copy(
          this->m_allocator, init_list.begin(), init_list.end(), this->m_start);
   }

   constexpr explicit Vector(
       size_type count, Allocator const &allocator = Allocator())
       : Base{ allocator }
   {
      this->create_storage(count);

      detail::ElementGuard elem_guard(
          this->m_allocator, this->m_start, this->m_start);

      for (; count != 0UZ; --count)
      {
         a_traits::construct(this->m_allocator, elem_guard.finish);
         ++elem_guard.finish;
      }

      this->m_finish = elem_guard.finish;
      elem_guard.dismiss();
   }

   constexpr Vector(size_type count, T const &value)
   {
      this->create_storage(count);

      detail::ElementGuard elem_guard(
          this->m_allocator, this->m_start, this->m_start);

      for (; count != 0UZ; --count)
      {
         a_traits::construct(this->m_allocator, elem_guard.finish, value);
         ++elem_guard.finish;
      }

      this->m_finish = elem_guard.finish;
      elem_guard.dismiss();
   }

   template <
       std::input_iterator InputIterator,
       std::sentinel_for<InputIterator> Sentinel
   >
   constexpr Vector(
       InputIterator first,
       Sentinel last,
       Allocator const &allocator = Allocator())
       : Base{ allocator }
   {
      this->create_storage(static_cast<size_type>(std::distance(first, last)));
      this->m_finish
          = uninitialized_copy(this->m_allocator, first, last, this->m_start);
   }

   template <container_compatible_range<T> Range>
   constexpr Vector(std::from_range_t, Range &&range)
   {
      if constexpr (std::ranges::sized_range<Range>)
      {
         auto const count{ static_cast<size_type>(std::ranges::size(range)) };
         this->create_storage(count);

         this->m_finish = uninitialized_copy(
             this->m_allocator, range.begin(), range.end(), this->m_start);
      }
      else
      {
         detail::ElementGuard elem_guard(
             this->m_allocator, this->m_start, this->m_start);

         for (auto &&element : range)
         {
            a_traits::construct(this->m_allocator, elem_guard.finish, element);
            ++elem_guard.finish;
         }

         this->m_finish = elem_guard.finish;
         elem_guard.dismiss();
      }
   }

   // ** ASSIGNMENT **
   constexpr void
   assign(size_type count, T const &value)
   {
      if (capacity() < count)
      {
         auto [ptr, size]{ this->allocate_memory_for_at_least(count) };

         if constexpr (std::is_nothrow_copy_constructible_v<T>)
         {
            clear();
            this->deallocate_memory();
            this->m_start = this->m_finish = ptr;
            this->m_end_of_storage = ptr + size;

            for (; count != 0UZ; --count)
            {
               a_traits::construct(this->m_allocator, this->m_finish++, value);
            }
         }
         else
         {
            detail::AllocationGuard mem_guard(this->m_allocator, ptr, size);
            {
               detail::ElementGuard elem_guard(this->m_allocator, ptr, ptr);

               for (; count != 0UZ; --count)
               {
                  a_traits::construct(
                      this->m_allocator, elem_guard.finish, value);
                  ++elem_guard.finish;
               }

               mem_guard.reassign(this->m_start, capacity());
               elem_guard.reassign(
                   this->m_start,
                   std::exchange(this->m_finish, elem_guard.finish));

               this->m_start = ptr;
               this->m_end_of_storage = ptr + size;
            }
         }
      }
      else
      {
         auto current = this->m_start;

         for (; current != this->m_finish && count != 0UZ; --count)
         {
            *current++ = value;
         }

         if (current < this->m_finish)
         {
            destroy(this->m_allocator, current, this->m_finish);
            this->m_finish = current;
         }

         for (; count != 0UZ; --count)
         {
            a_traits::construct(this->m_allocator, this->m_finish, value);
            ++this->m_finish;  // Only increment after successful construction.
         }
      }
   }

   template <
       std::input_iterator InputIterator,
       std::sentinel_for<InputIterator> Sentinel
   >
   constexpr void
   assign(InputIterator first, Sentinel last)
   // Preconditions seem to be broken entirely, even `pre(false)` doesn't
   // trigger.  The code won't compile in a contract_assert, so I'm not sure if
   // it'll even work in a precondition - but we should try when preconditions
   // get fixed.
   /*
       pre(!std::sized_sentinel_for<Sentinel, InputIterator>
           || (last - first >= 0
               && "Are your iterator arguments backwards?"))
   */
   {
      if constexpr (std::sized_sentinel_for<Sentinel, InputIterator>)
      {
         contract_assert(
            last - first >= 0
            && "Contract violation: last is not reachable from first."
            "Are your iterator arguments reversed?");
      }

      assign_from_range(first, last);
   }

   constexpr void
   assign(std::initializer_list<T> init_list)
   {
      assign_from_range(init_list.begin(), init_list.end());
   }

   template <container_compatible_range<T> Range>
   constexpr void
   assign_range(Range &&range)
   {
      assign_from_range(std::ranges::begin(range), std::ranges::end(range));
   }

   constexpr allocator_type
   get_allocator() const noexcept
   {
      return this->m_allocator;
   }

   // ** SPECIAL MEMBER FUNCTIONS **
   constexpr Vector(Vector const &other)
       : Base{ other }
   {
      this->create_storage(other.size());
      this->m_finish = uninitialized_copy(
          this->m_allocator, other.begin(), other.end(), this->m_start);
   }

   constexpr Vector &
   operator=(Vector const &other)
   {
      if (this == std::addressof(other))
      {
         return *this;
      }

      if constexpr (a_traits::propagate_on_container_copy_assignment::value)
      {
         // If propagate is true, the source allocator must be copied into the
         // destination.

         if (this->m_allocator != other.m_allocator)
         {
            // If allocators do not compare as equal, the source allocator
            // cannot manage the memory allocated by the destination allocator
            // and must allocate new memory for the elements in the destination
            // instance using the source allocator, then the destination
            // allocator must free it's memory before being replaced with a
            // copy of the source allocator.

            Allocator new_alloc{ other.m_allocator };
            auto [ptr, count]{ a_traits::allocate_at_least(
                new_alloc, other.size()) };

            {
               detail::AllocationGuard mem_guard{ new_alloc, ptr, count };

               auto const new_finish{ uninitialized_copy(
                   new_alloc, other.begin(), other.end(), ptr) };

               clear();
               this->m_finish = new_finish;
               mem_guard.switch_allocator(this->m_allocator);
               mem_guard.reassign(this->m_start, capacity());
            }

            this->m_allocator = new_alloc;
            this->m_start = ptr;
            this->m_end_of_storage = ptr + count;
            return *this;
         }
         else
         {
            // If allocators do compare equal, the source allocator can manage
            // the memory of the destination allocator, but the allocator must
            // be copied per the propagate_on_container_copy_assignment
            // condition.

            this->m_allocator = other.m_allocator;
         }
      }

      // If propagate is false, copying of the allocator is not necessary, no
      // need to worry about which allocator can manage which memory since the
      // destination retains it's allocator. Simply copy over the elements from
      // source.

      if constexpr (std::is_nothrow_copy_assignable_v<T>)
      {
         // If copy assignment is nothrow, the existing memory may be able to be
         // reused if there's enough capacity to do so.

         if (capacity() >= other.capacity())
         {
            auto end_of_copied_data{ this->m_start };

            for (auto pair : std::views::zip(*this, other))
            {
               std::get<0>(pair) = std::get<1>(pair);
               ++end_of_copied_data;
            }

            // After data is copied, destroy the existing elements that were
            // not overwritten with the copied data so there's no phantom
            // elements hanging around.
            destroy(this->m_allocator, end_of_copied_data, this->m_finish);

            this->m_finish = end_of_copied_data;
            return *this;
         }
      }

      // If copy assignment may throw, allocate new memory to maintain strong
      // exception safety guarantees.

      auto [ptr, count]{ this->allocate_memory_for_at_least(other.size()) };

      {
         detail::AllocationGuard mem_guard{ this->m_allocator, ptr, count };

         auto const new_finish{ uninitialized_copy(
             this->m_allocator, other.begin(), other.end(), ptr) };

         clear();
         this->m_finish = new_finish;
         mem_guard.reassign(this->m_start, capacity());
      }

      this->m_start = ptr;
      this->m_end_of_storage = ptr + count;
      return *this;
   }

   constexpr Vector(Vector &&other) noexcept
       : Base(std::move(other))
   {}

   constexpr Vector &
   operator=(Vector &&other) noexcept(
       a_traits::propagate_on_container_move_assignment::value
       || a_traits::is_always_equal::value)
   {
      if (this == std::addressof(other))
      {
         return *this;
      }

      if constexpr (a_traits::propagate_on_container_move_assignment::value)
      {
         // Allocator can be moved with the container - noexcept.

         clear();
         this->deallocate_memory();
         this->m_allocator = std::move(other.m_allocator);
         this->m_start = other.m_start;
         this->m_finish = other.m_finish;
         this->m_end_of_storage = other.m_end_of_storage;
         other.m_start = other.m_finish = other.m_end_of_storage = nullptr;
         return *this;
      }
      else if (this->m_allocator == other.m_allocator)
      {
         // Allocator can't be moved over, but the destination and source
         // allocators compare equal.  Ignore the allocator, move the rest -
         // noexcept.

         clear();
         this->deallocate_memory();
         this->m_start = other.m_start;
         this->m_finish = other.m_finish;
         this->m_end_of_storage = other.m_end_of_storage;
         other.m_start = other.m_finish = other.m_end_of_storage = nullptr;
         return *this;
      }
      else
      {
         // Worst case scenario - may throw - we can't transfer memory at all
         // since the source allocator can't be moved and does not compare equal
         // to the destination allocator.  Allocate new memory with the
         // destination allocator and move the elements over.  On success,
         // destroy destination's existing objects, free the memory, and replace
         // the storage with the newly allocated memory.
         //
         // Once that's done, we have two options depending on how consistent we
         // want to be:
         // - Should the source wind up in the same state as the other cases? If
         // so, then destroy moved from elements, free the memory, and reset the
         // invariants.
         // - Or should we leave the source as is, with moved from elements left
         // in allocated memory?  If that's the case, we're done.  I'm opting
         // for this option because I believe there's two cases to consider
         // here:
         // 1. The moved from container is about to die anyway, so let the
         // destructor destroy the elements and free the memory.
         // 2. If the container is going to be reused, that previously allocated
         // memory can also be reused, saving an allocation.  The user just has
         // to be aware of the fact.  Extra documentation for the move
         // assignment operator, but I believe this to be the correct choice.

         auto [ptr, count]{ this->allocate_memory_for_at_least(other.size()) };
         {
            detail::AllocationGuard mem_guard{ this->m_allocator, ptr, count };

            auto const new_finish{ uninitialized_move_if_noexcept(
                this->m_allocator, begin(), end(), ptr) };

            clear();
            this->m_finish = new_finish;

            mem_guard.reassign(this->m_start, capacity());
         }

         this->m_start = ptr;
         this->m_end_of_storage = ptr + count;
         return *this;
      }
   }  // namespace swtl

   constexpr ~Vector()
   {
      clear();
      // Deallocation is handled by VectorBase.
   }

   // ** ITERATORS **
   template <typename Self>
   [[nodiscard]]
   constexpr auto
   begin(this Self &&self) noexcept
   {
      using const_correct_iterator = std::conditional_t<
          std::is_const_v<std::remove_reference_t<Self>>,
          const_iterator,
          iterator
      >;
      return static_cast<const_correct_iterator>(self.m_start);
   }

   template <typename Self>
   [[nodiscard]]
   constexpr auto
   end(this Self &&self) noexcept
   {
      using const_correct_iterator = std::conditional_t<
          std::is_const_v<std::remove_reference_t<Self>>,
          const_iterator,
          iterator
      >;
      return static_cast<const_correct_iterator>(self.m_finish);
   }

   [[nodiscard]]
   constexpr const_iterator
   cbegin() const noexcept
   {
      return begin();
   }

   [[nodiscard]]
   constexpr const_iterator
   cend() const noexcept
   {
      return end();
   }

   template <typename Self>
   [[nodiscard]]
   constexpr auto
   rbegin(this Self &&self) noexcept
   {
      using const_correct_reverse_iterator = std::conditional_t<
          std::is_const_v<std::remove_reference_t<Self>>,
          const_reverse_iterator,
          reverse_iterator
      >;
      return static_cast<const_correct_reverse_iterator>(self.end());
   }

   template <typename Self>
   [[nodiscard]]
   constexpr auto
   rend(this Self &&self) noexcept
   {
      using const_correct_reverse_iterator = std::conditional_t<
          std::is_const_v<std::remove_reference_t<Self>>,
          const_reverse_iterator,
          reverse_iterator
      >;
      return static_cast<const_correct_reverse_iterator>(self.begin());
   }

   [[nodiscard]]
   constexpr const_reverse_iterator
   crbegin() const noexcept
   {
      return rbegin();
   }

   [[nodiscard]]
   constexpr const_reverse_iterator
   crend() const noexcept
   {
      return rend();
   }

   // ** ELEMENT ACCESS **
   template <typename Self>
   [[nodiscard]]
   constexpr decltype(auto)
   at(this Self &&self, size_type position)
   {
      if (position >= self.size())
      {
         std::string reason{ "Vector Range Check: position (which is " };
         reason += integral_to_string(position);
         reason += ") >= this->size() (which is ";
         reason += integral_to_string(self.size());
         reason += ")";
         throw std::out_of_range(reason);
      }
      return std::forward_like<Self>(*(self.m_start + position));
   }

   // TODO: Add contract precondition.
   template <typename Self>
   [[nodiscard]]
   constexpr decltype(auto)
   operator[](this Self &&self, size_type position) noexcept
   {
      return std::forward_like<Self>(*(self.m_start + position));
   }

   // TODO: Add contract precondition.
   template <typename Self>
   [[nodiscard]]
   constexpr decltype(auto)
   front(this Self &&self) noexcept
   {
      return std::forward_like<Self>(*self.m_start);
   }

   // TODO: Add contract precondition.
   template <typename Self>
   [[nodiscard]]
   constexpr decltype(auto)
   back(this Self &&self) noexcept
   {
      return std::forward_like<Self>(*(self.m_finish - 1));
   }

   template <typename Self>
   [[nodiscard]]
   constexpr auto
   data(this Self &&self) noexcept
   {
      using const_correct_pointer = std::conditional_t<
          std::is_const_v<std::remove_reference_t<Self>>,
          const_pointer,
          pointer
      >;
      return static_cast<const_correct_pointer>(self.m_start);
   }

   // ** CAPACITY **
   [[nodiscard]]
   constexpr bool
   is_empty() const noexcept
   {
      return this->m_start == this->m_finish;
   }

   [[nodiscard]]
   constexpr size_type
   size() const noexcept
   {
      return static_cast<size_type>(this->m_finish - this->m_start);
   }

   [[nodiscard]]
   constexpr size_type
   max_size() const noexcept
   {
      return this->max_allocatable_elements();
   }

   constexpr void
   reserve(size_type new_capacity)
   {
      if (new_capacity > max_size())
      {
         std::string reason{ "Vector::reserve: new_capacity (which is " };
         reason += integral_to_string(new_capacity);
         reason += ") is greater than max_size() (which is ";
         reason += integral_to_string(max_size());
         reason += ").";
         throw std::length_error(reason);
      }

      if (new_capacity <= capacity())
      {
         return;
      }

      if (this->m_start == nullptr)
      {
         this->create_storage(new_capacity);
         return;
      }

      auto [ptr, count]{ this->allocate_memory_for_at_least(new_capacity) };

      {
         detail::AllocationGuard mem_guard{ this->m_allocator, ptr, count };

         auto const new_finish{ uninitialized_move_if_noexcept(
             this->m_allocator, begin(), end(), ptr) };

         clear();
         this->m_finish = new_finish;

         mem_guard.reassign(this->m_start, capacity());
      }

      this->m_start = ptr;
      this->m_end_of_storage = ptr + count;
   }

   [[nodiscard]]
   constexpr size_type
   capacity() const noexcept
   {
      return this->allocated_capacity();
   }

   // shrink_to_fit()

   // ** MODIFIERS **
   constexpr void
   clear() noexcept
   {
      destroy(this->m_allocator, begin(), end());
      this->m_finish = this->m_start;
   }

   // TODO: insert()
   // TODO: insert_range()
   // TODO: emplace()
   // TODO: erase()
   constexpr void
   push_back(T const &value)
   {
      emplace_back(value);
   }

   constexpr void
   push_back(T &&value)
   {
      emplace_back(std::move(value));
   }

   template <typename... Args>
   constexpr reference
   emplace_back(Args &&...args)
   {
      if (this->m_finish == this->m_end_of_storage)
      {
         if (this->m_end_of_storage == nullptr)
         {
            this->create_storage(1);
         }
         else
         {
            return realloc_emplace(std::forward<Args>(args)...);
         }
      }

      a_traits::construct(
          this->m_allocator, this->m_finish, std::forward<Args>(args)...);
      return *this->m_finish++;
   }

   // TODO: append_range()
   // TODO: pop_back()
   // TODO: resize()

   constexpr void
   swap(Vector &other) noexcept
   {
      if constexpr (a_traits::propagate_on_container_swap::value)
      {
         using std::swap;
         swap(this->m_allocator, other.m_allocator);
      }
      else if constexpr (!a_traits::is_always_equal::value)
      {
         contract_assert(
             this->m_allocator != other.m_allocator
             /*"If propagate_on_container_swap is not provided or
               is derived from std::false_type and the allocators
               of the two containers do not compare equal, the
               behavior of container swap is undefined."*/
             && "You are invoking undefined behavior here.");
      }

      std::swap(this->m_start, other.m_start);
      std::swap(this->m_finish, other.m_finish);
      std::swap(this->m_end_of_storage, other.m_end_of_storage);
   }

   // ** NON-MEMBER FUNCTIONS **
   constexpr friend auto
   swap(Vector &lhs, Vector &rhs) noexcept -> void
   {
      lhs.swap(rhs);
   }

   constexpr friend auto
   operator==(Vector const &lhs, Vector const &rhs)
       noexcept(noexcept(std::declval<T>() == std::declval<T>())) -> bool
      requires std::equality_comparable<T>
   {
      if (rhs.size() != lhs.size())
      {
         return false;
      }
      return std::ranges::equal(lhs, rhs);
   }

   constexpr friend auto
   operator<=>(Vector const &lhs, Vector const &rhs)
       noexcept(noexcept(std::declval<T>() <=> std::declval<T>()))
      requires std::three_way_comparable<T>
   {
      for (auto const pair : std::views::zip(lhs, rhs))
      {
         if (auto const comparison{ std::get<0>(pair) <=> std::get<1>(pair) };
             comparison != 0)
         {
            return comparison;
         }
      }
      return lhs.size() <=> rhs.size();
   }

private:
   template <
       std::input_iterator InputIterator,
       std::sentinel_for<InputIterator> Sentinel
   >
   constexpr void
   assign_from_range(InputIterator first, Sentinel last)
   {
      auto current{ begin() };

      while (first != last && current != end())
      {
         *current++ = *first++;
      }

      if (first == last)
      {
         destroy(this->m_allocator, current, end());
         this->m_finish = std::to_address(current);
         return;
      }

      while (first != last)
      {
         push_back(*first++);
      }
   }

   template <
       std::forward_iterator ForwardIterator,
       std::sentinel_for<ForwardIterator> Sentinel
   >
   constexpr void
   assign_from_range(ForwardIterator first, Sentinel last)
   {
      if (auto const input_size{
            static_cast<size_type>(std::distance(first, last)) };
          capacity() < input_size)
      {
         auto [ptr, count]{ this->allocate_memory_for_at_least(input_size) };

         if constexpr (std::is_nothrow_copy_constructible_v<T>)
         {
            clear();
            this->deallocate_memory();
            this->m_start = ptr;
            this->m_end_of_storage = ptr + count;

            this->m_finish
                = uninitialized_copy(this->m_allocator, first, last, ptr);
         }
         else
         {
            detail::AllocationGuard mem_guard(this->m_allocator, ptr, count);

            auto new_finish{ uninitialized_copy(
                this->m_allocator, first, last, ptr) };

            clear();
            mem_guard.reassign(this->m_start, capacity());

            this->m_start = ptr;
            this->m_finish = new_finish;  // Assign AFTER the call to clear().
            this->m_end_of_storage = ptr + count;
         }
      }
      else
      {
         auto current{ begin() };

         while (first != last && current != end())
         {
            *current++ = *first++;
         }

         if (first == last)
         {
            destroy(this->m_allocator, current, end());
            this->m_finish = std::to_address(current);
            return;
         }

         while (first != last)
         {
            a_traits::construct(this->m_allocator, this->m_finish, *first++);
            ++this->m_finish;  // Only increment after successful construction.
         }
      }
   }

   [[nodiscard]]
   constexpr size_type
   calculate_growth_size(size_type target_growth = 1UZ)
   {
      auto const current_size{ size() };
      auto const max_possible_growth{ max_size() - current_size };

      if (max_possible_growth < target_growth)
      {
         throw std::length_error(
             "Vector growth request exceeded maximum possible size.");
      }

      if (target_growth < current_size)
      {
         target_growth = current_size;
      }
      if (target_growth > max_possible_growth)
      {
         target_growth = max_possible_growth;
      }
      return current_size + target_growth;
   }

   template <typename... Args>
   constexpr reference
   realloc_emplace(Args &&...args)
   {
      auto [ptr, count]{ this->allocate_memory_for_at_least(
          calculate_growth_size()) };
      auto const new_element_ptr{ ptr + size() };
      auto const new_element_finish{ new_element_ptr + 1 };

      {
         detail::AllocationGuard mem_guard{ this->m_allocator, ptr, count };

         a_traits::construct(
             this->m_allocator, new_element_ptr, std::forward<Args>(args)...);
         {
            detail::ElementGuard elem_guard{ this->m_allocator,
                                             new_element_ptr,
                                             new_element_finish };

            uninitialized_move_if_noexcept(
                this->m_allocator, begin(), end(), ptr);

            elem_guard.reassign(this->m_start, this->m_finish);
            mem_guard.reassign(this->m_start, capacity());

            this->m_start = ptr;
            this->m_finish = new_element_finish;
            this->m_end_of_storage = ptr + count;
            return back();
         }
      }
   }
};

// Explicit Deduction Guides for CTAD.
template <
    std::input_iterator InputIterator,
    std::sentinel_for<InputIterator> Sentinel
>
Vector(InputIterator, Sentinel) -> Vector<std::iter_value_t<InputIterator>>;

template <std::ranges::input_range Range>
Vector(std::from_range_t, Range &&)
    -> Vector<std::ranges::range_value_t<Range>>;

}  // namespace swtl
