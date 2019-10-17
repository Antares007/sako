#include <functional>
#include <git2.h>
#include <git2/oid.h>
#include <git2/types.h>

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
template <typename T, typename... Args> struct lift<T *, Args...> {
  int (*ctor)(T **, Args...);
  void (*dtor)(T *);
  template <typename O> void operator()(Args... args, O &&o) const {
    T *pv = nullptr;
    if (int e = ctor(&pv, args...))
      o(e);
    else {
      o(pv);
      dtor(pv);
    }
  }
};

template <typename T, typename... Args> struct lift<T, Args...> {
  int (*ctor)(T *, Args...);
  template <typename O> void operator()(Args... args, O &&o) const {
    T v{};
    if (int e = ctor(&v, args...))
      o(e);
    else
      o(v);
  }
};

template <typename T, typename... Args>
lift(int (*)(T **, Args...), void (*)(T *))->lift<T *, Args...>;
template <typename T, typename... Args>
lift(int (*)(T *, Args...))->lift<T, Args...>;

inline auto repository_open =
    abo::git::lift{git_repository_open, git_repository_free};
inline auto tree_lookup = abo::git::lift{git_tree_lookup, git_tree_free};
inline auto treebuilder_new =
    abo::git::lift{git_treebuilder_new, git_treebuilder_free};

inline auto treebuilder_write = abo::git::lift{git_treebuilder_write};
inline auto oid_fromstr = abo::git::lift{git_oid_fromstr};

template <typename... T> struct ray;
template <> struct ray<> {
  template <typename U> void operator()(U &&) const {}
};
template <typename T> struct ray<T> { void operator()(T &&) const {}; };
template <typename T, typename... Rest>
struct ray<T, Rest...> : ray<T>, ray<Rest...> {
  using ray<T>::operator();
  using ray<Rest...>::operator();
};

template <typename Pith> struct bark {
  Pith pith;

  auto operator()(int i) const { return i; }
  auto operator()(git_repository *) const {}
};
template <typename Pith> bark(Pith)->bark<Pith>;
} // namespace abo::git

#include <iostream>
using namespace abo;


auto main() -> int {
  git_libgit2_init();

  auto repoPith = std::bind_front(git::repository_open, ".");
  repoPith(abo::o{[](int) {}, [](git_repository *) {}});

  (abo::o{[](int) {},
          [](auto id) { std::cout << git_oid_tostr_s(&id) << '\n'; }}) |
      [](auto o) {
        auto l = [&](int i) { o(i); };
        std::bind_front(git::repository_open, ".")(abo::o{
            l, [&](auto repo) {
              std::bind_front(
                  git::oid_fromstr,
                  "f5880cf63a4372dcafb791620731637b4130d9df")(abo::o{
                  l, [&](auto oid) {
                    std::bind_front(git::tree_lookup, repo, &oid)(abo::o{
                        l, [&](auto tree) {
                          std::bind_front(
                              git::treebuilder_new, repo,
                              nullptr)(abo::o{l, [&](auto bld) {
                                                auto count =
                                                    git_tree_entrycount(tree);
                                                for (size_t i = 1; i < count;
                                                     i++) {
                                                  auto e =
                                                      git_tree_entry_byindex(
                                                          tree, i);
                                                  git_treebuilder_insert(
                                                      nullptr, bld,
                                                      git_tree_entry_name(e),
                                                      git_tree_entry_id(e),
                                                      git_tree_entry_filemode(
                                                          e));
                                                }
                                                git::treebuilder_write(bld, o);
                                              }});
                        }});
                  }});
            }});
      };

  return 3;
}
