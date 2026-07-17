module;
#include <limits>
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
   // Default to max() so that reference objects can be constructed safetly.
   static inline std::size_t limit{ std::numeric_limits<std::size_t>::max() };
};

template <typename T>
constexpr void
reset_instance_counts_of() noexcept
{
   T::reset_lifetime_instance_count();
   T::reset_throwing_instance_count();
}

struct TrivialObject : ThrowingObject<TrivialObject>,
                       LifetimeTracker<TrivialObject>
{
   float x{};
   float y{};
   float z{};
};

struct ThrowingCopyConstructor
{
   int x{};
   float y{};
   std::string z{ "Enough text so that we heap allocate the data." };

   auto
   operator<=>(ThrowingCopyConstructor const &other) const = default;

   ThrowingCopyConstructor() = default;

   ThrowingCopyConstructor(ThrowingCopyConstructor const &other)
       : x{ other.x }
       , y{ other.y }
       , z{ other.z }
   {
      throw std::runtime_error("Oh noes, I throws!");
   }

   ThrowingCopyConstructor(ThrowingCopyConstructor &&other)
       = delete ("Testing throwing copy constructor, don't move me.");
};

struct ThrowingMoveConstructor
{
   int x{};
   float y{};
   std::string z{ "Enough text so that we heap allocate the data." };

   auto
   operator<=>(ThrowingMoveConstructor const &other) const = default;

   ThrowingMoveConstructor() = default;
   ThrowingMoveConstructor(ThrowingMoveConstructor const &other) = default;

   ThrowingMoveConstructor(ThrowingMoveConstructor &&other)
       : x{ other.x }
       , y{ other.y }
       , z{ other.z }
   {
      throw std::runtime_error("Oh noes, I throws!");
   }
};

struct ThrowingMoveOnlyObject
{
   int x{};
   float y{};
   std::string z{ "Enough text so that we heap allocate the data." };

   auto
   operator<=>(ThrowingMoveOnlyObject const &other) const = default;

   ThrowingMoveOnlyObject() = default;
   ThrowingMoveOnlyObject(ThrowingMoveOnlyObject const &other)
       = delete ("Object is move-only.");

   ThrowingMoveOnlyObject(ThrowingMoveOnlyObject &&other)
       : x{ other.x }
       , y{ other.y }
       , z{ std::move(other.z) }
   {
      throw std::runtime_error("Oh noes, I throws!");
   }
};

struct ThrowsAfterNCopies
{
   int *x_{};
   int &counter_;
   int limit_;

   ThrowsAfterNCopies(int &counter, int limit)
       : counter_{ counter }
       , limit_{ limit }
   {}

   ThrowsAfterNCopies(ThrowsAfterNCopies const &other)
       : counter_{ other.counter_ }
       , limit_{ other.limit_ }
   {
      if (counter_ == limit_)
      {
         throw std::runtime_error("Limit reached, do you leak?");
      }

      x_ = new int(42);
      ++counter_;
   }

   ~ThrowsAfterNCopies()
   {
      delete x_;
      --counter_;
   }
};

}  // namespace swtl_test_helpers
