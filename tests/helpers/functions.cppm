export module swtl.test.helpers:functions;

import std;

import swtl.vector;

import :objects;

/// @cond INTERNAL_DOCUMENTATION

export namespace swtl::test_helpers
{

/// @brief Fills a container to capacity via `emplace_back()`.
///
/// Populates all available memory in a container by repeatedly calling
/// `container.emplace_back()` until `container.size() == container.capacity()`.
///
/// @param container Reference to the container to fill.
///
/// @pre `container` must support `emplace_back()`, `size()`, and `capacity()`.
///
template <typename T>
constexpr void
fill_to_capacity(T &container)
{
   for (auto size{ container.size() }; size < container.capacity(); ++size)
   {
      container.emplace_back();
   }
}

/// @brief Concept supporting construction from a braced initializer list.
///
template <typename Container>
concept BracedListConstructible
    = requires { Container{ *std::declval<Container>().begin() }; };

/// @brief Returns a `Container` populated with predefined values.
///
/// @tparam Container The container and type to populate; i.e.
/// `std::vector<T>` where `T` is a supported type.
///
/// @pre Container must be constructible from a braced initializer list.
///
/// @note Supported Types: `unsigned char`, `bool`, `int`, `double`,
/// `std::string`.
///
/// @throws std::invalid_argument If `T` is an unsupported type.
///
template <BracedListConstructible Container>
auto
generate_populated()
{
   using T = typename Container::value_type;

   return []() -> Container
   {
      /*
      Currently a bug that prevents this from linking with libstdc++,
      in gcc 16.1, still broken in 16.2, appears to be fixed in gcc(trunk).

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

/// @brief Concept for a container that supports `emplace_back()` with a
/// std::size_t value.
///
template <typename Container>
concept EmplaceableWithUniqueID
    = requires(Container c, std::size_t val) { c.emplace_back(val); };

/// @brief Returns a `Container` with `count` `value_type`s with incrementing id
/// values.
///
/// @tparam Container The container and to populate; i.e.,
/// `std::vector<TrackedObject>`.
///
/// @param count The number of elements to be placed in the container.
///
/// @return A `Container` populated with `count` `value_type`s where
/// `value_type.id` increases per element from zero to `count - 1`.
///
/// @pre `Container` where `Container` supports `emplace_back()` and
/// `Container::value_type` supports argument construction with a std::size_t.
///
template <EmplaceableWithUniqueID Container>
constexpr Container
generate_unique(std::size_t count)
{
   Container container;
   container.reserve(count);

   for (auto const value : std::views::iota(0UZ, count))
   {
      container.emplace_back(value);
   }

   return container;
}

}  // namespace swtl::test_helpers

/// @endcond
