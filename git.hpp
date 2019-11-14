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
C empty_tree_oid = oid_fromstr ^ "4b825dc642cb6eb9a060e54bf8d69288fbee4904";

} // namespace git

namespace git {
constexpr inline auto ls = [](auto o, const git_tree *tree) {
  const auto rec = [=](auto rec, size_t i) {
    if (i-- < 1)
      return;
    const auto e = git_tree_entry_byindex(tree, i);
    o(git_tree_entry_name(e), git_tree_entry_id(e), git_tree_entry_filemode(e));
    rec(rec, i);
  };
  rec(rec, git_tree_entrycount(tree));
};

template <typename Pith> struct tree_bark {
  Pith pith;
  template <typename O> struct R {
    O o;
    git_treebuilder *bld;
    constexpr void operator()(int err) const { o(err); }
    constexpr void operator()(const char *filename, const git_oid *id,
                              git_filemode_t filemode) const {
      git_treebuilder_insert(nullptr, bld, filename, id, filemode);
    }
    template <typename Pith_> constexpr void operator()(Pith_ pith_) const {
      pith_(*this);
    }
  };
  template <typename O>
  constexpr void operator()(O o, git_repository *r,
                            const git_tree *source) const {
    (git::treebuilder_new ^ r ^ source | [&](auto o, git_treebuilder *bld) {
      pith(R<O>{o, bld}, r, source);
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
