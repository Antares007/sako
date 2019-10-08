#include <functional>
#include <git2.h>

namespace abo {
template <class... O> struct o : O... { using O::operator()...; };
template <class... O> o(O...)->o<O...>;

template <typename... Ts> struct print;

template <typename A, typename F>
constexpr auto operator|(A &&a, F &&f)
    -> decltype(std::invoke(std::forward<F>(f), std::forward<A>(a))) {
  return std::invoke(std::forward<F>(f), std::forward<A>(a));
}
} // namespace abo

namespace abo::git {

template <typename... Args> struct lift;
template <typename L, typename T, typename... Args>
struct lift<L, T *, Args...> {
  L (*ctor)(T **, Args...);
  void (*dtor)(T *);
  template <
      typename O,
      typename = std::enable_if_t<std::conjunction_v<
          std::is_invocable_r<void, O, L>, std::is_invocable_r<void, O, T *>>>>
  constexpr void operator()(Args... args, O &&o) const {
    T *ptr = nullptr;
    if (L l = ctor(&ptr, std::move(args)...))
      std::invoke(std::forward<O>(o), std::move(l));
    else {
      std::invoke(std::forward<O>(o), ptr);
      dtor(ptr);
    }
  }
};
template <typename L, typename T, typename... Args> struct lift<L, T, Args...> {
  L (*f)(T *, Args...);
  template <
      typename O,
      typename = std::enable_if_t<std::conjunction_v<
          std::is_invocable_r<void, O, L>, std::is_invocable_r<void, O, T>>>>
  constexpr void operator()(Args... args, O &&o) const {
    T t;
    if (L error = f(&t, std::move(args)...))
      std::invoke(std::forward<O>(o), error);
    else {
      std::invoke(std::forward<O>(o), std::move(t));
    }
  }
};
template <typename L, typename T, typename... Args>
lift(L (*)(T **, Args...), void (*)(T *))->lift<L, T *, Args...>;
template <typename L, typename T, typename... Args>
lift(L (*)(T *, Args...))->lift<L, T, Args...>;

constexpr inline auto repository_open =
    abo::git::lift{git_repository_open, git_repository_free};
constexpr inline auto tree_lookup =
    abo::git::lift{git_tree_lookup, git_tree_free};
constexpr inline auto oid_fromstr = abo::git::lift{git_oid_fromstr};

template <typename Pith> struct bark {
  Pith pith;
  template <typename U,
            typename = std::enable_if_t<std::is_invocable_r_v<void, U, int>>>
  bark(U &&) {}

  template <typename O,
            typename = std::enable_if_t<std::is_invocable_r_v<void, O, int>>>
  constexpr void operator()(git_repository *, O &&) const {}
};
template <typename Pith> bark(Pith)->bark<Pith>;
} // namespace abo::git

#include <iostream>
using namespace abo;
int main() {
  git_libgit2_init();

  o{[](int) {},
    [](git_repository *repo) {
      abo::o{[](int) {},
             [&](git_oid oid) {
               abo::o{[](int) {},
                      [&](git_tree *tree) {
                        std::cout << repo << '\n';
                        std::cout << tree << '\n';
                        std::cout << oid.id << '\n';
                      }} |
                   std::bind_front(git::tree_lookup, repo, &oid);
             }} |
          std::bind_front(git::oid_fromstr,
                          "f5880cf63a4372dcafb791620731637b4130d9df");
    }} |
      std::bind_front(git::repository_open, ".");

  return 3;
}
