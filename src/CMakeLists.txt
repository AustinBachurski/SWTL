add_library(swtl)

target_sources(swtl PUBLIC
  FILE_SET CXX_MODULES
  BASE_DIRS
  "${PROJECT_SOURCE_DIR}/src"

  FILES
  "${PROJECT_SOURCE_DIR}/src/containers/vector.cppm"
  "${PROJECT_SOURCE_DIR}/src/containers/forward_list.cppm"
)

add_executable(example)

target_sources(example PRIVATE
  main.cpp
)

target_link_libraries(example PRIVATE swtl)

enable_warnings_for_target(example)

target_compile_options(example PRIVATE
  $<$<CONFIG:Debug>:-fsanitize=address,undefined>
)

target_link_options(example PRIVATE
  $<$<CONFIG:Debug>:-fsanitize=address,undefined>
)
