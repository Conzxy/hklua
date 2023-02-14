#ifndef HKLUA_TABLE_H__
#define HKLUA_TABLE_H__

#include <lua.hpp>

#include <string>
#include <utility> // forward
#include <assert.h>

#include "hklua/stack.h"
#include "hklua/util/type_traits.h"

namespace hklua {

class Table;
class TableGuard;
class Env;

Table CreateTable(lua_State *env, int narr = 0, int nrec = 0);

/**
 * \brief Represents a Lua table object
 * 
 * The table don't manage the lifetime of object
 * since lua C API also can pop the table object in the stack
 * e.g. function arguments
 */
class Table {
  /**
   * Used for implementing operator[]
   */
  class Proxy;

  friend class Proxy;
  friend class TableGuard;
  friend Table CreateTable(lua_State *env, int narr, int nrec);
  friend bool StackConv(lua_State *env, int index, Table &tb);
 public:
  /**
   * \index The index of the table in the stack(default: 0, invalid index)
   */
  explicit Table(lua_State *env, int index=0)
    : env_(env)
    , index_(index)
  {
  }

  Table()
    : Table(nullptr)
  {
  }
  
  explicit Table(Env &env, int index=0);
#if 0
  Table(Table const &) = delete;
  Table &operator=(Table const &) = delete;

  Table(Table &&rhs) noexcept
   : env_(rhs.env_)
   , index_(rhs.index_)
  {
    rhs.index_ = 0;
  }

  Table &operator=(Table &&rhs) noexcept
  {
    // this == &rhs is also OK
    std::swap(env_, rhs.env_); // Not neccessary
    std::swap(index_, rhs.index_);
    return *this;
  }

  ~Table() noexcept
  {
    if (index_ == lua_gettop(env_))
      lua_pop(env_, 1);
    else
      lua_remove(env_, index_);
  }
#else
  ~Table() noexcept
  {
  }
#endif
  
  /**
   * Like associated array
   * You can think this is a syntactic sugar
   */
  template <typename T>
  Proxy operator[](T const &key);
  
  /*--------------------------------------------------*/
  /* Field Module                                     */
  /*--------------------------------------------------*/

  template <typename K, typename F>
  void SetField(K &&key, F&& field)
  {
    StackPush(env_, key);
    StackPush(env_, field);
    SetTable();
  }
  
  /**
   * Use lua_setfield to optimize
   */
  template <typename F>
  void SetField(char const *key, F&& field)
  {
    SetStringField(key, std::forward<F>(field));
  }
  
  template <typename F>
  void SetField(std::string const &key, F &&field)
  {
    SetStringField(key, std::forward<F>(field));
  }
  
  template <typename F, typename K>
  bool GetField(K const &key, F &field, bool pop=true)
  {
    StackPush(env_, key);
    GetTable();
    const auto ret = StackConv(env_, lua_gettop(env_), field);
    if (pop) lua_pop(env_, 1);
    return ret;
  }
 
  template <typename K>
  bool GetFieldTable(K const &key, Table &field, bool pop=false)
  {
    return GetField<Table, K>(key, field, pop);
  }

  template <typename K>
  bool GetField(K const &key, Table &field, bool pop=false)
  {
    return GetFieldTable(key, field, pop);
  }

  /**
   * Use lua_getfield to optimize
   */
  template <typename F>
  bool GetField(char const *key, F &field, bool pop=true)
  {
    auto ret = GetStringField(key, field);
    if (pop) lua_pop(env_, 1);
    return ret;
  }

  template <typename F>
  bool GetField(std::string const &key, F &field, bool pop=true)
  {
    auto ret = GetStringField(key, field);
    if (pop) lua_pop(env_, 1);
    return ret;
  }
  
  template <typename F, typename K>
  F GetFieldR(K const &key, bool pop=true, bool *success=nullptr)
  {
    F ret;
    auto const result = GetField<F, K>(key, ret, pop);
    if (success) *success = result;
    return ret;
  }

  template <typename K>
  Table GetFieldTableR(K const &key, bool pop=false, bool *success=nullptr)
  {
    return GetFieldR<Table, K>(key, pop, success);
  }


  template <typename T>
  void SetStringField(char const *key, T &&field)
  {
    StackPush(env_, field);
    lua_setfield(env_, index_, key);
  }
  
  template <typename T>
  void SetStringField(std::string const &key, T &&field)
  {
    SetStringField(key.c_str(), std::forward<T>(field));
  }
  
  template <typename T>
  bool GetStringField(char const *key, T &field)
  {
    lua_getfield(env_, index_, key);
    return StackConv(env_, lua_gettop(env_), field);
  }

  template <typename T>
  bool GetStringField(std::string const &key, T &field)
  {
    return GetStringField(key.c_str(), std::forward<T>(field));
  }

  /*--------------------------------------------------*/
  /* Getter Module                                    */
  /*--------------------------------------------------*/
  
  Integer len() const noexcept
  { 
    Integer ret;
    lua_len(env_, index_);
    StackConv(env_, -1, ret);
    lua_pop(env_, 1);
    return ret;
  }

  int index() const noexcept { return index_; }
  lua_State *env() const noexcept { return env_; }
 private:
  void SetIndex(int index) noexcept { index_ = index; }

  void SetTable()
  {
    lua_settable(env_, index_);
  }
  
  void GetTable()
  {
    lua_gettable(env_, index_);
  }

  lua_State *env_;
  int index_;
};


inline Table CreateTable(lua_State *env, int narr, int nrec)
{
  Table table(env, lua_gettop(env));
  lua_createtable(env, narr, nrec);
  table.index_ = lua_gettop(env);
  return table;
}

inline bool StackConv(lua_State *env, int index, Table &tb)
{
  /* Must in the same environment */
  if (!lua_istable(env, index)) return false;
#if 0
  if (tb.index() != index) {
    lua_remove(env, tb.index());
    tb.SetIndex(index);
  }
#else
  new (&tb) Table(env, index);
#endif
  return true;
}

inline void StackPush(lua_State *env, Table tb)
{
  /* Unsetted table is not pushed */
  if (tb.index() == 0) return;
#if 0
  const auto top = lua_gettop(env);
  if (tb.index() != top) {
    lua_pushvalue(env, tb.index());
    /* Although lua_rotate() time cost is O(n),
     * the size of stack is not large in most cases
     */
    // lua_remove(env, tb.index());
    tb.SetIndex(top+1);
  }
  // do nothing
#else
    lua_pushvalue(env, tb.index());
#endif
}

/**
 * RAII to manage table resource
 */
class TableGuard {
 public:
  explicit TableGuard(Table &tb)
   : tb_(tb)
  {
  }

  ~TableGuard() noexcept
  {
#if 0
    if (tb_.index_ == lua_gettop(tb_.env_))
      lua_pop(tb_.env_, 1);
    else
      lua_remove(tb_.env_, tb_.index_);
#else
    assert(tb_.index_ == lua_gettop(tb_.env_) || tb_.index_ == -1);
    lua_pop(tb_.env_, 1);
#endif
  }

 private:
  Table &tb_;
};

} // namespace hklua

#endif // HKLUA_TABLE_H__
