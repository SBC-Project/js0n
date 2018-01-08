cmake_minimum_required(VERSION 3.9.0)

set(BINARY_DIRECTORY ${CMAKE_SOURCE_DIR})
option(COMPILE_TESTS "COMPILE_TESTS" OFF)

set(COMPILER_FLAGS "")

project(js0nPP)

set(FILES "src/js0nPP.cpp" "src/js0nPP.h" "test/example.cpp")
include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/test)

add_executable(js0nPP_Example ${FILES})

if(COMPILE_TESTS)
  set(TEST_FILES "src/js0nPP.cpp" "src/js0nPP.h" "test/test.cpp")

  add_executable(js0nPP_Tests ${TEST_FILES})
  install(TARGETS js0nPP_Tests DESTINATION ${BINARY_DIRECTORY})
endif()

install(TARGETS js0nPP_Example DESTINATION ${BINARY_DIRECTORY})
