add_library(example_lib INTERFACE)

target_sources(example_lib INTERFACE
    FILE_SET HEADERS
    BASE_DIRS
    "${PROJECT_SOURCE_DIR}/include"

    FILES
    #"${PROJECT_SOURCE_DIR}/include/Example_Lib/example.hpp"
)

add_executable(example)

target_sources(example PRIVATE
    main.cpp
)

target_link_libraries(example PRIVATE example_lib)

enable_warnings_for_target(example)

target_compile_options(example PRIVATE
    $<$<CONFIG:Debug>:-freflection -fsanitize=address,undefined>
    $<$<CONFIG:Release>:-freflection>
)

target_link_options(example PRIVATE
    $<$<CONFIG:Debug>:-fsanitize=address,undefined>
)
