#include <functional>
#include <git2.h>
#include <iostream>
#include <memory>
#include <tuple>
#include <union.hpp>

using namespace abo;

template <typename T> using UPtr = std::unique_ptr<T, void (*)(T *)>;
template <typename... Args> std::string pf(Args &&...) {
  return {__PRETTY_FUNCTION__};
}
template <typename T, typename... Args>
constexpr auto makeuptr(int (*c)(T **, Args...), void (*d)(T *),
                        Args &&... args) {
  return union_fn{[c, d,
                   args = std::tuple<std::decay_t<Args>...>{
                       std::forward<Args>(args)...}](auto o) {
    T *ptr = nullptr;
    int error = std::apply(std::bind_front(c, &ptr), args);
    if (error)
      o(error, pf(c));
    else
      o(UPtr<T>(ptr, d));
  }};
}
// {int (&)(git_oid*, const char*)}
template <typename T, typename... Args>
constexpr auto run(int (*c)(T *, Args...), Args &&... args) {
  return union_fn{[c, args = std::tuple{std::forward<Args>(args)...}](auto o) {
    T v = {};
    int error = std::apply(std::bind_front(c, &v), args);
    if (error)
      o(error, pf(c));
    else
      o(std::move(v));
  }};
}

int main() {
  auto u1 = abo::union_fn{[](auto o) { o(1); }};
  auto u2 = abo::union_fn{[](auto o) { o("B"); }};
  auto u3 = u1 + u2 + u1;

  u3(overloaded{
      [](int i, const char *m, int) { std::cout << i << " " << m << '\n'; },
  });
}
