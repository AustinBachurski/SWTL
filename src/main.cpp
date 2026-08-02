import std;

import swtl;
import swtl.test_helpers;

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

}  // namespace

int
main()
{}
