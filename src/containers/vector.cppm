export module swtl_vector;

import std;
import swtl_format;
import swtl_memory;

namespace swtl {

export template <typename T> class VectorIterator {
public:
  using iterator_category = std::contiguous_iterator_tag;
  using value_type = std::remove_cv_t<T>;
  using difference_type = std::ptrdiff_t;
  using pointer = T *;
  using reference = T &;

  template <typename U> friend class VectorIterator;

  template <typename U>
    requires std::is_const_v<T> && std::same_as<U, std::remove_const_t<T>>
  constexpr VectorIterator(VectorIterator<U> const &other) : ptr_{other.ptr_} {}

  constexpr VectorIterator() = default;
  constexpr explicit VectorIterator(pointer ptr) : ptr_{ptr} {}

  [[nodiscard]] constexpr auto operator*() const noexcept -> reference {
    return *ptr_;
  }

  [[nodiscard]] constexpr auto operator->() const noexcept -> pointer {
    return ptr_;
  }

  [[nodiscard]] constexpr auto operator[](difference_type idx) const noexcept
      -> reference {
    return ptr_[idx];
  }

  constexpr auto operator++() noexcept -> VectorIterator & {
    ++ptr_;
    return *this;
  }

  constexpr auto operator++(int) noexcept -> VectorIterator {
    auto temp{*this};
    ++ptr_;
    return temp;
  }

  constexpr auto operator--() noexcept -> VectorIterator & {
    --ptr_;
    return *this;
  }

  constexpr auto operator--(int) noexcept -> VectorIterator {
    auto temp{*this};
    --ptr_;
    return temp;
  }

  constexpr auto operator+=(difference_type distance) noexcept
      -> VectorIterator & {
    ptr_ += distance;
    return *this;
  }

  constexpr auto operator-=(difference_type distance) noexcept
      -> VectorIterator & {
    ptr_ -= distance;
    return *this;
  }

  [[nodiscard]] constexpr friend auto
  operator+(VectorIterator const &lhs, difference_type distance) noexcept
      -> VectorIterator {
    return VectorIterator{lhs.ptr_ + distance};
  }

  [[nodiscard]] constexpr friend auto
  operator+(difference_type distance, VectorIterator const &rhs) noexcept
      -> VectorIterator {
    return VectorIterator{rhs + distance};
  }

  [[nodiscard]] constexpr friend auto
  operator-(VectorIterator const &lhs, difference_type distance) noexcept
      -> VectorIterator {
    return VectorIterator{lhs.ptr_ - distance};
  }

  [[nodiscard]] constexpr friend auto
  operator-(VectorIterator const &lhs, VectorIterator const &rhs) noexcept
      -> difference_type {
    return lhs.ptr_ - rhs.ptr_;
  }

  [[nodiscard]] constexpr friend auto
  operator<=>(VectorIterator const &lhs,
              VectorIterator const &rhs) noexcept = default;

private:
  pointer ptr_{};
};

// Ensures that the iterator meets the
// requirements for the appropriate iterator tag.
static_assert(std::contiguous_iterator<VectorIterator<int>>);
static_assert(std::contiguous_iterator<VectorIterator<int const>>);

template <typename T, typename Allocator> struct VectorBase {
  using allocator_type = Allocator;
  using pointer = std::allocator_traits<Allocator>::pointer;
  using size_type = std::allocator_traits<Allocator>::size_type;

  constexpr VectorBase() = default;
  constexpr VectorBase(Allocator const &allocator) : allocator_{allocator} {}
  constexpr ~VectorBase() {
    if (data_begin_ != nullptr) {
      std::allocator_traits<Allocator>::deallocate(
          allocator_, data_begin_,
          static_cast<size_type>(capacity_end_ - data_begin_));
    }
  }

  [[no_unique_address]] Allocator allocator_;
  pointer data_begin_{};
  pointer data_end_{};
  pointer capacity_end_{};
};

export template <typename T, typename Allocator = std::allocator<T>>
class Vector : public VectorBase<T, Allocator> {
private:
  using Base = VectorBase<T, Allocator>;
  using a_traits = std::allocator_traits<Allocator>;

public:
  // ** MEMBER TYPES **
  using value_type = std::remove_cv_t<T>;
  using allocator_type = Base::allocator_type;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using reference = value_type &;
  using const_reference = value_type const &;
  using pointer = Base::pointer;
  using const_pointer = std::allocator_traits<Allocator>::const_pointer;
  using iterator = VectorIterator<T>;
  using const_iterator = VectorIterator<T const>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
  using AllocResult = std::allocation_result<pointer, size_type>;

public:
  // ** CONSTRUCTORS **
  Vector() = default;
  Vector(Allocator const &allocator = Allocator()) : Base{allocator} {}

  explicit Vector(size_type count) {
    create_storage(count);

    for (auto const _ : std::views::iota(0UZ, count)) {
      emplace_back();
    }
  }

  Vector(std::initializer_list<T> const &init_list) {
    create_storage(init_list.size());
    data_end_ = memory::uninitialized_copy_range(allocator_, init_list.begin(),
                                                 init_list.end(), begin());
  }

  template <std::input_iterator InputIterator>
  Vector(InputIterator src_begin, InputIterator src_end) {
    auto const distance{std::ranges::distance(src_begin, src_end)};

    if (distance < 0) {
      throw std::invalid_argument(
          "Vector constructor: 'src_end' must be reachable from 'src_begin'");
    }

    auto const count{static_cast<size_type>(distance)};
    create_storage(count);

    data_end_ = memory::uninitialized_copy_range(allocator_, src_begin, src_end,
                                                 begin());
  }

  // TODO: (count, value)
  // TODO: container-compatible-range
  // TODO: assign_range
  // TODO: get_allocator

  // ** SPECIAL MEMBER FUNCTIONS **
  Vector(Vector const &other)
      : Base{std::allocator_traits<Allocator>::
                 select_on_container_copy_construction(other.allocator_)} {
    data_ = std::allocator_traits<Allocator>::allocate(allocator_, other.size_);
    try {
      allocator_aware::uninitialized_copy_range(
          allocator_, other.begin(), other.end(), VectorIterator{data_});
      capacity_ = other.size_;
      size_ = other.size_;
    } catch (...) {
      std::allocator_traits<Allocator>::deallocate(allocator_, data_,
                                                   other.size_);
      throw;
    }
  }

  auto operator=(Vector const &other) -> Vector & {
    if (this == std::addressof(other)) {
      return *this;
    }

    if constexpr (std::allocator_traits<Allocator>::
                      propagate_on_container_copy_assignment::value) {
      // If propagate is true, the source allocator must be copied into the
      // destination.

      if (allocator_ != other.allocator_) {
        // If allocators do not compare as equal, the source allocator cannot
        // manage the memory allocated by the destination allocator and must
        // allocate new memory for the elements in the destination instance
        // using the source allocator, then the destination allocator must free
        // it's memory before being replaced with a copy of the source
        // allocator.

        Allocator new_alloc{other.allocator_};
        auto *new_data{
            std::allocator_traits<Allocator>::allocate(new_alloc, other.size_)};

        try {
          allocator_aware::uninitialized_copy_range(new_alloc, other.begin(),
                                                    other.end(), new_data);

          allocator_aware::destroy_range(allocator_, begin(), end());
          std::allocator_traits<Allocator>::deallocate(allocator_, data_,
                                                       capacity_);

          allocator_ = new_alloc;
          data_ = new_data;
          size_ = other.size_;
          capacity_ = other.size_;
          return *this;
        } catch (...) {
          std::allocator_traits<Allocator>::deallocate(new_alloc, new_data,
                                                       other.size_);
          throw;
        }
      } else {
        // If allocators do compare equal, the source allocator can manage the
        // memory of the destination allocator, but the allocator must be copied
        // per the propagate_on_container_copy_assignment condition.

        allocator_ = other.allocator_;
      }
    }

    // If propagate is false, copying of the allocator is not necessary, no need
    // to worry about which allocator can manage which memory since the
    // destination retains it's allocator. Simply copy over the elements from
    // source.

    if constexpr (std::is_nothrow_copy_assignable_v<T>) {
      // If copy assignment is nothrow, the existing memory may be able to be
      // reused.

      if (capacity_ >= other.size_) {
        auto end_of_copied_data{begin()};

        for (auto pair : std::views::zip(*this, other)) {
          std::get<0>(pair) = std::get<1>(pair);
          ++end_of_copied_data;
        }

        // After data is copied, destroy the existing elements that were not
        // overwritten with the copied data so there's no phantom elements
        // hanging around.
        allocator_aware::destroy_range(allocator_, end_of_copied_data, end());

        size_ = other.size_;
        return *this;
      }
    }

    // If copy assignment may throw, allocate new memory to maintain strong
    // exception safety guarantees.

    auto *new_data{
        std::allocator_traits<Allocator>::allocate(allocator_, other.size_)};

    try {
      allocator_aware::uninitialized_copy_range(allocator_, other.begin(),
                                                other.end(), new_data);

      allocator_aware::destroy_range(allocator_, begin(), end());
      std::allocator_traits<Allocator>::deallocate(allocator_, data_,
                                                   capacity_);
      data_ = new_data;
      size_ = other.size_;
      capacity_ = other.size_;
      return *this;
    } catch (...) {
      std::allocator_traits<Allocator>::deallocate(allocator_, new_data,
                                                   other.size_);
      throw;
    }
  }

  Vector(Vector &&other) noexcept
      : allocator_{std::move(other.allocator_)}, data_{other.data_},
        capacity_{other.capacity_}, size_{other.size_} {
    other.data_ = nullptr;
    other.capacity_ = 0UZ;
    other.size_ = 0UZ;
  }

  auto operator=(Vector &&other) noexcept(
      std::allocator_traits<
          Allocator>::propagate_on_container_move_assignment::value ||
      std::allocator_traits<Allocator>::is_always_equal::value) -> Vector & {
    if (this == std::addressof(other)) {
      return *this;
    }

    if constexpr (std::allocator_traits<Allocator>::
                      propagate_on_container_move_assignment::value) {
      // Allocator can be moved, with the container.  Move over the allocator,
      // pointer, and invariants.

      allocator_aware::destroy_range(allocator_, begin(), end());
      std::allocator_traits<Allocator>::deallocate(allocator_, data_,
                                                   capacity_);
      allocator_ = std::move(other.allocator_);
      data_ = other.data_;
      capacity_ = other.capacity_;
      size_ = other.size_;
      other.data_ = nullptr;
      other.capacity_ = 0UZ;
      other.size_ = 0UZ;
    } else if (allocator_ == other.allocator_) {
      // Allocator can't be moved over, but the destination and source
      // allocators compare equal.  Ignore the allocator, move over the pointer
      // and invariants.

      allocator_aware::destroy_range(allocator_, begin(), end());
      std::allocator_traits<Allocator>::deallocate(allocator_, data_,
                                                   capacity_);
      data_ = other.data_;
      capacity_ = other.capacity_;
      size_ = other.size_;
      other.data_ = nullptr;
      other.capacity_ = 0UZ;
      other.size_ = 0UZ;
    } else {
      // Worst case scenario, can't transfer memory at all since the allocator
      // can't be moved and is not equal to the existing one.  Allocate new
      // memory with the current allocator and move the elements over.  On
      // success, destroy the destination container's existing objects, free the
      // memory, and swap the pointer from the newly allocated memory into
      // `data_`, then copy the size and capacity from the source.
      //
      // Once that's done, we have two options depending on how consistent we
      // want to be:
      // - Should the source wind up in the same state as the other cases?  If
      // so, then destroy moved from elements, free the memory, and reset the
      // invariants.
      // - Or should we leave the source as is, with moved from elements left in
      // allocated memory?  If that's the case, we're done.  I'm opting for this
      // option because I believe there's two cases here:
      // 1. The moved from container is about to die anyway, so let the
      // destructor destroy the elements and free the memory.
      // 2. If the container is going to be reused, that previously allocated
      // memory can also be reused, saving an allocation.  The user just has to
      // be aware of the fact.  Extra documentation for the move assignment
      // operator, but I believe this to be the correct choice.

      auto *new_data{
          std::allocator_traits<Allocator>::allocate(allocator_, other.size_)};
      try {
        allocator_aware::uninitialized_move_range(
            allocator_, other.begin(), other.end(), VectorIterator{new_data});
        allocator_aware::destroy_range(allocator_, begin(), end());
        std::allocator_traits<Allocator>::deallocate(allocator_, data_,
                                                     capacity_);
        data_ = new_data;
        new_data = nullptr;
        size_ = other.size_;
        capacity_ = other.size_;
      } catch (...) {
        std::allocator_traits<Allocator>::deallocate(allocator_, new_data,
                                                     other.size_);
        throw;
      }
    }
    return *this;
  }

  constexpr ~Vector() {
    memory::destroy_range(allocator_, begin(), end());
    // Deallocation is handled by VectorBase.
  }

  // ** ITERATORS **
  template <typename Self>
  [[nodiscard]] constexpr auto begin(this Self &&self) noexcept {
    using const_correct_iterator =
        std::conditional_t<std::is_const_v<std::remove_reference_t<Self>>,
                           const_iterator, iterator>;
    return static_cast<const_correct_iterator>(self.data_begin_);
  }

  template <typename Self>
  [[nodiscard]] constexpr auto end(this Self &&self) noexcept {
    using const_correct_iterator =
        std::conditional_t<std::is_const_v<std::remove_reference_t<Self>>,
                           const_iterator, iterator>;
    return static_cast<const_correct_iterator>(self.data_end_);
  }

  [[nodiscard]] constexpr auto cbegin() const noexcept -> const_iterator {
    return begin();
  }

  [[nodiscard]] constexpr auto cend() const noexcept -> const_iterator {
    return end();
  }

  template <typename Self>
  [[nodiscard]] constexpr auto rbegin(this Self &&self) noexcept {
    using const_correct_reverse_iterator =
        std::conditional_t<std::is_const_v<std::remove_reference_t<Self>>,
                           const_reverse_iterator, reverse_iterator>;
    return static_cast<const_correct_reverse_iterator>(self.end());
  }

  template <typename Self>
  [[nodiscard]] constexpr auto rend(this Self &&self) noexcept {
    using const_correct_reverse_iterator =
        std::conditional_t<std::is_const_v<std::remove_reference_t<Self>>,
                           const_reverse_iterator, reverse_iterator>;
    return static_cast<const_correct_reverse_iterator>(self.begin());
  }

  [[nodiscard]] constexpr auto crbegin() const noexcept
      -> const_reverse_iterator {
    return rbegin();
  }

  [[nodiscard]] constexpr auto crend() const noexcept
      -> const_reverse_iterator {
    return rend();
  }

  // ** ELEMENT ACCESS **
  template <typename Self>
  [[nodiscard]] constexpr auto at(this Self &&self, size_type position)
      -> decltype(auto) {
    if (position >= self.size()) {
      std::string reason{"Vector Range Check: position (which is "};
      reason += format::integral_to_string(position);
      reason += ") >= this->size() (which is ";
      reason += format::integral_to_string(self.size());
      reason += ")";
      throw std::out_of_range(reason);
    }
    return std::forward_like<Self>(*(self.data_begin_ + position));
  }

  // TODO: Add contract precondition.
  template <typename Self>
  [[nodiscard]] constexpr auto operator[](this Self &&self,
                                          size_type position) noexcept
      -> decltype(auto) {
    return std::forward_like<Self>(*(self.data_begin_ + position));
  }

  // TODO: Add contract precondition.
  template <typename Self>
  [[nodiscard]] constexpr auto front(this Self &&self) noexcept
      -> decltype(auto) {
    return std::forward_like<Self>(*self.data_begin_);
  }

  // TODO: Add contract precondition.
  template <typename Self>
  [[nodiscard]] constexpr auto back(this Self &&self) noexcept
      -> decltype(auto) {
    return std::forward_like<Self>(*(self.data_end_ - 1));
  }

  template <typename Self>
  [[nodiscard]] constexpr auto data(this Self &&self) noexcept {
    using const_correct_pointer =
        std::conditional_t<std::is_const_v<std::remove_reference_t<Self>>,
                           const_pointer, pointer>;
    return static_cast<const_correct_pointer>(self.data_begin_);
  }

  // ** CAPACITY **
  [[nodiscard]] constexpr auto is_empty() const noexcept -> bool {
    return data_begin_ == data_end_;
  }

  [[nodiscard]] constexpr auto size() const noexcept -> size_type {
    return static_cast<size_type>(data_end_ - data_begin_);
  }

  [[nodiscard]] constexpr auto max_size() const noexcept -> size_type {
    return std::min(a_traits::max_size(allocator_),
                    std::numeric_limits<size_type>::max() / sizeof(value_type));
  }

  constexpr auto reserve(size_type new_capacity) -> void {
    if (new_capacity > max_size()) {
      std::string reason{"Vector::reserve: new_capacity (which is "};
      reason += format::integral_to_string(new_capacity);
      reason += ") is greater than max_size() (which is ";
      reason += format::integral_to_string(max_size());
      reason += ").";
      throw std::length_error(reason);
    }

    if (new_capacity <= capacity()) {
      return;
    }

    if (data_begin_ == capacity_end_) {
      create_storage(new_capacity);
      return;
    }

    auto new_storage{allocate_at_least(calculate_growth_size(new_capacity))};

    memory::AllocationGuard mem_guard{allocator_, new_storage.ptr,
                                      new_storage.size};

    migrate_to_new_storage(new_storage.ptr);

    // Let the guard free the old memory.
    mem_guard.reassign(data_begin_, capacity());

    data_begin_ = new_storage.ptr;
    data_end_ = data_begin_ + size();
    capacity_end_ = data_begin_ + new_storage.count;
  }

  [[nodiscard]] constexpr auto capacity() const noexcept -> size_type {
    return static_cast<size_type>(capacity_end_ - data_begin_);
  }

  // shrink_to_fit()

  // ** MODIFIERS **
  // TODO: clear()
  // TODO: insert()
  // TODO: insert_range()
  // TODO: emplace()
  // TODO: erase()
  constexpr auto push_back(T const &value) -> void { emplace_back(value); }
  constexpr auto push_back(T &&value) -> void {
    emplace_back(std::move(value));
  }

  template <typename... Args>
  constexpr auto emplace_back(Args &&...args) -> reference {
    if (data_end_ == capacity_end_) {
      size_type new_capacity{};
      if (capacity_end_ == nullptr) {
        create_storage(1);
      } else {
        return realloc_emplace(std::forward<Args>(args)...)
      }
    }

    a_traits::construct(allocator_, data_end_, std::forward<Args>(args)...);
    return *data_end_++;
  }

  // TODO: append_range()
  // TODO: pop_back()
  // TODO: resize()

  constexpr auto swap(Vector &other) noexcept -> void {
    if constexpr (std::allocator_traits<
                      Allocator>::propagate_on_container_swap::value) {
      using std::swap;
      swap(allocator_, other.allocator_);
    } else if constexpr (!std::allocator_traits<
                             Allocator>::is_always_equal::value) {
      contract_assert(allocator_ != other.allocator_
                      /*"If propagate_on_container_swap is not provided or is
                        derived from std::false_type and the allocators of the
                        two containers do not compare equal, the behavior of
                        container swap is undefined."*/
                      && "You are invoking undefined behavior here.");
    }

    std::swap(data_begin_, other.data_begin_);
    std::swap(data_end_, other.data_end_);
    std::swap(capacity_end_, other.capacity_end_);
  }

  constexpr friend auto swap(Vector &lhs, Vector &rhs) noexcept -> void {
    lhs.swap(rhs);
  }

  // ** NON-MEMBER FUNCTIONS **
  constexpr friend auto
  operator==(Vector const &lhs,
             Vector const &rhs) noexcept(noexcept(std::declval<T>() ==
                                                  std::declval<T>())) -> bool
    requires std::equality_comparable<T>
  {
    if (rhs.size_ != lhs.size_) {
      return false;
    }
    return std::ranges::equal(lhs, rhs);
  }

  constexpr friend auto
  operator<=>(Vector const &lhs,
              Vector const &rhs) noexcept(noexcept(std::declval<T>() <=>
                                                   std::declval<T>()))
    requires std::three_way_comparable<T>
  {
    for (auto const pair : std::views::zip(lhs, rhs)) {
      if (auto const comparison{std::get<0>(pair) <=> std::get<1>(pair)};
          comparison != 0) {
        return comparison;
      }
    }
    return lhs.size_ <=> rhs.size_;
  }

private:
  [[nodiscard]] constexpr auto allocate_at_least(size_type n) -> AllocResult {
    auto result{a_traits::allocate_at_least(allocator_, n)};

    auto max{max_size()};
    if (count > max) {
      result.count = max
    };

    return result;
  }

  constexpr auto create_storage(size_type n) -> void {
    auto result{allocate_at_least(n)};

    data_begin_, data_end_ = result.ptr;
    capacity_end_ = data_begin_ + result.count;
  }

  constexpr auto calculate_growth_size(size_type target_growth = 1UZ)
      -> size_type {
    auto current_size{size()};
    auto max_possible_growth{max_size() - current_size};

    if (max_possible_growth < target_growth) {
      throw std::length_error(
          "Vector growth request exceeded maximum possible size.");
    }

    if (target_growth < current_size) {
      target_growth = current_size;
    }
    if (target_growth > max_possible_growth) {
      target_growth = max_possible_growth;
    }
    return current_size + target_growth;
  }

  constexpr auto migrate_to_new_storage(pointer new_storage) -> void {
    static_assert(
        std::is_copy_constructible_v<T> || std::is_move_constructible_v<T>,
        "Object type 'T' is neither moveable nor copyable, "
        "making Vector reallocation impossible - use a different container.");

    if constexpr (std::is_nothrow_move_constructible_v<T> ||
                  !std::is_copy_constructible_v<T>) {
      memory::uninitialized_move_range(allocator_, begin(), end(),
                                       VectorIterator{new_storage});
    } else {
      memory::uninitialized_copy_range(allocator_, begin(), end(),
                                       VectorIterator{new_storage});
    }
  }

  memory::destroy_range(allocator_, begin(), end());
}

template <typename... Args>
constexpr auto realloc_emplace(Args &&...args) -> reference {
  auto new_storage{allocate_at_least(calculate_growth_size())};

  { // Allocation guard in place here.

    memory::AllocationGuard mem_guard{allocator, new_storage.ptr,
                                      new_storage.count};

    auto new_element_begin{new_storage.ptr + size()};
    auto new_element_end{new_element_begin + 1};

    // If this throws the guard will clean up the allocation and the original
    // elements remain untouched.
    a_traits::construct(allocator_, new_element_begin,
                        std::forward<Args>(args)...);

    { // Element guard in place here.

      // A valid element exists in new memory and must be destroyed prior to
      // deallocation if an exception is thrown during data migration.
      memory::ElementGuard elem_guard{allocator, new_element_begin,
                                      new_element_end};

      migrate_to_new_storage(new_storage.ptr);
      elem_guard.dismiss();

    } // Element guard in place here.

    // Let the guard free the old memory.
    mem_guard.reassign(data_begin_, capacity());

    data_begin_ = new_storage.ptr;
    data_end_ = new_element_end;
    capacity_end_ = data_begin_ + new_storage.count;

  } // Allocation guard in place here.

  return back();
}
};

// Explicit Deduction Guide for CTAD.
template <std::input_iterator InputIterator,
          std::sentinel_for<InputIterator> Sentinel>
Vector(InputIterator, Sentinel) -> Vector<std::iter_value_t<InputIterator>>;

} // namespace swtl
