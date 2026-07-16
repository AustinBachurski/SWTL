export module swtl_test_helper_objects;

import std;

export namespace swtl_test_helpers
{

template <typename Derived>
struct LifetimeTracker
{
   static inline long long count{};

   LifetimeTracker()
   {
      ++count;
   }

   LifetimeTracker(LifetimeTracker const &other)
   {
      ++count;
   }

   LifetimeTracker(LifetimeTracker &&other)
   {
      ++count;
   }

   LifetimeTracker &
   operator=(LifetimeTracker const &other) = default;

   LifetimeTracker &
   operator=(LifetimeTracker &&other) = default;

   ~LifetimeTracker()
   {
      --count;
   }
};

template <typename Derived>
class ThrowingObject
{
public:
   ThrowingObject()
   {
      count_and_throw_if();
   }

   ThrowingObject(ThrowingObject const &other)
   {
      count_and_throw_if();
   }

   ThrowingObject(ThrowingObject &&other)
   {
      count_and_throw_if();
   }

   ThrowingObject &
   operator=(ThrowingObject const &other) = default;

   ThrowingObject &
   operator=(ThrowingObject &&other) = default;

   ~ThrowingObject() = default;

   void
   reset()
   {
      constructions = 0;
   }

private:
   static void
   count_and_throw_if()
   {
      if (++constructions == limit)
      {
         throw std::runtime_error("Oh noes, I throws!");
      }
   }

   static inline std::size_t constructions{};
   static inline std::size_t limit{ std::numeric_limits<std::size_t>::max() };
};

struct ThrowingConstructor
{
   ThrowingConstructor()
   {
      throw std::runtime_error("Oh noes, I throws!");
   };
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
