add_library(swtl_test_helpers)

target_sources(swtl_test_helpers PUBLIC
   FILE_SET CXX_MODULES
   BASE_DIRS
   "${PROJECT_SOURCE_DIR}/tests/helpers"

   FILES
   "${PROJECT_SOURCE_DIR}/tests/helpers/swtl_test_helper_functions.cppm"
   "${PROJECT_SOURCE_DIR}/tests/helpers/swtl_test_helper_objects.cppm"
)

target_link_libraries(swtl_test_helpers PRIVATE swtl)

enable_warnings_for_target(swtl_test_helpers)
enable_address_sanitizer_for_target(swtl_test_helpers)
enable_undefined_behavior_sanitizer_for_target(swtl_test_helpers)

add_executable(tests)

target_sources(tests PRIVATE
   vector_tests.cpp
)

target_link_libraries(tests PRIVATE Catch2::Catch2WithMain
   swtl
   swtl_test_helpers
)

enable_warnings_for_target(tests)
enable_address_sanitizer_for_target(tests)
enable_undefined_behavior_sanitizer_for_target(tests)

catch_discover_tests(tests)
