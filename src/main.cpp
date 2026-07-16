import std;
import swtl_vector;
import swtl_forward_list;

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
}
