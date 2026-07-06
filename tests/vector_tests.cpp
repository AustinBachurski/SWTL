#include "catch2/catch_template_test_macros.hpp"
#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers.hpp"
#include "catch2/matchers/catch_matchers_exception.hpp"
#include "catch2/matchers/catch_matchers_string.hpp"

#include <contracts>
#include <cstddef>
#include <format>
#include <initializer_list>
#include <limits>
#include <ranges>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

import swtl_vector;
import swtl_test_helper_functions;
import swtl_test_helper_objects;

namespace helpers = swtl_test_helpers;

auto handle_contract_violation(
    std::contracts::contract_violation const &violation) -> void {
  throw std::logic_error(std::format(
      "Contract Violation: {}\nLocation: {}:{}", violation.comment(),
      violation.location().file_name(), violation.location().line()));
}

// ** VECTOR ITERATOR TESTS **
TEST_CASE("VectorIterator initialization.", "[vector_iterator]") {
  SECTION("Valid initalization.") {
    swtl::Vector<int> empty_vec;
    swtl::Vector<int> non_empty_vec{1, 2, 3};
    swtl::VectorIterator empty_iter{empty_vec.data()};
    swtl::VectorIterator populated_iter{non_empty_vec.data()};

    REQUIRE(empty_vec.data() == std::to_address(empty_iter));
    REQUIRE(non_empty_vec.data() == std::to_address(populated_iter));
    REQUIRE(std::addressof(non_empty_vec.front()) ==
            std::to_address(populated_iter));
  }
}

TEST_CASE("VectorIterator const conversion.", "[vector_iterator]") {
  SECTION("Non-const to const.") {
    swtl::Vector<int> vec{1, 2, 3};
    auto iter{vec.begin()};
    auto const_iter{vec.cbegin()};

    STATIC_REQUIRE(std::is_convertible_v<decltype(iter), decltype(const_iter)>);
  }
}

TEST_CASE("VectorIterator access operators.", "[vector_iterator]") {
  struct CustomObject {
    int value{};
    std::string string{};
  };

  CustomObject base{1, "that"};
  CustomObject const const_base{4, "it's mine"};

  swtl::Vector<CustomObject> vec{{1, "that"}, {2, "tasted"}, {3, "purple"}};
  swtl::Vector<CustomObject> const const_vec{
      {4, "it's mine"}, {5, "I"}, {6, "licked it"}};

  auto iter{vec.begin()};
  auto const_iter{const_vec.begin()};

  SECTION("operator* returns a reference to the underlying element.") {
    // Non-const element reference expected.
    REQUIRE(std::is_lvalue_reference_v<decltype(*iter)>);
    REQUIRE(!std::is_const_v<std::remove_reference_t<decltype(*iter)>>);
    REQUIRE(std::is_same_v<std::remove_reference_t<decltype(*iter)>,
                           decltype(base)>);

    REQUIRE((*iter).value == base.value);
    REQUIRE((*iter).string == base.string);

    // Const element reference expected.
    REQUIRE(std::is_lvalue_reference_v<decltype(*const_iter)>);
    REQUIRE(std::is_const_v<std::remove_reference_t<decltype(*const_iter)>>);
    REQUIRE(std::is_same_v<std::remove_reference_t<decltype(*const_iter)>,
                           decltype(const_base)>);

    REQUIRE((*const_iter).value == const_base.value);
    REQUIRE((*const_iter).string == const_base.string);
  }

  SECTION("operator-> accesses the underlying object via pointer semantics.") {
    // Non-const pointer to element expected.
    REQUIRE(std::is_same_v<decltype(iter.operator->()), decltype(base) *>);

    REQUIRE(iter->value == base.value);
    REQUIRE(iter->string == base.string);

    // Const pointer to element expected.
    REQUIRE(std::is_same_v<decltype(const_iter.operator->()),
                           decltype(const_base) *>);

    REQUIRE(const_iter->value == const_base.value);
    REQUIRE(const_iter->string == const_base.string);
  }

  SECTION("operator[] returns a reference to the element at the specified "
          "offset.") {
    // Non-const element reference expected.
    REQUIRE(std::is_lvalue_reference_v<decltype(iter[0])>);
    REQUIRE(!std::is_const_v<std::remove_reference_t<decltype(iter[0])>>);
    REQUIRE(std::is_same_v<std::remove_reference_t<decltype(iter[0])>,
                           decltype(base)>);

    REQUIRE(iter[0].value == 1);
    REQUIRE(iter[0].string == "that");
    REQUIRE(iter[1].value == 2);
    REQUIRE(iter[1].string == "tasted");
    REQUIRE(iter[2].value == 3);
    REQUIRE(iter[2].string == "purple");

    // Const element reference expected.
    REQUIRE(std::is_lvalue_reference_v<decltype(const_iter[0])>);
    REQUIRE(std::is_const_v<std::remove_reference_t<decltype(const_iter[0])>>);
    REQUIRE(std::is_same_v<std::remove_reference_t<decltype(const_iter[0])>,
                           decltype(const_base)>);

    REQUIRE(const_iter[0].value == 4);
    REQUIRE(const_iter[0].string == "it's mine");
    REQUIRE(const_iter[1].value == 5);
    REQUIRE(const_iter[1].string == "I");
    REQUIRE(const_iter[2].value == 6);
    REQUIRE(const_iter[2].string == "licked it");
  }
}

TEST_CASE("VectorIterator arithmetic operators.", "[vector_iterator]") {
  std::array const values{1, 2, 3, 4, 5};
  swtl::Vector const vec(values.begin(), values.end());

  auto begin_iter{vec.begin()};
  auto end_iter{vec.end()};

  SECTION("operator++ increments the iterator.") {
    REQUIRE(*begin_iter++ == values.front());
    REQUIRE(*begin_iter == values[1]);
    REQUIRE(*++begin_iter == values[2]);
  }

  SECTION("operator-- decrements the iterator.") {
    REQUIRE(*--end_iter == values.back());
    REQUIRE(*end_iter-- == values.back());
    REQUIRE(*end_iter == values[3]);
  }

  SECTION("operator+= increments the iterator by n.") {
    REQUIRE(*(begin_iter += 1) == values[1]);
    REQUIRE(*(begin_iter += 2) == values[3]);
  }

  SECTION("operator-= decrements the iterator by n.") {
    REQUIRE(*(end_iter -= 1) == values.back());
    REQUIRE(*(end_iter -= 2) == values[2]);
  }

  SECTION("operator+(iterator, difference_type) returns a new iterator "
          "incremented by n.") {
    REQUIRE(*(begin_iter + 2) == values[2]);
    REQUIRE(*begin_iter == values.front());
    REQUIRE(*(begin_iter + values.size() - 1) == values.back());
  }

  SECTION("operator+(difference_type, iterator) returns a new iterator "
          "incremented by n.") {
    REQUIRE(*(2 + begin_iter) == values[2]);
    REQUIRE(*begin_iter == values.front());
    REQUIRE(*(values.size() - 1 + begin_iter) == values.back());
  }

  SECTION("operator-(iterator, difference_type) returns a new iterator "
          "decremented by n.") {
    REQUIRE(*(end_iter - 2) == values[3]);
    REQUIRE(end_iter == vec.end());
    REQUIRE(*(end_iter - values.size()) == values.front());
  }

  SECTION("operator-(iterator, iterator) returns the distance between two "
          "iterators.") {
    REQUIRE(begin_iter - end_iter == -5);
    REQUIRE(end_iter - (begin_iter + 2) == 3);
    REQUIRE(begin_iter + 2 - vec.begin() == 2);
    REQUIRE(std::cmp_equal(vec.end() - vec.begin(), vec.size()));
  }
}

TEST_CASE("VectorIterator comparison operators.", "[vector_iterator]") {
  std::array const values{0, 1};
  swtl::Vector const vec(values.begin(), values.end());

  auto first{vec.begin()};
  auto middle{vec.begin() + 1};
  auto last{vec.end()};

  SECTION("operator==") {
    REQUIRE(first == middle - 1);
    REQUIRE(middle == last - 1);
    REQUIRE(first + 2 == last);
  }

  SECTION("operator!=.") {
    REQUIRE(first != last);
    REQUIRE(first != middle);
    REQUIRE(middle != last);
  }

  SECTION("operator<.") {
    REQUIRE(first < middle);
    REQUIRE(first < last);
    REQUIRE(middle < last);
  }

  SECTION("operator<=.") {
    REQUIRE(first <= first);
    REQUIRE(first <= middle);
    REQUIRE(first <= last);
  }

  SECTION("operator>.") {
    REQUIRE(last > middle);
    REQUIRE(last > first);
    REQUIRE(middle > first);
  }

  SECTION("operator>=.") {
    REQUIRE(last >= last);
    REQUIRE(last >= middle);
    REQUIRE(last >= first);
  }
}

// ** VECTOR TESTS **
TEST_CASE("Default construction creates an empty Vector.", "[vector]") {
  swtl::Vector<int> const vec;

  REQUIRE(vec.is_empty());
  REQUIRE(vec.size() == 0UZ);
  REQUIRE(vec.capacity() == 0UZ);
  REQUIRE(vec.data() == nullptr);
}

TEST_CASE("Vector(std::initializer_list) creates a Vector with elements from "
          "the initializer list.",
          "[vector]") {
  std::initializer_list<int> const init_list{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  swtl::Vector<int> const vec(init_list);

  REQUIRE(!vec.is_empty());
  REQUIRE(vec.size() == init_list.size());
  REQUIRE(vec.capacity() >= init_list.size());
  REQUIRE(vec.data() != nullptr);
  REQUIRE(std::ranges::equal(vec, init_list));
}

TEST_CASE("Vector(size_type n) creates a Vector with n elements of type T.",
          "[vector]") {
  swtl::Vector<int> const should_be_empty(0);
  swtl::Vector<int> const expected{0, 0, 0, 0, 0};
  swtl::Vector<int> const vec(expected.size());

  REQUIRE(should_be_empty.is_empty());
  REQUIRE(!vec.is_empty());
  REQUIRE(vec.size() == expected.size());
  REQUIRE(vec.capacity() >= expected.size());
  REQUIRE(vec.data() != nullptr);
  REQUIRE(vec == expected);
}

TEMPLATE_TEST_CASE(
    "Vector(size_type n, T &value) creates a Vector with n elements of "
    "type T equal to value.",
    "[vector]", int, double, std::string) {

  auto const count{4UZ};
  auto const value{[]() {
    if constexpr (std::same_as<TestType, int>) {
      return 42;
    } else if constexpr (std::same_as<TestType, double>) {
      return 4.2;
    } else if constexpr (std::same_as<TestType, std::string>) {
      return std::string{"forty-two"};
    } else {
      throw std::invalid_argument("Missing conditional block to generate "
                                  "value for TestType.");
    }
  }()};

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
    "[vector]", bool, int, double, std::string) {
  auto const source_data{helpers::generate_baseline_data<TestType>()};
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
    "[vector]", bool, int, double, std::string) {
  auto const range_of_data{helpers::generate_baseline_data<TestType>()};
  swtl::Vector const vec(std::from_range, range_of_data);

  REQUIRE(!vec.is_empty());
  REQUIRE(vec.size() == range_of_data.size());
  REQUIRE(vec.capacity() >= range_of_data.size());
  REQUIRE(vec.data() != nullptr);
  REQUIRE(std::ranges::equal(vec, range_of_data));
}

TEMPLATE_TEST_CASE("CTAD correctly deduces types.", "[vector]", int, bool,
                   float, double, char const *, std::string_view, std::string) {
  TestType value{};
  std::vector<TestType> std_vector_of_value;

  SECTION("CTAD from braced construction.") {
    swtl::Vector vec{value};

    STATIC_REQUIRE(std::is_same_v<decltype(vec), swtl::Vector<TestType>>);
  }

  SECTION("CTAD from iterator construction.") {
    swtl::Vector vec(std_vector_of_value.begin(), std_vector_of_value.end());

    STATIC_REQUIRE(std::is_same_v<decltype(vec), swtl::Vector<TestType>>);
  }
}

TEST_CASE("Iterator calls return a const correct iterators.", "[vector]") {
  auto vec{helpers::generate_populated_swtl_vector<int>()};
  auto const const_vec{vec};

  // Forward iterators.
  SECTION("begin() returns non-const iterator from non-const container and a "
          "const iterator from a const container..") {
    STATIC_REQUIRE(
        std::is_same_v<decltype(vec.begin()), swtl::VectorIterator<int>>);
    STATIC_REQUIRE(std::is_same_v<decltype(const_vec.begin()),
                                  swtl::VectorIterator<int const>>);
  }

  SECTION("end() returns non-const iterator from non-const container and a "
          "const iterator from a const container..") {
    STATIC_REQUIRE(
        std::is_same_v<decltype(vec.end()), swtl::VectorIterator<int>>);
    STATIC_REQUIRE(std::is_same_v<decltype(const_vec.end()),
                                  swtl::VectorIterator<int const>>);
  }

  SECTION("cbegin() returns a const iterator regardless of the container.") {
    STATIC_REQUIRE(std::is_same_v<decltype(vec.cbegin()),
                                  swtl::VectorIterator<int const>>);
    STATIC_REQUIRE(std::is_same_v<decltype(const_vec.cbegin()),
                                  swtl::VectorIterator<int const>>);
  }

  SECTION("cend() returns a const iterator regardless of the container.") {
    STATIC_REQUIRE(
        std::is_same_v<decltype(vec.cend()), swtl::VectorIterator<int const>>);
    STATIC_REQUIRE(std::is_same_v<decltype(const_vec.cend()),
                                  swtl::VectorIterator<int const>>);
  }

  // Reverse iterators.
  SECTION("rbegin() returns non-const iterator from non-const container and a "
          "const iterator from a const container..") {
    STATIC_REQUIRE(
        std::is_same_v<decltype(vec.rbegin()),
                       std::reverse_iterator<swtl::VectorIterator<int>>>);
    STATIC_REQUIRE(
        std::is_same_v<decltype(const_vec.rbegin()),
                       std::reverse_iterator<swtl::VectorIterator<int const>>>);
  }

  SECTION("rend() returns non-const iterator from non-const container and a "
          "const iterator from a const container..") {
    STATIC_REQUIRE(
        std::is_same_v<decltype(vec.rend()),
                       std::reverse_iterator<swtl::VectorIterator<int>>>);
    STATIC_REQUIRE(
        std::is_same_v<decltype(const_vec.rend()),
                       std::reverse_iterator<swtl::VectorIterator<int const>>>);
  }

  SECTION("crbegin() returns a const iterator regardless of the container.") {
    STATIC_REQUIRE(
        std::is_same_v<decltype(vec.crbegin()),
                       std::reverse_iterator<swtl::VectorIterator<int const>>>);
    STATIC_REQUIRE(
        std::is_same_v<decltype(const_vec.crbegin()),
                       std::reverse_iterator<swtl::VectorIterator<int const>>>);
  }

  SECTION("crend() returns a const iterator regardless of the container.") {
    STATIC_REQUIRE(
        std::is_same_v<decltype(vec.crend()),
                       std::reverse_iterator<swtl::VectorIterator<int const>>>);
    STATIC_REQUIRE(
        std::is_same_v<decltype(const_vec.crend()),
                       std::reverse_iterator<swtl::VectorIterator<int const>>>);
  }
}

TEST_CASE("Iteration moves in the correct direction and returns const correct "
          "elements.",
          "[vector]") {
  auto vec{helpers::generate_populated_swtl_vector<int>()};

  // References are used in these sections so that the actual return value of
  // the iterator can be tested, as opposed to the result of a copy.

  SECTION("Non-const forward iteration.") {
    auto previous_element{std::numeric_limits<int>::lowest()};

    for (auto &current_element : vec) {
      REQUIRE(previous_element < current_element);
      STATIC_REQUIRE(
          !std::is_const_v<std::remove_reference_t<decltype(current_element)>>);
      previous_element = current_element;
    }
  }

  SECTION("Const forward iteration.") {
    auto previous_element{std::numeric_limits<int>::lowest()};

    for (auto &current_element : std::as_const(vec)) {
      REQUIRE(previous_element < current_element);
      STATIC_REQUIRE(
          std::is_const_v<std::remove_reference_t<decltype(current_element)>>);
      previous_element = current_element;
    }
  }

  SECTION("Non-const reverse iteration.") {
    auto previous_element{std::numeric_limits<int>::max()};

    for (auto &current_element : vec | std::views::reverse) {
      REQUIRE(previous_element > current_element);
      STATIC_REQUIRE(
          !std::is_const_v<std::remove_reference_t<decltype(current_element)>>);
      previous_element = current_element;
    }
  }

  SECTION("Const reverse iteration.") {
    auto previous_element{std::numeric_limits<int>::max()};

    for (auto &current_element : std::as_const(vec) | std::views::reverse) {
      REQUIRE(previous_element > current_element);
      STATIC_REQUIRE(
          std::is_const_v<std::remove_reference_t<decltype(current_element)>>);
      previous_element = current_element;
    }
  }
}

TEST_CASE("Non-const iterator mutability.", "[vector]") {
  auto const vec{helpers::generate_populated_swtl_vector<int>()};

  SECTION("Non-const forward iterator is mutable.") {
    auto mutated_vec{vec};

    for (auto &element : mutated_vec) {
      ++element;
    }

    REQUIRE(mutated_vec != vec);
  }

  SECTION("Non-const reverse iterator is mutable.") {
    auto mutated_vec{vec};

    for (auto &element : mutated_vec | std::views::reverse) {
      ++element;
    }

    REQUIRE(mutated_vec != vec);
  }
}

TEMPLATE_TEST_CASE("Special Member Functions: Copy Operations",
                   "[vector][default_allocator]", bool, int, double,
                   std::string) {
  auto source{helpers::generate_populated_swtl_vector<TestType>()};

  SECTION("Copy constructor from non-const source copies correctly.") {
    auto const copied{source};

    REQUIRE(source == copied);
    REQUIRE(source.data() != copied.data());
  }

  SECTION("Copy assignment operator allocates new memory.") {
    swtl::Vector<TestType> destination;
    destination = source;

    REQUIRE(destination == source);
    REQUIRE(destination.data() != source.data());
  }

  SECTION("Self copy assignment does nothing.") {
    auto const data_ptr_before_copy{source.data()};
    auto const capacity_before_copy{source.capacity()};
    auto const size_before_copy{source.size()};
    auto const &reference_to_source{source};
    source = reference_to_source;

    REQUIRE(source.data() == data_ptr_before_copy);
    REQUIRE(source.capacity() == capacity_before_copy);
    REQUIRE(source.size() == size_before_copy);
  }
}

TEMPLATE_TEST_CASE("Special Member Functions: Move Operations",
                   "[vector][default_allocator]", bool, int, double,
                   std::string) {
  auto source{helpers::generate_populated_swtl_vector<TestType>()};

  SECTION(
      "Move constructor from non-const source moves data without allocating.") {
    auto const known_good_copy{source};
    auto const data_ptr_before_move{source.data()};
    auto const capacity_before_move{source.capacity()};
    auto const size_before_move{source.size()};
    auto const moved{std::move(source)};

    REQUIRE(known_good_copy == moved);
    REQUIRE(source != moved);
    REQUIRE(moved.data() == data_ptr_before_move);
    REQUIRE(moved.capacity() == capacity_before_move);
    REQUIRE(moved.size() == size_before_move);

    REQUIRE(source.data() == nullptr);
    REQUIRE(source.size() == 0UZ);
    REQUIRE(source.capacity() == 0UZ);
  }

  SECTION("Move assignment operator from non-const source moves data without "
          "allocating.") {
    auto const known_good_copy{source};
    auto const data_ptr_before_move{source.data()};

    swtl::Vector<TestType> destination;
    destination = std::move(source);

    REQUIRE(destination == known_good_copy);
    REQUIRE(destination != source);
    REQUIRE(destination.data() == data_ptr_before_move);

    REQUIRE(source.data() == nullptr);
    REQUIRE(source.size() == 0UZ);
    REQUIRE(source.capacity() == 0UZ);
  }

  SECTION("Move constructor from const source allocates new memory and does "
          "not modify the source.") {
    auto const known_good_copy{source};
    auto const data_ptr_before_move{source.data()};
    auto const &reference_to_const_source{source};
    auto moved{std::move(reference_to_const_source)};

    REQUIRE(known_good_copy == moved);
    REQUIRE(source == moved);
    REQUIRE(moved.data() != data_ptr_before_move);

    REQUIRE(source.data() != nullptr);
    REQUIRE(source.size() != 0UZ);
    REQUIRE(source.capacity() != 0UZ);
  }

  SECTION("Move assignment operator from const source allocates new memory and "
          "does not modify the source.") {
    auto const known_good_copy{source};
    auto const data_ptr_before_move{source.data()};
    auto const &reference_to_const_source{source};

    swtl::Vector<TestType> destination;
    destination = std::move(reference_to_const_source);

    REQUIRE(destination == known_good_copy);
    REQUIRE(destination == source);
    REQUIRE(destination.data() != data_ptr_before_move);

    REQUIRE(source.data() != nullptr);
    REQUIRE(source.size() != 0UZ);
    REQUIRE(source.capacity() != 0UZ);
  }

  SECTION("Self move assignment does nothing.") {
    auto const known_good_copy{source};
    auto const data_ptr_before_move{source.data()};
    auto const capacity_before_move{source.capacity()};
    auto const size_before_move{source.size()};
    auto &ref_to_self{source}; // To bypass -Wself-move.
    source = std::move(ref_to_self);

    REQUIRE(source == known_good_copy);
    REQUIRE(source.data() == data_ptr_before_move);
    REQUIRE(source.capacity() == capacity_before_move);
    REQUIRE(source.size() == size_before_move);
  }
}

TEST_CASE("Exception safety with user defined types - throwing constructor.",
          "[vector]") {
  REQUIRE_THROWS_AS(swtl::Vector<helpers::ThrowingConstructor>(1),
                    std::runtime_error);
}

TEST_CASE(
    "Exception safety with user defined types - throwing copy constructor.",
    "[vector]") {
  swtl::Vector<helpers::ThrowingCopyConstructor> throws_on_copy(3);
  auto const full_count{throws_on_copy.capacity()};

  while (throws_on_copy.size() != full_count) {
    throws_on_copy.emplace_back();
  }
  swtl::Vector<helpers::ThrowingCopyConstructor> const expected(full_count);

  REQUIRE_THROWS_AS(throws_on_copy.emplace_back(), std::runtime_error);
  REQUIRE(throws_on_copy == expected);
}

TEST_CASE("Exception safety with user defined types - throwing move "
          "constructor (copy fallback).",
          "[vector]") {
  swtl::Vector<helpers::ThrowingMoveConstructor> throws_on_move(3);
  auto const full_count{throws_on_move.capacity()};

  while (throws_on_move.size() != full_count) {
    throws_on_move.emplace_back();
  }
  swtl::Vector<helpers::ThrowingMoveConstructor> const expected(full_count + 1);

  // Reallocation succeeds because migration falls back to the copy constructor.
  REQUIRE_NOTHROW(throws_on_move.emplace_back());
  REQUIRE(throws_on_move == expected);
}

TEST_CASE("Exception safety with user defined types - throwing move-only "
          "constructor.",
          "[vector]") {
  swtl::Vector<helpers::ThrowingMoveOnlyObject> throws_on_move(3);
  auto const full_count{throws_on_move.capacity()};

  while (throws_on_move.size() != full_count) {
    throws_on_move.emplace_back();
  }
  swtl::Vector<helpers::ThrowingMoveOnlyObject> const expected(full_count);

  // Reallocation fails, but no memory should be leaked and invariants should
  // hold.  Cannot guarantee the state of the contained elements.
  REQUIRE_THROWS_AS(throws_on_move.emplace_back(), std::runtime_error);
  REQUIRE(throws_on_move.data() != nullptr);
  REQUIRE(throws_on_move.size() == expected.size());
}

// TODO: WORKING HERE: Refactor work in progress.
TEMPLATE_TEST_CASE("Element access, const & non-const.", "[vector]", int,
                   int const, double, double const, bool, bool const,
                   std::string, std::string const) {
  using T = typename std::remove_const_t<TestType>;
  using ConstCorrectVector =
      std::conditional_t<std::is_const_v<TestType>, swtl::Vector<T> const,
                         swtl::Vector<T>>;
  using ExpectedQualifiedRef =
      std::conditional_t<std::is_const_v<TestType>, T const &, T &>;

  auto const expected{helpers::generate_baseline_data<T>()};

  ConstCorrectVector vec(expected.begin(), expected.end());

  auto const first_idx{0UZ};
  auto const last_idx{vec.size() - 1};

  SECTION("Vector::at returns a reference to the element at position.") {
    REQUIRE(vec.at(first_idx) == expected[first_idx]);
    REQUIRE(vec.at(last_idx) == expected[last_idx]);
    STATIC_REQUIRE(
        std::is_same_v<decltype(vec.at(first_idx)), ExpectedQualifiedRef>);
  }

  SECTION("Vector::at throws when accessing an element out of bounds.") {
    using Catch::Matchers::ContainsSubstring;
    using Catch::Matchers::MessageMatches;

    auto const invalid_index{vec.size()};

    REQUIRE_THROWS_MATCHES(
        vec.at(invalid_index), std::out_of_range,
        MessageMatches(ContainsSubstring(std::to_string(invalid_index))) &&
            MessageMatches(ContainsSubstring(std::to_string(vec.size()))));
  }

  SECTION("Vector::operator[] returns a reference to the element at "
          "position.") {
    REQUIRE(vec[first_idx] == expected[first_idx]);
    REQUIRE(vec[last_idx] == expected[last_idx]);
    STATIC_REQUIRE(
        std::is_same_v<decltype(vec[first_idx]), ExpectedQualifiedRef>);
  }

  SECTION("Vector::front returns a reference to the first element.") {
    REQUIRE(vec.front() == expected.front());
    STATIC_REQUIRE(std::is_same_v<decltype(vec.front()), ExpectedQualifiedRef>);
  }

  SECTION("Vector::back returns a reference to the last element.") {
    REQUIRE(vec.back() == expected.back());
    STATIC_REQUIRE(std::is_same_v<decltype(vec.back()), ExpectedQualifiedRef>);
  }
}

TEMPLATE_TEST_CASE("Reservation on an empty vector.", "[vector]", bool, int,
                   double, std::string) {
  swtl::Vector<TestType> vec;

  // Capacity may be greater than requested due to using allocate_at_least().

  SECTION("Reserve increases capacity but does not affect size.") {
    auto const initial_capacity{10UZ};
    vec.reserve(initial_capacity);

    REQUIRE(vec.is_empty());
    REQUIRE(vec.size() == 0UZ);
    REQUIRE(vec.capacity() >= initial_capacity);

    SECTION("Continued reservation grows capacity again.") {
      auto const final_capacity{20UZ};
      vec.reserve(final_capacity);

      REQUIRE(vec.is_empty());
      REQUIRE(vec.size() == 0UZ);
      REQUIRE(vec.capacity() >= final_capacity);

      SECTION("Reserving less than the current capacity does nothing.") {
        auto const before_resize_attempt{vec.capacity()};
        vec.reserve(initial_capacity);

        REQUIRE(vec.is_empty());
        REQUIRE(vec.size() == 0UZ);
        REQUIRE(vec.capacity() == before_resize_attempt);
      }
    }
  }

  SECTION("Reserving more than the maximum number of elements throws.") {
    REQUIRE_THROWS_AS(vec.reserve(std::numeric_limits<std::size_t>::max()),
                      std::length_error);
  }
}

TEMPLATE_TEST_CASE("Reservation on a populated vector.", "[vector]", bool, int,
                   double, std::string) {
  auto vec{helpers::generate_populated_swtl_vector<TestType>()};
  auto const initial_capacity{vec.capacity()};
  swtl::Vector const expected{vec};

  // Capacity may be greater than requested due to using allocate_at_least().

  SECTION("Reservation grows capacity but does not modify elements.") {
    auto new_capacity{initial_capacity + 10UZ};
    vec.reserve(new_capacity);

    REQUIRE(vec == expected);
    REQUIRE(vec.size() == expected.size());
    REQUIRE(vec.capacity() >= new_capacity);

    SECTION("Continued reservation grows capacity again.") {
      auto const final_capacity{vec.capacity() + 20UZ};
      vec.reserve(final_capacity);

      REQUIRE(vec == expected);
      REQUIRE(vec.size() == expected.size());
      REQUIRE(vec.capacity() >= final_capacity);

      SECTION("Reserving less than the current capacity does nothing.") {
        vec.reserve(initial_capacity);

        REQUIRE(vec == expected);
        REQUIRE(vec.size() == expected.size());
        REQUIRE(vec.capacity() >= final_capacity);
      }
    }
  }
}

TEST_CASE("Vector size growth", "[vector]") {
  swtl::Vector<int> vec;

  SECTION("Element insertion increases size to match the number of elements.") {
    vec.push_back(1);

    REQUIRE(vec.size() == 1UZ);

    SECTION("Again...") {
      vec.push_back(2);

      REQUIRE(vec.size() == 2UZ);

      SECTION("And again...") {
        for (auto const value : std::views::iota(0, 40)) {
          vec.push_back(value);
        }

        REQUIRE(vec.size() == 42UZ);
      }
    }
  }
}

TEST_CASE("Vector memory growth.", "[vector]") {
  swtl::Vector<std::size_t> vec;

  SECTION("When filling a vector...") {
    auto const initial_capacity{40UZ};
    vec.reserve(initial_capacity);

    for (auto const value : std::views::iota(0UZ, initial_capacity)) {
      vec.push_back(value);
    }

    SECTION("Insertion up to capacity does not trigger growth.") {
      REQUIRE(vec.size() == initial_capacity);
      REQUIRE(vec.capacity() >= initial_capacity);
    }

    SECTION("Insertion beyond capacity does trigger growth.") {
      auto const difference{vec.capacity() - vec.size() + 1};
      for (auto const _ : std::views::iota(0UZ, difference)) {
        vec.push_back(42UZ);
      }

      REQUIRE(vec.size() == initial_capacity + difference);
      REQUIRE(vec.capacity() > initial_capacity);
    }
  }

  SECTION("Growth preserves elements previously inserted.") {
    auto const initial_capacity{100UZ};
    vec.reserve(initial_capacity);

    for (auto const value : std::views::iota(0UZ, initial_capacity)) {
      vec.push_back(value);
    }

    auto before_growth{vec};
    vec.push_back(42UZ);

    bool old_values_intact{true};

    for (auto const idx : std::views::iota(0UZ, initial_capacity)) {
      if (before_growth[idx] != vec[idx]) {
        old_values_intact = false;
      }
    }

    REQUIRE(old_values_intact);
  }
}

TEST_CASE("Comparing vectors.", "[vector]") {
  SECTION("Comparing values.") {
    swtl::Vector<int> const baseline_vec{0, 1, 2, 3, 4, 5};
    swtl::Vector<int> const equal_vec{baseline_vec};
    swtl::Vector<int> const greater_vec{0, 1, 2, 3, 4, 6};
    swtl::Vector<int> const lesser_vec{0, 1, 2, 3, 4, 4};

    REQUIRE(baseline_vec == equal_vec);
    REQUIRE(baseline_vec != greater_vec);
    REQUIRE(baseline_vec != lesser_vec);
    REQUIRE(baseline_vec < greater_vec);
    REQUIRE(baseline_vec > lesser_vec);
    REQUIRE(baseline_vec <= greater_vec);
    REQUIRE(baseline_vec >= lesser_vec);
    REQUIRE(baseline_vec <= equal_vec);
    REQUIRE(baseline_vec >= equal_vec);
  }

  SECTION("Ensure size and capacity doesn't influence correctness.") {
    swtl::Vector<int> const baseline_vec{0, 1, 2, 3, 4, 5};
    swtl::Vector<int> const bigger_vec_with_lesser_values{0, 0, 1, 2, 3, 4, 5};
    swtl::Vector<int> const smaller_vec_with_greater_values{9, 8, 7};
    swtl::Vector<int> const different_elements_but_same_size{1, 2, 3, 4, 5, 6};
    swtl::Vector<int> same_elements_different_capacity{0, 1, 2, 3, 4, 5};
    same_elements_different_capacity.reserve(100);

    REQUIRE(baseline_vec > bigger_vec_with_lesser_values);
    REQUIRE(baseline_vec < smaller_vec_with_greater_values);
    REQUIRE(baseline_vec != different_elements_but_same_size);
    REQUIRE(baseline_vec == same_elements_different_capacity);
  }
}

TEST_CASE("Element inseration.", "[vector]") {
  SECTION("Vector::push_back of lvalue.") {
    swtl::Vector vec{0, 1, 2};
    swtl::Vector const expected{0, 1, 2, 3};

    vec.push_back(3);

    REQUIRE(vec == expected);
  }

  SECTION("Vector::push_back of rvalue.") {
    swtl::Vector<std::unique_ptr<int>> vec;
    auto ptr{std::make_unique<int>(42)};

    vec.push_back(std::move(ptr));

    REQUIRE(*vec.back() == 42);
    REQUIRE(vec.back() != ptr);
  }

  SECTION("Vector::emplace_back default constructs an element.") {
    swtl::Vector<std::string> vec;
    vec.emplace_back();

    REQUIRE(vec.back() == "");
  }

  SECTION("Vector::emplace_back forwards arguments.") {
    struct Point {
      int x;
      int y;
    };
    swtl::Vector<Point> vec;
    auto &inserted{vec.emplace_back(3, 4)};

    REQUIRE(inserted.x == 3);
    REQUIRE(inserted.y == 4);
  }

  SECTION("Vector::emplace_back returns a reference to inserted.") {
    swtl::Vector<std::string> vec;
    auto &inserted{vec.emplace_back()};

    REQUIRE(vec.back() == inserted);
    REQUIRE(std::addressof(vec.back()) == std::addressof(inserted));
  }
}

TEST_CASE("Element modification.", "[vector]") {
  SECTION("Vector::operator[] modifies correct element.") {
    swtl::Vector<int> actual{1, 4, 3, 4};
    swtl::Vector<int> const expected{1, 2, 3, 4};

    actual[1] = 2;

    REQUIRE(actual == expected);
  }

  SECTION("Vector::at modifies correct element.") {
    swtl::Vector<int> actual{1, 2, 4, 4};
    swtl::Vector<int> const expected{1, 2, 3, 4};

    actual.at(2) = 3;

    REQUIRE(actual == expected);
  }

  SECTION("Vector::front modifies correct element.") {
    swtl::Vector<int> actual{2, 2, 3, 4};
    swtl::Vector<int> const expected{1, 2, 3, 4};

    actual.front() = 1;

    REQUIRE(actual == expected);
  }

  SECTION("Vector::back modifies correct element.") {
    swtl::Vector<int> actual{1, 2, 3, 5};
    swtl::Vector<int> const expected{1, 2, 3, 4};

    actual.back() = 4;

    REQUIRE(actual == expected);
  }

  SECTION("Vector::data modifies internal data.") {
    swtl::Vector<int> actual{1, 3, 3, 4};
    swtl::Vector<int> const expected{1, 2, 3, 4};

    actual.data()[1] = 2;

    REQUIRE(actual == expected);
  }
}

TEST_CASE("Vector swap works correctly.", "[vector]") {
  SECTION("Swap primitives") {
    swtl::Vector<int> a{1, 2, 3};
    swtl::Vector<int> b{4, 5};
    auto *a_ptr{a.data()};
    auto a_size{a.size()};
    auto *b_ptr{b.data()};
    auto b_size{b.size()};
    swap(a, b);

    REQUIRE(a.size() == b_size);
    REQUIRE(b.size() == a_size);
    REQUIRE(b.data() == a_ptr);
    REQUIRE(a.data() == b_ptr);
  }

  SECTION("Swap objects.") {
    swtl::Vector<std::string> a{"Stop right there criminal scum!",
                                "Nobody breaks the law on my watch!",
                                "I'm confiscating your stolen goods.",
                                "Now pay the fine or it's off to jail."};
    swtl::Vector<std::string> b{"I used to be an adventurer like you, ",
                                "but I took an arrow to the knee..."};
    auto *a_ptr{a.data()};
    auto a_size{a.size()};
    auto *b_ptr{b.data()};
    auto b_size{b.size()};
    swap(a, b);

    REQUIRE(a.size() == b_size);
    REQUIRE(b.size() == a_size);
    REQUIRE(b.data() == a_ptr);
    REQUIRE(a.data() == b_ptr);
  }
}

TEST_CASE("Vector::max_size correctness.", "[vector]") {
  SECTION("Vector::max_size.") {
    swtl::Vector<char> char_vec;
    swtl::Vector<int> int_vec;

    REQUIRE(char_vec.max_size() > int_vec.max_size());
    REQUIRE(int_vec.max_size() > 0UZ);
  }
}
