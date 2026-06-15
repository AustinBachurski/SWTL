#include <catch2/catch_test_macros.hpp>

#include <cstddef>
#include <limits>
#include <stdexcept>

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

TEST_CASE("Reservation on empty vectors.", "[vector]") {
  swtl::Vector<int> vec;

  SECTION("Correct reservation increases capacity but not size.") {
    vec.reserve(10);

    REQUIRE(vec.is_empty());
    REQUIRE(vec.size() == 0UZ);
    REQUIRE(vec.capacity() == 10UZ);

    vec.reserve(20);

    REQUIRE(vec.is_empty());
    REQUIRE(vec.size() == 0UZ);
    REQUIRE(vec.capacity() == 20UZ);
    vec.reserve(50);

    REQUIRE(vec.is_empty());
    REQUIRE(vec.size() == 0UZ);
    REQUIRE(vec.capacity() == 50UZ);
  }

  SECTION("Attempting to reserve less than current capacity does nothing.") {
    vec.reserve(10);
    vec.reserve(8);

    REQUIRE(vec.is_empty());
    REQUIRE(vec.size() == 0UZ);
    REQUIRE(vec.capacity() == 10UZ);

    vec.reserve(5);

    REQUIRE(vec.is_empty());
    REQUIRE(vec.size() == 0UZ);
    REQUIRE(vec.capacity() == 10UZ);

    vec.reserve(2);

    REQUIRE(vec.is_empty());
    REQUIRE(vec.size() == 0UZ);
    REQUIRE(vec.capacity() == 10UZ);
  }

  SECTION("Reserving an impossible number of elements throws.") {
    REQUIRE_THROWS_AS(vec.reserve(std::numeric_limits<std::size_t>::max()),
                      std::length_error);
  }
}
