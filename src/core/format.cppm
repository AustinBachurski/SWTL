export module swtl_format;

import std;

namespace swtl::format
{

export template <std::integral T>
constexpr std::string
integral_to_string(T value)
{
   std::size_t const buffer_size{ 20 };  // Number of digits in 64 bit size_t.
   char buffer[buffer_size]{};
   auto const [end, ec]{ std::to_chars(buffer, buffer + buffer_size, value) };

   if (ec != std::errc())
   {
      if consteval
      {
         throw std::out_of_range(
             "swtl::format::integral_to_string() internal buffer overflow!");
      }
      return "[formatting error encountered]";
   }
   else
   {
      return std::string(buffer, end);
   }
}

}  // namespace swtl::format
