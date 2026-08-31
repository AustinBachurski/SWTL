module;

#include <cassert>

export module swtl.vector;

import std;

import swtl.config;

import swtl.algorithm;
import swtl.container_concepts;
import swtl.contiguous_iterator;
import swtl.format;
import swtl.memory;

namespace swtl
{

/// @cond INTERNAL_DOCUMENTATION
/// @brief Resource management base class for `Vector`.
///
/// Handles raw memory allocation, deallocation, and allocator propagation
/// mechanics independently of element construction and destruction.
///
/// @tparam T The type of element to allocate storage for.
/// @tparam Allocator The allocator type used to acquire and release memory.
///
template <typename T, typename Allocator>
class VectorBase
{
public:
   /// @name Aliases
   /// @{

   /// Allocator Traits alias.
   using a_traits = std::allocator_traits<Allocator>;

   /// @}

   /// @name Member Types
   /// @{

   /// Allocator type.
   using allocator_type = Allocator;

   /// Value type.
   using value_type = a_traits::value_type;

   /// Unsigned size type.
   using size_type = a_traits::size_type;

   /// Signed difference type.
   using difference_type = a_traits::difference_type;

   /// Pointer to element.
   using pointer = a_traits::pointer;

   /// @}

   /// @brief Default constructor. Initializes with a default-constructed
   /// allocator and null storage pointers.
   ///
   constexpr VectorBase() = default;

   /// @brief Constructs the base storage with a specific allocator copy.
   ///
   /// @param allocator The allocator to copy.
   ///
   constexpr VectorBase(Allocator const &allocator)
       : m_allocator{ allocator }
   {}

   /// @brief Copy constructor. Copies the allocator using
   /// `select_on_container_copy_construction`.
   ///
   /// @param other The base storage object to copy the allocator from.
   ///
   constexpr VectorBase(VectorBase const &other) noexcept
       : m_allocator{ a_traits::select_on_container_copy_construction(
             other.m_allocator) }
   {}

   /// @brief Move constructor. Transfers storage ownership and moves the
   /// allocator.
   ///
   /// @param other The base storage object to move from. Leaves `other` with
   /// null storage pointers.
   ///
   constexpr VectorBase(VectorBase &&other) noexcept
       : m_allocator{ std::move(other.m_allocator) }
       , m_start{ other.m_start }
       , m_finish{ other.m_finish }
       , m_end_of_storage{ other.m_end_of_storage }
   {
      other.m_start = other.m_finish = other.m_end_of_storage = nullptr;
   }

   /// @brief Copy assignment operator.
   ///
   constexpr VectorBase &
   operator=(VectorBase const &other)
       = delete ("Copy assignment must be handled by the Vector container.");

   /// @brief Move assignment operator.
   ///
   constexpr VectorBase &
   operator=(VectorBase &&other)
       = delete ("Move assignment must be handled by the Vector container.");

   /// @brief Destructor. Deallocates any held memory storage.
   ///
   /// @note Does not destroy elements; element destruction is the
   /// responsibility of the derived container class.
   ///
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
   /// @brief Allocates raw storage for at least `num_elements`.
   ///
   /// Uses `allocate_at_least` to take advantage of any excess capacity
   /// provided by the underlying allocator, capping the reported count at
   /// `max_allocatable_elements()`.
   ///
   /// @param num_elements The minimum number of elements to allocate storage
   /// for.
   ///
   /// @return `std::allocation_result` containing the pointer and actual
   /// capacity allocated.
   ///
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

   /// @brief Returns the current allocated storage capacity in number of
   /// elements.
   ///
   /// @return The allocated capacity.
   ///
   [[nodiscard]]
   constexpr size_type
   allocated_capacity() const noexcept
   {
      return static_cast<size_type>(m_end_of_storage - m_start);
   }

   /// @brief Allocates storage and initializes `m_start`, `m_finish`, and
   /// `m_end_of_storage`.
   ///
   /// @param num_elements The number of elements to allocate storage for.
   ///
   /// @throws (...) Any exception thrown by the allocator as a result of a call
   /// to `std::allocator_traits<Allocator>::allocate`.
   ///
   /// @note Provides a strong exception guarantee: if an exception is thrown,
   /// no resources are leaked and the vector is unmodified.
   ///
   constexpr void
   create_storage(size_type num_elements)
   {
      auto [ptr, count]{ allocate_memory_for_at_least(num_elements) };

      m_start = m_finish = ptr;
      m_end_of_storage = m_start + count;
   }

   /// @brief Deallocates all currently owned storage and resets pointers to
   /// null.
   ///
   /// @note Does not destroy elements prior to deallocation.
   ///
   constexpr void
   deallocate_memory() noexcept
   {
      a_traits::deallocate(m_allocator, m_start, allocated_capacity());
      m_start = m_finish = m_end_of_storage = nullptr;
   }

   /// @brief Computes the theoretical maximum number of elements this allocator
   /// can hold.
   ///
   /// @return The minimum of `allocator_traits::max_size` and pointer
   /// difference limits.
   ///
   [[nodiscard]]
   constexpr size_type
   max_allocatable_elements() const noexcept
   {
      return std::min<size_type>(
          a_traits::max_size(m_allocator),
          std::numeric_limits<difference_type>::max() / sizeof(T));
   }

   /// Underlying memory allocator instance.
   [[no_unique_address]]
   Allocator m_allocator;

   /// Pointer to the beginning of allocated storage.
   pointer m_start{};

   /// Pointer to one-past-the-end of constructed elements.
   pointer m_finish{};

   /// Pointer to one-past-the-end of allocated storage.
   pointer m_end_of_storage{};
};

/// @endcond

/// @brief A dynamically resizable contiguous array container.
///
/// Satisfies the standard container requirements, contiguous container
/// requirements, and allocator-aware container requirements.
///
/// @tparam T The type of element stored in the vector.
/// @tparam Allocator The allocator type used for memory management.
///
export template <typename T, typename Allocator = std::allocator<T>>
class Vector : protected VectorBase<T, Allocator>
{
private:
   /// @publicsection
   ///
   /// @name Aliases
   /// @{

   /// @cond INTERNAL_DOCUMENTATION

   /// VectorBase alias
   using Base = VectorBase<T, Allocator>;

   /// @endcond

   /// Allocator Traits alias
   using a_traits = std::allocator_traits<Allocator>;

   /// @}

public:
   /// @name Member Types
   /// @{

   /// Element value type.
   using value_type = std::remove_cv_t<T>;

   /// Allocator type.
   using allocator_type = Base::allocator_type;

   /// Unsigned size type.
   using size_type = std::size_t;

   /// Signed difference type.
   using difference_type = std::ptrdiff_t;

   /// Reference type.
   using reference = value_type &;

   /// Const reference type.
   using const_reference = value_type const &;

   /// Pointer type.
   using pointer = Base::pointer;

   /// Const pointer type.
   using const_pointer = a_traits::const_pointer;

   /// Random-access contiguous iterator.
   using iterator = ContiguousIterator<T>;

   /// Const contiguous iterator.
   using const_iterator = ContiguousIterator<T const>;

   /// Reverse contiguous iterator.
   using reverse_iterator = std::reverse_iterator<iterator>;

   /// Const reverse contiguous iterator.
   using const_reverse_iterator = std::reverse_iterator<const_iterator>;

   /// @}

public:
   /// @name Constructors
   /// @{

   /// @brief Default constructor. Constructs an empty vector with an optional
   /// allocator.
   ///
   /// @param allocator The allocator to use for memory allocation.
   ///
   constexpr Vector(Allocator const &allocator = Allocator())
       : Base{ allocator }
   {}

   /// @brief Constructs a vector with copies of the elements from an
   /// initializer list.
   ///
   /// @param init_list The initializer list to copy elements from.
   /// @param allocator The allocator to use for memory allocation.
   ///
   /// @throws (...) Any exception thrown by the allocator as a result of a call
   /// to `std::allocator_traits<Allocator>::allocate`.
   /// @throws (...) Any exception thrown by T during element construction.
   ///
   /// @note Provides a strong exception guarantee: if an exception is thrown
   /// during construction, all elements that were constructed prior to the
   /// exception are destroyed and any allocated memory is deallocated.
   ///
   /// @note Not marked as explicit to allow for implicit conversion from a
   /// braced initializer list.
   ///
   constexpr Vector(
       std::initializer_list<T> const &init_list,
       Allocator const &allocator = Allocator())
       : Base{ allocator }
   {
      this->create_storage(init_list.size());
      this->m_finish = uninitialized_copy(
          this->m_allocator, init_list.begin(), init_list.end(), this->m_start);
   }

   /// @brief Constructs a vector with `count` default-inserted elements.
   ///
   /// @param count The number of elements to construct.
   /// @param allocator The allocator to use for memory allocation.
   ///
   /// @throws (...) Any exception thrown by the allocator as a result of a call
   /// to `std::allocator_traits<Allocator>::allocate`.
   /// @throws (...) Any exception thrown by T during element construction.
   ///
   /// @note Provides a strong exception guarantee: if an exception is thrown
   /// during construction, all elements that were constructed prior to the
   /// exception are destroyed and any allocated memory is deallocated.
   ///
   constexpr explicit Vector(
       size_type count, Allocator const &allocator = Allocator())
       : Base{ allocator }
   {
      this->create_storage(count);

      detail::ElementGuard elem_guard(
          this->m_allocator, this->m_start, this->m_start);

      for (; count != 0UZ; --count)
      {
         a_traits::construct(this->m_allocator, elem_guard.last);
         ++elem_guard.last;
      }

      this->m_finish = elem_guard.last;
      elem_guard.dismiss();
   }

   /// @brief Constructs a vector with `count` copies of `value`.
   ///
   /// @param count The number of elements to construct.
   /// @param value The value to copy-construct elements from.
   ///
   /// @throws (...) Any exception thrown by the allocator as a result of a call
   /// to `std::allocator_traits<Allocator>::allocate`.
   /// @throws (...) Any exception thrown by T during element construction.
   ///
   /// @note Provides a strong exception guarantee: if an exception is thrown
   /// during construction, all elements that were constructed prior to the
   /// exception are destroyed and any allocated memory is deallocated.
   ///
   constexpr Vector(
       size_type count,
       T const &value,
       Allocator const &allocator = Allocator())
       : Base{ allocator }
   {
      this->create_storage(count);

      detail::ElementGuard elem_guard(
          this->m_allocator, this->m_start, this->m_start);

      for (; count != 0UZ; --count)
      {
         a_traits::construct(this->m_allocator, elem_guard.last, value);
         ++elem_guard.last;
      }

      this->m_finish = elem_guard.last;
      elem_guard.dismiss();
   }

   /// @brief Constructs a vector with the contents of the range `[first,
   /// last)`.
   ///
   /// @tparam InputIterator An iterator type satisfying `std::input_iterator`.
   /// @tparam Sentinel A sentinel type for `InputIterator`.
   ///
   /// @param first The beginning of the range to copy.
   /// @param last The end of the range to copy.
   /// @param allocator The allocator to use for memory allocation.
   ///
   /// @throws (...) Any exception thrown by the allocator as a result of a call
   /// to `std::allocator_traits<Allocator>::allocate`.
   /// @throws (...) Any exception thrown by T during element construction.
   ///
   /// @note Provides a strong exception guarantee: if an exception is thrown
   /// during construction, all elements that were constructed prior to the
   /// exception are destroyed and any allocated memory is deallocated.
   ///
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
      if constexpr (
          std::sized_sentinel_for<Sentinel, InputIterator>
          || std::forward_iterator<InputIterator>)
      {
         this->create_storage(
             static_cast<size_type>(std::ranges::distance(first, last)));
         this->m_finish = uninitialized_copy(
             this->m_allocator, first, last, this->m_start);
      }
      else
      {
         try
         {
            while (first != last)
            {
               push_back(*first);
               // Post-increment returns void for single pass iterators.
               ++first;
            }
         }
         catch (...)
         {
            clear();
            throw;
         }
      }
   }

   /// @brief Constructs a vector from a container-compatible range.
   ///
   /// @tparam Range A range type compatible with `T`.
   ///
   /// @param range The input range to copy or move elements from.
   ///
   /// @throws (...) Any exception thrown by the allocator as a result of a call
   /// to `std::allocator_traits<Allocator>::allocate`.
   /// @throws (...) Any exception thrown by T during element construction.
   ///
   /// @note Provides a strong exception guarantee: if an exception is thrown
   /// during construction, all elements that were constructed prior to the
   /// exception are destroyed and any allocated memory is deallocated.
   ///
   template <container_compatible_range<T> Range>
   constexpr Vector(
       std::from_range_t,
       Range &&range,
       Allocator const &allocator = Allocator())
       : Base{ allocator }
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
         try
         {
            for (auto &&element : range)
            {
               push_back(element);
            }
         }
         catch (...)
         {
            clear();
            throw;
         }
      }
   }

   /// @}

   /// @name Assignment
   /// @{

   /// @brief Replaces the vector's contents with `count` copies of `value`.
   ///
   /// @param count The new size of the vector.
   /// @param value The value to assign to all elements.
   ///
   /// @throws (...) Any exception thrown by the allocator as a result of a call
   /// to `std::allocator_traits<Allocator>::allocate` if reallocation occurs.
   /// @throws (...) Any exception thrown by T during element assignment or
   /// construction.
   ///
   /// @note Provides a strong exception guarantee if reallocation occurs: if an
   /// exception is thrown during reallocation, any new elements that were
   /// constructed prior to the exception are destroyed, any newly allocated
   /// memory is deallocated, and the vector is unmodified.
   ///
   /// @note Provides a basic exception guarantee if no reallocation occurs: if
   /// an exception is thrown during element assignment or construction, the
   /// vector is left in a valid but unspecified state and no resources are
   /// leaked.
   ///
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
                      this->m_allocator, elem_guard.last, value);
                  ++elem_guard.last;
               }

               // Scary part's over, reassign the guards to the old elements and
               // memory.
               mem_guard.reassign(this->m_start, capacity());
               elem_guard.reassign(
                   this->m_start,
                   std::exchange(this->m_finish, elem_guard.last));

               this->m_start = ptr;
               this->m_end_of_storage = ptr + size;
            }
         }
      }
      else
      {
         auto current = this->m_start;

         // Overwrite existing elements.
         for (; current != this->m_finish && count != 0UZ; --count)
         {
            *current++ = value;
         }

         // Destroy old elements if we haven't reached `m_finish`.
         this->m_finish = destroy(this->m_allocator, current, this->m_finish);

         // Construct the new elements in memory.
         for (; count != 0UZ; --count)
         {
            a_traits::construct(this->m_allocator, this->m_finish, value);
            ++this->m_finish;  // Increment after successful construction.
         }
      }
   }

   /// @brief Replaces the vector's contents with the elements from the range
   /// `[first, last)`.
   ///
   /// @tparam InputIterator An iterator type satisfying `std::input_iterator`.
   /// @tparam Sentinel A sentinel type for `InputIterator`.
   ///
   /// @param first The beginning of the range.
   /// @param last The end of the range.
   ///
   /// @note Elements are copied or moved depending on the value category of
   /// `*first` (e.g., passing a `std::move_iterator` will move elements into
   /// the container).
   ///
   /// @throws (...) Any exception thrown by the allocator as a result of a call
   /// to `std::allocator_traits<Allocator>::allocate` if reallocation occurs.
   /// @throws (...) Any exception thrown by T during element assignment or
   /// construction.
   ///
   /// @note Provides a strong exception guarantee if the size of the range can
   /// be checked and reallocation occurs: if an exception is thrown during
   /// reallocation, any new elements that were constructed prior to the
   /// exception are destroyed, any newly allocated memory is deallocated, and
   /// the vector is unmodified.
   ///
   /// @note Provides a basic exception guarantee if the size of the range
   /// cannot be checked: if an exception is thrown during element assignment or
   /// construction, the vector is left in a valid but unspecified state and no
   /// resources are leaked.
   ///
   /// @note If `first` is a move iterator and an exception is thrown, any
   /// previously processed elements in the source range remain in a moved-from
   /// state.
   ///
   template <
       std::input_iterator InputIterator,
       std::sentinel_for<InputIterator> Sentinel
   >
   constexpr void
   assign(InputIterator first, Sentinel last)
   {
      if constexpr (std::sized_sentinel_for<Sentinel, InputIterator>)
      {
         assert(last - first >= 0 && "`last` must be reachable from `first`");
      }

      // If distance can be checked then do so and reallocate or overwrite
      // elements as necessary.
      if constexpr (
          std::sized_sentinel_for<Sentinel, InputIterator>
          || std::forward_iterator<InputIterator>)
      {
         if (auto const input_size{
               static_cast<size_type>(std::ranges::distance(first, last)) };
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

               auto const new_finish{ uninitialized_copy(
                   this->m_allocator, first, last, ptr) };

               clear();
               mem_guard.reassign(this->m_start, capacity());

               this->m_start = ptr;
               this->m_finish
                   = new_finish;  // Assign AFTER the call to clear().
               this->m_end_of_storage = ptr + count;
            }
         }
         else
         {
            auto [src_pos, dest_pos]{ zip_copy(first, last, begin(), end()) };

            this->m_finish
                = std::to_address(destroy(this->m_allocator, dest_pos, end()));

            while (src_pos != last)
            {
               a_traits::construct(
                   this->m_allocator, this->m_finish, *src_pos++);
               ++this->m_finish;  // Increment after successful construction.
            }
         }
      }
      // If distance cannot be checked, overwrite existing elements and then
      // let push_back resize if needed.
      else
      {
         auto [src_pos, dest_pos]{ zip_copy(first, last, begin(), end()) };

         this->m_finish
             = std::to_address(destroy(this->m_allocator, dest_pos, end()));

         while (src_pos != last)
         {
            push_back(*src_pos);
            // Post-increment returns void for single pass iterators.
            ++src_pos;
         }
      }
   }

   /// @brief Replaces the vector's contents with the elements from an
   /// initializer list.
   ///
   /// @param init_list The initializer list to copy from.
   ///
   /// @throws (...) Any exception thrown by the allocator as a result of a call
   /// to `std::allocator_traits<Allocator>::allocate` if reallocation occurs.
   /// @throws (...) Any exception thrown by T during element assignment or
   /// construction.
   ///
   /// @note Provides a strong exception guarantee if reallocation occurs: if an
   /// exception is thrown during reallocation, any new elements that were
   /// constructed prior to the exception are destroyed, any newly allocated
   /// memory is deallocated, and the vector is unmodified.
   ///
   /// @note Provides a basic exception guarantee if no reallocation occurs: if
   /// an exception is thrown during element assignment or construction, the
   /// vector is left in a valid but unspecified state and no resources are
   /// leaked.
   ///
   constexpr void
   assign(std::initializer_list<T> init_list)
   {
      assign(init_list.begin(), init_list.end());
   }

   /// @brief Replaces the vector's contents with the elements from a
   /// container-compatible range.
   ///
   /// @tparam Range A range type compatible with `T`.
   ///
   /// @param range The range to copy or move elements from.
   ///
   /// @throws (...) Any exception thrown by the allocator as a result of a call
   /// to `std::allocator_traits<Allocator>::allocate` if reallocation occurs.
   /// @throws (...) Any exception thrown by T during element assignment or
   /// construction.
   ///
   /// @note Provides a strong exception guarantee if the size of the range can
   /// be checked and reallocation occurs: if an exception is thrown during
   /// reallocation, any new elements that were constructed prior to the
   /// exception are destroyed, any newly allocated memory is deallocated, and
   /// the vector is unmodified.
   ///
   /// @note Provides a basic exception guarantee if the size of the range
   /// cannot be checked: if an exception is thrown during element assignment or
   /// construction, the vector is left in a valid but unspecified state and no
   /// resources are leaked.
   ///
   /// @note If elements are moved from the source range and an exception is
   /// thrown, any previously processed elements in the source range remain in a
   /// moved-from state.
   ///
   template <container_compatible_range<T> Range>
   constexpr void
   assign_range(Range &&range)
   {
      assign(std::ranges::begin(range), std::ranges::end(range));
   }

   /// @}

   /// @name Special Member Functions
   /// @{

   /// @brief Copy constructor. Constructs a container with a copy of `other`'s
   /// elements.
   ///
   /// @param other The vector to copy from.
   ///
   /// @throws (...) Any exception thrown by the allocator as a result of a call
   /// to `std::allocator_traits<Allocator>::allocate`.
   /// @throws (...) Any exception thrown by T during element construction.
   ///
   /// @note Provides a strong exception guarantee: if an exception is thrown
   /// during construction, any new elements that were constructed prior to the
   /// exception are destroyed and any newly allocated memory is deallocated.
   /// The source vector remains in an unmodified state.
   ///
   constexpr Vector(Vector const &other)
       : Base{ other }
   {
      this->create_storage(other.size());
      this->m_finish = uninitialized_copy(
          this->m_allocator, other.begin(), other.end(), this->m_start);
   }

   /// @brief Copy assignment operator. Replaces the vector's contents with a
   /// copy of `other`.
   ///
   /// Observes `propagate_on_container_copy_assignment` to determine
   /// whether the destination allocator should be replaced.
   ///
   /// @param other The vector to copy from.
   ///
   /// @return A reference to `*this`.
   ///
   /// @throws (...) Any exception thrown by the allocator as a result of a call
   /// to `std::allocator_traits<Allocator>::allocate` if reallocation occurs or
   /// the allocators do not compare as equal to one another.
   /// @throws (...) Any exception thrown by T during element assignment or
   /// construction.
   ///
   /// @note Provides a strong exception guarantee if
   /// `propagate_on_container_copy_assignment` is true and the destination and
   /// source allocators do not compare as equal to one another: if an
   /// exception is thrown during copy assignment, any new elements that were
   /// constructed prior to the exception are destroyed, any newly allocated
   /// memory is deallocated, and the vector is unmodified.
   ///
   /// @note Provides a strong exception guarantee if reallocation occurs: if an
   /// exception is thrown during reallocation, any new elements that were
   /// constructed prior to the exception are destroyed, any newly allocated
   /// memory is deallocated, and the vector is unmodified.
   ///
   /// @note Provides a basic exception guarantee if the elements from `other`
   /// will fit in the existing storage of `*this`: if an exception is thrown
   /// during element assignment, the vector is left in a valid but unspecified
   /// state and no resources are leaked.
   ///
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

               // Scary part's over, destroy old elements and reassign the guard
               // to the old memory.
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

      assign(other.begin(), other.end());
      return *this;
   }

   /// @brief Move constructor. Transfers ownership of `other`'s storage.
   ///
   /// @param other The vector to move from.
   ///
   /// @note Move construction is handled by the base class.
   ///
   constexpr Vector(Vector &&other) noexcept
       : Base(std::move(other))
   {}

   /// @brief Move assignment operator. Transfers ownership or moves elements
   /// from `other`.
   ///
   /// Observes `propagate_on_container_move_assignment` to determine allocator
   /// behavior.
   ///
   /// @warning If allocators are not propagated nor compare equal, new storage
   /// is allocated and elements are moved individually.  The storage of `other`
   /// remains and its elements are in a moved-from state.  This may result in
   /// a loss of data if an exception occurs during element move construction.
   ///
   /// @param other The vector to move from.
   ///
   /// @return A reference to `*this`.
   ///
   /// @throws noexcept If `noexcept(
   /// std::allocator_traits<Allocator>::propagate_on_container_move_assignment::value
   /// || std::allocator_traits<Allocator>::is_always_equal::value)`
   /// @throws (...) Any exception thrown by the allocator as a result of a call
   /// to `std::allocator_traits<Allocator>::allocate` if the allocators do not
   /// propagate nor compare as equal to one another.
   /// @throws (...) Any exception thrown by T during element construction if
   /// the allocators do not propagate nor compare as equal to one another.
   ///
   /// @note Provides a basic exception guarantee if the allocators do not
   /// propagate nor compare equal: if an exception is thrown during
   /// construction, any new elements that were constructed prior to the
   /// exception are destroyed and any newly allocated memory is deallocated.
   /// Any previously processed elements in the source vector remain in a
   /// moved-from state.
   ///
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

            // Scary part's over, destroy old elements and reassign the guard to
            // the old memory.
            clear();
            this->m_finish = new_finish;
            mem_guard.reassign(this->m_start, capacity());
         }

         this->m_start = ptr;
         this->m_end_of_storage = ptr + count;
         return *this;
      }
   }

   /// @brief Destructor. Destroys all elements; storage deallocation is handled
   /// by the base class.
   ///
   constexpr ~Vector()
   {
      clear();
      // Deallocation is handled by the base class.
   }

   /// @}

   /// @name Allocator
   /// @{

   /// @brief Returns a copy of the allocator associated with the container.
   ///
   /// @return The underlying allocator copy.
   ///
   [[nodiscard]]
   constexpr allocator_type
   get_allocator() const noexcept
   {
      return this->m_allocator;
   }

   /// @}

   /// @name Iterators
   /// @{

   /// @brief Returns an iterator to the beginning of the vector.
   ///
   /// Uses explicit object parameter deduction to yield either `iterator` or
   /// `const_iterator` depending on the const-qualification of `self`.
   ///
   /// @tparam Self Deduced self type.
   ///
   /// @param self The vector object.
   ///
   /// @return An iterator to the first element.
   ///
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

   /// @brief Returns an iterator to one past the end of the vector.
   ///
   /// Uses explicit object parameter deduction to yield either `iterator` or
   /// `const_iterator` depending on the const-qualification of `self`.
   ///
   /// @tparam Self Deduced self type.
   ///
   /// @param self The vector object.
   ///
   /// @return An iterator to the element following the last element.
   ///
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

   /// @brief Returns a const iterator to the beginning of the vector.
   ///
   /// @return A `const_iterator` pointing to the first element.
   ///
   [[nodiscard]]
   constexpr const_iterator
   cbegin() const noexcept
   {
      return begin();
   }

   /// @brief Returns a const iterator to one past the end of the vector.
   ///
   /// @return A `const_iterator` pointing one past the last element.
   ///
   [[nodiscard]]
   constexpr const_iterator
   cend() const noexcept
   {
      return end();
   }

   /// @brief Returns a reverse iterator to the first element of the reversed
   /// vector.
   ///
   /// Uses explicit object parameter deduction to yield either `iterator` or
   /// `const_iterator` depending on the const-qualification of `self`.
   ///
   /// @tparam Self Deduced self type.
   ///
   /// @param self The vector object.
   ///
   /// @return A reverse iterator pointing to the last element of the
   /// non-reversed vector.
   ///
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

   /// @brief Returns a reverse iterator to one past the end of the reversed
   /// vector.
   ///
   /// Uses explicit object parameter deduction to yield either `iterator` or
   /// `const_iterator` depending on the const-qualification of `self`.
   ///
   /// @tparam Self Deduced self type.
   ///
   /// @param self The vector object.
   ///
   /// @return A reverse iterator pointing one before the first element of the
   /// non-reversed vector.
   ///
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

   /// @brief Returns a const reverse iterator to the beginning of the reversed
   /// vector.
   ///
   /// @return A `const_reverse_iterator` pointing to the last element.
   ///
   [[nodiscard]]
   constexpr const_reverse_iterator
   crbegin() const noexcept
   {
      return rbegin();
   }

   /// @brief Returns a const reverse iterator to one past the end of the
   /// reversed vector.
   ///
   /// @return A `const_reverse_iterator` pointing one before the first element.
   ///
   [[nodiscard]]
   constexpr const_reverse_iterator
   crend() const noexcept
   {
      return rend();
   }

   /// @}

   /// @name Element Access
   /// @{

   /// @brief Accesses the element at `position` with bounds checking.
   ///
   /// Uses explicit object parameter deduction to yield either `reference` or
   /// `const_reference` depending on the const-qualification of `self`.
   ///
   /// @tparam Self Deduced self type.
   ///
   /// @param self The vector object.
   /// @param position The index of the element to access.
   ///
   /// @throws std::out_of_range If `position >= size()`.
   ///
   /// @return A reference to the requested element, matching the const/value
   /// category of `self`.
   ///
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

   /// @brief Accesses the element at `position` without bounds checking.
   ///
   /// @tparam Self Deduced self type.
   ///
   /// @param self The vector object.
   /// @param position The index of the element to access.
   ///
   /// @pre `position < size()`.
   ///
   /// @return A reference to the requested element, matching the const/value
   /// category of `self`.
   ///
   /// @note `noexcept` outside of testing;
   ///
   template <typename Self>
   [[nodiscard]]
   constexpr decltype(auto)
   operator[](this Self &&self, size_type position)
       noexcept(swtl::config::nothrow_contracts)
   {
      // TODO: Move to `pre()` once it is supported for methods.
      //       Then you can drop the noexcept doc comment.
      contract_assert(position < self.size());
      return std::forward_like<Self>(*(self.m_start + position));
   }

   /// @brief Accesses the first element of the vector.
   ///
   /// @tparam Self Deduced self type.
   ///
   /// @param self The vector object.
   ///
   /// @pre The container must not be empty.
   ///
   /// @return A reference to the first element.
   ///
   /// @note `noexcept` outside of testing;
   ///
   template <typename Self>
   [[nodiscard]]
   constexpr decltype(auto)
   front(this Self &&self) noexcept(swtl::config::nothrow_contracts)
   {
      // TODO: Move to `pre()` once it is supported for methods.
      //       Then you can drop the noexcept doc comment.
      contract_assert(!self.is_empty());
      return std::forward_like<Self>(*self.m_start);
   }

   /// @brief Accesses the last element of the vector.
   ///
   /// @tparam Self Deduced self type.
   ///
   /// @param self The vector object.
   ///
   /// @pre The container must not be empty.
   ///
   /// @return A reference to the last element.
   ///
   /// @note `noexcept` outside of testing;
   ///
   ///
   template <typename Self>
   [[nodiscard]]
   constexpr decltype(auto)
   back(this Self &&self) noexcept(swtl::config::nothrow_contracts)
   {
      // TODO: Move to `pre()` once it is supported for methods.
      //       Then you can drop the noexcept doc comment.
      contract_assert(!self.is_empty());
      return std::forward_like<Self>(*(self.m_finish - 1));
   }

   /// @brief Returns a pointer to the underlying array.
   ///
   /// @tparam Self Deduced self type.
   ///
   /// @param self The vector object.
   ///
   /// @return A pointer to the first element, or null if the storage is
   /// unallocated.
   ///
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

   /// @}

   /// @name Capacity
   /// @{

   /// @brief Checks whether the container is empty.
   ///
   /// @return `true` if `size() == 0`, otherwise `false`.
   ///
   [[nodiscard]]
   constexpr bool
   is_empty() const noexcept
   {
      return this->m_start == this->m_finish;
   }

   /// @brief Returns the number of elements currently stored in the vector.
   ///
   /// @return The element count.
   ///
   [[nodiscard]]
   constexpr size_type
   size() const noexcept
   {
      return static_cast<size_type>(this->m_finish - this->m_start);
   }

   /// @brief Returns the maximum number of elements the vector is theoretically
   /// capable of holding.
   ///
   /// @return The maximum number of allocatable elements.
   ///
   [[nodiscard]]
   constexpr size_type
   max_size() const noexcept
   {
      return this->max_allocatable_elements();
   }

   /// @brief Ensures that the capacity is at least `new_capacity`.
   ///
   /// Reallocates storage and moves existing elements if `new_capacity >
   /// capacity()`. Does nothing if `new_capacity <= capacity()`.
   ///
   /// @param new_capacity The minimum desired capacity in elements.
   ///
   /// @throws std::length_error If `new_capacity > max_size()`.
   /// @throws (...) Any exception thrown by the allocator as a result of a call
   /// to `std::allocator_traits<Allocator>::allocate` if the vector is resized.
   /// @throws (...) Any exception thrown by T during element construction.
   ///
   /// @note Provides a strong exception guarantee: if an exception is thrown
   /// during reallocation, any new elements that were constructed prior to the
   /// exception are destroyed, any newly allocated memory is deallocated, and
   /// the vector is unmodified.
   ///
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

         // Scary part's over, destroy old elements and reassign the guard to
         // the old memory.
         clear();
         this->m_finish = new_finish;
         mem_guard.reassign(this->m_start, capacity());
      }

      this->m_start = ptr;
      this->m_end_of_storage = ptr + count;
   }

   /// @brief Returns the total number of elements that can be held without
   /// reallocation.
   ///
   /// @return The current allocated capacity in number of elements.
   ///
   [[nodiscard]]
   constexpr size_type
   capacity() const noexcept
   {
      return this->allocated_capacity();
   }

   /// @brief Requests reallocation to new memory in order to reduce
   /// `capacity()` to `size()`.
   ///
   /// Has no effect if `capacity()` is equal to `size()`, nor if the
   /// newly allocated chunk is greater than or equal in size to `capacity()`
   /// since `allocate_at_least()` is used for allocation.
   ///
   /// @throws (...) Any exception thrown by the allocator as a result of a call
   /// to `std::allocator_traits<Allocator>::allocate` if the vector is resized.
   /// @throws (...) Any exception thrown by T during element construction or
   /// assignment.
   ///
   /// @note Proveds a strong exception guarantee: if `T` is nothrow move
   /// constructible or copy constructible and an exception is thrown during
   /// reallocation, any new elements that were constructed prior to the
   /// exception are destroyed, any newly allocated memory is deallocated, and
   /// the vector is unmodified.
   ///
   /// @note Provides a basic exception guarantee: if `T` is not nothrow
   /// moveable or copyable and an exception is thrown during reallocation,
   /// the vector is left in a valid but unspecified state and no resources are
   /// leaked.
   ///
   constexpr void
   shrink_to_fit()
   {
      if (capacity() == size())
      {
         return;
      }

      if (is_empty())
      {
         this->deallocate_memory();
         return;
      }

      auto [ptr, count]{ this->allocate_memory_for_at_least(size()) };

      detail::AllocationGuard mem_guard(this->m_allocator, ptr, count);

      if (capacity() <= count)
      {
         return;
      }

      auto const new_finish{ uninitialized_move_if_noexcept(
          this->m_allocator, begin(), end(), ptr) };

      clear();
      mem_guard.reassign(this->m_start, capacity());

      this->m_start = ptr;
      this->m_finish = new_finish;
      this->m_end_of_storage = ptr + count;
   }

   /// @}

   /// @name Modifiers
   /// @{

   /// @brief Destroys all elements in the container, leaving `size() == 0`.
   ///
   /// @note Does not deallocate any allocated memory.
   ///
   constexpr void
   clear() noexcept
   {
      this->m_finish
          = destroy(this->m_allocator, this->m_start, this->m_finish);
   }

   /// @brief Inserts a copy of `value` at `pos`.
   ///
   /// Copy-inserts `value` into the vector at iterator `pos`; the existing
   /// element(s), if any, at and after `pos` are shifted one position to the
   /// right in the vector.
   ///
   /// @param pos Iterator to the position in the vector to insert `value`.
   /// @param value The value to be copy-inserted into the vector.
   ///
   /// @return An iterator to the inserted value.
   ///
   /// @pre `pos` must be an iterator to `*this`.
   ///
   /// @throws (...) Any exception thrown by the allocator as a result of a call
   /// to `std::allocator_traits<Allocator>::allocate` if the vector is resized.
   /// @throws (...) Any exception thrown by T during element construction or
   /// assignment.
   ///
   /// @note Provides a strong exception guarantee: if an exception is thrown by
   /// `value` construction at the end, the vector is unmodified.
   ///
   /// @note Provides a basic exception guarantee: if an exception is thrown
   /// during element assignment or construction, the vector is left in a valid
   /// but unspecified state and no resources are leaked.
   ///
   iterator
   insert(const_iterator pos, T const &value)
   {
      return emplace(pos, value);
   }

   /// @brief Moves `value` into the vector at `pos`.
   ///
   /// Move-inserts `value` into the vector at iterator `pos`; the existing
   /// element(s), if any, at and after `pos` are shifted one position to the
   /// right in the vector.
   ///
   /// @param pos Iterator to the position in the vector to insert `value`.
   /// @param value The value to be move-inserted into the vector.
   ///
   /// @return An iterator to the inserted value.
   ///
   /// @pre `pos` must be an iterator to `*this`.
   ///
   /// @throws (...) Any exception thrown by the allocator as a result of a call
   /// to `std::allocator_traits<Allocator>::allocate` if the vector is resized.
   /// @throws (...) Any exception thrown by T during element construction or
   /// assignment.
   ///
   /// @note Provides a strong exception guarantee: if an exception is thrown by
   /// `value` construction at the end, the vector is unmodified.
   ///
   /// @note Provides a basic exception guarantee: if an exception is thrown
   /// during element assignment or construction, the vector is left in a valid
   /// but unspecified state and no resources are leaked.
   ///
   iterator
   insert(const_iterator pos, T &&value)
   {
      return emplace(pos, std::move(value));
   }

   /// @brief Copy-inserts `count` copies of `value` into the vector before
   /// `pos`.
   ///
   /// Inserts `count` copies of `value` into the vector at iterator `pos`; the
   /// existing element(s), if any, at and after `pos` are shifted one position
   /// to the right in the vector.
   ///
   /// @param pos Iterator to the position in the vector to insert `value`.
   /// @param count The number of elements to construct.
   /// @param value The value to be copy-inserted into the vector.
   ///
   /// @return An iterator to the first inserted elements.
   ///
   /// @pre `pos` must be an iterator to `*this`.
   ///
   /// @throws (...) Any exception thrown by the allocator as a result of a call
   /// to `std::allocator_traits<Allocator>::allocate` if the vector is resized.
   /// @throws (...) Any exception thrown by T during element construction or
   /// assignment.
   ///
   /// @note Provides a strong exception guarantee: if an exception is thrown by
   /// `value` construction at the end, the vector is unmodified.
   ///
   /// @note Provides a basic exception guarantee: if an exception is thrown
   /// during element assignment or construction, the vector is left in a valid
   /// but unspecified state and no resources are leaked.
   ///
   iterator
   insert(const_iterator pos, size_type count, T const &value)
   {
      auto mut_pos{ begin() + std::ranges::distance(cbegin(), pos) };

      if (count == 0UZ)
      {
         return mut_pos;
      }

      if (capacity() - size() < count)
      {
         return realloc_insert(mut_pos, count, value);
      }

      if (pos == cend())
      {
         // Strong exception guarantee if pos == end().
         if constexpr (std::is_nothrow_copy_constructible_v<T>)
         {
            for (
                auto stop{ this->m_finish + count }; this->m_finish != stop;
                ++this->m_finish)
            {
               a_traits::construct(this->m_allocator, this->m_finish, value);
            }
         }
         else
         {
            detail::ElementGuard elem_guard(
                this->m_allocator, this->m_finish, this->m_finish);

            for (
                auto stop{ this->m_finish + count }; elem_guard.last != stop;
                ++elem_guard.last)
            {
               a_traits::construct(this->m_allocator, elem_guard.last, value);
            }

            this->m_finish = elem_guard.last;
            elem_guard.dismiss();
         }

         return mut_pos;
      }

      // Make a local copy because `value` may be a reference to an existing
      // element in the vector - so the new value must be constructed before
      // we shuffle the data around.
      auto local_value{ value };
      auto old_end{ end() };
      auto const elems_after_pos{ std::ranges::distance(pos, end()) };

      // If the inserted elements will only be copy-assigned, shuffle the
      // existing elements into new memory and update finish. This keeps
      // exception cleanup automatic.
      if (std::cmp_less_equal(count, elems_after_pos))
      {
         auto mid{ end() - count };

         this->m_finish = uninitialized_move_if_noexcept(
             this->m_allocator, mid, end(), this->m_finish);

         if constexpr (
             std::is_nothrow_move_assignable_v<T>
             || !std::is_copy_assignable_v<T>)
         {
            std::move_backward(mut_pos, mid, old_end);
         }
         else
         {
            std::copy_backward(mut_pos, mid, old_end);
         }

         std::ranges::fill(mut_pos, mid + 1, local_value);
      }
      // If some inserted elements need to be constructed in uninitialized
      // memory, do that first and update finish along the way.  Then shuffle
      // elements over and update finish again.  This keeps exception cleanup
      // automatic.
      else
      {
         auto mid{ this->m_finish + (count - elems_after_pos) };

         for (; this->m_finish != mid; ++this->m_finish)
         {
            a_traits::construct(this->m_allocator, this->m_finish, local_value);
         }

         this->m_finish = uninitialized_move_if_noexcept(
             this->m_allocator, mut_pos, old_end, this->m_finish);

         std::ranges::fill(mut_pos, old_end, local_value);
      }

      return mut_pos;
   }

   /// @brief Inserts elements from the range `[first, last)` at `pos`.
   ///
   /// Constructs elements from the range `[first, last)` in the vector at
   /// iterator `pos`; the existing element(s), if any, at and after `pos` are
   /// shifted one position to the right in the vector.
   ///
   /// @param pos Iterator to the position in the vector to insert `value`.
   /// @param first Iterator to the beginning of the range to insert.
   /// @param last Sentinel for `first`.
   ///
   /// @return An iterator to the first inserted element.
   ///
   /// @pre `pos` must be an iterator to `*this`.
   ///
   /// @throws (...) Any exception thrown by the allocator as a result of a
   /// call to `std::allocator_traits<Allocator>::allocate` if the vector is
   /// resized.
   /// @throws (...) Any exception thrown by T during element construction or
   /// assignment.
   ///
   /// @note Provides a strong exception guarantee: if an exception is thrown
   /// by construction at the end by an iterator pair that supports
   /// `sized_sentinel_for`, the vector is unmodified.
   ///
   /// @note Provides a basic exception guarantee: if an exception is thrown
   /// during element assignment or construction, the vector is left in a
   /// valid but unspecified state and no resources are leaked.
   ///
   template <typename InputIterator, std::sentinel_for<InputIterator> Sentinel>
   iterator
   insert(const_iterator pos, InputIterator first, Sentinel last)
   {
      // If distance can be checked then do so and reallocate or overwrite
      // elements as necessary.
      if constexpr (
          std::sized_sentinel_for<Sentinel, InputIterator>
          || std::forward_iterator<InputIterator>)
      {
         auto const count{ std::ranges::distance(first, last) };
         auto mut_pos{ begin() + std::ranges::distance(cbegin(), pos) };

         if (this->m_end_of_storage - this->m_finish < count)
         {
            return realloc_insert(mut_pos, first, last);
         }

         if (pos == cend())
         {
            this->m_finish = uninitialized_move_if_noexcept(
                this->m_allocator, first, last, this->m_finish);

            return mut_pos;
         }

         auto old_end{ end() };
         auto const elems_after_pos{ std::ranges::distance(pos, end()) };

         if (std::cmp_less_equal(count, elems_after_pos))
         {
            auto mid{ end() - count };

            this->m_finish = uninitialized_move_if_noexcept(
                this->m_allocator, mid, end(), this->m_finish);

            if constexpr (
                std::is_nothrow_move_assignable_v<T>
                || !std::is_copy_assignable_v<T>)
            {
               std::move_backward(mut_pos, mid, old_end);
            }
            else
            {
               std::copy_backward(mut_pos, mid, old_end);
            }

            zip_copy(first, last, mut_pos, mid + 1);
         }
         else
         {
            auto mid{ this->m_finish + (count - elems_after_pos) };

            // In moving existing elements out of the way of the new ones we're
            // leaving a gap of uninitialized memory between the existing
            // elements.  Guard this region in case of exception.
            detail::ElementGuard elem_guard(
                this->m_allocator,
                mid,
                uninitialized_move_if_noexcept(
                    this->m_allocator, mut_pos, old_end, mid));

            // Copy the first half into initialized memory.
            auto [next, _]{ zip_copy(first, last, mut_pos, old_end) };

            // Pick up where we left off and construct the rest in uninitialized
            // memory.
            uninitialized_copy(this->m_allocator, next, last, old_end);

            this->m_finish = elem_guard.last;
            elem_guard.dismiss();
         }

         return mut_pos;
      }
      // If we have no way to check the size of the input range, either create a
      // temporary vector with the data from the single pass iterator and call
      // insert again with that temporary range, or if `pos` is `end()` let
      // `push_back` handle it.
      else
      {
         if (pos == cend())
         {
            while (first != last)
            {
               push_back(*first);
               // Post-increment returns void for single pass iterators.
               ++first;
            }

            return begin() + std::ranges::distance(cbegin(), pos);
         }

         Vector temp(first, last, this->m_allocator);
         return insert(
             pos,
             std::make_move_iterator(temp.begin()),
             std::make_move_iterator(temp.end()));
      }
   }

   /// @brief Inserts the contents of the initializer list at `pos`.
   ///
   /// Constructs elements from the initializer list in the vector at iterator
   /// `pos`; the existing element(s), if any, at and after `pos` are shifted
   /// one position to the right in the vector.
   ///
   /// @param pos Iterator to the position in the vector to insert `value`.
   /// @param init_list The initializer list of elements to insert.
   ///
   /// @return An iterator to the first inserted element.
   ///
   /// @pre `pos` must be an iterator to `*this`.
   ///
   /// @throws (...) Any exception thrown by the allocator as a result of a
   /// call to `std::allocator_traits<Allocator>::allocate` if the vector is
   /// resized.
   /// @throws (...) Any exception thrown by T during element construction or
   /// assignment.
   ///
   /// @note Provides a strong exception guarantee: if an exception is thrown
   /// by construction at the end, the vector is unmodified.
   ///
   /// @note Provides a basic exception guarantee: if an exception is thrown
   /// during element assignment or construction, the vector is left in a
   /// valid but unspecified state and no resources are leaked.
   ///
   iterator
   insert(const_iterator pos, std::initializer_list<T> init_list)
   {
      return insert(pos, init_list.begin(), init_list.end());
   }

   /// @brief Inserts the contents of the range at `pos`.
   ///
   /// Constructs elements from the range in the vector at iterator `pos`; the
   /// existing element(s), if any, at and after `pos` are shifted one position
   /// to the right in the vector.
   ///
   /// @param pos Iterator to the position in the vector to insert `value`.
   /// @param range The range of elements to insert.
   ///
   /// @return An iterator to the inserted value.
   ///
   /// @pre `pos` must be an iterator to `*this`.
   ///
   /// @throws (...) Any exception thrown by the allocator as a result of a
   /// call to `std::allocator_traits<Allocator>::allocate` if the vector is
   /// resized.
   /// @throws (...) Any exception thrown by T during element construction or
   /// assignment.
   ///
   /// @note Provides a strong exception guarantee: if an exception is thrown
   /// by construction at the end, the vector is unmodified.
   ///
   /// @note Provides a basic exception guarantee: if an exception is thrown
   /// during element assignment or construction, the vector is left in a
   /// valid but unspecified state and no resources are leaked.
   ///
   template <container_compatible_range<T> Range>
   constexpr iterator
   insert_range(const_iterator pos, Range &&range)
   {
      return insert(pos, std::ranges::begin(range), std::ranges::end(range));
   }

   /// @brief Inserts a new element at `pos`.
   ///
   /// Constructs a new element in the vector at iterator `pos`; the existing
   /// element(s), if any, at and after `pos` are shifted one position to the
   /// right in the vector.
   ///
   /// @param pos Iterator to the position in the vector to insert `value`.
   /// @param args The arguments to forward to `T`'s constructor.
   ///
   /// @return An iterator to the first inserted element.
   ///
   /// @pre `pos` must be an iterator to `*this`.
   ///
   /// @throws (...) Any exception thrown by the allocator as a result of a
   /// call to `std::allocator_traits<Allocator>::allocate` if the vector is
   /// resized.
   /// @throws (...) Any exception thrown by T during element construction or
   /// assignment.
   ///
   /// @note Provides a strong exception guarantee: if an exception is thrown
   /// by construction at the end, the vector is unmodified.
   ///
   /// @note Provides a basic exception guarantee: if an exception is thrown
   /// during element assignment or construction, the vector is left in a
   /// valid but unspecified state and no resources are leaked.
   ///
   template <typename... Args>
   iterator
   emplace(const_iterator pos, Args &&...args)
   {
      if (this->m_finish == this->m_end_of_storage)
      {
         return realloc_emplace(pos, std::forward<Args>(args)...);
      }

      auto old_end{ end() };

      if (pos == old_end)
      {
         a_traits::construct(
             this->m_allocator, this->m_finish, std::forward<Args>(args)...);
         ++this->m_finish;  // Increment after successful construction.
         return old_end;
      }

      // Make a local copy because `value` may be a reference to an existing
      // element in the vector - so the new value must be constructed before
      // we shuffle the data around.
      T local_value{ std::forward<Args>(args)... };

      a_traits::construct(
          this->m_allocator, this->m_finish, std::move_if_noexcept(back()));
      ++this->m_finish;  // Increment after successful construction.

      // `pos` is a const_iterator, so it can't be used for insertion; have
      // to make a mutable iterator first and use it for the _backward
      // algorithms data shuffle, new element construction, and the return
      // value.
      auto mut_pos{ begin() + std::ranges::distance(cbegin(), pos) };

      // Even though the cppreference example shows `last` being
      // dereferenced, being a sentinel value it can't.  So if `first` is
      // const then it breaks move semantics, use only a mutable iterator for
      // the first argument to the _backwards algorithms.
      if constexpr (
          std::is_nothrow_move_assignable_v<T> || !std::is_copy_assignable_v<T>)
      {
         std::ranges::move_backward(mut_pos, old_end - 1, old_end);
         *mut_pos = std::move(local_value);
      }
      else
      {
         std::ranges::copy_backward(mut_pos, old_end - 1, old_end);
         *mut_pos = local_value;
      }

      return mut_pos;
   }

   // TODO: erase()

   /// @brief Appends a copy of `value` to the end of the container.
   ///
   /// @param value The value to copy into the new element.
   ///
   /// @throws (...) Any exception thrown by T during element construction.
   /// @throws (...) Any exception thrown by the allocator as a result of a
   /// call to `std::allocator_traits<Allocator>::allocate` if reallocation
   /// occurs.
   ///
   /// @note Provides a strong exception guarantee: if an exception is thrown
   /// during reallocation, any new elements that were constructed prior to
   /// the exception are destroyed, any newly allocated memory is
   /// deallocated, and the vector is unmodified.
   ///
   /// @warning If T is move-only and the move constructor of T is not
   /// `noexcept`, the vector is forced to use the throwing move constructor
   /// and the guarantee is reduced to a basic exception guarantee: if an
   /// exception is thrown during reallocation, all new elements that were
   /// constructed prior to the exception are destroyed, any newly allocated
   /// memory is deallocated and the vector is left in a valid but
   /// unspecified state.
   ///
   constexpr void
   push_back(T const &value)
   {
      emplace_back(value);
   }

   /// @brief Appends `value` to the end of the container via move semantics.
   ///
   /// @param value The value to move into the new element.
   ///
   /// @throws (...) Any exception thrown by T during element construction.
   /// @throws (...) Any exception thrown by the allocator as a result of a
   /// call to `std::allocator_traits<Allocator>::allocate` if reallocation
   /// occurs.
   ///
   /// @note Provides a strong exception guarantee: if an exception is thrown
   /// during reallocation, any new elements that were constructed prior to
   /// the exception are destroyed, any newly allocated memory is
   /// deallocated, and the vector is unmodified.
   ///
   /// @warning If T is move-only and the move constructor of T is not
   /// `noexcept`, the vector is forced to use the throwing move constructor
   /// and the guarantee is reduced to a basic exception guarantee: if an
   /// exception is thrown during reallocation, all new elements that were
   /// constructed prior to the exception are destroyed, any newly allocated
   /// memory is deallocated, and the vector as well as `value` are left in a
   /// valid but unspecified state.
   ///
   constexpr void
   push_back(T &&value)
   {
      emplace_back(std::move(value));
   }

   /// @brief Constructs a new element in-place at the end of the container.
   ///
   /// Reallocates storage if `size() == capacity()`.
   ///
   /// @tparam Args Parameter pack of constructor argument types for `T`.
   ///
   /// @param args Forwarded arguments to construct the element with.
   ///
   /// @return A reference to the newly constructed element.
   ///
   /// @throws (...) Any exception thrown by T during element construction.
   /// @throws (...) Any exception thrown by the allocator as a result of a
   /// call to `std::allocator_traits<Allocator>::allocate` if reallocation
   /// occurs.
   ///
   /// @note Provides a strong exception guarantee: if an exception is thrown
   /// during reallocation, any new elements that were constructed prior to
   /// the exception are destroyed, any newly allocated memory is
   /// deallocated, and the vector is unmodified.
   ///
   /// @warning If T is move-only and the move constructor of T is not
   /// `noexcept`, the vector is forced to use the throwing move constructor
   /// and the guarantee is reduced to a basic exception guarantee: if an
   /// exception is thrown during reallocation, all new elements that were
   /// constructed prior to the exception are destroyed, any newly allocated
   /// memory is deallocated and the vector is left in a valid but
   /// unspecified state.
   ///
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
            return realloc_emplace_back(std::forward<Args>(args)...);
         }
      }

      a_traits::construct(
          this->m_allocator, this->m_finish, std::forward<Args>(args)...);
      return *this->m_finish++;
   }

   // TODO: append_range()
   // TODO: pop_back()
   // TODO: resize()

   /// @brief Swaps the contents and storage of this container with `other`.
   ///
   /// Observes `propagate_on_container_swap` to determine if allocators
   /// should be swapped.
   ///
   /// @pre
   /// `std::allocator_traits<Allocator>::propagate_on_container_swap::value`
   /// is provided and is `std::true_type`, or the allocators must compare
   /// equal to one another.
   ///
   /// @param other The container to swap contents with.
   ///
   constexpr void
   swap(Vector &other) noexcept
   {
      if constexpr (a_traits::propagate_on_container_swap::value)
      {
         using std::swap;
         swap(this->m_allocator, other.m_allocator);
      }
      // If the allocators always compare equal there's no point in checking
      // the contract_assert.
      else if constexpr (!a_traits::is_always_equal::value)
      {
         contract_assert(
             this->m_allocator != other.m_allocator
             && "swap requires that allocators compare equal when "
                "`propagate_on_container_swap` is false");
      }

      std::swap(this->m_start, other.m_start);
      std::swap(this->m_finish, other.m_finish);
      std::swap(this->m_end_of_storage, other.m_end_of_storage);
   }

   /// @}

   /// @name Non-Member Functions
   /// @{

   /// @brief Swaps the contents of two vectors.
   ///
   /// @param lhs The first vector.
   /// @param rhs The second vector.
   ///
   constexpr friend void
   swap(Vector &lhs, Vector &rhs) noexcept
   {
      lhs.swap(rhs);
   }

   /// @brief Equality comparison operator.
   ///
   /// Compares sizes first, then delegates element-wise equality to
   /// `std::ranges::equal`.
   ///
   /// @param lhs The left-hand vector.
   /// @param rhs The right-hand vector.
   ///
   /// @return `true` if sizes and all elements compare equal, otherwise
   /// `false`.
   ///
   /// @throws noexcept If `noexcept(noexcept(std::declval<T>() ==
   /// std::declval<T>()))`
   ///
   [[nodiscard]]
   constexpr friend bool
   operator==(Vector const &lhs, Vector const &rhs)
       noexcept(noexcept(std::declval<T>() == std::declval<T>()))
      requires std::equality_comparable<T>
   {
      if (rhs.size() != lhs.size())
      {
         return false;
      }
      return std::ranges::equal(lhs, rhs);
   }

   /// @brief Three-way comparison operator.
   ///
   /// Performs lexicographical comparison of elements.
   ///
   /// @param lhs The left-hand vector.
   /// @param rhs The right-hand vector.
   ///
   /// @return The result of the three-way comparison.
   ///
   [[nodiscard]]
   constexpr friend auto
   operator<=>(Vector const &lhs, Vector const &rhs)
       noexcept(noexcept(std::declval<T>() <=> std::declval<T>()))
      requires std::three_way_comparable<T>
   {
      for (auto const &[left, right] : std::views::zip(lhs, rhs))
      {
         if (auto const comparison{ left <=> right }; comparison != 0)
         {
            return comparison;
         }
      }
      return lhs.size() <=> rhs.size();
   }

   /// @}

private:
   /// @cond INTERNAL_DOCUMENTATION

   /// @brief Calculates the new capacity required for reallocation during
   /// growth.
   ///
   /// @param target_growth The minimum number of additional elements
   /// required.
   ///
   /// @throws std::length_error If the growth request exceeds `max_size()`.
   ///
   /// @return The newly calculated capacity in elements.
   ///
   [[nodiscard]]
   constexpr size_type
   calculate_growth_size(size_type target_growth = 1UZ) const
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

   /// @brief Reallocates storage and constructs a new element at the end.
   ///
   /// The new element is constructed prior to moving or copying the old
   /// elements to new storage.
   ///
   /// @tparam Args Parameter pack of constructor argument types for `T`.
   ///
   /// @param args Forwarded arguments to construct the element with.
   ///
   /// @return A reference to the newly constructed element.
   ///
   /// @throws (...) Any exception thrown by the allocator as a result of a
   /// call to `std::allocator_traits<Allocator>::allocate`.
   /// @throws (...) Any exception thrown by T during element construction.
   ///
   /// @note Provides a strong exception guarantee: if an exception is thrown
   /// during reallocation, any new elements that were constructed prior to
   /// the exception are destroyed, any newly allocated memory is
   /// deallocated, and the vector is unmodified.
   ///
   /// @warning If T is move-only and the move constructor of T is not
   /// `noexcept`, the vector is forced to use the throwing move constructor
   /// and the guarantee is reduced to a basic exception guarantee: if an
   /// exception is thrown during reallocation, all new elements that were
   /// constructed prior to the exception are destroyed, any newly allocated
   /// memory is deallocated and the vector is left in a valid but
   /// unspecified state.
   ///
   template <typename... Args>
   constexpr reference
   realloc_emplace_back(Args &&...args)
   {
      auto [ptr, count]{ this->allocate_memory_for_at_least(
          calculate_growth_size()) };

      auto const new_element_ptr{ ptr + size() };
      auto const new_element_finish{ new_element_ptr + 1 };

      detail::AllocationGuard mem_guard{ this->m_allocator, ptr, count };

      a_traits::construct(
          this->m_allocator, new_element_ptr, std::forward<Args>(args)...);
      {
         // The newly inserted element exists and must be destroyed if an
         // exception is thrown.
         detail::ElementGuard elem_guard{ this->m_allocator,
                                          new_element_ptr,
                                          new_element_finish };

         uninitialized_move_if_noexcept(this->m_allocator, begin(), end(), ptr);

         // Scary part's over, reassign the guards to the old elements and
         // memory.
         elem_guard.reassign(this->m_start, this->m_finish);
         mem_guard.reassign(this->m_start, capacity());

         this->m_start = ptr;
         this->m_finish = new_element_finish;
         this->m_end_of_storage = ptr + count;
         return back();
      }
   }

   /// @brief Reallocates storage and constructs a new element at `pos`.
   ///
   /// The new element is constructed prior to moving or copying the old
   /// elements to new storage.
   ///
   /// @param pos Iterator to the position in the vector to insert `value`.
   /// @param args The arguments to forward to `T`'s constructor.
   ///
   /// @return An iterator to the inserted value.
   ///
   /// @pre `pos` must be an iterator to `*this`.
   ///
   /// @throws (...) Any exception thrown by the allocator as a result of a
   /// call to `std::allocator_traits<Allocator>::allocate`.
   /// @throws (...) Any exception thrown by T during element construction or
   /// assignment.
   ///
   /// @note Provides a strong exception guarantee: if an exception is thrown
   /// by `value` during construction, the vector is unmodified.
   ///
   /// @note Provides a basic exception guarantee: if an exception is thrown
   /// during element assignment or construction during reallocation, the
   /// vector is left in a valid but unspecified state and no resources are
   /// leaked.
   ///
   template <typename... Args>
   constexpr iterator
   realloc_emplace(const_iterator pos, Args &&...args)
   {
      auto [ptr, count]{ this->allocate_memory_for_at_least(
          calculate_growth_size()) };

      auto const distance{ std::ranges::distance(cbegin(), pos) };
      auto const new_element_ptr{ ptr + distance };

      {
         detail::AllocationGuard mem_guard{ this->m_allocator, ptr, count };

         a_traits::construct(
             this->m_allocator, new_element_ptr, std::forward<Args>(args)...);

         {
            // The newly inserted element exists and must be destroyed if an
            // exception is thrown.
            detail::ElementGuard elem_guard{ this->m_allocator,
                                             new_element_ptr,
                                             new_element_ptr + 1 };

            auto const expected{ uninitialized_move_if_noexcept(
                this->m_allocator, begin(), pos, ptr) };

            // Making sure we didn't overrun the buffer and clobber the
            // inserted value we wrote previously, nor stop short and leave
            // uninitialized memory in the middle.
            assert(expected == new_element_ptr);

            // The inserted element still exists at the end, but now the
            // elements before the insertion exist as well - guard those
            // during the final data shuffle.
            elem_guard.first = ptr;

            // `pos` is a const_iterator, so it's elements can't be moved
            // from; have to make a mutable iterator first and use it for the
            // `first` arguments of uninitialized_move.
            auto mut_pos{ begin() + distance };

            auto const new_finish{ uninitialized_move_if_noexcept(
                this->m_allocator, mut_pos, end(), new_element_ptr + 1) };

            // Scary part's over, reassign the guards to the old elements and
            // memory.
            elem_guard.reassign(this->m_start, this->m_finish);
            mem_guard.reassign(this->m_start, capacity());

            this->m_start = ptr;
            this->m_finish = new_finish;
            this->m_end_of_storage = ptr + count;
            return new_element_ptr;
         }
      }
   }

   /// @brief Copy-inserts `count` copies of `value` into new memory, then moves
   /// elements from old memory before and/or after the new elements based on
   /// `pos`.
   ///
   /// @param pos Mutable iterator to the position in the vector to insert
   /// `value`.
   /// @param count The number of elements to construct.
   /// @param value The value to be copy-inserted into the vector.
   ///
   /// @return An iterator to the first inserted element.
   ///
   /// @pre `pos` must be an iterator to `*this`.
   ///
   /// @throws (...) Any exception thrown by the allocator as a result of a call
   /// to `std::allocator_traits<Allocator>::allocate`.
   /// @throws (...) Any exception thrown by T during element construction.
   ///
   /// @note Provides a strong exception guarantee: if an exception is thrown as
   /// a result of allocation, or `T`'s copy constructor the vector is
   /// unmodified.
   ///
   /// @note Provides a basic exception guarantee: if an exception is thrown
   /// during element move assignment or construction, the vector is left in a
   /// valid but unspecified state and no resources are leaked.
   ///
   constexpr iterator
   realloc_insert(iterator pos, size_type count, T const &value)
   {
      auto [ptr, mem_count]{ this->allocate_memory_for_at_least(
          calculate_growth_size(size() + count)) };
      auto pos_ptr{ ptr + std::ranges::distance(cbegin(), pos) };

      if constexpr (
          std::is_nothrow_move_constructible_v<T>
          && std::is_nothrow_copy_constructible_v<T>)
      {
         auto tail_ptr{ uninitialized_fill_n(
             this->m_allocator, pos_ptr, count, value) };

         uninitialized_move_if_noexcept(this->m_allocator, begin(), pos, ptr);

         auto const new_finish{ uninitialized_move_if_noexcept(
             this->m_allocator, pos, end(), tail_ptr) };

         clear();
         this->deallocate_memory();

         this->m_start = ptr;
         this->m_finish = new_finish;
         this->m_end_of_storage = ptr + mem_count;
      }
      else
      {
         detail::AllocationGuard mem_guard(this->m_allocator, ptr, mem_count);
         {
            auto tail_ptr{ uninitialized_fill_n(
                this->m_allocator, pos_ptr, count, value) };

            // Guard elements that were previously constructed in case an
            // exception is thrown while migrating data from the first half of
            // the old memory.
            detail::ElementGuard elem_guard(
                this->m_allocator, pos_ptr, tail_ptr);

            uninitialized_move_if_noexcept(
                this->m_allocator, begin(), pos, ptr);

            // Expand the guard to include the first half of migrated data
            // in case an exception is thrown while migrating the last half of
            // data from the old memory.
            elem_guard.first = ptr;

            auto const new_finish{ uninitialized_move_if_noexcept(
                this->m_allocator, pos, end(), tail_ptr) };

            // Migration complete, use the guards to destroy and deallocate the
            // old memory.
            elem_guard.reassign(this->m_start, this->m_finish);
            mem_guard.reassign(this->m_start, capacity());

            this->m_start = ptr;
            this->m_finish = new_finish;
            this->m_end_of_storage = ptr + mem_count;
         }
      }

      return pos_ptr;
   }

   template <
       std::input_iterator InputIterator,
       std::sentinel_for<InputIterator> Sentinel
   >
   constexpr iterator
   realloc_insert(iterator pos, InputIterator first, Sentinel last)
   {
      auto const count{ std::ranges::distance(first, last) };
      auto [ptr, mem_count]{ this->allocate_memory_for_at_least(
          calculate_growth_size(size() + count)) };
      auto pos_ptr{ ptr + std::ranges::distance(cbegin(), pos) };

      if constexpr (
          std::is_nothrow_move_constructible_v<T>
          && std::is_nothrow_copy_constructible_v<T>)
      {
         auto tail_ptr{ uninitialized_copy(
             this->m_allocator, first, last, pos_ptr) };

         uninitialized_move_if_noexcept(this->m_allocator, begin(), pos, ptr);

         auto const new_finish{ uninitialized_move_if_noexcept(
             this->m_allocator, pos, end(), tail_ptr) };

         clear();
         this->deallocate_memory();

         this->m_start = ptr;
         this->m_finish = new_finish;
         this->m_end_of_storage = ptr + mem_count;
      }
      else
      {
         detail::AllocationGuard mem_guard(this->m_allocator, ptr, mem_count);
         {
            auto tail_ptr{ uninitialized_copy(
                this->m_allocator, first, last, pos_ptr) };

            // Guard elements that were previously constructed in case an
            // exception is thrown while migrating data from the first half of
            // the old memory.
            detail::ElementGuard elem_guard(
                this->m_allocator, pos_ptr, tail_ptr);

            uninitialized_move_if_noexcept(
                this->m_allocator, begin(), pos, ptr);

            // Expand the guard to include the first half of migrated data
            // in case an exception is thrown while migrating the last half of
            // data from the old memory.
            elem_guard.first = ptr;

            auto const new_finish{ uninitialized_move_if_noexcept(
                this->m_allocator, pos, end(), tail_ptr) };

            // Migration complete, use the guards to destroy and deallocate the
            // old memory.
            elem_guard.reassign(this->m_start, this->m_finish);
            mem_guard.reassign(this->m_start, capacity());

            this->m_start = ptr;
            this->m_finish = new_finish;
            this->m_end_of_storage = ptr + mem_count;
         }
      }

      return pos_ptr;
   }

   /// @endcond
};

/// @name Explicit Deduction Guides
/// @{

/// @brief Deduces the element type from an iterator/sentinel pair.
///
template <
    std::input_iterator InputIterator,
    std::sentinel_for<InputIterator> Sentinel
>
Vector(InputIterator, Sentinel) -> Vector<std::iter_value_t<InputIterator>>;

/// @brief Deduces the element type from a container-compatible range.
///
template <std::ranges::input_range Range>
Vector(std::from_range_t, Range &&)
    -> Vector<std::ranges::range_value_t<Range>>;

/// @}

}  // namespace swtl
