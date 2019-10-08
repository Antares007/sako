#include <git2.h>
#include <memory>

namespace abo::git {

template <typename T, typename... Args> struct lift;
template <typename T, typename... Args> struct lift<T **, Args...> {
  int (*ctor)(T **, Args...);
  void (*dtor)(T *);
  template <typename O>
  constexpr void operator()(Args... args, const O &o) const {
    T *ptr = nullptr;
    if (int error = ctor(&ptr, args...))
      o(error);
    else {
      o(ptr);
      dtor(ptr);
    }
  }
};
template <typename T, typename... Args> struct lift<T *, Args...> {
  int (*ctor)(T *, Args...);
  template <typename O>
  constexpr void operator()(Args... args, const O &o) const {
    T t = nullptr;
    if (int error = ctor(&t, args...))
      o(error);
    else {
      o(std::move(t));
    }
  }
};
template <typename T, typename... Args>
lift(int (*)(T **, Args...), void (*)(T *))->lift<T **, Args...>;
template <typename T, typename... Args>
lift(int (*)(T *, Args...))->lift<T *, Args...>;

template <typename Pith> struct bark {
  Pith pith;
  template <typename U,
            typename = std::enable_if_t<std::is_invocable_r_v<void, U, int>>>
  bark(U &&) {}

  template <typename O,
            typename = std::enable_if_t<std::is_invocable_r_v<void, O, int>>>
  constexpr void operator()(git_repository *repo, O &&) const {}
};
template <typename Pith> bark(Pith)->bark<Pith>;
} // namespace abo::git

int main() {
  //
  auto a = abo::git::lift{git_repository_open, git_repository_free};
  auto b = abo::git::bark{[](int) {}};
  return 3;
}
