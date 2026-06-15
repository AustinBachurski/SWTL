#include "catch2/catch_template_test_macros.hpp"
#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers.hpp"
#include "catch2/matchers/catch_matchers_exception.hpp"
#include "catch2/matchers/catch_matchers_string.hpp"

#include <cstddef>
#include <limits>
#include <numeric>
#include <ranges>
#include <stdexcept>
#include <type_traits>

import swtl_vector;

TEST_CASE("Vector initialization.", "[vector]") {
  SECTION("A default constructed Vector should be empty with a size and "
          "capacity of zero.") {
    swtl::Vector<int> vec;
    REQUIRE(vec.is_empty());
    REQUIRE(vec.size() == 0UZ);
    REQUIRE(vec.capacity() == 0UZ);
  }
}

TEMPLATE_TEST_CASE("Special member functions.", "[vector]", swtl::Vector<int>,
                   swtl::Vector<double>, swtl::Vector<bool>,
                   swtl::Vector<std::string>, swtl::Vector<int> const,
                   swtl::Vector<double> const, swtl::Vector<bool> const,
                   swtl::Vector<std::string> const) {
  using T = typename std::remove_const_t<TestType>::value_type;
  using ExpectedQualifiedRef =
      std::conditional_t<std::is_const_v<TestType>, T const &, T &>;

  TestType initial;

  auto source{}; // TODO: Working Here! - initialize source data by type.

  TEMPLATE_TEST_CASE("Element access, const & non-const.", "[vector]",
                     swtl::Vector<int>, swtl::Vector<int> const,
                     swtl::Vector<std::string>,
                     swtl::Vector<std::string> const) {
    using T = typename std::remove_const_t<TestType>::value_type;
    using ExpectedQualifiedRef =
        std::conditional_t<std::is_const_v<TestType>, T const &, T &>;

    auto expected{[]() {
      if constexpr (std::is_same_v<T, int>) {
        return std::array<int, 5>{1, 2, 3, 4, 5};
      } else if constexpr (std::is_same_v<T, std::string>) {
        return std::array<std::string, 5>{"one", "two", "three", "four",
                                          "five"};
      }
    }()};

    TestType vec(expected.begin(), expected.end());

    SECTION("Vector::at returns a reference to the element at position.") {
      REQUIRE(vec.at(0) == expected[0]);
      REQUIRE(vec.at(1) == expected[1]);
      REQUIRE(vec.at(2) == expected[2]);
      REQUIRE(vec.at(3) == expected[3]);
      REQUIRE(vec.at(4) == expected[4]);
      STATIC_REQUIRE(std::is_same_v<decltype(vec.at(0)), ExpectedQualifiedRef>);
    }

    SECTION("Vector::at throws when accessing an element out of bounds.") {
      using Catch::Matchers::ContainsSubstring;
      using Catch::Matchers::MessageMatches;
      auto const invalid_index{42UZ};

      REQUIRE_THROWS_MATCHES(
          vec.at(invalid_index), std::out_of_range,
          MessageMatches(ContainsSubstring(std::to_string(invalid_index))) &&
              MessageMatches(ContainsSubstring(std::to_string(vec.size()))));
    }

    SECTION("Vector::operator[] returns a reference to the element at "
            "position.") {
      REQUIRE(vec[0] == expected[0]);
      REQUIRE(vec[1] == expected[1]);
      REQUIRE(vec[2] == expected[2]);
      REQUIRE(vec[3] == expected[3]);
      REQUIRE(vec[4] == expected[4]);
      STATIC_REQUIRE(std::is_same_v<decltype(vec[0]), ExpectedQualifiedRef>);
    }

    SECTION("Vector::front returns a reference to the first element.") {
      REQUIRE(vec.front() == expected[0]);
      STATIC_REQUIRE(
          std::is_same_v<decltype(vec.front()), ExpectedQualifiedRef>);
    }

    SECTION("Vector::back returns a reference to the last element.") {
      REQUIRE(vec.back() == expected[4]);
      STATIC_REQUIRE(
          std::is_same_v<decltype(vec.back()), ExpectedQualifiedRef>);
    }
  }

  TEST_CASE("Reservation on empty vectors.", "[vector]") {
    swtl::Vector<int> vec;

    SECTION("Reservation increases capacity but does not affect size.") {
      auto const initial_capacity{10UZ};
      vec.reserve(initial_capacity);

      REQUIRE(vec.is_empty());
      REQUIRE(vec.size() == 0UZ);
      REQUIRE(vec.capacity() == initial_capacity);

      SECTION("Continued reservation grows capacity.") {
        auto const final_capacity{20UZ};
        vec.reserve(final_capacity);

        REQUIRE(vec.capacity() == final_capacity);

        SECTION("Reserving less than the current capacity does nothing.") {
          vec.reserve(initial_capacity);

          REQUIRE(vec.capacity() == final_capacity);
        }
      }
    }

    SECTION("Reserving more than the maximum number of elements throws.") {
      REQUIRE_THROWS_AS(vec.reserve(std::numeric_limits<std::size_t>::max()),
                        std::length_error);
    }
  }

  TEST_CASE("Vector size growth", "[vector]") {
    swtl::Vector<int> vec;

    SECTION(
        "Element insertion increases size to match the number of elements.") {
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
        REQUIRE(vec.capacity() == initial_capacity);
      }

      SECTION("Insertion beyond capacity does trigger growth.") {
        vec.push_back(42UZ);

        REQUIRE(vec.size() == initial_capacity + 1UZ);
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
