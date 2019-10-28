#include "_o_.hpp"
#include "m.hpp"
#include <functional>
#include <git2.h>

namespace git {
struct pith_tag {};
template <typename... Args> struct lift;
template <typename T, typename... Args> struct lift<T *, Args...> {
  int (*ctor)(T **, Args...);
  void (*dtor)(T *);
  template <typename O> void operator()(const O &o, Args... args) const {
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
  template <typename O> void operator()(const O &o, Args... args) const {
    T v{};
    if (int e = ctor(&v, args...))
      o(e);
    else
      o(&v);
  }
};
template <typename T, typename... Args>
lift(int (*)(T **, Args...), void (*)(T *))->lift<T *, Args...>;
template <typename T, typename... Args>
lift(int (*)(T *, Args...))->lift<T, Args...>;

template <typename> struct is_lift : std::false_type {};
template <typename T, typename... A>
struct is_lift<lift<T, A...>> : std::true_type {};

#define C constexpr inline auto

C BLOB = GIT_FILEMODE_BLOB;
C TREE = GIT_FILEMODE_TREE;

C repository_open = git::lift{git_repository_open, git_repository_free};
C tree_lookup = git::lift{git_tree_lookup, git_tree_free};
C treebuilder_new = git::lift{git_treebuilder_new, git_treebuilder_free};

C treebuilder_write = git::lift{git_treebuilder_write};
C oid_fromstr = git::lift{git_oid_fromstr};
C blob_lookup = git::lift{git_blob_lookup, git_blob_free};


C bray = [](const auto &o, git_treebuilder *bld, git_repository *repo) {
  return _o_{[=]<typename T>(const char *name, git_filemode_t mode,
                             T r){
      ///
      if constexpr (std::is_invocable_r_v<void, T,
                                          _o_<ray<int>, ray<git_oid *>>>)
          r(_o_{[&](int err) { o(err); },
                [&](git_oid *id) {
                  git_treebuilder_insert(nullptr, bld, name, id, mode);
                }});
  else r(_o_{[&](int err) { o(err); },
             [&](git_oid *id) {
               git_treebuilder_insert(nullptr, bld, name, id, mode);
             }},
         repo);
}
}; // namespace git
};

UFB(bark);
MO()(git_repository *repo) {
  treebuilder_new(_o_{o,
                      [&](git_treebuilder *bld) {
                        a(bray(o, bld, repo));
                        treebuilder_write(o, bld);
                      }},
                  repo, nullptr);
}
UFE(bark, std::is_invocable_r<
              void, A,
              decltype(bray(std::declval<_o_<ray<int>, ray<git_oid *>>>(),
                            nullptr, nullptr))>);
} // namespace git

template <typename Pith, typename A> struct pin {
  Pith pith;
  A a;
  template <typename O, typename... Rest>
  constexpr void operator()(const O &o, Rest &&... rest) const {
    auto r = _o_{[&](int err) { o(err); },
                 [&, ... rest = std::forward<Rest>(rest)](auto &&a) {
                   pith(o, std::forward<decltype(a)>(a), rest...);
                 }};
    if constexpr (std::is_invocable_r_v<void, A, decltype(r)>)
      a(r);
    else
      pith(o, a, rest...);
  }
};
template <typename Pith, typename A> pin(Pith, A)->pin<Pith, A>;

template <typename> struct is_pin : std::false_type {};
template <typename T, typename A> struct is_pin<pin<T, A>> : std::true_type {};

template <typename L, typename R,
          typename = std::enable_if_t<std::disjunction_v<
              git::is_lift<std::decay_t<L>>, is_pin<std::decay_t<L>>>>
          /**/>
constexpr auto operator^(L &&l, R &&r) {
  return pin{std::forward<L>(l), std::forward<R>(r)};
}

template <typename... T> struct print;

#ifndef NVIM

#include <iostream>

auto main() -> int {
  git_libgit2_init();
  auto l = git::blob_lookup;
  auto aaa = l                       //
             ^ (git::repository_open //
                ^ ".")               //
             ^ (git::oid_fromstr     //
                ^ "080ca003cef9e73967ff818672c3b15e26fe0817");
  aaa(_o_{[](int err) { std::cout << "bbb" << err << "\n"; },
          [](git_blob *blob) {
            auto buff = git_blob_rawcontent(blob);
            auto size = git_blob_rawsize(blob);
            std::cout << buff << " - " << size << "aaa\n";
          }});

  git::repository_open( ///
      _o_{[](int) {},
          [](git_repository *repo) {
            const auto pid = [](const auto &o) {
              git::oid_fromstr(o, "2096476c4b64612e8db373e838078ee213527476");
            };

            git::bark([&](auto o) {
              o("Aaaaaa", git::TREE, pid);
              o("Bbbb", git::TREE, pid);
            })(_o_{[](int) {},
                   [](const git_oid *id) {
                     std::cout << git_oid_tostr_s(id) << '\n';
                   }},
               repo);
          }},
      ".");
  return 3;
}
#endif
