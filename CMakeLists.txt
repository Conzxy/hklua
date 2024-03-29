cmake_minimum_required(VERSION 3.10)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

project(hklua)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-Og")

set(CXX_FLAGS 
 -Wall
 -Wextra
 -Wno-return-local-addr
 -Wno-unused-parameter
 -Wno-unused-function
 -Wno-switch
 -Wno-format-security
 # support INT2DOUBLE
 -Wno-strict-aliasing
 -Werror
 # make non-trivial(but like "trivial") class can reallocate
 -Wno-class-memaccess
 -Wno-implicit-fallthrough
 #-Wconversion
 #-Wshadow
 -Wno-maybe-uninitialized
 -Wwrite-strings # in fact, this is default specified
 #cxx standard 
 -std=c++14
 -pthread
    
 # linker opt
 -rdynamic
 # machine opt
 -march=native
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  list(REMOVE_ITEM CXX_FLAGS "-Wno-return-local-addr")
  list(REMOVE_ITEM CXX_FLAGS "-rdynamic")
  list(APPEND CXX_FLAGS "-Wthread-safety")
endif()

string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message(STATUS "BUILD_TYPE: ${CMAKE_BUILD_TYPE}")


set(HKLUA_SOURCE_DIR ${PROJECT_SOURCE_DIR}/hklua)
#set(THIRD_PARTY_DIR ${PROJECT_SOURCE_DIR}/third-party)

include_directories(${PROJECT_SOURCE_DIR})

# I don't want to use the builtin variable ${BUILD_SHARED_LIBS}
# since it you don't set it to ON explicitly, then the NOT 
# ${BUILD_SHARED_LIBS} will be false, and generated libraries 
# are static. That's not I wanted behavior. I want a variable,
# it can build shared libraries default even though I don't set 
# it explitly, and the build of static libraries is an option.
set(BUILD_STATIC_LIBS ON CACHE BOOL "Build static libraries")

macro (GenLib lib)
  #if (NOT ${BUILD_SHARED_LIBS})
  message(STATUS "Source list: ${ARGN}")
  if (${BUILD_STATIC_LIBS})
    message(STATUS "Build static library: ${lib}")
    add_library(${lib} STATIC ${ARGN})
    set_target_properties(${lib}
      PROPERTIES
      ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    )
  else ()
    message(STATUS "Build shared library: ${lib}")
    add_library(${lib} SHARED ${ARGN})
    set_target_properties(${lib}
      PROPERTIES
      LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    )
  endif (${BUILD_STATIC_LIBS})
endmacro ()

macro (GenApplication app_name src_list lib_list)
  set(SRC_LIST ${${src_list}})
  set(LIB_LIST ${${lib_list}})
  message(STATUS "application: ${app_name}")
  message(STATUS "Source list: ${SRC_LIST}")

  add_executable(${app_name} EXCLUDE_FROM_ALL ${SRC_LIST})
  list (LENGTH LIB_LIST lib_list_len)
  if (NOT ${lib_list_len} EQUAL 0)
    message(STATUS "Lib list: ${LIB_LIST}")
    target_link_libraries(${app_name} ${LIB_LIST})
  endif ()

  set_target_properties(${app_name}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
  )
endmacro()

message(STATUS "BUILD_ALL_TESTS = ${BUILD_ALL_TESTS}")
message(STATUS "BUILD_ALL_EXAMPLES = ${BUILD_ALL_EXAMPLES}")

set(BUILD_ALL_EXAMPLES OFF CACHE BOOL "Determine if build all examples")
set(EXAMPLE_SOURCE_DIR ${PROJECT_SOURCE_DIR}/example)

function (GenExample exec_name)
  if (${BUILD_ALL_EXAMPLES})   
    add_executable(${exec_name} ${ARGN})
  else ()
    add_executable(${exec_name} EXCLUDE_FROM_ALL ${ARGN})
  endif (${BUILD_ALL_EXAMPLES})   
  
  target_link_libraries(${exec_name} hklua) 
  set_target_properties(${exec_name}
    PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/example
  )
endfunction ()

if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
  set(HKLUA_LIB hklua)
else ()
  set(HKLUA_LIB hklua_debug)
endif()

add_subdirectory(third-party)
add_subdirectory(hklua)
add_subdirectory(test)
#add_subdirectory(third-party)
