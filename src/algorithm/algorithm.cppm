export module swtl.algorithm;

import std;

import swtl.config;

namespace swtl
{

/// @brief Holds the result of a zip_copy operation.
///
/// Similar to `std::ranges::mismatch_result`, contains two iterators that
/// represent the positions at which the ranges diverged while the elements of
/// source were copied to destination, i.e. `source.begin() == source.end()` or
/// `destination.begin() == destination.end()` while `*source++` is copied to
/// `*destination++`.
///
export template <
    std::input_iterator SourceIterator,
    std::input_or_output_iterator DestinationIterator
>
struct ZipCopyResult
{
   /// The source iterator after the zip_copy.
   SourceIterator src_pos;
   /// The destination iterator after the zip_copy.
   DestinationIterator dest_pos;
};

/// @brief Copies elements from source to dest while the two ranges are valid.
///
/// A mix of std::copy and std::mismatch, it compares the begin and end
/// iterators of source and dest before each copy.  Returning a ZipCopyResult
/// that contains the positions at which the ranges diverged.
///
/// @param src_first The `begin()` iterator for the source range.
/// @param src_last The `end()` iterator for the source range.
/// @param dest_first The `begin()` iterator for the destination range.
/// @param dest_last The `end()` iterator for the destination range.
///
/// @pre `[src_first, src_last)` must denote a valid range.
/// @pre `[dest_first, dest_last)` must denote a valid range.
///
/// @return A ZipCopyResult containing iterators to the source range and
/// destination range at the point where the ranges diverged.
///
/// @note `noexcept` if assigning the value referenced by the source iterator to
/// the destination iterator is nothrow.
/// @note `noexcept` is disabled during testing.
///
export template <
    std::input_iterator SourceIterator,
    std::sentinel_for<SourceIterator> SourceSentinel,
    std::input_or_output_iterator DestinationIterator,
    std::sentinel_for<DestinationIterator> DestinationSentinel
>
constexpr ZipCopyResult<SourceIterator, DestinationIterator>
zip_copy(
    SourceIterator src_first,
    SourceSentinel src_last,
    DestinationIterator dest_first,
    DestinationSentinel dest_last)
    noexcept(
        swtl::config::nothrow_contracts && noexcept(*dest_first = *src_first))
{
   if constexpr (std::sized_sentinel_for<SourceSentinel, SourceIterator>)
   {
      contract_assert(
          src_last - src_first >= 0
          && "`src_last` must be reachable from `src_first`");
   }

   if constexpr (
       std::sized_sentinel_for<DestinationSentinel, DestinationIterator>)
   {
      contract_assert(
          dest_last - dest_first >= 0
          && "`dest_last` must be reachable from `dest_first`");
   }

   while (src_first != src_last && dest_first != dest_last)
   {
      *dest_first++ = *src_first++;
   }

   return { .src_pos = src_first, .dest_pos = dest_first };
}

/// @brief Copies elements from source to dest while the two ranges are valid.
///
/// A mix of std::copy and std::mismatch, it compares the begin and end
/// iterators of source and dest before each copy.  Returning a ZipCopyResult
/// that contains the positions at which the ranges diverged.
///
/// @param src_range The source range to copy from.
/// @param dest_range The destination range to copy to.
///
/// @return A ZipCopyResult containing iterators to the source range and
/// destination range at the point where the ranges diverged.
///
/// @note `noexcept` if assignment from the source iterator's reference type to
/// the destination iterator's reference type is nothrow.
/// @note `noexcept` is disabled during testing.
///
export template <
    std::ranges::range SourceRange,
    std::ranges::range DestinationRange
>
constexpr auto
zip_copy(SourceRange &&src_range, DestinationRange &&dest_range) noexcept(
    swtl::config::nothrow_contracts
    && noexcept(
        *std::ranges::begin(dest_range) = *std::ranges::begin(src_range)))
{
   return zip_copy(
       std::ranges::begin(src_range),
       std::ranges::end(src_range),
       std::ranges::begin(dest_range),
       std::ranges::end(dest_range));
}

}  // namespace swtl
