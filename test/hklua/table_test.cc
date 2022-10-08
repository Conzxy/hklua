#include "hklua/table.h"
#include "hklua/env.h"
#include "hklua/variant.h"

#include <gtest/gtest.h>

using namespace hklua;

TEST (hklua_table, constrctor) {
  Env env("LUA VM");
  env.DoString("name = { firstname = \"X\", lastname = \"Y\" }");
  
  auto t = env.GetGlobal<Table>("name", nullptr);
  char const *firstname;
  char const *lastname;
  t.GetField("firstname", firstname);
  t.GetField("lastname", lastname);
  env.StackPop(3);
  env.SetGlobal("name", lua_nil);
  
  bool success;
  t = env.GetGlobal<Table>("name", &success);
    
  EXPECT_TRUE(!success);
  env.StackPop(1);
  
  assert(env.StackEmpty());
  env.StackDump();

  printf("firstname = %s, lastname = %s\n", firstname, lastname);
  
  auto t2 = env.GetGlobal<Table>("xxx", &success);
  EXPECT_TRUE(!success);
  
  printf("\nGet nonexists global variable:\n");
  env.StackDump();
  env.StackPop(1);
  env.StackDump();
}

TEST (hklua_table, index_operator) {
  Env env("LUA VM");
  env.DoString("name = { firstname = \"X\", lastname = \"Y\" }");
  
  auto t = env.GetGlobal<Table>("name");
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
