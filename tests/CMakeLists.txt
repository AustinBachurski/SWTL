add_executable(tests)

target_sources(tests PRIVATE
  vector_tests.cpp
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
