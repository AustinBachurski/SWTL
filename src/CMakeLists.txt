add_library(swtl)

target_sources(swtl PUBLIC
  FILE_SET CXX_MODULES
  BASE_DIRS
  "${PROJECT_SOURCE_DIR}/src"

  FILES
  "${PROJECT_SOURCE_DIR}/src/containers/vector.cppm"
  "${PROJECT_SOURCE_DIR}/src/containers/forward_list.cppm"
  "${PROJECT_SOURCE_DIR}/src/core/memory.cppm"
  "${PROJECT_SOURCE_DIR}/src/core/format.cppm"
)

add_executable(example)

target_sources(example PRIVATE
  main.cpp
)

target_link_libraries(example PRIVATE swtl)

enable_warnings_for_target(example)
enable_address_sanitizer_for_target(example)
enable_undefined_behavior_sanitizer_for_target(example)
