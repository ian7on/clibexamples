# Minimum required version of CMake
cmake_minimum_required(VERSION 3.15)

# Project name and the type of project
project(avl_tree C)

# Add library for the data structure
add_library(avl_tree INTERFACE)

# Specify the include directories for this target
target_include_directories(avl_tree INTERFACE include)

# Enable testing if this project is built standalone or a specific option is ON
if(BUILD_UNIT_TESTS)
  include(CTest)
  enable_testing()

  # 1. Basic test
  set(TEST_NAME "test_avl_tree_basic")
  add_executable(test_avl_tree_basic.elf tests/test_avl_tree_basic.c)
  target_link_libraries(test_avl_tree_basic.elf PRIVATE avl_tree)
  target_compile_definitions(test_avl_tree_basic.elf
                             PRIVATE BUILD_UNIT_TESTS=${BUILD_UNIT_TESTS})
  add_test(NAME Test_AVL_Tree_Basic COMMAND test_avl_tree_basic.elf)
  if(CMAKE_BUILD_TYPE STREQUAL "Coverage")
    set_c_coverage_flags("${TEST_NAME}" C_COVERAGE_FLAGS)
    set_tests_properties(Test_AVL_Tree_Basic PROPERTIES ENVIRONMENT
                                                        "${C_COVERAGE_FLAGS}")
  endif()

endif()
