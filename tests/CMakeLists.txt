add_executable(tests)

target_sources(tests PRIVATE
  vector_tests.cpp

  PRIVATE FILE_SET CXX_MODULES
  BASE_DIRS
  "${PROJECT_SOURCE_DIR}/tests/helpers"

  FILES
  "${PROJECT_SOURCE_DIR}/tests/helpers/swtl_test_helper_functions.cppm"
  "${PROJECT_SOURCE_DIR}/tests/helpers/swtl_test_helper_objects.cppm"
)

target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
target_link_libraries(tests PRIVATE swtl)

enable_warnings_for_target(tests)
enable_address_sanitizer_for_target(example)
enable_undefined_behavior_sanitizer_for_target(example)

catch_discover_tests(tests)
