file (GLOB HKLUA_SRC
  *.cc)

GenLib(${HKLUA_LIB} ${HKLUA_SRC})

find_path(LUA_INCLUDE_DIR lua.hpp PATHS "/usr/local/include" "/usr/include" "~" NO_DEFAULT_PATH)

if (${LUA_INCLUDE_DIR} STREQUAL "LUA_INCLUDE_DIR-NOTFOUND")
  message(FATAL_ERROR "There is no lua include directory in your machine")
endif ()

message(STATUS "Lua include dir = ${LUA_INCLUDE_DIR}")
target_include_directories(${HKLUA_LIB} PUBLIC ${LUA_INCLUDE_DIR})
