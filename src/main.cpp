#include <type_traits>
import std;
import swtl_vector;
import swtl_forward_list;

namespace {

template <typename T> auto printAll(swtl::Vector<T> const &vec) -> void {
  if (vec.size() == 0) {
    std::println("Empty...");
    return;
  }

  for (auto const &element : vec) {
    std::print("{}, ", element);
  }
  std::println();
}

void print_list(auto const &list) {
  std::print("[");

  bool first{true};

  for (auto const &elem : list) {
    if (first) {
      std::print(" {}", elem);
      first = false;
    } else {
      std::print(", {}", elem);
    }
  }

  std::println(" ]");
}

template <typename T> auto test_non_const(swtl::VectorIterator<T> it) -> void {
  ++it;
  std::println("Non-const Iterator Accepted");
}

template <typename T>
auto test_const(swtl::VectorIterator<T const> it) -> void {
  ++it;
  std::println("Const Iterator Accepted");
}

consteval auto does_it_work() {
  constexpr auto count{10UZ};
  swtl::Vector<std::size_t> vec;

  for (auto const num : std::views::iota(0UZ, count)) {
    vec.push_back(num);
  }

  std::array<std::size_t, count> values;

  for (auto &&[idx, num] : std::views::enumerate(vec)) {
    values.at(idx) = num;
  }

  return values;
}

consteval auto get_constexpr_val() {
  swtl::Vector<int> vec;
  vec.push_back(69);
  return vec.at(0);
}

} // namespace

auto main() -> int {
  swtl::Vector<int> vec;
  vec.push_back(42);
  const auto const_vec{vec};

  std::println("Pointer should not be const: {}",
               std::is_const_v<std::remove_reference_t<decltype(vec.data())>>);
  std::println(
      "Pointer should not be const: {}",
      std::is_const_v<std::remove_reference_t<decltype(const_vec.data())>>);

  std::println(
      "Should not be const: {}",
      std::is_const_v<std::remove_reference_t<decltype(vec.data()[0])>>);
  std::println(
      "Should be const: {}",
      std::is_const_v<std::remove_reference_t<decltype(const_vec.data()[0])>>);
}
