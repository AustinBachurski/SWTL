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
}
