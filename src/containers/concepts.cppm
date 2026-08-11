export module swtl.container_concepts;

import std;

/// @brief Concept supporting a `container-compatible range`.
///
export template <typename Range, typename T>
concept container_compatible_range
    = std::ranges::input_range<Range>
   && std::convertible_to<std::ranges::range_reference_t<Range>, T>;
