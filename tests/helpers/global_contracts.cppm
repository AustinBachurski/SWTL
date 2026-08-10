export module swtl.test.helpers:global_contracts;

import std;

/// @cond INTERNAL_DOCUMENTATION

/// @brief Exception class used for testing contract assertions.
///
export class ContractException : std::exception
{
public:
   /// @brief Constructor taking a string with details about the contract
   /// violation.
   ///
   constexpr ContractException(std::string_view message)
       : msg{ message }
   {}

   /// @brief Extract the message in the exception.
   ///
   constexpr char const *
   what() const noexcept
   {
      return msg.c_str();
   }

private:
   /// @brief The message in the exception.
   ///
   std::string msg;
};

/// @brief Throwing contract violation handler used for testing contract
/// assertions.
///
/// @throws ContractException When called, which contains details about the
/// contract violation in the message of the exception.
///
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

/// @endcond INTERNAL_DOCUMENTATION
