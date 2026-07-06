add_executable(tests)

target_sources(tests PRIVATE
  vector_tests.cpp

  PRIVATE FILE_SET CXX_MODULES
  BASE_DIRS
  "${PROJECT_SOURCE_DIR}/tests/helpers"

  FILES
  "${PROJECT_SOURCE_DIR}/tests/helpers/swtl_test_helpers.cppm"
)

target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
target_link_libraries(tests PRIVATE swtl)

enable_warnings_for_target(tests)

target_compile_options(tests PRIVATE
  $<$<CONFIG:Debug>:-fsanitize=address,undefined>
)

target_link_options(tests PRIVATE
  $<$<CONFIG:Debug>:-fsanitize=address,undefined>
)

catch_discover_tests(tests)
