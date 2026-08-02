add_library(swtl)

target_sources(swtl PUBLIC
   FILE_SET CXX_MODULES
   BASE_DIRS
   "${PROJECT_SOURCE_DIR}/src"

   FILES
   # ** PRIMARY MODULE INTERFACE **
   "${PROJECT_SOURCE_DIR}/src/swtl.cppm"

   # ** CONTAINERS **
   "${PROJECT_SOURCE_DIR}/src/containers/containers.cppm"
   "${PROJECT_SOURCE_DIR}/src/containers/concepts.cppm"
   "${PROJECT_SOURCE_DIR}/src/containers/vector.cppm"
   "${PROJECT_SOURCE_DIR}/src/containers/forward_list.cppm"

   # ** FORMAT **
   "${PROJECT_SOURCE_DIR}/src/format/format.cppm"

   # ** ITERATORS **
   "${PROJECT_SOURCE_DIR}/src/iterators/iterators.cppm"
   "${PROJECT_SOURCE_DIR}/src/iterators/contiguous_iterator.cppm"

   # ** MEMORY **
   "${PROJECT_SOURCE_DIR}/src/memory/memory.cppm"
   "${PROJECT_SOURCE_DIR}/src/memory/alloc_aware_destroy.cppm"
   "${PROJECT_SOURCE_DIR}/src/memory/alloc_aware_guards.cppm"
   "${PROJECT_SOURCE_DIR}/src/memory/alloc_aware_uninitialized.cppm"
   "${PROJECT_SOURCE_DIR}/src/memory/concepts.cppm"
)

enable_warnings_for_target(swtl)
enable_address_sanitizer_for_target(swtl)
enable_undefined_behavior_sanitizer_for_target(swtl)

add_executable(example)

target_sources(example PRIVATE
   main.cpp
)

target_link_libraries(example PRIVATE
   swtl
   swtl_test_helpers
)

enable_warnings_for_target(example)
enable_address_sanitizer_for_target(example)
enable_undefined_behavior_sanitizer_for_target(example)
