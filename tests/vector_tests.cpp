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
#include <numeric>
#include <ranges>
#include <stdexcept>
#include <type_traits>
#include <vector>

import swtl_vector;

auto handle_contract_violation(
    std::contracts::contract_violation const &violation) -> void {
  throw std::logic_error(std::format(
      "Contract Violation: {}\nLocation: {}:{}", violation.comment(),
      violation.location().file_name(), violation.location().line()));
}

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
    // Non const element reference expected.
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
    // Non const pointer to element expected.
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
    // Non const element reference expected.
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

  SECTION("operator++.") {
    auto iter{vec.begin()};

    REQUIRE(*iter++ == values.front());
    REQUIRE(*iter == values[1]);
    REQUIRE(*++iter == values[2]);
  }

  SECTION("operator--.") {
    auto iter{vec.end()};

    REQUIRE(*--iter == values.back());
    REQUIRE(*iter-- == values.back());
    REQUIRE(*iter == values[3]);
  }

  SECTION("operator+=.") {
    auto iter{vec.begin()};

    REQUIRE(*(iter += 1) == values[1]);
    REQUIRE(*(iter += 2) == values[3]);
  }

  SECTION("operator-=.") {
    auto iter{vec.end()};

    REQUIRE(*(iter -= 1) == values.back());
    REQUIRE(*(iter -= 2) == values[2]);
  }

  SECTION("operator+(iterator, difference_type.") {
    auto iter{vec.begin()};

    REQUIRE(*(iter + 2) == values[2]);
    REQUIRE(*iter == values.front());
    REQUIRE(*(iter + values.size() - 1) == values.back());
  }

  SECTION("operator+(difference_type, iterator.") {
    auto iter{vec.begin()};

    REQUIRE(*(2 + iter) == values[2]);
    REQUIRE(*iter == values.front());
    REQUIRE(*(values.size() - 1 + iter) == values.back());
  }

  SECTION("operator-(iterator, difference_type).") {
    auto iter{vec.end()};

    REQUIRE(*(iter - 2) == values[3]);
    REQUIRE(iter == vec.end());
    REQUIRE(*(iter - values.size()) == values.front());
  }

  SECTION("operator-(iterator, iterator).") {
    auto lhs{vec.end()};
    auto rhs{vec.begin() + 2};

    REQUIRE(vec.begin() - vec.end() == -5);
    REQUIRE(lhs - rhs == 3);
    REQUIRE(rhs - vec.begin() == 2);
    REQUIRE(vec.end() - vec.begin() == static_cast<std::ptrdiff_t>(vec.size()));
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

TEST_CASE("Vector initialization.", "[vector]") {
  SECTION("Default construction should result in a valid container.") {
    swtl::Vector<int> vec;
    REQUIRE(vec.is_empty());
    REQUIRE(vec.size() == 0UZ);
    REQUIRE(vec.capacity() == 0UZ);
    REQUIRE(vec.data() == nullptr);
  }

  SECTION("Initializer list constructor.") {
    std::initializer_list<int> init_list{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    swtl::Vector<int> vec(init_list);

    REQUIRE(!vec.is_empty());
    REQUIRE(vec.size() == init_list.size());
    REQUIRE(vec.capacity() >= init_list.size());
    REQUIRE(vec.data() != nullptr);
    REQUIRE(std::ranges::equal(vec, init_list));
  }

  SECTION("Iterator constructor.") {
    std::initializer_list<int> init_list{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    swtl::Vector<int> vec(init_list.begin(), init_list.end());

    REQUIRE(!vec.is_empty());
    REQUIRE(vec.size() == init_list.size());
    REQUIRE(vec.capacity() >= init_list.size());
    REQUIRE(vec.data() != nullptr);
    REQUIRE(std::ranges::equal(vec, init_list));
  }

  SECTION("Count constructor.") {
    swtl::Vector<int> should_be_empty(0);
    swtl::Vector<int> expected{0, 0, 0, 0, 0};
    swtl::Vector<int> vec(expected.size());

    REQUIRE(should_be_empty.is_empty());
    REQUIRE(!vec.is_empty());
    REQUIRE(vec.size() == expected.size());
    REQUIRE(vec.capacity() >= expected.size());
    REQUIRE(vec.data() != nullptr);
    REQUIRE(vec == expected);
  }

  SECTION("Count, Value constructor.") {
    swtl::Vector<int> vec_of_int(4, 2);
    swtl::Vector<std::string> vec_of_string(2, "four");

    REQUIRE(vec_of_int == swtl::Vector{2, 2, 2, 2});
    REQUIRE(vec_of_string == swtl::Vector<std::string>{
                                 "four",
                                 "four",
                             });
  }

  SECTION("Range constructor.") {
    std::vector init_list_of_int{1, 2, 3, 4, 5};
    std::vector<std::string> init_list_of_string{"one", "two", "three"};
    swtl::Vector vec_of_int(std::from_range, init_list_of_int);
    swtl::Vector vec_of_string(std::from_range, init_list_of_string);

    REQUIRE(std::ranges::equal(init_list_of_int, vec_of_int));
    REQUIRE(std::ranges::equal(init_list_of_string, vec_of_string));
  }
}

TEMPLATE_TEST_CASE("CTAD correctly deduces types.", "[vector]", int, bool,
                   char const *, std::string) {
  SECTION("Braced initialization.") {
    TestType init{};
    swtl::Vector vec{init};

    STATIC_REQUIRE(std::is_same_v<decltype(vec), swtl::Vector<TestType>>);
  }

  SECTION("Iterator initialization.") {
    std::vector<TestType> init(4);
    swtl::Vector vec(init.begin(), init.end());

    STATIC_REQUIRE(std::is_same_v<decltype(vec), swtl::Vector<TestType>>);
  }
}

TEST_CASE("Iterator validation.", "[vector]") {
  swtl::Vector vec{1, 2, 3, 4, 5};

  SECTION("Const correctness.") {
    STATIC_REQUIRE(std::is_same_v<decltype(vec.cbegin()),
                                  swtl::VectorIterator<int const>>);
    STATIC_REQUIRE(
        std::is_same_v<decltype(vec.crbegin()),
                       std::reverse_iterator<swtl::VectorIterator<int const>>>);
  }

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

  SECTION("Non-const forward iterator mutability.") {
    auto mutated_vec{vec};

    for (auto &element : mutated_vec) {
      ++element;
    }

    REQUIRE(mutated_vec != vec);
  }

  SECTION("Non-const reverse iterator mutability.") {
    auto mutated_vec{vec};

    for (auto &element : mutated_vec | std::views::reverse) {
      ++element;
    }

    REQUIRE(mutated_vec != vec);
  }
}

TEMPLATE_TEST_CASE("Special member functions with std::allocator.", "[vector]",
                   swtl::Vector<int>, swtl::Vector<double>, swtl::Vector<bool>,
                   swtl::Vector<std::string>) {
  using T = typename std::remove_const_t<TestType>::value_type;

  [[maybe_unused]] TestType initial;

  TestType source{[]() {
    if constexpr (std::same_as<T, int>) {
      return TestType{0, 1, 2, 3, 4, 5};
    } else if constexpr (std::same_as<T, double>) {
      return TestType{0.0, 1.1, 2.2, 3.3, 4.4, 5.5};
    } else if constexpr (std::same_as<T, bool>) {
      return TestType{true, false, false, false, true, true,
                      true, false, true,  false, true};
    } else if constexpr (std::same_as<T, std::string>) {
      return TestType{
          "one zero, zero zero one one, one zero one zero one",
          "three point one four one five nine two six five three five",
          "eighty-two eighty-two eighty-two, two hundred and forty-six total",
          "we've been trying to reach you about your car's extended warranty"};
    }
  }()};

  SECTION("Copy constructor from non-const source.") {
    auto copied{source};

    REQUIRE(source == copied);
    REQUIRE(source.data() != copied.data());
  }

  SECTION("Self copy assignment.") {
    auto data_ptr_before_copy{source.data()};
    source = source;

    REQUIRE(source.data() == data_ptr_before_copy);
  }

  SECTION("Copy assignment operator from non-const source.") {
    initial = source;

    REQUIRE(initial == source);
    REQUIRE(initial.data() != source.data());
  }

  SECTION("Copy constructor from const source.") {
    auto const &const_reference_source{source};
    auto copied{const_reference_source};

    REQUIRE(source == copied);
    REQUIRE(source.data() != copied.data());
  }

  SECTION("Copy assignment operator from const source.") {
    auto const &const_reference_to_source{source};
    initial = const_reference_to_source;

    REQUIRE(initial == source);
    REQUIRE(initial.data() != source.data());
  }

  SECTION("Move constructor from non-const source.") {
    auto known_good_copy{source};
    auto data_ptr_before_move{source.data()};
    auto moved{std::move(source)};

    REQUIRE(known_good_copy == moved);
    REQUIRE(source != moved);
    REQUIRE(moved.data() == data_ptr_before_move);

    REQUIRE(source.data() == nullptr);
    REQUIRE(source.size() == 0UZ);
    REQUIRE(source.capacity() == 0UZ);
  }

  SECTION("Self move assignment.") {
    auto known_good_copy{source};
    auto data_ptr_before_move{source.data()};
    auto size_before_move{source.size()};
    auto capacity_before_move{source.capacity()};
    auto &ref_to_self{source}; // To bypass -Wself-move.
    source = std::move(ref_to_self);

    REQUIRE(source == known_good_copy);
    REQUIRE(source.data() == data_ptr_before_move);
    REQUIRE(source.size() == size_before_move);
    REQUIRE(source.capacity() == capacity_before_move);
  }

  SECTION("Move assignment operator from non-const source.") {
    auto known_good_copy{source};
    auto data_ptr_before_move{source.data()};
    initial = std::move(source);

    REQUIRE(initial == known_good_copy);
    REQUIRE(initial != source);
    REQUIRE(initial.data() == data_ptr_before_move);

    // TODO: These might not be true depending on allocator propagation and
    // equality.
    REQUIRE(source.data() == nullptr);
    REQUIRE(source.size() == 0UZ);
    REQUIRE(source.capacity() == 0UZ);
  }

  SECTION("Move constructor from const source.") {
    auto known_good_copy{source};
    auto data_ptr_before_move{source.data()};
    auto const &const_reference_to_source{source};
    auto moved{std::move(const_reference_to_source)};

    REQUIRE(known_good_copy == moved);
    REQUIRE(source == moved);
    REQUIRE(moved.data() != data_ptr_before_move);

    REQUIRE(source.data() != nullptr);
    REQUIRE(source.size() != 0UZ);
    REQUIRE(source.capacity() != 0UZ);
  }

  SECTION("Move assignment operator from const source.") {
    auto known_good_copy{source};
    auto data_ptr_before_move{source.data()};
    auto const &const_reference_to_source{source};
    initial = std::move(const_reference_to_source);

    REQUIRE(initial == known_good_copy);
    REQUIRE(initial == source);
    REQUIRE(initial.data() != data_ptr_before_move);

    REQUIRE(source.data() != nullptr);
    REQUIRE(source.size() != 0UZ);
    REQUIRE(source.capacity() != 0UZ);
  }
}

TEST_CASE("Exception safety guarantees with throwing objects.", "[vector]") {
  struct ThrowingConstructor {
    int x{};

    ThrowingConstructor() { throw std::runtime_error("Do you leak?"); };
  };

  struct ThrowingCopyConstructor {
    int x{};
    float y{};
    std::string z{"Enough text so that we heap allocate the data."};

    auto operator<=>(ThrowingCopyConstructor const &other) const = default;

    ThrowingCopyConstructor() = default;
    ThrowingCopyConstructor(
        [[maybe_unused]] ThrowingCopyConstructor const &other) {
      throw std::runtime_error("Oh noes, I throws!");
    }
    ThrowingCopyConstructor(ThrowingCopyConstructor &&other) =
        delete ("Testing throwing copy constructor, don't move me.");
  };

  struct ThrowingMoveConstructor {
    int x{};
    float y{};
    std::string z{"Enough text so that we heap allocate the data."};

    auto operator<=>(ThrowingMoveConstructor const &other) const = default;

    ThrowingMoveConstructor() = default;
    ThrowingMoveConstructor(ThrowingMoveConstructor const &other) = default;
    ThrowingMoveConstructor([[maybe_unused]] ThrowingMoveConstructor &&other) {
      throw std::runtime_error("Oh noes, I throws!");
    }
  };

  struct ThrowingMoveOnlyObject {
    int x{};
    float y{};
    std::string z{"Enough text so that we heap allocate the data."};

    auto operator<=>(ThrowingMoveOnlyObject const &other) const = default;

    ThrowingMoveOnlyObject() = default;
    ThrowingMoveOnlyObject(ThrowingMoveOnlyObject const &other) = delete;
    ThrowingMoveOnlyObject([[maybe_unused]] ThrowingMoveOnlyObject &&other) {
      throw std::runtime_error("Oh noes, I throws!");
    }
  };

  SECTION("Object with throwing constructor.") {
    REQUIRE_THROWS_AS(swtl::Vector<ThrowingConstructor>(1), std::runtime_error);
  }

  SECTION("Object with throwing copy constructor.") {
    swtl::Vector<ThrowingCopyConstructor> throws_on_copy(3);
    swtl::Vector<ThrowingCopyConstructor> const expected(3);

    REQUIRE_THROWS_AS(throws_on_copy.emplace_back(), std::runtime_error);
    REQUIRE(throws_on_copy == expected);
    // Insertion fails, but the container remains in it's previous state.
  }

  SECTION("Object with throwing move constructor.") {
    swtl::Vector<ThrowingMoveConstructor> throws_on_move(3);
    swtl::Vector<ThrowingMoveConstructor> const expected(4);

    REQUIRE_NOTHROW(throws_on_move.emplace_back());
    REQUIRE(throws_on_move == expected);
    // Insertion succeeds because migration falls back to the copy constructor.
  }

  SECTION("Move only object with throwing move constructor.") {
    swtl::Vector<ThrowingMoveOnlyObject> throws_on_move(3);
    swtl::Vector<ThrowingMoveOnlyObject> const expected(3);

    // Insertion will fail, but no memory should be leaked and invariants should
    // hold.  Cannot guarantee the state of the contained elements.
    REQUIRE_THROWS_AS(throws_on_move.emplace_back(), std::runtime_error);
    REQUIRE(throws_on_move.size() == expected.size());
    REQUIRE(throws_on_move.capacity() >= expected.size());
    REQUIRE(throws_on_move.data() != nullptr);
  }
}

TEMPLATE_TEST_CASE("Element access, const & non-const.", "[vector]",
                   swtl::Vector<int>, swtl::Vector<int> const,
                   swtl::Vector<std::string>, swtl::Vector<std::string> const) {
  using T = typename std::remove_const_t<TestType>::value_type;
  using ExpectedQualifiedRef =
      std::conditional_t<std::is_const_v<TestType>, T const &, T &>;

  auto expected{[]() {
    if constexpr (std::is_same_v<T, int>) {
      return std::array<int, 5>{1, 2, 3, 4, 5};
    } else if constexpr (std::is_same_v<T, std::string>) {
      return std::array<std::string, 5>{"one", "two", "three", "four", "five"};
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
    STATIC_REQUIRE(std::is_same_v<decltype(vec.front()), ExpectedQualifiedRef>);
  }

  SECTION("Vector::back returns a reference to the last element.") {
    REQUIRE(vec.back() == expected[4]);
    STATIC_REQUIRE(std::is_same_v<decltype(vec.back()), ExpectedQualifiedRef>);
  }
}

// TODO: Add test case for double reserve to prevent the bug we just had to fix!
// TODO: Add test case for pointer overflow is max_size() + ptr > ptr max?
TEST_CASE("Reservation on an empty vector.", "[vector]") {
  swtl::Vector<int> vec;

  SECTION("Reservation increases capacity but does not affect size.") {
    auto const initial_capacity{10UZ};
    vec.reserve(initial_capacity);

    REQUIRE(vec.is_empty());
    REQUIRE(vec.size() == 0UZ);
    REQUIRE(vec.capacity() >= initial_capacity);

    SECTION("Continued reservation grows capacity again.") {
      auto const final_capacity{20UZ};
      vec.reserve(final_capacity);

      REQUIRE(vec.capacity() >= final_capacity);

      SECTION("Reserving less than the current capacity does nothing.") {
        auto const before_resize_attempt{vec.capacity()};
        vec.reserve(initial_capacity);

        REQUIRE(vec.capacity() == before_resize_attempt);
      }
    }
  }

  SECTION("Reserving more than the maximum number of elements throws.") {
    REQUIRE_THROWS_AS(vec.reserve(std::numeric_limits<std::size_t>::max()),
                      std::length_error);
  }
}

TEST_CASE("Reservation on a populated vector.", "[vector]") {
  swtl::Vector<int> vec{1, 2, 3};
  swtl::Vector<int> const expected{vec};
  auto const initial_capacity{vec.capacity()};

  // Capacity may be greater than requested due to using allocate_at_least().

  SECTION("Reservation grows capacity but does not modify elements.") {
    auto new_capacity{10UZ};
    vec.reserve(new_capacity);

    REQUIRE(vec == expected);
    REQUIRE(vec.size() == expected.size());
    REQUIRE(vec.capacity() >= new_capacity);

    SECTION("Continued reservation grows capacity again.") {
      auto const final_capacity{20UZ};
      vec.reserve(final_capacity);

      REQUIRE(vec == expected);
      REQUIRE(vec.size() == expected.size());
      REQUIRE(vec.capacity() >= final_capacity);

      SECTION("Reserving less than the current capacity does nothing.") {
        vec.reserve(initial_capacity);

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
