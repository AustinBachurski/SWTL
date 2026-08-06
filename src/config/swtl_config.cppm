export module swtl.config;

namespace swtl::config
{

// Used to disable nothrow during testing so that contract assertions can be
// properly tested via exceptions.
#if SWTL_TESTING_DISABLE_NOEXCEPT
/// @internal
/// @brief Disable `noexcept` during testsing for functions that use
/// contract_assert.
export inline constexpr bool nothrow_contracts{ false };
#else
/// @internal
/// @brief Enable `noexcept` outside of testing.
export inline constexpr bool nothrow_contracts{ true };
#endif

}  // namespace swtl::config
