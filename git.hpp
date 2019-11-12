#pragma once

#include "lift.hpp"
#include <git2.h>
#include <string>

namespace git {

#define C constexpr inline auto

C BLOB = GIT_FILEMODE_BLOB;
C TREE = GIT_FILEMODE_TREE;

C repository_open = lift{git_repository_open, git_repository_free};
C tree_lookup = lift{git_tree_lookup, git_tree_free};
C treebuilder_new = lift{git_treebuilder_new, git_treebuilder_free};

C treebuilder_write = lift{git_treebuilder_write};
C oid_fromstr = lift{git_oid_fromstr};
C blob_lookup = lift{git_blob_lookup, git_blob_free};
C commit_lookup = lift{git_commit_lookup, git_commit_free};
C commit_tree = lift{git_commit_tree, git_tree_free};

C repository_index = lift{git_repository_index, git_index_free};
C index_write_tree = lift{git_index_write_tree};
C reference_name_to_id = lift{git_reference_name_to_id};
C blob_create_frombuffer = lift{git_blob_create_frombuffer};

} // namespace git

namespace git {
template <typename Pith> struct tree_bark {
  Pith pith;
  template <typename O>
  constexpr void operator()(const O &o, git_repository *r,
                            const git_tree *source = nullptr) const {
    (git::treebuilder_new ^ r ^ source | [&](auto o, git_treebuilder *bld) {
      pith(_o_{[&](int err) { o(err); },
               [&](const char *filename, const git_oid *id,
                   git_filemode_t filemode) {
                 git_treebuilder_insert(nullptr, bld, filename, id, filemode);
               },
               [&](auto pith) {
                 pith(_o_{[&](int err) { o(err); },
                          [&](const char *filename, const git_oid *id,
                              git_filemode_t filemode) {
                            git_treebuilder_insert(nullptr, bld, filename, id,
                                                   filemode);
                          }});
               }});
      git::treebuilder_write(o, bld);
    })(o);
  }
};
template <typename Pith> tree_bark(Pith) -> tree_bark<Pith>;
} // namespace git

template <typename Pith, typename R>
constexpr auto operator^(git::tree_bark<Pith> l, R &&r) {
  return pin{l, static_cast<R &&>(r)};
}

