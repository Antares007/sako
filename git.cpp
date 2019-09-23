#include "git.h"
#include <cstdio>
#include <git2.h>
#include <iostream>
#include <string>
#include <tuple>
#include <utility>

namespace git {

TreeId TreeBark::operator()(void (*)(const O &, const TreeBark &)) const
    noexcept {
  return TreeId{git_oid{}};
}

lr::LR<size_t> lookup(const UPtr<git_repository> &repo, const TreeId &tid,
                      const TreeBark::O &o) {
  return lr::map(
      [&o](const UPtr<git_tree> &pTree) {
        const auto tree = pTree.get();
        auto count = git_tree_entrycount(tree);
        for (size_t i = 0; i < count; i++) {
          auto entry = git_tree_entry_byindex(tree, i);
          auto oid = git_tree_entry_id(entry);
          auto mode = git_tree_entry_filemode(entry);
          auto name = git_tree_entry_name(entry);

          if (mode == GIT_FILEMODE_TREE)
            o(name, TreeId(*oid));
          else if (mode == GIT_FILEMODE_COMMIT)
            o(name, CommitId(*oid));
          else if (mode == GIT_FILEMODE_BLOB_EXECUTABLE)
            o(name, ExecId(*oid));
          else if (mode == GIT_FILEMODE_LINK)
            o(name, LinkId(*oid));
          else
            o(name, BlobId(*oid));
        }
        return count;
      },
      git::make(git_tree_lookup, git_tree_free, repo.get(), &tid.get()));
}

std::vector<Entry> getEntries(const UPtr<git_tree> &tree) {
  const git_tree *ptree = tree.get();
  std::vector<Entry> entries;
  for (size_t i = 0; i < git_tree_entrycount(ptree); i++) {
    auto entry = git_tree_entry_byindex(ptree, i);
    auto oid = *git_tree_entry_id(entry);
    auto mode = git_tree_entry_filemode(entry);
    auto name = git_tree_entry_name(entry);

    if (mode == GIT_FILEMODE_TREE)
      entries.emplace_back(name, TreeId(oid));
    else if (mode == GIT_FILEMODE_COMMIT)
      entries.emplace_back(name, CommitId(oid));
    else if (mode == GIT_FILEMODE_BLOB_EXECUTABLE)
      entries.emplace_back(name, ExecId(oid));
    else if (mode == GIT_FILEMODE_LINK)
      entries.emplace_back(name, LinkId(oid));
    else
      entries.emplace_back(name, BlobId(oid));
  }
  return entries;
}

// void test() {
//  auto repo = open(".");
//  lr::map(
//      [](UPtr<git_repository> &repo) {
//        auto oid = git_oid{};
//        git_tree *tree = NULL;
//        int error = git_tree_lookup(&tree, &*repo, &oid);
//      },
//      std::move(repo));
//}

} // namespace git

// struct S {
//   S() noexcept { puts("S()"); }
//   S(S const &) noexcept { puts("S(const S &)"); }
//   S(S &&) noexcept { puts("S(S &&)"); }
//   S &operator=(const S &) noexcept {
//     puts("S &operator = (const S &)");
//     return *this;
//   }
//   S &operator=(S &&) noexcept {
//     puts("S &operator = (S &&)");
//     return *this;
//   }
//   ~S() noexcept { puts("~S()"); }
// };

// Bark::Bark(UPtr<git_repository> &&repo)
//     : repo(std::forward<UPtr<git_repository>>(repo)) {}

// lr::LR<TreeId> Bark::operator()(TreeId tid, Pith pith) const {
//   auto tree = lr::map(
//       [this, &pith](const UPtr<git_tree> &x) {
//         pith(Bark::Ray(getEntries(x), *this));
//         return 1;
//       },
//       lookup(this->repo, tid));
//   return TreeId(git_oid{});
// }

// lr::LR<TreeId> Bark::operator()(Pith pith) const {
//   auto ray = Bark::Ray(std::vector<Entry>(), *this);
//   pith(std::move(ray));
//   return TreeId(git_oid{});
// }
