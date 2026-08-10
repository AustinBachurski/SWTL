export module swtl.test.helpers:objects;

import std;

/// @cond INTERNAL_DOCUMENTATION

export namespace swtl::test_helpers
{

/// @brief CRTP base class for tracking lifetimes.
///
/// Tracks lifetime via a static `count` variable, incremented in the default
/// constructor, copy constructor, and move constructor; decremented in the
/// destructor.  Assignment operators do nothing.
///
template <typename Derived>
class LifetimeTracker
{
public:
   /// @brief Default constructor.
   ///
   /// Increments `count`.
   ///
   constexpr LifetimeTracker() noexcept
   {
      ++count;
   }

   /// @brief Copy constructor.
   ///
   /// Increments `count`.
   ///
   constexpr LifetimeTracker(
       [[maybe_unused]] LifetimeTracker const &other) noexcept
   {
      ++count;
   }

   /// @brief Move constructor.
   ///
   /// Increments `count`.
   ///
   constexpr LifetimeTracker([[maybe_unused]] LifetimeTracker &&other) noexcept
   {
      ++count;
   }

   /// @brief Copy assignment operator.
   ///
   /// @note Does not modify `count`.
   ///
   constexpr LifetimeTracker &
   operator=([[maybe_unused]] LifetimeTracker const &other) = default;

   /// @brief Move assignment operator.
   ///
   /// @note Does not modify `count`.
   ///
   constexpr LifetimeTracker &
   operator=([[maybe_unused]] LifetimeTracker &&other) = default;

   /// @brief Destructor.
   ///
   /// Decrements `count`.
   ///
   constexpr ~LifetimeTracker()
   {
      --count;
   }

   /// @brief Three-way comparison operator.
   ///
   constexpr auto
   operator<=>(LifetimeTracker const &other) const = default;

   /// @brief Returns boolean `true` if `count == zero`, otherwise `false`.
   ///
   [[nodiscard]]
   static constexpr bool
   all_instances_destroyed() noexcept
   {
      return count == 0LL;
   }

   /// @brief Returns the integral count of instances alive, may be negative.
   ///
   [[nodiscard]]
   static constexpr long long
   instances_alive() noexcept
   {
      return count;
   }

   /// @brief Returns the `count` of living objects.
   ///
   static constexpr void
   reset_lifetime_instance_count() noexcept
   {
      count = 0LL;
   }

private:
   /// @brief Static `count` incremented when objects are constructed and
   /// decremented when they are destroyed.
   static inline long long count{};
};

/// @brief CRTP base class for testing exception handling.
///
/// // TODO: WORKING HERE
///
template <typename Derived>
class ThrowingObject
{
public:
   constexpr ThrowingObject()
   {
      count_and_throw_if();
   }

   constexpr ThrowingObject([[maybe_unused]] ThrowingObject const &other)
   {
      count_and_throw_if();
   }

   constexpr ThrowingObject([[maybe_unused]] ThrowingObject &&other)
   {
      count_and_throw_if();
   }

   constexpr ThrowingObject &
   operator=([[maybe_unused]] ThrowingObject const &other) = default;

   constexpr ThrowingObject &
   operator=([[maybe_unused]] ThrowingObject &&other) = default;

   constexpr ~ThrowingObject() = default;

   constexpr auto
   operator<=>(ThrowingObject const &other) const = default;

   static constexpr void
   reset_throwing_instance_count() noexcept
   {
      instances = 0;
   }

   static constexpr void
   throw_when_constructing_instance(std::size_t count) noexcept
   {
      limit = count;
   }

private:
   static void
   count_and_throw_if()
   {
      // Use equality so that this doesn't trigger when reference objects are
      // constructed in consecutive test cases.
      if (++instances == limit)
      {
         throw std::runtime_error("Oh noes, I throws!");
      }
   }

   static inline std::size_t instances{};
   // Defaulting to zero means that the initial objects can be constructed at
   // will during test setup, only once the limit is set will it throw.
   static inline std::size_t limit{};
};

struct UniqueID
{
   std::size_t id{};

   constexpr UniqueID() = default;

   constexpr UniqueID(std::size_t identifier)
       : id{ identifier }
   {}

   constexpr auto
   operator<=>(UniqueID const &other) const = default;
};

struct TestObject : public LifetimeTracker<TestObject>,
                    public ThrowingObject<TestObject>,
                    public UniqueID
{
   constexpr TestObject() = default;

   constexpr TestObject(std::size_t identifier)
       : UniqueID{ identifier }
   {}

   constexpr auto
   operator<=>(TestObject const &other) const = default;
};

struct NoThrowTestObject : public LifetimeTracker<NoThrowTestObject>,
                           public UniqueID
{
   constexpr NoThrowTestObject() = default;

   constexpr NoThrowTestObject(std::size_t identifier)
       : UniqueID{ identifier }
   {}

   constexpr auto
   operator<=>(NoThrowTestObject const &other) const = default;
};

struct MoveOnlyTestObject : public LifetimeTracker<MoveOnlyTestObject>,
                            public ThrowingObject<MoveOnlyTestObject>,
                            public UniqueID
{
   constexpr MoveOnlyTestObject() = default;

   constexpr MoveOnlyTestObject(std::size_t identifier)
       : UniqueID{ identifier }
   {}

   constexpr MoveOnlyTestObject(MoveOnlyTestObject const &other) = default;
   constexpr MoveOnlyTestObject &
   operator=(MoveOnlyTestObject const &other) = default;

   constexpr MoveOnlyTestObject(MoveOnlyTestObject &&other)
       = delete ("Object is move only.");
   constexpr MoveOnlyTestObject &
   operator=(MoveOnlyTestObject &&other) = delete ("Object is move only.");

   constexpr ~MoveOnlyTestObject() = default;

   constexpr auto
   operator<=>(MoveOnlyTestObject const &other) const = default;
};

struct CopyOnlyTestObject : public LifetimeTracker<CopyOnlyTestObject>,
                            public ThrowingObject<CopyOnlyTestObject>,
                            public UniqueID
{
   constexpr CopyOnlyTestObject() = default;

   constexpr CopyOnlyTestObject(std::size_t identifier)
       : UniqueID{ identifier }
   {}

   constexpr CopyOnlyTestObject(CopyOnlyTestObject const &other) = default;
   constexpr CopyOnlyTestObject &
   operator=(CopyOnlyTestObject const &other) = default;

   constexpr CopyOnlyTestObject(CopyOnlyTestObject &&other)
       = delete ("Object is copy only.");
   constexpr CopyOnlyTestObject &
   operator=(CopyOnlyTestObject &&other) = delete ("Object is copy only.");

   constexpr ~CopyOnlyTestObject() = default;

   constexpr auto
   operator<=>(CopyOnlyTestObject const &other) const = default;
};

template <typename T>
class TestInputIterator
{
public:
   // Iterator Traits
   using iterator_category = std::input_iterator_tag;
   using value_type = std::remove_cv_t<T>;
   using difference_type = std::ptrdiff_t;
   using pointer = T *;
   using reference = T &;

   constexpr TestInputIterator() = default;

   constexpr explicit TestInputIterator(pointer ptr)
       : ptr_{ ptr }
   {}

   [[nodiscard]]
   constexpr reference
   operator*() const noexcept
   {
      return *ptr_;
   }

   constexpr TestInputIterator &
   operator++() noexcept
   {
      ++ptr_;
      return *this;
   }

   constexpr TestInputIterator
   operator++(int) noexcept
   {
      auto temp{ *this };
      ++ptr_;
      return temp;
   }

   [[nodiscard]]
   constexpr friend auto
   operator<=>(
       TestInputIterator const &lhs, TestInputIterator const &rhs) noexcept
       = default;

private:
   pointer ptr_{};
};

// Ensures that the iterator meets the
// requirements for the appropriate iterator tag.
static_assert(std::input_or_output_iterator<TestInputIterator<int>>);

template <typename T>
constexpr void
reset_counts_and_set_nothrow() noexcept
{
   if constexpr (std::derived_from<T, LifetimeTracker<T>>)
   {
      T::reset_lifetime_instance_count();
   }

   if constexpr (std::derived_from<T, ThrowingObject<T>>)
   {
      T::reset_throwing_instance_count();
      T::throw_when_constructing_instance(0UZ);
   }
}

}  // namespace swtl::test_helpers

/// @endcond INTERNAL_DOCUMENTATION
