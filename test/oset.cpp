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
  return union_fn{
      [c, d, args = std::tuple{std::forward<Args>(args)...}](auto o) {
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

#include <cstdio>
int main() {
  git_libgit2_init();
  auto oid = run(
      git_oid_fromstr,
      static_cast<const char *>("84d43a3be66fb3262f178278c1d748c798785b2e"));

  constexpr static auto left = []() {
    return [](int, std::string s) { std::cout << s << '\n'; };
  };
  oid(overloaded{left(), [](auto x) { std::cout << pf(x) << '\n'; }});

  union_fn{[](auto o) {
    makeuptr(git_repository_open, git_repository_free,
             static_cast<const char *>("."))(overloaded{
        left(), [&](UPtr<git_repository> &&repo) {
          std::cout << &repo << '\n';

          git_oid id = git_oid{};
          //  print<decltype(git_tree_lookup)> p;
          auto r = makeuptr(git_tree_lookup, git_tree_free, repo.get(),
                            static_cast<const git_oid *>(&id));

          o("");
          o(1);
          o(1.f);
          //
          //
        }});
  }}(overloaded{
      [](float) {},
      [](int i, std::string &&m) { std::cout << i << ' ' << m << '\n'; },
      [](int) {}, [](const char *) {}});
}
