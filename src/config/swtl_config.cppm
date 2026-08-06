export module swtl.config;

namespace swtl::config
{

// Used to disable nothrow during testing so that contract assertions can be
// properly tested via exceptions.
#if SWTL_TESTING_DISABLE_NOEXCEPT
export inline constexpr bool nothrow_contracts{ false };
#else
export inline constexpr bool nothrow_contracts{ true };
#endif

}  // namespace swtl::config
