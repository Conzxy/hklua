#ifndef HKLUA_FUNCTION_H__
#define HKLUA_FUNCTION_H__

#include <lua.hpp>
#include <tuple>

#include "hklua/stack.h"

namespace hklua {

namespace detail {

inline void StackPushMultiple(lua_State *env)
{
  // do nothing
}

template <typename T, typename... Args>
void StackPushMultiple(lua_State *env, T &&arg, Args &&...args)
{
  StackPush(env, arg);
  StackPushMultiple(env, std::forward<Args>(args)...);
}

template <size_t Idx, typename... Rets>
struct StackConvMultiple_impl {
  static bool apply(lua_State *env, int index, std::tuple<Rets...> &retval)
  {
    if (!StackConv(env, index, std::get<Idx - 1>(retval))) return false;
    return StackConvMultiple_impl<Idx - 1, Rets...>::apply(env, index - 1,
                                                           retval);
  }
};

template <typename... Rets>
struct StackConvMultiple_impl<0, Rets...> {
  inline static bool apply(lua_State *env, int index,
                           std::tuple<Rets...> &retval)
  {
    // do nothing
    return true;
  }
};

template <typename... Rets>
inline bool StackConvMultiple(lua_State *env, int index,
                              std::tuple<Rets...> &retval)
{
  return StackConvMultiple_impl<sizeof...(Rets), Rets...>::apply(env, index,
                                                                 retval);
}

} // namespace detail


} // namespace hklua

#endif // HKLUA_FUNCTION_H__
