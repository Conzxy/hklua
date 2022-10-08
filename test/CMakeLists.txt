#enable_testing()
file(GLOB HKLUA_TEST_SOURCES "${PROJECT_SOURCE_DIR}/test/*/*test*.cc")
file(GLOB HKLUA_BENCH_SOURCES "${PROJECT_SOURCE_DIR}/test/*/*bench*.cc")

#list(APPEND HKLUA_TEST_BENCH_SOURCES ${HKLUA_TEST_SOURCES} ${HKLUA_BENCH_SOURCES})

############################################
# User can determine whether to build all tests when build target all
# e.g. cmake --build */hklua/build [--target all -j 2]
############################################
set(BUILD_ALL_TESTS OFF CACHE BOOL "Don't exclude tests from target all")

message(STATUS "hklua test source files: ${HKLUA_TEST_SOURCES}")
message(STATUS "hklua bench source files: ${HKLUA_BENCH_SOURCES}")

if (NOT ${BUILD_ALL_TESTS})
  message(STATUS "[The hklua test and bench files are excluded from the build of target all]")
endif (NOT ${BUILD_ALL_TESTS})

############################################
# generate test and bench program
############################################
macro (GenTest SRC)
  foreach (hklua_test_source ${${SRC}})
    get_filename_component(hklua_test_filename ${hklua_test_source} NAME_WE)

    if (${BUILD_ALL_TESTS})
      add_executable(${hklua_test_filename} ${hklua_test_source})
    else ()
      message(STATUS "${hklua_test_filename} is excluded from target all")
      add_executable(${hklua_test_filename} EXCLUDE_FROM_ALL ${hklua_test_source})
    endif (${BUILD_ALL_TESTS})
  
    target_link_libraries(${hklua_test_filename} ${ARGN} lua gc)
    set_target_properties(${hklua_test_filename}
      PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test"
      COMMAND ${hklua_test_filename}
    )

    add_test(NAME ${hklua_test_filename} COMMAND ${hklua_test_filename})
  endforeach (hklua_test_source ${HKLUA_TEST_SOURCES})
endmacro()

# set (HKLUA_LIB "")
GenTest(HKLUA_TEST_SOURCES gtest gtest_main ${HKLUA_LIB})
GenTest(HKLUA_BENCH_SOURCES benchmark benchmark_main ${HKLUA_LIB})
