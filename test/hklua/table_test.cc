#include "hklua/table.h"
#include "hklua/env.h"
#include "hklua/variant.h"

#include <gtest/gtest.h>

using namespace hklua;

TEST (hklua_table, constrctor) {
  Env env("LUA VM");
  env.DoString("name = { firstname = \"X\", lastname = \"Y\" }");
  
  Table t;
  char const *firstname;
  char const *lastname;
  {
  TableGuard g(t);
  env.GetGlobal("name", t);
  t.GetField("firstname", firstname);
  t.GetField("lastname", lastname);
  env.SetGlobal("name", lua_nil);
  }
  
  env.StackDump();
  bool success = env.GetGlobal("name", t);
  EXPECT_TRUE(!success);
  
  env.StackDump();
  env.StackPop();

  printf("firstname = %s, lastname = %s\n", firstname, lastname);
  
  success = env.GetGlobal("xxx", t);
  EXPECT_TRUE(!success);
  
  printf("\nGet nonexists global variable:\n");
  env.StackDump();
  env.StackPop();
  env.StackDump();
}

TEST (hklua_table, index_operator) {
  Env env("LUA VM");
  env.DoString("name = { firstname = \"X\", lastname = \"Y\" }");
  
  auto t = env.GetGlobalTableR("name");
  auto firstname = t["firstname"];
  auto lastname = t["lastname"];
  
  env.StackDump();
  printf("firstname = %s, lastname = %s\n", firstname.value().ToCString(), lastname.value().ToCString());
  firstname = "Z";

  printf("firstname = %s\n", firstname.value().ToCString());

  auto firstname1 = t["firstname"];
  assert(strcmp(firstname1.value().ToCString(), "Z") == 0);
  printf("firstname1 = %s\n", firstname1.value().ToCString());
  env.StackPop();

  EXPECT_TRUE(env.StackEmpty());
}

TEST (hklua_table, full) {
  Env env;
  env.DoFile("/home/conzxy/hklua/test/lua/table_test.lua");

  auto str = env.GetGlobalR<const char*>("str");
  printf("str = %s\n", str);

  auto integer = env.GetGlobalR<int>("integer");
  printf("integer = %d\n", integer);
  
  auto number = env.GetGlobalR<Number>("number");
  printf("number = %g\n", number);

  // auto func = env.GetGlobalR<lua_CFunction>("func");
  // printf("function: %p\n", func);
  // env.StackPop();

  auto table = env.GetGlobalTableR("table");
  {
  TableGuard g(table);
  char const* a;
  int b;
  Number c;

  table.GetField("a", a);
  table.GetField("b", b);
  table.GetField("c", c);
  printf("a = %s\n", a);
  printf("b = %d\n", b);
  printf("c = %g\n", c);
  
  auto td = table.GetFieldTableR("d");
  {
    TableGuard tdg(td);
    env.StackDump();
    auto tda = td.GetFieldR<int>("a");
    printf("table.d.a = %d\n", tda);
  }
  }

  assert(env.StackEmpty());
}
