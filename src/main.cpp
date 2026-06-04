#include "vector.hpp"

#include <print>
#include <string>

namespace {
template <typename T>
auto printAll(StrictlyWorse::Vector<T> const &vec) -> void {
  if (vec.size() == 0) {
    std::println("Empty...");
    return;
  }

  for (auto const idx : std::views::iota(0UZ, vec.size())) {
    std::print("{}, ", vec.data()[idx]);
  }
  std::println();
}
} // namespace

auto main() -> int {
  {
    std::println("Creating <int> vector.");

    StrictlyWorse::Vector<int> vec;
    std::println("{}", static_cast<void *>(vec.data()));
    printAll(vec);

    vec.push_back(1);
    std::println("{}", static_cast<void *>(vec.data()));
    printAll(vec);

    vec.push_back(2);
    std::println("{}", static_cast<void *>(vec.data()));
    printAll(vec);

    vec.push_back(3);
    std::println("{}", static_cast<void *>(vec.data()));
    printAll(vec);

    vec.push_back(4);
    std::println("{}", static_cast<void *>(vec.data()));
    printAll(vec);

    vec.push_back(5);
    std::println("{}", static_cast<void *>(vec.data()));
    printAll(vec);

    vec.push_back(6);
    std::println("{}", static_cast<void *>(vec.data()));
    printAll(vec);

    vec.push_back(7);
    std::println("{}", static_cast<void *>(vec.data()));
    printAll(vec);

    vec.push_back(8);
    std::println("{}", static_cast<void *>(vec.data()));
    printAll(vec);

    std::println("Destroying <int> vector.\n");
  }
  {
    std::println("Creating <std::string> vector.");

    StrictlyWorse::Vector<std::string> vec;
    std::println("{}", static_cast<void *>(vec.data()));
    printAll(vec);

    vec.push_back("loooooooooooong string w/value: 1");
    std::println("{}", static_cast<void *>(vec.data()));
    printAll(vec);

    vec.push_back("loooooooooooong string w/value: 2");
    std::println("{}", static_cast<void *>(vec.data()));
    printAll(vec);

    vec.push_back("loooooooooooong string w/value: 3");
    std::println("{}", static_cast<void *>(vec.data()));
    printAll(vec);

    vec.push_back("loooooooooooong string w/value: 4");
    std::println("{}", static_cast<void *>(vec.data()));
    printAll(vec);

    vec.push_back("loooooooooooong string w/value: 5");
    std::println("{}", static_cast<void *>(vec.data()));
    printAll(vec);

    vec.push_back("loooooooooooong string w/value: 6");
    std::println("{}", static_cast<void *>(vec.data()));
    printAll(vec);

    vec.push_back("loooooooooooong string w/value: 7");
    std::println("{}", static_cast<void *>(vec.data()));
    printAll(vec);

    vec.push_back("loooooooooooong string w/value: 8");
    std::println("{}", static_cast<void *>(vec.data()));
    printAll(vec);

    std::println("Destroying <std::string> vector.\n");
  }
}
