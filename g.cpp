#include <git2.h>
#include <memory>

namespace abo {

namespace git {
template <typename T> using UPtr = std::unique_ptr<T, void (*)(T *)>;

namespace detail {
template <typename T> constexpr const char *p(T &&) {
  return __PRETTY_FUNCTION__;
}

template <typename T, typename... Args>
constexpr auto m1(int (*c)(T **, Args...), void (*d)(T *)) {
  return [c, d](Args... args, auto o) {
    T *ptr = nullptr;
    if (int error = c(&ptr, args...))
      o(p(c));
    else
      o(UPtr<T>(ptr, d));
  };
}

template <typename T, typename... Args>
constexpr auto m2(int (*c)(T *, Args...)) {
  return [c](Args... args, auto o) {
    T v = {};
    if (int error = c(&v, args...))
      o(p(c));
    else
      o(std::move(v));
  };
}
} // namespace detail

constexpr inline auto repository_open =
    detail::m1(git_repository_open, git_repository_free);
constexpr inline auto tree_lookup = detail::m1(git_tree_lookup, git_tree_free);
constexpr inline auto oid_fromstr = detail::m2(git_oid_fromstr);
} // namespace git

} // namespace abo

int main() {
  //
  int iaa = 1;
  return 3;
}
