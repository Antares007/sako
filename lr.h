#ifndef LR_H
#define LR_H 1
#include "overloaded.h"
#include <type_traits>
#include <variant>

namespace lr {
template <typename... Rs> using LR = std::variant<struct L, Rs...>;

struct L {
  const char *message = "";
};

template <typename OSet, typename... Rs>
constexpr decltype(auto) map(OSet &&os, LR<Rs...> &&lr) {
  using R = decltype(os(std::get<1>(lr)));
  return std::visit(overloaded{[](L &l) { return LR<R>(l); },
                               [&os](auto &&a) { return LR<R>(os(a)); }},
                    lr);
}

// template <typename OSet, typename... Ts>
// constexpr decltype(auto) map(OSet &&os, LR<Ts...> &lr) {
//  return map(std::forward<OSet>(os), std::move(lr));
//}
} // namespace lr
#endif