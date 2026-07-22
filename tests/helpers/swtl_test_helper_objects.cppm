export module swtl_test_helper_objects;

import std;

export namespace swtl_test_helpers
{

template <typename Derived>
class LifetimeTracker
{
public:
   constexpr LifetimeTracker() noexcept
   {
      ++count;
   }

   constexpr LifetimeTracker(
       [[maybe_unused]] LifetimeTracker const &other) noexcept
   {
      ++count;
   }

   constexpr LifetimeTracker([[maybe_unused]] LifetimeTracker &&other) noexcept
   {
      ++count;
   }

   constexpr LifetimeTracker &
   operator=([[maybe_unused]] LifetimeTracker const &other) = default;

   constexpr LifetimeTracker &
   operator=([[maybe_unused]] LifetimeTracker &&other) = default;

   constexpr ~LifetimeTracker()
   {
      --count;
   }

   constexpr auto
   operator<=>(LifetimeTracker const &other) const = default;

   [[nodiscard]]
   static constexpr bool
   all_instances_destroyed() noexcept
   {
      return count == 0LL;
   }

   static constexpr void
   reset_lifetime_instance_count() noexcept
   {
      count = 0LL;
   }

private:
   static inline long long count{};
};

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

template <typename T>
constexpr void
reset_instance_counts_of() noexcept
{
   T::reset_lifetime_instance_count();
   T::reset_throwing_instance_count();
}

struct TestObject : public LifetimeTracker<TestObject>,
                    public ThrowingObject<TestObject>
{
   std::size_t id{};

   constexpr TestObject() = default;

   constexpr TestObject(std::size_t identifier)
       : id{ identifier }
   {}

   constexpr auto
   operator<=>(TestObject const &other) const = default;
};

struct NoThrowTestObject : public LifetimeTracker<NoThrowTestObject>
{
   std::size_t id{};

   constexpr NoThrowTestObject() = default;

   constexpr NoThrowTestObject(std::size_t identifier)
       : id{ identifier }
   {}

   constexpr auto
   operator<=>(NoThrowTestObject const &other) const = default;
};

struct MoveOnlyTestObject : public LifetimeTracker<MoveOnlyTestObject>,
                            public ThrowingObject<MoveOnlyTestObject>
{
   std::size_t id{};

   constexpr MoveOnlyTestObject() = default;

   constexpr MoveOnlyTestObject(std::size_t identifier)
       : id{ identifier }
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
                            public ThrowingObject<CopyOnlyTestObject>
{
   std::size_t id{};

   constexpr CopyOnlyTestObject() = default;

   constexpr CopyOnlyTestObject(std::size_t identifier)
       : id{ identifier }
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

}  // namespace swtl_test_helpers
