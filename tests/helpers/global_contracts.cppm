export module swtl.test.helpers:global_contracts;

import std;

export class ContractException : std::exception
{
public:
   constexpr ContractException(std::string_view message)
       : msg{ message }
   {}

   constexpr char const *
   what() const noexcept
   {
      return msg.c_str();
   }

private:
   std::string msg;
};

export void
handle_contract_violation(std::contracts::contract_violation const &violation)
{
   throw ContractException(
       std::format(
           "Contract Violation: {}\nLocation: {}:{}",
           violation.comment(),
           violation.location().file_name(),
           violation.location().line()));
}
