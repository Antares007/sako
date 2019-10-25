#include "_o_.hpp"
#include <functional>
#include <git2.h>

namespace git {
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

#define C constexpr inline auto

C BLOB = GIT_FILEMODE_BLOB;
C TREE = GIT_FILEMODE_TREE;

C repository_open = git::lift{git_repository_open, git_repository_free};
C tree_lookup = git::lift{git_tree_lookup, git_tree_free};
C treebuilder_new = git::lift{git_treebuilder_new, git_treebuilder_free};

C treebuilder_write = git::lift{git_treebuilder_write};
C oid_fromstr = git::lift{git_oid_fromstr};
C revparse = git::lift{git_revparse};

template <typename Pith> struct bark {
  Pith pith;

  template <typename O> ///
  void operator()(git_repository *repo, O &&o) const {
    treebuilder_new(
        repo, nullptr,
        _o_{o, [&](git_treebuilder *bld) { ///
              pith([&]<typename T>(const char *name, git_filemode_t mode,
                                   T r) { ///
                if constexpr (std::is_invocable_r_v<void, T, ray<int, git_oid>>)
                  r(_o_{[&](int err) { o(err); },
                        [&](git_oid id) {
                          git_treebuilder_insert(nullptr, bld, name, &id, mode);
                        }});
                else
                  r(repo, _o_{[&](int err) { o(err); },
                              [&](git_oid id) {
                                git_treebuilder_insert(nullptr, bld, name, &id,
                                                       mode);
                              }});

              });
              treebuilder_write(bld, o);
            }});
  }
};
template <typename Pith> bark(Pith)->bark<Pith>; ///
} // namespace git

#ifndef NVIM
#include <iostream>
auto main() -> int {
  git_libgit2_init();
  auto pith = [](auto o) {
    o("...start");
    o(1);
    o("end...");
    git::repository_open(
        ".", _o_{o, [&](git_repository *repo) {
                   const auto pid = std::bind_front(
                       git::oid_fromstr,
                       "2096476c4b64612e8db373e838078ee213527476");

                   auto amocana1 = git::bark{[&, op = o](auto o) { ///
                     op("hello");
                     // o("readme.txt", BLOB, "hello world!\n");

                     o("Aaaaaa", git::TREE, pid); ///
                     o("Bbbb", git::TREE, pid);

                     op("world");

                   }};

                   amocana1(repo, o);

                   git::bark{[&](auto o) { ///
                     o("A", git::TREE, pid);
                     o("B", git::TREE, git::bark{[&](auto o) { ///
                         o("A", git::TREE, pid);
                         o("B", git::TREE, pid);
                       }});
                   }}(repo, o);
                 }});
  };

  pith(_o_{[](int err) { ///
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
#endif
