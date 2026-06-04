cmake_minimum_required(VERSION 3.25...4.3)

project(CPP_TEMPLATE LANGUAGES CXX)

option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors." ON)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/CompilerWarnings.cmake")

add_subdirectory(src)
