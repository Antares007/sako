#include "union.hpp"
#include <git2.h>
#include <iostream>
#include <memory>

using namespace abo;

template <typename T> using UPtr = std::unique_ptr<T, void (*)(T *)>;

template <typename T, typename... Args>
constexpr auto makeuptr(int (*c)(T **, Args...), void (*d)(T *),
                        Args &&... args) {
  return union_fn{[c, d, &args...](auto o) {
    T *ptr = nullptr;
    int error = c(&ptr, std::forward<Args>(args)...);
    if (error)
      o(error, __PRETTY_FUNCTION__);
    o(UPtr<T>(ptr, d));
  }};
}

#include <cstdio>
int main() {
  git_libgit2_init();
  union_fn{[](auto o) {
    makeuptr(git_repository_open, git_repository_free,
             static_cast<const char *>("."))(
        [&](int err, const char *p) { o(err, p); },
        [&](UPtr<git_repository> &&repo) {
          o("");
          o(1);
          o(1.f);
          //
          //
        });
  }}([](float) {}, [](int, const char *) {}, [](int) {}, [](const char *) {});
}
