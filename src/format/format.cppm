export module swtl.format;

import std;

namespace swtl
{

/// @brief Converts an integral number to a string at constexpr time.
///
/// @param value Integral number to convert.
///
/// @return A std::string representation of `value`.
///
/// @note Will return "[formatting error encountered]" on run-time parse error,
/// results in a compile error during constexpr evaluation.  Considering type
/// limitiations, ideally 'this shouldn't happen'...
///
export template <std::integral T>
constexpr std::string
integral_to_string(T value)
{
   // +1 for a negative sign.
   std::size_t const buffer_size{ std::numeric_limits<T>::digits10 + 1 };
   char buffer[buffer_size]{};
   auto const [end, ec]{ std::to_chars(buffer, buffer + buffer_size, value) };

   if (ec != std::errc())
   {
      if consteval
      {
         throw std::out_of_range(
             "swtl::integral_to_string(T value) parsing failed.");
      }
      return "[formatting error encountered]";
   }
   else
   {
      return std::string(buffer, end);
   }
}

}  // namespace swtl
