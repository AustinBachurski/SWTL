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
  std::puts("");
  swtl::Vector a{1, 2, 3, 4, 5};
  swtl::Vector b{1, 2, 3, 4, 5};
  swtl::Vector c{2, 2, 3, 4, 5};
  swtl::Vector d{1, 2, 3, 4, 5, 6};

  std::println("a==b: {}", a == b);
  std::println("a!=b: {}", a != b);
  std::println("c<d: {}", c < d);
  std::println("c>d: {}", c > d);
}
