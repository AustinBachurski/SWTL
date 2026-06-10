#include <catch2/catch_test_macros.hpp>

import swtl_vector;

TEST_CASE("Container: Initialization", "[vector]") {
  swtl::Vector<int> vec;

  REQUIRE(vec.is_empty());
}

TEST_CASE("Container: Element Insertion", "[vector][push_back]") {
  swtl::Vector<int> vec;
  vec.push_back(42);

  REQUIRE(vec.size() == 1UZ);
  REQUIRE(*vec.begin() == 42);
}
