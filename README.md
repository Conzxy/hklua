# Hikari-lua
## Introduction
`Hikari-lua`是将Lua的C API封装成更为方便操作的C++ API。

## Usage
将Lua虚拟机（即`lua_State`）抽象为`Env`（表示Lua环境），因此你必须要创建该类型对象才能与Lua交互。
```cpp
Env env("Lua environment of main"); // No argument is also ok
```

### Manipulate table
```cpp
Env env;
bool success;
/* Default, the table Variable is not poped */
auto tb = env.GetGlobalTableR("name", 
                              &success /* nullptr is also ok */
                              /* true */ /* Pop the table immediatedly */);

/*
 * 2nd style:
 *  Table tb(env);
 *  env.GetGlobalTable("name", tb);
 */

/* You can use TableGuard to pop the table automatically */
{
  TableGuard tbg(tb);

  /*
   * tb = { a = 1.1, b = 3, c = "A", d = { a = 1 }}
   */
  auto a = tb.GetFieldR<double>("a" 
                                /*, true */ /* Pop the Variable */
                                /*, &success*/); // If you like Lua native type, use "Number"
  auto b = tb.GetFieldR<int>("b"); // If you like lua native type, use "Integer"
  auto c = tb.GetFieldR<char const *>("c");
  auto d = tb.GetFieldR<Table>("d");
  {
    TableGuard dg(d);

    auto d_a = d.GetFieldR<Table>("a");
  }
}
```
当然，你可能更喜欢用类似关联数组的语法来操作表，比如`table[key] = value`。
```cpp
env.DoString("name = { firstname = \"X\", lastname = \"Y\" }");

auto t = env.GetGlobalTableR("name");
TableGuard tg(t);
auto firstname = t["firstname"];
auto lastname = t["lastname"];
firstname = "Z";

firstname1 = t["firstname"];
assert(strcmp(firstname1.value().ToCString(), "Z") == 0);
```

### Call function
```cpp
Env env;
bool success;

/* 
function f(n1, n2, n3)
  return n1, n2, nil
end
*/

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
```

其他API可以参考`hklua/env.h`。
