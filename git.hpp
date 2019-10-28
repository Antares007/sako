#pragma once
#include "_o_.hpp"
#include "lift.hpp"
#include <functional>
#include <git2.h>

namespace git {

#define C constexpr inline auto

C BLOB = GIT_FILEMODE_BLOB;
C TREE = GIT_FILEMODE_TREE;

C repository_open = lift::lift{git_repository_open, git_repository_free};
C tree_lookup = lift::lift{git_tree_lookup, git_tree_free};
C treebuilder_new = lift::lift{git_treebuilder_new, git_treebuilder_free};

C treebuilder_write = lift::lift{git_treebuilder_write};
C oid_fromstr = lift::lift{git_oid_fromstr};
C blob_lookup = lift::lift{git_blob_lookup, git_blob_free};

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
#include "m.hpp"
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
