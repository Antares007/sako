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

} // namespace abo::git

#include <iostream>

inline auto O = []<typename... Rays>(Rays... rays) {
  return abo::o<Rays...>{rays...};
};

auto repository_open = abo::git::lift{git_repository_open, git_repository_free};
auto tree_lookup = abo::git::lift{git_tree_lookup, git_tree_free};
auto treebuilder_new =
    abo::git::lift{git_treebuilder_new, git_treebuilder_free};

auto treebuilder_write = abo::git::lift{git_treebuilder_write};
auto oid_fromstr = abo::git::lift{git_oid_fromstr};

template <typename Pith> struct bark {
  Pith pith;

  template <typename O> ///
  auto operator()(git_repository *repo, O &&o) const {
    treebuilder_new(
        repo, nullptr,
        abo::o{o, [&](git_treebuilder *bld) {                              ///
                 pith([&](const char *name, git_filemode_t mode, auto r) { ///
                   r(abo::o{[&](int err) { o(err); },
                            [&](git_oid id) {
                              git_treebuilder_insert(nullptr, bld, name, &id,
                                                     mode);
                            }});

                 });
                 treebuilder_write(bld, o);
               }});
  }
};
template <typename Pith> bark(Pith)->bark<Pith>;

auto main() -> int {
  git_libgit2_init();

  [](auto o) {
    o("...start");
    repository_open(".", O(o, [&](git_repository *repo) {
                      bark{[op = o](auto o) { ///
                        op("hello");
                        o("A", GIT_FILEMODE_TREE,
                          std::bind_front(
                              oid_fromstr,
                              "c66a2f2774b216952d0d3a070be0e5fbe58edaad"));
                        o("B", GIT_FILEMODE_TREE,
                          std::bind_front(
                              oid_fromstr,
                              "c66a2f2774b216952d0d3a070be0e5fbe58edaad"));
                        op("world");
                      }}(repo, o);
                    }));
    o("end...");
  }(abo::o{[](int err) { ///
             std::cout << err << '\n';
           },
           [](git_oid id) { ///
             std::cout << git_oid_tostr_s(&id) << '\n';
           },
           [](auto r) { ///
             std::cout << r << '\n';
           }});
  /*
  ...start
  hello
  -1
  world
  end...

  [Process exited 3]
  */

  return 3;
  }
