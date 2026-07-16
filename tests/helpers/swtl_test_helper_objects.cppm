export module swtl_test_helper_objects;

import std;

export namespace swtl_test_helpers
{

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

}  // namespace swtl_test_helpers
