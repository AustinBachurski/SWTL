#include "catch2/catch_test_macros.hpp"

import std;

import swtl.contiguous_iterator;

TEST_CASE(
    "ContiguousIterator initialization.", "[iterator][contiguous_iterator]")
{
   SECTION("Valid initalization.")
   {
      std::vector<int> empty_vec;
      std::vector<int> non_empty_vec{ 1, 2, 3 };
      swtl::ContiguousIterator empty_iter{ empty_vec.data() };
      swtl::ContiguousIterator populated_iter{ non_empty_vec.data() };

      REQUIRE(empty_vec.data() == std::to_address(empty_iter));
      REQUIRE(non_empty_vec.data() == std::to_address(populated_iter));
      REQUIRE(
          std::addressof(non_empty_vec.front())
          == std::to_address(populated_iter));
   }
}

TEST_CASE(
    "ContiguousIterator const conversion.", "[iterator][contiguous_iterator]")
{
   SECTION("Non-const to const.")
   {
      std::vector<int> vec{ 1, 2, 3 };
      auto iter{ vec.begin() };
      auto const_iter{ vec.cbegin() };

      STATIC_REQUIRE(
          std::is_convertible_v<decltype(iter), decltype(const_iter)>);
   }
}

TEST_CASE(
    "ContiguousIterator access operators.", "[iterator][contiguous_iterator]")
{
   struct CustomObject
   {
      int value{};
      std::string string{};
   };

   CustomObject base{ 1, "that" };
   CustomObject const const_base{ 4, "it's mine" };

   std::vector<CustomObject> vec{
      { 1, "that"   },
      { 2, "tasted" },
      { 3, "purple" }
   };
   std::vector<CustomObject> const const_vec{
      { 4, "it's mine" },
      { 5, "I"         },
      { 6, "licked it" }
   };

   auto iter{ vec.begin() };
   auto const_iter{ const_vec.begin() };

   SECTION("operator* returns a reference to the underlying element.")
   {
      // Non-const element reference expected.
      REQUIRE(std::is_lvalue_reference_v<decltype(*iter)>);
      REQUIRE(!std::is_const_v<std::remove_reference_t<decltype(*iter)>>);
      REQUIRE(
          std::is_same_v<
              std::remove_reference_t<decltype(*iter)>,
              decltype(base)
          >);

      REQUIRE((*iter).value == base.value);
      REQUIRE((*iter).string == base.string);

      // Const element reference expected.
      REQUIRE(std::is_lvalue_reference_v<decltype(*const_iter)>);
      REQUIRE(std::is_const_v<std::remove_reference_t<decltype(*const_iter)>>);
      REQUIRE(
          std::is_same_v<
              std::remove_reference_t<decltype(*const_iter)>,
              decltype(const_base)
          >);

      REQUIRE((*const_iter).value == const_base.value);
      REQUIRE((*const_iter).string == const_base.string);
   }

   SECTION("operator-> accesses the underlying object via pointer semantics.")
   {
      // Non-const pointer to element expected.
      REQUIRE(std::is_same_v<decltype(iter.operator->()), decltype(base) *>);

      REQUIRE(iter->value == base.value);
      REQUIRE(iter->string == base.string);

      // Const pointer to element expected.
      REQUIRE(
          std::is_same_v<
              decltype(const_iter.operator->()),
              decltype(const_base) *
          >);

      REQUIRE(const_iter->value == const_base.value);
      REQUIRE(const_iter->string == const_base.string);
   }

   SECTION(
       "operator[] returns a reference to the element at the specified "
       "offset.")
   {
      // Non-const element reference expected.
      REQUIRE(std::is_lvalue_reference_v<decltype(iter[0])>);
      REQUIRE(!std::is_const_v<std::remove_reference_t<decltype(iter[0])>>);
      REQUIRE(
          std::is_same_v<
              std::remove_reference_t<decltype(iter[0])>,
              decltype(base)
          >);

      REQUIRE(iter[0].value == 1);
      REQUIRE(iter[0].string == "that");
      REQUIRE(iter[1].value == 2);
      REQUIRE(iter[1].string == "tasted");
      REQUIRE(iter[2].value == 3);
      REQUIRE(iter[2].string == "purple");

      // Const element reference expected.
      REQUIRE(std::is_lvalue_reference_v<decltype(const_iter[0])>);
      REQUIRE(
          std::is_const_v<std::remove_reference_t<decltype(const_iter[0])>>);
      REQUIRE(
          std::is_same_v<
              std::remove_reference_t<decltype(const_iter[0])>,
              decltype(const_base)
          >);

      REQUIRE(const_iter[0].value == 4);
      REQUIRE(const_iter[0].string == "it's mine");
      REQUIRE(const_iter[1].value == 5);
      REQUIRE(const_iter[1].string == "I");
      REQUIRE(const_iter[2].value == 6);
      REQUIRE(const_iter[2].string == "licked it");
   }
}

TEST_CASE(
    "ContiguousIterator arithmetic operators.",
    "[iterator][contiguous_iterator]")
{
   std::array const values{ 1, 2, 3, 4, 5 };
   std::vector const vec(values.begin(), values.end());

   auto begin_iter{ vec.begin() };
   auto end_iter{ vec.end() };

   SECTION("operator++ increments the iterator.")
   {
      REQUIRE(*begin_iter++ == values.front());
      REQUIRE(*begin_iter == values[1]);
      REQUIRE(*++begin_iter == values[2]);
   }

   SECTION("operator-- decrements the iterator.")
   {
      REQUIRE(*--end_iter == values.back());
      REQUIRE(*end_iter-- == values.back());
      REQUIRE(*end_iter == values[3]);
   }

   SECTION("operator+= increments the iterator by n.")
   {
      REQUIRE(*(begin_iter += 1) == values[1]);
      REQUIRE(*(begin_iter += 2) == values[3]);
   }

   SECTION("operator-= decrements the iterator by n.")
   {
      REQUIRE(*(end_iter -= 1) == values.back());
      REQUIRE(*(end_iter -= 2) == values[2]);
   }

   SECTION(
       "operator+(iterator, difference_type) returns a new iterator "
       "incremented by n.")
   {
      REQUIRE(*(begin_iter + 2) == values[2]);
      REQUIRE(*begin_iter == values.front());
      REQUIRE(*(begin_iter + values.size() - 1) == values.back());
   }

   SECTION(
       "operator+(difference_type, iterator) returns a new iterator "
       "incremented by n.")
   {
      REQUIRE(*(2 + begin_iter) == values[2]);
      REQUIRE(*begin_iter == values.front());
      REQUIRE(*(values.size() - 1 + begin_iter) == values.back());
   }

   SECTION(
       "operator-(iterator, difference_type) returns a new iterator "
       "decremented by n.")
   {
      REQUIRE(*(end_iter - 2) == values[3]);
      REQUIRE(end_iter == vec.end());
      REQUIRE(*(end_iter - values.size()) == values.front());
   }

   SECTION(
       "operator-(iterator, iterator) returns the distance between two "
       "iterators.")
   {
      REQUIRE(begin_iter - end_iter == -5);
      REQUIRE(end_iter - (begin_iter + 2) == 3);
      REQUIRE(begin_iter + 2 - vec.begin() == 2);
      REQUIRE(std::cmp_equal(vec.end() - vec.begin(), vec.size()));
   }
}

TEST_CASE(
    "ContiguousIterator comparison operators.",
    "[iterator][contiguous_iterator]")
{
   std::vector const vec{ 0, 1 };

   auto first{ vec.begin() };
   auto middle{ vec.begin() + 1 };
   auto last{ vec.end() };

   SECTION("operator==")
   {
      REQUIRE(first == middle - 1);
      REQUIRE(middle == last - 1);
      REQUIRE(first + 2 == last);
   }

   SECTION("operator!=.")
   {
      REQUIRE(first != last);
      REQUIRE(first != middle);
      REQUIRE(middle != last);
   }

   SECTION("operator<.")
   {
      REQUIRE(first < middle);
      REQUIRE(first < last);
      REQUIRE(middle < last);
   }

   SECTION("operator<=.")
   {
      REQUIRE(first <= first);
      REQUIRE(first <= middle);
      REQUIRE(first <= last);
   }

   SECTION("operator>.")
   {
      REQUIRE(last > middle);
      REQUIRE(last > first);
      REQUIRE(middle > first);
   }

   SECTION("operator>=.")
   {
      REQUIRE(last >= last);
      REQUIRE(last >= middle);
      REQUIRE(last >= first);
   }
}
