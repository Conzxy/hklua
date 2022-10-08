#ifndef HKLUA_TYPE_H__
#define HKLUA_TYPE_H__

#include <lua.hpp>

namespace hklua {

using Integer = lua_Integer;
using Number = lua_Number;
using Double = Number;

struct NilIndicator {
};

using Nil = NilIndicator;

extern Nil lua_nil;

/** Enumrator wrapper */
enum HKLuaError { 
  HKLUA_OK = LUA_OK,
  HKLUA_ERRMEM = LUA_ERRMEM,
  HKLUA_ERRSYNTAX = LUA_ERRSYNTAX,
  HKLUA_ERRFILE = LUA_ERRFILE,
  HKLUA_ERRRUN = LUA_ERRRUN,
};

} // namespace hklua

#endif // HKLUA_TYPE_H__
