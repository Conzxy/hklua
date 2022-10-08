#ifndef HKLUA_VARIABLE_INDICATOR_H__
#define HKLUA_VARIABLE_INDICATOR_H__

namespace hklua {

template <typename T>
struct VariableIndicator {
  T data;
  bool is_nil = false;
};

template <typename T>
using VI = VariableIndicator<T>;

} // namespace hklua

#endif // HKLUA_VARIABLE_INDICATOR_H__
