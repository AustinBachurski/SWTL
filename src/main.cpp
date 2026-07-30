import std;
import swtl_vector;
import swtl_forward_list;

import swtl_test_helper_functions;
import swtl_test_helper_objects;

namespace helpers = swtl_test_helpers;

void
handle_contract_violation(std::contracts::contract_violation const &violation)
{
   throw std::logic_error(
       std::format(
           "Contract Violation: {}\nLocation: {}:{}",
           violation.comment(),
           violation.location().file_name(),
           violation.location().line()));
}

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

template <
    std::input_iterator InputIterator,
    std::sentinel_for<InputIterator> Sentinel
>
constexpr void
func(
    [[maybe_unused]] InputIterator src_begin, [[maybe_unused]] Sentinel src_end)
    pre(!std::sized_sentinel_for<Sentinel, InputIterator>
        || (src_end - src_begin >= 0
            && "Are your iterator arguments backwards?"))
{
   std::puts("entered function body");
   if constexpr (std::sized_sentinel_for<Sentinel, InputIterator>)
   {
      std::puts("if constexpr entered");
      contract_assert(
          src_end - src_begin >= 0 && "Exploded in the function body!");
   }

   std::puts("function exit success");
}

int
main()
{
   auto const source{
      helpers::generate_populated_container<swtl::Vector<int>>()
   };
   swtl::Vector<int> vec;

   helpers::TestInputIterator ti{ source.data() };
   helpers::TestInputIterator tie{ source.data() + source.size() };

   /*
   func(source.begin(), source.end());
   std::puts("that should've worked");

   func(ti, tie);
   std::puts("that should've worked too");

   func(source.end(), source.begin());
   std::puts("rut roe...");
   */

   vec.assign(source.begin(), source.end());
   std::puts("that should've worked");

   vec.assign(ti, tie);
   std::puts("that should've too");

   vec.assign(source.end(), source.begin());
   std::puts("rut roe...");
}
