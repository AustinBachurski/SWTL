#include "catch2/catch_template_test_macros.hpp"
#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers.hpp"
#include "catch2/matchers/catch_matchers_exception.hpp"
#include "catch2/matchers/catch_matchers_string.hpp"

import std;

import swtl.vector;
import swtl.contiguous_iterator;
import swtl.test_helpers;

namespace helpers = swtl::test_helpers;

void
handle_contract_violation(std::contracts::contract_violation const &violation)
{
   throw std::logic_error(
       std::format(
           "Contract Violation: {}\nLocation: {}:{}",
           violation.comment(),
           violation.location().file_name(),
           violation.location().line()));
}

TEST_CASE(
    "Default construction creates an empty Vector.", "[vector][constructor]")
{
   swtl::Vector<int> const vec;

   REQUIRE(vec.is_empty());
   REQUIRE(vec.size() == 0UZ);
   REQUIRE(vec.capacity() == 0UZ);
   REQUIRE(vec.data() == nullptr);
}

TEST_CASE(
    "Vector(std::initializer_list) creates a Vector with elements from "
    "the initializer list.",
    "[vector][constructor]")
{
   std::initializer_list<int> const init_list{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
   swtl::Vector<int> const vec(init_list);

   REQUIRE(!vec.is_empty());
   REQUIRE(vec.size() == init_list.size());
   REQUIRE(vec.capacity() >= init_list.size());
   REQUIRE(vec.data() != nullptr);
   REQUIRE(std::ranges::equal(vec, init_list));
}

TEST_CASE(
    "Vector(size_type count) creates a Vector with count elements of type T.",
    "[vector][constructor]")
{
   swtl::Vector<int> const should_be_empty(0);
   swtl::Vector<int> const expected{ 0, 0, 0, 0, 0 };
   swtl::Vector<int> const vec(expected.size());

   REQUIRE(should_be_empty.is_empty());
   REQUIRE(!vec.is_empty());
   REQUIRE(vec.size() == expected.size());
   REQUIRE(vec.capacity() >= expected.size());
   REQUIRE(vec.data() != nullptr);
   REQUIRE(vec == expected);
}

TEMPLATE_TEST_CASE(
    "Vector(size_type count, T const &value) creates a Vector with count "
    "elements of type T equal to value.",
    "[vector][constructor]",
    int,
    double,
    std::string)
{
   auto const count{ 4UZ };
   auto const value{ []()
   {
      if constexpr (std::same_as<TestType, int>)
      {
         return 42;
      }
      else if constexpr (std::same_as<TestType, double>)
      {
         return 4.2;
      }
      else if constexpr (std::same_as<TestType, std::string>)
      {
         return std::string{ "forty-two" };
      }
      else
      {
         throw std::invalid_argument(
             "Missing conditional block to generate "
             "value for TestType.");
      }
   }() };

   swtl::Vector<TestType> const vec(count, value);

   REQUIRE(!vec.is_empty());
   REQUIRE(vec.size() == count);
   REQUIRE(vec.capacity() >= count);
   REQUIRE(vec.data() != nullptr);
   REQUIRE(std::ranges::equal(vec, std::vector<TestType>(count, value)));
}

TEMPLATE_TEST_CASE(
    "Vector(iterator, iterator) creates a Vector with elements from the "
    "source container.",
    "[vector][constructor]",
    bool,
    unsigned char,
    int,
    double,
    std::string)
{
   auto const source_data{
      helpers::generate_populated_container<std::vector<TestType>>()
   };
   swtl::Vector const vec(source_data.begin(), source_data.end());

   REQUIRE(!vec.is_empty());
   REQUIRE(vec.size() == source_data.size());
   REQUIRE(vec.capacity() >= source_data.size());
   REQUIRE(vec.data() != nullptr);
   REQUIRE(std::ranges::equal(vec, source_data));
}

TEMPLATE_TEST_CASE(
    "Vector(std::from_range, range) creates a Vector with elements from "
    "the provided range.",
    "[vector][constructor]",
    bool,
    unsigned char,
    int,
    double,
    std::string)
{
   auto const range_of_data{
      helpers::generate_populated_container<std::vector<TestType>>()
   };
   swtl::Vector const vec(std::from_range, range_of_data);

   REQUIRE(!vec.is_empty());
   REQUIRE(vec.size() == range_of_data.size());
   REQUIRE(vec.capacity() >= range_of_data.size());
   REQUIRE(vec.data() != nullptr);
   REQUIRE(std::ranges::equal(vec, range_of_data));
}

TEMPLATE_TEST_CASE(
    "CTAD correctly deduces types.",
    "[vector][constructor]",
    int,
    bool,
    unsigned char,
    float,
    double,
    char const *,
    std::string_view,
    std::string)
{
   TestType value{};
   std::vector<TestType> std_vector_of_value;

   SECTION("CTAD from braced construction.")
   {
      swtl::Vector vec{ value };

      STATIC_REQUIRE(std::is_same_v<decltype(vec), swtl::Vector<TestType>>);
   }

   SECTION("CTAD from iterator construction.")
   {
      swtl::Vector vec(std_vector_of_value.begin(), std_vector_of_value.end());

      STATIC_REQUIRE(std::is_same_v<decltype(vec), swtl::Vector<TestType>>);
   }
}

TEST_CASE(
    "Iterator calls return a const correct iterators.", "[vector][iterator]")
{
   auto vec{ helpers::generate_populated_container<swtl::Vector<int>>() };
   auto const const_vec{ vec };

   // Forward iterators.
   SECTION(
       "begin() returns non-const iterator from non-const container and a "
       "const iterator from a const container..")
   {
      STATIC_REQUIRE(
          std::is_same_v<decltype(vec.begin()), swtl::ContiguousIterator<int>>);
      STATIC_REQUIRE(
          std::is_same_v<
              decltype(const_vec.begin()),
              swtl::ContiguousIterator<int const>
          >);
   }

   SECTION(
       "end() returns non-const iterator from non-const container and a "
       "const iterator from a const container..")
   {
      STATIC_REQUIRE(
          std::is_same_v<decltype(vec.end()), swtl::ContiguousIterator<int>>);
      STATIC_REQUIRE(
          std::is_same_v<
              decltype(const_vec.end()),
              swtl::ContiguousIterator<int const>
          >);
   }

   SECTION("cbegin() returns a const iterator regardless of the container.")
   {
      STATIC_REQUIRE(
          std::is_same_v<
              decltype(vec.cbegin()),
              swtl::ContiguousIterator<int const>
          >);
      STATIC_REQUIRE(
          std::is_same_v<
              decltype(const_vec.cbegin()),
              swtl::ContiguousIterator<int const>
          >);
   }

   SECTION("cend() returns a const iterator regardless of the container.")
   {
      STATIC_REQUIRE(
          std::is_same_v<
              decltype(vec.cend()),
              swtl::ContiguousIterator<int const>
          >);
      STATIC_REQUIRE(
          std::is_same_v<
              decltype(const_vec.cend()),
              swtl::ContiguousIterator<int const>
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
              std::reverse_iterator<swtl::ContiguousIterator<int>>
          >);
      STATIC_REQUIRE(
          std::is_same_v<
              decltype(const_vec.rbegin()),
              std::reverse_iterator<swtl::ContiguousIterator<int const>>
          >);
   }

   SECTION(
       "rend() returns non-const iterator from non-const container and a "
       "const iterator from a const container..")
   {
      STATIC_REQUIRE(
          std::is_same_v<
              decltype(vec.rend()),
              std::reverse_iterator<swtl::ContiguousIterator<int>>
          >);
      STATIC_REQUIRE(
          std::is_same_v<
              decltype(const_vec.rend()),
              std::reverse_iterator<swtl::ContiguousIterator<int const>>
          >);
   }

   SECTION("crbegin() returns a const iterator regardless of the container.")
   {
      STATIC_REQUIRE(
          std::is_same_v<
              decltype(vec.crbegin()),
              std::reverse_iterator<swtl::ContiguousIterator<int const>>
          >);
      STATIC_REQUIRE(
          std::is_same_v<
              decltype(const_vec.crbegin()),
              std::reverse_iterator<swtl::ContiguousIterator<int const>>
          >);
   }

   SECTION("crend() returns a const iterator regardless of the container.")
   {
      STATIC_REQUIRE(
          std::is_same_v<
              decltype(vec.crend()),
              std::reverse_iterator<swtl::ContiguousIterator<int const>>
          >);
      STATIC_REQUIRE(
          std::is_same_v<
              decltype(const_vec.crend()),
              std::reverse_iterator<swtl::ContiguousIterator<int const>>
          >);
   }
}

TEST_CASE(
    "Iteration moves in the correct direction and returns const correct "
    "elements.",
    "[vector][iterator]")
{
   auto vec{ helpers::generate_populated_container<swtl::Vector<int>>() };

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
   auto const vec{ helpers::generate_populated_container<swtl::Vector<int>>() };

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
    "Special Member Functions: Copy Operations",
    "[vector][special member functions][default_allocator]",
    bool,
    unsigned char,
    int,
    double,
    std::string)
{
   auto source{
      helpers::generate_populated_container<swtl::Vector<TestType>>()
   };

   SECTION("Copy constructor from non-const source copies correctly.")
   {
      auto const copied{ source };

      REQUIRE(source == copied);
      REQUIRE(source.data() != copied.data());
   }

   SECTION("Copy assignment operator allocates new memory.")
   {
      swtl::Vector<TestType> destination;
      destination = source;

      REQUIRE(destination == source);
      REQUIRE(destination.data() != source.data());
   }

   SECTION("Self copy assignment does nothing.")
   {
      auto const data_ptr_before_copy{ source.data() };
      auto const capacity_before_copy{ source.capacity() };
      auto const size_before_copy{ source.size() };
      auto const &reference_to_source{ source };
      source = reference_to_source;

      REQUIRE(source.data() == data_ptr_before_copy);
      REQUIRE(source.capacity() == capacity_before_copy);
      REQUIRE(source.size() == size_before_copy);
   }
}

TEMPLATE_TEST_CASE(
    "Special Member Functions: Move Operations",
    "[vector][special member functions][default_allocator]",
    bool,
    int,
    double,
    std::string)
{
   auto source{
      helpers::generate_populated_container<swtl::Vector<TestType>>()
   };

   SECTION(
       "Move constructor from non-const source moves data without allocating.")
   {
      auto const known_good_copy{ source };
      auto const data_ptr_before_move{ source.data() };
      auto const capacity_before_move{ source.capacity() };
      auto const size_before_move{ source.size() };
      auto const moved{ std::move(source) };

      REQUIRE(known_good_copy == moved);
      REQUIRE(source != moved);
      REQUIRE(moved.data() == data_ptr_before_move);
      REQUIRE(moved.capacity() == capacity_before_move);
      REQUIRE(moved.size() == size_before_move);

      REQUIRE(source.data() == nullptr);
      REQUIRE(source.size() == 0UZ);
      REQUIRE(source.capacity() == 0UZ);
   }

   SECTION(
       "Move assignment operator from non-const source moves data without "
       "allocating.")
   {
      auto const known_good_copy{ source };
      auto const data_ptr_before_move{ source.data() };

      swtl::Vector<TestType> destination;
      destination = std::move(source);

      REQUIRE(destination == known_good_copy);
      REQUIRE(destination != source);
      REQUIRE(destination.data() == data_ptr_before_move);

      REQUIRE(source.data() == nullptr);
      REQUIRE(source.size() == 0UZ);
      REQUIRE(source.capacity() == 0UZ);
   }

   SECTION(
       "Move constructor from const source allocates new memory and does "
       "not modify the source.")
   {
      auto const known_good_copy{ source };
      auto const data_ptr_before_move{ source.data() };
      auto const &reference_to_const_source{ source };
      auto moved{ std::move(reference_to_const_source) };

      REQUIRE(known_good_copy == moved);
      REQUIRE(source == moved);
      REQUIRE(moved.data() != data_ptr_before_move);

      REQUIRE(source.data() != nullptr);
      REQUIRE(source.size() != 0UZ);
      REQUIRE(source.capacity() != 0UZ);
   }

   SECTION(
       "Move assignment operator from const source allocates new memory and "
       "does not modify the source.")
   {
      auto const known_good_copy{ source };
      auto const data_ptr_before_move{ source.data() };
      auto const &reference_to_const_source{ source };

      swtl::Vector<TestType> destination;
      destination = std::move(reference_to_const_source);

      REQUIRE(destination == known_good_copy);
      REQUIRE(destination == source);
      REQUIRE(destination.data() != data_ptr_before_move);

      REQUIRE(source.data() != nullptr);
      REQUIRE(source.size() != 0UZ);
      REQUIRE(source.capacity() != 0UZ);
   }

   SECTION("Self move assignment does nothing.")
   {
      auto const known_good_copy{ source };
      auto const data_ptr_before_move{ source.data() };
      auto const capacity_before_move{ source.capacity() };
      auto const size_before_move{ source.size() };
      auto &ref_to_self{ source };  // To bypass -Wself-move.
      source = std::move(ref_to_self);

      REQUIRE(source == known_good_copy);
      REQUIRE(source.data() == data_ptr_before_move);
      REQUIRE(source.capacity() == capacity_before_move);
      REQUIRE(source.size() == size_before_move);
   }
}

TEST_CASE(
    "Special Member Functions: Strong Exception Safety",
    "[vector][special member functions][default_allocator]")
{
   auto const element_count{ 10UZ };
   auto source{ helpers::generate_vector_of_count<helpers::TestObject>(
       element_count) };
   auto const expected{ source };
   helpers::reset_instance_counts_of<helpers::TestObject>();
   helpers::TestObject::throw_when_constructing_instance(element_count);

   SECTION(
       "Copy construction doesn't modify source elements and all new elements "
       "are destroyed when an exception is thrown.")
   {
      REQUIRE_THROWS_AS(
          swtl::Vector<helpers::TestObject>{ source }, std::runtime_error);
      REQUIRE(helpers::TestObject::all_instances_destroyed());
      REQUIRE(source == expected);
   }

   SECTION(
       "Copy assignment doesn't modify source elements and all new elements "
       "are destroyed when an exception is thrown.")
   {
      swtl::Vector<helpers::TestObject> vec;

      REQUIRE_THROWS_AS(vec = source, std::runtime_error);
      REQUIRE(helpers::TestObject::all_instances_destroyed());
      REQUIRE(source == expected);
   }
}

TEST_CASE(
    "Vector(size_type count) exception safety.",
    "[vector][constructor][exception safety]")
{
   helpers::reset_instance_counts_of<helpers::TestObject>();

   SECTION(
       "Memory owned by the vector does not leak if an exception is thrown "
       "during construction.")
   {
      const auto instances{ 1UZ };
      helpers::TestObject::throw_when_constructing_instance(instances);

      REQUIRE_THROWS_AS(
          swtl::Vector<helpers::TestObject>(instances), std::runtime_error);
   }

   SECTION(
       "Any elements that were constructed are destroyed if an exception is "
       "thrown during construction.")
   {
      const auto instances{ 5UZ };
      helpers::TestObject::throw_when_constructing_instance(instances);

      REQUIRE_THROWS_AS(
          swtl::Vector<helpers::TestObject>(instances), std::runtime_error);
      REQUIRE(helpers::TestObject::all_instances_destroyed());
   }
}

TEST_CASE(
    "Vector(size_type count, T const &value) exception safety.",
    "[vector][constructor][exception safety]")
{
   helpers::TestObject reference_object;
   helpers::reset_instance_counts_of<helpers::TestObject>();

   SECTION(
       "Memory owned by the vector does not leak if an exception is thrown "
       "during construction.")
   {
      const auto instances{ 1UZ };
      helpers::TestObject::throw_when_constructing_instance(instances);

      REQUIRE_THROWS_AS(
          swtl::Vector<helpers::TestObject>(instances, reference_object),
          std::runtime_error);
   }

   SECTION(
       "Any elements that were constructed are destroyed if an exception is "
       "thrown during construction.")
   {
      const auto instances{ 5UZ };
      helpers::TestObject::throw_when_constructing_instance(instances);

      REQUIRE_THROWS_AS(
          swtl::Vector<helpers::TestObject>(instances, reference_object),
          std::runtime_error);
      REQUIRE(helpers::TestObject::all_instances_destroyed());
   }
}

TEST_CASE(
    "Vector(InputIterator src_begin, Sentinel src_end) exception safety.",
    "[vector][constructor][exception safety]")
{
   auto const source_count{ 5UZ };
   std::vector<helpers::TestObject> const source(source_count);
   helpers::reset_instance_counts_of<helpers::TestObject>();

   SECTION(
       "Memory owned by the vector does not leak if an exception is thrown "
       "during construction.")
   {
      const auto instances{ 1UZ };
      helpers::TestObject::throw_when_constructing_instance(instances);

      REQUIRE(instances <= source_count);
      REQUIRE_THROWS_AS(
          swtl::Vector<helpers::TestObject>(source.begin(), source.end()),
          std::runtime_error);
   }

   SECTION(
       "Any elements that were constructed are destroyed if an exception is "
       "thrown during construction.")
   {
      const auto instances{ 5UZ };
      helpers::TestObject::throw_when_constructing_instance(instances);

      REQUIRE(instances <= source_count);
      REQUIRE_THROWS_AS(
          swtl::Vector<helpers::TestObject>(source.begin(), source.end()),
          std::runtime_error);
      REQUIRE(helpers::TestObject::all_instances_destroyed());
   }
}

TEST_CASE(
    "Vector(std::from_range_t, Range, &&range) exception safety.",
    "[vector][constructor][exception safety]")
{
   auto const source_count{ 5UZ };
   std::vector<helpers::TestObject> const source(source_count);
   helpers::reset_instance_counts_of<helpers::TestObject>();

   SECTION(
       "Memory owned by the vector does not leak if an exception is thrown "
       "during construction.")
   {
      const auto instances{ 1UZ };
      helpers::TestObject::throw_when_constructing_instance(instances);

      REQUIRE(instances <= source_count);
      REQUIRE_THROWS_AS(
          swtl::Vector<helpers::TestObject>(std::from_range, source),
          std::runtime_error);
   }

   SECTION(
       "Any elements that were constructed are destroyed if an exception is "
       "thrown during construction.")
   {
      const auto instances{ 5UZ };
      helpers::TestObject::throw_when_constructing_instance(instances);

      REQUIRE(instances <= source_count);
      REQUIRE_THROWS_AS(
          swtl::Vector<helpers::TestObject>(std::from_range, source),
          std::runtime_error);
      REQUIRE(helpers::TestObject::all_instances_destroyed());
   }
}

// ** ASSIGNMENT **
TEMPLATE_TEST_CASE(
    "Vector::assign(size_type count, T const &value) updates vector with new "
    "data.",
    "[vector][assign]",
    helpers::TestObject,
    helpers::NoThrowTestObject)
{
   auto const new_count{ 10UZ };
   swtl::Vector<TestType> vec;
   swtl::Vector<TestType> const expected(new_count, TestType{});

   vec.assign(new_count, TestType{});

   REQUIRE(vec == expected);
}

TEMPLATE_TEST_CASE(
    "Vector::assign(size_type count, T const &value) does not leak memory when "
    "reallocating.",
    "[vector][assign]",
    helpers::TestObject,
    helpers::NoThrowTestObject)
{
   auto const base_count{ 2UZ };
   auto const new_count{ 128UZ };
   swtl::Vector<TestType> const expected(new_count, TestType{});

   helpers::reset_instance_counts_of<TestType>();
   swtl::Vector<TestType> vec(base_count);

   INFO(
       "BAD TEST: initial capacity (which was "
       << vec.capacity() << ") must be less than new_count (which was "
       << new_count << ") for test to trigger reallocation - test is invalid!");
   REQUIRE(vec.capacity() < new_count);

   vec.assign(new_count, TestType{});

   REQUIRE(vec == expected);
   REQUIRE(std::cmp_equal(TestType::instances_alive(), new_count));
}

TEST_CASE(
    "Vector::assign(size_type count, T const &value) correctly manages "
    "lifetimes of existing elements if old size > new size.",
    "[vector][assign]")
{
   auto const base_count{ 10UZ };
   auto const expected_count{ 5UZ };
   swtl::Vector<helpers::TestObject> const expected(expected_count);

   helpers::reset_instance_counts_of<helpers::TestObject>();

   swtl::Vector<helpers::TestObject> vec(base_count);
   vec.assign(expected_count, helpers::TestObject{});

   REQUIRE(vec == expected);
   REQUIRE(
       std::cmp_equal(helpers::TestObject::instances_alive(), expected_count));
}

TEST_CASE(
    "Vector::assign(size_type count, T const &value) does not leak if an "
    "exception is thrown.",
    "[vector][assign][exception]")
{
   auto const base_count{ 5UZ };
   auto const new_count{ 10UZ };
   swtl::Vector<helpers::TestObject> vec(base_count);
   auto const expected{ vec };

   helpers::reset_instance_counts_of<helpers::TestObject>();
   helpers::TestObject::throw_when_constructing_instance(new_count);

   REQUIRE_THROWS_AS(
       vec.assign(new_count, helpers::TestObject{}), std::runtime_error);
   REQUIRE(helpers::TestObject::all_instances_destroyed());
   REQUIRE(vec == expected);
}

TEMPLATE_TEST_CASE(
    "Vector::assign(InputIterator src_begin, Sentinel src_end) assigns "
    "from the source iterator and doesn't leak elements.",
    "[vector][assign]",
    helpers::TestInputIterator<helpers::TestObject const>,
    swtl::ContiguousIterator<helpers::TestObject const>)
{
   auto const source_size{ 10UZ };
   auto const initial_size{ 20UZ };

   auto const source{ helpers::generate_vector_of_count<helpers::TestObject>(
       source_size) };

   TestType begin{ source.data() };
   TestType end{ source.data() + source.size() };

   helpers::reset_instance_counts_of<helpers::TestObject>();

   swtl::Vector<helpers::TestObject> vec(initial_size);

   vec.assign(begin, end);

   REQUIRE(vec == source);
   REQUIRE(helpers::TestObject::instances_alive() == source_size);
}

TEMPLATE_TEST_CASE(
    "Vector::assign(InputIterator src_begin, Sentinel src_end) assigns "
    "from the source iterator and grows when needed.",
    "[vector][assign]",
    helpers::TestInputIterator<helpers::TestObject const>,
    swtl::ContiguousIterator<helpers::TestObject const>)
{
   auto const source_size{ 20UZ };
   auto const initial_size{ 10UZ };

   auto const source{ helpers::generate_vector_of_count<helpers::TestObject>(
       source_size) };

   TestType begin{ source.data() };
   TestType end{ source.data() + source.size() };

   helpers::reset_instance_counts_of<helpers::TestObject>();

   swtl::Vector<helpers::TestObject> vec(initial_size);

   vec.assign(begin, end);

   REQUIRE(vec == source);
   REQUIRE(helpers::TestObject::instances_alive() == source_size);
}

TEMPLATE_TEST_CASE(
    "Vector::assign(InputIterator src_begin, Sentinel src_end) does not "
    "leak if an exception is thrown.",
    "[vector][assign]",
    helpers::TestInputIterator<helpers::TestObject const>,
    swtl::ContiguousIterator<helpers::TestObject const>)
{
   auto const source_size{ 20UZ };
   auto const initial_size{ 10UZ };

   auto const source{ helpers::generate_vector_of_count<helpers::TestObject>(
       source_size) };

   TestType begin{ source.data() };
   TestType end{ source.data() + source.size() };

   helpers::reset_instance_counts_of<helpers::TestObject>();
   helpers::TestObject::throw_when_constructing_instance(initial_size + 1);

   swtl::Vector<helpers::TestObject> vec(initial_size);

   REQUIRE_THROWS_AS(vec.assign(begin, end), std::runtime_error);
   REQUIRE(helpers::TestObject::instances_alive() == initial_size);
}

TEST_CASE(
    "Vector::assign(InputIterator src_begin, Sentinel src_end) triggers a "
    "contract assert if arguments are reversed and the iterator supports "
    "operator-.",
    "[vector][assign]")
{
   auto const source{
      helpers::generate_populated_container<std::vector<int>>()
   };
   swtl::Vector<int> vec;

   REQUIRE_NOTHROW(vec.assign(source.begin(), source.end()));
   REQUIRE_THROWS_AS(
       vec.assign(source.end(), source.begin()), std::logic_error);
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
   auto const range{
      helpers::generate_populated_container<std::vector<int>>()
   };
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

   auto const expected{
      helpers::generate_populated_container<std::vector<T>>()
   };

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

TEST_CASE("Element modification via front().")
{
   swtl::Vector<int> actual{ 2, 2, 3, 4 };
   swtl::Vector<int> const expected{ 1, 2, 3, 4 };

   actual.front() = 1;

   REQUIRE(actual == expected);
}

TEST_CASE("Element modification via back().")
{
   swtl::Vector<int> actual{ 1, 2, 3, 5 };
   swtl::Vector<int> const expected{ 1, 2, 3, 4 };

   actual.back() = 4;

   REQUIRE(actual == expected);
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
   auto vec{ helpers::generate_populated_container<swtl::Vector<TestType>>() };
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
   auto const data{
      helpers::generate_populated_container<std::vector<TestType>>()
   };

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
   auto const data{
      helpers::generate_populated_container<std::vector<TestType>>()
   };

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
   auto const expected{
      helpers::generate_populated_container<std::vector<TestType>>()
   };

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
   auto vec{ helpers::generate_populated_container<swtl::Vector<TestType>>() };
   auto const before_growth{ vec };

   while (vec.size() < vec.capacity())
   {
      vec.emplace_back();
   }
   vec.emplace_back();

   for (auto const &pair : std::views::zip(vec, before_growth))
   {
      REQUIRE(std::get<0>(pair) == std::get<1>(pair));
   }
}

TEST_CASE("Reallocation exception safety.", "[vector][growth][exception]")
{
   auto source{ helpers::generate_vector_of_count<helpers::TestObject>(10UZ) };

   for (auto const _ : std::views::iota(0UZ, source.capacity() - source.size()))
   {
      source.emplace_back();
   }

   auto const expected{ source };

   helpers::reset_instance_counts_of<helpers::TestObject>();
   helpers::TestObject::throw_when_constructing_instance(source.size());

   REQUIRE_THROWS_AS(source.emplace_back(), std::runtime_error);
   REQUIRE(helpers::TestObject::all_instances_destroyed());
   REQUIRE(source == expected);
}

TEST_CASE(
    "Reallocation copies if move is not noexcept.",
    "[vector][growth][exception]")
{
   struct MoveThrows
   {
      std::size_t id{};

      constexpr MoveThrows() = default;

      constexpr MoveThrows(std::size_t identifier)
          : id{ identifier }
      {}

      constexpr MoveThrows([[maybe_unused]] MoveThrows const &other) = default;
      constexpr MoveThrows &
      operator=([[maybe_unused]] MoveThrows const &other) = default;

      constexpr MoveThrows([[maybe_unused]] MoveThrows &&other)
      {
         throw std::runtime_error("Shouldn't happen.");
      }

      constexpr MoveThrows &
      operator=([[maybe_unused]] MoveThrows &&other)
      {
         throw std::runtime_error("Shouldn't happen.");
      }

      constexpr auto
      operator<=>(MoveThrows const &other) const = default;
   };

   auto source{ helpers::generate_vector_of_count<MoveThrows>(10UZ) };

   for (auto const _ : std::views::iota(0UZ, source.capacity() - source.size()))
   {
      source.emplace_back();
   }

   auto const expected{ source.size() + 1 };

   REQUIRE_NOTHROW(source.emplace_back());
   REQUIRE(source.size() == expected);
}

TEST_CASE(
    "Reallocation copies if object is not movable.",
    "[vector][growth][exception]")
{
   auto source{ helpers::generate_vector_of_count<helpers::CopyOnlyTestObject>(
       10UZ) };

   for (auto const _ : std::views::iota(0UZ, source.capacity() - source.size()))
   {
      source.emplace_back();
   }

   auto const expected{ source.size() + 1 };

   REQUIRE_NOTHROW(source.emplace_back());
   REQUIRE(source.size() == expected);
}

TEST_CASE(
    "Reallocation exception safety with throwing move only object.",
    "[vector][growth][exception]")
{
   auto const element_count{ 10UZ };
   auto source{ helpers::generate_vector_of_count<helpers::MoveOnlyTestObject>(
       element_count) };

   for (auto const _ : std::views::iota(0UZ, source.capacity() - source.size()))
   {
      source.emplace_back();
   }

   auto const expected{ source.size() };

   helpers::reset_instance_counts_of<helpers::MoveOnlyTestObject>();
   helpers::MoveOnlyTestObject::throw_when_constructing_instance(element_count);

   // Reallocation fails, but no memory should be leaked and invariants should
   // hold.  Cannot guarantee the state of the contained elements.
   REQUIRE_THROWS_AS(source.emplace_back(), std::runtime_error);
   REQUIRE(source.data() != nullptr);
   REQUIRE(source.size() == expected);
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
   same_elements_different_capacity.reserve(100);

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
   auto const non_empty_vec{
      helpers::generate_populated_container<swtl::Vector<int>>()
   };

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

TEST_CASE("capacity() returns a sane value.", "[vector][capacity]")
{
   swtl::Vector<int> vec;

   REQUIRE(vec.capacity() == 0);

   SECTION(
       "Adding capacity increases the return value of capacity() as expected.")
   {
      auto const value{ 10UZ };
      vec.reserve(value);

      REQUIRE(vec.capacity() >= value);
   }
}

TEST_CASE(
    "clear() removes all elements of the vector without affecting capacity.",
    "[vector][modifiers]")
{
   auto vec{ helpers::generate_populated_container<swtl::Vector<int>>() };
   auto const populated_capacity{ vec.capacity() };
   vec.clear();

   REQUIRE(vec.is_empty());
   REQUIRE(vec.size() == 0UZ);
   REQUIRE(vec.capacity() == populated_capacity);
   REQUIRE(vec.data() != nullptr);
}
