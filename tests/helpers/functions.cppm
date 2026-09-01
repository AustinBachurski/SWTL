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

/// @brief Returns a swtl::Vector<T> with `count` elements with incrementing
/// values.
///
/// Used to generate test data for unit tests.
///
/// @tparam T The template argument for the vector.
///
/// @return A new swtl::Vector<T> with `count` elements with incrementing
/// values.
///
/// @note Since the default type pack for testing includes a `std::uint8_t`, the
/// reference value is of type `std::uint8_t`; this means that ids will go to
/// 255 and wrap if you request a large amount of elements.  Well defined, just
/// be aware of that.
///
template <typename T>
constexpr swtl::Vector<T>
generate_vector(std::size_t count = 8UZ)
{
   swtl::Vector<T> vec;

   std::uint8_t reference_value{};

   for (; count != 0UZ; --count)
   {
      vec.emplace_back(reference_value++);
   }

   return vec;
}

/// @brief Returns a swtl::Vector<T> with elements from the argument list.
///
/// Used to generate test data that can't be used with a braced initializer
/// list.
///
/// @tparam T The template argument for the vector.
///
/// @return A new swtl::Vector<T> with elements from the argument list.
///
template <typename T, typename... Args>
constexpr auto
make_vec_of(Args &&...args)
{
   swtl::Vector<T> vec;
   vec.reserve(sizeof...(Args));

   (vec.emplace_back(std::forward<Args>(args)), ...);

   return vec;
}

}  // namespace swtl::test_helpers

/// @endcond
