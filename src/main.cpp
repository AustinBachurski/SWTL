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

struct CustomObject {
  std::unique_ptr<int> p = std::make_unique<int>(42);
};

struct ThrowingObject {
  int x{};

  ThrowingObject() { throw std::exception(); }
};

auto main() -> int {
  std::puts("");
  std::puts("");

<<<<<<< HEAD
  swtl::Vector<int> vec;
  std::println("Before: {}", vec.capacity());
  vec.reserve(10);

  vec.push_back(1);
  vec.push_back(2);
  vec.push_back(3);
  vec.push_back(4);
  vec.push_back(5);

  std::println("After: {}", vec.capacity());
  vec.reserve(100);
=======
  swtl::Vector<ThrowingObject> vec(10);
>>>>>>> cc0dc39 (Exception concerns and implementation refactor TODOs.)
}
