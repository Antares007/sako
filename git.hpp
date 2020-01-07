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
C empty_tree_oid = (oid_fromstr, "4b825dc642cb6eb9a060e54bf8d69288fbee4904");

} // namespace git

#include <functional>
namespace git {

C ls = [](auto o, const git_tree *tree) {
  auto r = [=](auto r, size_t i) {
    if (i-- < 1)
      return;
    const auto e = git_tree_entry_byindex(tree, i);
    o(git_tree_entry_name(e), git_tree_entry_id(e), git_tree_entry_filemode(e));
    r(r, i);
  };
  r(r, git_tree_entrycount(tree));
};

C diff = [](auto o, git_tree *lhs, git_tree *rhs) { //
  const size_t rc = git_tree_entrycount(rhs);
  const size_t lc = git_tree_entrycount(lhs);
  size_t li = 0;
  size_t ri = 0;
  while (li < lc && ri < rc) {
    const auto le = git_tree_entry_byindex(lhs, li);
    const auto re = git_tree_entry_byindex(rhs, ri);
    const auto rez = git_tree_entry_cmp(le, re);
    if (rez == 0) {
      if (git_tree_entry_filemode(le) != git_tree_entry_filemode(re) ||
          git_oid_cmp(git_tree_entry_id(le), git_tree_entry_id(re))) {
        o(le, re);
      }
      li++;
      ri++;
    } else if (rez < 0) {
      o(le, rez);
      li++;
    } else {
      o(rez, re);
      ri++;
    }
  }
  while (li < lc)
    o(git_tree_entry_byindex(lhs, li++), -1);
  while (ri < rc)
    o(1, git_tree_entry_byindex(rhs, ri++));
};

C tree_ring = [](auto o, git_repository *r, auto pith) {
  ([&](auto o, git_treebuilder *bld) {
    pith(rays{[&o](error_ray *, int err) { o(error_ray_v, err); },
              [&bld](const char *filename, const git_oid *id,
                     git_filemode_t filemode) {
                git_treebuilder_insert(nullptr, bld, filename, id, filemode);
              }});
    git::treebuilder_write(o, bld);
  } ^
   (treebuilder_new, r, nullptr))(o);
};
} // namespace git
