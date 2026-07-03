cmake_minimum_required(VERSION 4.3)
#set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "f35a9ac6-8463-4d38-8eec-5d6008153e7d") # 4.3.3/4 version?  But says invalid???
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "451f2fe2-a8a2-47c3-bc32-94786d8fc91b")
set(CMAKE_CXX_MODULE_STD 1)

project(SWTL LANGUAGES CXX)

option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors." ON)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/CompilerWarnings.cmake")

Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.15.1
)

FetchContent_MakeAvailable(Catch2)

# When using FetchContent, include(Catch) will fail unless CMAKE_MODULE_PATH
# is explicitly updated to include the extras directory.
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)

add_subdirectory(src)
add_subdirectory(tests)
