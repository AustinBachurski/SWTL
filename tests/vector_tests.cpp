/*  Default set of template test case types - remove after refactor is done.

    std::uint8_t,
    int,
    helpers::TrackedObject,
    helpers::NoThrowTrackedObject,
    helpers::CopyOnlyTrackedObject,
    helpers::MoveOnlyTrackedObject)

   helpers::g_test_controller.reset();
   {
      // TODO: WORKING HERE NEXT
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());

*/

#include "catch2/catch_template_test_macros.hpp"
#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers.hpp"
#include "catch2/matchers/catch_matchers_exception.hpp"
#include "catch2/matchers/catch_matchers_string.hpp"

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
   helpers::g_test_controller.reset();
   {
      using T = TestType;

      swtl::Vector<T> const vec{ T{ 0 }, T{ 1 }, T{ 2 }, T{ 3 }, T{ 4 },
                                 T{ 5 }, T{ 6 }, T{ 7 }, T{ 8 }, T{ 9 } };

      std::initializer_list<T> const init_list{ T{ 0 }, T{ 1 }, T{ 2 }, T{ 3 },
                                                T{ 4 }, T{ 5 }, T{ 6 }, T{ 7 },
                                                T{ 8 }, T{ 9 } };
      REQUIRE(std::ranges::equal(vec, init_list));
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEST_CASE(
    "Vector{braced-initializer-list} exception safety.",
    "[vector][constructor][exception]")
{
   helpers::g_test_controller.reset();
   {
      using T = helpers::TrackedObject;

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
   helpers::g_test_controller.reset();
   {
      using T = TestType;

      std::initializer_list<T> const init_list{ T{ 0 }, T{ 1 }, T{ 2 }, T{ 3 },
                                                T{ 4 }, T{ 5 }, T{ 6 }, T{ 7 },
                                                T{ 8 }, T{ 9 } };
      swtl::Vector<T> const vec(init_list);

      REQUIRE(std::ranges::equal(vec, init_list));
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEST_CASE(
    "Vector(std::initializer_list init_list) exception safety.",
    "[vector][constructor][exception]")
{
   helpers::g_test_controller.reset();
   {
      using T = helpers::TrackedObject;

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
   helpers::g_test_controller.reset();
   {
      using T = TestType;

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
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEST_CASE(
    "Vector(size_type count) exception safety.",
    "[vector][constructor][exception]")
{
   helpers::g_test_controller.reset();
   {
      using T = helpers::TrackedObject;

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
   helpers::g_test_controller.reset();
   {
      using T = TestType;

      auto const count{ 4UZ };
      auto const reference_value{ T{ 42UZ } };

      swtl::Vector<TestType> const vec(count, reference_value);
      swtl::Vector<TestType> expected;
      expected.assign(count, reference_value);

      REQUIRE(vec == expected);
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEST_CASE(
    "Vector(size_type count, T const &value) exception safety.",
    "[vector][constructor][exception]")
{
   helpers::g_test_controller.reset();
   {
      using T = helpers::TrackedObject;

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
   helpers::g_test_controller.reset();
   {
      auto source{ helpers::generate_vector<TestType>() };

      if constexpr (!std::is_move_constructible_v<TestType>)
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
   helpers::g_test_controller.reset();
   {
      auto source{ helpers::generate_vector<TestType>() };

      auto const count{ source.size() };

      helpers::g_test_controller.enable_throwing();
      helpers::g_test_controller.throw_when.copy_construction
          = helpers::g_test_controller.count_of.copy_construction + count;
      helpers::g_test_controller.throw_when.move_construction
          = helpers::g_test_controller.count_of.move_construction + count;

      if constexpr (!std::is_move_constructible_v<TestType>)
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
   helpers::g_test_controller.reset();
   {
      auto source{ helpers::generate_vector<TestType>() };

      auto begin{ helpers::InputIterator(source.data()) };
      auto end{ helpers::InputIterator(source.data() + source.size()) };

      if constexpr (!std::is_move_constructible_v<TestType>)
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
   helpers::g_test_controller.reset();
   {
      auto source{ helpers::generate_vector<TestType>() };

      auto const count{ source.size() };

      auto begin{ helpers::InputIterator(source.data()) };
      auto end{ helpers::InputIterator(source.data() + source.size()) };

      helpers::g_test_controller.enable_throwing();
      helpers::g_test_controller.throw_when.copy_construction
          = helpers::g_test_controller.count_of.copy_construction + count;
      helpers::g_test_controller.throw_when.move_construction
          = helpers::g_test_controller.count_of.move_construction + count;

      if constexpr (!std::is_move_constructible_v<TestType>)
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
   helpers::g_test_controller.reset();
   {
      auto source{ helpers::generate_vector<TestType>() };

      if constexpr (!std::is_move_constructible_v<TestType>)
      {
         swtl::Vector const vec(std::from_range, source);
         REQUIRE(vec == source);
      }
      else
      {
         swtl::Vector const vec(std::from_range, std::views::as_rvalue(source));
         REQUIRE(vec == source);
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
   helpers::g_test_controller.reset();
   {
      auto source{ helpers::generate_vector<TestType>() };

      auto const count{ source.size() };

      helpers::g_test_controller.enable_throwing();
      helpers::g_test_controller.throw_when.copy_construction
          = helpers::g_test_controller.count_of.copy_construction + count;
      helpers::g_test_controller.throw_when.move_construction
          = helpers::g_test_controller.count_of.move_construction + count;

      if constexpr (!std::is_move_constructible_v<TestType>)
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
   TestType value{};
   swtl::Vector<TestType> source;

   SECTION("CTAD from a braced initializer list.")
   {
      swtl::Vector vec{ value, value, value };

      STATIC_REQUIRE(std::is_same_v<decltype(vec), swtl::Vector<TestType>>);
   }

   SECTION("CTAD from iterators.")
   {
      swtl::Vector vec(source.begin(), source.end());

      STATIC_REQUIRE(std::is_same_v<decltype(vec), swtl::Vector<TestType>>);
   }

   SECTION("CTAD from a range.")
   {
      swtl::Vector vec(std::from_range, source);

      STATIC_REQUIRE(std::is_same_v<decltype(vec), swtl::Vector<TestType>>);
   }
}

TEMPLATE_TEST_CASE(
    "Iterator calls return const correct iterators.",
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
   helpers::g_test_controller.reset();
   {
      auto source{ helpers::generate_vector<TestType>() };
      auto const const_source{ helpers::generate_vector<TestType>() };
      auto const expected{ helpers::generate_vector<TestType>() };

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
         swtl::Vector<TestType> destination;
         destination = source;

         REQUIRE(destination == expected);
         REQUIRE(source == expected);
         REQUIRE(destination.data() != source.data());
      }

      SECTION("Copy assignment operator copies from const source.")
      {
         swtl::Vector<TestType> destination;
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
   helpers::g_test_controller.reset();
   {
      auto source{ helpers::generate_vector<TestType>() };
      auto const count{ source.size() };

      auto const expected{ helpers::generate_vector<TestType>() };

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

         swtl::Vector<TestType> destination;

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
   helpers::g_test_controller.reset();
   {
      auto source{ helpers::generate_vector<TestType>() };
      auto const &const_reference_to_source{ source };

      auto const data_ptr_before_move{ source.data() };
      auto const capacity_before_move{ source.capacity() };
      auto const size_before_move{ source.size() };

      auto const expected{ helpers::generate_vector<TestType>() };

      if constexpr (std::is_move_constructible_v<TestType>)
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

      if constexpr (std::is_move_assignable_v<TestType>)
      {
         SECTION("Move assignment from non-const source moves data.")
         {
            swtl::Vector<TestType> destination;
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

      if constexpr (std::is_copy_constructible_v<TestType>)
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

      if constexpr (std::is_copy_assignable_v<TestType>)
      {
         SECTION("Move assignment from const source falls back to copying.")
         {
            swtl::Vector<TestType> destination;
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

      if constexpr (std::is_move_assignable_v<TestType>)
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
   helpers::g_test_controller.reset();
   {
      auto const initial_count{ 8UZ };
      swtl::Vector<TestType> vec(initial_count);

      SECTION("Assignment triggers reallocation.")
      {
         auto const count{ 10UZ };
         swtl::Vector<TestType> const expected(count);

         vec.assign(count, TestType{});

         REQUIRE(vec == expected);
      }

      SECTION("Assignment to existing memory.")
      {
         auto const count{ 6UZ };
         swtl::Vector<TestType> const expected(count);

         vec.assign(count, TestType{});

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
   helpers::g_test_controller.reset();
   {
      auto const initial_count{ 8UZ };
      swtl::Vector<TestType> vec(initial_count);

      helpers::g_test_controller.enable_throwing();
      SECTION("Assignment triggers reallocation.")
      {
         auto const count{ 10UZ };

         swtl::Vector<TestType> const expected(initial_count);

         helpers::g_test_controller.throw_when.copy_construction
             = helpers::g_test_controller.count_of.copy_construction + count;

         INFO(
             "If count (which was "
             << count << " ) is less than initial_count (which was "
             << initial_count << " ) this test is invalid.");

         REQUIRE_THROWS_AS(
             vec.assign(count, TestType{}), helpers::TestException);

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

         REQUIRE_THROWS_AS(
             vec.assign(count, TestType{}), helpers::TestException);
      }
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
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
   helpers::g_test_controller.reset();
   {
      // TODO: WORKING HERE: Implement test for contiguour iterator, then
      // duplicate and modify for insert iterator.
      auto const source_size{ 10UZ };
      auto const initial_size{ 20UZ };

      swtl::Vector<helpers::TrackedObject> const source(source_size);

      TestType begin{ source.data() };
      TestType end{ source.data() + source.size() };

      helpers::g_test_controller.reset();

      swtl::Vector<helpers::TrackedObject> vec(initial_size);
      vec.assign(begin, end);

      REQUIRE(vec == source);
      REQUIRE(helpers::g_test_controller.instances_alive() == source_size);
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEMPLATE_TEST_CASE(
    "Vector::assign(InputIterator src_begin, Sentinel src_end) assigns "
    "from the source iterator and grows when needed.",
    "[vector][assign]",
    helpers::InputIterator<helpers::TrackedObject const>,
    swtl::ContiguousIterator<helpers::TrackedObject const>)
{
   auto const source_size{ 20UZ };
   auto const initial_size{ 10UZ };

   swtl::Vector<helpers::TrackedObject> const source(source_size);

   TestType begin{ source.data() };
   TestType end{ source.data() + source.size() };

   helpers::g_test_controller.reset();

   swtl::Vector<helpers::TrackedObject> vec(initial_size);
   vec.assign(begin, end);

   REQUIRE(vec == source);
   REQUIRE(helpers::g_test_controller.instances_alive() == source_size);
}

TEST_CASE(
    "Vector::assign(InputIterator src_begin, Sentinel src_end) manages "
    "lifetimes correctly if an exception is thrown with an iterator pair that "
    "can be checked for size.",
    "[vector][assign][exception]")
{
   auto const source_size{ 20UZ };
   auto const initial_size{ 10UZ };

   swtl::Vector<helpers::TrackedObject> const source(source_size);

   helpers::g_test_controller.reset();
   helpers::g_test_controller.enable_throwing();
   helpers::g_test_controller.throw_when.copy_construction = source_size;

   swtl::Vector<helpers::TrackedObject> vec(initial_size);

   REQUIRE_THROWS_AS(
       vec.assign(source.begin(), source.end()), helpers::TestException);
   REQUIRE(helpers::g_test_controller.instances_alive() == initial_size);
}

TEST_CASE(
    "Vector::assign(InputIterator src_begin, Sentinel src_end) manages "
    "lifetimes correctly if an exception is thrown with an iterator pair that "
    "cannot be checked for size.",
    "[vector][assign][exception]")
{
   auto const source_size{ 20UZ };
   auto const initial_size{ 10UZ };

   swtl::Vector<helpers::TrackedObject> const source(source_size);

   helpers::InputIterator<helpers::TrackedObject const> begin{ source.data() };
   helpers::InputIterator<helpers::TrackedObject const> end{ source.data()
                                                             + source.size() };

   helpers::g_test_controller.reset();
   helpers::g_test_controller.enable_throwing();
   helpers::g_test_controller.throw_when.copy_construction = source_size;

   swtl::Vector<helpers::TrackedObject> vec(initial_size);

   REQUIRE_THROWS_AS(vec.assign(begin, end), helpers::TestException);

   // When the size of the input range cannot be determined due to the iterator
   // not supporting operator-, copy assignment will occur until the initial
   // elements of the destination vector are overwritten; after which
   // `push_back` is used.  This means that each element successfully
   // constructed is effectively a "transaction" rather than the whole
   // collection like we'd see if we could calculate the size before hand,
   // allocating new memory and copying everything in before committing the
   // pointers.
   REQUIRE(helpers::g_test_controller.instances_alive() == source_size - 1);
}

TEST_CASE(
    "Vector::assign(std::initializer_list<T> init_list) updates vector with "
    "new data.",
    "[vector][assign]")
{
   std::initializer_list<int> const init_list{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
   swtl::Vector<int> vec(10UZ);

   vec.assign(init_list);

   REQUIRE(std::ranges::equal(vec, init_list));
   // assign(std::initializer_list<T> init_list) calls the iterator helper under
   // the hood, this is well tested as
   // Vector::assign(InputIterator src_begin, Sentinel src_end) above.
}

TEST_CASE(
    "Vector::assign_range(Range &&range) updates vector with new data.",
    "[vector][assign]")
{
   auto const range{ helpers::generate_populated<std::vector<int>>() };
   swtl::Vector<int> vec(10UZ);

   vec.assign_range(range);

   REQUIRE(std::ranges::equal(vec, range));
   // assign(Range &&range) calls the iterator helper under the hood, this is
   // well tested as Vector::assign(InputIterator src_begin, Sentinel src_end)
   // above.
}

// TODO: Add additional allocators via template test case.
TEST_CASE(
    "Vector::get_allocator() returns the correct allocator.",
    "[vector][allocator]")
{
   std::allocator<int> test_alloc;
   swtl::Vector<int, std::allocator<int>> vec(test_alloc);

   REQUIRE(vec.get_allocator() == test_alloc);
}

TEMPLATE_TEST_CASE(
    "Element access, const & non-const.",
    "[vector][accessors]",
    int,
    bool,
    bool const,
    unsigned char,
    unsigned char const,
    int const,
    double,
    double const,
    std::string,
    std::string const)
{
   using T = typename std::remove_const_t<TestType>;
   using ConstCorrectVector = std::conditional_t<
       std::is_const_v<TestType>,
       swtl::Vector<T> const,
       swtl::Vector<T>
   >;
   using ExpectedQualifiedRef
       = std::conditional_t<std::is_const_v<TestType>, T const &, T &>;

   auto const expected{ helpers::generate_populated<std::vector<T>>() };

   ConstCorrectVector vec(expected.begin(), expected.end());

   auto const first_idx{ 0UZ };
   auto const last_idx{ vec.size() - 1 };

   SECTION("Vector::at returns a reference to the element at position.")
   {
      REQUIRE(vec.at(first_idx) == expected[first_idx]);
      REQUIRE(vec.at(last_idx) == expected[last_idx]);
      STATIC_REQUIRE(
          std::is_same_v<decltype(vec.at(first_idx)), ExpectedQualifiedRef>);
   }

   SECTION("Vector::at throws when accessing an element out of bounds.")
   {
      using Catch::Matchers::ContainsSubstring;
      using Catch::Matchers::MessageMatches;

      auto const invalid_index{ vec.size() };

      REQUIRE_THROWS_MATCHES(
          vec.at(invalid_index),
          std::out_of_range,
          MessageMatches(ContainsSubstring(std::to_string(invalid_index)))
              && MessageMatches(ContainsSubstring(std::to_string(vec.size()))));
   }

   SECTION(
       "Vector::operator[] returns a reference to the element at "
       "position.")
   {
      REQUIRE(vec[first_idx] == expected[first_idx]);
      REQUIRE(vec[last_idx] == expected[last_idx]);
      STATIC_REQUIRE(
          std::is_same_v<decltype(vec[first_idx]), ExpectedQualifiedRef>);
   }

   SECTION("Vector::front returns a reference to the first element.")
   {
      REQUIRE(vec.front() == expected.front());
      STATIC_REQUIRE(
          std::is_same_v<decltype(vec.front()), ExpectedQualifiedRef>);
   }

   SECTION("Vector::back returns a reference to the last element.")
   {
      REQUIRE(vec.back() == expected.back());
      STATIC_REQUIRE(
          std::is_same_v<decltype(vec.back()), ExpectedQualifiedRef>);
   }
}

TEST_CASE(
    "Element modification via operator[] and at().", "[vector][accessors]")
{
   swtl::Vector<int> actual{ 1, 4, 3, 4 };
   swtl::Vector<int> const expected{ 1, 2, 3, 4 };

   SECTION("Vector::operator[] modifies correct element.")
   {
      actual[1] = 2;

      REQUIRE(actual == expected);
   }

   SECTION("Vector::at modifies correct element.")
   {
      actual.at(1) = 2;

      REQUIRE(actual == expected);
   }

   SECTION("Vector::data modifies internal data.")
   {
      actual.data()[1] = 2;

      REQUIRE(actual == expected);
   }
}

TEST_CASE(
    "Vector::operator[] triggers a contract violation when accessing out of "
    "bounds access.",
    "[vector][contract]")
{
   auto const out_of_bounds_index{ 100UZ };
   auto const vec{ helpers::generate_populated<swtl::Vector<int>>() };

   INFO(
       "INFO: If `out_of_bounds_index` (which was "
       << out_of_bounds_index
       << ") is not greater than or equal to `vec.size()` (which was "
       << vec.size() << ") this test is invalid.");
   REQUIRE(out_of_bounds_index >= vec.size());

   REQUIRE_THROWS_AS(vec[out_of_bounds_index], ContractException);
}

TEST_CASE("Element modification via front().")
{
   swtl::Vector<int> actual{ 2, 2, 3, 4 };
   swtl::Vector<int> const expected{ 1, 2, 3, 4 };

   actual.front() = 1;

   REQUIRE(actual == expected);
}

TEST_CASE(
    "Vector::front() triggers a contract violation when called on an empty "
    "vector."
    "vector.",
    "[vector][contracts]")
{
   swtl::Vector<int> const vec;

   REQUIRE_THROWS_AS(vec.front(), ContractException);
}

TEST_CASE("Element modification via back().")
{
   swtl::Vector<int> actual{ 1, 2, 3, 5 };
   swtl::Vector<int> const expected{ 1, 2, 3, 4 };

   actual.back() = 4;

   REQUIRE(actual == expected);
}

TEST_CASE(
    "Vector::back() triggers a contract violation when called on an empty "
    "vector.",
    "[vector][contracts]")
{
   swtl::Vector<int> const vec;

   REQUIRE_THROWS_AS(vec.back(), ContractException);
}

TEMPLATE_TEST_CASE(
    "Reservation on an empty vector.",
    "[vector][reserve]",
    bool,
    unsigned char,
    int,
    double,
    std::string)
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
    "Reservation on a populated vector.",
    "[vector][reserve]",
    bool,
    unsigned char,
    int,
    double,
    std::string)
{
   auto vec{ helpers::generate_populated<swtl::Vector<TestType>>() };
   auto const initial_capacity{ vec.capacity() };
   swtl::Vector const expected{ vec };

   // Capacity may be greater than requested due to using allocate_at_least().

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

TEMPLATE_TEST_CASE(
    "Element insertion via push_back.",
    "[vector][insertion]",
    bool,
    unsigned char,
    int,
    double,
    std::string)
{
   swtl::Vector<TestType> vec;
   auto const data{ helpers::generate_populated<std::vector<TestType>>() };

   SECTION("Inserting lvalue references works as expected.")
   {
      for (auto const &element : data)
      {
         vec.push_back(element);
      }

      REQUIRE(std::ranges::equal(vec, data));
   }

   SECTION("Inserting rvalue references works as expected.")
   {
      auto expiring{ data };

      // Using rvalue reference to support std::vector<bool> specialization.
      for (auto &&element : expiring)
      {
         vec.push_back(std::move(element));
      }

      REQUIRE(std::ranges::equal(vec, data));
   }
}

TEMPLATE_TEST_CASE(
    "Element insertion via emplace_back.",
    "[vector][insertion]",
    bool,
    unsigned char,
    int,
    double,
    std::string)
{
   swtl::Vector<TestType> vec;
   auto const data{ helpers::generate_populated<std::vector<TestType>>() };

   SECTION("Inserting lvalue references succeeds.")
   {
      for (auto const &element : data)
      {
         vec.emplace_back(element);
      }

      REQUIRE(std::ranges::equal(vec, data));
   }

   SECTION("Inserting rvalue references succeeds.")
   {
      auto expiring{ data };

      // Using rvalue reference to support std::vector<bool> specialization.
      for (auto &&element : expiring)
      {
         vec.emplace_back(std::move(element));
      }

      REQUIRE(std::ranges::equal(vec, data));
   }
}

TEMPLATE_TEST_CASE(
    "Calling emplace_back() with no arguments default constructs an object.",
    "[vector][insertion]",
    bool,
    unsigned char,
    int,
    double,
    std::string)
{
   swtl::Vector<TestType> vec;
   vec.emplace_back();

   REQUIRE(vec.size() == 1UZ);
   REQUIRE(vec.back() == TestType{});
}

TEST_CASE(
    "Calling emplace_back() with arguments constructs an object using "
    "said arguments.",
    "[vector][insertion]")
{
   struct CustomObject
   {
      int value{};
      std::string string{};

      auto
      operator<=>(CustomObject const &other) const = default;
   };

   auto const int_value{ 42 };
   auto const string_value{ "is the correct answer" };

   swtl::Vector<CustomObject> vec;
   vec.emplace_back(int_value, string_value);

   REQUIRE(vec.back() == CustomObject{ int_value, string_value });
}

TEMPLATE_TEST_CASE(
    "emplace_back() returns a reference to the inserted element.",
    "[vector][insertion]",
    bool,
    unsigned char,
    int,
    double,
    std::string)
{
   swtl::Vector<TestType> vec;
   auto const expected{ helpers::generate_populated<std::vector<TestType>>() };

   for (auto const &element : expected)
   {
      auto const &result{ vec.emplace_back(element) };

      REQUIRE(std::same_as<decltype(result), decltype(element)>);
      REQUIRE(result == element);
      REQUIRE(result == vec.back());
   }
}

TEMPLATE_TEST_CASE(
    "Reallocation growth.",
    "[vector][growth]",
    bool,
    unsigned char,
    int,
    double,
    std::string)
{
   swtl::Vector<TestType> vec(1);

   SECTION("Insertion up to capacity does not trigger growth.")
   {
      auto const initial_capacity{ vec.capacity() };

      while (vec.size() < initial_capacity)
      {
         vec.emplace_back();
      }

      REQUIRE(vec.size() == initial_capacity);
      REQUIRE(vec.capacity() == initial_capacity);

      SECTION("Adding elements beyond capacity at minimum doubles capacity.")
      {
         vec.emplace_back();

         REQUIRE(vec.capacity() >= initial_capacity * 2);

         SECTION("Consistent lack of growth.")
         {
            auto const expanded_capacity{ vec.size() };

            while (vec.size() < expanded_capacity)
            {
               vec.emplace_back();
            }

            REQUIRE(vec.size() == expanded_capacity);
            REQUIRE(vec.capacity() == expanded_capacity);

            SECTION("Consistent growth.")
            vec.emplace_back();

            REQUIRE(vec.capacity() >= expanded_capacity * 2);
         }
      }
   }
}

TEMPLATE_TEST_CASE(
    "Reallocation preserves existing elements.",
    "[vector][growth]",
    bool,
    unsigned char,
    int,
    double,
    std::string)
{
   auto vec{ helpers::generate_populated<swtl::Vector<TestType>>() };
   helpers::fill_to_capacity(vec);
   auto const before_growth{ vec };

   vec.emplace_back();

   for (auto const &[value, expected] : std::views::zip(vec, before_growth))
   {
      REQUIRE(value == expected);
   }
}

TEST_CASE("Reallocation exception safety.", "[vector][growth][exception]")
{
   auto vec{ helpers::generate_unique<swtl::Vector<helpers::TrackedObject>>(
       10UZ) };
   helpers::fill_to_capacity(vec);

   auto const expected{ vec };
   auto const next_instance{ 1UZ };

   helpers::g_test_controller.reset();
   helpers::g_test_controller.enable_throwing();
   helpers::g_test_controller.throw_when.default_construction = next_instance;

   REQUIRE_THROWS_AS(vec.emplace_back(), helpers::TestException);
   REQUIRE(vec == expected);
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEST_CASE(
    "Reallocation copies elements if `T`'s move constructor is not noexcept.",
    "[vector][growth][exception]")
{
   swtl::Vector<helpers::TrackedObject> vec(10UZ);
   helpers::fill_to_capacity(vec);

   auto const expected{ vec.size() + 1 };
   helpers::g_test_controller.reset();

   REQUIRE_NOTHROW(vec.emplace_back());
   REQUIRE(vec.size() == expected);
   REQUIRE(helpers::g_test_controller.count_of.copy_construction > 0UZ);
   REQUIRE(helpers::g_test_controller.count_of.move_construction == 0UZ);
}

TEST_CASE(
    "Reallocation copies elements if `T` is not movable.",
    "[vector][growth][exception]")
{
   swtl::Vector<helpers::CopyOnlyTrackedObject> vec(10UZ);
   helpers::fill_to_capacity(vec);

   auto const expected{ vec.size() + 1 };
   helpers::g_test_controller.reset();

   REQUIRE_NOTHROW(vec.emplace_back());
   REQUIRE(vec.size() == expected);
   REQUIRE(helpers::g_test_controller.count_of.copy_construction > 0UZ);
   REQUIRE(helpers::g_test_controller.count_of.move_construction == 0UZ);
}

TEST_CASE(
    "Reallocation moves elements even with a throwing move constructor if `T` "
    "is not copyable.",
    "[vector][growth][exception]")
{
   swtl::Vector<helpers::MoveOnlyTrackedObject> vec(10UZ);
   helpers::fill_to_capacity(vec);

   auto const expected{ vec.size() + 1 };
   helpers::g_test_controller.reset();

   REQUIRE_NOTHROW(vec.emplace_back());
   REQUIRE(vec.size() == expected);
   REQUIRE(helpers::g_test_controller.count_of.move_construction > 0UZ);
   REQUIRE(helpers::g_test_controller.count_of.copy_construction == 0UZ);
}

TEST_CASE(
    "Reallocation exception safety with throwing move only object.",
    "[vector][growth][exception]")
{
   auto const element_count{ 10UZ };
   auto vec{
      helpers::generate_unique<swtl::Vector<helpers::MoveOnlyTrackedObject>>(
          element_count)
   };

   helpers::fill_to_capacity(vec);

   auto const expected{ vec.size() };

   helpers::g_test_controller.reset();
   helpers::g_test_controller.enable_throwing();
   helpers::g_test_controller.throw_when.move_construction = element_count;

   // Reallocation fails, but no memory should be leaked and invariants should
   // hold.  Cannot guarantee the state of the contained elements.
   REQUIRE_THROWS_AS(vec.emplace_back(), helpers::TestException);
   REQUIRE(vec.data() != nullptr);
   REQUIRE(vec.size() == expected);
}

TEST_CASE("Vector comparison.", "[vector][comparison]")
{
   swtl::Vector<int> const baseline_vec{ 0, 1, 2, 3, 4, 5 };
   swtl::Vector<int> const equal_vec{ baseline_vec };
   swtl::Vector<int> const greater_vec{ 0, 1, 2, 3, 4, 6 };
   swtl::Vector<int> const lesser_vec{ 0, 1, 2, 3, 4, 4 };
   swtl::Vector<int> const bigger_vec_with_lesser_values{ 0, 0, 1, 2, 3, 4, 5 };
   swtl::Vector<int> const smaller_vec_with_greater_values{ 9, 8, 7 };
   swtl::Vector<int> const different_elements_but_same_size{ 1, 2, 3, 4, 5, 6 };
   swtl::Vector<int> same_elements_different_capacity{ 0, 1, 2, 3, 4, 5 };
   same_elements_different_capacity.reserve(100UZ);

   REQUIRE(baseline_vec == equal_vec);
   REQUIRE(baseline_vec == same_elements_different_capacity);

   REQUIRE(baseline_vec != greater_vec);
   REQUIRE(baseline_vec != lesser_vec);
   REQUIRE(baseline_vec != different_elements_but_same_size);

   REQUIRE(baseline_vec < greater_vec);
   REQUIRE(baseline_vec < smaller_vec_with_greater_values);

   REQUIRE(baseline_vec > lesser_vec);
   REQUIRE(baseline_vec > bigger_vec_with_lesser_values);

   REQUIRE(baseline_vec <= greater_vec);
   REQUIRE(baseline_vec <= equal_vec);

   REQUIRE(baseline_vec >= lesser_vec);
   REQUIRE(baseline_vec >= equal_vec);
}

TEST_CASE("Vector swaps primitives correctly.", "[vector][swap]")
{
   swtl::Vector<int> a{ 1, 2, 3 };
   swtl::Vector<int> b{ 4, 5 };
   auto const *a_ptr{ a.data() };
   auto const a_size{ a.size() };
   auto const *b_ptr{ b.data() };
   auto const b_size{ b.size() };
   swap(a, b);

   REQUIRE(a.size() == b_size);
   REQUIRE(b.size() == a_size);
   REQUIRE(b.data() == a_ptr);
   REQUIRE(a.data() == b_ptr);
}

TEST_CASE("Vector swaps objects correctly.", "[vector][swap]")
{
   swtl::Vector<std::string> a{ "Stop right there criminal scum!",
                                "Nobody breaks the law on my watch!",
                                "I'm confiscating your stolen goods.",
                                "Now pay the fine or it's off to jail." };
   swtl::Vector<std::string> b{ "I used to be an adventurer like you, ",
                                "but I took an arrow to the knee..." };
   auto const *a_ptr{ a.data() };
   auto const a_size{ a.size() };
   auto const *b_ptr{ b.data() };
   auto const b_size{ b.size() };

   using std::swap;
   swap(a, b);

   REQUIRE(a.size() == b_size);
   REQUIRE(b.size() == a_size);
   REQUIRE(b.data() == a_ptr);
   REQUIRE(a.data() == b_ptr);
}

TEMPLATE_TEST_CASE(
    "max_size() returns sane values.",
    "[vector][capacity]",
    unsigned char,
    int,
    double,
    std::string)
{
   swtl::Vector<TestType> vec;

   REQUIRE(
       vec.max_size()
       <= std::numeric_limits<std::ptrdiff_t>::max() / sizeof(TestType));
}

TEST_CASE("is_empty() returns the correct boolean value.", "[vector][capacity]")
{
   swtl::Vector<int> empty_vec;
   auto const non_empty_vec{ helpers::generate_populated<swtl::Vector<int>>() };

   REQUIRE(empty_vec.is_empty());
   REQUIRE(!non_empty_vec.is_empty());
}

TEST_CASE(
    "size() returns the correct value as elements are added.",
    "[vector][capacity]")
{
   swtl::Vector<int> vec;

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

TEST_CASE("capacity() returns expected values.", "[vector][capacity]")
{
   swtl::Vector<int> vec;

   REQUIRE(vec.capacity() == 0);

   SECTION("Adding capacity increases the return value as expected.")
   {
      auto const value{ 10UZ };
      vec.reserve(value);

      REQUIRE(vec.capacity() >= value);
   }
}

TEMPLATE_TEST_CASE(
    "shrink_to_fit() reduces capacity to size if appropriate.",
    "[vector][capacity]",
    helpers::TrackedObject,
    helpers::NoThrowTrackedObject,
    helpers::CopyOnlyTrackedObject,
    helpers::MoveOnlyTrackedObject)
{
   helpers::g_test_controller.reset();
   {
      swtl::Vector<TestType> vec;

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
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEST_CASE(
    "clear() removes all elements of the vector without affecting capacity.",
    "[vector]")
{
   auto vec{ helpers::generate_populated<swtl::Vector<int>>() };
   auto const populated_capacity{ vec.capacity() };
   vec.clear();

   REQUIRE(vec.is_empty());
   REQUIRE(vec.size() == 0UZ);
   REQUIRE(vec.capacity() == populated_capacity);
   REQUIRE(vec.data() != nullptr);
}

TEMPLATE_TEST_CASE(
    "emplace(const_iterator pos, Args &&...args) constructs a value at the "
    "desired location of a vector with sufficient storage.",
    "[vector][emplace]",
    helpers::TrackedObject,
    helpers::NoThrowTrackedObject)
{
   using T = TestType;

   helpers::g_test_controller.reset();
   {
      swtl::Vector vec{ T{ 1UZ }, T{ 2UZ }, T{ 3UZ }, T{ 4UZ }, T{ 5UZ },
                        T{ 6UZ }, T{ 7UZ }, T{ 8UZ }, T{ 9UZ } };

      vec.reserve(10UZ);
      auto const args{ 0UZ };

      SECTION("Calling emplace with `begin()` inserts at the beginning.")
      {
         swtl::Vector const expected{ T{ 0UZ }, T{ 1UZ }, T{ 2UZ }, T{ 3UZ },
                                      T{ 4UZ }, T{ 5UZ }, T{ 6UZ }, T{ 7UZ },
                                      T{ 8UZ }, T{ 9UZ } };

         vec.emplace(vec.cbegin(), args);

         REQUIRE(vec == expected);
      }

      SECTION("Calling emplace with `end()` inserts at the end.")
      {
         swtl::Vector const expected{ T{ 1UZ }, T{ 2UZ }, T{ 3UZ }, T{ 4UZ },
                                      T{ 5UZ }, T{ 6UZ }, T{ 7UZ }, T{ 8UZ },
                                      T{ 9UZ }, T{ 0UZ } };

         vec.emplace(vec.cend(), args);

         REQUIRE(vec == expected);
      }

      SECTION(
          "Calling emplace with an iterator to the middle inserts in the "
          "correct position.")
      {
         swtl::Vector const expected{ T{ 1UZ }, T{ 2UZ }, T{ 3UZ }, T{ 4UZ },
                                      T{ 0UZ }, T{ 5UZ }, T{ 6UZ }, T{ 7UZ },
                                      T{ 8UZ }, T{ 9UZ } };
         auto const index{ 4UZ };
         auto const iter_pos{ vec.cbegin() + index };

         vec.emplace(iter_pos, args);

         REQUIRE(vec == expected);
      }

      SECTION("Vector does not grow when there is sufficient capacity.")
      {
         auto const old_capacity{ vec.capacity() };
         swtl::Vector const expected{ T{ 0UZ }, T{ 1UZ }, T{ 2UZ }, T{ 3UZ },
                                      T{ 4UZ }, T{ 5UZ }, T{ 6UZ }, T{ 7UZ },
                                      T{ 8UZ }, T{ 9UZ } };

         vec.emplace(vec.cbegin(), args);

         REQUIRE(vec == expected);
         REQUIRE(vec.capacity() == old_capacity);
      }

      SECTION("Vector does grow when additional capacity is required.")
      {
         helpers::fill_to_capacity(vec);
         auto const old_capacity{ vec.capacity() };

         swtl::Vector expected{ T{ 0UZ }, T{ 1UZ }, T{ 2UZ }, T{ 3UZ },
                                T{ 4UZ }, T{ 5UZ }, T{ 6UZ }, T{ 7UZ },
                                T{ 8UZ }, T{ 9UZ } };

         while (expected.size() != vec.size() + 1)
         {
            expected.emplace_back();
         }

         vec.emplace(vec.cbegin(), args);

         REQUIRE(vec == expected);
         REQUIRE(vec.capacity() > old_capacity);
      }
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEST_CASE(
    "emplace(const_iterator pos, Args &&...args) returns an iterator to the "
    "inserted element.",
    "[vector][emplace]")
{
   auto vec{ helpers::generate_unique<swtl::Vector<helpers::TrackedObject>>(
       10UZ) };
   helpers::TrackedObject const value_to_insert(42UZ);

   auto inserted_iter{ vec.emplace(vec.cbegin(), value_to_insert) };

   REQUIRE(inserted_iter == vec.begin());
   REQUIRE(*inserted_iter == value_to_insert);
}

TEST_CASE(
    "Passing arguments to emplace(const_iterator pos, Args &&...args) "
    "constructs an object in place.",
    "[vector][emplace]")
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

TEST_CASE(
    "emplace(const_iterator pos, Args &&...args) exception safety.",
    "[vector][emplace][exception]")
{
   using T = helpers::TrackedObject;

   helpers::g_test_controller.reset();
   {
      swtl::Vector vec{ T{ 1UZ }, T{ 2UZ }, T{ 3UZ }, T{ 4UZ }, T{ 5UZ },
                        T{ 6UZ }, T{ 7UZ }, T{ 8UZ }, T{ 9UZ } };

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

// Vector::insert(const_iterator pos, T const &value) uses emplace() internally.
TEST_CASE(
    "insert(const_iterator pos, T const &value) copy-inserts an lvalue",
    "[vector][insert]")
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
    "insert(const_iterator pos, T &&value) inserts an rvalue",
    "[vector][insert]")
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
    "insert(const_iterator pos, size_type count, T const &value) inserts "
    "`count` instances of `value` before `pos`.",
    "[vector][insert]",
    helpers::TrackedObject,
    helpers::NoThrowTrackedObject)
{
   using T = TestType;

   helpers::g_test_controller.reset();
   {
      swtl::Vector vec{ T{ 0UZ }, T{ 1UZ }, T{ 2UZ }, T{ 3UZ }, T{ 4UZ },
                        T{ 5UZ }, T{ 6UZ }, T{ 7UZ }, T{ 8UZ }, T{ 9UZ } };

      auto const reference_element{ T{ 42UZ } };
      auto const count{ 4UZ };

      SECTION(
          "Insertion when elements fit inside the existing initialized "
          "range.")
      {
         vec.reserve(20UZ);
         auto const pos{ vec.cbegin() + 3UZ };

         swtl::Vector const expected{ T{ 0UZ },  T{ 1UZ },  T{ 2UZ },
                                      T{ 42UZ }, T{ 42UZ }, T{ 42UZ },
                                      T{ 42UZ }, T{ 3UZ },  T{ 4UZ },
                                      T{ 5UZ },  T{ 6UZ },  T{ 7UZ },
                                      T{ 8UZ },  T{ 9UZ } };

         vec.insert(pos, count, reference_element);

         REQUIRE(vec == expected);
      }

      SECTION(
          "Insertion when elements are placed outside of the initialized "
          "range.")
      {
         vec.reserve(20UZ);
         auto const pos{ vec.cend() - 2UZ };

         swtl::Vector const expected{ T{ 0UZ },  T{ 1UZ },  T{ 2UZ },
                                      T{ 3UZ },  T{ 4UZ },  T{ 5UZ },
                                      T{ 6UZ },  T{ 7UZ },  T{ 42UZ },
                                      T{ 42UZ }, T{ 42UZ }, T{ 42UZ },
                                      T{ 8UZ },  T{ 9UZ } };

         vec.insert(pos, count, reference_element);

         REQUIRE(vec == expected);
      }

      SECTION("Insertion at the end.")
      {
         vec.reserve(20UZ);
         auto const pos{ vec.cend() };

         swtl::Vector const expected{
            T{ 0UZ },  T{ 1UZ },  T{ 2UZ },  T{ 3UZ },  T{ 4UZ },
            T{ 5UZ },  T{ 6UZ },  T{ 7UZ },  T{ 8UZ },  T{ 9UZ },
            T{ 42UZ }, T{ 42UZ }, T{ 42UZ }, T{ 42UZ },
         };

         vec.insert(pos, count, reference_element);

         REQUIRE(vec == expected);
      }

      SECTION("Resizes as needed.")
      {
         helpers::fill_to_capacity(vec);
         auto const pos{ vec.cbegin() };

         swtl::Vector const expected{ T{ 42UZ }, T{ 42UZ }, T{ 42UZ },
                                      T{ 42UZ }, T{ 0UZ },  T{ 1UZ },
                                      T{ 2UZ },  T{ 3UZ },  T{ 4UZ },
                                      T{ 5UZ },  T{ 6UZ },  T{ 7UZ },
                                      T{ 8UZ },  T{ 9UZ } };

         vec.insert(pos, count, reference_element);

         REQUIRE(vec == expected);
      }
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEST_CASE(
    "insert(const_iterator pos, size_type count, T const &value) exception "
    "safety.",
    "[vector][insert][exception]")
{
   using T = helpers::TrackedObject;

   helpers::g_test_controller.reset();
   {
      swtl::Vector vec{ T{ 0UZ }, T{ 1UZ }, T{ 2UZ }, T{ 3UZ }, T{ 4UZ },
                        T{ 5UZ }, T{ 6UZ }, T{ 7UZ }, T{ 8UZ }, T{ 9UZ } };

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
         swtl::Vector const expected{
            T{ 0UZ }, T{ 1UZ }, T{ 2UZ }, T{ 3UZ }, T{ 4UZ },  T{ 5UZ },
            T{ 6UZ }, T{ 7UZ }, T{ 8UZ }, T{ 9UZ }, T{ 42UZ }, T{ 42UZ },
         };

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
    "insert(const_iterator pos, InputIterator first, Sentinel last) inserts "
    "elements from the source range into the vector before `pos`.",
    "[vector][insert]",
    helpers::InputIterator<helpers::NoThrowTrackedObject const>,
    swtl::ContiguousIterator<helpers::NoThrowTrackedObject const>,
    helpers::InputIterator<helpers::TrackedObject const>,
    swtl::ContiguousIterator<helpers::TrackedObject const>)
{
   using T = std::iter_value_t<TestType>;

   helpers::g_test_controller.reset();
   {
      swtl::Vector vec{ T{ 0UZ }, T{ 1UZ }, T{ 2UZ }, T{ 3UZ }, T{ 4UZ },
                        T{ 5UZ }, T{ 6UZ }, T{ 7UZ }, T{ 8UZ }, T{ 9UZ } };

      swtl::Vector const source{ T{ 42UZ }, T{ 43UZ }, T{ 44UZ }, T{ 45UZ } };

      TestType begin{ source.data() };
      TestType end{ source.data() + source.size() };

      SECTION(
          "Insertion when elements fit inside the existing initialized "
          "range.")
      {
         vec.reserve(20UZ);
         auto const pos{ vec.cbegin() + 3UZ };

         swtl::Vector const expected{ T{ 0UZ },  T{ 1UZ },  T{ 2UZ },
                                      T{ 42UZ }, T{ 43UZ }, T{ 44UZ },
                                      T{ 45UZ }, T{ 3UZ },  T{ 4UZ },
                                      T{ 5UZ },  T{ 6UZ },  T{ 7UZ },
                                      T{ 8UZ },  T{ 9UZ } };

         vec.insert(pos, begin, end);

         REQUIRE(vec == expected);
      }

      SECTION(
          "Insertion when elements are placed outside of the initialized "
          "range.")
      {
         vec.reserve(20UZ);
         auto const pos{ vec.cend() - 2UZ };

         swtl::Vector const expected{ T{ 0UZ },  T{ 1UZ },  T{ 2UZ },
                                      T{ 3UZ },  T{ 4UZ },  T{ 5UZ },
                                      T{ 6UZ },  T{ 7UZ },  T{ 42UZ },
                                      T{ 43UZ }, T{ 44UZ }, T{ 45UZ },
                                      T{ 8UZ },  T{ 9UZ } };

         vec.insert(pos, begin, end);

         REQUIRE(vec == expected);
      }

      SECTION("Insertion at the end.")
      {
         vec.reserve(20UZ);
         auto const pos{ vec.cend() };

         swtl::Vector const expected{
            T{ 0UZ },  T{ 1UZ },  T{ 2UZ },  T{ 3UZ },  T{ 4UZ },
            T{ 5UZ },  T{ 6UZ },  T{ 7UZ },  T{ 8UZ },  T{ 9UZ },
            T{ 42UZ }, T{ 43UZ }, T{ 44UZ }, T{ 45UZ },
         };

         vec.insert(pos, begin, end);

         REQUIRE(vec == expected);
      }

      SECTION("Resizes as needed.")
      {
         helpers::fill_to_capacity(vec);

         auto const pos{ vec.cbegin() };

         swtl::Vector const expected{ T{ 42UZ }, T{ 43UZ }, T{ 44UZ },
                                      T{ 45UZ }, T{ 0UZ },  T{ 1UZ },
                                      T{ 2UZ },  T{ 3UZ },  T{ 4UZ },
                                      T{ 5UZ },  T{ 6UZ },  T{ 7UZ },
                                      T{ 8UZ },  T{ 9UZ } };

         vec.insert(pos, begin, end);

         for (auto const &[left, right] : std::views::zip(vec, expected))
         {
            REQUIRE(left == right);
         }
      }
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

TEMPLATE_TEST_CASE(
    "insert(const_iterator pos, InputIterator first, Sentinel last) exception "
    "safety.",
    "[vector][insert][exception]",
    helpers::InputIterator<helpers::TrackedObject const>,
    swtl::ContiguousIterator<helpers::TrackedObject const>)
{
   using T = std::iter_value_t<TestType>;

   helpers::g_test_controller.reset();
   {
      swtl::Vector vec{ T{ 0UZ }, T{ 1UZ }, T{ 2UZ }, T{ 3UZ }, T{ 4UZ },
                        T{ 5UZ }, T{ 6UZ }, T{ 7UZ }, T{ 8UZ }, T{ 9UZ } };
      swtl::Vector const source{ T{ 42UZ }, T{ 43UZ }, T{ 44UZ }, T{ 45UZ } };

      TestType begin{ source.data() };
      TestType end{ source.data() + source.size() };

      swtl::Vector const unmodified{ vec };

      SECTION(
          "Insertion when elements fit inside the existing initialized "
          "range.")
      {
         vec.reserve(20UZ);
         auto const pos{ vec.cbegin() + 3UZ };

         swtl::Vector const expected{ T{ 0UZ },  T{ 1UZ },  T{ 2UZ },
                                      T{ 42UZ }, T{ 43UZ }, T{ 44UZ },
                                      T{ 45UZ }, T{ 3UZ },  T{ 4UZ },
                                      T{ 5UZ },  T{ 6UZ },  T{ 7UZ },
                                      T{ 8UZ },  T{ 9UZ } };

         helpers::g_test_controller.enable_throwing();
         helpers::g_test_controller.throw_when.copy_construction
             = helpers::g_test_controller.count_of.copy_construction
             + source.size();

         REQUIRE_THROWS_AS(vec.insert(pos, begin, end), helpers::TestException);

         REQUIRE(vec == unmodified);
      }

      SECTION(
          "Insertion when elements are placed outside of the initialized "
          "range.")
      {
         vec.reserve(20UZ);
         auto const pos{ vec.cend() - 2UZ };

         helpers::g_test_controller.enable_throwing();
         helpers::g_test_controller.throw_when.copy_construction
             = helpers::g_test_controller.count_of.copy_construction
             + source.size();

         REQUIRE_THROWS_AS(vec.insert(pos, begin, end), helpers::TestException);

         if constexpr (
             std::is_same_v<TestType, swtl::ContiguousIterator<T const>>)
         {
            // Elements are moved out of the way first, new elements are
            // assigned, then constructed at the end - which throws.
            swtl::Vector const expected{
               T{ 0UZ }, T{ 1UZ }, T{ 2UZ }, T{ 3UZ },  T{ 4UZ },
               T{ 5UZ }, T{ 6UZ }, T{ 7UZ }, T{ 42UZ }, T{ 43UZ },
            };

            REQUIRE(vec == expected);
         }
         else
         {
            REQUIRE(vec == unmodified);
         }
      }

      SECTION("Insertion at the end.")
      {
         vec.reserve(20UZ);
         auto const pos{ vec.cend() };

         helpers::g_test_controller.enable_throwing();
         helpers::g_test_controller.throw_when.copy_construction
             = helpers::g_test_controller.count_of.copy_construction
             + source.size();

         REQUIRE_THROWS_AS(vec.insert(pos, begin, end), helpers::TestException);

         if constexpr (
             std::is_same_v<TestType, swtl::ContiguousIterator<T const>>)
         {
            REQUIRE(vec == unmodified);
         }
         else
         {
            // Insertion at the end with an iterator that does not support
            // `sized_sentinel_for` is done via repeat calls to push_back.
            swtl::Vector const expected{
               T{ 0UZ },  T{ 1UZ },  T{ 2UZ },  T{ 3UZ }, T{ 4UZ },
               T{ 5UZ },  T{ 6UZ },  T{ 7UZ },  T{ 8UZ }, T{ 9UZ },
               T{ 42UZ }, T{ 43UZ }, T{ 44UZ },
            };

            REQUIRE(vec == expected);
         }
      }

      SECTION("Resizes as needed.")
      {
         helpers::fill_to_capacity(vec);

         auto const pos{ vec.cbegin() };

         helpers::g_test_controller.enable_throwing();
         helpers::g_test_controller.throw_when.copy_construction
             = helpers::g_test_controller.count_of.copy_construction
             + source.size();

         REQUIRE_THROWS_AS(vec.insert(pos, begin, end), helpers::TestException);

         REQUIRE(vec == unmodified);
      }
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

// Vector::insert(const_iterator pos, std::initializer_list init_list) uses
// insert(const_iterator pos, InputIterator first, Sentinel last) internally.
TEMPLATE_TEST_CASE(
    "insert(const_iterator pos, std::initializer_list init_list) inserts the "
    "elements from the initializer list before `pos`.",
    "[vector][insert]",
    helpers::TrackedObject,
    helpers::NoThrowTrackedObject)
{
   using T = TestType;

   helpers::g_test_controller.reset();
   {
      swtl::Vector vec{ T{ 0UZ }, T{ 1UZ }, T{ 2UZ }, T{ 3UZ }, T{ 4UZ },
                        T{ 5UZ }, T{ 6UZ }, T{ 7UZ }, T{ 8UZ }, T{ 9UZ } };

      std::initializer_list const init_list{
         T{ 42UZ }, T{ 43UZ }, T{ 44UZ }, T{ 45UZ }
      };

      swtl::Vector const expected{ T{ 42UZ }, T{ 43UZ }, T{ 44UZ }, T{ 45UZ },
                                   T{ 0UZ },  T{ 1UZ },  T{ 2UZ },  T{ 3UZ },
                                   T{ 4UZ },  T{ 5UZ },  T{ 6UZ },  T{ 7UZ },
                                   T{ 8UZ },  T{ 9UZ } };

      auto const pos{ vec.cbegin() };

      vec.insert(pos, init_list);

      REQUIRE(vec == expected);
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}

// Vector::insert(const_iterator pos, Range &&range) uses
// insert(const_iterator pos, InputIterator first, Sentinel last) internally.
TEMPLATE_TEST_CASE(
    "insert(const_iterator pos, Range &&range) inserts the elements from "
    "the "
    "source range before `pos`.",
    "[vector][insert]",
    helpers::TrackedObject,
    helpers::NoThrowTrackedObject)
{
   using T = TestType;

   helpers::g_test_controller.reset();
   {
      swtl::Vector vec{ T{ 0UZ }, T{ 1UZ }, T{ 2UZ }, T{ 3UZ }, T{ 4UZ },
                        T{ 5UZ }, T{ 6UZ }, T{ 7UZ }, T{ 8UZ }, T{ 9UZ } };

      swtl::Vector const source{ T{ 42UZ }, T{ 43UZ }, T{ 44UZ }, T{ 45UZ } };

      swtl::Vector const expected{ T{ 42UZ }, T{ 43UZ }, T{ 44UZ }, T{ 45UZ },
                                   T{ 0UZ },  T{ 1UZ },  T{ 2UZ },  T{ 3UZ },
                                   T{ 4UZ },  T{ 5UZ },  T{ 6UZ },  T{ 7UZ },
                                   T{ 8UZ },  T{ 9UZ } };

      auto const pos{ vec.cbegin() };

      vec.insert_range(pos, source);

      REQUIRE(vec == expected);
   }
   REQUIRE(helpers::g_test_controller.all_instances_destroyed());
}
