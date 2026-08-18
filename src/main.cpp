import std;

import swtl;
import swtl.test.helpers;

namespace helpers = swtl::test_helpers;

namespace
{

struct S
{
   S()
   {
      std::puts("S()");
   };

   S(S const &)
   {
      std::puts("S(S const &)");
   }

   S(S &&) noexcept
   {
      std::puts("S(S &&)");
   }

   S &
   operator=(S const &)
   {
      std::puts("operator=(S const &)");
      return *this;
   }

   S &
   operator=(S &&) noexcept
   {
      std::puts("operator=(S &&)");
      return *this;
   }

   ~S()
   {
      std::puts("~S()");
   }
};

template <typename T>
void
printAll(swtl::Vector<T> const &vec)
{
   if (vec.size() == 0)
   {
      std::println("Empty...");
      return;
   }

   for (auto const &element : vec)
   {
      std::print("{}, ", element);
   }
   std::println();
}

}  // namespace

int
main()
{
   S s;

   swtl::Vector<S> vec(10);
   vec.reserve(20);

   std::puts("\n\n\nbegin test");

   auto const iter{ vec.begin() + 4 };

   vec.emplace(iter, std::move(s));

   std::puts("\nend\n");
}
