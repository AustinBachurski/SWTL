export module swtl.test.helpers:global_contracts;

import std;

// Intentionally in the global namespace.

/// @cond INTERNAL_DOCUMENTATION

/// @brief Exception class used for testing contract assertions.
///
export class ContractException : std::runtime_error
{
public:
   /// @brief Constructor taking a string with details about the contract
   /// violation.
   ///
   /// @param message The message to include in the exception.
   ///
   constexpr ContractException(std::string_view message)
       : std::runtime_error{ std::string{ message } }
   {}
};

/// @brief Throwing contract violation handler used for testing contract
/// assertions.
///
/// @param violation Reference to the contract_violation object.
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

/// @endcond
