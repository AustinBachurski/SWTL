function(enable_warnings_for_target target)
    set(project_warnings "")

    if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
        set(project_warnings
            -Weverything
            # TODO: Disable what isn't needed.
        )

        if (WARNINGS_AS_ERRORS)
            list(APPEND project_warnings -Werror)
        endif()

    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(project_warnings
            -Wall
            -Wcast-align
            -Wconversion
            -Wdouble-promotion
            -Wduplicated-branches
            -Wduplicated-cond
            -Wextra
            -Wformat=2
            -Wimplicit-fallthrough
            -Wlogical-op
            -Wmisleading-indentation
            -Wnon-virtual-dtor
            -Wnull-dereference
            -Wold-style-cast
            -Woverloaded-virtual
            -Wpedantic
            -Wshadow
            -Wunused
            -Wuseless-cast
        )

        if (WARNINGS_AS_ERRORS)
            list(APPEND project_warnings -Werror)
        endif()

    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        set(project_warnings
            /Wall
            # TODO: Disable what isn't needed.
        )

        if (WARNINGS_AS_ERRORS)
            list(APPEND project_warnings /WX)
        endif()

    else()
        message(AUTHOR_WARNING
            "No warnings defined for compiler: '${CMAKE_CXX_COMPILER_ID}'")

    endif()

    target_compile_options(${target} PRIVATE ${project_warnings})
endfunction()
