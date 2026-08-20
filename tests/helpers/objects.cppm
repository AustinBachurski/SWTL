export module swtl.test.helpers:objects;

import std;

import swtl.format;

/// @cond INTERNAL_DOCUMENTATION

export namespace swtl::test_helpers
{

/// @brief Exception class used during testing.
///
class TestException : std::runtime_error
{
public:
   /// @brief Constructor taking a string with details about the exception.
   ///
   /// @param message The message to include in the exception.
   ///
   constexpr TestException(std::string_view message)
       : std::runtime_error{ std::string{ message } }
   {}
};

/// @brief Struct containing special member function counters.
///
struct OpCounters
{
   std::size_t default_construction{};  ///< Count of default constructions.
   std::size_t arg_construction{};      ///< Count of argument constructions.
   std::size_t copy_construction{};     ///< Count of copy constructions.
   std::size_t copy_assignment{};       ///< Count of copy assignment calls.
   std::size_t move_construction{};     ///< Count of move constructions.
   std::size_t move_assignment{};       ///< Count of move assigment calls.
   std::size_t destruction{};           ///< Count of destructor calls.
};

/// @brief Struct for tracking counts and triggering throws.
///
struct TestController
{
   OpCounters count_of;    ///< The count for each operation.
   OpCounters throw_when;  ///< The count at which an operation should throw.
   bool should_throw{};    ///< Whether operations should throw or not.

   /// @brief Returns true if the sum of construction counts equal destruction
   /// count.
   ///
   /// @return The sum of `count_of` `default_construction`, `arg_construction`,
   /// `copy_construction`, and `move_construction`, compared with
   /// `count_of.destruction`.
   ///
   constexpr bool
   all_new_instances_destroyed() const noexcept
   {
      return sum_of_constructions() == count_of.destruction;
   }

   /// @brief Prevents the TestController from triggering throws.
   ///
   /// @post `should_throw` == false
   ///
   constexpr void
   disable_throwing() noexcept
   {
      should_throw = false;
   }

   /// @brief Allows the TestController to trigger throws.
   ///
   /// @post `should_throw` == true
   ///
   constexpr void
   enable_throwing() noexcept
   {
      should_throw = true;
   }

   /// @brief Returns the count of instances that are alive.
   ///
   /// @return Sum of constructions - destructions.
   ///
   constexpr std::size_t
   instances_alive() const noexcept
   {
      return sum_of_constructions() - count_of.destruction;
   }

   /// @brief Prints the state of the controller to stdout.
   ///
   constexpr void
   report() const
   {
      // TODO: Use reflection here when supported?
      std::println();
      std::println("TestController State");
      std::println("--------------------");
      std::println("should_throw = {}", should_throw);
      std::println("Counts:");
      std::println(
          "- default_construction = {}", count_of.default_construction);
      std::println("- arg_construction = {}", count_of.arg_construction);
      std::println("- copy_construction = {}", count_of.copy_construction);
      std::println("- copy_assignment = {}", count_of.copy_assignment);
      std::println("- move_construction = {}", count_of.move_construction);
      std::println("- move_assignment = {}", count_of.move_assignment);
      std::println("- destruction = {}", count_of.destruction);
      std::println();
      std::println("Throws When:");
      std::println(
          "- default_construction == {}", throw_when.default_construction);
      std::println("- arg_construction == {}", throw_when.arg_construction);
      std::println("- copy_construction == {}", throw_when.copy_construction);
      std::println("- copy_assignment == {}", throw_when.copy_assignment);
      std::println("- move_construction == {}", throw_when.move_construction);
      std::println("- move_assignment == {}", throw_when.move_assignment);
      std::println("- destruction == {}", throw_when.destruction);
      std::println();
   }

   /// @brief Resets all counts to zero and the `should_throw` boolean to false.
   ///
   /// @note Run this to start tracking from zero once test setup is complete.
   ///
   constexpr void
   reset() noexcept
   {
      *this = TestController{};
   }

   /// @brief Returns the sum of all constructions.
   ///
   /// @return The sum of `count_of` `default_construction`, `arg_construction`,
   /// `copy_construction`, and `move_construction`.
   ///
   constexpr std::size_t
   sum_of_constructions() const noexcept
   {
      return count_of.default_construction + count_of.arg_construction
           + count_of.copy_construction + count_of.move_construction;
   }
};

/// Global controller for tracking counts and triggering throws.
inline thread_local TestController g_test_controller;

/// @brief Basic tracked object without throw capability.
///
struct NoThrowTrackedObject
{
   /// @brief Default constructor.
   ///
   constexpr NoThrowTrackedObject() noexcept
   {
      ++g_test_controller.count_of.default_construction;
   }

   /// @brief Single argument constructor that sets `id`.
   ///
   /// @param identifier The number to use for `id`.
   ///
   constexpr NoThrowTrackedObject(std::size_t identifier) noexcept
       : id{ identifier }
   {
      ++g_test_controller.count_of.arg_construction;
   }

   /// @brief Copy constructor.
   ///
   constexpr NoThrowTrackedObject(NoThrowTrackedObject const &other) noexcept
       : id{ other.id }
   {
      ++g_test_controller.count_of.copy_construction;
   }

   /// @brief Copy assignment operator.
   ///
   /// @return A reference to `*this`.
   ///
   constexpr NoThrowTrackedObject &
   operator=(NoThrowTrackedObject const &other) noexcept
   {
      id = other.id;
      ++g_test_controller.count_of.copy_assignment;
      return *this;
   }

   /// @brief Move constructor.
   ///
   constexpr NoThrowTrackedObject(NoThrowTrackedObject &&other) noexcept
       : id{ other.id }
   {
      ++g_test_controller.count_of.move_construction;
   }

   /// @brief Move assignment operator.
   ///
   /// @return A reference to `*this`.
   ///
   constexpr NoThrowTrackedObject &
   operator=(NoThrowTrackedObject &&other) noexcept
   {
      id = other.id;
      ++g_test_controller.count_of.move_assignment;
      return *this;
   }

   /// @brief Destructor.
   ///
   constexpr ~NoThrowTrackedObject()
   {
      ++g_test_controller.count_of.destruction;
   }

   /// @brief Three-way comparison operator.
   ///
   /// @return The result of the three-way comparison.
   ///
   [[nodiscard]]
   constexpr auto
   operator<=>(NoThrowTrackedObject const &other) const = default;

   std::size_t id{};  ///< Uniquely identifies the object instance.
};

/// @brief Basic tracked object, may throw.
///
/// @throws TestException During the execution of the operation for the Nth
/// object where N is equal to the value set for the operation via `throw_when`.
/// i.e. - If `throw_when.default_construction == 10` then after 9 objects have
/// been default constructed attempting to default-construct another instance of
/// this object will trigger an exception.
///
/// @note Sets the global test controller's `should_throw` member to `false` if
/// an exception is thrown, this prevents erroneous throws during the setup of
/// another test.
///
struct TrackedObject
{
   /// @brief Default constructor.
   ///
   /// @throws TextException If dictated by the test controller.
   ///
   constexpr TrackedObject()
   {
      if (g_test_controller.should_throw
          && g_test_controller.count_of.default_construction + 1
                 == g_test_controller.throw_when.default_construction)
      {
         g_test_controller.disable_throwing();
         throw TestException("Throw from default constructor.");
      }

      ++g_test_controller.count_of.default_construction;
   }

   /// @brief Single argument constructor that sets `id`.
   ///
   /// @param identifier The number to use for `id`.
   ///
   /// @throws TextException If dictated by the test controller.
   ///
   constexpr TrackedObject(std::size_t identifier)
       : id{ identifier }
   {
      if (g_test_controller.should_throw
          && g_test_controller.count_of.arg_construction + 1
                 == g_test_controller.throw_when.arg_construction)
      {
         g_test_controller.disable_throwing();
         auto const id_value{ swtl::integral_to_string(identifier) };
         throw TestException(
             "Throw from argument constructor with identifier (" + id_value
             + ").");
      }

      ++g_test_controller.count_of.arg_construction;
   }

   /// @brief Copy constructor.
   ///
   /// @throws TextException If dictated by the test controller.
   ///
   constexpr TrackedObject(TrackedObject const &other)
       : id{ other.id }
   {
      if (g_test_controller.should_throw
          && g_test_controller.count_of.copy_construction + 1
                 == g_test_controller.throw_when.copy_construction)
      {
         g_test_controller.disable_throwing();
         throw TestException("Throw from copy constructor.");
      }

      ++g_test_controller.count_of.copy_construction;
   }

   /// @brief Copy assignment operator.
   ///
   /// @throws TextException If dictated by the test controller.
   ///
   /// @return A reference to `*this`.
   ///
   constexpr TrackedObject &
   operator=(TrackedObject const &other)
   {
      id = other.id;

      if (g_test_controller.should_throw
          && g_test_controller.count_of.copy_assignment + 1
                 == g_test_controller.throw_when.copy_assignment)
      {
         g_test_controller.disable_throwing();
         throw TestException("Throw from copy assignment.");
      }

      ++g_test_controller.count_of.copy_assignment;
      return *this;
   }

   /// @brief Move constructor.
   ///
   /// @throws TextException If dictated by the test controller.
   ///
   constexpr TrackedObject(TrackedObject &&other)
       : id{ other.id }
   {
      if (g_test_controller.should_throw
          && g_test_controller.count_of.move_construction + 1
                 == g_test_controller.throw_when.move_construction)
      {
         g_test_controller.disable_throwing();
         throw TestException("Throw from move constructor.");
      }

      ++g_test_controller.count_of.move_construction;
   }

   /// @brief Move assignment operator.
   ///
   /// @throws TextException If dictated by the test controller.
   ///
   /// @return A reference to `*this`.
   ///
   constexpr TrackedObject &
   operator=(TrackedObject &&other)
   {
      id = other.id;

      if (g_test_controller.should_throw
          && g_test_controller.count_of.move_assignment + 1
                 == g_test_controller.throw_when.move_assignment)
      {
         g_test_controller.disable_throwing();
         throw TestException("Throw from move assignment.");
      }

      ++g_test_controller.count_of.move_assignment;
      return *this;
   }

   /// @brief Destructor.
   ///
   constexpr ~TrackedObject()
   {
      ++g_test_controller.count_of.destruction;
   }

   /// @brief Three-way comparison operator.
   ///
   /// @return The result of the three-way comparison.
   ///
   [[nodiscard]]
   constexpr auto
   operator<=>(TrackedObject const &other) const = default;

   std::size_t id{};  ///< Uniquely identifies the object instance.
};

/// @brief Copy-only test type.
///
/// @throws TextException If dictated by the test controller.
///
struct CopyOnlyTrackedObject : public TrackedObject
{
   /// @brief Default constructor.
   ///
   /// @throws TextException If dictated by the test controller.
   ///
   CopyOnlyTrackedObject() = default;

   /// @brief Argument constructor.
   ///
   /// @throws TextException If dictated by the test controller.
   ///
   CopyOnlyTrackedObject(std::size_t identifier)
       : TrackedObject(identifier)
   {}

   /// @brief Copy constructor.
   ///
   /// @throws TextException If dictated by the test controller.
   ///
   CopyOnlyTrackedObject(CopyOnlyTrackedObject const &) = default;

   /// @brief Copy assignment operator.
   ///
   /// @throws TextException If dictated by the test controller.
   ///
   /// @return A reference to `*this`.
   ///
   CopyOnlyTrackedObject &
   operator=(CopyOnlyTrackedObject const &) = default;

   /// @brief Move constructor.
   ///
   /// @throws TextException If dictated by the test controller.
   ///
   CopyOnlyTrackedObject(CopyOnlyTrackedObject &&) = delete;

   /// @brief Move assignment operator.
   ///
   /// @throws TextException If dictated by the test controller.
   ///
   /// @return A reference to `*this`.
   ///
   CopyOnlyTrackedObject &
   operator=(CopyOnlyTrackedObject &&) = delete;

   /// @brief Destructor.
   ///
   ~CopyOnlyTrackedObject() = default;
};

/// @brief Copy-only test type.
///
/// @throws TextException If dictated by the test controller.
///
struct MoveOnlyTrackedObject : public TrackedObject
{
   /// @brief Default constructor.
   ///
   /// @throws TextException If dictated by the test controller.
   ///
   MoveOnlyTrackedObject() = default;

   /// @brief Argument constructor.
   ///
   /// @throws TextException If dictated by the test controller.
   ///
   MoveOnlyTrackedObject(std::size_t identifier)
       : TrackedObject(identifier)
   {}

   /// @brief Copy constructor.
   ///
   /// @throws TextException If dictated by the test controller.
   ///
   MoveOnlyTrackedObject(MoveOnlyTrackedObject const &) = delete;

   /// @brief Copy assignment operator.
   ///
   /// @throws TextException If dictated by the test controller.
   ///
   /// @return A reference to `*this`.
   ///
   MoveOnlyTrackedObject &
   operator=(MoveOnlyTrackedObject const &) = delete;

   /// @brief Move constructor.
   ///
   /// @throws TextException If dictated by the test controller.
   ///
   MoveOnlyTrackedObject(MoveOnlyTrackedObject &&) = default;

   /// @brief Move assignment operator.
   ///
   /// @throws TextException If dictated by the test controller.
   ///
   /// @return A reference to `*this`.
   ///
   MoveOnlyTrackedObject &
   operator=(MoveOnlyTrackedObject &&) = default;

   /// @brief Destructor.
   ///
   ~MoveOnlyTrackedObject() = default;
};

}  // namespace swtl::test_helpers

/// @endcond
