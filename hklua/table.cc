#include "hklua/table.h"
#include "hklua/variant.h"
#include "hklua/util/type_traits.h"

using namespace hklua;

void Table::Proxy::SetKv(Variant k, Variant v) noexcept
{
  assert(!kv_);
  kv_.reset(new KeyValue(std::move(k), std::move(v)));
}

Variant &Table::Proxy::key() const noexcept { return kv_->first; }
Variant &Table::Proxy::value() const noexcept { return kv_->second; }

// auto Table::operator[](char const *key) -> Table::Proxy
// {
//   Proxy proxy(this);
//   lua_getfield(env_, index_, key);
//   Variant tmp;
//   if (StackConv(env_, -1, tmp)) {
//     proxy.SetKv(key, std::move(tmp));
//   }

//   return proxy;
// }

// auto Table::operator[](std::string const &key) -> Table::Proxy
// {
//   return operator[](key.c_str());
// }
