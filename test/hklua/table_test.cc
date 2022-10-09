#include "hklua/table.h"
#include "hklua/env.h"
#include "hklua/variant.h"

#include <gtest/gtest.h>

using namespace hklua;

TEST (hklua_table, constrctor) {
  Env env("LUA VM");
  env.DoString("name = { firstname = \"X\", lastname = \"Y\" }");
  
  Table t(env);
  env.GetGlobal<Table>("name", t);
  char const *firstname;
  char const *lastname;
  t.GetField("firstname", firstname);
  t.GetField("lastname", lastname);
  env.StackPop(3);
  env.SetGlobal("name", lua_nil);
  
  bool success = env.GetGlobal<Table>("name", t);
    
  EXPECT_TRUE(!success);
  env.StackPop(1);
  
  assert(env.StackEmpty());
  env.StackDump();

  printf("firstname = %s, lastname = %s\n", firstname, lastname);
  
  success = env.GetGlobal<Table>("xxx", t);
  EXPECT_TRUE(!success);
  
  printf("\nGet nonexists global variable:\n");
  env.StackDump();
  env.StackPop(1);
  env.StackDump();
}

TEST (hklua_table, index_operator) {
  Env env("LUA VM");
  env.DoString("name = { firstname = \"X\", lastname = \"Y\" }");
  
  auto t = env.GetGlobalR<Table>("name");
  auto firstname = t["firstname"];
  auto lastname = t["lastname"];
  
  env.StackDump();
  printf("firstname = %s, lastname = %s\n", firstname.value().ToCString(), lastname.value().ToCString());
  
  env.StackPop(2);

  firstname = "Z";
  
  printf("firstname = %s\n", firstname.value().ToCString());
  env.StackPop();
  
  firstname = t["firstname"];
  printf("firstname = %s\n", firstname.value().ToCString());

  env.StackPop(2);
  EXPECT_TRUE(env.StackEmpty());
}

TEST (hklua_table, full) {
  Env env;
  env.DoFile("/home/conzxy/hklua/test/lua/table_test.lua");

  auto str = env.GetGlobalR<const char*>("str");
  printf("str = %s\n", str);
  env.StackPop();

  auto integer = env.GetGlobalR<int>("integer");
  printf("integer = %d\n", integer);
  env.StackPop();
  
  auto number = env.GetGlobalR<Number>("number");
  printf("number = %g\n", number);
  env.StackPop();

  // auto func = env.GetGlobalR<lua_CFunction>("func");
  // printf("function: %p\n", func);
  // env.StackPop();

  auto table = env.GetGlobalR<Table>("table");
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
  env.StackPop(3);
  
  auto td = table.GetFieldR<Table>("d");
  {
    TableGuard tdg(td);
    auto tda = td.GetFieldR<int>("a");
    printf("table.d.a = %d\n", tda);
    env.StackPop();
  }
  }

  assert(env.StackEmpty());
}
