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

} // namespace hklua

#endif // HKLUA_TYPE_H__
