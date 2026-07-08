set_property(GLOBAL PROPERTY ENABLED_SANITIZERS "")

set(ASAN  "AddressSanitizer")
set(LSAN  "LeakSanitizer")
set(MSAN  "MemorySanitizer")
set(TSAN  "ThreadSanitizer")
set(UBSAN "UndefinedBehaviorSanitizer")

function(enable_address_sanitizer_for_target target)
  if(NOT CMAKE_CXX_COMPILER_ID MATCHES ".*Clang|GNU|MSVC")
    message(FATAL_ERROR "${ASAN} is not supported by compiler: '${CMAKE_CXX_COMPILER_ID}'.")
  endif()

  get_property(enabled_sanitizers GLOBAL PROPERTY ENABLED_SANITIZERS)

  if("${TSAN}" IN_LIST enabled_sanitizers OR "${MSAN}" IN_LIST enabled_sanitizers)
    message(FATAL_ERROR 
      "${ASAN} conflicts with currently enabled sanitizers.\n"
      "${ASAN} cannot be enabled while ${TSAN} or ${MSAN} is enabled.\n"
      "Currently enabled sanitizers: ${enabled_sanitizers}\n"
    )
  endif()

  if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(${target} PRIVATE $<$<CONFIG:Debug>:/fsanitize=address>)
  else()
    target_compile_options(${target} PRIVATE $<$<CONFIG:Debug>:-fsanitize=address -fno-omit-frame-pointer>)
    target_link_options(${target} PRIVATE $<$<CONFIG:Debug>:-fsanitize=address>)
  endif()

  set_property(GLOBAL APPEND PROPERTY ENABLED_SANITIZERS "${ASAN}")
  message(VERBOSE "${ASAN} has been enabled.")
endfunction()


function(enable_undefined_behavior_sanitizer_for_target target)
  if(NOT CMAKE_CXX_COMPILER_ID MATCHES ".*Clang|GNU")
    message(FATAL_ERROR "${UBSAN} is not supported by compiler: '${CMAKE_CXX_COMPILER_ID}'.")
  endif()

  target_compile_options(${target} PRIVATE $<$<CONFIG:Debug>:-fsanitize=undefined -fno-omit-frame-pointer>)
  target_link_options(${target} PRIVATE $<$<CONFIG:Debug>:-fsanitize=undefined>)

  set_property(GLOBAL APPEND PROPERTY ENABLED_SANITIZERS "${UBSAN}")
  message(VERBOSE "${UBSAN} has been enabled.")
endfunction()


function(enable_thread_sanitizer_for_target target)
  if(NOT CMAKE_CXX_COMPILER_ID MATCHES ".*Clang|GNU")
    message(FATAL_ERROR "${TSAN} is not supported by compiler: '${CMAKE_CXX_COMPILER_ID}'.")
  endif()

  get_property(enabled_sanitizers GLOBAL PROPERTY ENABLED_SANITIZERS)

  if("${ASAN}" IN_LIST enabled_sanitizers OR "${MSAN}" IN_LIST enabled_sanitizers OR "${LSAN}" IN_LIST enabled_sanitizers)
    message(FATAL_ERROR 
      "${TSAN} conflicts with currently enabled sanitizers.\n"
      "${TSAN} cannot be enabled while ${ASAN}, ${LSAN}, or ${MSAN} is enabled.\n"
      "Currently enabled sanitizers: ${enabled_sanitizers}\n"
    )
  endif()

  target_compile_options(${target} PRIVATE $<$<CONFIG:Debug>:-fsanitize=thread -fno-omit-frame-pointer>)
  target_link_options(${target} PRIVATE $<$<CONFIG:Debug>:-fsanitize=thread>)

  set_property(GLOBAL APPEND PROPERTY ENABLED_SANITIZERS "${TSAN}")
  message(VERBOSE "${TSAN} has been enabled.")
endfunction()


function(enable_memory_sanitizer_for_target target)
  if(NOT CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
    message(FATAL_ERROR "${MSAN} is not supported by compiler: '${CMAKE_CXX_COMPILER_ID}'.")
  endif()

  get_property(enabled_sanitizers GLOBAL PROPERTY ENABLED_SANITIZERS)

  if("${ASAN}" IN_LIST enabled_sanitizers OR "${TSAN}" IN_LIST enabled_sanitizers OR "${LSAN}" IN_LIST enabled_sanitizers)
    message(FATAL_ERROR 
      "${MSAN} conflicts with currently enabled sanitizers.\n"
      "${MSAN} cannot be enabled while ${ASAN}, ${LSAN}, or ${TSAN} is enabled.\n"
      "Currently enabled sanitizers: ${enabled_sanitizers}\n"
    )
  endif()

  target_compile_options(${target} PRIVATE $<$<CONFIG:Debug>:-fsanitize=memory -fsanitize-memory-track-origins -fno-omit-frame-pointer>)
  target_link_options(${target} PRIVATE $<$<CONFIG:Debug>:-fsanitize=memory -fsanitize-memory-track-origins>)

  set_property(GLOBAL APPEND PROPERTY ENABLED_SANITIZERS "${MSAN}")
  message(VERBOSE "${MSAN} has been enabled.")
endfunction()


function(enable_leak_sanitizer_for_target target)
  if(NOT CMAKE_CXX_COMPILER_ID MATCHES ".*Clang|GNU")
    message(FATAL_ERROR "${LSAN} is not supported by compiler: '${CMAKE_CXX_COMPILER_ID}'.")
  endif()

  get_property(enabled_sanitizers GLOBAL PROPERTY ENABLED_SANITIZERS)

  if("${TSAN}" IN_LIST enabled_sanitizers OR "${MSAN}" IN_LIST enabled_sanitizers)
    message(FATAL_ERROR 
      "${LSAN} conflicts with currently enabled sanitizers.\n"
      "${LSAN} cannot be enabled while ${TSAN} or ${MSAN} is enabled.\n"
      "Currently enabled sanitizers: ${enabled_sanitizers}\n"
    )
  endif()

  target_compile_options(${target} PRIVATE $<$<CONFIG:Debug>:-fsanitize=leak -fno-omit-frame-pointer>)
  target_link_options(${target} PRIVATE $<$<CONFIG:Debug>:-fsanitize=leak>)

  set_property(GLOBAL APPEND PROPERTY ENABLED_SANITIZERS "${LSAN}")
  message(VERBOSE "${LSAN} has been enabled.")
endfunction()
