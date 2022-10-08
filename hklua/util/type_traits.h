#ifndef HKLUA_UTIL_TYPE_TRAITS_H__
#define HKLUA_UTIL_TYPE_TRAITS_H__

#include <type_traits>

namespace hklua {

template <typename T, typename U>
using AmbigiousCond = std::is_same<T, typename std::decay<U>::type>;

}

#endif // HKLUA_UTIL_TYPE_TRAITS_H__
