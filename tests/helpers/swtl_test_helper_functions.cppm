export module swtl_test_helper_functions;

import std;
import swtl_vector;

export namespace swtl_test_helpers
{

template <typename Container>
concept VectorLike
    = std::same_as<Container, std::vector<typename Container::value_type>>
   || std::same_as<Container, swtl::Vector<typename Container::value_type>>;

template <VectorLike Container>
auto
generate_populated_container()
{
   using T = typename Container::value_type;

   return []() -> Container
   {
      /*
      Currently a bug that prevents this from linking with libstdc++,
      in gcc 16.1, appears to be fixed in gcc(trunk).

      if constexpr (std::same_as<T, char>) {
        return {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',
      'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y',
      'z'};
      */
      if constexpr (std::same_as<T, unsigned char>)
      {
         return { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i',
                  'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',
                  's', 't', 'u', 'v', 'w', 'x', 'y', 'z' };
      }
      else if constexpr (std::same_as<T, bool>)
      {
         return { true, false, false, false, true, true,
                  true, false, true,  false, true };
      }
      else if constexpr (std::same_as<T, int>)
      {
         return { 0, 1, 2, 3, 4, 5 };
      }
      else if constexpr (std::same_as<T, double>)
      {
         return { 0.0, 1.1, 2.2, 3.3, 4.4, 5.5 };
      }
      else if constexpr (std::same_as<T, std::string>)
      {
         return { "one zero, zero zero one one, one zero one zero one",
                  "three point one four one five nine two six five three five",
                  "eighty-two eighty-two eighty-two, two hundred and forty-six "
                  "total",
                  "we've been trying to reach you about your car's extended "
                  "warranty" };
      }
      else
      {
         throw std::invalid_argument(
             "Missing conditional block to generate "
             "values for the passed in VectorLike type.");
      }
   }();
}

template <typename T>
swtl::Vector<T>
generate_vector_of_count(std::size_t count)
{
   swtl::Vector<T> vec;
   vec.reserve(count);

   for (auto const value : std::views::iota(0UZ, count))
   {
      vec.emplace_back(value);
   }

   return vec;
}

}  // namespace swtl_test_helpers
