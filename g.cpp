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
  template <typename O> auto operator()(Args... args, O &&o) const -> int {
    T *pv = nullptr;
    int e = ctor(&pv, args...);
    if (e)
      return e;
    o(pv);
    dtor(pv);
    return e;
  }
};

template <typename T, typename... Args> struct lift<T, Args...> {
  int (*ctor)(T *, Args...);
  template <typename O> auto operator()(Args... args, O &&o) const -> int {
    T v;
    int e = ctor(&v, args...);
    if (e)
      return e;
    o(v);
    return e;
  }

};

template <typename T, typename... Args>
lift(int (*)(T **, Args...), void (*)(T *))->lift<T *, Args...>;
template <typename T, typename... Args>
lift(int (*)(T *, Args...))->lift<T, Args...>;

#define LFTP(T, O)                                                             \
  constexpr inline auto T##_##O = abo::git::lift {                             \
    git_##T##_##O, git_##T##_free                                              \
  }
#define LFT(T, O)                                                              \
  constexpr inline auto T##_##O = abo::git::lift { git_##T##_##O }

LFTP(repository, open);
LFTP(tree, lookup);
LFTP(treebuilder, new);

LFT(treebuilder, write);
LFT(oid, fromstr);

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

  template <typename U> explicit bark(U &&u) : pith(std::forward<U>(u)) {}

  auto operator()(int i) const { return i; }
  auto operator()(git_repository *repo) const {
    return pith(abo::o{[](const char *name, git_filemode_t mode, git_oid id) {
                  //
                }},
                repo);
  }
};
template <typename Pith> bark(Pith)->bark<Pith>;
} // namespace abo::git

#include <iostream>
using namespace abo;

int main() {
  git_libgit2_init();

  auto see =
      git::repository_open(".", git::bark([](auto o, git_repository *) { //
                             git_oid id;
                             if (int err = git_oid_fromstr(&id, ""))
                               return err;
                           }));

  (abo::o{[](int) {},
          [](auto id) { std::cout << git_oid_tostr_s(&id) << '\n'; }}) |
      [](auto o) {
        auto l = [&](int i) { o(i); };
        git::repository_open(
            ".",
            abo::o{
                l, [&](auto repo) {
                  git::oid_fromstr(
                      "f5880cf63a4372dcafb791620731637b4130d9df",
                      abo::o{
                          l, [&](auto oid) {
                            std::cout << "abo\n";
                            git::tree_lookup(
                                repo, &oid,
                                abo::o{
                                    l, [&](auto tree) {
                                      git::treebuilder_new(
                                          repo, nullptr,
                                          abo::o{
                                              l, [&](auto bld) {
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
