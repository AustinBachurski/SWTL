#include "catch2/catch_template_test_macros.hpp"
#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers.hpp"
#include "catch2/matchers/catch_matchers_exception.hpp"
#include "catch2/matchers/catch_matchers_string.hpp"
#include <stdexcept>

import std;

import swtl.vector;
import swtl.iterators;

import swtl.test.helpers;

namespace helpers = swtl::test_helpers;

namespace Catch
{

// Allows Catch2 to print the id of any test object.
template <helpers::PrintableTestType T>
struct StringMaker<T>
{
   static std::string
   convert(T const &type)
   {
      return "{" + std::to_string(type.id) + "}";
   }
};

}  // namespace Catch

TEMPLATE_TEST_CASE(
    "Default construction creates an empty vector.",
    "[vector][constructor]",
    std::uint8_t,
    int,
    helpers::TrackedObject,
    helpers::NoThrowTrackedObject,
    helpers::CopyOnlyTrackedObject,
    helpers::MoveOnlyTrackedObject)
{
   helpers::g_test_controller.reset();

   swtl::Vector<TestType> vec;

   // Vector is empty.
   REQUIRE(vec.is_empty());
   REQUIRE(vec.size() == 0UZ);
   REQUIRE(vec.capacity() == 0UZ);
   REQUIRE(vec.data() == nullptr);

   // No elements were created.
   REQUIRE(helpers::g_test_controller.count_of.default_construction == 0UZ);
   REQUIRE(helpers::g_test_controller.count_of.arg_construction == 0UZ);
   REQUIRE(helpers::g_test_controller.count_of.copy_construction == 0UZ);
   REQUIRE(helpers::g_test_controller.count_of.copy_assignment == 0UZ);
   REQUIRE(helpers::g_test_controller.count_of.move_construction == 0UZ);
   REQUIRE(helpers::g_test_controller.count_of.move_assignment == 0UZ);
   REQUIRE(helpers::g_test_controller.count_of.destruction == 0UZ);
}

TEMPLATE_TEST_CASE(
    "Vector{braced-initializer-list} creates a vector with elements from the "
    "braced initializer list.",
    "[vector][constructor]",
    std::uint8_t,
    int,
    helpers::TrackedObject,
    helpers::NoThrowTrackedObject,
    helpers::CopyOnlyTrackedObject)
{
   using T = TestType;

   helpers::g_test_controller.reset();
   {
      auto const vec{ helpers::make_vec_of<T>(
          0UZ, 1UZ, 2UZ, 3UZ, 4UZ, 5UZ, 6UZ, 7UZ, 8UZ, 9UZ) };

      std::initializer_list<T> const init_list{ T{ 0UZ }, T{ 1UZ }, T{ 2UZ },
                                                T{ 3UZ }, T{ 4UZ }, T{ 5UZ },
                                                T{ 6UZ }, T{ 7UZ }, T{ 8UZ },
                                                T{ 9UZ } };
      REQUIRE(std::ranges::equal(vec, init_list));
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEST_CASE(
    "Vector{braced-initializer-list} exception safety.",
    "[vector][constructor][exception]")
{
   using T = helpers::TrackedObject;

   helpers::g_test_controller.reset();
   {
      helpers::g_test_controller.enable_throwing();
      helpers::g_test_controller.throw_when.copy_construction
          = helpers::g_test_controller.count_of.copy_construction + 10UZ;

      REQUIRE_THROWS(
          swtl::Vector<T>{ T{ 0 },
                           T{ 1 },
                           T{ 2 },
                           T{ 3 },
                           T{ 4 },
                           T{ 5 },
                           T{ 6 },
                           T{ 7 },
                           T{ 8 },
                           T{ 9 } });
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEMPLATE_TEST_CASE(
    "Vector(std::initializer_list init_list) creates a vector with elements "
    "from the initializer list.",
    "[vector][constructor]",
    std::uint8_t,
    int,
    helpers::TrackedObject,
    helpers::NoThrowTrackedObject,
    helpers::CopyOnlyTrackedObject)
{
   using T = TestType;

   helpers::g_test_controller.reset();
   {
      SECTION("Passing an empty initializer list results in an empty vector.")
      {
         std::initializer_list<T> init_list;

         swtl::Vector<T> const vec(init_list);

         REQUIRE(vec.is_empty());

         if constexpr (std::derived_from<T, helpers::TrackedObject>)
         {
            REQUIRE(helpers::g_test_controller.instances_alive() == 0UZ);
         }
      }

      SECTION(
          "Passing a populated initializer list results in a vector with "
          "elements.")
      {
         std::initializer_list<T> const init_list{ T{ 0 }, T{ 1 }, T{ 2 },
                                                   T{ 3 }, T{ 4 }, T{ 5 },
                                                   T{ 6 }, T{ 7 }, T{ 8 },
                                                   T{ 9 } };
         swtl::Vector<T> const vec(init_list);

         REQUIRE(std::ranges::equal(vec, init_list));
      }
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEST_CASE(
    "Vector(std::initializer_list init_list) exception safety.",
    "[vector][constructor][exception]")
{
   using T = helpers::TrackedObject;

   helpers::g_test_controller.reset();
   {
      std::initializer_list<T> const init_list{ T{ 0 }, T{ 1 }, T{ 2 }, T{ 3 },
                                                T{ 4 }, T{ 5 }, T{ 6 }, T{ 7 },
                                                T{ 8 }, T{ 9 } };

      helpers::g_test_controller.enable_throwing();
      helpers::g_test_controller.throw_when.copy_construction
          = helpers::g_test_controller.count_of.copy_construction
          + init_list.size();

      REQUIRE_THROWS_AS(swtl::Vector<T>(init_list), helpers::TestException);
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEMPLATE_TEST_CASE(
    "Vector(size_type count) creates a vector with count default constructed "
    "elements of type T.",
    "[vector][constructor]",
    std::uint8_t,
    int,
    helpers::TrackedObject,
    helpers::NoThrowTrackedObject,
    helpers::CopyOnlyTrackedObject,
    helpers::MoveOnlyTrackedObject)
{
   using T = TestType;

   helpers::g_test_controller.reset();
   {
      SECTION("A count of zero results in an empty vector.")
      {
         swtl::Vector<T> const vec(0UZ);

         REQUIRE(vec.is_empty());

         if constexpr (std::derived_from<T, helpers::TrackedObject>)
         {
            REQUIRE(helpers::g_test_controller.instances_alive() == 0UZ);
         }
      }

      SECTION("A count of non-zero results in a vector of count elements.")
      {
         auto count{ 10UZ };
         swtl::Vector<T> const empty_vec(0);

         swtl::Vector<T> expected;

         for (auto counter{ 0UZ }; counter != count; ++counter)
         {
            expected.emplace_back();
         }

         swtl::Vector<T> const vec(count);

         REQUIRE(empty_vec.is_empty());
         REQUIRE(vec == expected);
      }
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEST_CASE(
    "Vector(size_type count) exception safety.",
    "[vector][constructor][exception]")
{
   using T = helpers::TrackedObject;

   helpers::g_test_controller.reset();
   {
      auto const count{ 5UZ };

      helpers::g_test_controller.enable_throwing();
      helpers::g_test_controller.throw_when.default_construction = count;

      REQUIRE_THROWS_AS(swtl::Vector<T>(count), helpers::TestException);
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEMPLATE_TEST_CASE(
    "Vector(size_type count, T const &value) creates a vector with count "
    "elements of type T equal to value.",
    "[vector][constructor]",
    std::uint8_t,
    int,
    helpers::TrackedObject,
    helpers::NoThrowTrackedObject,
    helpers::CopyOnlyTrackedObject)
{
   using T = TestType;

   helpers::g_test_controller.reset();
   {
      SECTION("A count of zero results in an empty vector.")
      {
         swtl::Vector<T> const vec(0UZ, T{});

         REQUIRE(vec.is_empty());

         if constexpr (std::derived_from<T, helpers::TrackedObject>)
         {
            REQUIRE(helpers::g_test_controller.instances_alive() == 0UZ);
         }
      }

      SECTION("A count of non-zero results in a vector of count elements.")
      {
         auto const count{ 4UZ };
         auto const reference_value{ T{ 42UZ } };

         swtl::Vector<T> const vec(count, reference_value);
         swtl::Vector<T> expected;
         expected.assign(count, reference_value);

         REQUIRE(vec == expected);
      }
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEST_CASE(
    "Vector(size_type count, T const &value) exception safety.",
    "[vector][constructor][exception]")
{
   using T = helpers::TrackedObject;

   helpers::g_test_controller.reset();
   {
      auto const count{ 4UZ };
      auto const reference_value{ T{ 42UZ } };

      swtl::Vector<T> expected;
      expected.assign(count, reference_value);

      helpers::g_test_controller.enable_throwing();
      helpers::g_test_controller.throw_when.copy_construction
          = helpers::g_test_controller.count_of.copy_construction + count;

      REQUIRE_THROWS_AS(
          swtl::Vector<T>(count, reference_value), helpers::TestException);
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEMPLATE_TEST_CASE(
    "Vector(InputIterator first, Sentinel last) creates a vector with elements "
    "from the source container using a contiguous iterator.",
    "[vector][constructor]",
    std::uint8_t,
    int,
    helpers::TrackedObject,
    helpers::NoThrowTrackedObject,
    helpers::CopyOnlyTrackedObject,
    helpers::MoveOnlyTrackedObject)
{
   using T = TestType;

   helpers::g_test_controller.reset();
   {
      auto source{ helpers::generate_vector<T>() };

      SECTION("Passing a range of size zero results in an empty vector.")
      {
         if constexpr (!std::is_move_constructible_v<T>)
         {
            swtl::Vector const vec(source.begin(), source.begin());

            REQUIRE(vec.is_empty());

            if constexpr (std::derived_from<T, helpers::TrackedObject>)
            {
               REQUIRE(
                   helpers::g_test_controller.instances_alive()
                   == source.size());
            }
         }
         else
         {
            swtl::Vector const vec(
                std::make_move_iterator(source.begin()),
                std::make_move_iterator(source.begin()));

            REQUIRE(vec.is_empty());

            if constexpr (std::derived_from<T, helpers::TrackedObject>)
            {
               REQUIRE(
                   helpers::g_test_controller.instances_alive()
                   == source.size());
            }
         }
      }

      SECTION("Passing a range with elements results in a vector of elements.")
      {
         if constexpr (!std::is_move_constructible_v<T>)
         {
            swtl::Vector const vec(source.begin(), source.end());
            REQUIRE(vec == source);
         }
         else
         {
            swtl::Vector const vec(
                std::make_move_iterator(source.begin()),
                std::make_move_iterator(source.end()));
            REQUIRE(vec == source);
         }
      }
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEMPLATE_TEST_CASE(
    "Vector(InputIterator first, Sentinel last) exception saftey for "
    "contiguous iterators.",
    "[vector][constructor]",
    helpers::TrackedObject,
    helpers::CopyOnlyTrackedObject,
    helpers::MoveOnlyTrackedObject)
{
   using T = TestType;

   helpers::g_test_controller.reset();
   {
      auto source{ helpers::generate_vector<T>() };

      auto const count{ source.size() };

      helpers::g_test_controller.enable_throwing();
      helpers::g_test_controller.throw_when.copy_construction
          = helpers::g_test_controller.count_of.copy_construction + count;
      helpers::g_test_controller.throw_when.move_construction
          = helpers::g_test_controller.count_of.move_construction + count;

      if constexpr (!std::is_move_constructible_v<T>)
      {
         REQUIRE_THROWS_AS(
             swtl::Vector(source.begin(), source.end()),
             helpers::TestException);
      }
      else
      {
         REQUIRE_THROWS_AS(
             swtl::Vector(
                 std::make_move_iterator(source.begin()),
                 std::make_move_iterator(source.end())),
             helpers::TestException);
      }
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEMPLATE_TEST_CASE(
    "Vector(InputIterator first, Sentinel last) creates a vector with elements "
    "from the source container using an input iterator.",
    "[vector][constructor]",
    std::uint8_t,
    int,
    helpers::TrackedObject,
    helpers::NoThrowTrackedObject,
    helpers::CopyOnlyTrackedObject,
    helpers::MoveOnlyTrackedObject)
{
   using T = TestType;

   helpers::g_test_controller.reset();
   {
      auto source{ helpers::generate_vector<T>() };

      auto begin{ helpers::InputIterator(source.data()) };
      auto end{ helpers::InputIterator(source.data() + source.size()) };

      SECTION("Passing a range of size zero results in an empty vector.")
      {
         if constexpr (!std::is_move_constructible_v<T>)
         {
            swtl::Vector const vec(begin, begin);

            REQUIRE(vec.is_empty());

            if constexpr (std::derived_from<T, helpers::TrackedObject>)
            {
               REQUIRE(
                   helpers::g_test_controller.instances_alive()
                   == source.size());
            }
         }
         else
         {
            swtl::Vector const vec(
                std::make_move_iterator(begin), std::make_move_iterator(begin));

            REQUIRE(vec.is_empty());

            if constexpr (std::derived_from<T, helpers::TrackedObject>)
            {
               REQUIRE(
                   helpers::g_test_controller.instances_alive()
                   == source.size());
            }
         }
      }

      SECTION("Passing a range with elements results in a vector of elements.")
      {
         if constexpr (!std::is_move_constructible_v<T>)
         {
            swtl::Vector const vec(begin, end);
            REQUIRE(vec == source);
         }
         else
         {
            swtl::Vector const vec(
                std::make_move_iterator(begin), std::make_move_iterator(end));
            REQUIRE(vec == source);
         }
      }
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEMPLATE_TEST_CASE(
    "Vector(InputIterator first, Sentinel last) exception safety for input "
    "iterators.",
    "[vector][constructor][exception]",
    helpers::TrackedObject,
    helpers::CopyOnlyTrackedObject,
    helpers::MoveOnlyTrackedObject)
{
   using T = TestType;

   helpers::g_test_controller.reset();
   {
      auto source{ helpers::generate_vector<T>() };

      auto const count{ source.size() };

      auto begin{ helpers::InputIterator(source.data()) };
      auto end{ helpers::InputIterator(source.data() + source.size()) };

      helpers::g_test_controller.enable_throwing();
      helpers::g_test_controller.throw_when.copy_construction
          = helpers::g_test_controller.count_of.copy_construction + count;
      helpers::g_test_controller.throw_when.move_construction
          = helpers::g_test_controller.count_of.move_construction + count;

      if constexpr (!std::is_move_constructible_v<T>)
      {
         REQUIRE_THROWS_AS(swtl::Vector(begin, end), helpers::TestException);
      }
      else
      {
         REQUIRE_THROWS_AS(
             swtl::Vector(
                 std::make_move_iterator(begin), std::make_move_iterator(end)),
             helpers::TestException);
      }
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEMPLATE_TEST_CASE(
    "Vector(std::from_range, Range &&range) creates a vector with elements "
    "from the provided range.",
    "[vector][constructor][range]",
    std::uint8_t,
    int,
    helpers::TrackedObject,
    helpers::NoThrowTrackedObject,
    helpers::CopyOnlyTrackedObject,
    helpers::MoveOnlyTrackedObject)
{
   using T = TestType;

   helpers::g_test_controller.reset();
   {
      SECTION("Passing an empty range results in an empty vector.")
      {
         swtl::Vector<T> source;

         if constexpr (!std::is_move_constructible_v<T>)
         {
            swtl::Vector const vec(std::from_range, source);
            REQUIRE(vec.is_empty());

            if constexpr (std::derived_from<T, helpers::TrackedObject>)
            {
               REQUIRE(helpers::g_test_controller.instances_alive() == 0UZ);
            }
         }
         else
         {
            swtl::Vector const vec(
                std::from_range, std::views::as_rvalue(source));
            REQUIRE(vec.is_empty());

            if constexpr (std::derived_from<T, helpers::TrackedObject>)
            {
               REQUIRE(helpers::g_test_controller.instances_alive() == 0UZ);
            }
         }
      }

      SECTION("Passing a non-empty range results in a vector of elements.")
      {
         auto source{ helpers::generate_vector<T>() };

         if constexpr (!std::is_move_constructible_v<T>)
         {
            swtl::Vector const vec(std::from_range, source);
            REQUIRE(vec == source);
         }
         else
         {
            swtl::Vector const vec(
                std::from_range, std::views::as_rvalue(source));
            REQUIRE(vec == source);
         }
      }
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEMPLATE_TEST_CASE(
    "Vector(std::from_range, Range &&range) exception safety.",
    "[vector][constructor][range][exception]",
    helpers::TrackedObject,
    helpers::CopyOnlyTrackedObject,
    helpers::MoveOnlyTrackedObject)
{
   using T = TestType;

   helpers::g_test_controller.reset();
   {
      auto source{ helpers::generate_vector<T>() };

      auto const count{ source.size() };

      helpers::g_test_controller.enable_throwing();
      helpers::g_test_controller.throw_when.copy_construction
          = helpers::g_test_controller.count_of.copy_construction + count;
      helpers::g_test_controller.throw_when.move_construction
          = helpers::g_test_controller.count_of.move_construction + count;

      if constexpr (!std::is_move_constructible_v<T>)
      {
         REQUIRE_THROWS_AS(
             swtl::Vector(std::from_range, source), helpers::TestException);
      }
      else
      {
         REQUIRE_THROWS_AS(
             swtl::Vector(std::from_range, std::views::as_rvalue(source)),
             helpers::TestException);
      }
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEMPLATE_TEST_CASE(
    "CTAD correctly deduces types.",
    "[vector][constructor][ctad]",
    std::uint8_t,
    int,
    helpers::TrackedObject,
    helpers::NoThrowTrackedObject,
    helpers::CopyOnlyTrackedObject)
{
   using T = TestType;

   T value{};
   swtl::Vector<T> source;

   SECTION("CTAD from a braced initializer list.")
   {
      swtl::Vector vec{ value, value, value };

      STATIC_REQUIRE(std::is_same_v<decltype(vec), swtl::Vector<T>>);
   }

   SECTION("CTAD from iterators.")
   {
      swtl::Vector vec(source.begin(), source.end());

      STATIC_REQUIRE(std::is_same_v<decltype(vec), swtl::Vector<T>>);
   }

   SECTION("CTAD from a range.")
   {
      swtl::Vector vec(std::from_range, source);

      STATIC_REQUIRE(std::is_same_v<decltype(vec), swtl::Vector<T>>);
   }
}

TEMPLATE_TEST_CASE(
    "Vector iterator calls return const correct iterators.",
    "[vector][iterator]",
    std::uint8_t,
    int,
    helpers::TrackedObject,
    helpers::NoThrowTrackedObject,
    helpers::CopyOnlyTrackedObject,
    helpers::MoveOnlyTrackedObject)
{
   using T = TestType;

   auto vec{ helpers::generate_vector<T>() };
   auto const const_vec{ helpers::generate_vector<T>() };

   // Forward iterators.
   SECTION(
       "begin() returns non-const iterator from non-const container and a "
       "const iterator from a const container..")
   {
      STATIC_REQUIRE(
          std::is_same_v<decltype(vec.begin()), swtl::ContiguousIterator<T>>);
      STATIC_REQUIRE(
          std::is_same_v<
              decltype(const_vec.begin()),
              swtl::ContiguousIterator<T const>
          >);
   }

   SECTION(
       "end() returns non-const iterator from non-const container and a "
       "const iterator from a const container..")
   {
      STATIC_REQUIRE(
          std::is_same_v<decltype(vec.end()), swtl::ContiguousIterator<T>>);
      STATIC_REQUIRE(
          std::is_same_v<
              decltype(const_vec.end()),
              swtl::ContiguousIterator<T const>
          >);
   }

   SECTION("cbegin() returns a const iterator regardless of the container.")
   {
      STATIC_REQUIRE(
          std::is_same_v<
              decltype(vec.cbegin()),
              swtl::ContiguousIterator<T const>
          >);
      STATIC_REQUIRE(
          std::is_same_v<
              decltype(const_vec.cbegin()),
              swtl::ContiguousIterator<T const>
          >);
   }

   SECTION("cend() returns a const iterator regardless of the container.")
   {
      STATIC_REQUIRE(
          std::is_same_v<
              decltype(vec.cend()),
              swtl::ContiguousIterator<T const>
          >);
      STATIC_REQUIRE(
          std::is_same_v<
              decltype(const_vec.cend()),
              swtl::ContiguousIterator<T const>
          >);
   }

   // Reverse iterators.
   SECTION(
       "rbegin() returns non-const iterator from non-const container and a "
       "const iterator from a const container..")
   {
      STATIC_REQUIRE(
          std::is_same_v<
              decltype(vec.rbegin()),
              std::reverse_iterator<swtl::ContiguousIterator<T>>
          >);
      STATIC_REQUIRE(
          std::is_same_v<
              decltype(const_vec.rbegin()),
              std::reverse_iterator<swtl::ContiguousIterator<T const>>
          >);
   }

   SECTION(
       "rend() returns non-const iterator from non-const container and a "
       "const iterator from a const container..")
   {
      STATIC_REQUIRE(
          std::is_same_v<
              decltype(vec.rend()),
              std::reverse_iterator<swtl::ContiguousIterator<T>>
          >);
      STATIC_REQUIRE(
          std::is_same_v<
              decltype(const_vec.rend()),
              std::reverse_iterator<swtl::ContiguousIterator<T const>>
          >);
   }

   SECTION("crbegin() returns a const iterator regardless of the container.")
   {
      STATIC_REQUIRE(
          std::is_same_v<
              decltype(vec.crbegin()),
              std::reverse_iterator<swtl::ContiguousIterator<T const>>
          >);
      STATIC_REQUIRE(
          std::is_same_v<
              decltype(const_vec.crbegin()),
              std::reverse_iterator<swtl::ContiguousIterator<T const>>
          >);
   }

   SECTION("crend() returns a const iterator regardless of the container.")
   {
      STATIC_REQUIRE(
          std::is_same_v<
              decltype(vec.crend()),
              std::reverse_iterator<swtl::ContiguousIterator<T const>>
          >);
      STATIC_REQUIRE(
          std::is_same_v<
              decltype(const_vec.crend()),
              std::reverse_iterator<swtl::ContiguousIterator<T const>>
          >);
   }
}

TEST_CASE(
    "Iteration moves in the correct direction and returns const correct "
    "elements.",
    "[vector][iterator]")
{
   auto vec{ helpers::generate_vector<int>() };

   // References are used in these sections so that the actual return value of
   // the iterator can be tested, as opposed to the result of a copy.

   SECTION("Non-const forward iteration.")
   {
      auto previous_element{ std::numeric_limits<int>::lowest() };

      for (auto &current_element : vec)
      {
         REQUIRE(previous_element < current_element);
         STATIC_REQUIRE(!std::is_const_v<
                        std::remove_reference_t<decltype(current_element)>
         >);
         previous_element = current_element;
      }
   }

   SECTION("Const forward iteration.")
   {
      auto previous_element{ std::numeric_limits<int>::lowest() };

      for (auto &current_element : std::as_const(vec))
      {
         REQUIRE(previous_element < current_element);
         STATIC_REQUIRE(
             std::is_const_v<
                 std::remove_reference_t<decltype(current_element)>
             >);
         previous_element = current_element;
      }
   }

   SECTION("Non-const reverse iteration.")
   {
      auto previous_element{ std::numeric_limits<int>::max() };

      for (auto &current_element : vec | std::views::reverse)
      {
         REQUIRE(previous_element > current_element);
         STATIC_REQUIRE(!std::is_const_v<
                        std::remove_reference_t<decltype(current_element)>
         >);
         previous_element = current_element;
      }
   }

   SECTION("Const reverse iteration.")
   {
      auto previous_element{ std::numeric_limits<int>::max() };

      for (auto &current_element : std::as_const(vec) | std::views::reverse)
      {
         REQUIRE(previous_element > current_element);
         STATIC_REQUIRE(
             std::is_const_v<
                 std::remove_reference_t<decltype(current_element)>
             >);
         previous_element = current_element;
      }
   }
}

TEST_CASE("Non-const iterator mutability.", "[vector][iterator]")
{
   auto const vec{ helpers::generate_vector<int>() };

   SECTION("Non-const forward iterator is mutable.")
   {
      auto mutated_vec{ vec };

      for (auto &element : mutated_vec)
      {
         ++element;
      }

      REQUIRE(mutated_vec != vec);
   }

   SECTION("Non-const reverse iterator is mutable.")
   {
      auto mutated_vec{ vec };

      for (auto &element : mutated_vec | std::views::reverse)
      {
         ++element;
      }

      REQUIRE(mutated_vec != vec);
   }
}

TEMPLATE_TEST_CASE(
    "Special Member Functions: Copy operations",
    "[vector][special member functions]",
    std::uint8_t,
    int,
    helpers::TrackedObject,
    helpers::NoThrowTrackedObject,
    helpers::CopyOnlyTrackedObject)
{
   using T = TestType;

   helpers::g_test_controller.reset();
   {
      auto source{ helpers::generate_vector<T>() };
      auto const const_source{ helpers::generate_vector<T>() };
      auto const expected{ helpers::generate_vector<T>() };

      SECTION("Copying from an empty source results in an empty vector.")
      {
         swtl::Vector<T> empty_source;

         auto const vec{ empty_source };

         REQUIRE(vec.is_empty());
      }

      SECTION("Copy constructor from copies from non-const source.")
      {
         auto const destination{ source };

         REQUIRE(destination == expected);
         REQUIRE(source == expected);
         REQUIRE(destination.data() != source.data());
      }

      SECTION("Copy constructor from copies from const source.")
      {
         auto const destination{ const_source };

         REQUIRE(destination == expected);
         REQUIRE(destination.data() != const_source.data());
      }

      SECTION("Copy assignment operator copies from non-const source.")
      {
         swtl::Vector<T> destination;
         destination = source;

         REQUIRE(destination == expected);
         REQUIRE(source == expected);
         REQUIRE(destination.data() != source.data());
      }

      SECTION("Copy assignment operator copies from const source.")
      {
         swtl::Vector<T> destination;
         destination = const_source;

         REQUIRE(destination == expected);
         REQUIRE(destination.data() != const_source.data());
      }

      SECTION("Self copy assignment does nothing.")
      {
         auto const &reference_to_source{ source };

         auto const data_ptr_before_copy{ source.data() };
         auto const capacity_before_copy{ source.capacity() };
         auto const size_before_copy{ source.size() };

         source = reference_to_source;

         REQUIRE(source.data() == data_ptr_before_copy);
         REQUIRE(source.capacity() == capacity_before_copy);
         REQUIRE(source.size() == size_before_copy);
      }
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEMPLATE_TEST_CASE(
    "Special Member Functions: Copy operations - exception saftey.",
    "[vector][special member functions][exception]",
    helpers::TrackedObject,
    helpers::CopyOnlyTrackedObject)
{
   using T = TestType;

   helpers::g_test_controller.reset();
   {
      auto source{ helpers::generate_vector<T>() };
      auto const count{ source.size() };

      auto const expected{ helpers::generate_vector<T>() };

      helpers::g_test_controller.enable_throwing();

      SECTION("Copy constructor.")
      {
         helpers::g_test_controller.throw_when.copy_construction
             = helpers::g_test_controller.count_of.copy_construction + count;

         REQUIRE_THROWS_AS(swtl::Vector(source), helpers::TestException);
         REQUIRE(source == expected);
      }

      SECTION("Copy assignment operator.")
      {
         // Although this is copy assignment for the vector, the elements are
         // still being copy constructed in the empty destination.
         helpers::g_test_controller.throw_when.copy_construction
             = helpers::g_test_controller.count_of.copy_construction + count;

         swtl::Vector<T> destination;

         REQUIRE_THROWS_AS(destination = source, helpers::TestException);
         REQUIRE(source == expected);
      }
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEMPLATE_TEST_CASE(
    "Special Member Functions: Move Operations",
    "[vector][special member functions]",
    std::uint8_t,
    int,
    helpers::TrackedObject,
    helpers::NoThrowTrackedObject,
    helpers::CopyOnlyTrackedObject,
    helpers::MoveOnlyTrackedObject)
{
   using T = TestType;

   helpers::g_test_controller.reset();
   {
      auto source{ helpers::generate_vector<T>() };
      auto const &const_reference_to_source{ source };

      auto const data_ptr_before_move{ source.data() };
      auto const capacity_before_move{ source.capacity() };
      auto const size_before_move{ source.size() };

      auto const expected{ helpers::generate_vector<T>() };

      if constexpr (std::is_move_constructible_v<T>)
      {
         SECTION("Move constructor from non-const source moves data.")
         {
            auto const destination{ std::move(source) };

            REQUIRE(destination == expected);
            REQUIRE(destination != source);
            REQUIRE(destination.data() == data_ptr_before_move);
            REQUIRE(destination.capacity() == capacity_before_move);
            REQUIRE(destination.size() == size_before_move);

            REQUIRE(source.data() == nullptr);
            REQUIRE(source.size() == 0UZ);
            REQUIRE(source.capacity() == 0UZ);
         }
      }

      if constexpr (std::is_move_assignable_v<T>)
      {
         SECTION("Move assignment from non-const source moves data.")
         {
            swtl::Vector<T> destination;
            destination = std::move(source);

            REQUIRE(destination == expected);
            REQUIRE(destination != source);
            REQUIRE(destination.data() == data_ptr_before_move);
            REQUIRE(destination.capacity() == capacity_before_move);
            REQUIRE(destination.size() == size_before_move);

            REQUIRE(source.data() == nullptr);
            REQUIRE(source.size() == 0UZ);
            REQUIRE(source.capacity() == 0UZ);
         }
      }

      if constexpr (std::is_copy_constructible_v<T>)
      {
         SECTION("Move constructor from const source falls back to copying.")
         {
            auto destination{ std::move(const_reference_to_source) };

            REQUIRE(destination == expected);
            REQUIRE(destination == source);
            REQUIRE(destination.data() != data_ptr_before_move);
            // Capacity may be different due to use of allocate_at_least().
            REQUIRE(destination.size() == size_before_move);

            REQUIRE(source.data() != nullptr);
            REQUIRE(source.size() != 0UZ);
            REQUIRE(source.capacity() != 0UZ);
         }
      }

      if constexpr (std::is_copy_assignable_v<T>)
      {
         SECTION("Move assignment from const source falls back to copying.")
         {
            swtl::Vector<T> destination;
            destination = std::move(const_reference_to_source);

            REQUIRE(destination == expected);
            REQUIRE(destination == source);
            REQUIRE(destination.data() != data_ptr_before_move);
            // Capacity may be different due to use of allocate_at_least().
            REQUIRE(destination.size() == size_before_move);

            REQUIRE(source.data() != nullptr);
            REQUIRE(source.size() != 0UZ);
            REQUIRE(source.capacity() != 0UZ);
         }
      }

      if constexpr (std::is_move_assignable_v<T>)
      {
         SECTION("Self move assignment does nothing.")
         {
            auto &ref_to_self{ source };  // To bypass -Wself-move.

            source = std::move(ref_to_self);

            REQUIRE(source == expected);
            REQUIRE(source.data() == data_ptr_before_move);
            REQUIRE(source.capacity() == capacity_before_move);
            REQUIRE(source.size() == size_before_move);
         }
      }
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

// Move construction is always noexcept.
// Move assignment may throw, but we need to use a different allocator.
// TODO: Implement an allocator that will allow an exception in the move
// assignment operator and test accordingly.

TEMPLATE_TEST_CASE(
    "Vector::assign(size_type count, T const &value) updates the vector with "
    "count copies of value.",
    "[vector][assign]",
    std::uint8_t,
    int,
    helpers::TrackedObject,
    helpers::NoThrowTrackedObject,
    helpers::CopyOnlyTrackedObject)
{
   using T = TestType;

   helpers::g_test_controller.reset();
   {
      auto vec{ helpers::generate_vector<T>() };
      vec.reserve(10UZ);

      SECTION("Count of zero clears the vector.")
      {
         vec.assign(0UZ, T{});

         REQUIRE(vec.is_empty());
         REQUIRE(helpers::g_test_controller.all_instances_destroyed());
      }

      SECTION("Assignment triggers reallocation.")
      {
         auto const count{ 20UZ };

         INFO(
             "If count (which was "
             << count
             << " ) is less than or equal to vec.capacity() (which was "
             << vec.capacity() << " ) this test is invalid.");
         REQUIRE(count > vec.capacity());

         swtl::Vector<T> const expected(count);

         vec.assign(count, T{});

         REQUIRE(vec == expected);
      }

      SECTION("Assignment to existing memory.")
      {
         auto const count{ vec.capacity() };

         swtl::Vector<T> const expected(count);

         vec.assign(count, T{});

         REQUIRE(vec == expected);
      }
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEMPLATE_TEST_CASE(
    "Vector::assign(size_type count, T const &value) exception saftey.",
    "[vector][assign][exception]",
    helpers::TrackedObject,
    helpers::CopyOnlyTrackedObject)
{
   using T = TestType;

   helpers::g_test_controller.reset();
   {
      auto const initial_count{ 8UZ };
      swtl::Vector<T> vec(initial_count);

      helpers::g_test_controller.enable_throwing();
      SECTION("Assignment triggers reallocation.")
      {
         auto const count{ 10UZ };

         swtl::Vector<T> const expected(initial_count);

         helpers::g_test_controller.throw_when.copy_construction
             = helpers::g_test_controller.count_of.copy_construction + count;

         INFO(
             "If count (which was "
             << count << " ) is less than initial_count (which was "
             << initial_count << " ) this test is invalid.");

         REQUIRE_THROWS_AS(vec.assign(count, T{}), helpers::TestException);

         REQUIRE(vec == expected);
      }

      SECTION("Assignment to existing memory.")
      {
         auto const count{ 6UZ };

         helpers::g_test_controller.throw_when.copy_assignment
             = helpers::g_test_controller.count_of.copy_assignment + count;

         INFO(
             "If count (which was "
             << count
             << " ) is greater than or equal to initial_count (which was "
             << initial_count << " ) this test is invalid.");

         REQUIRE_THROWS_AS(vec.assign(count, T{}), helpers::TestException);
      }
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEMPLATE_TEST_CASE(
    "Vector::assign(size_type count, T const &value) exception saftey - "
    "exhaustive test.",
    "[vector][modifiers][assign][exception][exhaustive]",
    helpers::TrackedObject,
    helpers::CopyOnlyTrackedObject)
{
   using T = TestType;

   auto const count{ 10UZ };

   bool clean_pass{ false };

   for (auto ops_count{ 1UZ }; !clean_pass; ++ops_count)
   {
      helpers::g_test_controller.reset();
      {
         swtl::Vector vec{ helpers::generate_vector<T>() };

         helpers::g_test_controller.enable_throwing();
         helpers::g_test_controller.throw_when.total_operations
             = helpers::g_test_controller.count_of.total_operations + ops_count;

         try
         {
            vec.assign(count, T{});
            clean_pass = true;
            helpers::g_test_controller.disable_throwing();
         }
         catch (helpers::TestException const &)
         {
            REQUIRE(vec.size() <= vec.capacity());
            REQUIRE(helpers::g_test_controller.instances_alive() == vec.size());
         }
      }
      REQUIRE(helpers::g_test_controller.all_instances_destroyed());
   }
}

TEMPLATE_TEST_CASE(
    "Vector::assign(InputIterator src_begin, Sentinel src_end) assigns from a "
    "contiguous iterator.",
    "[vector][assign]",
    std::uint8_t,
    int,
    helpers::TrackedObject,
    helpers::NoThrowTrackedObject,
    helpers::CopyOnlyTrackedObject,
    helpers::MoveOnlyTrackedObject)
{
   using T = TestType;

   helpers::g_test_controller.reset();
   {
      auto const initial_size{ 10UZ };
      auto const source_size{ initial_size * 2 };

      swtl::Vector<T> vec(initial_size);
      auto source{ helpers::generate_vector<T>(source_size) };

      SECTION("Passing a zero sized range results in an empty vector.")
      {
         if constexpr (!std::is_move_constructible_v<T>)
         {
            vec.assign(source.begin(), source.begin());
         }
         else
         {
            vec.assign(
                std::make_move_iterator(source.begin()),
                std::make_move_iterator(source.begin()));
         }

         REQUIRE(vec.is_empty());

         if constexpr (std::derived_from<T, helpers::TrackedObject>)
         {
            REQUIRE(
                helpers::g_test_controller.instances_alive() == source.size());
         }
      }

      SECTION("Assignment triggers reallocation.")
      {
         auto const expected{ helpers::generate_vector<T>(source_size) };

         if constexpr (!std::is_move_constructible_v<T>)
         {
            vec.assign(source.begin(), source.end());
         }
         else
         {
            vec.assign(
                std::make_move_iterator(source.begin()),
                std::make_move_iterator(source.end()));
         }

         REQUIRE(vec == expected);
      }

      SECTION("Assignment to some existing elements.")
      {
         auto const count{ initial_size / 2 };

         auto const expected{ helpers::generate_vector<T>(count) };

         if constexpr (!std::is_move_assignable_v<T>)
         {
            vec.assign(source.begin(), source.begin() + count);
         }
         else
         {
            vec.assign(
                std::make_move_iterator(source.begin()),
                std::make_move_iterator(source.begin() + count));
         }

         REQUIRE(vec == expected);

         if constexpr (std::derived_from<T, helpers::TrackedObject>)
         {
            // Ensures that existing elements past the new elements are
            // destroyed following assignment.
            REQUIRE(
                helpers::g_test_controller.instances_alive()
                == vec.size() + expected.size() + source.size());
         }
      }

      SECTION("Assignment to existing memory.")
      {
         vec.reserve(source_size);

         auto const expected{ helpers::generate_vector<T>(source_size) };

         if constexpr (
             !std::is_move_assignable_v<T> && !std::is_move_constructible_v<T>)
         {
            vec.assign(source.begin(), source.end());
         }
         else
         {
            vec.assign(
                std::make_move_iterator(source.begin()),
                std::make_move_iterator(source.end()));
         }

         REQUIRE(vec == expected);
      }
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEMPLATE_TEST_CASE(
    "Vector::assign(InputIterator src_begin, Sentinel src_end) exception "
    "safety for contiguous iterators - exhaustive test.",
    "[vector][assign][exception][exhaustive]",
    helpers::TrackedObject,
    helpers::CopyOnlyTrackedObject,
    helpers::MoveOnlyTrackedObject)
{
   using T = TestType;

   SECTION("Source is equal in size to dest.")
   {
      auto const size{ 20UZ };

      bool clean_pass{ false };

      for (auto ops_count{ 1UZ }; !clean_pass; ++ops_count)
      {
         helpers::g_test_controller.reset();
         {
            swtl::Vector vec{ helpers::generate_vector<T>(size) };
            swtl::Vector source{ helpers::generate_vector<T>(size) };

            helpers::g_test_controller.enable_throwing();
            helpers::g_test_controller.throw_when.total_operations
                = helpers::g_test_controller.count_of.total_operations
                + ops_count;

            try
            {
               if constexpr (!std::is_move_constructible_v<T>)
               {
                  vec.assign(source.begin(), source.end());
               }
               else
               {
                  vec.assign(
                      std::make_move_iterator(source.begin()),
                      std::make_move_iterator(source.end()));
               }

               clean_pass = true;
               helpers::g_test_controller.disable_throwing();
            }
            catch (helpers::TestException const &)
            {
               REQUIRE(vec.size() <= vec.capacity());
               REQUIRE(
                   helpers::g_test_controller.instances_alive()
                   == vec.size() + source.size());
            }
         }
         REQUIRE(helpers::g_test_controller.all_instances_destroyed());
      }
   }

   SECTION("Source is smaller than dest.")
   {
      auto const source_size{ 5UZ };

      bool clean_pass{ false };

      for (auto ops_count{ 1UZ }; !clean_pass; ++ops_count)
      {
         helpers::g_test_controller.reset();
         {
            swtl::Vector vec{ helpers::generate_vector<T>(10UZ) };
            swtl::Vector source{ helpers::generate_vector<T>(source_size) };

            INFO(
                "If source_size (which was "
                << source_size << " ) is greater than vec.size() (which was "
                << vec.size() << " ) this test is invalid.");
            REQUIRE(source_size < vec.size());

            helpers::g_test_controller.enable_throwing();
            helpers::g_test_controller.throw_when.total_operations
                = helpers::g_test_controller.count_of.total_operations
                + ops_count;

            try
            {
               if constexpr (!std::is_move_constructible_v<T>)
               {
                  vec.assign(source.begin(), source.end());
               }
               else
               {
                  vec.assign(
                      std::make_move_iterator(source.begin()),
                      std::make_move_iterator(source.end()));
               }

               clean_pass = true;
               helpers::g_test_controller.disable_throwing();
            }
            catch (helpers::TestException const &)
            {
               REQUIRE(vec.size() <= vec.capacity());
               REQUIRE(
                   helpers::g_test_controller.instances_alive()
                   == vec.size() + source.size());
            }
         }
         REQUIRE(helpers::g_test_controller.all_instances_destroyed());
      }
   }

   SECTION("Source is larger than dest.")
   {
      auto const source_size{ 20UZ };

      bool clean_pass{ false };

      for (auto ops_count{ 1UZ }; !clean_pass; ++ops_count)
      {
         helpers::g_test_controller.reset();
         {
            swtl::Vector vec{ helpers::generate_vector<T>(10UZ) };
            swtl::Vector source{ helpers::generate_vector<T>(source_size) };

            INFO(
                "If source_size (which was "
                << source_size
                << " ) is less than or equal to vec.size() (which was "
                << vec.size() << " ) this test is invalid.");
            REQUIRE(source_size > vec.size());

            helpers::g_test_controller.enable_throwing();
            helpers::g_test_controller.throw_when.total_operations
                = helpers::g_test_controller.count_of.total_operations
                + ops_count;

            try
            {
               if constexpr (!std::is_move_constructible_v<T>)
               {
                  vec.assign(source.begin(), source.end());
               }
               else
               {
                  vec.assign(
                      std::make_move_iterator(source.begin()),
                      std::make_move_iterator(source.end()));
               }

               clean_pass = true;
               helpers::g_test_controller.disable_throwing();
            }
            catch (helpers::TestException const &)
            {
               REQUIRE(vec.size() <= vec.capacity());
               REQUIRE(
                   helpers::g_test_controller.instances_alive()
                   == vec.size() + source.size());
            }
         }
         REQUIRE(helpers::g_test_controller.all_instances_destroyed());
      }
   }
}

TEMPLATE_TEST_CASE(
    "Vector::assign(InputIterator src_begin, Sentinel src_end) assigns from an "
    "input iterator.",
    "[vector][assign]",
    std::uint8_t,
    int,
    helpers::TrackedObject,
    helpers::NoThrowTrackedObject,
    helpers::CopyOnlyTrackedObject,
    helpers::MoveOnlyTrackedObject)
{
   using T = TestType;

   helpers::g_test_controller.reset();
   {
      auto const initial_size{ 10UZ };
      auto const source_size{ initial_size * 2 };

      swtl::Vector<T> vec(initial_size);
      auto source{ helpers::generate_vector<T>(source_size) };

      helpers::InputIterator<T> begin{ source.data() };
      helpers::InputIterator<T> end{ source.data() + source.size() };

      SECTION("Assignment triggers reallocation.")
      {
         auto const expected{ helpers::generate_vector<T>(source_size) };

         if constexpr (!std::is_move_constructible_v<T>)
         {
            vec.assign(begin, end);
         }
         else
         {
            vec.assign(
                std::make_move_iterator(begin), std::make_move_iterator(end));
         }

         REQUIRE(vec == expected);
      }

      SECTION("Assignment to some existing elements.")
      {
         auto const count{ initial_size / 2 };

         auto const expected{ helpers::generate_vector<T>(count) };

         end = source.data() + count;

         if constexpr (!std::is_move_assignable_v<T>)
         {
            vec.assign(begin, end);
         }
         else
         {
            vec.assign(
                std::make_move_iterator(begin), std::make_move_iterator(end));
         }

         REQUIRE(vec == expected);

         if constexpr (std::derived_from<T, helpers::TrackedObject>)
         {
            // Ensures that existing elements past the new elements are
            // destroyed following assignment.
            REQUIRE(
                helpers::g_test_controller.instances_alive()
                == vec.size() + expected.size() + source.size());
         }
      }

      SECTION("Assignment to existing memory.")
      {
         vec.reserve(source_size);

         auto const expected{ helpers::generate_vector<T>(source_size) };

         if constexpr (
             !std::is_move_assignable_v<T> && !std::is_move_constructible_v<T>)
         {
            vec.assign(begin, end);
         }
         else
         {
            vec.assign(
                std::make_move_iterator(begin), std::make_move_iterator(end));
         }

         REQUIRE(vec == expected);
      }
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEMPLATE_TEST_CASE(
    "Vector::assign(InputIterator src_begin, Sentinel src_end) exception "
    "safety for input iterators - exhaustive test.",
    "[vector][assign][exception][exhaustive]",
    helpers::TrackedObject,
    helpers::CopyOnlyTrackedObject,
    helpers::MoveOnlyTrackedObject)
{
   using T = TestType;

   SECTION("Source is equal in size to dest.")
   {
      auto const size{ 20UZ };

      bool clean_pass{ false };

      for (auto ops_count{ 1UZ }; !clean_pass; ++ops_count)
      {
         helpers::g_test_controller.reset();
         {
            swtl::Vector vec{ helpers::generate_vector<T>(size) };
            swtl::Vector source{ helpers::generate_vector<T>(size) };

            helpers::InputIterator<T> begin{ source.data() };
            helpers::InputIterator<T> end{ source.data() + source.size() };

            helpers::g_test_controller.enable_throwing();
            helpers::g_test_controller.throw_when.total_operations
                = helpers::g_test_controller.count_of.total_operations
                + ops_count;

            try
            {
               if constexpr (!std::is_move_constructible_v<T>)
               {
                  vec.assign(begin, end);
               }
               else
               {
                  vec.assign(
                      std::make_move_iterator(begin),
                      std::make_move_iterator(end));
               }

               clean_pass = true;
               helpers::g_test_controller.disable_throwing();
            }
            catch (helpers::TestException const &)
            {
               REQUIRE(vec.size() <= vec.capacity());
               REQUIRE(
                   helpers::g_test_controller.instances_alive()
                   == vec.size() + source.size());
            }
         }
         REQUIRE(helpers::g_test_controller.all_instances_destroyed());
      }
   }

   SECTION("Source is smaller than dest.")
   {
      auto const source_size{ 5UZ };

      bool clean_pass{ false };

      for (auto ops_count{ 1UZ }; !clean_pass; ++ops_count)
      {
         helpers::g_test_controller.reset();
         {
            swtl::Vector vec{ helpers::generate_vector<T>(10UZ) };
            swtl::Vector source{ helpers::generate_vector<T>(source_size) };

            INFO(
                "If source_size (which was "
                << source_size << " ) is greater than vec.size() (which was "
                << vec.size() << " ) this test is invalid.");
            REQUIRE(source_size < vec.size());

            helpers::InputIterator<T> begin{ source.data() };
            helpers::InputIterator<T> end{ source.data() + source.size() };

            helpers::g_test_controller.enable_throwing();
            helpers::g_test_controller.throw_when.total_operations
                = helpers::g_test_controller.count_of.total_operations
                + ops_count;

            try
            {
               if constexpr (!std::is_move_constructible_v<T>)
               {
                  vec.assign(begin, end);
               }
               else
               {
                  vec.assign(
                      std::make_move_iterator(begin),
                      std::make_move_iterator(end));
               }

               clean_pass = true;
               helpers::g_test_controller.disable_throwing();
            }
            catch (helpers::TestException const &)
            {
               REQUIRE(vec.size() <= vec.capacity());
               REQUIRE(
                   helpers::g_test_controller.instances_alive()
                   == vec.size() + source.size());
            }
         }
         REQUIRE(helpers::g_test_controller.all_instances_destroyed());
      }
   }

   SECTION("Source is larger than dest.")
   {
      auto const source_size{ 20UZ };

      bool clean_pass{ false };

      for (auto ops_count{ 1UZ }; !clean_pass; ++ops_count)
      {
         helpers::g_test_controller.reset();
         {
            swtl::Vector vec{ helpers::generate_vector<T>(10UZ) };
            swtl::Vector source{ helpers::generate_vector<T>(source_size) };

            INFO(
                "If source_size (which was "
                << source_size
                << " ) is less than or equal to vec.size() (which was "
                << vec.size() << " ) this test is invalid.");
            REQUIRE(source_size > vec.size());

            helpers::InputIterator<T> begin{ source.data() };
            helpers::InputIterator<T> end{ source.data() + source.size() };

            helpers::g_test_controller.enable_throwing();
            helpers::g_test_controller.throw_when.total_operations
                = helpers::g_test_controller.count_of.total_operations
                + ops_count;

            try
            {
               if constexpr (!std::is_move_constructible_v<T>)
               {
                  vec.assign(begin, end);
               }
               else
               {
                  vec.assign(
                      std::make_move_iterator(begin),
                      std::make_move_iterator(end));
               }

               clean_pass = true;
               helpers::g_test_controller.disable_throwing();
            }
            catch (helpers::TestException const &)
            {
               REQUIRE(vec.size() <= vec.capacity());
               REQUIRE(
                   helpers::g_test_controller.instances_alive()
                   == vec.size() + source.size());
            }
         }
         REQUIRE(helpers::g_test_controller.all_instances_destroyed());
      }
   }
}

// Vector::assign(std::initializer_list<T> init_list) uses assign(InputIterator
// first, Sentinel last) internally.
TEST_CASE(
    "Vector::assign(std::initializer_list<T> init_list) updates vector with "
    "new data.",
    "[vector][assign]")
{
   using T = helpers::TrackedObject;

   helpers::g_test_controller.reset();
   {
      std::initializer_list<T> const init_list{ T{ 1 }, T{ 2 }, T{ 3 },
                                                T{ 4 }, T{ 5 }, T{ 6 },
                                                T{ 7 }, T{ 8 }, T{ 9 } };
      swtl::Vector<T> vec(init_list.size());

      vec.assign(init_list);

      REQUIRE(std::ranges::equal(vec, init_list));
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

// Vector::assign_range(Range &&range) uses assign(InputIterator first, Sentinel
// last) internally.
TEST_CASE(
    "Vector::assign_range(Range &&range) updates vector with new data.",
    "[vector][assign]")
{
   using T = helpers::TrackedObject;

   helpers::g_test_controller.reset();
   {
      auto const range{ helpers::generate_vector<T>() };
      swtl::Vector<T> vec;

      vec.assign_range(range);

      REQUIRE(std::ranges::equal(vec, range));
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEST_CASE(
    "Vector::get_allocator() returns the correct allocator.",
    "[vector][allocator]")
{
   std::allocator<int> test_alloc;
   swtl::Vector<int, std::allocator<int>> vec(test_alloc);

   REQUIRE(vec.get_allocator() == test_alloc);
}

TEST_CASE(
    "Vector element access, return value const qualification.",
    "[vector][accessors]")
{
   using T = helpers::TrackedObject;

   helpers::g_test_controller.reset();
   {
      auto vec{ helpers::generate_vector<T>() };
      std::vector<T> expected(vec.begin(), vec.end());

      auto const const_vec{ vec };
      std::vector<T> const const_expected(vec.begin(), vec.end());

      auto const first_idx{ 0UZ };
      auto const last_idx{ vec.size() - 1 };

      SECTION("at() returns a reference to the element at position.")
      {
         REQUIRE(vec.at(first_idx) == expected[first_idx]);
         REQUIRE(vec.at(last_idx) == expected[last_idx]);
         REQUIRE(const_vec.at(first_idx) == const_expected[first_idx]);
         REQUIRE(const_vec.at(last_idx) == const_expected[last_idx]);
         STATIC_REQUIRE(
             std::is_same_v<
                 decltype(vec.at(first_idx)),
                 decltype(expected.at(first_idx))
             >);
         STATIC_REQUIRE(
             std::is_same_v<
                 decltype(const_vec.at(first_idx)),
                 decltype(const_expected.at(first_idx))
             >);
      }

      SECTION(
          "operator[] returns a reference to the element at "
          "position.")
      {
         REQUIRE(vec[first_idx] == expected[first_idx]);
         REQUIRE(vec[last_idx] == expected[last_idx]);
         REQUIRE(const_vec[first_idx] == const_expected[first_idx]);
         REQUIRE(const_vec[last_idx] == const_expected[last_idx]);
         STATIC_REQUIRE(
             std::is_same_v<
                 decltype(vec[first_idx]),
                 decltype(expected[first_idx])
             >);
         STATIC_REQUIRE(
             std::is_same_v<
                 decltype(const_vec[first_idx]),
                 decltype(const_expected[first_idx])
             >);
      }

      SECTION("front() returns a reference to the first element.")
      {
         REQUIRE(vec.front() == expected.front());
         REQUIRE(const_vec.front() == const_expected.front());
         STATIC_REQUIRE(
             std::is_same_v<decltype(vec.front()), decltype(expected.front())>);
         STATIC_REQUIRE(
             std::is_same_v<
                 decltype(const_vec.front()),
                 decltype(const_expected.front())
             >);
      }

      SECTION("back() returns a reference to the last element.")
      {
         REQUIRE(vec.back() == expected.back());
         REQUIRE(const_vec.back() == const_expected.back());
         STATIC_REQUIRE(
             std::is_same_v<decltype(vec.back()), decltype(expected.back())>);
         STATIC_REQUIRE(
             std::is_same_v<
                 decltype(const_vec.back()),
                 decltype(const_expected.back())
             >);
      }
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEST_CASE("Vector element access, bounds safety.", "[vector][accessors]")
{
   using T = helpers::TrackedObject;

   helpers::g_test_controller.reset();
   {
      swtl::Vector<T> const empty_vec;
      auto const vec{ helpers::generate_vector<T>() };
      auto const out_of_bounds_index{ vec.size() };

      INFO(
          "INFO: If `out_of_bounds_index` (which was "
          << out_of_bounds_index
          << ") is not greater than or equal to `vec.size()` (which was "
          << vec.size() << ") this test is invalid.");
      REQUIRE(out_of_bounds_index >= vec.size());

      SECTION("Vector::at throws when accessing an index out of bounds.")
      {
         using Catch::Matchers::ContainsSubstring;
         using Catch::Matchers::MessageMatches;

         REQUIRE_THROWS_MATCHES(
             vec.at(out_of_bounds_index),
             std::out_of_range,
             MessageMatches(
                 ContainsSubstring(std::to_string(out_of_bounds_index)))
                 && MessageMatches(
                     ContainsSubstring(std::to_string(vec.size()))));
      }

      SECTION(
          "Vector::operator[] triggers a contract violation when accessing an "
          "index out of bounds.")
      {
         REQUIRE_THROWS_AS(vec[out_of_bounds_index], ContractException);
      }

      SECTION(
          "Vector::front() triggers a contract violation when called on an "
          "empty vector.")
      {
         REQUIRE_THROWS_AS(empty_vec.front(), ContractException);
      }

      SECTION(
          "Vector::back() triggers a contract violation when called on an "
          "empty vector.")
      {
         REQUIRE_THROWS_AS(empty_vec.back(), ContractException);
      }
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEST_CASE(
    "Vector element access, modification with front(), back(), at(), and "
    "operator[].",
    "[vector][accessors]")
{
   swtl::Vector actual{ 0, 0, 0, 0 };
   swtl::Vector const expected{ 1, 2, 3, 4 };

   actual.front() = 1;
   actual[1UZ] = 2;
   actual.at(2UZ) = 3;
   actual.back() = 4;

   REQUIRE(actual == expected);
}

TEST_CASE(
    "Vector::data() returns a pointer to mutable internal data.",
    "[vector][accessors]")
{
   swtl::Vector vec{ 1, 2, 3 };
   auto data_ptr{ vec.data() };
   auto const expected{ 2 };

   *data_ptr = expected;

   REQUIRE(data_ptr == std::to_address(vec.begin()));
   REQUIRE(*data_ptr == vec.front());
   REQUIRE(vec.front() == expected);
}

// TODO: Coverage Testing - implement a testing allocator to test the path where
// requested capacity is valid but the amount of memory returned by the
// allocator exceeds max.
TEMPLATE_TEST_CASE(
    "Vector::reserve() increases capacity on an empty vector.",
    "[vector][capacity][reserve]",
    std::uint8_t,
    int,
    helpers::TrackedObject,
    helpers::NoThrowTrackedObject,
    helpers::CopyOnlyTrackedObject,
    helpers::MoveOnlyTrackedObject)
{
   swtl::Vector<TestType> vec;

   // Capacity may be greater than requested due to using allocate_at_least().

   SECTION("Reserve increases capacity but does not affect size.")
   {
      auto const initial_capacity{ 10UZ };
      vec.reserve(initial_capacity);

      REQUIRE(vec.is_empty());
      REQUIRE(vec.size() == 0UZ);
      REQUIRE(vec.capacity() >= initial_capacity);

      SECTION("Continued reservation grows capacity again.")
      {
         auto const final_capacity{ 20UZ };
         vec.reserve(final_capacity);

         REQUIRE(vec.is_empty());
         REQUIRE(vec.size() == 0UZ);
         REQUIRE(vec.capacity() >= final_capacity);

         SECTION("Reserving less than the current capacity does nothing.")
         {
            auto const before_resize_attempt{ vec.capacity() };
            vec.reserve(initial_capacity);

            REQUIRE(vec.is_empty());
            REQUIRE(vec.size() == 0UZ);
            REQUIRE(vec.capacity() == before_resize_attempt);
         }
      }
   }

   SECTION("Reserving more than the maximum number of elements throws.")
   {
      REQUIRE_THROWS_AS(
          vec.reserve(std::numeric_limits<std::size_t>::max()),
          std::length_error);
   }
}

TEMPLATE_TEST_CASE(
    "Vector::reserve() increases capacity on a populated vector.",
    "[vector][capacity][reserve]",
    std::uint8_t,
    int,
    helpers::TrackedObject,
    helpers::NoThrowTrackedObject,
    helpers::CopyOnlyTrackedObject,
    helpers::MoveOnlyTrackedObject)
{
   using T = TestType;

   helpers::g_test_controller.reset();
   {
      auto vec{ helpers::generate_vector<T>() };
      auto const initial_capacity{ vec.capacity() };
      swtl::Vector const expected{ helpers::generate_vector<T>() };

      // Capacity may be greater than requested due to using
      // allocate_at_least().

      SECTION("Reservation grows capacity but does not modify elements.")
      {
         auto new_capacity{ initial_capacity + 10UZ };
         vec.reserve(new_capacity);

         REQUIRE(vec == expected);
         REQUIRE(vec.size() == expected.size());
         REQUIRE(vec.capacity() >= new_capacity);

         SECTION("Continued reservation grows capacity again.")
         {
            auto const final_capacity{ vec.capacity() + 20UZ };
            vec.reserve(final_capacity);

            REQUIRE(vec == expected);
            REQUIRE(vec.size() == expected.size());
            REQUIRE(vec.capacity() >= final_capacity);

            SECTION("Reserving less than the current capacity does nothing.")
            {
               vec.reserve(initial_capacity);

               REQUIRE(vec == expected);
               REQUIRE(vec.size() == expected.size());
               REQUIRE(vec.capacity() >= final_capacity);
            }
         }
      }
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

// Vector::push_back() uses emplace_back() internally.
TEMPLATE_TEST_CASE(
    "Vector::push_back() inserts a new element at the end.",
    "[vector][modifiers][push_back]",
    std::uint8_t,
    int,
    helpers::TrackedObject,
    helpers::NoThrowTrackedObject,
    helpers::CopyOnlyTrackedObject,
    helpers::MoveOnlyTrackedObject)
{
   using T = TestType;

   helpers::g_test_controller.reset();
   {
      swtl::Vector<T> vec;
      auto const data{ helpers::generate_vector<T>() };

      if constexpr (std::is_copy_constructible_v<T>)
      {
         SECTION("Inserting lvalue references works as expected.")
         {
            for (auto const &element : data)
            {
               vec.push_back(element);
            }

            REQUIRE(std::ranges::equal(vec, data));
         }
      }

      if constexpr (std::is_move_constructible_v<T>)
      {
         SECTION("Inserting rvalue references works as expected.")
         {
            auto expiring{ helpers::generate_vector<T>() };

            for (auto &element : expiring)
            {
               vec.push_back(std::move(element));
            }

            REQUIRE(std::ranges::equal(vec, data));
         }
      }
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEMPLATE_TEST_CASE(
    "Vector::emplace_back() constructs a new element at the end.",
    "[vector][modifiers][emplace_back]",
    std::uint8_t,
    int,
    helpers::TrackedObject,
    helpers::NoThrowTrackedObject,
    helpers::CopyOnlyTrackedObject,
    helpers::MoveOnlyTrackedObject)
{
   using T = TestType;

   helpers::g_test_controller.reset();
   {
      swtl::Vector<T> vec;
      auto const expected{ helpers::generate_vector<T>() };

      if constexpr (std::is_copy_constructible_v<T>)
      {
         SECTION("Inserting lvalue references succeeds.")
         {
            for (auto const &element : expected)
            {
               vec.emplace_back(element);
            }

            REQUIRE(vec == expected);
         }
      }

      if constexpr (std::is_move_constructible_v<T>)
      {
         SECTION("Inserting rvalue references succeeds.")
         {
            auto expiring{ helpers::generate_vector<T>() };

            for (auto &element : expiring)
            {
               vec.emplace_back(std::move(element));
            }

            REQUIRE(vec == expected);
         }
      }

      SECTION("Default construction succeeds.")
      {
         auto const count{ 10UZ };
         swtl::Vector<T> default_expected(count);

         for (auto counter{ 0UZ }; counter != count; ++counter)
         {
            vec.emplace_back();
         }

         REQUIRE(vec == default_expected);
      }

      SECTION("Argument construction succeeds.")
      {
         for (auto const &[idx, _] : std::views::enumerate(expected))
         {
            vec.emplace_back(idx);
         }

         REQUIRE(vec == expected);
      }

      if constexpr (std::is_copy_constructible_v<T>)
      {
         SECTION("emplace_back returns a reference to the inserted element.")
         {
            T reference_object{ 42 };

            REQUIRE(vec.emplace_back(reference_object) == reference_object);
            REQUIRE(vec.back() == reference_object);
         }
      }
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEMPLATE_TEST_CASE(
    "Vector::emplace_back() memory growth.",
    "[vector][modifiers][emplace_back]",
    std::uint8_t,
    int,
    helpers::TrackedObject,
    helpers::NoThrowTrackedObject,
    helpers::CopyOnlyTrackedObject,
    helpers::MoveOnlyTrackedObject)
{
   using T = TestType;

   helpers::g_test_controller.reset();
   {
      auto vec{ helpers::generate_vector<T>() };
      vec.reserve(10UZ);
      auto const initial_capacity{ vec.capacity() };

      SECTION("Insertion up to capacity does not trigger growth.")
      {
         while (vec.size() < initial_capacity)
         {
            vec.emplace_back(vec.size());
         }

         auto expected{ helpers::generate_vector<T>(vec.size()) };

         REQUIRE(vec.size() == initial_capacity);
         REQUIRE(vec.capacity() == initial_capacity);
         REQUIRE(vec == expected);

         SECTION("Adding elements beyond capacity at minimum doubles capacity.")
         {
            vec.emplace_back(vec.size());

            expected = helpers::generate_vector<T>(vec.size());

            REQUIRE(vec.capacity() >= initial_capacity * 2UZ);
            REQUIRE(vec == expected);

            SECTION("Consistent lack of growth.")
            {
               auto const expanded_capacity{ vec.capacity() };

               while (vec.size() < expanded_capacity)
               {
                  vec.emplace_back(vec.size());
               }

               expected = helpers::generate_vector<T>(vec.size());

               REQUIRE(vec.size() == expanded_capacity);
               REQUIRE(vec.capacity() == expanded_capacity);
               REQUIRE(vec == expected);

               SECTION("Consistent growth.")
               {
                  // For coverage testing.
                  T lvalue{ static_cast<std::uint8_t>(vec.size()) };
                  if constexpr (std::is_copy_constructible_v<T>)
                  {
                     vec.emplace_back(lvalue);
                  }
                  else
                  {
                     vec.emplace_back(std::move(lvalue));
                  }

                  expected = helpers::generate_vector<T>(vec.size());

                  REQUIRE(vec.capacity() >= expanded_capacity * 2UZ);
                  REQUIRE(vec == expected);
               }
            }
         }
      }

      // TODO: Implement a "MeagerAllocator" that returns a small max_size. Then
      // you can test the case where calculate_growth_size throws a length
      // error.
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEMPLATE_TEST_CASE(
    "Vector::emplace_back() exception safety.",
    "[vector][modifiers][exception]",
    helpers::TrackedObject,
    helpers::CopyOnlyTrackedObject,
    helpers::MoveOnlyTrackedObject)
{
   using T = TestType;

   helpers::g_test_controller.reset();
   {
      auto reserved_vec{ helpers::generate_vector<T>() };
      reserved_vec.reserve(reserved_vec.size() + 1);

      auto full_vec{ helpers::generate_vector<T>() };
      helpers::fill_to_capacity(full_vec);

      auto const reserved_expected{ helpers::generate_vector<T>() };
      auto const full_expected{ helpers::generate_vector<T>(full_vec.size()) };

      T reference_object{ 42 };

      helpers::g_test_controller.enable_throwing();
      helpers::g_test_controller.throw_when.default_construction
          = helpers::g_test_controller.count_of.default_construction + 1UZ;
      helpers::g_test_controller.throw_when.arg_construction
          = helpers::g_test_controller.count_of.arg_construction + 1UZ;
      helpers::g_test_controller.throw_when.copy_construction
          = helpers::g_test_controller.count_of.copy_construction + 1UZ;
      helpers::g_test_controller.throw_when.move_construction
          = helpers::g_test_controller.count_of.move_construction + 1UZ;

      if constexpr (std::is_copy_constructible_v<T>)
      {
         SECTION("lvalue reference with available space.")
         {
            REQUIRE_THROWS_AS(
                reserved_vec.emplace_back(reference_object),
                helpers::TestException);
            REQUIRE(reserved_vec == reserved_expected);
         }

         SECTION("lvalue reference with reallocation.")
         {
            REQUIRE_THROWS_AS(
                full_vec.emplace_back(reference_object),
                helpers::TestException);
            REQUIRE(full_vec == full_expected);
         }
      }

      if constexpr (std::is_move_constructible_v<T>)
      {
         SECTION("rvalue reference with available space.")
         {
            REQUIRE_THROWS_AS(
                reserved_vec.emplace_back(std::move(reference_object)),
                helpers::TestException);
            REQUIRE(reserved_vec == reserved_expected);
         }

         SECTION("rvalue reference with reallocation.")
         {
            REQUIRE_THROWS_AS(
                full_vec.emplace_back(std::move(reference_object)),
                helpers::TestException);
            REQUIRE(full_vec == full_expected);
         }
      }

      SECTION("Default construction with available space.")
      {
         REQUIRE_THROWS_AS(reserved_vec.emplace_back(), helpers::TestException);
         REQUIRE(reserved_vec == reserved_expected);
      }

      SECTION("Default construction with reallocation.")
      {
         REQUIRE_THROWS_AS(full_vec.emplace_back(), helpers::TestException);
         REQUIRE(full_vec == full_expected);
      }

      SECTION("Argument construction with available space.")
      {
         REQUIRE_THROWS_AS(
             reserved_vec.emplace_back(42), helpers::TestException);
         REQUIRE(reserved_vec == reserved_expected);
      }

      SECTION("Argument construction with reallocation.")
      {
         REQUIRE_THROWS_AS(full_vec.emplace_back(42), helpers::TestException);
         REQUIRE(full_vec == full_expected);
      }
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEST_CASE(
    "Vector reallocation falls back to copying if move constructor is not "
    "noexcept.",
    "[vector][modifiers]")
{
   helpers::g_test_controller.reset();
   {
      auto vec{ helpers::generate_vector<helpers::TrackedObject>() };
      helpers::fill_to_capacity(vec);

      auto const copies_before_growth{
         helpers::g_test_controller.count_of.copy_construction
      };
      auto const moves_before_growth{
         helpers::g_test_controller.count_of.move_construction
      };
      auto const expected_size{ vec.size() + 1UZ };

      REQUIRE_NOTHROW(vec.emplace_back());
      REQUIRE(vec.size() == expected_size);
      REQUIRE(
          helpers::g_test_controller.count_of.copy_construction
          > copies_before_growth);
      REQUIRE(
          helpers::g_test_controller.count_of.move_construction
          == moves_before_growth);
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEST_CASE("Vector growth with copy-only types.", "[vector][modifiers]")
{
   helpers::g_test_controller.reset();
   {
      auto vec{ helpers::generate_vector<helpers::CopyOnlyTrackedObject>() };
      helpers::fill_to_capacity(vec);

      auto const copies_before_growth{
         helpers::g_test_controller.count_of.copy_construction
      };
      auto const moves_before_growth{
         helpers::g_test_controller.count_of.move_construction
      };
      auto const expected_size{ vec.size() + 1UZ };

      REQUIRE_NOTHROW(vec.emplace_back());
      REQUIRE(vec.size() == expected_size);
      REQUIRE(
          helpers::g_test_controller.count_of.copy_construction
          > copies_before_growth);
      REQUIRE(
          helpers::g_test_controller.count_of.move_construction
          == moves_before_growth);
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEST_CASE("Vector growth with move-only types.", "[vector][modifiers]")
{
   helpers::g_test_controller.reset();
   {
      auto vec{ helpers::generate_vector<helpers::MoveOnlyTrackedObject>() };
      helpers::fill_to_capacity(vec);

      auto const copies_before_growth{
         helpers::g_test_controller.count_of.copy_construction
      };
      auto const moves_before_growth{
         helpers::g_test_controller.count_of.move_construction
      };
      auto const expected_size{ vec.size() + 1UZ };

      REQUIRE_NOTHROW(vec.emplace_back());
      REQUIRE(vec.size() == expected_size);
      REQUIRE(
          helpers::g_test_controller.count_of.copy_construction
          == copies_before_growth);
      REQUIRE(
          helpers::g_test_controller.count_of.move_construction
          > moves_before_growth);
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEMPLATE_TEST_CASE(
    "Vector comparison.",
    "[vector][comparison]",
    std::uint8_t,
    int,
    helpers::TrackedObject,
    helpers::NoThrowTrackedObject,
    helpers::CopyOnlyTrackedObject,
    helpers::MoveOnlyTrackedObject)
{
   using T = TestType;

   auto const baseline_vec{ helpers::make_vec_of<T>(
       0UZ, 1UZ, 2UZ, 3UZ, 4UZ, 5UZ) };
   auto const equal_vec{ helpers::make_vec_of<T>(
       0UZ, 1UZ, 2UZ, 3UZ, 4UZ, 5UZ) };
   auto const greater_vec{ helpers::make_vec_of<T>(
       0UZ, 1UZ, 2UZ, 3UZ, 4UZ, 6UZ) };
   auto const lesser_vec{ helpers::make_vec_of<T>(
       0UZ, 1UZ, 2UZ, 3UZ, 4UZ, 4UZ) };
   auto const bigger_vec_with_lesser_values{ helpers::make_vec_of<T>(
       0UZ, 0UZ, 1UZ, 2UZ, 3UZ, 4UZ, 5UZ) };
   auto const smaller_vec_with_greater_values{ helpers::make_vec_of<T>(
       9UZ, 8UZ, 7UZ) };
   auto const smaller_vec_with_similar_values{ helpers::make_vec_of<T>(
       0UZ, 1UZ, 2UZ) };
   auto const different_elements_but_same_size{ helpers::make_vec_of<T>(
       1UZ, 2UZ, 3UZ, 4UZ, 5UZ, 6UZ) };
   auto same_elements_different_capacity{ helpers::make_vec_of<T>(
       0UZ, 1UZ, 2UZ, 3UZ, 4UZ, 5UZ) };
   same_elements_different_capacity.reserve(100UZ);

   REQUIRE(baseline_vec == equal_vec);
   REQUIRE(baseline_vec == same_elements_different_capacity);

   REQUIRE(baseline_vec != greater_vec);
   REQUIRE(baseline_vec != lesser_vec);
   REQUIRE(baseline_vec != different_elements_but_same_size);
   REQUIRE(baseline_vec != bigger_vec_with_lesser_values);
   REQUIRE(baseline_vec != smaller_vec_with_greater_values);

   REQUIRE(baseline_vec < greater_vec);
   REQUIRE(baseline_vec < smaller_vec_with_greater_values);

   REQUIRE(baseline_vec > lesser_vec);
   REQUIRE(baseline_vec > bigger_vec_with_lesser_values);

   REQUIRE(baseline_vec <= greater_vec);
   REQUIRE(baseline_vec <= equal_vec);

   REQUIRE(baseline_vec >= lesser_vec);
   REQUIRE(baseline_vec >= equal_vec);

   REQUIRE((baseline_vec <=> smaller_vec_with_similar_values) > 0);
   REQUIRE((smaller_vec_with_similar_values <=> baseline_vec) < 0);
}

TEMPLATE_TEST_CASE(
    "Vector swaps objects correctly.",
    "[vector][modifiers][swap]",
    std::uint8_t,
    int,
    helpers::TrackedObject,
    helpers::NoThrowTrackedObject,
    helpers::CopyOnlyTrackedObject,
    helpers::MoveOnlyTrackedObject)
{
   using T = TestType;

   helpers::g_test_controller.reset();
   {
      auto vec_a{ helpers::generate_vector<T>(10UZ) };
      auto vec_b{ helpers::generate_vector<T>(20UZ) };

      auto const *vec_a_ptr{ vec_a.data() };
      auto const vec_a_size{ vec_a.size() };
      auto const *vec_b_ptr{ vec_b.data() };
      auto const vec_b_size{ vec_b.size() };

      using std::swap;
      swap(vec_a, vec_b);

      REQUIRE(vec_a.size() == vec_b_size);
      REQUIRE(vec_b.size() == vec_a_size);
      REQUIRE(vec_b.data() == vec_a_ptr);
      REQUIRE(vec_a.data() == vec_b_ptr);
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEMPLATE_TEST_CASE(
    "Vector::max_size() returns sane values.",
    "[vector][capacity][max_size]",
    std::uint8_t,
    int,
    helpers::TrackedObject)
{
   swtl::Vector<TestType> vec;

   REQUIRE(
       vec.max_size()
       <= std::numeric_limits<std::ptrdiff_t>::max() / sizeof(TestType));
}

TEMPLATE_TEST_CASE(
    "Vector::is_empty() returns the correct boolean value.",
    "[vector][capacity][is_empty]",
    std::uint8_t,
    int,
    helpers::TrackedObject,
    helpers::NoThrowTrackedObject,
    helpers::CopyOnlyTrackedObject,
    helpers::MoveOnlyTrackedObject)
{
   helpers::g_test_controller.reset();
   {
      swtl::Vector<TestType> empty_vec;
      auto const non_empty_vec{ helpers::generate_vector<TestType>() };

      REQUIRE(empty_vec.is_empty());
      REQUIRE(!non_empty_vec.is_empty());
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEMPLATE_TEST_CASE(
    "Vector::size() returns the correct value as elements are added.",
    "[vector][capacity][size]",
    std::uint8_t,
    int,
    helpers::TrackedObject,
    helpers::NoThrowTrackedObject,
    helpers::CopyOnlyTrackedObject,
    helpers::MoveOnlyTrackedObject)
{
   helpers::g_test_controller.reset();
   {
      swtl::Vector<TestType> vec;

      SECTION("Empty vector returns size of zero.")
      {
         REQUIRE(vec.size() == 0UZ);

         SECTION("Adding elements increases size accordingly.")
         {
            auto const limit{ 100UZ };

            for (auto const count : std::views::iota(0UZ, limit))
            {
               REQUIRE(vec.size() == count);
               vec.emplace_back();
            }

            REQUIRE(vec.size() == limit);
         }
      }
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEMPLATE_TEST_CASE(
    "Vector::capacity() returns expected values.",
    "[vector][capacity]",
    std::uint8_t,
    int,
    helpers::TrackedObject)
{
   swtl::Vector<TestType> vec;

   SECTION("Empty vector has zero capacity.")
   {
      REQUIRE(vec.capacity() == 0UZ);
   }

   SECTION("Reserved vector has increased capaicty.")
   {
      auto const value{ 10UZ };
      vec.reserve(value);

      REQUIRE(vec.capacity() >= value);
   }
}

TEMPLATE_TEST_CASE(
    "Vector::shrink_to_fit() reduces capacity to size if appropriate.",
    "[vector][capacity][shrink_to_fit]",
    helpers::TrackedObject,
    helpers::NoThrowTrackedObject,
    helpers::CopyOnlyTrackedObject,
    helpers::MoveOnlyTrackedObject)
{
   using T = TestType;

   helpers::g_test_controller.reset();
   {
      swtl::Vector<T> vec;

      SECTION("Shrinking a vector with no capacity does nothing.")
      {
         vec.shrink_to_fit();

         REQUIRE(vec.capacity() == 0UZ);
         REQUIRE(vec.data() == nullptr);
      }

      SECTION("Shrinking an empty vector releases memroy.")
      {
         vec.reserve(100UZ);

         vec.shrink_to_fit();

         REQUIRE(vec.capacity() == 0UZ);
         REQUIRE(vec.data() == nullptr);
      }

      SECTION(
          "Shrinking a container that has no reserve capacity does nothing.")
      {
         vec.reserve(100UZ);
         helpers::fill_to_capacity(vec);

         auto const previous_capacity{ vec.capacity() };
         auto const previous_data_pointer{ vec.data() };
         auto const previous_size{ vec.size() };

         vec.shrink_to_fit();

         REQUIRE(vec.capacity() == previous_capacity);
         REQUIRE(vec.data() == previous_data_pointer);
         REQUIRE(vec.size() == previous_size);
      }

      SECTION(
          "Shrinking a vector with excess capacity reduces capacity without "
          "modifying elements.")
      {
         vec.reserve(100UZ);
         helpers::fill_to_capacity(vec);
         vec.reserve(200UZ);

         auto const previous_capacity{ vec.capacity() };
         auto const previous_data_pointer{ vec.data() };
         auto const previous_size{ vec.size() };

         vec.shrink_to_fit();

         REQUIRE(vec.capacity() < previous_capacity);
         REQUIRE(vec.data() != previous_data_pointer);
         REQUIRE(vec.size() == previous_size);
      }

      // TODO: Implement an "OvereagerAllocator" that allocates double the
      // memory requested.  Then you can test the case where shrink to fit
      // allocates more than capacity() and doesn't change the vector.
      /*
      SECTION(
          "If the allocation for shrinkage exceeds current capacity the vector "
          "is unmodified.")
      {
         swtl::Vector<T, helpers::OvereagerAllocator> overeager_vec;
         overeager_vec.reserve(2UZ);
         overeager_vec.assign(5UZ, T{});

         auto const previous_capacity{ overeager_vec.capacity() };
         auto const previous_data_pointer{ overeager_vec.data() };
         auto const previous_size{ overeager_vec.size() };

         overeager_vec.shrink_to_fit();

         REQUIRE(overeager_vec.capacity() == previous_capacity);
         REQUIRE(overeager_vec.data() == previous_data_pointer);
         REQUIRE(overeager_vec.size() == previous_size);
      }
      */
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEMPLATE_TEST_CASE(
    "Vector::clear() removes all elements of the vector without effecting "
    "capacity.",
    "[vector][modifiers][clear]",
    std::uint8_t,
    int,
    helpers::TrackedObject,
    helpers::NoThrowTrackedObject,
    helpers::CopyOnlyTrackedObject,
    helpers::MoveOnlyTrackedObject)
{
   helpers::g_test_controller.reset();
   {
      auto vec{ helpers::generate_vector<TestType>() };
      auto const populated_capacity{ vec.capacity() };

      vec.clear();

      REQUIRE(vec.is_empty());
      REQUIRE(vec.size() == 0UZ);
      REQUIRE(vec.capacity() == populated_capacity);
      REQUIRE(vec.data() != nullptr);
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEMPLATE_TEST_CASE(
    "Vector::emplace(const_iterator pos, Args &&...args) constructs a value at "
    "the desired location of a vector.",
    "[vector][modifiers][emplace]",
    helpers::TrackedObject,
    helpers::NoThrowTrackedObject,
    helpers::CopyOnlyTrackedObject)
{
   using T = TestType;

   helpers::g_test_controller.reset();
   {
      auto vec{ helpers::make_vec_of<T>(
          1UZ, 2UZ, 3UZ, 4UZ, 5UZ, 6UZ, 7UZ, 8UZ, 9UZ) };

      vec.reserve(10UZ);
      auto const args{ 0UZ };

      SECTION("Calling emplace with `begin()` inserts at the beginning.")
      {
         auto const expected{ helpers::make_vec_of<T>(
             0UZ, 1UZ, 2UZ, 3UZ, 4UZ, 5UZ, 6UZ, 7UZ, 8UZ, 9UZ) };

         vec.emplace(vec.cbegin(), args);

         REQUIRE(vec == expected);
      }

      SECTION("Calling emplace with `end()` inserts at the end.")
      {
         auto const expected{ helpers::make_vec_of<T>(
             1UZ, 2UZ, 3UZ, 4UZ, 5UZ, 6UZ, 7UZ, 8UZ, 9UZ, 0UZ) };

         vec.emplace(vec.cend(), args);

         REQUIRE(vec == expected);
      }

      SECTION(
          "Calling emplace with an iterator to the middle inserts in the "
          "correct position.")
      {
         auto const expected{ helpers::make_vec_of<T>(
             1UZ, 2UZ, 3UZ, 4UZ, 0UZ, 5UZ, 6UZ, 7UZ, 8UZ, 9UZ) };
         auto const index{ 4UZ };
         auto const iter_pos{ vec.cbegin() + index };

         vec.emplace(iter_pos, args);

         REQUIRE(vec == expected);
      }

      SECTION("Vector does not grow when there is sufficient capacity.")
      {
         auto const old_capacity{ vec.capacity() };
         auto const expected{ helpers::make_vec_of<T>(
             0UZ, 1UZ, 2UZ, 3UZ, 4UZ, 5UZ, 6UZ, 7UZ, 8UZ, 9UZ) };

         vec.emplace(vec.cbegin(), args);

         REQUIRE(vec == expected);
         REQUIRE(vec.capacity() == old_capacity);
      }

      SECTION("Vector does grow when additional capacity is required.")
      {
         helpers::fill_to_capacity(vec);
         auto const old_capacity{ vec.capacity() };

         auto expected{ helpers::make_vec_of<T>(
             0UZ, 1UZ, 2UZ, 3UZ, 4UZ, 5UZ, 6UZ, 7UZ, 8UZ, 9UZ) };

         while (expected.size() != vec.size() + 1)
         {
            expected.emplace_back();
         }

         vec.emplace(vec.cbegin(), args);

         REQUIRE(vec == expected);
         REQUIRE(vec.capacity() > old_capacity);
      }

      SECTION("emplace returns an iterator to the inserted element.")
      {
         T value_to_insert{ 42UZ };

         auto inserted_iter{ vec.emplace(vec.cbegin(), value_to_insert) };

         REQUIRE(inserted_iter == vec.begin());
         REQUIRE(*inserted_iter == value_to_insert);
      }
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEST_CASE(
    "Vector::emplace(const_iterator pos, Args &&...args) constructs an object "
    "in place with arguments passed to the method.",
    "[vector][modifiers][emplace]")
{
   swtl::Vector<helpers::TrackedObject> vec(10UZ);
   vec.reserve(12UZ);

   helpers::g_test_controller.reset();

   vec.emplace(vec.cend(), 42UZ);

   REQUIRE(helpers::g_test_controller.count_of.arg_construction == 1UZ);
   REQUIRE(helpers::g_test_controller.count_of.default_construction == 0UZ);
   REQUIRE(helpers::g_test_controller.count_of.copy_construction == 0UZ);
   REQUIRE(helpers::g_test_controller.count_of.move_construction == 0UZ);
}

TEMPLATE_TEST_CASE(
    "Vector::emplace(const_iterator pos, Args &&...args) exception safety.",
    "[vector][emplace][exception]",
    helpers::TrackedObject,
    helpers::NoThrowTrackedObject,
    helpers::CopyOnlyTrackedObject)
{
   using T = helpers::TrackedObject;

   helpers::g_test_controller.reset();
   {
      auto vec{ helpers::make_vec_of<T>(
          1UZ, 2UZ, 3UZ, 4UZ, 5UZ, 6UZ, 7UZ, 8UZ, 9UZ) };

      vec.reserve(10UZ);
      auto const args{ 0UZ };
      swtl::Vector const unmodified{ vec };

      SECTION("Calling emplace with `begin()`.")
      {
         helpers::g_test_controller.enable_throwing();
         helpers::g_test_controller.throw_when.arg_construction
             = helpers::g_test_controller.count_of.arg_construction + 1UZ;

         REQUIRE_THROWS_AS(
             vec.emplace(vec.cbegin(), args), helpers::TestException);

         REQUIRE(vec == unmodified);
      }

      SECTION("Calling emplace with `end()`.")
      {
         helpers::g_test_controller.enable_throwing();
         helpers::g_test_controller.throw_when.arg_construction
             = helpers::g_test_controller.count_of.arg_construction + 1UZ;

         REQUIRE_THROWS_AS(
             vec.emplace(vec.cend(), args), helpers::TestException);

         REQUIRE(vec == unmodified);
      }

      SECTION("Calling emplace with an iterator to the middle.")
      {
         auto const index{ 4UZ };
         auto const iter_pos{ vec.cbegin() + index };

         helpers::g_test_controller.enable_throwing();
         helpers::g_test_controller.throw_when.arg_construction
             = helpers::g_test_controller.count_of.arg_construction + 1UZ;

         REQUIRE_THROWS_AS(vec.emplace(iter_pos, args), helpers::TestException);

         REQUIRE(vec == unmodified);
      }

      SECTION("No leaks if an exception happens during growth.")
      {
         helpers::fill_to_capacity(vec);

         auto const old_capacity{ vec.capacity() };
         swtl::Vector const expected{ vec };

         helpers::g_test_controller.enable_throwing();
         helpers::g_test_controller.throw_when.arg_construction
             = helpers::g_test_controller.count_of.arg_construction + 1UZ;

         REQUIRE_THROWS_AS(
             vec.emplace(vec.cbegin(), args), helpers::TestException);

         REQUIRE(vec == expected);
         REQUIRE(vec.capacity() == old_capacity);
      }
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEMPLATE_TEST_CASE(
    "Vector::emplace(const_iterator pos, Args &&...args) exception safety - "
    "exhaustive test.",
    "[vector][modifiers][emplace][exception][exhaustive]",
    helpers::TrackedObject,
    helpers::CopyOnlyTrackedObject,
    helpers::MoveOnlyTrackedObject)
{
   using T = TestType;

   bool possibilities_exhausted{ false };
   auto const args{ 42UZ };

   for (auto pos{ 0UZ }; !possibilities_exhausted; ++pos)
   {
      bool clean_pass{ false };

      for (auto ops_count{ 1UZ }; !clean_pass; ++ops_count)
      {
         helpers::g_test_controller.reset();
         {
            swtl::Vector vec{ helpers::generate_vector<T>() };

            helpers::g_test_controller.enable_throwing();
            helpers::g_test_controller.throw_when.total_operations
                = helpers::g_test_controller.count_of.total_operations
                + ops_count;

            try
            {
               if (pos <= vec.size())
               {
                  vec.emplace(vec.cbegin() + pos, args);
                  clean_pass = true;
               }
               else
               {
                  helpers::g_test_controller.disable_throwing();
                  possibilities_exhausted = true;
                  clean_pass = true;
               }
            }
            catch (helpers::TestException const &)
            {
               REQUIRE(vec.size() <= vec.capacity());
               REQUIRE(
                   helpers::g_test_controller.instances_alive() == vec.size());
            }
         }
         REQUIRE(helpers::g_test_controller.all_instances_destroyed());
      }
   }
}

// Vector::insert(const_iterator pos, T const &value) uses emplace() internally.
TEST_CASE(
    "Vector::insert(const_iterator pos, T const &value) copy-inserts an lvalue",
    "[vector][modifiers][insert]")
{
   auto const initial_size{ 10UZ };
   swtl::Vector<helpers::TrackedObject> vec(initial_size);
   vec.reserve(initial_size + 1UZ);
   helpers::TrackedObject const element;

   helpers::g_test_controller.reset();

   vec.insert(vec.cend(), element);

   REQUIRE(helpers::g_test_controller.count_of.copy_construction > 0UZ);
   REQUIRE(helpers::g_test_controller.count_of.move_construction == 0UZ);
}

// Vector::insert(const_iterator pos, T &&value) uses emplace() internally.
TEST_CASE(
    "Vector::insert(const_iterator pos, T &&value) inserts an rvalue",
    "[vector][modifiers][insert]")
{
   auto const initial_size{ 10UZ };
   swtl::Vector<helpers::TrackedObject> vec(initial_size);
   vec.reserve(initial_size + 1UZ);
   helpers::TrackedObject element;

   helpers::g_test_controller.reset();

   vec.insert(vec.cend(), std::move(element));

   REQUIRE(helpers::g_test_controller.count_of.move_construction > 0UZ);
   REQUIRE(helpers::g_test_controller.count_of.copy_construction == 0UZ);
}

TEMPLATE_TEST_CASE(
    "Vector::insert(const_iterator pos, size_type count, T const &value) "
    "inserts `count` instances of `value` before `pos`.",
    "[vector][modifiers][insert]",
    std::uint8_t,
    int,
    helpers::TrackedObject,
    helpers::NoThrowTrackedObject,
    helpers::CopyOnlyTrackedObject)
{
   using T = TestType;

   helpers::g_test_controller.reset();
   {
      auto vec{ helpers::make_vec_of<T>(
          0UZ, 1UZ, 2UZ, 3UZ, 4UZ, 5UZ, 6UZ, 7UZ, 8UZ, 9UZ) };

      auto const reference_element{ T{ 42UZ } };
      auto const count{ 4UZ };

      SECTION("Requesting zero elements does nothing.")
      {
         auto const expected{ vec };

         auto ret_val{ vec.insert(vec.cbegin(), 0UZ, reference_element) };

         REQUIRE(vec == expected);
         REQUIRE(ret_val == vec.begin());
      }

      SECTION(
          "Insertion when elements fit inside the existing initialized "
          "range.")
      {
         vec.reserve(20UZ);
         auto const pos{ 3UZ };

         auto const expected{ helpers::make_vec_of<T>(
             0UZ,
             1UZ,
             2UZ,
             42UZ,
             42UZ,
             42UZ,
             42UZ,
             3UZ,
             4UZ,
             5UZ,
             6UZ,
             7UZ,
             8UZ,
             9UZ) };

         auto ret_val{ vec.insert(
             vec.cbegin() + pos, count, reference_element) };

         REQUIRE(vec == expected);
         REQUIRE(ret_val == vec.begin() + pos);
      }

      SECTION(
          "Insertion when elements are placed outside of the initialized "
          "range.")
      {
         vec.reserve(20UZ);
         auto const pos{ vec.size() - 2UZ };

         auto const expected{ helpers::make_vec_of<T>(
             0UZ,
             1UZ,
             2UZ,
             3UZ,
             4UZ,
             5UZ,
             6UZ,
             7UZ,
             42UZ,
             42UZ,
             42UZ,
             42UZ,
             8UZ,
             9UZ) };

         auto ret_val{ vec.insert(
             vec.cbegin() + pos, count, reference_element) };

         REQUIRE(vec == expected);
         REQUIRE(ret_val == vec.begin() + pos);
      }

      SECTION("Insertion at the end.")
      {
         vec.reserve(20UZ);
         auto const pos{ vec.size() };

         auto const expected{ helpers::make_vec_of<T>(
             0UZ,
             1UZ,
             2UZ,
             3UZ,
             4UZ,
             5UZ,
             6UZ,
             7UZ,
             8UZ,
             9UZ,
             42UZ,
             42UZ,
             42UZ,
             42UZ) };

         auto ret_val{ vec.insert(
             vec.cbegin() + pos, count, reference_element) };

         REQUIRE(vec == expected);
         REQUIRE(ret_val == vec.begin() + pos);
      }

      SECTION("Resizes as needed.")
      {
         helpers::fill_to_capacity(vec);

         auto const expected{ helpers::make_vec_of<T>(
             42UZ,
             42UZ,
             42UZ,
             42UZ,
             0UZ,
             1UZ,
             2UZ,
             3UZ,
             4UZ,
             5UZ,
             6UZ,
             7UZ,
             8UZ,
             9UZ) };

         auto ret_val{ vec.insert(vec.cbegin(), count, reference_element) };

         REQUIRE(vec == expected);
         REQUIRE(ret_val == vec.begin());
      }
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEMPLATE_TEST_CASE(
    "Vector::insert(const_iterator pos, size_type count, T const &value) "
    "exception safety.",
    "[vector][modifiers][insert][exception]",
    helpers::TrackedObject,
    helpers::CopyOnlyTrackedObject)
{
   using T = TestType;

   helpers::g_test_controller.reset();
   {
      auto vec{ helpers::make_vec_of<T>(
          0UZ, 1UZ, 2UZ, 3UZ, 4UZ, 5UZ, 6UZ, 7UZ, 8UZ, 9UZ) };

      auto const reference_element{ T{ 42UZ } };
      auto const count{ 4UZ };
      swtl::Vector const unmodified{ vec };

      SECTION(
          "Insertion when elements fit inside the existing initialized "
          "range.")
      {
         vec.reserve(20UZ);
         auto const pos{ vec.cbegin() + 3UZ };

         helpers::g_test_controller.enable_throwing();
         helpers::g_test_controller.throw_when.copy_construction
             = helpers::g_test_controller.count_of.copy_construction + count;

         REQUIRE_THROWS_AS(
             vec.insert(pos, count, reference_element), helpers::TestException);

         REQUIRE(vec == unmodified);
      }

      SECTION(
          "Insertion when elements are placed outside of the initialized "
          "range.")
      {
         vec.reserve(20UZ);
         auto const pos{ vec.cend() - 2UZ };

         // Because elements are constructed in uninitialized storage before
         // other elements are shuffled around, one local copy, two successful
         // insertions, and then the third insertion throws.
         auto const expected{ helpers::make_vec_of<T>(
             0UZ, 1UZ, 2UZ, 3UZ, 4UZ, 5UZ, 6UZ, 7UZ, 8UZ, 9UZ, 42UZ, 42UZ) };

         helpers::g_test_controller.enable_throwing();
         helpers::g_test_controller.throw_when.copy_construction
             = helpers::g_test_controller.count_of.copy_construction + count;

         REQUIRE_THROWS_AS(
             vec.insert(pos, count, reference_element), helpers::TestException);

         REQUIRE(vec == expected);
      }

      SECTION("Insertion at the end.")
      {
         vec.reserve(20UZ);
         auto const pos{ vec.cend() };

         helpers::g_test_controller.enable_throwing();
         helpers::g_test_controller.throw_when.copy_construction
             = helpers::g_test_controller.count_of.copy_construction + count;

         REQUIRE_THROWS_AS(
             vec.insert(pos, count, reference_element), helpers::TestException);

         REQUIRE(vec == unmodified);
      }

      SECTION("When resize is triggered.")
      {
         helpers::fill_to_capacity(vec);
         auto const initial_capacity{ vec.capacity() };
         auto const pos{ vec.cbegin() };

         helpers::g_test_controller.enable_throwing();
         helpers::g_test_controller.throw_when.copy_construction
             = helpers::g_test_controller.count_of.copy_construction + count;

         REQUIRE_THROWS_AS(
             vec.insert(pos, count, reference_element), helpers::TestException);

         REQUIRE(vec == unmodified);
         REQUIRE(vec.capacity() == initial_capacity);
      }
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEMPLATE_TEST_CASE(
    "Vector::insert(const_iterator pos, size_type count, T const &value) "
    "exception safety - exhaustive test.",
    "[vector][modifiers][insert][exception][exhaustive]",
    helpers::TrackedObject,
    helpers::CopyOnlyTrackedObject)
{
   using T = TestType;

   bool possibilities_exhausted{ false };
   auto const reference_element{ T{ 42UZ } };
   auto const count{ 4UZ };

   for (auto pos{ 0UZ }; !possibilities_exhausted; ++pos)
   {
      bool clean_pass{ false };

      for (auto ops_count{ 1UZ }; !clean_pass; ++ops_count)
      {
         helpers::g_test_controller.reset();
         {
            swtl::Vector vec{ helpers::generate_vector<T>() };

            helpers::g_test_controller.enable_throwing();
            helpers::g_test_controller.throw_when.total_operations
                = helpers::g_test_controller.count_of.total_operations
                + ops_count;

            try
            {
               if (pos <= vec.size())
               {
                  vec.insert(vec.cbegin() + pos, count, reference_element);
                  clean_pass = true;
               }
               else
               {
                  helpers::g_test_controller.disable_throwing();
                  possibilities_exhausted = true;
                  clean_pass = true;
               }
            }
            catch (helpers::TestException const &)
            {
               REQUIRE(vec.size() <= vec.capacity());
               REQUIRE(
                   helpers::g_test_controller.instances_alive() == vec.size());
            }
         }
         REQUIRE(helpers::g_test_controller.all_instances_destroyed());
      }
   }
}

TEMPLATE_TEST_CASE(
    "Vector::insert(const_iterator pos, InputIterator first, Sentinel last) "
    "inserts elements from the source range into the vector before `pos` for "
    "contiguous iterators.",
    "[vector][modifiers][insert]",
    std::uint8_t,
    int,
    helpers::TrackedObject,
    helpers::NoThrowTrackedObject,
    helpers::CopyOnlyTrackedObject,
    helpers::MoveOnlyTrackedObject)
{
   using T = TestType;

   helpers::g_test_controller.reset();
   {
      swtl::Vector vec{ helpers::generate_vector<T>(10UZ) };

      swtl::Vector source{ helpers::make_vec_of<T>(42UZ, 43UZ, 44UZ, 45UZ) };

      SECTION(
          "Insertion when elements fit inside the existing initialized "
          "range.")
      {
         vec.reserve(20UZ);
         auto const pos{ vec.cbegin() + 3UZ };

         swtl::Vector const expected{ helpers::make_vec_of<T>(
             0UZ,
             1UZ,
             2UZ,
             42UZ,
             43UZ,
             44UZ,
             45UZ,
             3UZ,
             4UZ,
             5UZ,
             6UZ,
             7UZ,
             8UZ,
             9UZ) };

         if constexpr (!std::is_move_constructible_v<T>)
         {
            vec.insert(pos, source.begin(), source.end());
         }
         else
         {
            vec.insert(
                pos,
                std::make_move_iterator(source.begin()),
                std::make_move_iterator(source.end()));
         }

         REQUIRE(vec == expected);
      }

      SECTION(
          "Insertion when elements are placed outside of the initialized "
          "range.")
      {
         vec.reserve(20UZ);
         auto const pos{ vec.cend() - 2UZ };

         swtl::Vector const expected{ helpers::make_vec_of<T>(
             0UZ,
             1UZ,
             2UZ,
             3UZ,
             4UZ,
             5UZ,
             6UZ,
             7UZ,
             42UZ,
             43UZ,
             44UZ,
             45UZ,
             8UZ,
             9UZ) };

         if constexpr (!std::is_move_constructible_v<T>)
         {
            vec.insert(pos, source.begin(), source.end());
         }
         else
         {
            vec.insert(
                pos,
                std::make_move_iterator(source.begin()),
                std::make_move_iterator(source.end()));
         }

         REQUIRE(vec == expected);
      }

      SECTION("Insertion at the end.")
      {
         vec.reserve(20UZ);
         auto const pos{ vec.cend() };

         swtl::Vector const expected{ helpers::make_vec_of<T>(
             0UZ,
             1UZ,
             2UZ,
             3UZ,
             4UZ,
             5UZ,
             6UZ,
             7UZ,
             8UZ,
             9UZ,
             42UZ,
             43UZ,
             44UZ,
             45UZ) };

         if constexpr (!std::is_move_constructible_v<T>)
         {
            vec.insert(pos, source.begin(), source.end());
         }
         else
         {
            vec.insert(
                pos,
                std::make_move_iterator(source.begin()),
                std::make_move_iterator(source.end()));
         }

         REQUIRE(vec == expected);
      }

      SECTION("Resizes as needed.")
      {
         helpers::fill_to_capacity(vec);

         auto const pos{ vec.cbegin() };

         swtl::Vector const expected{ helpers::make_vec_of<T>(
             42UZ,
             43UZ,
             44UZ,
             45UZ,
             0UZ,
             1UZ,
             2UZ,
             3UZ,
             4UZ,
             5UZ,
             6UZ,
             7UZ,
             8UZ,
             9UZ) };

         if constexpr (!std::is_move_constructible_v<T>)
         {
            vec.insert(pos, source.begin(), source.end());
         }
         else
         {
            vec.insert(
                pos,
                std::make_move_iterator(source.begin()),
                std::make_move_iterator(source.end()));
         }

         for (auto const &[left, right] : std::views::zip(vec, expected))
         {
            REQUIRE(left == right);
         }
      }
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEMPLATE_TEST_CASE(
    "Vector::insert(const_iterator pos, InputIterator first, Sentinel last) "
    "exception safety for contiguous iterators.",
    "[vector][modifiers][insert]",
    helpers::TrackedObject,
    helpers::CopyOnlyTrackedObject,
    helpers::MoveOnlyTrackedObject)
{
   using T = TestType;

   helpers::g_test_controller.reset();
   {
      swtl::Vector vec{ helpers::generate_vector<T>(10UZ) };

      swtl::Vector source{ helpers::make_vec_of<T>(42UZ, 43UZ, 44UZ, 45UZ) };

      helpers::g_test_controller.enable_throwing();

      SECTION(
          "Insertion when elements fit inside the existing initialized "
          "range.")
      {
         vec.reserve(20UZ);
         auto const pos{ vec.cbegin() + 3UZ };

         if constexpr (!std::is_move_constructible_v<T>)
         {
            helpers::g_test_controller.throw_when.copy_assignment
                = helpers::g_test_controller.count_of.copy_assignment
                + source.size();

            REQUIRE_THROWS_AS(
                vec.insert(pos, source.begin(), source.end()),
                helpers::TestException);
         }
         else
         {
            helpers::g_test_controller.throw_when.move_assignment
                = helpers::g_test_controller.count_of.move_assignment
                + source.size();

            REQUIRE_THROWS_AS(
                vec.insert(
                    pos,
                    std::make_move_iterator(source.begin()),
                    std::make_move_iterator(source.end())),
                helpers::TestException);
         }
      }

      SECTION(
          "Insertion when elements are placed outside of the initialized "
          "range.")
      {
         vec.reserve(20UZ);
         auto const pos{ vec.cend() - 2UZ };

         if constexpr (!std::is_move_constructible_v<T>)
         {
            helpers::g_test_controller.throw_when.copy_construction
                = helpers::g_test_controller.count_of.copy_construction
                + source.size() - std::ranges::distance(pos, vec.cend());

            REQUIRE_THROWS_AS(
                vec.insert(pos, source.begin(), source.end()),
                helpers::TestException);
         }
         else
         {
            helpers::g_test_controller.throw_when.move_construction
                = helpers::g_test_controller.count_of.move_construction
                + source.size() - std::ranges::distance(pos, vec.cend());

            REQUIRE_THROWS_AS(
                vec.insert(
                    pos,
                    std::make_move_iterator(source.begin()),
                    std::make_move_iterator(source.end())),
                helpers::TestException);
         }
      }

      SECTION("Insertion at the end.")
      {
         vec.reserve(20UZ);
         auto const pos{ vec.cend() };

         if constexpr (!std::is_move_constructible_v<T>)
         {
            helpers::g_test_controller.throw_when.copy_construction
                = helpers::g_test_controller.count_of.copy_construction
                + source.size();

            REQUIRE_THROWS_AS(
                vec.insert(pos, source.begin(), source.end()),
                helpers::TestException);
         }
         else
         {
            helpers::g_test_controller.throw_when.move_construction
                = helpers::g_test_controller.count_of.move_construction
                + source.size();

            REQUIRE_THROWS_AS(
                vec.insert(
                    pos,
                    std::make_move_iterator(source.begin()),
                    std::make_move_iterator(source.end())),
                helpers::TestException);
         }
      }

      SECTION("Resizes as needed.")
      {
         helpers::fill_to_capacity(vec);

         auto const pos{ vec.cbegin() };

         if constexpr (!std::is_move_constructible_v<T>)
         {
            helpers::g_test_controller.throw_when.copy_construction
                = helpers::g_test_controller.count_of.copy_construction
                + source.size();

            REQUIRE_THROWS_AS(
                vec.insert(pos, source.begin(), source.end()),
                helpers::TestException);
         }
         else
         {
            helpers::g_test_controller.throw_when.move_construction
                = helpers::g_test_controller.count_of.move_construction
                + source.size();

            REQUIRE_THROWS_AS(
                vec.insert(
                    pos,
                    std::make_move_iterator(source.begin()),
                    std::make_move_iterator(source.end())),
                helpers::TestException);
         }
      }
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEMPLATE_TEST_CASE(
    "Vector::insert(const_iterator pos, InputIterator first, Sentinel last) "
    "exception safety for contiguous iterators - exhaustive test.",
    "[vector][modifiers][insert][exception][exhaustive]",
    helpers::TrackedObject,
    helpers::CopyOnlyTrackedObject,
    helpers::MoveOnlyTrackedObject)
{
   using T = TestType;

   bool possibilities_exhausted{ false };

   for (auto pos{ 0UZ }; !possibilities_exhausted; ++pos)
   {
      bool clean_pass{ false };

      for (auto ops_count{ 1UZ }; !clean_pass; ++ops_count)
      {
         helpers::g_test_controller.reset();
         {
            swtl::Vector vec{ helpers::generate_vector<T>(10UZ) };

            swtl::Vector source{ helpers::make_vec_of<T>(
                42UZ, 43UZ, 44UZ, 45UZ) };

            helpers::g_test_controller.enable_throwing();
            helpers::g_test_controller.throw_when.total_operations
                = helpers::g_test_controller.count_of.total_operations
                + ops_count;

            try
            {
               if (pos <= vec.size())
               {
                  if constexpr (!std::is_move_constructible_v<T>)
                  {
                     vec.insert(
                         vec.cbegin() + pos, source.begin(), source.end());
                  }
                  else
                  {
                     vec.insert(
                         vec.cbegin() + pos,
                         std::make_move_iterator(source.begin()),
                         std::make_move_iterator(source.end()));
                  }

                  clean_pass = true;
               }
               else
               {
                  helpers::g_test_controller.disable_throwing();
                  possibilities_exhausted = true;
                  clean_pass = true;
               }
            }
            catch (helpers::TestException const &)
            {
               REQUIRE(vec.size() <= vec.capacity());
               REQUIRE(
                   helpers::g_test_controller.instances_alive()
                   == vec.size() + source.size());
            }
         }
         REQUIRE(helpers::g_test_controller.all_instances_destroyed());
      }
   }
}

TEMPLATE_TEST_CASE(
    "Vector::insert(const_iterator pos, InputIterator first, Sentinel last) "
    "inserts elements from the source range into the vector before `pos` for "
    "input iterators.",
    "[vector][modifiers][insert]",
    std::uint8_t,
    int,
    helpers::TrackedObject,
    helpers::NoThrowTrackedObject,
    helpers::CopyOnlyTrackedObject,
    helpers::MoveOnlyTrackedObject)
{
   using T = TestType;

   helpers::g_test_controller.reset();
   {
      swtl::Vector vec{ helpers::generate_vector<T>(10UZ) };

      swtl::Vector source{ helpers::make_vec_of<T>(42UZ, 43UZ, 44UZ, 45UZ) };

      helpers::InputIterator<T> begin{ source.data() };
      helpers::InputIterator<T> end{ source.data() + source.size() };

      SECTION(
          "Insertion when elements fit inside the existing initialized "
          "range.")
      {
         vec.reserve(20UZ);
         auto const pos{ vec.cbegin() + 3UZ };

         swtl::Vector const expected{ helpers::make_vec_of<T>(
             0UZ,
             1UZ,
             2UZ,
             42UZ,
             43UZ,
             44UZ,
             45UZ,
             3UZ,
             4UZ,
             5UZ,
             6UZ,
             7UZ,
             8UZ,
             9UZ) };

         if constexpr (!std::is_move_constructible_v<T>)
         {
            vec.insert(pos, begin, end);
         }
         else
         {
            vec.insert(
                pos,
                std::make_move_iterator(begin),
                std::make_move_iterator(end));
         }

         REQUIRE(vec == expected);
      }

      SECTION(
          "Insertion when elements are placed outside of the initialized "
          "range.")
      {
         vec.reserve(20UZ);
         auto const pos{ vec.cend() - 2UZ };

         swtl::Vector const expected{ helpers::make_vec_of<T>(
             0UZ,
             1UZ,
             2UZ,
             3UZ,
             4UZ,
             5UZ,
             6UZ,
             7UZ,
             42UZ,
             43UZ,
             44UZ,
             45UZ,
             8UZ,
             9UZ) };

         if constexpr (!std::is_move_constructible_v<T>)
         {
            vec.insert(pos, begin, end);
         }
         else
         {
            vec.insert(
                pos,
                std::make_move_iterator(begin),
                std::make_move_iterator(end));
         }

         REQUIRE(vec == expected);
      }

      SECTION("Insertion at the end.")
      {
         vec.reserve(20UZ);
         auto const pos{ vec.cend() };

         swtl::Vector const expected{ helpers::make_vec_of<T>(
             0UZ,
             1UZ,
             2UZ,
             3UZ,
             4UZ,
             5UZ,
             6UZ,
             7UZ,
             8UZ,
             9UZ,
             42UZ,
             43UZ,
             44UZ,
             45UZ) };

         if constexpr (!std::is_move_constructible_v<T>)
         {
            vec.insert(pos, begin, end);
         }
         else
         {
            vec.insert(
                pos,
                std::make_move_iterator(begin),
                std::make_move_iterator(end));
         }

         REQUIRE(vec == expected);
      }

      SECTION("Resizes as needed.")
      {
         helpers::fill_to_capacity(vec);

         auto const pos{ vec.cbegin() };

         swtl::Vector const expected{ helpers::make_vec_of<T>(
             42UZ,
             43UZ,
             44UZ,
             45UZ,
             0UZ,
             1UZ,
             2UZ,
             3UZ,
             4UZ,
             5UZ,
             6UZ,
             7UZ,
             8UZ,
             9UZ) };

         if constexpr (!std::is_move_constructible_v<T>)
         {
            vec.insert(pos, begin, end);
         }
         else
         {
            vec.insert(
                pos,
                std::make_move_iterator(begin),
                std::make_move_iterator(end));
         }

         for (auto const &[left, right] : std::views::zip(vec, expected))
         {
            REQUIRE(left == right);
         }
      }
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEMPLATE_TEST_CASE(
    "Vector::insert(const_iterator pos, InputIterator first, Sentinel last) "
    "exception safety for input iterators.",
    "[vector][modifiers][insert][exception]",
    helpers::TrackedObject,
    helpers::CopyOnlyTrackedObject,
    helpers::MoveOnlyTrackedObject)
{
   using T = TestType;

   helpers::g_test_controller.reset();
   {
      swtl::Vector vec{ helpers::generate_vector<T>(10UZ) };

      swtl::Vector source{ helpers::make_vec_of<T>(42UZ, 43UZ, 44UZ, 45UZ) };

      helpers::InputIterator<T> begin{ source.data() };
      helpers::InputIterator<T> end{ source.data() + source.size() };

      helpers::g_test_controller.enable_throwing();

      SECTION(
          "Insertion when elements fit inside the existing initialized "
          "range.")
      {
         vec.reserve(20UZ);
         auto const pos{ vec.cbegin() + 3UZ };

         if constexpr (!std::is_move_constructible_v<T>)
         {
            helpers::g_test_controller.throw_when.copy_assignment
                = helpers::g_test_controller.count_of.copy_assignment
                + source.size();

            REQUIRE_THROWS_AS(
                vec.insert(pos, begin, end), helpers::TestException);
         }
         else
         {
            helpers::g_test_controller.throw_when.move_assignment
                = helpers::g_test_controller.count_of.move_assignment
                + source.size();

            REQUIRE_THROWS_AS(
                vec.insert(
                    pos,
                    std::make_move_iterator(begin),
                    std::make_move_iterator(end)),
                helpers::TestException);
         }
      }

      SECTION(
          "Insertion when elements are placed outside of the initialized "
          "range.")
      {
         vec.reserve(20UZ);
         auto const pos{ vec.cend() - 2UZ };

         if constexpr (!std::is_move_constructible_v<T>)
         {
            helpers::g_test_controller.throw_when.copy_construction
                = helpers::g_test_controller.count_of.copy_construction
                + source.size() - std::ranges::distance(pos, vec.cend());

            REQUIRE_THROWS_AS(
                vec.insert(pos, begin, end), helpers::TestException);
         }
         else
         {
            helpers::g_test_controller.throw_when.move_construction
                = helpers::g_test_controller.count_of.move_construction
                + source.size() - std::ranges::distance(pos, vec.cend());

            REQUIRE_THROWS_AS(
                vec.insert(
                    pos,
                    std::make_move_iterator(begin),
                    std::make_move_iterator(end)),
                helpers::TestException);
         }
      }

      SECTION("Insertion at the end.")
      {
         vec.reserve(20UZ);
         auto const pos{ vec.cend() };

         if constexpr (!std::is_move_constructible_v<T>)
         {
            helpers::g_test_controller.throw_when.copy_construction
                = helpers::g_test_controller.count_of.copy_construction
                + source.size();

            REQUIRE_THROWS_AS(
                vec.insert(pos, begin, end), helpers::TestException);
         }
         else
         {
            helpers::g_test_controller.throw_when.move_construction
                = helpers::g_test_controller.count_of.move_construction
                + source.size();

            REQUIRE_THROWS_AS(
                vec.insert(
                    pos,
                    std::make_move_iterator(begin),
                    std::make_move_iterator(end)),
                helpers::TestException);
         }
      }

      SECTION("Resizes as needed.")
      {
         helpers::fill_to_capacity(vec);

         auto const pos{ vec.cbegin() };

         if constexpr (!std::is_move_constructible_v<T>)
         {
            helpers::g_test_controller.throw_when.copy_construction
                = helpers::g_test_controller.count_of.copy_construction
                + source.size();

            REQUIRE_THROWS_AS(
                vec.insert(pos, begin, end), helpers::TestException);
         }
         else
         {
            helpers::g_test_controller.throw_when.move_construction
                = helpers::g_test_controller.count_of.move_construction
                + source.size();

            REQUIRE_THROWS_AS(
                vec.insert(
                    pos,
                    std::make_move_iterator(begin),
                    std::make_move_iterator(end)),
                helpers::TestException);
         }
      }
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEMPLATE_TEST_CASE(
    "Vector::insert(const_iterator pos, InputIterator first, Sentinel last) "
    "exception safety for input iterators - exhaustive test.",
    "[vector][modifiers][insert][exception][exhaustive]",
    helpers::TrackedObject,
    helpers::CopyOnlyTrackedObject,
    helpers::MoveOnlyTrackedObject)
{
   using T = TestType;

   bool possibilities_exhausted{ false };

   for (auto pos{ 0UZ }; !possibilities_exhausted; ++pos)
   {
      bool clean_pass{ false };

      for (auto ops_count{ 1UZ }; !clean_pass; ++ops_count)
      {
         helpers::g_test_controller.reset();
         {
            swtl::Vector vec{ helpers::generate_vector<T>(10UZ) };

            swtl::Vector source{ helpers::make_vec_of<T>(
                42UZ, 43UZ, 44UZ, 45UZ) };

            helpers::InputIterator<T> begin{ source.data() };
            helpers::InputIterator<T> end{ source.data() + source.size() };

            helpers::g_test_controller.enable_throwing();
            helpers::g_test_controller.throw_when.total_operations
                = helpers::g_test_controller.count_of.total_operations
                + ops_count;

            try
            {
               if (pos <= vec.size())
               {
                  if constexpr (!std::is_move_constructible_v<T>)
                  {
                     vec.insert(vec.cbegin() + pos, begin, end);
                  }
                  else
                  {
                     vec.insert(
                         vec.cbegin() + pos,
                         std::make_move_iterator(begin),
                         std::make_move_iterator(end));
                  }

                  clean_pass = true;
               }
               else
               {
                  helpers::g_test_controller.disable_throwing();
                  possibilities_exhausted = true;
                  clean_pass = true;
               }
            }
            catch (helpers::TestException const &)
            {
               REQUIRE(vec.size() <= vec.capacity());
               REQUIRE(
                   helpers::g_test_controller.instances_alive()
                   == vec.size() + source.size());
            }
         }
         REQUIRE(helpers::g_test_controller.all_instances_destroyed());
      }
   }
}

// Vector::insert(const_iterator pos, std::initializer_list init_list) uses
// insert(const_iterator pos, InputIterator first, Sentinel last) internally.
TEMPLATE_TEST_CASE(
    "Vector::insert(const_iterator pos, std::initializer_list init_list) "
    "inserts the elements from the initializer list before `pos`.",
    "[vector][insert]",
    std::uint8_t,
    int,
    helpers::TrackedObject,
    helpers::NoThrowTrackedObject)
{
   using T = TestType;

   helpers::g_test_controller.reset();
   {
      auto vec{ helpers::make_vec_of<T>(
          0UZ, 1UZ, 2UZ, 3UZ, 4UZ, 5UZ, 6UZ, 7UZ, 8UZ, 9UZ) };

      std::initializer_list const init_list{
         T{ 42UZ }, T{ 43UZ }, T{ 44UZ }, T{ 45UZ }
      };

      auto const expected{ helpers::make_vec_of<T>(
          42UZ,
          43UZ,
          44UZ,
          45UZ,
          0UZ,
          1UZ,
          2UZ,
          3UZ,
          4UZ,
          5UZ,
          6UZ,
          7UZ,
          8UZ,
          9UZ) };

      auto const pos{ vec.cbegin() };

      vec.insert(pos, init_list);

      REQUIRE(vec == expected);
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

// Vector::insert(const_iterator pos, Range &&range) uses
// insert(const_iterator pos, InputIterator first, Sentinel last) internally.
TEMPLATE_TEST_CASE(
    "Vector::insert(const_iterator pos, Range &&range) inserts the elements "
    "from the source range before `pos`.",
    "[vector][modifiers][insert]",
    std::uint8_t,
    int,
    helpers::TrackedObject,
    helpers::NoThrowTrackedObject)
{
   using T = TestType;

   helpers::g_test_controller.reset();
   {
      auto vec{ helpers::make_vec_of<T>(
          0UZ, 1UZ, 2UZ, 3UZ, 4UZ, 5UZ, 6UZ, 7UZ, 8UZ, 9UZ) };

      auto const source{ helpers::make_vec_of<T>(42UZ, 43UZ, 44UZ, 45UZ) };

      auto const expected{ helpers::make_vec_of<T>(
          42UZ,
          43UZ,
          44UZ,
          45UZ,
          0UZ,
          1UZ,
          2UZ,
          3UZ,
          4UZ,
          5UZ,
          6UZ,
          7UZ,
          8UZ,
          9UZ) };

      auto const pos{ vec.cbegin() };

      vec.insert_range(pos, source);

      REQUIRE(vec == expected);
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEMPLATE_TEST_CASE(
    "Vector::erase(const_iterator pos) removes the element at pos and returns "
    "an iterator to the next element.",
    "[vector][modifiers][erase]",
    std::uint8_t,
    int,
    helpers::TrackedObject,
    helpers::NoThrowTrackedObject,
    helpers::CopyOnlyTrackedObject,
    helpers::MoveOnlyTrackedObject)
{
   using T = TestType;

   helpers::g_test_controller.reset();
   {
      auto vec{ helpers::generate_vector<T>() };

      SECTION("Erase removes the element at begin.")
      {
         auto const expected{ helpers::make_vec_of<T>(
             1UZ, 2UZ, 3UZ, 4UZ, 5UZ, 6UZ, 7UZ) };

         auto ret_val{ vec.erase(vec.cbegin()) };

         REQUIRE(vec == expected);
         REQUIRE(ret_val == vec.begin());
      }

      SECTION("Erase removes the element at end.")
      {
         auto const expected{ helpers::make_vec_of<T>(
             0UZ, 1UZ, 2UZ, 3UZ, 4UZ, 5UZ, 6UZ) };

         auto ret_val{ vec.erase(vec.cend() - 1) };

         REQUIRE(vec == expected);
         REQUIRE(ret_val == vec.end());
      }

      SECTION("Erase removes an element in the middle.")
      {
         auto const expected{ helpers::make_vec_of<T>(
             0UZ, 1UZ, 2UZ, 4UZ, 5UZ, 6UZ, 7UZ) };

         auto const element_idx{ 3UZ };

         auto ret_val{ vec.erase(vec.cbegin() + element_idx) };

         REQUIRE(vec == expected);
         REQUIRE(ret_val == vec.begin() + element_idx);
      }
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEMPLATE_TEST_CASE(
    "Vector::erase(const_iterator pos) exception safety - exhaustive test.",
    "[vector][modifiers][erase][exception][exhaustive]",
    helpers::TrackedObject,
    helpers::CopyOnlyTrackedObject,
    helpers::MoveOnlyTrackedObject)
{
   using T = TestType;

   bool possibilities_exhausted{ false };

   for (auto pos{ 0UZ }; !possibilities_exhausted; ++pos)
   {
      bool clean_pass{ false };

      for (auto ops_count{ 1UZ }; !clean_pass; ++ops_count)
      {
         helpers::g_test_controller.reset();
         {
            swtl::Vector vec{ helpers::generate_vector<T>() };

            helpers::g_test_controller.enable_throwing();
            helpers::g_test_controller.throw_when.total_operations
                = helpers::g_test_controller.count_of.total_operations
                + ops_count;

            try
            {
               if (pos <= vec.size())
               {
                  vec.erase(vec.cbegin() + pos);

                  clean_pass = true;
               }
               else
               {
                  helpers::g_test_controller.disable_throwing();
                  possibilities_exhausted = true;
                  clean_pass = true;
               }
            }
            catch (helpers::TestException const &)
            {
               REQUIRE(vec.size() <= vec.capacity());
               REQUIRE(
                   helpers::g_test_controller.instances_alive() == vec.size());
            }
         }
         REQUIRE(helpers::g_test_controller.all_instances_destroyed());
      }
   }
}

TEMPLATE_TEST_CASE(
    "Vector::erase(const_iterator first, const_iterator last) removes elements "
    "in the specified range and returns an iterator to the next element.",
    "[vector][modifiers][erase]",
    std::uint8_t,
    int,
    helpers::TrackedObject,
    helpers::NoThrowTrackedObject,
    helpers::CopyOnlyTrackedObject,
    helpers::MoveOnlyTrackedObject)
{
   using T = TestType;

   helpers::g_test_controller.reset();
   {
      auto vec{ helpers::generate_vector<T>() };

      SECTION("Erase removes elements from the beginning.")
      {
         auto const expected{ helpers::make_vec_of<T>(
             3UZ, 4UZ, 5UZ, 6UZ, 7UZ) };

         auto ret_val{ vec.erase(vec.cbegin(), vec.cbegin() + 3UZ) };

         REQUIRE(vec == expected);
         REQUIRE(ret_val == vec.begin());
      }

      SECTION("Erase removes elements at the end.")
      {
         auto const expected{ helpers::make_vec_of<T>(0UZ, 1UZ, 2UZ, 3UZ) };

         auto ret_val{ vec.erase(vec.cbegin() + 4UZ, vec.cend()) };

         REQUIRE(vec == expected);
         REQUIRE(ret_val == vec.end());
      }

      SECTION("Erase removes elements from the middle.")
      {
         auto const expected{ helpers::make_vec_of<T>(
             0UZ, 1UZ, 2UZ, 5UZ, 6UZ, 7UZ) };

         auto const first_distance{ 3UZ };
         auto const last_distance{ 5UZ };

         auto const first{ vec.cbegin() + first_distance };
         auto const last{ vec.cbegin() + last_distance };

         auto ret_val{ vec.erase(first, last) };

         REQUIRE(vec == expected);
         REQUIRE(ret_val == vec.begin() + first_distance);
      }
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEMPLATE_TEST_CASE(
    "Vector::erase(const_iterator first, const_iterator last) exception safety "
    "- exhaustive test.",
    "[vector][modifiers][erase][exception][exhaustive]",
    helpers::TrackedObject,
    helpers::CopyOnlyTrackedObject,
    helpers::MoveOnlyTrackedObject)
{
   using T = TestType;

   bool first_range_exhausted{ false };

   for (auto first_pos{ 0UZ }; !first_range_exhausted; ++first_pos)
   {
      bool last_range_exhausted{ false };

      for (auto last_pos{ first_pos }; !last_range_exhausted; ++last_pos)
      {
         bool clean_pass{ false };

         for (auto ops_count{ 1UZ }; !clean_pass; ++ops_count)
         {
            helpers::g_test_controller.reset();
            {
               swtl::Vector vec{ helpers::generate_vector<T>() };
               auto const limit{ vec.size() };

               helpers::g_test_controller.enable_throwing();
               helpers::g_test_controller.throw_when.total_operations
                   = helpers::g_test_controller.count_of.total_operations
                   + ops_count;

               try
               {
                  if (first_pos < limit && last_pos <= limit)
                  {
                     vec.erase(
                         vec.cbegin() + first_pos, vec.cbegin() + last_pos);

                     clean_pass = true;
                  }
                  else
                  {
                     helpers::g_test_controller.disable_throwing();

                     if (first_pos == limit)
                     {
                        first_range_exhausted = true;
                     }

                     if (last_pos > limit)
                     {
                        last_range_exhausted = true;
                     }

                     clean_pass = true;
                  }
               }
               catch (helpers::TestException const &)
               {
                  REQUIRE(vec.size() <= vec.capacity());
                  REQUIRE(
                      helpers::g_test_controller.instances_alive()
                      == vec.size());
               }
            }
            REQUIRE(helpers::g_test_controller.all_instances_destroyed());
         }
      }
   }
}

/* Test template.

TEMPLATE_TEST_CASE(
    "Vector::BLAH-BLAH-BLAH",
    "[vector][]",
    std::uint8_t,
    int,
    helpers::TrackedObject,
    helpers::NoThrowTrackedObject,
    helpers::CopyOnlyTrackedObject,
    helpers::MoveOnlyTrackedObject)
{
   using T = TestType;

   helpers::g_test_controller.reset();
   {
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

*/
