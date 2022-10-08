#ifndef HKLUA_TABLE_H__
#define HKLUA_TABLE_H__

#include <lua.hpp>

#include <string>
#include <utility> // forward
#include <assert.h>
#include <memory>

#include "hklua/stack.h"
#include "hklua/util/type_traits.h"

namespace hklua {

class Table;
class Variant;

Table CreateTable(lua_State *env, int narr = 0, int nrec = 0);


class Table {
#if 1
  class Proxy {
    friend class Table;

    using KeyValue = std::pair<Variant, Variant>;
   public:
    explicit Proxy(Table *table, KeyValue *kv = nullptr)
      : table_(table)
      , kv_(kv)
    {
    }
    
    // Proxy(Proxy const &) = default;
    Proxy(Proxy &&) = default;

    template <typename T, typename = typename std::enable_if<!AmbigiousCond<Proxy, T>::value>::type>
    Proxy &operator=(T const &v);
    /* std::pair<> requires the type must be complete type */
    
    Proxy &operator=(Proxy &&rhs)
    {
      std::swap(table_, rhs.table_);
      std::swap(kv_, rhs.kv_);
      return *this;
    }
    
    Variant &key() const noexcept;
    Variant &value() const noexcept;

   private:
    void SetKv(Variant k, Variant v) noexcept;

    Table *table_;
    std::unique_ptr<KeyValue> kv_;
  };

  friend class Proxy;
#endif
  friend Table CreateTable(lua_State *env, int narr, int nrec);
 public:
  /**
   * \index The index of the table in the stack(default: 0, invalid index)
   */
  explicit Table(lua_State *env, int index=0)
    : env_(env)
    , index_(index)
  {
  }
 
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
  
  template <typename T>
  Proxy operator[](T const &key);

  // Proxy operator[](char const *key);
  // Proxy operator[](std::string const &key);

  template <typename K, typename F>
  void SetField(K &&key, F&& field)
  {
    StackPush(env_, key);
    StackPush(env_, field);
    SetTable();
  }
  
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
  bool GetField(K &&key, F &field)
  {
    StackPush(env_, key);
    GetTable();
    return StackConv(env_, -1, field);
  }
  
  template <typename F>
  bool GetField(char const *key, F &field)
  {
    return GetStringField(key, field);
  }

  template <typename F>
  bool GetField(std::string const &key, F &field)
  {
    return GetStringField(key, field);
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
    return StackConv(env_, -1, field);
  }

  template <typename T>
  bool GetStringField(std::string const &key, T &field)
  {
    return GetStringField(key.c_str(), std::forward<T>(field));
  }

  void SetTable()
  {
    lua_settable(env_, index_);
  }
  
  void GetTable()
  {
    lua_gettable(env_, index_);
  }
  
  int index() const noexcept { return index_; }
  void SetIndex(int index) noexcept { index_ = index; }
  lua_State *env() const noexcept { return env_; }
 private:
  lua_State *env_;
  int index_;
};

inline Table CreateTable(lua_State *env, int narr, int nrec)
{
  Table table(env, -1);
  lua_createtable(env, narr, nrec);
  table.index_ = lua_gettop(env);
  return table;
}

inline bool StackConv(lua_State *env, int index, Table &tb)
{
  assert(env == tb.env());
  if (!lua_istable(env, index)) return false;
#if 0
  if (tb.index() != index) {
    lua_remove(env, tb.index());
    tb.SetIndex(index);
  }
#else
  tb.SetIndex(index);
#endif
  return true;
}

inline void StackPush(lua_State *env, Table tb)
{
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

} // namespace hklua

#endif // HKLUA_TABLE_H__
