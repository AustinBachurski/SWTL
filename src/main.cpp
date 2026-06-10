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

} // namespace

auto main() -> int {
  {
    std::println("Creating <int> vector.");

    swtl::Vector<int> vec;
    std::println("{}", static_cast<void *>(vec.data()));
    std::println("Size: {}, Capacity: {}", vec.size(), vec.capacity());
    printAll(vec);

    vec.push_back(1);
    std::println("{}", static_cast<void *>(vec.data()));
    std::println("Size: {}, Capacity: {}", vec.size(), vec.capacity());
    printAll(vec);

    vec.push_back(2);
    std::println("{}", static_cast<void *>(vec.data()));
    std::println("Size: {}, Capacity: {}", vec.size(), vec.capacity());
    printAll(vec);

    vec.push_back(3);
    std::println("{}", static_cast<void *>(vec.data()));
    std::println("Size: {}, Capacity: {}", vec.size(), vec.capacity());
    printAll(vec);

    vec.push_back(4);
    std::println("{}", static_cast<void *>(vec.data()));
    std::println("Size: {}, Capacity: {}", vec.size(), vec.capacity());
    printAll(vec);

    vec.push_back(5);
    std::println("{}", static_cast<void *>(vec.data()));
    std::println("Size: {}, Capacity: {}", vec.size(), vec.capacity());
    printAll(vec);

    vec.push_back(6);
    std::println("{}", static_cast<void *>(vec.data()));
    std::println("Size: {}, Capacity: {}", vec.size(), vec.capacity());
    printAll(vec);

    vec.push_back(7);
    std::println("{}", static_cast<void *>(vec.data()));
    std::println("Size: {}, Capacity: {}", vec.size(), vec.capacity());
    printAll(vec);

    vec.push_back(8);
    std::println("{}", static_cast<void *>(vec.data()));
    std::println("Size: {}, Capacity: {}", vec.size(), vec.capacity());
    printAll(vec);

    vec.push_back(9);
    std::println("{}", static_cast<void *>(vec.data()));
    std::println("Size: {}, Capacity: {}", vec.size(), vec.capacity());
    printAll(vec);

    std::println("Destroying <int> vector.\n");
  }
  {
    std::println("Creating <std::string> vector.");

    swtl::Vector<std::string> vec;
    std::println("{}", static_cast<void *>(vec.data()));
    std::println("Size: {}, Capacity: {}", vec.size(), vec.capacity());
    printAll(vec);

    vec.push_back("string w/value: 1");
    std::println("{}", static_cast<void *>(vec.data()));
    std::println("Size: {}, Capacity: {}", vec.size(), vec.capacity());
    printAll(vec);

    vec.push_back("string w/value: 2");
    std::println("{}", static_cast<void *>(vec.data()));
    std::println("Size: {}, Capacity: {}", vec.size(), vec.capacity());
    printAll(vec);

    vec.push_back("string w/value: 3");
    std::println("{}", static_cast<void *>(vec.data()));
    std::println("Size: {}, Capacity: {}", vec.size(), vec.capacity());
    printAll(vec);

    vec.push_back("string w/value: 4");
    std::println("{}", static_cast<void *>(vec.data()));
    std::println("Size: {}, Capacity: {}", vec.size(), vec.capacity());
    printAll(vec);

    vec.push_back("string w/value: 5");
    std::println("{}", static_cast<void *>(vec.data()));
    std::println("Size: {}, Capacity: {}", vec.size(), vec.capacity());
    printAll(vec);

    vec.push_back("string w/value: 6");
    std::println("{}", static_cast<void *>(vec.data()));
    std::println("Size: {}, Capacity: {}", vec.size(), vec.capacity());
    printAll(vec);

    vec.push_back("string w/value: 7");
    std::println("{}", static_cast<void *>(vec.data()));
    std::println("Size: {}, Capacity: {}", vec.size(), vec.capacity());
    printAll(vec);

    vec.push_back("string w/value: 8");
    std::println("{}", static_cast<void *>(vec.data()));
    std::println("Size: {}, Capacity: {}", vec.size(), vec.capacity());
    printAll(vec);

    vec.push_back("string w/value: 9");
    std::println("{}", static_cast<void *>(vec.data()));
    std::println("Size: {}, Capacity: {}", vec.size(), vec.capacity());
    printAll(vec);

    std::println("Destroying <std::string> vector.\n");
  }
  //{
  //  std::println("Creating <int> forward_list.");

  //  Forward_List<int> empty;
  //  Forward_List<int> list;
  //  list.push_front(42);
  //  list.push_front(41);
  //  list.push_front(40);
  //  list.push_front(39);
  //  list.push_front(38);
  //  list.push_front(37);

  //  std::print("Empty: ");
  //  print_list(empty);

  //  std::print("Populated: ");
  //  print_list(list);

  //  empty = list;
  //  std::print("Post Copy Assignment: ");
  //  print_list(empty);

  //  auto copied{list};
  //  std::print("Copy Constructor: ");
  //  print_list(copied);

  //  auto moved{std::move(empty)};
  //  std::print("Move Constructor: ");
  //  print_list(moved);
  //  std::print("Moved From: ");
  //  print_list(empty);

  //  empty = std::move(list);
  //  std::print("Move Assignment: ");
  //  print_list(empty);
  //  std::print("Moved From: ");
  //  print_list(list);

  //  std::println("Destroying <int> forward_list.\n");
  //}
  //{
  //  std::println("Creating <std::string> forward_list.");

  //  Forward_List<std::string> empty;
  //  Forward_List<std::string> list;
  //  list.push_front("string w/value: 42");
  //  list.push_front("string w/value: 41");
  //  list.push_front("string w/value: 40");
  //  list.push_front("string w/value: 39");
  //  list.push_front("string w/value: 38");
  //  list.push_front("string w/value: 37");

  //  std::print("Empty: ");
  //  print_list(empty);

  //  std::print("Populated: ");
  //  print_list(list);

  //  empty = list;
  //  std::print("Post Copy Assignment: ");
  //  print_list(empty);

  //  auto copied{list};
  //  std::print("Copy Constructor: ");
  //  print_list(copied);

  //  auto moved{std::move(empty)};
  //  std::print("Move Constructor: ");
  //  print_list(moved);
  //  std::print("Moved From: ");
  //  print_list(empty);

  //  empty = std::move(list);
  //  std::print("Move Assignment: ");
  //  print_list(empty);
  //  std::print("Moved From: ");
  //  print_list(list);

  //  std::println("Destroying <std::string> forward_list.\n");
  //}

  constexpr auto from_constexpr{does_it_work()};

  std::println("Values from constexpr swtl::Vector:\n{}", from_constexpr);

  {
    swtl::Vector<std::string> vec;

    vec.push_back("I");
    vec.push_back("used");
    vec.push_back("to");
    vec.push_back("be");
    vec.push_back("an");
    vec.push_back("adventurer");
    vec.push_back("like");
    vec.push_back("you");

    // copy ctor
    auto new_vec{vec};
    new_vec.push_back("but I took an arrow to the knee...");

    // copy assign
    new_vec = vec;

    // move ctor
    auto stolen{std::move(vec)};

    // move assign
    stolen = std::move(new_vec);

    printAll(stolen);
  }

  {
    swtl::Vector<int> non_const_vec;
    swtl::Vector<int> const const_vec;

    test_non_const<int>(non_const_vec.begin());
    test_const<int>(const_vec.begin());
    test_const<int>(non_const_vec.begin());
    // test_non_const<int>(const_vec.begin()); // Shound fail to compile.
  }
}
