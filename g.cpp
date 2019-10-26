#include "_o_.hpp"
#include "m.hpp"
#include <functional>
#include <git2.h>

namespace git {
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
C revparse = [](const auto &o, git_repository *repo, const char *spec) {
  git::lift{git_revparse}(_o_{o,
                              [&](const git_revspec &spec) { //
                                if (spec.from && spec.to) {
                                  o(spec.from, spec.flags, spec.to);
                                  git_object_free(spec.from);
                                  git_object_free(spec.to);
                                } else if (spec.from) {
                                  o(spec.from, spec.flags);
                                  git_object_free(spec.from);
                                } else if (spec.to) {
                                  o(spec.flags, spec.to);
                                  git_object_free(spec.to);
                                }
                              }},
                          repo, spec);
};
C blob_lookup = git::lift{git_blob_lookup, git_blob_free};

C bray = [](const auto &o, git_treebuilder *bld, git_repository *repo) {
  return _o_{[=]<typename T>(const char *name, git_filemode_t mode,
                             T r) { ///
    if constexpr (std::is_invocable_r_v<void, T, _o_<ray<int>, ray<git_oid>>>)
      r(_o_{[&](int err) { o(err); },
            [&](git_oid id) {
              git_treebuilder_insert(nullptr, bld, name, &id, mode);
            }});
    else
      r(_o_{[&](int err) { o(err); },
            [&](git_oid id) {
              git_treebuilder_insert(nullptr, bld, name, &id, mode);
            }},
        repo);

  }};
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
              decltype(bray(std::declval<_o_<ray<int>, ray<git_oid>>>(),
                            nullptr, nullptr))>);

} // namespace git

#ifndef NVIM
#include <iostream>
auto main() -> int {
  git_libgit2_init();
  git::repository_open( ///
      _o_{[](int) {},
          [](git_repository *repo) {
            git::revparse( ///
                _o_{[](auto...) {},
                    [](git_object *obj, unsigned int) {
                      auto tree = reinterpret_cast<git_tree *>(obj);
                      for (size_t i = 0; i < git_tree_entrycount(tree); i++) {
                        auto e = git_tree_entry_byindex(tree, i);
                        std::cout << git_tree_entry_filemode(e) << " - "
                                  << git_tree_entry_name(e) << "\n";
                      }
                      std::cout << "ahaaa "
                                << (git_object_type(obj) == GIT_OBJECT_TREE)
                                << " ";
                    }},
                repo, "HEAD^{tree}:test");

            const auto pid = [](const auto &o) {
              git::oid_fromstr(o, "2096476c4b64612e8db373e838078ee213527476");
            };

            git::bark([&](auto o) {
              o("Aaaaaa", git::TREE, pid);
              o("Bbbb", git::TREE, pid);
            })(_o_{[](int) {},
                   [](const git_oid &id) {
                     std::cout << git_oid_tostr_s(&id) << '\n';
                   }},
               repo);
          }},
      ".");
  return 3;
}
#endif
