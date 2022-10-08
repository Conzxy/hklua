#include "hklua/function.h"

#include <inttypes.h>
#include <gtest/gtest.h>

#include "hklua/env.h"
#include "leak_detector.h" 

using namespace hklua;

TEST (function_test, simple) {
  Env env("Lua VM main");
  bool success;
  if (HKLUA_OK != env.DoFile("/home/conzxy/hklua/test/lua/function_test.lua")) {
    printf("Failed to dofile\n");
    env.CheckError();
    env.StackDump();
    exit(1);
  }
  
  int a;
  char const *b;
  VI<int> c; // return values
  std::tie(a, b, c) = env.CallFunction<int, char const*, VI<int>>("f", 
      0, // msgh
      &success, // success indicator
      true, // pop
      1, "ABCDEFGHIJKL", 3 // arguments
      );

  if (!success) {
    printf("Failed to call function\n");
    env.CheckError();
    env.StackDump();
    exit(1);
  }
  
  printf("a = %d, b = %s\n", a, b);
  
  if (!c.is_nil) {
    printf("c = %d\n", c.data);
  } else {
    printf("c is nil\n");
  }

  env.StackDump();
  CHECK_LEAKS();
}
