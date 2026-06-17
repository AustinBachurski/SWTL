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

struct ThrowingMoveConstructor {
  int x{};
  float y{};
  std::string z{"Enough text so that we heap allocate the data."};

  auto
  operator<=>(ThrowingMoveConstructor const &other) const noexcept = default;

  ThrowingMoveConstructor() = default;
  ThrowingMoveConstructor([[maybe_unused]] ThrowingMoveConstructor &&other) {
    throw std::runtime_error("Oh noes, I throws!");
  }
};

auto main() -> int {
  std::puts("");
  std::puts("");
}
