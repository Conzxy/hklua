#include "hklua/stack.h"

#include <stdio.h>

namespace hklua {

void StackDump(lua_State *env)
{
  int top = lua_gettop(env);
  if (top == 0) {
    puts("Stack is empty!");
    return;
  }
  
  printf("Total elements in stack: %d\n", top);
  puts("--------------------------------------------------");
  int type;
  for (int i = top; i > 0; --i) {
    type = lua_type(env, i);
    printf("(%s): ", lua_typename(env, type));
    switch (lua_type(env, i)) {
      case LUA_TNUMBER:
        printf("%g\n", lua_tonumber(env, i));
        break;
      case LUA_TSTRING:
        printf("%s\n", lua_tostring(env, i));
        break;
      case LUA_TNIL:
        printf("\n");
        break;
      case LUA_TBOOLEAN:
        printf("%s\n", lua_toboolean(env, i) != 0 ? (char const*)"true" : (char const*)"false");
        break;
      case LUA_TTABLE:
      case LUA_TTHREAD:
      case LUA_TUSERDATA:
      case LUA_TLIGHTUSERDATA:
      case LUA_TFUNCTION:
        // FIXME
        printf("\n");
        break;
    }
  }
  puts("--------------------------------------------------");
}

} // namespace hklu
