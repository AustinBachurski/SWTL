export module swtl.test.helpers:objects;

import std;

/// @cond INTERNAL_DOCUMENTATION

export namespace swtl::test_helpers
{

/// @brief CRTP base class for tracking lifetimes.
///
/// Tracks lifetime via a static `instances` variable, incremented in the
/// default constructor, copy constructor, and move constructor; decremented in
/// the destructor.  Assignment operators do nothing.
///
/// @tparam Derived The inheriting class.
///
template <typename Derived>
class LifetimeTracker
{
public:
   /// @brief Default constructor.  Increments `instances`.
   ///
   constexpr LifetimeTracker() noexcept
   {
      ++instances;
   }

   /// @brief Copy constructor.  Increments `instances`.
   ///
   constexpr LifetimeTracker(LifetimeTracker const &) noexcept
   {
      ++instances;
   }

   /// @brief Move constructor.  Increments `instances`.
   ///
   constexpr LifetimeTracker(LifetimeTracker &&) noexcept
   {
      ++instances;
   }

   /// @brief Copy assignment operator.
   ///
   /// @note Does not modify `instances`.
   ///
   constexpr LifetimeTracker &
   operator=(LifetimeTracker const &) = default;

   /// @brief Move assignment operator.
   ///
   /// @note Does not modify `instances`.
   ///
   constexpr LifetimeTracker &
   operator=(LifetimeTracker &&) = default;

   /// @brief Destructor. Decrements `instances`.
   ///
   constexpr ~LifetimeTracker()
   {
      --instances;
   }

   /// @brief Three-way comparison operator.
   ///
   /// @return The result of the three-way comparison.
   ///
   [[nodiscard]]
   constexpr auto
   operator<=>(LifetimeTracker const &other) const = default;

   /// @brief Returns `true` if `instances == zero`, otherwise `false`.
   ///
   /// @return `true` if `instances == zero`, otherwise `false`
   ///
   [[nodiscard]]
   static constexpr bool
   all_instances_destroyed() noexcept
   {
      return instances == 0LL;
   }

   /// @brief Returns the number of instances currently alive.
   ///
   /// @return Count of living instances.
   ///
   [[nodiscard]]
   static constexpr long long
   instances_alive() noexcept
   {
      return instances;
   }

   /// @brief Resets `instances` to zero.
   ///
   static constexpr void
   reset_lifetime_instance_count() noexcept
   {
      instances = 0LL;
   }

private:
   /// Static counter incremented when objects are constructed and
   /// decremented when they are destroyed.
   static inline long long instances{};
};

/// @brief CRTP base class for testing exception handling.
///
/// Increments `instances` each time an object is constructed.
///
/// @tparam Derived The inheriting class.
///
/// @throws std::runtime_error When `instances` == `limit` in a constructor.
///
template <typename Derived>
class ThrowingObject
{
public:
   /// @brief Default constructor.  Increments `instances` and throws if
   /// `instances == limit`.
   ///
   /// @throws std::runtime_error When `instances` == `limit`.
   ///
   constexpr ThrowingObject()
   {
      increment_and_throw_if_limit_reached();
   }

   /// @brief Copy constructor.  Increments `instances` and throws if `instances
   /// == limit`.
   ///
   /// @throws std::runtime_error When `instances` == `limit`.
   ///
   constexpr ThrowingObject([[maybe_unused]] ThrowingObject const &other)
   {
      increment_and_throw_if_limit_reached();
   }

   /// @brief Move constructor.  Increments `instances` and throws if `instances
   /// == limit`.
   ///
   /// @throws std::runtime_error When `instances` == `limit`.
   ///
   constexpr ThrowingObject([[maybe_unused]] ThrowingObject &&other)
   {
      increment_and_throw_if_limit_reached();
   }

   /// @brief Copy assignment operator.
   ///
   /// @note Does not modify `instances` nor throw.
   ///
   constexpr ThrowingObject &
   operator=([[maybe_unused]] ThrowingObject const &other) = default;

   /// @brief Move assignment operator.
   ///
   /// @note Does not modify `instances` nor throw.
   ///
   constexpr ThrowingObject &
   operator=([[maybe_unused]] ThrowingObject &&other) = default;

   /// @brief Destructor.
   ///
   /// @note Does not modify `instances`.
   ///
   constexpr ~ThrowingObject() = default;

   /// @brief Three-way comparison operator.
   ///
   /// @return The result of the three-way comparison.
   ///
   [[nodiscard]]
   constexpr auto
   operator<=>(ThrowingObject const &other) const = default;

   /// @brief Resets `instances` to zero.
   ///
   static constexpr void
   reset_throwing_instance_count() noexcept
   {
      instances = 0;
   }

   /// @brief Set the instance count where constructing that instance should
   /// throw.
   ///
   /// @param count The target value where construcing that instance will throw.
   ///
   static constexpr void
   throw_when_constructing_instance(std::size_t count) noexcept
   {
      limit = count;
   }

private:
   /// @brief Increments `instances` and then compares `instances == limit`, if
   /// they are equal throw a `std::runtime_error`; otherwise the object is
   /// constructed.
   ///
   /// @throws std::runtime_error If `++instances == limit`.
   ///
   /// @note Only throws if `instances` == `limit`, which allows test setup to
   /// happen without fear of triggering an exception in any realistic case.
   ///
   static void
   increment_and_throw_if_limit_reached()
   {
      // Use equality so that this doesn't trigger when reference objects are
      // constructed in consecutive test cases.
      if (++instances == limit)
      {
         throw std::runtime_error("Oh noes, I throws!");
      }
   }

   /// Static counter incremented when objects are constructed.
   static inline std::size_t instances{};

   /// Static limit that will trigger an exception if reached.
   static inline std::size_t limit{};
};

/// @brief Base class that enables unique ids for test objects.
///
struct UniqueID
{
   /// Id number for the instance.
   std::size_t id{};

   /// @brief Default constructor.
   ///
   constexpr UniqueID() = default;

   /// @brief Single argument constructor that sets `id`.
   ///
   /// @param identifier The number to use for `id`.
   ///
   constexpr explicit UniqueID(std::size_t identifier)
       : id{ identifier }
   {}

   /// @brief Three-way comparison operator.
   ///
   /// @return The result of the three-way comparison.
   ///
   [[nodiscard]]
   constexpr auto
   operator<=>(UniqueID const &other) const = default;
};

/// @brief Basic test object.
///
struct TestObject : public LifetimeTracker<TestObject>,
                    public ThrowingObject<TestObject>,
                    public UniqueID
{
   /// @brief Default constructor.
   ///
   constexpr TestObject() = default;

   /// @brief Single argument constructor that sets `id`.
   ///
   /// @param identifier The number to use for `id`.
   ///
   constexpr TestObject(std::size_t identifier)
       : UniqueID{ identifier }
   {}

   /// @brief Three-way comparison operator.
   ///
   /// @return The result of the three-way comparison.
   ///
   [[nodiscard]]
   constexpr auto
   operator<=>(TestObject const &other) const = default;
};

struct NoThrowTestObject : public LifetimeTracker<NoThrowTestObject>,
                           public UniqueID
{
   /// @brief Default constructor.
   ///
   constexpr NoThrowTestObject() = default;

   /// @brief Single argument constructor that sets `id`.
   ///
   /// @param identifier The number to use for `id`.
   ///
   constexpr NoThrowTestObject(std::size_t identifier)
       : UniqueID{ identifier }
   {}

   /// @brief Three-way comparison operator.
   ///
   /// @return The result of the three-way comparison.
   ///
   [[nodiscard]]
   constexpr auto
   operator<=>(NoThrowTestObject const &other) const = default;
};

struct MoveOnlyTestObject : public LifetimeTracker<MoveOnlyTestObject>,
                            public ThrowingObject<MoveOnlyTestObject>,
                            public UniqueID
{
   /// @brief Default constructor.
   ///
   constexpr MoveOnlyTestObject() = default;

   /// @brief Single argument constructor that sets `id`.
   ///
   /// @param identifier The number to use for `id`.
   ///
   constexpr MoveOnlyTestObject(std::size_t identifier)
       : UniqueID{ identifier }
   {}

   /// @brief Copy constructor.
   ///
   constexpr MoveOnlyTestObject(MoveOnlyTestObject const &other) = default;

   /// @brief Copy assignment operator.
   ///
   constexpr MoveOnlyTestObject &
   operator=(MoveOnlyTestObject const &other) = default;

   /// @brief Move constructor.
   ///
   constexpr MoveOnlyTestObject(MoveOnlyTestObject &&other)
       = delete ("Object is move only.");

   /// @brief Move assignment opertator.
   ///
   constexpr MoveOnlyTestObject &
   operator=(MoveOnlyTestObject &&other) = delete ("Object is move only.");

   /// @brief Destructor.
   ///
   constexpr ~MoveOnlyTestObject() = default;

   /// @brief Three-way comparison operator.
   ///
   /// @return The result of the three-way comparison.
   ///
   [[nodiscard]]
   constexpr auto
   operator<=>(MoveOnlyTestObject const &other) const = default;
};

struct CopyOnlyTestObject : public LifetimeTracker<CopyOnlyTestObject>,
                            public ThrowingObject<CopyOnlyTestObject>,
                            public UniqueID
{
   /// @brief Default constructor.
   ///
   constexpr CopyOnlyTestObject() = default;

   /// @brief Single argument constructor that sets `id`.
   ///
   /// @param identifier The number to use for `id`.
   ///
   constexpr CopyOnlyTestObject(std::size_t identifier)
       : UniqueID{ identifier }
   {}

   /// @brief Copy constructor.
   ///
   constexpr CopyOnlyTestObject(CopyOnlyTestObject const &other) = default;

   /// @brief Copy assignment operator.
   ///
   constexpr CopyOnlyTestObject &
   operator=(CopyOnlyTestObject const &other) = default;

   /// @brief Move constructor.
   ///
   constexpr CopyOnlyTestObject(CopyOnlyTestObject &&other)
       = delete ("Object is copy only.");

   /// @brief Move assignment operator.
   ///
   constexpr CopyOnlyTestObject &
   operator=(CopyOnlyTestObject &&other) = delete ("Object is copy only.");

   /// @brief Destructor.
   ///
   constexpr ~CopyOnlyTestObject() = default;

   /// @brief Three-way comparison operator.
   ///
   /// @return The result of the three-way comparison.
   ///
   [[nodiscard]]
   constexpr auto
   operator<=>(CopyOnlyTestObject const &other) const = default;
};

/// @brief Helper which resets the underlying lifetime and/or throwing counts.
///
/// @tparam T The class to reset the counts of.
///
template <typename T>
constexpr void
reset_instances_and_disable_throw() noexcept
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
