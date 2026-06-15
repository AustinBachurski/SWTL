import std;
import swtl_vector;
import swtl_forward_list;

namespace {

template <typename T> auto printAll(swtl::Vector<T> const &vec) -> void {
  if (vec.size() == 0) {
    std::println("Empty...");
    return;
  }

  for (auto const &element : vec | std::views::reverse) {
    std::print("{}, ", element);
  }
  std::println();
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

} // namespace

auto main() -> int {
  swtl::Vector<int> vec;
  vec.reserve(4);

  std::println("Size: {}, Capacity: {}", vec.size(), vec.capacity());
  vec.push_back(42);
  std::println("Size: {}, Capacity: {}", vec.size(), vec.capacity());
  vec.push_back(42);
  std::println("Size: {}, Capacity: {}", vec.size(), vec.capacity());
  vec.push_back(42);
  std::println("Size: {}, Capacity: {}", vec.size(), vec.capacity());
  vec.push_back(42);
  std::println("Size: {}, Capacity: {}", vec.size(), vec.capacity());

  printAll(vec);

  vec.push_back(42);
  std::println("Should Resize Here!");
  std::println("Size: {}, Capacity: {}", vec.size(), vec.capacity());
}
