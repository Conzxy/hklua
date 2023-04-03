file (GLOB HKLUA_SRC
  *.cc)

GenLib(${HKLUA_LIB} ${HKLUA_SRC})

# Don't use find_path.
# It is not recursive.
file(GLOB_RECURSE LUA_HPP_PATHS 
  /usr/include/*/lua.hpp
  /usr/local/*/lua.hpp
  ~/*/lua.hpp)
message(STATUS "LUA_HPP_PATHS = ${LUA_HPP_PATHS}")

# Check if lua path contains lua.hpp exists
list(LENGTH LUA_HPP_PATHS lua_path_len)
if (lua_path_len EQUAL 0)
  message(FATAL_ERROR "There is no such directory include lua.hpp")
endif ()

list(GET LUA_HPP_PATHS 0 LUA_HPP_PATH)
get_filename_component(LUA_INCLUDE_DIR ${LUA_HPP_PATH} DIRECTORY)

message(STATUS "Lua include dir = ${LUA_INCLUDE_DIR}")
target_include_directories(${HKLUA_LIB} PUBLIC ${LUA_INCLUDE_DIR})
