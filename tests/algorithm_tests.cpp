#include "catch2/catch_test_macros.hpp"

import std;

import swtl.algorithm;

import swtl.test.helpers;

namespace helpers = swtl::test_helpers;

TEST_CASE("zip_copy copys even ranges correctly.", "[algorithm][zip_copy]")
{
   auto const source{ helpers::generate_vector<int>() };
   auto destination{ source };

   for (
       [[maybe_unused]]
       auto each : destination)
   {
      ++each;
   }

   SECTION("Via iterators.")
   {
      auto [src_pos, dest_pos]{ swtl::zip_copy(
          source.begin(),
          source.end(),
          destination.begin(),
          destination.end()) };

      REQUIRE(src_pos == source.end());
      REQUIRE(dest_pos == destination.end());
      REQUIRE(source == destination);
   }

   SECTION("Via ranges.")
   {
      auto [src_pos, dest_pos]{ swtl::zip_copy(source, destination) };

      REQUIRE(src_pos == source.end());
      REQUIRE(dest_pos == destination.end());
      REQUIRE(source == destination);
   }
}

TEST_CASE("zip_copy copys uneven ranges correctly.", "[algorithm][zip_copy]")
{
   auto destination{ helpers::generate_vector<int>() };
   auto source{ destination };

   for (
       [[maybe_unused]]
       auto const each : destination)
   {
      source.push_back(each + 1);
   }

   SECTION("Via iterators.")
   {
      auto [src_pos, dest_pos]{ swtl::zip_copy(
          source.begin(),
          source.end(),
          destination.begin(),
          destination.end()) };

      REQUIRE(src_pos != source.end());
      REQUIRE(dest_pos == destination.end());
      REQUIRE(source != destination);
      REQUIRE(
          std::ranges::distance(source.begin(), src_pos)
          == std::ranges::distance(destination.begin(), dest_pos));
      REQUIRE(
          std::ranges::equal(
              std::ranges::subrange(source.begin(), src_pos),
              std::ranges::subrange(destination.begin(), dest_pos)));
   }

   SECTION("Via ranges.")
   {
      auto [src_pos, dest_pos]{ swtl::zip_copy(source, destination) };

      REQUIRE(src_pos != source.end());
      REQUIRE(dest_pos == destination.end());
      REQUIRE(source != destination);
      REQUIRE(
          std::ranges::distance(source.begin(), src_pos)
          == std::ranges::distance(destination.begin(), dest_pos));
      REQUIRE(
          std::ranges::equal(
              std::ranges::subrange(source.begin(), src_pos),
              std::ranges::subrange(destination.begin(), dest_pos)));
   }
}
