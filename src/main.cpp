import std;
import swtl_vector;
import swtl_forward_list;

import swtl_test_helper_functions;
import swtl_test_helper_objects;

namespace helpers = swtl_test_helpers;

namespace
{

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

consteval auto
does_it_work()
{
   constexpr auto count{ 10UZ };
   swtl::Vector<std::size_t> vec;

   for (auto const num : std::views::iota(0UZ, count))
   {
      vec.push_back(num);
   }

   std::array<std::size_t, count> values;

   for (auto &&[idx, num] : std::views::enumerate(vec))
   {
      values.at(idx) = num;
   }

   return values;
}

}  // namespace

int
main()
{
   std::puts("");
   std::puts("");

   auto source{ helpers::generate_vector_of_count<helpers::TestObject>(42UZ) };
   helpers::reset_instance_counts_of<helpers::TestObject>();
   swtl::Vector<helpers::TestObject> vec(50);

   vec.assign(source.begin(), source.end());

   std::println("{}", helpers::TestObject::instances_alive());
}
