#ifndef LR_H
#define LR_H 1
#include <variant>

namespace lr
{
template <class... Ts>
struct overloaded : Ts...
{
    using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...)->overloaded<Ts...>;

template <typename... Rs>
using LR = std::variant<struct L, Rs...>;

struct L
{
    const char *message = "";
};

//template <typename L, typename R, typename F>
//decltype(auto) map(F f, LR<L, R> &lr)
//{
//
//    return std::visit(overloaded{
//                          [](L l) { throw ""; },
//                          [&f](R r) { return f(r); },
//                          [](LRTag) { throw "never"; },
//                      },
//                      lr);
//}

} // namespace lr
#endif