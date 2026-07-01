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
  using value_type = std::allocator_traits<Allocator>::value_type;
  using allocator_type = Allocator;
  using size_type = std::allocator_traits<Allocator>::size_type;
  using pointer = std::allocator_traits<Allocator>::pointer;
  using AllocResult = std::allocation_result<pointer, size_type>;
  using a_traits = std::allocator_traits<Allocator>;

  constexpr VectorBase() = default;
  constexpr VectorBase(Allocator const &allocator) : allocator_{allocator} {}

  constexpr VectorBase(VectorBase const &other) noexcept
      : allocator_{
            a_traits::select_on_container_copy_construction(other.allocator_)} {
  }

  constexpr VectorBase(VectorBase &&other) noexcept
      : allocator_{std::move(other.allocator_)}, data_begin_{other.data_begin_},
        data_end_{other.data_end_}, capacity_end_{other.capacity_end_} {
    other.data_begin_ = other.data_end_ = other.capacity_end_ = nullptr;
  }

  constexpr auto operator=(VectorBase const &other) -> VectorBase & =
      delete ("Copy assignment must be handled by the Vector container.");
  constexpr auto operator=(VectorBase &&other) -> VectorBase & =
      delete ("Move assignment must be handled by the Vector container.");

  constexpr ~VectorBase() {
    if (data_begin_ != nullptr) {
      a_traits::deallocate(allocator_, data_begin_,
                           static_cast<size_type>(capacity_end_ - data_begin_));
    }
  }

  [[nodiscard]] constexpr auto allocate_at_least(size_type n) -> AllocResult {
    if (n == 0) {
      return {nullptr, n};
    }

    auto [ptr, count]{a_traits::allocate_at_least(allocator_, n)};

    auto max{max_allocatable_size()};
    if (count > max) {
      count = max;
    }

    return {ptr, count};
  }

  constexpr auto create_storage(size_type n) -> void {
    auto [ptr, count]{allocate_at_least(n)};

    data_begin_ = data_end_ = ptr;
    capacity_end_ = data_begin_ + count;
  }

  constexpr auto deallocate_memory_of_this() -> void {
    a_traits::deallocate(allocator_, data_begin_,
                         static_cast<size_type>(capacity_end_ - data_begin_));
    data_begin_ = data_end_ = capacity_end_ = nullptr;
  }

  [[nodiscard]] constexpr auto max_allocatable_size() const noexcept
      -> size_type {
    return std::min(a_traits::max_size(allocator_),
                    std::numeric_limits<size_type>::max() / sizeof(value_type));
  }

  [[no_unique_address]] Allocator allocator_;
  pointer data_begin_{};
  pointer data_end_{};
  pointer capacity_end_{};
};

export template <typename T, typename Allocator = std::allocator<T>>
class Vector : protected VectorBase<T, Allocator> {
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

public:
  // ** CONSTRUCTORS **
  constexpr Vector(Allocator const &allocator = Allocator())
      : Base{allocator} {}

  constexpr explicit Vector(size_type count,
                            Allocator const &allocator = Allocator())
      : Base{allocator} {
    this->create_storage(count);

    for (auto const _ : std::views::iota(0UZ, count)) {
      emplace_back();
    }
  }

  // Should not be marked as explicit to allow for conversion from braced init
  // list.
  constexpr Vector(std::initializer_list<T> const &init_list,
                   Allocator const &allocator = Allocator())
      : Base{allocator} {
    this->create_storage(init_list.size());
    this->data_end_ =
        memory::uninitialized_copy(this->allocator_, init_list.begin(),
                                   init_list.end(), this->data_begin_);
  }

  template <std::input_iterator InputIterator>
  constexpr Vector(InputIterator src_begin, InputIterator src_end,
                   Allocator const &allocator = Allocator())
      : Base{allocator} {
    auto const distance{std::ranges::distance(src_begin, src_end)};

    if (distance < 0) {
      throw std::invalid_argument(
          "Vector constructor: 'src_end' must be reachable from 'src_begin'");
    }

    auto const count{static_cast<size_type>(distance)};
    this->create_storage(count);

    this->data_end_ = memory::uninitialized_copy(this->allocator_, src_begin,
                                                 src_end, this->data_begin_);
  }

  // TODO: (count, value)
  // TODO: container-compatible-range
  // TODO: assign_range
  // TODO: get_allocator

  // ** SPECIAL MEMBER FUNCTIONS **
  constexpr Vector(Vector const &other) : Base{other} {
    this->create_storage(other.size());
    this->data_end_ =
        memory::uninitialized_copy(this->allocator_, other.data_begin_,
                                   other.data_end_, this->data_begin_);
  }

  constexpr auto operator=(Vector const &other) -> Vector & {
    if (this == std::addressof(other)) {
      return *this;
    }

    if constexpr (std::allocator_traits<Allocator>::
                      propagate_on_container_copy_assignment::value) {
      // If propagate is true, the source allocator must be copied into the
      // destination.

      if (this->allocator_ != other.allocator_) {
        // If allocators do not compare as equal, the source allocator cannot
        // manage the memory allocated by the destination allocator and must
        // allocate new memory for the elements in the destination instance
        // using the source allocator, then the destination allocator must free
        // it's memory before being replaced with a copy of the source
        // allocator.

        Allocator new_alloc{other.allocator_};
        auto [ptr, count]{a_traits::allocate_at_least(new_alloc, other.size())};

        memory::AllocationGuard mem_guard{new_alloc, ptr, count};
        auto const new_end{memory::uninitialized_copy(
            new_alloc, other.data_begin_, other.data_end_, ptr)};
        mem_guard.dismiss();
        destroy_elements_of_this();
        this->deallocate_memory_of_this();

        this->allocator_ = new_alloc;
        this->data_begin_ = ptr;
        this->data_end_ = new_end;
        this->capacity_end_ = this->data_begin_ + count;
        return *this;
      } else {
        // If allocators do compare equal, the source allocator can manage the
        // memory of the destination allocator, but the allocator must be
        // copied per the propagate_on_container_copy_assignment condition.

        this->allocator_ = other.allocator_;
      }
    }

    // If propagate is false, copying of the allocator is not necessary, no
    // need to worry about which allocator can manage which memory since the
    // destination retains it's allocator. Simply copy over the elements from
    // source.

    if constexpr (std::is_nothrow_copy_assignable_v<T>) {
      // If copy assignment is nothrow, the existing memory may be able to be
      // reused if there's enough capacity to do so.

      if (capacity() >= other.capacity()) {
        auto end_of_copied_data{this->data_begin_};

        for (auto pair : std::views::zip(*this, other)) {
          std::get<0>(pair) = std::get<1>(pair);
          ++end_of_copied_data;
        }

        // After data is copied, destroy the existing elements that were not
        // overwritten with the copied data so there's no phantom elements
        // hanging around.
        memory::destroy(this->allocator_, end_of_copied_data, this->data_end_);

        this->data_end_ = end_of_copied_data;
        return *this;
      }
    }

    // If copy assignment may throw, allocate new memory to maintain strong
    // exception safety guarantees.

    auto [ptr, count]{this->allocate_at_least(other.size())};

    memory::AllocationGuard mem_guard{this->allocator_, ptr, count};
    auto new_end{memory::uninitialized_copy(this->allocator_, other.data_begin_,
                                            other.data_end_, ptr)};
    mem_guard.dismiss();
    destroy_elements_of_this();
    this->deallocate_memory_of_this();

    this->data_begin_ = ptr;
    this->data_end_ = new_end;
    this->capacity_end_ = this->data_begin_ + count;
    return *this;
  }

  constexpr Vector(Vector &&other) noexcept : Base(std::move(other)) {}

  constexpr auto operator=(Vector &&other) noexcept(
      a_traits::propagate_on_container_move_assignment::value ||
      a_traits::is_always_equal::value) -> Vector & {
    if (this == std::addressof(other)) {
      return *this;
    }

    if constexpr (a_traits::propagate_on_container_move_assignment::value) {
      // Allocator can be moved with the container - noexcept.

      destroy_elements_of_this();
      this->deallocate_memory_of_this();
      this->allocator_ = std::move(other.allocator_);
      this->data_begin_ = other.data_begin_;
      this->data_end_ = other.data_end_;
      this->capacity_end_ = other.capacity_end_;
      other.data_begin_ = other.data_end_ = other.capacity_end_ = nullptr;
      return *this;
    } else if (this->allocator_ == other.allocator_) {
      // Allocator can't be moved over, but the destination and source
      // allocators compare equal.  Ignore the allocator, move the rest -
      // noexcept.

      destroy_elements_of_this();
      this->deallocate_memory_of_this();
      this->data_begin_ = other.data_begin_;
      this->data_end_ = other.data_end_;
      this->capacity_end_ = other.capacity_end_;
      other.data_begin_ = other.data_end_ = other.capacity_end_ = nullptr;
      return *this;
    } else {
      // Worst case scenario, we can't transfer memory at all since the source
      // allocator can't be moved and does not compare equal to the destination
      // allocator.  Allocate new memory with the destination allocator and move
      // the elements over.  On success, destroy destination's existing objects,
      // free the memory, and replace the storage with the newly allocated
      // memory.
      //
      // Once that's done, we have two options depending on how consistent we
      // want to be:
      // - Should the source wind up in the same state as the other cases?  If
      // so, then destroy moved from elements, free the memory, and reset the
      // invariants.
      // - Or should we leave the source as is, with moved from elements left
      // in allocated memory?  If that's the case, we're done.  I'm opting for
      // this option because I believe there's two cases to consider here:
      // 1. The moved from container is about to die anyway, so let the
      // destructor destroy the elements and free the memory.
      // 2. If the container is going to be reused, that previously allocated
      // memory can also be reused, saving an allocation.  The user just has
      // to be aware of the fact.  Extra documentation for the move assignment
      // operator, but I believe this to be the correct choice.

      auto [ptr, count]{this->allocate_at_least(other.size())};
      memory::AllocationGuard mem_guard{this->allocator_, ptr, count};
      auto new_end{memory::uninitialized_move(
          this->allocator_, other.data_begin_, other.data_end_, ptr)};
      mem_guard.dismiss();
      destroy_elements_of_this();
      this->deallocate_memory_of_this();

      this->data_begin_ = ptr;
      this->data_end_ = new_end;
      this->capacity_end_ = this->data_begin_ + count;
      return *this;
    }
  }

  constexpr ~Vector() {
    destroy_elements_of_this();
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
    return this->data_begin_ == this->data_end_;
  }

  [[nodiscard]] constexpr auto size() const noexcept -> size_type {
    return static_cast<size_type>(this->data_end_ - this->data_begin_);
  }

  [[nodiscard]] constexpr auto max_size() const noexcept -> size_type {
    return this->max_allocatable_size();
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

    if (this->capacity_end_ == nullptr) {
      this->create_storage(new_capacity);
      return;
    }

    auto [ptr,
          count]{this->allocate_at_least(calculate_growth_size(new_capacity))};

    memory::AllocationGuard mem_guard{this->allocator_, ptr, count};
    auto const new_end{memory::uninitialized_move_if_noexcept(
        this->allocator_, this->data_begin_, this->data_end_, ptr)};
    mem_guard.dismiss();
    destroy_elements_of_this();
    this->deallocate_memory_of_this();

    this->data_begin_ = ptr;
    this->data_end_ = new_end;
    this->capacity_end_ = ptr + count;
  }

  [[nodiscard]] constexpr auto capacity() const noexcept -> size_type {
    return static_cast<size_type>(this->capacity_end_ - this->data_begin_);
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
    if (this->data_end_ == this->capacity_end_) {
      if (this->capacity_end_ == nullptr) {
        this->create_storage(1);
      } else {
        return realloc_emplace(std::forward<Args>(args)...);
      }
    }

    a_traits::construct(this->allocator_, this->data_end_,
                        std::forward<Args>(args)...);
    return *this->data_end_++;
  }

  // TODO: append_range()
  // TODO: pop_back()
  // TODO: resize()

  constexpr auto swap(Vector &other) noexcept -> void {
    if constexpr (std::allocator_traits<
                      Allocator>::propagate_on_container_swap::value) {
      using std::swap;
      swap(this->allocator_, other.allocator_);
    } else if constexpr (!std::allocator_traits<
                             Allocator>::is_always_equal::value) {
      contract_assert(this->allocator_ != other.allocator_
                      /*"If propagate_on_container_swap is not provided or is
                        derived from std::false_type and the allocators of the
                        two containers do not compare equal, the behavior of
                        container swap is undefined."*/
                      && "You are invoking undefined behavior here.");
    }

    std::swap(this->data_begin_, other.data_begin_);
    std::swap(this->data_end_, other.data_end_);
    std::swap(this->capacity_end_, other.capacity_end_);
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
    if (rhs.size() != lhs.size()) {
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
    return lhs.size() <=> rhs.size();
  }

private:
  [[nodiscard]] constexpr auto
  calculate_growth_size(size_type target_growth = 1UZ) -> size_type {
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

  constexpr auto destroy_elements_of_this() -> void {
    memory::destroy(this->allocator_, begin(), end());
    this->data_end_ = this->data_begin_;
  }

  template <typename... Args>
  constexpr auto realloc_emplace(Args &&...args) -> reference {
    auto [ptr, count]{this->allocate_at_least(calculate_growth_size())};
    memory::AllocationGuard mem_guard{this->allocator_, ptr, count};

    auto new_element_begin{ptr + size()};
    auto new_element_end{new_element_begin + 1};

    // If this throws the memory guard will clean up the allocation and the
    // original elements remain untouched.
    a_traits::construct(this->allocator_, new_element_begin,
                        std::forward<Args>(args)...);

    // A valid element exists in new memory and must be destroyed prior to
    // deallocation if an exception is thrown during data migration.
    memory::ElementGuard elem_guard{this->allocator_, new_element_begin,
                                    new_element_end};
    memory::uninitialized_move_if_noexcept(this->allocator_, this->data_begin_,
                                           this->data_end_, ptr);
    elem_guard.dismiss();
    mem_guard.dismiss();
    destroy_elements_of_this();
    this->deallocate_memory_of_this();

    this->data_begin_ = ptr;
    this->data_end_ = new_element_end;
    this->capacity_end_ = this->data_begin_ + count;
    return back();
  }
};

// Explicit Deduction Guide for CTAD.
template <std::input_iterator InputIterator,
          std::sentinel_for<InputIterator> Sentinel>
Vector(InputIterator, Sentinel) -> Vector<std::iter_value_t<InputIterator>>;

} // namespace swtl
