#ifndef LR_H
#define LR_H 1
#include "overloaded.h"
#include <variant>

namespace lr
{
template <typename... Rs>
using LR = std::variant<struct L, Rs...>;

struct L
{
    const char *message = "";
};

template <typename Os, typename... Ts>
constexpr decltype(auto) map(Os &&os, LR<Ts...> &&lr)
{
    return std::visit(
        overloaded{
            [&lr](L &) { return lr; },
            [&os](auto &&a) { return LR<Ts...>{os(a)}; }},
        lr);
};
} // namespace lr
#endif