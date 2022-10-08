#ifndef HKLUA_VARIANT_H__
#define HKLUA_VARIANT_H__

#include <assert.h>
#include <string.h>
#include <string>

#include "hklua/type.h"
#include "hklua/stack.h"
#include "hklua/table.h"

namespace hklua {

#define PADDING sizeof(size_t)

enum HKVariantType : unsigned char {
  HK_UNSET = 0,
  HK_INT,
  HK_BOOLEAN,
  HK_NUMBER,
  HK_CSTRING,
  HK_STRING,
  HK_FUNCTION,
  HK_TABLE,
  HK_NIL,
};

class Variant {
 public:
  Variant()
    : type_(HK_UNSET)
  {
  }

  Variant(Nil nil)
    : type_(HK_NIL)
  {
  }
  
  Variant(bool b)
    : type_(HK_BOOLEAN)
    , bool_(b)
  {
  }

  Variant(Integer i)
    : type_(HK_INT)
    , integer_(i)
  {
  }

  Variant(Number n)
    : type_(HK_NUMBER)
    , number_(n)
  {
  }

  Variant(char const *cstr)
    : type_(HK_CSTRING)
    , cstr_(cstr)
  {
  }

  Variant(lua_CFunction func)
    : type_(HK_FUNCTION)
    , func_(func)
  {
  }

  Variant(Table t)
    : type_(HK_TABLE)
    , table_(t)
  {
  }

  Variant(Variant const &rhs)
    : type_(rhs.type_)
  {
    // FIXME
    static_assert(sizeof(Variant) == sizeof(Table) + PADDING,
                  "The size of Variant must be the sum of Table and PADDING(8)");
    memcpy(reinterpret_cast<char *>(this) + PADDING, 
           reinterpret_cast<char const*>(&rhs) + PADDING, 
           sizeof(Variant)-PADDING);
  }

  Variant(Variant &&rhs) noexcept
    : type_(HK_UNSET)
    , table_(nullptr)
  {
    this->swap(rhs);
  }

  Variant &operator=(Variant const &rhs)
  {
    // type_ = rhs.type_;
    // // FIXME
    // static_assert(sizeof(type_) == sizeof(unsigned char),
    //               "The size of HKVariantType must be 1");
    // static_assert(sizeof(Variant) == sizeof(Table) + PADDING,
    //               "The size of Variant must be the sum of Table and type_");
    // memcpy(reinterpret_cast<char *>(this) + PADDING, &rhs + PADDING, 
    //        sizeof(Variant)-PADDING);
  
    Variant(rhs).swap(*this);
    return *this;
  }

  Variant &operator=(Variant &&rhs) noexcept
  {
    // FIXME
    this->swap(rhs);
    return *this;
  }

  Variant &operator=(Integer i)
  {
    // FIXME free resources
    type_ = HK_INT;
    integer_ = i;
    return *this;
  }

  Variant &operator=(bool b)
  {
    type_ = HK_BOOLEAN;
    bool_ = b;
    return *this;
  }

  Variant &operator=(Number n)
  {
    type_ = HK_NUMBER;
    number_ = n;
    return *this;
  }

  Variant &operator=(char const *str)
  {
    type_ = HK_CSTRING;
    cstr_ = str;
    return *this;
  }

  Variant &operator=(lua_CFunction func)
  {
    type_ = HK_FUNCTION;
    func_ = func;
    return *this;
  }

  Variant &operator=(Table tb)
  {
    type_ = HK_TABLE;
    table_ = tb;
    return *this;
  }

  Variant &operator=(Nil nil)
  {
    type_ = HK_NIL;
    return *this;
  }

  ~Variant() noexcept
  {
#if 0
    if (type_ == HK_TABLE) {
      table_.~Table();
    }
#endif
  }

  Integer &ToInteger() noexcept
  {
    assert(HK_INT == type_);
    return integer_;
  }
  
  bool &ToBoolean() noexcept
  {
    assert(HK_BOOLEAN == type_);
    return bool_;
  }

  Number &ToNumber() noexcept
  {
    assert(HK_NUMBER == type_);
    return number_;
  }

  char const *&ToCString() noexcept
  {
    assert(HK_CSTRING == type_);
    return cstr_;
  }

  lua_CFunction &ToCFunction() noexcept
  {
    assert(HK_FUNCTION == type_);
    return func_;
  }

  Table &ToTable() noexcept
  {
    assert(HK_TABLE == type_);
    return table_;
  }

  Nil ToNil() noexcept
  {
    assert(HK_NIL == type_);
    return lua_nil;
  }

  void swap(Variant &rhs) noexcept
  {
    static_assert(sizeof(Variant) == sizeof(Table) + PADDING,
                  "The size of Variant must be the sum of Table and PADDING");
    std::swap(type_, rhs.type_);
    char buf[sizeof(Variant) - PADDING];
    char *self = reinterpret_cast<char *>(this) + PADDING;
    char *other = reinterpret_cast<char *>(&rhs) + PADDING;
    memcpy(buf, self, sizeof buf);
    memcpy(self, other, sizeof buf);
    memcpy(other, buf, sizeof buf);
  }

  HKVariantType type() const noexcept { return type_; }
  
  void Dump() const noexcept;
 private:
  friend bool StackConv(lua_State *env, int index, Variant &);

  HKVariantType type_;
  union {
    Integer integer_;
    bool bool_;
    Number number_;
    char const *cstr_;
    lua_CFunction func_;
    Table table_;
  };
};

} // namespace hklua

namespace std {

inline void swap(hklua::Variant &lhs,
                 hklua::Variant &rhs) noexcept(noexcept(lhs.swap(rhs)))
{
  lhs.swap(rhs);
}

} // namespace std

#include "stack.h"

namespace hklua {

inline void StackPush(lua_State *env, Variant var)
{
  switch (var.type()) {
    case HK_INT:
      StackPush(env, var.ToInteger());
      break;
    case HK_NUMBER:
      StackPush(env, var.ToNumber());
      break;
    case HK_CSTRING:
      StackPush(env, var.ToCString());
      break;
    case HK_FUNCTION:
      StackPush(env, var.ToCFunction());
      break;
    case HK_TABLE:
      StackPush(env, var.ToTable());
      break;
    case HK_NIL:
      StackPush(env, var.ToNil());
      break;
  }
}

inline bool StackConv(lua_State *env, int index, Variant &var)
{
  switch (lua_type(env, index)) {
    case LUA_TNUMBER:
    {
      if (lua_isinteger(env, index)) {
        var.type_ = HK_INT;
        return StackConv(env, index, var.integer_);
      }
      var.type_ = HK_NUMBER;
      return StackConv(env, index, var.number_);
    }

    case LUA_TBOOLEAN:
    {
      var.type_ = HK_BOOLEAN;
      return StackConv(env, index, var.bool_);
    }
    
    case LUA_TSTRING:
    {
      var.type_ = HK_CSTRING;
      return StackConv(env, index, var.cstr_);
    }

    case LUA_TFUNCTION:
    {
      var.type_ = HK_FUNCTION;
      return StackConv(env, index, var.func_);
    }

    case LUA_TTABLE:
    {
      var.type_ = HK_TABLE;
      // FIXME
      return StackConv(env, index, var.table_);
    }

    case LUA_TNIL:
    {
      var.type_ = HK_NIL;
      return true;
    }
  }
  
  assert(false);
  return false;
}

#undef PADDING

class Table::Proxy {
  friend class Table;

  using KeyValue = std::pair<Variant, Variant>;
 public:
  explicit Proxy(Table *table, KeyValue kv)
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
  
  Variant &key() noexcept;
  Variant &value() noexcept;

 private:
  Table *table_;
  /* FIXME Reduce dynamic allocation */
  // std::unique_ptr<KeyValue> kv_;
  KeyValue kv_;
};

template <typename T, typename >
inline auto Table::Proxy::operator=(T const &v) -> Proxy&
{
  // assert(kv_);
  table_->SetField(kv_.first, v);
  table_->GetField(kv_.first, kv_.second);

  return *this;
}

template <typename T>
inline auto Table::operator[](T const &key) -> Proxy
{
  Variant tmp;
  GetField(key, tmp);
  return Proxy(this, Proxy::KeyValue(key, std::move(tmp)));
}

inline Variant &Table::Proxy::key() noexcept { return kv_.first; }
inline Variant &Table::Proxy::value() noexcept { return kv_.second; }

} // namespace hklua

#endif // HKLUA_VARIANT_H__
