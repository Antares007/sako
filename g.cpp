#include <functional>
#include <git2.h>

namespace abo {
template <class... O> struct o : O... { using O::operator()...; };
template <class... O> o(O...)->o<O...>;

template <typename... Ts> struct print;

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

inline auto repository_open =
    abo::git::lift{git_repository_open, git_repository_free};
inline auto tree_lookup = abo::git::lift{git_tree_lookup, git_tree_free};
inline auto treebuilder_new =
    abo::git::lift{git_treebuilder_new, git_treebuilder_free};

inline auto treebuilder_write = abo::git::lift{git_treebuilder_write};
inline auto oid_fromstr = abo::git::lift{git_oid_fromstr};
inline auto revparse = abo::git::lift{git_revparse};

template <typename Pith> struct bark {
  Pith pith;

  template <typename O> ///
  void operator()(git_repository *repo, O &&o) const {
    treebuilder_new(
        repo, nullptr,
        abo::o{o, [&](git_treebuilder *bld) { ///
                 pith([&]<typename T>(const char *name, git_filemode_t mode,
                                      T r) { ///

                   if constexpr (std::is_invocable_r_v<void, T,
                                                       abo::ray<int, git_oid>>)
                     r(abo::o{[&](int err) { o(err); },
                              [&](git_oid id) {
                                git_treebuilder_insert(nullptr, bld, name, &id,
                                                       mode);
                              }});
                   else
                     r(repo, abo::o{[&](int err) { o(err); },
                                    [&](git_oid id) {
                                      git_treebuilder_insert(nullptr, bld, name,
                                                             &id, mode);
                                    }});

                 });
                 treebuilder_write(bld, o);
               }});
  }
};
template <typename Pith> bark(Pith)->bark<Pith>; ///

inline auto BLOB = GIT_FILEMODE_BLOB;
inline auto TREE = GIT_FILEMODE_TREE;

auto main() -> int {
  git_libgit2_init();

  auto pith = [](auto o) {
    o("...start");
    o(1);
    o("end...");
    repository_open(".", O(o, [&](git_repository *repo) {
                      const auto pid = std::bind_front(
                          oid_fromstr,
                          "2096476c4b64612e8db373e838078ee213527476");

                      auto amocana1 = bark{[&, op = o](auto o) { ///

                        op("hello");
                        // o("readme.txt", BLOB, "hello world!\n");


                        o("Aaaaaa", TREE, pid); ///
                        o("Bbbb", TREE, pid);

                        op("world");

                      }};

                      amocana1(repo, o);

                      bark{[&](auto o) { ///
                        o("A", TREE, pid);
                        o("B", TREE, bark{[&](auto o) { ///
                            o("A", TREE, pid);
                            o("B", TREE, pid);
                          }});
                      }}(repo, o);

                    }));
  };

  pith(abo::o{[](int err) { ///
                std::cout << err << '\n';
              },
              [](git_oid id) { ///
                std::cout << git_oid_tostr_s(&id) << '\n';
              },
              [](auto r) { ///
                std::cout << " !!!! " << r << '\n';
              }});
  return 3;
  }
