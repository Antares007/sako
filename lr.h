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

template <typename OSet, typename... Ts>
constexpr decltype(auto) map(OSet &&os, LR<Ts...> &&lr) {
  using RT = decltype(os(std::get<1>(lr)));
  return std::visit(overloaded{[](L &l) { return LR<RT>(l); },
                               [&os](auto &&a) { return LR<RT>(os(a)); }},
                    lr);
};

// template <typename OSet, typename... Ts>
// constexpr decltype(auto) map(OSet &&os, LR<Ts...> &lr) {
//  return map(std::forward<OSet>(os), std::move(lr));
//}
} // namespace lr
#endif