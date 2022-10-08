#ifndef HKLUA_ENV_H__
#define HKLUA_ENV_H__

#include <lua.hpp>

#include <string>
#include <exception>

#include "hklua/function.h"
#include "hklua/table.h"

// #include "hklua/function.h"

namespace hklua {

/** Enumrator wrapper */
enum HKLuaError { 
  HKLUA_OK = LUA_OK,
  HKLUA_ERRMEM = LUA_ERRMEM,
  HKLUA_ERRSYNTAX = LUA_ERRSYNTAX,
  HKLUA_ERRFILE = LUA_ERRFILE,
  HKLUA_ERRRUN = LUA_ERRRUN,
};

struct EnvException : std::exception {
 public:
  explicit EnvException(char const *msg)
    : msg_(msg)
  {
  }
  
  explicit EnvException(std::string str)
    : msg_(std::move(str))
  {
  }
  
  ~EnvException() noexcept override
  {
  }

  char const *what() const noexcept override
  {
    return msg_.c_str();
  }
  
 private:
  std::string msg_;
};

class Env {
 public:
  Env()
    : Env("unnamed")
  {
  }

  explicit Env(std::string name)
   : env_(luaL_newstate())
   , name_(std::move(name))
  {
    if (!env_) {
      throw EnvException("Failed to create a Lua environment");
    }
  }
  
  ~Env() noexcept
  {
    if (env_)
      lua_close(env_);
  }

  Env(Env const &) = delete;
  Env &operator=(Env const &) = delete;

  Env(Env &&rhs) noexcept
    : env_(rhs.env_)
    , name_(std::move(rhs.name_))
  {
    rhs.env_ = nullptr;
  }
  
  Env &operator=(Env &&rhs) noexcept
  {
    std::swap(env_, rhs.env_);
    std::swap(name_, rhs.name_);
    return *this;
  }

  void OpenLibs()
  {
    luaL_openlibs(env_);
  }
  
  /*--------------------------------------------------*/
  /* Load Module                                      */
  /*--------------------------------------------------*/

  HKLuaError LoadString(char const *chunk)
  {
    return (HKLuaError)luaL_loadstring(env_, chunk);
  }
  
  HKLuaError DoString(char const *chunk)
  {
    return (HKLuaError)luaL_dostring(env_, chunk);
  }

  HKLuaError LoadFile(char const *filename)
  {
    return (HKLuaError)luaL_loadfile(env_, filename);
  }
  
  HKLuaError DoFile(char const *filename)
  {
    return (HKLuaError)luaL_dofile(env_, filename);
  }
  
  /*--------------------------------------------------*/
  /* Function Module                                  */
  /*--------------------------------------------------*/

  template <typename... Rets, typename... Args>
  std::tuple<Rets...> CallFunction(char const *name, int msgh,
                                   bool *success, bool pop, Args &&...args);
  
  char const *ToCString(int index=-1) const
  {
    return lua_tostring(env_, index);
  }

  std::string ToString(bool *success = nullptr, int index=-1) const
  {
    size_t len = 0;
    char const *cstr = lua_tolstring(env_, index, &len);
    if (!cstr) {
      if (success) *success = false;
      return {};
    }
    return std::string(cstr, len);
  }
  
  /**
   * \pre Error occurs
   */
  void CheckError()
  {
    // FIXME
    bool success;
    auto msg = ToString(&success);
    if (!success) {
      printf("No error message\n");
    } else {
      printf("Reason: %s\n", msg.c_str());
    }
  }
   
  /*--------------------------------------------------*/
  /* Global Variable Module                           */
  /*--------------------------------------------------*/

  template <typename T>
  bool GetGlobal(char const *name, T &var)
  {
    lua_getglobal(env_, name);
    return StackConv(env_, lua_gettop(env_), var);
  }

  template <typename T>
  T GetGlobal(char const *name, bool *success=nullptr)
  {
    T ret;
    const auto result = GetGlobal(name, ret);
    if (success) success = result;
    return ret;
  }
  
  template <typename T>
  void SetGlobal(char const *name, T value)
  {
    lua_setglobal(env_, name);
    StackPush(value);
  }

  /*--------------------------------------------------*/
  /* Stack Module                                     */
  /*--------------------------------------------------*/

  void StackPop(int n=1)
  {
    lua_pop(env_, n);
  }
  
  int StackGetTop() const noexcept
  {
    return lua_gettop(env_);
  }
  
  /**
   * StackGetTop() wrapper
   */
  int StackSize() const noexcept
  {
    return StackGetTop();
  }

  bool StackEmpty() const noexcept
  {
    return StackGetTop() == 0;
  }

  void StackSetTop(int index)
  {
    lua_settop(env_, index);
  }
  
  /**
   * lua_checkstack() wrapper
   */
  bool StackExpand(int n)
  {
    return lua_checkstack(env_, n) != 0;
  }

  void StackDump()
  {
    printf("StackDump of [%s]:\n", name_.c_str());
    ::hklua::StackDump(env_);
  }

  void StackRemove(int index)
  {
    lua_remove(env_, index);
  }

  void StackReplace(int index)
  {
    lua_replace(env_, index);
  }

  void StackCopy(int from, int to)
  {
    lua_copy(env_, from, to);
  }

  void StackPushValue(int index)
  {
    lua_pushvalue(env_, index);
  }

  void StackInsert(int index)
  {
    lua_insert(env_, index);
  }

  void StackRotate(int index, int n)
  {
    lua_rotate(env_, index, n);
  }
  
  template <typename T>
  void StackPush(T &&var)
  {
    ::hklua::StackPush(env_, std::forward<T>(var));
  }
  
  template <typename T>
  bool StackTo(int index, T &var)
  {
    return ::hklua::StackConv(env_, index, var);
  }

  /*--------------------------------------------------*/
  /* Table Module                                     */
  /*--------------------------------------------------*/

  Table CreateTable(int narr = 0, int nrec = 0)
  {
    return ::hklua::CreateTable(env_, narr, nrec);
  }
  
  /*--------------------------------------------------*/
  /* Getter                                           */
  /*--------------------------------------------------*/

  std::string const& name() const noexcept { return name_; }
  lua_State *env() const noexcept { return env_; }

 private:
  lua_State *env_;
  std::string name_;
};

/* This must be put outside of class */
template<>
inline Table Env::GetGlobal<Table>(char const *name, bool *success)
{
  Table ret(env_);
  const auto result = GetGlobal(name, ret);
  if (success) *success = result;
  return ret;
}


template <typename... Rets, typename... Args>
std::tuple<Rets...> Env::CallFunction(char const *name, int msgh,
                                 bool *success, bool pop, Args &&...args)
{
  using RetType = std::tuple<Rets...>;
  if (success) *success = false;

  lua_getglobal(env_, name);
  if (!lua_isfunction(env_, -1)) {
    return {};
  }

  detail::StackPushMultiple(env_, std::forward<Args>(args)...);
  if (LUA_OK != lua_pcall(env_, sizeof...(args), sizeof...(Rets), msgh)) {
    return {};
  }

  RetType retval;
  if (!detail::StackConvMultiple(env_, lua_gettop(env_), retval)) {
    return {};
  }

  if (success) *success = true;
  if (pop)
    lua_pop(env_, (int)sizeof...(Rets));

  return retval;
}

} // namespace hklua

#endif // HKLUA_ENV_H__
