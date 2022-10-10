#ifndef HKLUA_STACK_H__
#define HKLUA_STACK_H__

#include <lua.hpp>
#include <string>
#include <stddef.h>
#include <stdint.h>

#include "hklua/type.h"
#include "hklua/variable_indicator.h"

namespace hklua {

class Table;
class Variant;

#define STACKPUSH_INTEGER_DEFINE(type)                                         \
  inline void StackPush(lua_State *env, type i) { lua_pushinteger(env, i); }

STACKPUSH_INTEGER_DEFINE(uint8_t)
STACKPUSH_INTEGER_DEFINE(int8_t)
STACKPUSH_INTEGER_DEFINE(int16_t)
STACKPUSH_INTEGER_DEFINE(uint16_t)
STACKPUSH_INTEGER_DEFINE(int32_t)
STACKPUSH_INTEGER_DEFINE(uint32_t)
STACKPUSH_INTEGER_DEFINE(int64_t)
STACKPUSH_INTEGER_DEFINE(uint64_t)
STACKPUSH_INTEGER_DEFINE(Integer)

#undef STACKCONV_INTEGER_DEFINE

#define STACKPUSH_NUMBER_DEFINE(type) \
inline void StackPush(lua_State *env, type n) { lua_pushnumber(env, n); }

STACKPUSH_NUMBER_DEFINE(double)
STACKPUSH_NUMBER_DEFINE(float)

#undef STACKPUSH_NUMBER_DEFINE

inline void StackPush(lua_State *env, char const *str)
{
  lua_pushstring(env, str);
}

inline void StackPush(lua_State *env, std::string const &str)
{
  lua_pushlstring(env, str.c_str(), str.size());
}

inline void StackPush(lua_State *env, bool b)
{
  lua_pushboolean(env, b ? 0 : 1);
}

inline void StackPush(lua_State *env, lua_CFunction func)
{
  lua_pushcfunction(env, func);
}

void StackPush(lua_State *env, Table tb);
void StackPush(lua_State *env, Variant var);

template <typename T>
inline void StackPush(lua_State *env, VI<T> &vi)
{
  if (vi.is_nil)
    lua_pushnil(env);
  else
    StackPush(vi.data);
}

inline void StackPush(lua_State *env, Nil nil)
{
  lua_pushnil(env);
}

#define STACKCONV_INTEGER_DEFINE(type)                                         \
  inline bool StackConv(lua_State *env, int index, type &i)                    \
  {                                                                            \
    int ret;                                                                   \
    i = lua_tointegerx(env, index, &ret);                                      \
    return ret != 0;                                                           \
  }

STACKCONV_INTEGER_DEFINE(uint8_t)
STACKCONV_INTEGER_DEFINE(int8_t)
STACKCONV_INTEGER_DEFINE(int16_t)
STACKCONV_INTEGER_DEFINE(uint16_t)
STACKCONV_INTEGER_DEFINE(int32_t)
STACKCONV_INTEGER_DEFINE(uint32_t)
STACKCONV_INTEGER_DEFINE(int64_t)
STACKCONV_INTEGER_DEFINE(uint64_t)
STACKCONV_INTEGER_DEFINE(Integer)

#undef STACKCONV_INTEGER_DEFINE

#define STACKCONV_NUMBER_DEFINE(type)                                           \
  inline bool StackConv(lua_State *env, int index, type &n)                    \
  {                                                                            \
    int ret;                                                                   \
    n = lua_tonumberx(env, index, &ret);                                       \
    return ret != 0;                                                           \
  }

STACKCONV_NUMBER_DEFINE(double)
STACKCONV_NUMBER_DEFINE(float)

#undef STACKCONV_NUMBER_DEFINE

inline bool StackConv(lua_State *env, int index, bool &boolean)
{
  boolean = lua_toboolean(env, index);
  /* Except nil and false, other values should be think to true in Lua */
  return true;
}

inline bool StackConv(lua_State *env, int index, char const *&str)
{
  str = lua_tostring(env, index);
  return str != nullptr;
}

inline bool StackConv(lua_State *env, int index, std::string &str)
{
  size_t len = 0;
  auto ret = lua_tolstring(env, index, &len);
  if (ret) {
    str.append(ret, len);
  }
  return ret;
}

inline bool StackConv(lua_State *env, int index, lua_CFunction &func)
{
  func = lua_tocfunction(env, index);
  return func != NULL;
}

bool StackConv(lua_State *env, int index, Table &tb);
bool StackConv(lua_State *env, int index, Variant &var);

template <typename T>
inline bool StackConv(lua_State *env, int index, VI<T>& var)
{
  if (lua_isnil(env, index)) {
    var.is_nil = true;
    return true;
  }
  return StackConv(env, index, var.data);
}

void StackDump(lua_State *env);

} // namespace hklua

#endif // HKLUA_STACK_H__
