#include "git.h"
#include <cstdio>
#include <git2.h>
#include <iostream>
#include <numeric>
#include <string>
#include <tuple>
#include <utility>

namespace git {

lr::LR<git_oid> commit(const char *message, lr::LR<git_oid> &&tree,
                       std::vector<lr::LR<git_oid>> &&p) { //
  auto f = lr::fmap{[](git_oid &&) { return 1; }};
  auto rez = f(std::move(tree));
  return git_oid{};
}

// TreeId TreeBark::operator()(void (*)(const O &, const TreeBark &)) const
//    noexcept {
//  return TreeId{""};
//}
// void lookup(const TreeBark::O &o, const UPtr<git_tree> &pTree) {
//  const auto tree = pTree.get();
//  auto count = git_tree_entrycount(tree);
//  for (size_t i = 0; i < count; i++) {
//    o(git_tree_entry_byindex(tree, i));
//  }
//}
// std::vector<Entry> getEntries(const UPtr<git_tree> &tree) {
//  const git_tree *ptree = tree.get();
//  std::vector<Entry> entries;
//  for (size_t i = 0; i < git_tree_entrycount(ptree); i++) {
//    auto entry = git_tree_entry_byindex(ptree, i);
//    auto oid = git_oid_tostr_s(git_tree_entry_id(entry));
//    auto mode = git_tree_entry_filemode(entry);
//    auto name = git_tree_entry_name(entry);
//
//    if (mode == GIT_FILEMODE_TREE)
//      entries.emplace_back(name, TreeId(oid));
//    else if (mode == GIT_FILEMODE_COMMIT)
//      entries.emplace_back(name, CommitId(oid));
//    else if (mode == GIT_FILEMODE_BLOB_EXECUTABLE)
//      entries.emplace_back(name, ExecId(oid));
//    else if (mode == GIT_FILEMODE_LINK)
//      entries.emplace_back(name, LinkId(oid));
//    else
//      entries.emplace_back(name, BlobId(oid));
//  }
//  return entries;
//}

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
